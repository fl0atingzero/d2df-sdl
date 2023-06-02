#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include <enet/enet.h>
#include <enet/types.h>

#define MS_VERSION "0.3"
#define MS_MAX_SERVERS 128
#define MS_MAX_CLIENTS (MS_MAX_SERVERS + 1)
#define MS_URGENT_FILE "urgent.txt"
#define MS_MOTD_FILE "motd.txt"
#define MS_BAN_FILE "master_bans.txt"

#define DEFAULT_SPAM_CAP 10
#define DEFAULT_MAX_SERVERS MS_MAX_SERVERS
#define DEFAULT_MAX_PER_HOST 4
#define DEFAULT_TIMEOUT 100
#define DEFAULT_PORT 25665

#define NET_BUFSIZE 65536
#define NET_FULLMASK 0xFFFFFFFF

#define SV_PROTO_MIN 140
#define SV_PROTO_MAX 210
#define SV_NAME_MAX 64
#define SV_MAP_MAX 64
#define SV_MAX_PLAYERS 24
#define SV_MAX_GAMEMODE 5
#define SV_NEW_SERVER_INTERVAL 3

#define MAX_STRLEN 0xFF

enum log_severity_e {
  LOG_NOTE,
  LOG_WARN,
  LOG_ERROR
};

enum net_ch_e {
  NET_CH_MAIN,
  NET_CH_UPD,
  NET_CH_COUNT
};

enum net_msg_e {
  NET_MSG_ADD = 200,
  NET_MSG_RM = 201,
  NET_MSG_LIST = 202
};

enum sv_flags_e {
  SV_FL_PASSWORD = 1 << 0,
  SV_FL_VERIFIED = 1 << 1,
  SV_FL_MAX = SV_FL_PASSWORD | SV_FL_VERIFIED,
};

typedef struct enet_buf_s {
  enet_uint8 *data;
  size_t size;
  size_t pos;
  int overflow;
} enet_buf_t;

typedef struct ban_record_s {
  enet_uint32 host;
  enet_uint32 mask;
  int ban_count;
  time_t cur_ban;
  struct ban_record_s *next;
  struct ban_record_s *prev;
} ban_record_t;

typedef struct server_s {
  enet_uint32 host; // BE; 0 means this slot is unused
  enet_uint16 port; // LE, which is what the game and enet both expect
  enet_uint8  flags;
  enet_uint8  proto;
  enet_uint8  gamemode;
  enet_uint8  players;
  enet_uint8  maxplayers;
  char        name[MAX_STRLEN + 2];
  char        map[MAX_STRLEN + 2];
  time_t      death_time;
  time_t      timestamp;
} server_t;

// real servers
static server_t servers[MS_MAX_SERVERS];
static int max_servers = DEFAULT_MAX_SERVERS;
static int max_servers_per_host = DEFAULT_MAX_PER_HOST;
static int num_servers = 0;

// fake servers to show on old versions of the game
static const server_t fake_servers[] = {
  {
    .name  = "! \xc2\xc0\xd8\xc0 \xca\xce\xcf\xc8\xdf \xc8\xc3\xd0\xdb "
             "\xd3\xd1\xd2\xc0\xd0\xc5\xcb\xc0! "
             "\xd1\xca\xc0\xd7\xc0\xc9\xd2\xc5 \xcd\xce\xc2\xd3\xde C "
             "doom2d.org !",
    .map   = "! Your game is outdated. "
             "Get latest version at doom2d.org !",
    .proto = 255,
  },
  {
    .name  = "! \xcf\xd0\xce\xc1\xd0\xce\xd1\xdcTE \xcf\xce\xd0\xd2\xdb "
             "25666 \xc8 57133 HA CEPBEPE \xcf\xc5\xd0\xc5\xc4 \xc8\xc3\xd0\xce\xc9 !",
    .map   = "! Forward ports 25666 and 57133 before hosting !",
    .proto = 255,
  },
};
static const int num_fake_servers = sizeof(fake_servers) / sizeof(*fake_servers);

