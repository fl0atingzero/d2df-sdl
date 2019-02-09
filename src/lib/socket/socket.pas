// TCP/IP stack for winDOS
// ftp://ftp.delorie.com/pub/djgpp/current/v2tk/ls080b.zip
// ftp://ftp.delorie.com/pub/djgpp/current/v2tk/ls080d.zip
// ftp://ftp.delorie.com/pub/djgpp/current/v2tk/ls080s.zip

{$MODE OBJFPC}
{$PACKRECORDS C}

{$MODESWITCH OUT}
{$LONGSTRINGS ON}
{$MACRO ON}

{$LINKLIB libsocket.a}
{$LINKLIB libc.a}
{$DEFINE LibraryLibSockDecl := cdecl}
{$DEFINE LibraryLibSockImp := cdecl; external}
{$DEFINE LibraryLibSockVar := cvar; external}

unit socket;

interface

  uses ctypes;

  (* Start-up & shutdown *)
  function __lsck_init: cint; LibraryLibSockImp;
  procedure __lsck_uninit; LibraryLibSockImp;

  (* Configuration *)
  function __lsck_config_getdir: PChar; LibraryLibSockImp;
  function __lsck_config_setdir (newdir: PChar): PChar; LibraryLibSockImp;
  function __lsck_config_getfile: PChar; LibraryLibSockImp;
  function __lsck_config_setfile (newfile: PChar): PChar; LibraryLibSockImp;

  (* DNS address(es) *)
  function __lsck_getdnsaddr: PChar; LibraryLibSockImp;
  function __lsck_getdnsaddrs: PPChar; LibraryLibSockImp;

  (* Error fudging *)
  //function lsck_strerror (errnum: cint): PChar; LibraryLibSockImp;
  function lsck_strerror (errnum: cint): PChar; LibraryLibSockImp name 'strerror';

  (* File descriptor tests *)
  function __fd_is_socket (fd: cint): cint; LibraryLibSockImp;
  function __fd_is_valid (fd: cint): cint; LibraryLibSockImp;

  (* Debugging *)
  const
    LSCK_DEBUG_OFF = 0;
    LSCK_DEBUG_NORMAL = 1;
    LSCK_DEBUG_VERBOSE = 2;
    LSCK_DEBUG_ON = LSCK_DEBUG_NORMAL;

  procedure __lsck_debug_setlevel (level: cint); LibraryLibSockImp;
  function __lsck_debug_getlevel: cint; LibraryLibSockImp;
  procedure __lsck_debug_enable; LibraryLibSockImp;
  procedure __lsck_debug_disable; LibraryLibSockImp;
  function __lsck_debug_enabled: cint; LibraryLibSockImp;

  (* lsck/copyrite.h *)
  function __lsck_get_version: PChar; LibraryLibSockImp;
  function __lsck_get_copyright: PChar; LibraryLibSockImp;

  (* lsck/domname.h *)
  function getdomainname (name: PChar; len: csize_t): cint; LibraryLibSockImp;
  function setdomainname (const name: PChar; len: csize_t): cint; LibraryLibSockImp;

  (* lsck/hostname.h *)
  function gethostname (buf: PChar; size: cint): cint; LibraryLibSockImp;
  function sethostname (buf: PChar; size: cint): cint; LibraryLibSockImp;

  (* lsck/if.h *)
  {...}



  function htonl (_val: culong): culong; LibraryLibSockImp;
  function ntohl (_val: culong): culong; LibraryLibSockImp;
  function htons (_val: cushort): cushort; LibraryLibSockImp;
  function ntohs (_val: cushort): cushort; LibraryLibSockImp;



  (* MACRO *)
  const
    FD_MAXFDSET = 256; (* FD_SETSIZE *)

  type
    (* djgpp sys-include -> sys/wtypes.h -> fd_set *)
    TFDSet = record
      fd_bits: array [0..(FD_MAXFDSET + 7) DIV 8] of cuchar;
    end;

  function fpFD_SET (fdno: cint; var nset: TFDSet): cint;
  function fpFD_CLR (fdno: cint; var nset: TFDSet): cint;
  function fpFD_ZERO (out nset: TFDSet): cint;
  function fpFD_ISSET (fdno: cint; const nset: TFDSet): cint;

implementation

  function fpFD_SET (fdno: cint; var nset: TFDSet): cint;
  begin
    if (fdno < 0) or (fdno > FD_MAXFDSET) then
      exit(-1);
    nset.fd_bits[fdno div 8] := nset.fd_bits[fdno div 8] OR (culong(1) shl (fdno and 7));
    fpFD_SET := 0
  end;

  function fpFD_CLR (fdno: cint; var nset: TFDSet): cint;
  begin
    if (fdno < 0) or (fdno > FD_MAXFDSET) Then
      exit(-1);
    nset.fd_bits[fdno div 8] := nset.fd_bits[fdno div 8] AND Cardinal(NOT (culong(1) shl (fdno and 7)));
    fpFD_CLR := 0
  end;

  function fpFD_ZERO (out nset: TFDSet): cint;
    var i: longint;
  begin
    for i := 0 to (FD_MAXFDSET + 7) div 8 DO
      nset.fd_bits[i] := 0;
    fpFD_ZERO := 0
  end;

  function fpFD_ISSET (fdno: cint; const nset: TFDSet): cint;
  begin
    if (fdno < 0) or (fdno > FD_MAXFDSET) Then
      exit(-1);
    if ((nset.fd_bits[fdno div 8]) and (culong(1) shl (fdno and 7))) > 0 then
      fpFD_ISSET := 1
    else
      fpFD_ISSET := 0
  end;

end.
