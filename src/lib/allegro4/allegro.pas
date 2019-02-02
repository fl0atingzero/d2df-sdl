{$LONGSTRINGS ON}
{$MACRO ON}

{$IF DEFINED(GO32V2)}
  {$LINKLIB liballeg.a}
  {$LINKLIB libc.a}
  {$LINKLIB libgcc.a}
  {$DEFINE LibraryLibAllegroDecl := cdecl}
  {$DEFINE LibraryLibAllegroImp := cdecl; external}
  {$DEFINE LibraryLibAllegroVar := cvar; external}
{$ELSEIF DEFINED(WINDOWS)}
  {$DEFINE LibraryLibAllegroDecl := cdecl}
  {$DEFINE LibraryLibAllegroImp := cdecl; external 'alleg42.dll'}
  {$DEFINE LibraryLibAllegroVar := external 'alleg42.dll'}
{$ELSE}
  {$DEFINE LibraryLibAllegroDecl := cdecl}
  {$DEFINE LibraryLibAllegroImp := cdecl; external 'alleg'}
  {$DEFINE LibraryLibAllegroVar := cvar; external 'alleg'}
{$ENDIF}

unit allegro;

interface

  uses ctypes;

  const
{$IF DEFINED(GO32V2) OR DEFINED(WINDOWS)}
    ALLEGRO_VERSION = 4;
    ALEGRO_SUB_VERSION = 2;
    ALLEGRO_WIP_VERSION = 3;
    ALLEGRO_VERSION_STR = '4.2.3';
{$ELSE}
    ALLEGRO_VERSION = 4;
    ALEGRO_SUB_VERSION = 4;
    ALLEGRO_WIP_VERSION = 2;
    ALLEGRO_VERSION_STR = '4.4.2';
{$ENDIF}

    SYSTEM_AUTODETECT = 0;
    SYSTEM_NONE = $4e4f4e45;

    GFX_TEXT = -1;
    GFX_AUTODETECT = 0;
    GFX_AUTODETECT_FULLSCREEN = 1;
    GFX_AUTODETECT_WINDOWED = 2;
    GFX_SAFE = $53414645;

    DRAW_MODE_SOLID = 0;
    DRAW_MODE_XOR = 1;
    DRAW_MODE_COPY_PATTERN = 2;
    DRAW_MODE_SOLID_PATTERN = 3;
    DRAW_MODE_MASKED_PATTERN = 4;
    DRAW_MODE_TRANS = 5;

    KEY_A = 1;
    KEY_B = 2;
    KEY_C = 3;
    KEY_D = 4;
    KEY_E = 5;
    KEY_F = 6;
    KEY_G = 7;
    KEY_H = 8;
    KEY_I = 9;
    KEY_J = 10;
    KEY_K = 11;
    KEY_L = 12;
    KEY_M = 13;
    KEY_N = 14;
    KEY_O = 15;
    KEY_P = 16;
    KEY_Q = 17;
    KEY_R = 18;
    KEY_S = 19;
    KEY_T = 20;
    KEY_U = 21;
    KEY_V = 22;
    KEY_W = 23;
    KEY_X = 24;
    KEY_Y = 25;
    KEY_Z = 26;
    KEY_0 = 27;
    KEY_1 = 28;
    KEY_2 = 29;
    KEY_3 = 30;
    KEY_4 = 31;
    KEY_5 = 32;
    KEY_6 = 33;
    KEY_7 = 34;
    KEY_8 = 35;
    KEY_9 = 36;
    KEY_0_PAD = 37;
    KEY_1_PAD = 38;
    KEY_2_PAD = 39;
    KEY_3_PAD = 40;
    KEY_4_PAD = 41;
    KEY_5_PAD = 42;
    KEY_6_PAD = 43;
    KEY_7_PAD = 44;
    KEY_8_PAD = 45;
    KEY_9_PAD = 46;
    KEY_F1 = 47;
    KEY_F2 = 48;
    KEY_F3 = 49;
    KEY_F4 = 50;
    KEY_F5 = 51;
    KEY_F6 = 52;
    KEY_F7 = 53;
    KEY_F8 = 54;
    KEY_F9 = 55;
    KEY_F10 = 56;
    KEY_F11 = 57;
    KEY_F12 = 58;
    KEY_ESC = 59;
    KEY_TILDE = 60;
    KEY_MINUS = 61;
    KEY_EQUALS = 62;
    KEY_BACKSPACE = 63;
    KEY_TAB = 64;
    KEY_OPENBRACE = 65;
    KEY_CLOSEBRACE = 66;
    KEY_ENTER = 67;
    KEY_COLON = 68;
    KEY_QUOTE = 69;
    KEY_BACKSLASH = 70;
    KEY_BACKSLASH2 = 71;
    KEY_COMMA = 72;
    KEY_STOP  = 73;
    KEY_SLASH = 74;
    KEY_SPACE = 75;
    KEY_INSERT = 76;
    KEY_DEL = 77;
    KEY_HOME = 78;
    KEY_END = 79;
    KEY_PGUP = 80;
    KEY_PGDN = 81;
    KEY_LEFT = 82;
    KEY_RIGHT = 83;
    KEY_UP = 84;
    KEY_DOWN = 85;
    KEY_SLASH_PAD = 86;
    KEY_ASTERISK = 87;
    KEY_MINUS_PAD = 88;
    KEY_PLUS_PAD = 89;
    KEY_DEL_PAD = 90;
    KEY_ENTER_PAD = 91;
    KEY_PRTSCR = 92;
    KEY_PAUSE = 93;
    KEY_ABNT_C1 = 94;
    KEY_YEN = 95;
    KEY_KANA = 96;
    KEY_CONVERT = 97;
    KEY_NOCONVERT = 98;
    KEY_AT = 99;
    KEY_CIRCUMFLEX = 100;
    KEY_COLON2 = 101;
    KEY_KANJI = 102;
    KEY_EQUALS_PAD = 103;
    KEY_BACKQUOTE = 104;
    KEY_SEMICOLON = 105;
    KEY_COMMAND = 106;
    KEY_UNKNOWN1 = 107;
    KEY_UNKNOWN2 = 108;
    KEY_UNKNOWN3 = 109;
    KEY_UNKNOWN4 = 110;
    KEY_UNKNOWN5 = 111;
    KEY_UNKNOWN6 = 112;
    KEY_UNKNOWN7 = 113;
    KEY_UNKNOWN8 = 114;
    KEY_MODIFIERS = 115;
    KEY_LSHIFT = 115;
    KEY_RSHIFT = 116;
    KEY_LCONTROL = 117;
    KEY_RCONTROL = 118;
    KEY_ALT = 119;
    KEY_ALTGR = 120;
    KEY_LWIN = 121;
    KEY_RWIN = 122;
    KEY_MENU = 123;
    KEY_SCRLOCK = 124;
    KEY_NUMLOCK = 125;
    KEY_CAPSLOCK = 126;
    KEY_MAX   = 127;

    KB_SHIFT_FLAG = $0001;
    KB_CTRL_FLAG = $0002;
    KB_ALT_FLAG = $0004;
    KB_LWIN_FLAG = $0008;
    KB_RWIN_FLAG = $0010;
    KB_MENU_FLAG = $0020;
    KB_COMMAND_FLAG = $0040;
    KB_SCROLOCK_FLAG = $0100;
    KB_NUMLOCK_FLAG = $0200;
    KB_CAPSLOCK_FLAG = $0400;
    KB_INALTSEQ_FLAG = $0800;
    KB_ACCENT1_FLAG = $1000;
    KB_ACCENT2_FLAG = $2000;
    KB_ACCENT3_FLAG = $4000;
    KB_ACCENT4_FLAG = $8000;

    ALLEGRO_ERROR_SIZE = 256;
    PAL_SIZE = 256;

  type
    PBITMAP = ^BITMAP;
    BITMAP = record
      w, h: cint;
      clip: cint;
      cl, cr, ct, cb: cint;
      vtable: Pointer; {PGFX_VTABLE}
      write_bank: Pointer;
      read_bank: Pointer;
      dat: Pointer;
      id: culong;
      extra: Pointer;
      x_ofs: cint;
      y_ofs: cint;
      seg: cint;
      line: Pointer;
    end;

    PGFX_MODE = ^GFX_MODE;
    GFX_MODE = record
      width, height, bpp: cint;
    end;

    PGFX_MODE_LIST = ^GFX_MODE_LIST;
    GFX_MODE_LIST = record
      num_modes: cint;
      mode: PGFX_MODE;
    end;

    RGB = record
      r, g, b, filler: cuchar;
    end;

    PALETTE = array [0..PAL_SIZE - 1] of RGB;
    PPALETTE = ^PALETTE;

   KeyboardCallback = procedure (scancode: cint); LibraryLibAllegroDecl;   
   AtExitCallback = procedure; LibraryLibAllegroDecl;
   AtExitFunction = function (func: AtExitCallback): cint; LibraryLibAllegroDecl;
   TimerIntCallback = procedure; LibraryLibAllegroDecl;
   QuitCallback = procedure; LibraryLibAllegroDecl;

  var
    allegro_id: array [0..ALLEGRO_ERROR_SIZE] of char; LibraryLibAllegroVar;
    allegro_error: array [0..ALLEGRO_ERROR_SIZE] of char; LibraryLibAllegroVar;
    keyboard_lowlevel_callback: KeyboardCallback; LibraryLibAllegroVar;
    screen: PBITMAP; LibraryLibAllegroVar;
    black_palette: PALETTE; LibraryLibAllegroVar;
    desktop_palette: PALETTE; LibraryLibAllegroVar;
    default_palette: PALETTE; LibraryLibAllegroVar;
    _current_palette: PALETTE; LibraryLibAllegroVar;
    key_shifts: cint; LibraryLibAllegroVar;

  function get_desktop_resolution (width, height: Pcint): cint; LibraryLibAllegroImp;
  function get_gfx_mode_list (card: cint): PGFX_MODE_LIST; LibraryLibAllegroImp;
  procedure destroy_gfx_mode_list (gfx_mode_list: PGFX_MODE_LIST); LibraryLibAllegroImp;
  function set_gfx_mode (card, w, h, v_w, v_h: cint): cint; LibraryLibAllegroImp;
  procedure set_window_title (name: Pchar); LibraryLibAllegroImp;
  function create_video_bitmap (width, height: cint): PBITMAP; LibraryLibAllegroImp;
  procedure destroy_bitmap (bitmap: PBITMAP); LibraryLibAllegroImp;
  function show_video_bitmap (bitmap: PBITMAP): cint; LibraryLibAllegroImp;
  function poll_keyboard: cint; LibraryLibAllegroImp;
  function install_keyboard: cint; LibraryLibAllegroImp;
  procedure remove_keyboard; LibraryLibAllegroImp;
  function _install_allegro_version_check (system_id: cint; errno_ptr: Pcint; atexit_ptr: AtExitFunction; version: cint): cint; LibraryLibAllegroImp;

  function install_timer: cint; LibraryLibAllegroImp;
  procedure remove_timer; LibraryLibAllegroImp;
  procedure set_keyboard_rate (delay, _repeat: cint); LibraryLibAllegroImp;
  function makeacol (r, g, b, a: cint): cint; LibraryLibAllegroImp;
  function makecol (r, g, b: cint): cint; LibraryLibAllegroImp;
  procedure clear_to_color (source: PBITMAP; color: cint); LibraryLibAllegroImp;
  procedure putpixel (bmp: PBITMAP; x, y, color: cint); LibraryLibAllegroImp;
  function getpixel (bmp: PBITMAP; x, y: cint): cint; LibraryLibAllegroImp;
  procedure fastline (bmp: PBITMAP; x1, y_1, x2, y2, color: cint); LibraryLibAllegroImp;
  procedure draw_sprite (bmp, sprite: PBITMAP; x, y: cint); LibraryLibAllegroImp;
  procedure draw_sprite_v_flip (bmp, sprite: PBITMAP; x, y: cint); LibraryLibAllegroImp;
  procedure draw_sprite_h_flip (bmp, sprite: PBITMAP; x, y: cint); LibraryLibAllegroImp;
  procedure draw_sprite_vh_flip (bmp, sprite: PBITMAP; x, y: cint); LibraryLibAllegroImp;
  procedure rect (bmp: PBITMAP; x1, y_1, x2, y2, color: cint); LibraryLibAllegroImp;
  procedure rectfill (bmp: PBITMAP; x1, y_1, x2, y2, color: cint); LibraryLibAllegroImp;
  function create_bitmap (width, height: cint): PBITMAP; LibraryLibAllegroImp;
  function create_system_bitmap (width, height: cint): PBITMAP; LibraryLibAllegroImp;
  procedure allegro_exit; LibraryLibAllegroImp;

  procedure rest (time: cuint); LibraryLibAllegroImp;
  function install_int_ex (proc: TimerIntCallback; speed: clong): cint; LibraryLibAllegroImp;
  procedure blit (source, dest: PBITMAP; source_x, source_y, dest_x, dest_y, width, height: cint); LibraryLibAllegroImp;
  procedure masked_blit (source, dest: PBITMAP; source_x, source_y, dest_x, dest_y, width, height: cint); LibraryLibAllegroImp;
  procedure set_clip_rect (bitmap: PBITMAP; x1, y1, x2, y2: cint); LibraryLibAllegroImp;
  procedure get_clip_rect (bitmap: PBITMAP; var x1, y1, x2, y2: cint); LibraryLibAllegroImp;

  procedure set_palette (const p: PALETTE); LibraryLibAllegroImp;
  procedure set_color_depth (depth: cint); LibraryLibAllegroImp;
  function set_close_button_callback (proc: QuitCallback): cint; LibraryLibAllegroImp;