// ban list
static ban_record_t *banlist;

// settings
static int ms_port = DEFAULT_PORT;
static int ms_timeout = DEFAULT_TIMEOUT;
static int ms_spam_cap = DEFAULT_SPAM_CAP;
static char ms_motd[MAX_STRLEN + 1] = "";
static char ms_urgent[MAX_STRLEN + 1] = "";
static ENetHost *ms_host;

// network buffers
static enet_uint8 buf_send_data[NET_BUFSIZE];
static enet_buf_t buf_send = { .data = buf_send_data, .size = sizeof(buf_send_data) };
static enet_buf_t buf_recv; // rx data supplied by enet packets

// stupid client spam filter
static enet_uint32 cl_last_addr;
static time_t cl_last_time;
static int cl_spam_cnt;

/* common utility functions */

static char *u_vabuf(void) {
  static char vabuf[4][MAX_STRLEN];
  static int idx = 0;
  char *ret = vabuf[idx++];
  if (idx >= 4) idx = 0;
  return ret;
}

static const char *u_strtime(const time_t t) {
  char *buf = u_vabuf();
  struct tm *ptm = localtime(&t);
  strftime(buf, MAX_STRLEN - 1, "%d/%m/%y %H:%M:%S", ptm);  
  return buf;
}

static inline const char *u_logprefix(const enum log_severity_e s) {
  switch (s) {
    case LOG_WARN: return "WARNING: ";
    case LOG_ERROR: return "ERROR: ";
    default: return "";
  }
}

static void u_log(const enum log_severity_e severity, const char *fmt, ...) {
  printf("[%s] %s", u_strtime(time(NULL)), u_logprefix(severity));
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  printf("\n");
}

static void __attribute__((noreturn)) u_fatal(const char *fmt, ...) {
  fprintf(stderr, "[%s] FATAL ERROR:\n", u_strtime(time(NULL)));
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  fflush(stderr);
  exit(1);
}

static bool u_strisprint(const char *str) {
  if (!str || !*str)
    return false;
  for (const char *p = str; *p; ++p) {
    // only stuff before space, DEL, NBSP and SHY are considered garbage since we're on 1251
    if (*p < 0x20 || *p == 0x7F || *p == 0xA0 || *p == 0xAD)
      return false;
  }
  return true;
}

static bool u_strisver(const char *str) {
  if (!str || !*str)
    return false;
  for (const char *p = str; *p; ++p) {
    // version strings consist of 0-9 . and space
    if (!isdigit(*p) && *p != '.' && *p != ' ')
      return false;
  }
  return true;
}

static const char *u_iptostr(const enet_uint32 host) {
  ENetAddress addr = { .host = host, .port = 0 };
  char *buf = u_vabuf();
  enet_address_get_host_ip(&addr, buf, MAX_STRLEN - 1);
  return buf;
}

static bool u_readtextfile(const char *fname, char *buf, size_t max) {
  FILE *f = fopen(fname, "r");
  char *const end = buf + max - 1;
  char *p = buf;
  if (f) {
    char ln[max];
    char *const lend = ln + max - 1;
    while (p < end && fgets(ln, max, f)) {
      for (char *n = ln; n < lend && *n && *n != '\r' && *n != '\n'; ++n) {
        *(p++) = *n;
        if (p == end) break;
      }
    }
    *p = '\0';
    fclose(f);
    return true;
  }
  return false;
}

static inline enet_uint32 u_prefixtomask(const enet_uint32 prefix) {
  return ENET_HOST_TO_NET_32((0xFFFFFFFF << (32 - prefix)) & 0xFFFFFFFF);
}

static inline enet_uint32 u_masktoprefix(const enet_uint32 mask) {
  return (32 - __builtin_ctz(mask));
}

static inline void u_printsv(const server_t *sv) {
  printf("* addr: %s:%d\n", u_iptostr(sv->host), sv->port);
  printf("* name: %s\n", sv->name);
  printf("* map:  %s (mode %d)\n", sv->map, sv->gamemode);
  printf("* plrs: %d/%d\n", sv->players, sv->maxplayers);
  printf("* flag: %04x\n", sv->flags);
}

/* buffer utility functions */

static inline int b_enough_left(enet_buf_t *buf, size_t size) {
  if (buf->pos + size > buf->size) {
    buf->overflow = 1;
    return 0;
  }
  return 1;
}

static enet_uint8 b_read_uint8(enet_buf_t *buf) {
  if (b_enough_left(buf, 1))
    return buf->data[buf->pos++];
  return 0;
}

static enet_uint16 b_read_uint16(enet_buf_t *buf) {
  enet_uint16 ret = 0;

  if (b_enough_left(buf, sizeof(ret))) {
    ret = *(enet_uint16*)(buf->data + buf->pos);
    buf->pos += sizeof(ret);
  }

  return ret;
}

static char *b_read_dstring(enet_buf_t *buf) {
  char *ret = NULL;

  if (b_enough_left(buf, 1)) {
    const size_t len = b_read_uint8(buf);
    if (b_enough_left(buf, len)) {
      ret = malloc(len + 1);
      memmove(ret, (char*)(buf->data + buf->pos), len);
      buf->pos += len;
      ret[len] = '\0';
    }
  }

  return ret;
}

static char *b_read_dstring_to(enet_buf_t *buf, char *out, size_t out_size) {
  if (b_enough_left(buf, 1)) {
    const size_t len = b_read_uint8(buf);
    if (b_enough_left(buf, len)) {
      if (len < out_size) {
        memmove(out, (char*)(buf->data + buf->pos), len);
        out[len] = '\0';
      } else if (out_size) { 
        out[0] = '\0';
      }
      buf->pos += len;
      return out;
    }
  }
  return NULL;
}

static void b_write_uint8(enet_buf_t *buf, enet_uint8 val) {
  buf->data[buf->pos++] = val;
}

static void b_write_uint16(enet_buf_t *buf, enet_uint16 val) {
  *(enet_uint16*)(buf->data + buf->pos) = val;
  buf->pos += sizeof(val);
}

static void b_write_dstring(enet_buf_t *buf, const char* val) {
  enet_uint8 len = strlen(val);
  b_write_uint8(buf, len);
  memmove((char*)(buf->data + buf->pos), val, len);
  buf->pos += len;
}

void b_write_server(enet_buf_t *buf, const server_t *s) {
  b_write_dstring(buf, u_iptostr(s->host));
  b_write_uint16 (buf, s->port);
  b_write_dstring(buf, s->name);
  b_write_dstring(buf, s->map);
  b_write_uint8  (buf, s->gamemode);
  b_write_uint8  (buf, s->players);
  b_write_uint8  (buf, s->maxplayers);
  b_write_uint8  (buf, s->proto);
  b_write_uint8  (buf, (s->flags & SV_FL_PASSWORD));
}

/* server functions */

static void sv_remove(const enet_uint32 host, const enet_uint16 port) {
  for (int i = 0; i < max_servers; ++i) {
    if (servers[i].host == host && servers[i].port == port) {
      servers[i].host = 0;
      servers[i].port = 0;
      --num_servers;
    }
  }
}

static void sv_remove_by_host(enet_uint32 host, enet_uint32 mask) {
  host &= mask;
  for (int i = 0; i < max_servers; ++i) {
    if (servers[i].host && (servers[i].host & mask) == host) {
      servers[i].host = 0;
      servers[i].port = 0;
      --num_servers;
    }
  }
}