//  function _install_allegro (system_id: cint; errno_prt: Pcint; AtExitFunction): cint; LibraryLibAllegroImp;

  procedure acquire_bitmap (bmp: PBITMAP); LibraryLibAllegroImp;
  procedure release_bitmap (bmp: PBITMAP); LibraryLibAllegroImp;
  procedure acquire_screen; LibraryLibAllegroImp;
  procedure release_screen; LibraryLibAllegroImp;

  procedure rotate_sprite (bmp, sprite: PBITMAP; x, y: cint; a: cint32); LibraryLibAllegroImp;
  procedure rotate_sprite_v_flip (bmp, sprite: PBITMAP; x, y: cint; a: cint32); LibraryLibAllegroImp;

  function scancode_to_ascii (scancode: cint): cint; LibraryLibAllegroImp;
  function scancode_to_name (scancode: cint): PChar; LibraryLibAllegroImp;

  (* MACRO *)
  function install_allegro (system_id: cint; errno_ptr: Pcint; atexit_ptr: AtExitFunction): cint; inline;
  function allegro_init: cint; inline;

  function TIMERS_PER_SECOND: clong; inline;
  function SECS_TO_TIMER (x: clong): clong; inline;
  function MSEC_TO_TIMER (x: clong): clong; inline;
  function BPS_TO_TIMER (x: clong): clong; inline;
  function BPM_TO_TIMER (x: clong): clong; inline;