static int sv_count_by_host(enet_uint32 host, enet_uint32 mask) {
  host &= mask;
  int count = 0;
  for (int i = 0; i < max_servers; ++i) {
    if (servers[i].host && (servers[i].host & mask) == host)
      ++count;
  }
  return count;
}

static time_t sv_last_timestamp_for_host(enet_uint32 host, enet_uint32 mask) {
  host &= mask;
  time_t last = 0;
  for (int i = 0; i < max_servers; ++i) {
    if (servers[i].host && (servers[i].host & mask) == host) {
      if (servers[i].timestamp > last)
        last = servers[i].timestamp;
    }
  }
  return last;
}

static inline server_t *sv_find_or_add(const enet_uint32 host, const enet_uint32 port) {
  server_t *empty = NULL;
  for (int i = 0; i < max_servers; ++i) {
    server_t *s = servers + i;
    if (s->host == host && s->port == port)
      return s; // this server already exists
    if (!s->host && !empty)
      empty = s; // remember the first empty slot in case it's needed later
  }
  return empty;
}

/* ban list functions */

static inline time_t ban_get_time(const int cnt) {
  static const time_t times[] = {
       1 *  5 * 60,
       1 * 30 * 60,
       1 * 60 * 60,
      24 * 60 * 60,
      72 * 60 * 60,
     720 * 60 * 60,
    8760 * 60 * 60,
  };

  static const size_t numtimes = sizeof(times) / sizeof(*times);

  if (cnt >= numtimes || cnt < 0)
    return times[numtimes - 1];

  return times[cnt];
}

static ban_record_t *ban_check(const enet_uint32 host) {
  const time_t now = time(NULL);

  for (ban_record_t *b = banlist; b; b = b->next) {
    if ((b->host & b->mask) == (host & b->mask)) {
      if (b->cur_ban > now)
        return b;
    }
  }

  return NULL;
}

static inline ban_record_t *ban_record_check(const enet_uint32 host) {
  for (ban_record_t *b = banlist; b; b = b->next) {
    if ((b->host & b->mask) == (host & b->mask))
      return b;
  }
  return NULL;
}

static ban_record_t *ban_record_add_addr(const enet_uint32 host, const enet_uint32 mask, const int cnt, const time_t cur) {
  ban_record_t *rec = ban_record_check(host);
  if (rec) return rec;

  rec = calloc(1, sizeof(*rec));
  if (!rec) return NULL;

  rec->host = host & mask;
  rec->mask = mask;
  if (rec->mask == 0) rec->mask = NET_FULLMASK;
  rec->ban_count = cnt;
  rec->cur_ban = cur;

  if (banlist) banlist->prev = rec;
  rec->next = banlist;
  banlist = rec;

  return rec;
}

static ban_record_t *ban_record_add_ip(const char *ip, const int cnt, const time_t cur) {
  enet_uint32 prefix = 32;

  // find and get the prefix length, if any
  char ip_copy[24] = { 0 };
  strncpy(ip_copy, ip, sizeof(ip_copy) - 1);
  char *slash = strrchr(ip_copy, '/');
  if (slash) {
    *slash++ = '\0'; // strip the prefix length off
    if (*slash) prefix = atoi(slash);
  }

  ENetAddress addr = { 0 };
  if (enet_address_set_host_ip(&addr, ip_copy) != 0) {
    u_log(LOG_ERROR, "banlist: `%s` is not a valid IP address", ip_copy);
    return NULL;
  }

  // transform prefix length into mask
  const enet_uint32 mask = u_prefixtomask(prefix);

  return ban_record_add_addr(addr.host, mask, cnt, cur);
}

static void ban_free_list(void) {
  ban_record_t *rec = banlist;
  while (rec) {
    ban_record_t *next = rec->next;
    free(rec);
    rec = next;
  }
  banlist = NULL;
}

static void ban_load_list(const char *fname) {
  FILE *f = fopen(fname, "r");
  if (!f) {
    u_log(LOG_WARN, "banlist: could not open %s for reading", fname);
    return;
  }

  char ln[MAX_STRLEN] = { 0 };

  while (fgets(ln, sizeof(ln), f)) {
    for (int i = sizeof(ln) - 1; i >= 0; --i)
      if (ln[i] == '\n' || ln[i] == '\r')
        ln[i] = 0;

    if (ln[0] == 0)
      continue;

    char ip[21] = { 0 }; // optionally includes the "/nn" prefix length at the end
    time_t exp = 0;
    int count = 0;
    if (sscanf(ln, "%20s %ld %d", ip, &exp, &count) < 3) {
      u_log(LOG_ERROR, "banlist: malformed line: `%s`", ln);
      continue;
    }

    if (ban_record_add_ip(ip, count, exp))
      u_log(LOG_NOTE, "banlist: banned %s until %s (ban level %d)", ip, u_strtime(exp), count);
  }

  fclose(f);
}

static void ban_save_list(const char *fname) {
  FILE *f = fopen(fname, "w");
  if (!f) {
    u_log(LOG_ERROR, "banlist: could not open %s for writing", fname);
    return;
  }

  for (ban_record_t *rec = banlist; rec; rec = rec->next) {
    if (rec->ban_count)
      fprintf(f, "%s/%u %ld %d\n", u_iptostr(rec->host), u_masktoprefix(rec->mask), rec->cur_ban, rec->ban_count);
  }

  fclose(f);
}

static bool ban_sanity_check(const server_t *srv) {
  // can't have more than 24 maxplayers; can't have more than max
  if (srv->players > srv->maxplayers || srv->maxplayers > SV_MAX_PLAYERS || srv->maxplayers == 0)
    return false;
  // name and map have to be non-garbage
  if (!u_strisprint(srv->map) || !u_strisprint(srv->name))
    return false;
  // these protocols don't exist
  if (srv->proto < SV_PROTO_MIN || srv->proto > SV_PROTO_MAX)
    return false;
  // the game doesn't allow server names longer than 64 chars
  if (strlen(srv->name) > SV_NAME_MAX)
    return false;
  // game mode has to actually exist
  if (srv->gamemode > SV_MAX_GAMEMODE)
    return false;
  // flags field can't be higher than the sum of all the flags
  if (srv->flags > SV_FL_MAX)
    return false;
  return true;
}

static void ban_add(const enet_uint32 host, const char *reason) {
  const time_t now = time(NULL);

  ban_record_t *rec = ban_record_add_addr(host, NET_FULLMASK, 0, 0);
  if (!rec) u_fatal("OOM trying to ban %s", u_iptostr(host));

  rec->cur_ban = now + ban_get_time(rec->ban_count);
  rec->ban_count++;

  u_log(LOG_NOTE, "banned %s until %s, reason: %s, ban level: %d", u_iptostr(rec->host), u_strtime(rec->cur_ban), reason, rec->ban_count);

  ban_save_list(MS_BAN_FILE);

  sv_remove_by_host(host, NET_FULLMASK);

  if (host == cl_last_addr)
    cl_last_addr = 0;
}

static inline void ban_peer(ENetPeer *peer, const char *reason) {
  if (peer) {
    ban_add(peer->address.host, reason);
    enet_peer_reset(peer);
  }
}

/* main */

static void deinit(void) {
  // ban_save_list(MS_BAN_FILE);
  ban_free_list();
  if (ms_host) {
    enet_host_destroy(ms_host);
    ms_host = NULL;
  }
  enet_deinitialize();
}

#ifdef SIGUSR1
static void sigusr_handler(int signum) {
  if (signum == SIGUSR1) {
    u_log(LOG_WARN, "received SIGUSR1, reloading banlist");
    ban_free_list();
    ban_load_list(MS_BAN_FILE);
  }
}
#endif