implementation

  {$IF DEFINED(GO32V2)}
    function atexit (func: AtexitCallback): cint; cdecl; external;
  {$ELSEIF DEFINED(WINDOWS)}
    function atexit (func: AtexitCallback): cint; cdecl; external 'msvcrt.dll';
  {$ELSE}
    function atexit (func: AtexitCallback): cint; cdecl; external 'c';
  {$ENDIF}

  function install_allegro (system_id: cint; errno_ptr: Pcint; atexit_ptr: AtExitFunction): cint; inline;
  begin
    install_allegro := _install_allegro_version_check(system_id, errno_ptr, atexit_ptr, (ALLEGRO_VERSION shl 16) OR (ALEGRO_SUB_VERSION shl 8) OR ALLEGRO_WIP_VERSION)
  end;

  function allegro_init: cint; inline;
  begin
    (* original macros sets libc errno? *)
    allegro_init := _install_allegro_version_check(SYSTEM_AUTODETECT, nil, @atexit, (ALLEGRO_VERSION shl 16) OR (ALEGRO_SUB_VERSION shl 8) OR ALLEGRO_WIP_VERSION)
  end;

  function TIMERS_PER_SECOND: clong; inline;
  begin
    TIMERS_PER_SECOND := 1193181
  end;

  function SECS_TO_TIMER (x: clong): clong; inline;
  begin
    SECS_TO_TIMER := x * TIMERS_PER_SECOND
  end;

  function MSEC_TO_TIMER (x: clong): clong; inline;
  begin
    MSEC_TO_TIMER := x * TIMERS_PER_SECOND div 1000
  end;

  function BPS_TO_TIMER (x: clong): clong; inline;
  begin
    BPS_TO_TIMER := TIMERS_PER_SECOND div x
  end;

  function BPM_TO_TIMER (x: clong): clong; inline;
  begin
    BPM_TO_TIMER := 60 * TIMERS_PER_SECOND div x
  end;

(*
  function acquire_bitmap (bmp: PBITMAP); inline;
  begin
    ASSERT(bmp <> nil);
    if bmp.vtable.acquire <> nil then
      bmp.vtable.acquire(bmp)
  end;
*)

end.