static bool handle_msg(const enet_uint8 msgid, ENetPeer *peer) {
  server_t *sv = NULL;
  server_t tmpsv = { 0 };
  char clientver[MAX_STRLEN] = { 0 };
  const time_t now = time(NULL);

  switch (msgid) {
    case NET_MSG_ADD:
      tmpsv.port = b_read_uint16(&buf_recv);
      b_read_dstring_to(&buf_recv, tmpsv.name, sizeof(tmpsv.name));
      b_read_dstring_to(&buf_recv, tmpsv.map, sizeof(tmpsv.map));
      tmpsv.gamemode = b_read_uint8(&buf_recv);
      tmpsv.players = b_read_uint8(&buf_recv);
      tmpsv.maxplayers = b_read_uint8(&buf_recv);
      tmpsv.proto = b_read_uint8(&buf_recv);
      tmpsv.flags = b_read_uint8(&buf_recv);

      if (buf_recv.overflow) {
        ban_peer(peer, "malformed MSG_ADD");
        return true;
      }

      sv = sv_find_or_add(peer->address.host, tmpsv.port);
      if (!sv) {
        u_log(LOG_ERROR, "ran out of server slots trying to add %s:%d", u_iptostr(peer->address.host), tmpsv.port);
        return true;
      }

      if (sv->host == peer->address.host) {
        // old server; update it
        memcpy(sv->map, tmpsv.map, sizeof(sv->map));
        memcpy(sv->name, tmpsv.name, sizeof(sv->name));
        sv->players = tmpsv.players;
        sv->maxplayers = tmpsv.maxplayers;
        sv->flags = tmpsv.flags;
        sv->gamemode = tmpsv.gamemode;
        // first check if the new values are garbage
        if (!ban_sanity_check(sv)) {
          ban_peer(peer, "tripped sanity check");
          return true;
        }
        // only then update the times
        sv->death_time = now + ms_timeout;
        sv->timestamp = now;
        u_log(LOG_NOTE, "updated server #%d:", sv - servers);
        u_printsv(sv);
      } else {
        // new server; first check if this host is creating too many servers in the list
        if (max_servers_per_host) {
          const int count = sv_count_by_host(peer->address.host, NET_FULLMASK);
          if (count >= max_servers_per_host) {
            ban_peer(peer, "too many servers in list");
            return true;
          }
          /*
          // FIXME: commented out as this might trip when the master restarts
          if (count > 0) {
            // check if this is too soon to create a new server
            const time_t delta = now - sv_last_timestamp_for_host(peer->address.host, NET_FULLMASK);
            if (delta < count * SV_NEW_SERVER_INTERVAL) {
              ban_peer(peer, "creating servers too fast");
              return true;
            }
          }
          */
        }
        // then add that shit
        *sv = tmpsv;
        sv->host = peer->address.host;
        sv->death_time = now + ms_timeout;
        sv->timestamp = now;
        if (!ban_sanity_check(sv)) {
          sv->host = 0;
          sv->port = 0;
          ban_peer(peer, "tripped sanity check");
          return true;
        }
        ++num_servers;
        u_log(LOG_NOTE, "added new server #%d:", sv - servers);
        u_printsv(sv);
      }
      return true;

    case NET_MSG_RM:
      tmpsv.port = b_read_uint16(&buf_recv);
      if (buf_recv.overflow) {
        ban_peer(peer, "malformed MSG_RM");
        return true;
      }
      sv_remove(peer->address.host, tmpsv.port);
      return true;

    case NET_MSG_LIST:
      buf_send.pos = 0;
      b_write_uint8(&buf_send, NET_MSG_LIST);

      clientver[0] = 0;
      if (buf_recv.size > 2) {
        // holy shit a fresh client
        b_read_dstring_to(&buf_recv, clientver, sizeof(clientver));
        b_write_uint8(&buf_send, num_servers);
      } else {
        // old client; feed him fake servers first
        b_write_uint8(&buf_send, num_servers + num_fake_servers);
        for (int i = 0; i < num_fake_servers; ++i)
          b_write_server(&buf_send, &fake_servers[i]);
      }

      if (buf_recv.overflow) {
        ban_peer(peer, "malformed MSG_LIST");
        return true;
      }

      if (clientver[0] && !u_strisver(clientver)) {
        ban_peer(peer, "malformed MSG_LIST clientver");
        return true;
      }

      for (int i = 0; i < max_servers; ++i) {
        if (servers[i].host)
          b_write_server(&buf_send, servers + i);
      }

      if (clientver[0]) {
        // TODO: check if this client is outdated (?) and send back new verstring
        // for now just write the same shit back
        b_write_dstring(&buf_send, clientver);
        // write the motd and urgent message
        b_write_dstring(&buf_send, ms_motd);
        b_write_dstring(&buf_send, ms_urgent);
      }

      ENetPacket *p = enet_packet_create(buf_send.data, buf_send.pos, ENET_PACKET_FLAG_RELIABLE);
      enet_peer_send(peer, NET_CH_MAIN, p);
      enet_host_flush(ms_host);

      u_log(LOG_NOTE, "sent server list to %s:%d (ver %s)", u_iptostr(peer->address.host), peer->address.port, clientver[0] ? clientver : "<old>");
      return true;

    default:
      break;
  }

  return false;
}

static void print_usage(void) {
  printf("Usage: d2df_master [OPTIONS...]\n");
  printf("Available options:\n");
  printf("-h     show this message and exit\n");
  printf("-p N   listen on port N (default: %d)\n", DEFAULT_PORT);
  printf("-t N   seconds before server is removed from list (default: %d)\n", DEFAULT_TIMEOUT);
  printf("-s N   max number of servers in server list, 1-%d (default: %d)\n", MS_MAX_SERVERS, DEFAULT_MAX_SERVERS);
  printf("-d N   if N > 0, disallow more than N servers on the same IP (default: %d)\n", DEFAULT_MAX_PER_HOST);
  printf("-f N   crappy spam filter: ban people after they send N requests in a row too fast (default: %d)\n", DEFAULT_SPAM_CAP);
  fflush(stdout);
}

static inline bool parse_int_arg(int argc, char **argv, const int i, const char *name, int vmin, int vmax, int *outval) {
  if (strcmp(name, argv[i]))
    return false;

  if (i >= argc - 1) {
    fprintf(stderr, "expected integer value after %s\n", name);
    return false;
  }

  const int v = atoi(argv[i + 1]);
  if (v < vmin || v > vmax) {
    fprintf(stderr, "expected integer value in range %d - %d\n", vmin, vmax);
    return false;
  }

  *outval = v;
  return true;
}

static bool parse_args(int argc, char **argv) {
  if (argc < 2)
    return true;

  if (!strcmp(argv[1], "-h")) {
    print_usage();
    return false;
  }

  for (int i = 1; i < argc; ++i) {
    const bool success =
         parse_int_arg(argc, argv, i, "-p", 1, 0xFFFF, &ms_port)
      || parse_int_arg(argc, argv, i, "-t", 1, 0x7FFFFFFF, &ms_timeout)
      || parse_int_arg(argc, argv, i, "-s", 1, MS_MAX_SERVERS, &max_servers)
      || parse_int_arg(argc, argv, i, "-d", 0, MS_MAX_SERVERS, &max_servers_per_host)
      || parse_int_arg(argc, argv, i, "-f", 0, 0xFFFF, &ms_spam_cap);
    if (success) {
      ++i;
    } else {
      fprintf(stderr, "unknown or invalid argument: %s\n", argv[i]);
      return false;
    }
  }

  return true;
}

// a stupid thing to filter sustained spam from a single IP
static bool spam_filter(ENetPeer *peer) {
  const time_t now = time(NULL);
  if (peer->address.host == cl_last_addr) {
    // spam === sending shit faster than once a second
    if (now - cl_last_time < 1) {
      if (cl_spam_cnt > 1)
        u_log(LOG_WARN, "address %s is sending packets too fast", u_iptostr(peer->address.host));
      if (++cl_spam_cnt >= ms_spam_cap) {
        ban_peer(peer, "spam");
        return true;
      }
    } else {
      cl_spam_cnt = 0;
    }
  } else {
    cl_last_addr = peer->address.host;
    cl_spam_cnt = 0;
  }
  cl_last_time = now;
  return false;
}

int main(int argc, char **argv) {
  if (enet_initialize() != 0)
    u_fatal("could not init enet");

  if (!parse_args(argc, argv))
    return 1; // early exit

  u_log(LOG_NOTE, "d2df master server starting on port %d", ms_port);

  if (!u_readtextfile(MS_MOTD_FILE, ms_motd, sizeof(ms_motd)))
    u_log(LOG_NOTE, "couldn't read motd from %s", MS_MOTD_FILE);
  else
    u_log(LOG_NOTE, "motd: %s", ms_motd);

  if (!u_readtextfile(MS_URGENT_FILE, ms_urgent, sizeof(ms_urgent)))
    u_log(LOG_NOTE, "couldn't read urgentmsg from %s", MS_URGENT_FILE);
  else
    u_log(LOG_NOTE, "urgentmsg: %s", ms_urgent);

  ban_load_list(MS_BAN_FILE);

  atexit(deinit);

#ifdef SIGUSR1
  signal(SIGUSR1, sigusr_handler);
#endif

  ENetAddress addr;
  addr.host = 0;
  addr.port = ms_port;
  ms_host = enet_host_create(&addr, MS_MAX_CLIENTS, NET_CH_COUNT, 0, 0);
  if (!ms_host)
    u_fatal("could not create enet host on port %d", ms_port);

  bool running = true;
  enet_uint8 msgid = 0;
  ENetEvent event;
  while (running) {
    while (enet_host_service(ms_host, &event, 5000) > 0) {
      bool filtered = !event.peer || ban_check(event.peer->address.host);
      if (!filtered && event.type != ENET_EVENT_TYPE_DISCONNECT)
        filtered = spam_filter(event.peer);

      if (!filtered) {
        switch (event.type) {
          case ENET_EVENT_TYPE_CONNECT:
            u_log(LOG_NOTE, "%s:%d connected", u_iptostr(event.peer->address.host), event.peer->address.port);
            break;

          case ENET_EVENT_TYPE_RECEIVE:
            if (!event.packet || event.packet->dataLength == 0) {
              ban_peer(event.peer, "empty packet");
              break;
            }
            // set up receive buffer
            buf_recv.pos = 0;
            buf_recv.overflow = 0;
            buf_recv.data = event.packet->data;
            buf_recv.size = event.packet->dataLength;
            // read message id and handle the message
            msgid = b_read_uint8(&buf_recv);
            if (!handle_msg(msgid, event.peer)) {
              // cheeky cunt sending invalid messages
              ban_peer(event.peer, "unknown message");
            }
            break;

          default:
            break;
        }
      } else if (event.peer) {
        enet_peer_reset(event.peer);
      }

      if (event.packet) {
        buf_recv.data = NULL;
        buf_recv.pos = 0;
        buf_recv.size = 0;
        buf_recv.overflow = 0;
        enet_packet_destroy(event.packet);
      }
    }

    const time_t now = time(NULL);
    for (int i = 0; i < max_servers; ++i) {
      if (servers[i].host) {
        if (servers[i].death_time <= now) {
          u_log(LOG_NOTE, "server #%d %s:%d timed out", i, u_iptostr(servers[i].host), servers[i].port);
          servers[i].host = 0;
          servers[i].port = 0;
          --num_servers;
        }
      }
    }
  }
}
