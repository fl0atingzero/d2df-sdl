{$INCLUDE ../shared/a_modes.inc}
{$SCOPEDENUMS OFF}

interface

  uses Allegro;

  {$I sdltype.inc}
  {$I sdlpixels.inc}
  {$I sdlscancode.inc}

  const
    SDL_HAT_CENTERED  = $00;
    SDL_HAT_UP        = $01;
    SDL_HAT_RIGHT     = $02;
    SDL_HAT_DOWN      = $04;
    SDL_HAT_LEFT      = $08;
    SDL_HAT_RIGHTUP   = SDL_HAT_RIGHT or SDL_HAT_UP;
    SDL_HAT_RIGHTDOWN = SDL_HAT_RIGHT or SDL_HAT_DOWN;
    SDL_HAT_LEFTUP    = SDL_HAT_LEFT or SDL_HAT_UP;
    SDL_HAT_LEFTDOWN  = SDL_HAT_LEFT or SDL_HAT_DOWN;

  const
    AUDIO_U8      = $0008;
    AUDIO_S8      = $8008;
    AUDIO_U16LSB  = $0010;
    AUDIO_S16LSB  = $8010;
    AUDIO_U16MSB  = $1010;
    AUDIO_S16MSB  = $9010;
    AUDIO_U16     = AUDIO_U16LSB;
    AUDIO_S16     = AUDIO_S16LSB;

  const
    SDL_TOUCH_MOUSEID = UInt32(-1);

  type
    PSDL_Window = ^TSDL_Window;
    TSDL_Window = record
      w, h, mode: Integer; (* Allegro-specific *)
    end;

    PSDL_Joystick = Pointer;
    TSDL_JoystickGUID = record
      data: array[0..15] of UInt8;
    end;
    TSDL_JoystickID = SInt32;

    PSDL_RWops = ^TSDL_RWops;

    TSize = function(context: PSDL_RWops): SInt64; cdecl;
    TSeek = function(context: PSDL_RWops; offset: SInt64; whence: SInt32): SInt64; cdecl;
    TRead = function(context: PSDL_RWops; ptr: Pointer; size: size_t; maxnum: size_t): size_t; cdecl;
    TWrite = function(context: PSDL_RWops; const ptr: Pointer; size: size_t; num: size_t): size_t; cdecl;
    TClose = function(context: PSDL_RWops): SInt32; cdecl;

    TStdio = record
      autoclose: TSDL_Bool;
      fp: file;
    end;

    TMem = record
      base: PUInt8;
      here: PUInt8;
      stop: PUInt8;
    end;

    TUnknown = record
      data1: Pointer;
    end;

    TSDL_RWops = packed record
      size: TSize;
      seek: TSeek;
      read: TRead;
      write: TWrite;
      close: TClose;
      _type: UInt32;
    case Integer of
      0: (stdio: TStdio);
      1: (mem: TMem);
      2: (unknown: TUnknown);
      {$IFDEF ANDROID}
      //3: (androidio: TAndroidIO);
      {$ENDIF}
      {$IFDEF WINDOWS}
      //3: (windowsio: TWindowsIO);
      {$ENDIF}
    end;

    PSDL_TouchID  = ^TSDL_TouchID;
    TSDL_TouchID  = SInt64;

    PSDL_FingerID = ^TSDL_FingerID;
    TSDL_FingerID = SInt64;


  const
    { Touch events }
    SDL_FINGERDOWN      = $700;
    SDL_FINGERUP        = $701;
    SDL_FINGERMOTION    = $702;

  type
    TSDL_TouchFingerEvent = record
      type_: UInt32;
      timestamp: UInt32;
      touchId: TSDL_TouchID;
      fingerId: TSDL_FingerID;
      x, y, dx, dy: Float;
      pressure: Float;
    end;


  const
    SDL_WINDOWPOS_CENTERED_MASK = $2FFF0000;
    SDL_WINDOWPOS_CENTERED = SDL_WINDOWPOS_CENTERED_MASK or 0;

  type
    PSDL_DisplayMode = ^TSDL_DisplayMode;
    TSDL_DisplayMode = record
      format: UInt32;
      w: SInt32;
      h: SInt32;
      refresh_rate: SInt32;
      driverdata: Pointer;
    end;

    TSDL_GLContext = Pointer;

  const
    SDL_WINDOW_FULLSCREEN = $00000001;
    SDL_WINDOW_OPENGL = $00000002;
    SDL_WINDOW_SHOWN = $00000004;
    SDL_WINDOW_HIDDEN = $00000008;
    SDL_WINDOW_BORDERLESS = $00000010;
    SDL_WINDOW_RESIZABLE = $00000020;
    SDL_WINDOW_MINIMIZED = $00000040;
    SDL_WINDOW_MAXIMIZED = $00000080;
    SDL_WINDOW_INPUT_GRABBED = $00000100;
    SDL_WINDOW_INPUT_FOCUS = $00000200;
    SDL_WINDOW_MOUSE_FOCUS = $00000400;
    SDL_WINDOW_FULLSCREEN_DESKTOP = SDL_WINDOW_FULLSCREEN or $00001000;
    SDL_WINDOW_FOREIGN = $00000800;
    SDL_WINDOW_ALLOW_HIGHDPI = $00002000;

    SDL_WINDOWEVENT_EXPOSED = 3;
    SDL_WINDOWEVENT_MOVED = 4;
    SDL_WINDOWEVENT_RESIZED = 5;
    SDL_WINDOWEVENT_MINIMIZED = 7;
    SDL_WINDOWEVENT_MAXIMIZED = 8;
    SDL_WINDOWEVENT_RESTORED = 9;
    SDL_WINDOWEVENT_FOCUS_GAINED = 12;
    SDL_WINDOWEVENT_FOCUS_LOST = 13;

    SDL_DISABLE = 0;
    SDL_ENABLE = 1;

    SDL_TEXTINPUTEVENT_TEXT_SIZE = 32;

    SDL_FIRSTEVENT       = 0;
    SDL_COMMONEVENT      = 1;
    SDL_QUITEV           = $100;
    SDL_WINDOWEVENT      = $200;
    SDL_KEYDOWN          = $300;
    SDL_KEYUP            = $301;
    //SDL_TEXTEDITING      = $302;
    SDL_TEXTINPUT        = $303;

  type
    TSDL_WindowEvent = record
      type_: UInt32;       // SDL_WINDOWEVENT
      timestamp: UInt32;
      windowID: UInt32;    // The associated window
      event: UInt8;        // SDL_WindowEventID
      padding1: UInt8;
      padding2: UInt8;
      padding3: UInt8;
      data1: SInt32;       // event dependent data
      data2: SInt32;       // event dependent data
    end;

    PSDL_Keysym = ^TSDL_Keysym;
    TSDL_Keysym = record
      scancode: TSDL_ScanCode;      // SDL physical key code - see SDL_Scancode for details
      sym: TSDL_KeyCode;            // SDL virtual key code - see SDL_Keycode for details
      _mod: UInt16;                 // current key modifiers
      unicode: UInt32;              // (deprecated) use SDL_TextInputEvent instead
    end;

    TSDL_KeyboardEvent = record
      type_: UInt32;        // SDL_KEYDOWN or SDL_KEYUP
      timestamp: UInt32;
      windowID: UInt32;     // The window with keyboard focus, if any
      state: UInt8;         // SDL_PRESSED or SDL_RELEASED
      _repeat: UInt8;       // Non-zero if this is a key repeat
      padding2: UInt8;
      padding3: UInt8;
      keysym: TSDL_KeySym;  // The key that was pressed or released
    end;

    TSDL_TextInputEvent = record
      type_: UInt32;                                          // SDL_TEXTINPUT
      timestamp: UInt32;
      windowID: UInt32;                                       // The window with keyboard focus, if any
      text: array[0..SDL_TEXTINPUTEVENT_TEXT_SIZE - 1] of Char;   // The input text
    end;

    TSDL_QuitEvent = record
      type_: UInt32;        // SDL_QUIT
      timestamp: UInt32;
    end;

    PSDL_Event = ^TSDL_Event;
    TSDL_Event = record
      case Integer of
      0:  (type_: UInt32);
      //SDL_COMMONEVENT:  (common: TSDL_CommonEvent);
      SDL_WINDOWEVENT:  (window: TSDL_WindowEvent);
      SDL_KEYUP,
      SDL_KEYDOWN:  (key: TSDL_KeyboardEvent);
      //SDL_TEXTEDITING:  (edit: TSDL_TextEditingEvent);
      SDL_TEXTINPUT:  (text: TSDL_TextInputEvent);
      //SDL_MOUSEMOTION:  (motion: TSDL_MouseMotionEvent);
      //SDL_MOUSEBUTTONUP,
      //SDL_MOUSEBUTTONDOWN:  (button: TSDL_MouseButtonEvent);
      //SDL_MOUSEWHEEL:  (wheel: TSDL_MouseWheelEvent);
      //SDL_JOYAXISMOTION:  (jaxis: TSDL_JoyAxisEvent);
      //SDL_JOYBALLMOTION: (jball: TSDL_JoyBallEvent);
      //SDL_JOYHATMOTION: (jhat: TSDL_JoyHatEvent);
      //SDL_JOYBUTTONDOWN,
      //SDL_JOYBUTTONUP: (jbutton: TSDL_JoyButtonEvent);
      //SDL_JOYDEVICEADDED,
      //SDL_JOYDEVICEREMOVED: (jdevice: TSDL_JoyDeviceEvent);
      //SDL_CONTROLLERAXISMOTION: (caxis: TSDL_ControllerAxisEvent);
      //SDL_CONTROLLERBUTTONUP,
      //SDL_CONTROLLERBUTTONDOWN: (cbutton: TSDL_ControllerButtonEvent);
      //SDL_CONTROLLERDEVICEADDED,
      //SDL_CONTROLLERDEVICEREMOVED,
      //SDL_CONTROLLERDEVICEREMAPPED: (cdevice: TSDL_ControllerDeviceEvent);
      //SDL_AUDIODEVICEADDED,
      //SDL_AUDIODEVICEREMOVED: (adevice: TSDL_AudioDeviceEvent);
      SDL_QUITEV: (quit: TSDL_QuitEvent);
      //SDL_USEREVENT: (user: TSDL_UserEvent);
      //SDL_SYSWMEVENT: (syswm: TSDL_SysWMEvent);
      SDL_FINGERDOWN,
      SDL_FINGERUP,
      SDL_FINGERMOTION: (tfinger: TSDL_TouchFingerEvent);
      //SDL_MULTIGESTURE: (mgesture: TSDL_MultiGestureEvent);
      //SDL_DOLLARGESTURE,SDL_DOLLARRECORD: (dgesture: TSDL_DollarGestureEvent);
      //SDL_DROPFILE: (drop: TSDL_DropEvent);
    end;

  const
    SDL_GL_RED_SIZE = 0;
    SDL_GL_GREEN_SIZE = 1;
    SDL_GL_BLUE_SIZE = 2;
    SDL_GL_ALPHA_SIZE = 3;
    SDL_GL_BUFFER_SIZE = 4;
    SDL_GL_DOUBLEBUFFER = 5;
    SDL_GL_DEPTH_SIZE = 6;
    SDL_GL_STENCIL_SIZE = 7;
    SDL_GL_ACCUM_RED_SIZE = 8;
    SDL_GL_ACCUM_GREEN_SIZE = 9;
    SDL_GL_ACCUM_BLUE_SIZE = 10;
    SDL_GL_ACCUM_ALPHA_SIZE = 11;
    SDL_GL_STEREO = 12;
    SDL_GL_MULTISAMPLEBUFFERS = 13;
    SDL_GL_MULTISAMPLESAMPLES = 14;
    SDL_GL_ACCELERATED_VISUAL = 15;
    SDL_GL_RETAINED_BACKING = 16;
    SDL_GL_CONTEXT_MAJOR_VERSION = 17;
    SDL_GL_CONTEXT_MINOR_VERSION = 18;
    SDL_GL_CONTEXT_EGL = 19;
    SDL_GL_CONTEXT_FLAGS = 20;
    SDL_GL_CONTEXT_PROFILE_MASK = 21;
    SDL_GL_SHARE_WITH_CURRENT_CONTEXT = 22;
    SDL_GL_FRAMEBUFFER_SRGB_CAPABLE = 23;

  type
    TSDL_GLattr = DWord;

  const
    SDL_INIT_TIMER          = $00000001;
    SDL_INIT_AUDIO          = $00000010;
    SDL_INIT_VIDEO          = $00000020;
    SDL_INIT_JOYSTICK       = $00000200;
    SDL_INIT_HAPTIC         = $00001000;
    SDL_INIT_GAMECONTROLLER = $00002000;  //turn on game controller also implicitly does JOYSTICK
    SDL_INIT_NOPARACHUTE    = $00100000;  //Don't catch fatal signals
    SDL_INIT_EVERYTHING     = SDL_INIT_TIMER or SDL_INIT_AUDIO or
                              SDL_INIT_VIDEO or SDL_INIT_JOYSTICK or
                              SDL_INIT_HAPTIC or SDL_INIT_GAMECONTROLLER;

    SDL_HINT_ACCELEROMETER_AS_JOYSTICK = 'SDL_ACCELEROMETER_AS_JOYSTICK';

  var
    sdl2allegro_screen: PBITMAP;
    sdl2allegro_bpp: Integer;
    globalTransTable: COLOR_MAP_T;
    redTransTable: COLOR_MAP_T;
    greenTransTable: COLOR_MAP_T;
    blueTransTable: COLOR_MAP_T;
    darkTransTable: COLOR_MAP_T;
    lightTransTable: COLOR_MAP_T;


  /// FUNCTIONS ///

  function SDL_SetWindowGammaRamp(window: PSDL_Window; const red: PUInt16; const green: PUInt16; const blue: PUInt16): SInt32;
  function SDL_GetWindowGammaRamp(window: PSDL_Window; red: PUInt16; green: PUInt16; blue: PUInt16): SInt32;

  function SDL_NumJoysticks: SInt32;
  function SDL_JoystickOpen(device_index: SInt32): PSDL_Joystick;
  function SDL_JoystickName(joystick: PSDL_Joystick): PAnsiChar;
  function SDL_JoystickNumAxes(joystick: PSDL_Joystick): SInt32;
  function SDL_JoystickNumButtons(joystick: PSDL_Joystick): SInt32;
  function SDL_JoystickNumHats(joystick: PSDL_Joystick): SInt32;
  procedure SDL_JoystickClose(joystick: PSDL_Joystick);
  procedure SDL_JoystickUpdate;
  function SDL_JoystickGetButton(joystick: PSDL_Joystick; button: SInt32): UInt8;
  function SDL_JoystickGetAxis(joystick: PSDL_Joystick; axis: SInt32): SInt16;
  function SDL_JoystickGetHat(joystick: PSDL_Joystick; hat: SInt32): UInt8;
  
  function SDL_GetScancodeName(scancode: TSDL_ScanCode): PAnsiChar;

  function SDL_GetPerformanceCounter: UInt64;
  function SDL_GetPerformanceFrequency: UInt64;
  
  function SDL_RWFromFile(const _file: PAnsiChar; const mode: PAnsiChar): PSDL_RWops;
  function SDL_RWFromConstMem(const mem: Pointer; size: SInt32): PSDL_RWops;

  function SDL_GetDisplayDPI(displayIndex: SInt32; ddpi, hdpi, vdpi: PFloat): SInt32;
  function SDL_IsTextInputActive: TSDL_Bool;
  procedure SDL_StartTextInput;
  procedure SDL_StopTextInput;
  function SDL_GetNumTouchDevices: SInt32;

  function SDL_GetDesktopDisplayMode(displayIndex: SInt32; mode: PSDL_DisplayMode): SInt32;
  function SDL_GetClosestDisplayMode(displayIndex: SInt32; const mode: PSDL_DisplayMode; closest: PSDL_DisplayMode): PSDL_DisplayMode;

  procedure SDL_DestroyWindow(window: PSDL_Window);
  procedure SDL_GL_DeleteContext(context: TSDL_GLContext);
  procedure SDL_RestoreWindow(window: PSDL_Window);
  procedure SDL_SetWindowSize(window: PSDL_Window; w: SInt32; h: SInt32);
  function SDL_CreateWindow(const title: PAnsiChar; x: SInt32; y: SInt32; w: SInt32; h: SInt32; flags: UInt32): PSDL_Window;
  function SDL_SetWindowFullscreen(window: PSDL_Window; flags: UInt32): SInt32;
  function SDL_GL_MakeCurrent(window: PSDL_Window; context: TSDL_GLContext): SInt32;
  procedure SDL_GetWindowSize(window: PSDL_Window; w: PInt; h: PInt);
  function SDL_GetNumDisplayModes(displayIndex: SInt32): SInt32;
  function SDL_GetDisplayMode(displayIndex: SInt32; modeIndex: SInt32; mode: PSDL_DisplayMode): SInt32;

  function SDL_ShowCursor(toggle: SInt32): SInt32;
  procedure SDL_Delay(ms: UInt32);
  procedure SDL_GL_SwapWindow(window: PSDL_Window);

  function SDL_GL_CreateContext(window: PSDL_Window): TSDL_GLContext;
  function SDL_GetTicks: UInt32;

  function SDL_PushEvent(event: PSDL_Event): SInt32;
  function SDL_PollEvent(event: PSDL_Event): SInt32;
  function SDL_GL_SetSwapInterval(interval: SInt32): SInt32;
  function SDL_GL_SetAttribute(attr: TSDL_GLattr; value: SInt32): SInt32;
  function SDL_GL_GetAttribute(attr: TSDL_GLattr; value: PInt): SInt32;

  function SDL_GetError: PAnsiChar;

  function SDL_Init(flags: UInt32): SInt32;
  procedure SDL_Quit();
  function SDL_SetHint( const name: PChar; const value: PChar) : boolean;

implementation

  uses
    {$IFDEF GO32V2}
      go32,
    {$ENDIF}
    {$IFDEF USE_WATT32}
      Watt32,
    {$ENDIF}
    e_Log, g_options, SysUtils, Math, Classes, ctypes;

  const
    maxKeyBuffer = 64;

  var
    deskw, deskh, deskbpp: Integer;
    keyring: array [0..maxKeyBuffer - 1] of Integer;
    keybeg, keyend: Integer;
    inputChar: Char;
    inputText: Boolean;
    useVsync: Boolean;
    ticks: UInt32;
    quit: Boolean;
    custompal: PALETTE;

    s2lc: array [0..KEY_MAX] of char = (
      #00, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
      'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4',
      '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', #00,
      #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, '`', '-', '=', #00,
      #09, '[', ']', #10, ';', #39, '\', '\', ',', '.', '/', ' ', #00, #00, #00, #00,
      #00, #00, #00, #00, #00, '/', '*', '-', '+', #00, #00, #00, #00, #00, #00, #00,
      #00, #00, #00, #00, '@', #00, ':', #00, '=', #00, ';', #00, #00, #00, #00, #00,
      #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00
    );
    s2uc: array [0..KEY_MAX] of char = (
      #00, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
      'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ')', '!', '@', '#', '$',
      '%', '^', '&', '*', '(', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', #00,
      #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, '~', '_', '+', #00,
      #09, '{', '}', #10, ':', '"', '|', '|', '<', '>', '?', ' ', #00, #00, #00, #00,
      #00, #00, #00, #00, #00, '/', '*', '-', '+', #00, #00, #00, #00, #00, #00, #00,
      #00, #00, #00, #00, '@', #00, ':', #00, '=', #00, ';', #00, #00, #00, #00, #00,
      #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00, #00
    );
    s2sa: array [0..KEY_MAX] of Integer = (
      SDL_SCANCODE_UNKNOWN,
      SDL_SCANCODE_A,
      SDL_SCANCODE_B,
      SDL_SCANCODE_C,
      SDL_SCANCODE_D,
      SDL_SCANCODE_E,
      SDL_SCANCODE_F,
      SDL_SCANCODE_G,
      SDL_SCANCODE_H,
      SDL_SCANCODE_I,
      SDL_SCANCODE_J,
      SDL_SCANCODE_K,
      SDL_SCANCODE_L,
      SDL_SCANCODE_M,
      SDL_SCANCODE_N,
      SDL_SCANCODE_O,
      SDL_SCANCODE_P,
      SDL_SCANCODE_Q,
      SDL_SCANCODE_R,
      SDL_SCANCODE_S,
      SDL_SCANCODE_T,
      SDL_SCANCODE_U,
      SDL_SCANCODE_V,
      SDL_SCANCODE_W,
      SDL_SCANCODE_X,
      SDL_SCANCODE_Y,
      SDL_SCANCODE_Z,
      SDL_SCANCODE_0,
      SDL_SCANCODE_1,
      SDL_SCANCODE_2,
      SDL_SCANCODE_3,
      SDL_SCANCODE_4,
      SDL_SCANCODE_5,
      SDL_SCANCODE_6,
      SDL_SCANCODE_7,
      SDL_SCANCODE_8,
      SDL_SCANCODE_9,
      SDL_SCANCODE_KP_0,
      SDL_SCANCODE_KP_1,
      SDL_SCANCODE_KP_2,
      SDL_SCANCODE_KP_3,
      SDL_SCANCODE_KP_4,
      SDL_SCANCODE_KP_5,
      SDL_SCANCODE_KP_6,
      SDL_SCANCODE_KP_7,
      SDL_SCANCODE_KP_8,
      SDL_SCANCODE_KP_9,
      SDL_SCANCODE_F1,
      SDL_SCANCODE_F2,
      SDL_SCANCODE_F3,
      SDL_SCANCODE_F4,
      SDL_SCANCODE_F5,
      SDL_SCANCODE_F6,
      SDL_SCANCODE_F7,
      SDL_SCANCODE_F8,
      SDL_SCANCODE_F9,
      SDL_SCANCODE_F10,
      SDL_SCANCODE_F11,
      SDL_SCANCODE_F12,
      SDL_SCANCODE_ESCAPE,
      SDL_SCANCODE_GRAVE,
      SDL_SCANCODE_MINUS,
      SDL_SCANCODE_EQUALS,
      SDL_SCANCODE_BACKSPACE,
      SDL_SCANCODE_TAB,
      SDL_SCANCODE_LEFTBRACKET,
      SDL_SCANCODE_RIGHTBRACKET,
      SDL_SCANCODE_RETURN,
      SDL_SCANCODE_SEMICOLON,
      SDL_SCANCODE_APOSTROPHE,
      SDL_SCANCODE_BACKSLASH,
      SDL_SCANCODE_NONUSHASH,
      SDL_SCANCODE_COMMA,
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_STOP *)
      SDL_SCANCODE_SLASH,
      SDL_SCANCODE_SPACE,
      SDL_SCANCODE_INSERT,
      SDL_SCANCODE_DELETE,
      SDL_SCANCODE_HOME,
      SDL_SCANCODE_END,
      SDL_SCANCODE_PAGEUP,
      SDL_SCANCODE_PAGEDOWN,
      SDL_SCANCODE_LEFT,
      SDL_SCANCODE_RIGHT,
      SDL_SCANCODE_UP,
      SDL_SCANCODE_DOWN,
      SDL_SCANCODE_KP_DIVIDE,
      SDL_SCANCODE_KP_MULTIPLY,
      SDL_SCANCODE_KP_MINUS,
      SDL_SCANCODE_KP_PLUS,
      SDL_SCANCODE_KP_PERIOD,
      SDL_SCANCODE_KP_ENTER,
      SDL_SCANCODE_PRINTSCREEN,
      SDL_SCANCODE_PAUSE,
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_ABNT_C1 *)
      SDL_SCANCODE_INTERNATIONAL3, (* ??? KEY_YEN *)
      SDL_SCANCODE_LANG3, (* ??? KEY_KANA *)
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_CONVERT *)
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_NOCONVERT *)
      SDL_SCANCODE_KP_AT,
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_CIRCUMFLEX *)
      SDL_SCANCODE_KP_COLON,
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_KANJI *)
      SDL_SCANCODE_KP_EQUALS,
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_BACKQUOTE *)
      SDL_SCANCODE_SEMICOLON,
      SDL_SCANCODE_LGUI,
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_UNKNOWN1 *)
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_UNKNOWN2 *)
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_UNKNOWN3 *)
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_UNKNOWN4 *)
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_UNKNOWN5 *)
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_UNKNOWN6 *)
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_UNKNOWN7 *)
      SDL_SCANCODE_UNKNOWN, (* !!! KEY_UNKNOWN8 *)
      SDL_SCANCODE_LSHIFT,
      SDL_SCANCODE_RSHIFT,
      SDL_SCANCODE_LCTRL,
      SDL_SCANCODE_RCTRL,
      SDL_SCANCODE_LALT,
      SDL_SCANCODE_RALT,
      SDL_SCANCODE_LGUI,
      SDL_SCANCODE_RGUI,
      SDL_SCANCODE_MODE, (* ??? KEY_MENU *)
      SDL_SCANCODE_SCROLLLOCK,
      SDL_SCANCODE_NUMLOCKCLEAR,
      SDL_SCANCODE_CAPSLOCK,
      SDL_SCANCODE_UNKNOWN (* KEY_MAX *)
    );

  procedure LoadCustomPalette (const name: String);
    var f: TFileStream; i: Integer;
  begin
    try
      f := TFileStream.Create(name, fmOpenRead);
      e_LogWriteLn('Load custom palette ' + name);
      for i := 0 to 255 do
      begin
        custompal[i].r := f.ReadByte();
        custompal[i].g := f.ReadByte();
        custompal[i].b := f.ReadByte();
        custompal[i].filler := $FF;
      end;
      f.Destroy
    except
      e_LogWriteLn('Fallback to default palette');
      custompal := default_palette;
(*
      for i := 0 to 255 do
      begin
        custompal[i].r := i div 4;
        custompal[i].g := i div 4;
        custompal[i].b := i div 4;
        custompal[i].filler := $FF;
      end;
*)
    end
  end;

  procedure UpdatePalette;
  begin
    set_palette(custompal);
    select_palette(custompal);
    create_trans_table(@globalTransTable, custompal, 255, 255, 255, nil);
    create_trans_table(@redTransTable, custompal, 0, 255, 255, nil);
    create_trans_table(@greenTransTable, custompal, 255, 0, 255, nil);
    create_trans_table(@blueTransTable, custompal, 255, 255, 0, nil);
    create_trans_table(@darkTransTable, custompal, 191, 191, 191, nil);
    create_trans_table(@lightTransTable, custompal, 64, 64, 64, nil);
    color_map := @globalTransTable;
  end;

  function IsEmptyKeyboard: Boolean;
  begin
    result := keybeg = keyend
  end;

  function IsFullKeyboard: Boolean;
  begin
    result := (keybeg - 1) mod maxKeyBuffer = keyend
  end;

  function NextScancode: Integer;
  begin
    result := 0;
    if not IsEmptyKeyboard then
    begin
      result := keyring[keybeg];
      keybeg := (keybeg + 1) mod maxKeyBuffer
    end
  end;

  procedure KeyboardWatcher (scancode: Integer); cdecl;
  begin
    if (keybeg - 1) mod maxKeyBuffer <> keyend then (* not IsFullKeyboard *)
    begin
      keyring[keyend] := scancode;
      keyend := (keyend + 1) mod maxKeyBuffer
    end
  end;
  procedure KeyboardWatcherEND; begin end;

  /// MACRO ///

  function SDL_PIXELFLAG(X: Cardinal): Cardinal;
  begin
    Result := (X shr 28) and $0F;
  end;

  function SDL_PIXELTYPE(X: Cardinal): Cardinal;
  begin
    Result := (X shr 24) and $0F;
  end;

  function SDL_PIXELORDER(X: Cardinal): Cardinal;
  begin
    Result := (X shr 20) and $0F;
  end;

  function SDL_PIXELLAYOUT(X: Cardinal): Cardinal;
  begin
    Result := (X shr 16) and $0F;
  end;

  function SDL_BITSPERPIXEL(X: Cardinal): Cardinal;
  begin
    Result := (X shr 8) and $FF;
  end;

  function SDL_IsPixelFormat_FOURCC(format: Variant): Boolean;
  begin
    Result := format and SDL_PIXELFLAG(format) <> 1;
  end;

  /// FUNCTIONS ////

(********** FILES **********)

  function SDL_RWFromFile(const _file: PAnsiChar; const mode: PAnsiChar): PSDL_RWops;
  begin
    result := nil (* stub, used for sdl2stub_mixer *)
  end;

  function SDL_RWFromConstMem(const mem: Pointer; size: SInt32): PSDL_RWops;
  begin
    result := nil (* stub, used for sdl2stub_mixer *)
  end;

(********** KEYBOARD **********)

  function SDL_GetScancodeName(scancode: TSDL_ScanCode): PAnsiChar;
  begin
    case scancode of
      SDL_SCANCODE_A: result := 'A';
      SDL_SCANCODE_B: result := 'B';
      SDL_SCANCODE_C: result := 'C';
      SDL_SCANCODE_D: result := 'D';
      SDL_SCANCODE_E: result := 'E';
      SDL_SCANCODE_F: result := 'F';
      SDL_SCANCODE_G: result := 'G';
      SDL_SCANCODE_H: result := 'H';
      SDL_SCANCODE_I: result := 'I';
      SDL_SCANCODE_J: result := 'J';
      SDL_SCANCODE_K: result := 'K';
      SDL_SCANCODE_L: result := 'L';
      SDL_SCANCODE_M: result := 'M';
      SDL_SCANCODE_N: result := 'N';
      SDL_SCANCODE_O: result := 'O';
      SDL_SCANCODE_P: result := 'P';
      SDL_SCANCODE_Q: result := 'Q';
      SDL_SCANCODE_R: result := 'R';
      SDL_SCANCODE_S: result := 'S';
      SDL_SCANCODE_T: result := 'T';
      SDL_SCANCODE_U: result := 'U';
      SDL_SCANCODE_V: result := 'V';
      SDL_SCANCODE_W: result := 'W';
      SDL_SCANCODE_X: result := 'X';
      SDL_SCANCODE_Y: result := 'Y';
      SDL_SCANCODE_Z: result := 'Z';
      SDL_SCANCODE_0: result := '0';
      SDL_SCANCODE_1: result := '1';
      SDL_SCANCODE_2: result := '2';
      SDL_SCANCODE_3: result := '3';
      SDL_SCANCODE_4: result := '4';
      SDL_SCANCODE_5: result := '5';
      SDL_SCANCODE_6: result := '6';
      SDL_SCANCODE_7: result := '7';
      SDL_SCANCODE_8: result := '8';
      SDL_SCANCODE_9: result := '9';
      SDL_SCANCODE_RETURN: result := 'Return';
      SDL_SCANCODE_ESCAPE: result := 'Escape';
      SDL_SCANCODE_BACKSPACE: result := 'Backspace';
      SDL_SCANCODE_TAB: result := 'Tab';
      SDL_SCANCODE_SPACE: result := 'Space';
      SDL_SCANCODE_MINUS: result := '-';
      SDL_SCANCODE_EQUALS: result := '=';
      SDL_SCANCODE_LEFTBRACKET: result := '[';
      SDL_SCANCODE_RIGHTBRACKET: result := ']';
      SDL_SCANCODE_BACKSLASH: result := '\';
      SDL_SCANCODE_SEMICOLON: result := ';';
      SDL_SCANCODE_APOSTROPHE: result := '''';
      SDL_SCANCODE_GRAVE: result := '`';
      SDL_SCANCODE_COMMA: result := ',';
      SDL_SCANCODE_PERIOD: result := '.';
      SDL_SCANCODE_SLASH: result := '/';
      SDL_SCANCODE_CAPSLOCK: result := 'CapsLock';
      SDL_SCANCODE_F1: result := 'F1';
      SDL_SCANCODE_F2: result := 'F2';
      SDL_SCANCODE_F3: result := 'F3';
      SDL_SCANCODE_F4: result := 'F4';
      SDL_SCANCODE_F5: result := 'F5';
      SDL_SCANCODE_F6: result := 'F6';
      SDL_SCANCODE_F7: result := 'F7';
      SDL_SCANCODE_F8: result := 'F8';
      SDL_SCANCODE_F9: result := 'F9';
      SDL_SCANCODE_F10: result := 'F10';
      SDL_SCANCODE_F11: result := 'F11';
      SDL_SCANCODE_F12: result := 'F12';
      SDL_SCANCODE_PRINTSCREEN: result := 'PrintScreen';
      SDL_SCANCODE_SCROLLLOCK: result := 'ScrollLock';
      SDL_SCANCODE_PAUSE: result := 'Pause';
      SDL_SCANCODE_INSERT: result := 'Insert';
      SDL_SCANCODE_HOME: result := 'Home';
      SDL_SCANCODE_PAGEUP: result := 'PageUp';
      SDL_SCANCODE_DELETE: result := 'Delete';
      SDL_SCANCODE_END: result := 'End';
      SDL_SCANCODE_PAGEDOWN: result := 'PageDown';
      SDL_SCANCODE_RIGHT: result := 'Right';
      SDL_SCANCODE_LEFT: result := 'Left';
      SDL_SCANCODE_DOWN: result := 'Down';
      SDL_SCANCODE_UP: result := 'Up';
      SDL_SCANCODE_NUMLOCKCLEAR: result := 'Numlock';
      SDL_SCANCODE_KP_DIVIDE: result := 'Keypad /';
      SDL_SCANCODE_KP_MULTIPLY: result := 'Keypad *';
      SDL_SCANCODE_KP_MINUS: result := 'Keypad -';
      SDL_SCANCODE_KP_PLUS: result := 'Keypad +';
      SDL_SCANCODE_KP_ENTER: result := 'Keypad Enter';
      SDL_SCANCODE_KP_1: result := 'Keypad 1';
      SDL_SCANCODE_KP_2: result := 'Keypad 2';
      SDL_SCANCODE_KP_3: result := 'Keypad 3';
      SDL_SCANCODE_KP_4: result := 'Keypad 4';
      SDL_SCANCODE_KP_5: result := 'Keypad 5';
      SDL_SCANCODE_KP_6: result := 'Keypad 6';
      SDL_SCANCODE_KP_7: result := 'Keypad 7';
      SDL_SCANCODE_KP_8: result := 'Keypad 8';
      SDL_SCANCODE_KP_9: result := 'Keypad 9';
      SDL_SCANCODE_KP_0: result := 'Keypad 0';
      SDL_SCANCODE_KP_PERIOD: result := 'Keypad .';
      SDL_SCANCODE_APPLICATION: result := 'Application';
      SDL_SCANCODE_POWER: result := 'Power';
      SDL_SCANCODE_KP_EQUALS: result := 'Keypad =';
      (* !!! F1x/F2x and multimedia ... *)
      SDL_SCANCODE_KP_COMMA: result := 'Keypad ,';
      SDL_SCANCODE_KP_EQUALSAS400: result := 'Keypad = (AS400)';
      SDL_SCANCODE_ALTERASE: result := 'AltErase';
      SDL_SCANCODE_SYSREQ: result := 'SysReq';
      SDL_SCANCODE_CANCEL: result := 'Cancel';
      SDL_SCANCODE_CLEAR: result := 'Clear';
      SDL_SCANCODE_PRIOR: result := 'Prior';
      SDL_SCANCODE_RETURN2: result := 'Return';
      SDL_SCANCODE_SEPARATOR: result := 'Separator';
      SDL_SCANCODE_OUT: result := 'Out';
      SDL_SCANCODE_OPER: result := 'Oper';
      SDL_SCANCODE_CLEARAGAIN: result := 'Clear / Again';
      SDL_SCANCODE_CRSEL: result := 'CrSel';
      SDL_SCANCODE_EXSEL: result := 'ExSel';
      (* !!! Additional KP *)
      SDL_SCANCODE_LCTRL: result := 'Left Ctrl';
      SDL_SCANCODE_LSHIFT: result := 'Left Shift';
      SDL_SCANCODE_LALT: result := 'Left Alt';
      SDL_SCANCODE_LGUI: result := 'Left GUI';
      SDL_SCANCODE_RCTRL: result := 'Right Ctrl';
      SDL_SCANCODE_RSHIFT: result := 'Right Shift';
      SDL_SCANCODE_RALT: result := 'Right Alt';
      SDL_SCANCODE_RGUI: result := 'Right GUI';
      SDL_SCANCODE_MODE: result := 'ModeSwitch';
      (* !!! ... *)
    else
      result := ''
    end
  end;

  function SDL_IsTextInputActive: TSDL_Bool;
  begin
    if inputText then
      result := SDL_TRUE
    else
      result := SDL_FALSE
  end;

  procedure SDL_StartTextInput;
  begin
    inputText := True
  end;

  procedure SDL_StopTextInput;
  begin
    inputText := False
  end;

(********** JOYSTICK **********)

  function SDL_NumJoysticks: SInt32;
  begin
    result := 0
  end;

  function SDL_JoystickOpen(device_index: SInt32): PSDL_Joystick;
  begin
    result := nil
  end;

  function SDL_JoystickName(joystick: PSDL_Joystick): PAnsiChar;
  begin
    result := ''
  end;

  function SDL_JoystickNumAxes(joystick: PSDL_Joystick): SInt32;
  begin
    result := 0
  end;

  function SDL_JoystickNumButtons(joystick: PSDL_Joystick): SInt32;
  begin
    result := 0
  end;

  function SDL_JoystickNumHats(joystick: PSDL_Joystick): SInt32;
  begin
    result := 0
  end;

  procedure SDL_JoystickClose(joystick: PSDL_Joystick);
  begin
  end;

  procedure SDL_JoystickUpdate;
  begin
  end;

  function SDL_JoystickGetButton(joystick: PSDL_Joystick; button: SInt32): UInt8;
  begin
    result := 0
  end;

  function SDL_JoystickGetAxis(joystick: PSDL_Joystick; axis: SInt32): SInt16;
  begin
    result := 0
  end;

  function SDL_JoystickGetHat(joystick: PSDL_Joystick; hat: SInt32): UInt8;
  begin
    result := 0
  end;  

(********** TOUCH **********)

  function SDL_GetNumTouchDevices: SInt32;
  begin
    result := 0
  end;

(********** TIMERS **********)

  procedure AllegroTimerCallback; cdecl;
  begin
    inc(ticks);
    {$IFDEF USE_WATT32}
      userTimerTick(1);
    {$ENDIF}
  end;
  procedure AllegroTimerCallbackEND; begin end;

  function SDL_GetPerformanceCounter: UInt64;
  begin
    result := ticks (* !!! *)
  end;

  function SDL_GetPerformanceFrequency: UInt64;
  begin
    result := 1 (* !!! *)
  end;

  procedure SDL_Delay(ms: UInt32);
  begin
    rest(ms)
  end;

  function SDL_GetTicks: UInt32;
  begin
    result := ticks
  end;

(********** DISPLAY MODES **********)

  function GetPixelFormat (bpp: Integer): UInt32;
  begin
    case bpp of
    8: result := SDL_PIXELFORMAT_INDEX8;
    15: result := SDL_PIXELFORMAT_RGB555;
    16: result := SDL_PIXELFORMAT_RGB565;
    24: result := SDL_PIXELFORMAT_RGB888;
    32: result := SDL_PIXELFORMAT_RGBA8888;
    else result := SDL_PIXELFORMAT_UNKNOWN
    end
  end;

  function SDL_GetDesktopDisplayMode(displayIndex: SInt32; mode: PSDL_DisplayMode): SInt32;
  begin
    ASSERT(mode <> nil);
    mode.format := GetPixelFormat(deskbpp);
    mode.w := deskw;
    mode.h := deskh;
    mode.refresh_rate := 0;
    mode.driverdata := nil;
    result := 0
  end;

  function SDL_GetClosestDisplayMode(displayIndex: SInt32; const mode: PSDL_DisplayMode; closest: PSDL_DisplayMode): PSDL_DisplayMode;
    var m: PGFX_MODE_LIST; i, bpp: Integer;
  begin
    ASSERT(mode <> nil);
    ASSERT(closest <> nil);

    result := nil;
    bpp := SDL_BITSPERPIXEL(mode.format);
    if bpp = 0 then bpp := deskbpp;

    m := nil;
    if (gfx_driver <> nil) and (@gfx_driver.fetch_mode_list <> nil) then
       m := gfx_driver.fetch_mode_list;

    if m <> nil then
    begin
      i := 0;
      while (i < m.num_modes) and (m.mode[i].bpp <> bpp) do inc(i);
      if i < m.num_modes then
      begin
        closest.format := mode.format;
        closest.refresh_rate := 0;
        closest.driverdata := nil;
        closest.w := m.mode[i].width;
        closest.h := m.mode[i].height;
        result := closest
      end;
      while i < m.num_modes do
      begin
        if (m.mode[i].bpp = bpp) and (closest.w >= mode.w) and (m.mode[i].width <= closest.w) and (closest.h >= mode.h) and (m.mode[i].height <= closest.h) then
        begin
          closest.w := m.mode[i].width;
          closest.h := m.mode[i].height;
          result := closest
        end;
        inc(i)
      end;
      destroy_gfx_mode_list(m)
    end
  end;

  function SDL_GetNumDisplayModes(displayIndex: SInt32): SInt32;
    var m: PGFX_MODE_LIST;
  begin
    result := -1;

    m := nil;
    if (gfx_driver <> nil) and (@gfx_driver.fetch_mode_list <> nil) then
       m := gfx_driver.fetch_mode_list;

    if m <> nil then
    begin
      result := m.num_modes;
      destroy_gfx_mode_list(m);
    end
  end;

  function SDL_GetDisplayMode(displayIndex: SInt32; modeIndex: SInt32; mode: PSDL_DisplayMode): SInt32;
    var m: PGFX_MODE_LIST;
  begin
    result := -1;

    m := nil;
    if (gfx_driver <> nil) and (@gfx_driver.fetch_mode_list <> nil) then
       m := gfx_driver.fetch_mode_list;

    if m <> nil then
    begin
      if (modeIndex >= 0) and (modeIndex < m.num_modes) then
      begin
        mode.format := GetPixelFormat(m.mode[modeIndex].bpp);
        mode.w := m.mode[modeIndex].width;
        mode.h := m.mode[modeIndex].height;
        mode.refresh_rate := 0;
        mode.driverdata := nil;
        result := 0
      end;
      destroy_gfx_mode_list(m)
    end
  end;

  function SDL_GetDisplayDPI(displayIndex: SInt32; ddpi, hdpi, vdpi: PFloat): SInt32;
  begin
    result := -1 (* stub *)
  end;

(*********** WINDOW MANAGEMENT **********)

  function SDL_CreateWindow(const title: PAnsiChar; x: SInt32; y: SInt32; w: SInt32; h: SInt32; flags: UInt32): PSDL_Window;
    var window: PSDL_Window; mode: Integer;
  begin
    result := nil;

{$IF DEFINED(GO32V2)}
    mode := GFX_AUTODETECT;
{$ELSE}
    if (flags and (SDL_WINDOW_FULLSCREEN or SDL_WINDOW_FULLSCREEN_DESKTOP)) <> 0 then
      mode := GFX_AUTODETECT_FULLSCREEN
    else
      mode := GFX_AUTODETECT_WINDOWED;
{$ENDIF}

    if sdl2allegro_bpp = 0 then sdl2allegro_bpp := gBPP;
    if sdl2allegro_bpp = 0 then sdl2allegro_bpp := deskbpp;

    set_color_depth(sdl2allegro_bpp);
    if set_gfx_mode(mode, w, h, 0, 0) = 0 then
    begin
      new(window);
      set_window_title(title);
      if sdl2allegro_screen <> nil then
        destroy_bitmap(sdl2allegro_screen);
      sdl2allegro_screen := create_system_bitmap(w, h);
      if sdl2allegro_screen = nil then
        sdl2allegro_screen := create_bitmap(w, h);
      ASSERT(sdl2allegro_screen <> nil);
      window.w := w;
      window.h := h;
      window.mode := mode;
      UpdatePalette;
      result := window
    end
  end;

  function SDL_SetWindowFullscreen(window: PSDL_Window; flags: UInt32): SInt32;
    var mode: Integer;
  begin
    ASSERT(window <> nil);
    result := -1;

{$IF DEFINED(GO32V2)}
    mode := GFX_AUTODETECT;
{$ELSE}
    if (flags and (SDL_WINDOW_FULLSCREEN or SDL_WINDOW_FULLSCREEN_DESKTOP)) <> 0 then
      mode := GFX_AUTODETECT_FULLSCREEN
    else
      mode := GFX_AUTODETECT_WINDOWED;
{$ENDIF}

    set_color_depth(sdl2allegro_bpp);
    if set_gfx_mode(mode, window.w, window.h, 0, 0) = 0 then
    begin
     if sdl2allegro_screen <> nil then
        destroy_bitmap(sdl2allegro_screen);
      sdl2allegro_screen := create_system_bitmap(window.w, window.h);
      if sdl2allegro_screen = nil then
        sdl2allegro_screen := create_bitmap(window.w, window.h);
      ASSERT(sdl2allegro_screen <> nil);
      UpdatePalette;
      window.mode := mode;
      result := 0
    end
  end;

  procedure SDL_SetWindowSize(window: PSDL_Window; w: SInt32; h: SInt32);
  begin
    ASSERT(window <> nil);
    set_color_depth(sdl2allegro_bpp);
    if set_gfx_mode(window.mode, w, h, 0, 0) = 0 then
    begin
      if sdl2allegro_screen <> nil then
        destroy_bitmap(sdl2allegro_screen);
      sdl2allegro_screen := create_system_bitmap(w, h);
      if sdl2allegro_screen = nil then
        sdl2allegro_screen := create_bitmap(w, h);
      ASSERT(sdl2allegro_screen <> nil);
      UpdatePalette;
      window.w := w;
      window.h := h
    end
    else
    begin
      ASSERT(FALSE, 'Unable to set window size')
    end
  end;

  procedure SDL_DestroyWindow(window: PSDL_Window);
  begin
    if window = nil then exit;
    if sdl2allegro_screen <> nil then
      destroy_bitmap(sdl2allegro_screen);
    sdl2allegro_screen := nil;
    dispose(window)
  end;

  procedure SDL_GetWindowSize(window: PSDL_Window; w: PInt; h: PInt);
  begin
    ASSERT(window <> nil);
    if w <> nil then w^ := window.w;
    if h <> nil then h^ := window.h;
  end;

  procedure SDL_RestoreWindow(window: PSDL_Window);
  begin
    ASSERT(window <> nil);
    (* stub *)
  end;

  function SDL_SetWindowGammaRamp(window: PSDL_Window; const red: PUInt16; const green: PUInt16; const blue: PUInt16): SInt32;
  begin
    ASSERT(window <> nil);
    result := 0 (* stub *)
  end;

  function SDL_GetWindowGammaRamp(window: PSDL_Window; red: PUInt16; green: PUInt16; blue: PUInt16): SInt32;
  begin
    ASSERT(window <> nil);
    result := 0 (* stub *)
  end;

(********** OPENGL MANAGEMENT **********)

  function SDL_GL_SetAttribute(attr: TSDL_GLattr; value: SInt32): SInt32;
  begin
    e_LogWritefln('SDL_GL_SetAttribute %s %s', [attr, value]);
    allegro_error := 'Attribute ' + IntToStr(attr) + ' := ' + IntToStr(value) + 'not supported';
    result := -1
  end;

  function SDL_GL_GetAttribute(attr: TSDL_GLattr; value: PInt): SInt32;
  begin
    e_LogWritefln('SDL_GL_GetAttribute %s', [attr]);
    value^ := 0; result := -1
  end;

  function SDL_GL_CreateContext(window: PSDL_Window): TSDL_GLContext;
  begin
    result := window (* stub *)
  end;

  function SDL_GL_MakeCurrent(window: PSDL_Window; context: TSDL_GLContext): SInt32;
  begin
    ASSERT(window <> nil);
    if context <> nil then
      ASSERT(window = context);
    result := 0 (* stub *)
  end;

  procedure SDL_GL_DeleteContext(context: TSDL_GLContext);
  begin
    (* stub *)
  end;

  function SDL_GL_SetSwapInterval(interval: SInt32): SInt32;
  begin
    result := 0;
    if interval = 0 then
      useVsync := False
    else
      useVsync := True
  end;

  procedure SDL_GL_SwapWindow(window: PSDL_Window);
  begin
    ASSERT(window <> nil);
    ASSERT(sdl2allegro_screen <> nil);
    acquire_screen;
    blit(sdl2allegro_screen, screen, 0, 0, 0, 0, min(sdl2allegro_screen.w, screen.w), min(sdl2allegro_screen.h, screen.h));
    show_video_bitmap(screen);
    release_screen;
    if useVsync then
      vsync;

    {$IFDEF USE_WATT32}
      tcp_tick(nil);
    {$ENDIF}
  end;

(********** EVENTS **********)

  function SDL_PushEvent(event: PSDL_Event): SInt32;
  begin
    result := 1;
    case event.type_ of
      SDL_QUITEV: quit := True;
    else
      ASSERT(FALSE);
      result := 0
    end
  end;

  function SDL_PollEvent(event: PSDL_Event): SInt32;
    var alscan: Integer; pressed, shift, caps: Boolean;
  begin
    result := 0;

    if quit then
    begin
      quit := False;
      event.type_ := SDL_QUITEV;
      event.quit.timestamp := ticks;
      result := 1;
      Exit
    end;

    if inputText and (inputChar <> #0) then
    begin
      event.type_ := SDL_TEXTINPUT;
      event.text.timestamp := ticks;
      event.text.windowID := 0;
      event.text.text[0] := inputChar;
      event.text.text[1] := #0;
      inputChar := #0;
      result := 1;
      Exit
    end
    else
      inputChar := #0;

    poll_keyboard;
    if not IsEmptyKeyboard then
    begin
      alscan := NextScancode;
      pressed := alscan and $80 = 0;

      inputChar := #0;
      if pressed then
      begin
        shift := key_shifts and KB_SHIFT_FLAG <> 0;
        caps := key_shifts and KB_CAPSLOCK_FLAG <> 0;
        if shift xor caps then
          inputChar := s2uc[alscan and $7F]
        else
          inputChar := s2lc[alscan and $7F];
        event.type_ := SDL_KEYDOWN
      end
      else
        event.type_ := SDL_KEYUP;

      event.key.timestamp := ticks;
      event.key.windowID := 0;

      (**** df not use it?
      if pressed then
        event.key.state := SDL_PRESSED
      else
        event.key.state := SDL_RELEASED;
      ****)

      event.key._repeat := 0;
      event.key.keysym.scancode := s2sa[alscan and $7F];
      event.key.keysym.sym := 0; (* df not use it? *)
      event.key.keysym._mod := 0; (* df not use it? *)
      result := 1;
      Exit
    end;

    {$IFDEF USE_WATT32}
      tcp_tick(nil);
    {$ENDIF}
  end;

(********** MOUSE **********)

  function SDL_ShowCursor(toggle: SInt32): SInt32;
  begin
    result := 0 (* stub *)
  end;

(********** SDL **********)

  procedure AllegroQuitCallback; cdecl;
  begin
    quit := True
  end;
  procedure AllegroQuitCallbackEND; begin end;

  function SDL_SetHint(const name: PChar; const value: PChar): boolean;
  begin
    e_LogWritefln('SDL_SetHint %s %s', [name, value]);
    result := false (* stub *)
  end;

  function SDL_GetError: PAnsiChar;
  begin
    result := allegro_error
  end;

{$IFDEF GO32V2}
  (* HACK: allegro crashes while init without this *)
  var
    __crt0_argv: PPchar; cvar; external;
    myargv: array [0..1] of Pchar;

  procedure FIX_ARGV;
  begin
    __crt0_argv := @myargv[0];
    myargv[0] := PChar(ParamStr(0));
    e_LogWritefln('argv[0] = %s', [myargv[0]]);
  end;
{$ENDIF}

  function SDL_Init(flags: UInt32): SInt32;
  begin
    result := -1;
    {$IFDEF GO32V2}
      FIX_ARGV;
    {$ENDIF}
    if allegro_init = 0 then
    begin
      e_LogWriteln('Allegro Init: ok');
      {$IFDEF GO32V2}
        (* without this df dies with fire when swapped *)
        lock_data(ticks, sizeof(ticks));
        lock_code(@AllegroTimerCallback, PtrUInt(@AllegroTimerCallbackEND) - PtrUInt(@AllegroTimerCallback));
        lock_data(keybeg, sizeof(keybeg));
        lock_data(keyend, sizeof(keyend));
        lock_data(keyring, sizeof(keyring));
        lock_code(@KeyboardWatcher, PtrUInt(@KeyboardWatcherEND) - PtrUInt(@KeyboardWatcher));
        lock_data(quit, sizeof(quit));
        lock_code(@AllegroQuitCallback, PtrUInt(@AllegroQuitCallbackEND) - PtrUInt(@AllegroQuitCallback));
      {$ENDIF}
      install_timer;
      install_keyboard;
      keyboard_lowlevel_callback := KeyboardWatcher;
      set_keyboard_rate(0, 0);
      install_int_ex(AllegroTimerCallback, MSEC_TO_TIMER(1));
      set_close_button_callback(AllegroQuitCallback);
      deskbpp := desktop_color_depth;
      if deskbpp = 0 then deskbpp := 8;
      if get_desktop_resolution(@deskw, @deskh) <> 0 then
      begin
        deskw := 640;
        deskh := 480
      end;
      LoadCustomPalette('PLAYPAL.LMP');
      result := 0
    end
  end;

  procedure SDL_Quit;
  begin
    set_close_button_callback(nil);
    remove_keyboard;
    remove_timer;
    {$IFDEF GO32V2}
      unlock_data(ticks, sizeof(ticks));
      unlock_code(@AllegroTimerCallback, PtrUInt(@AllegroTimerCallbackEND) - PtrUInt(@AllegroTimerCallback));
      unlock_data(keybeg, sizeof(keybeg));
      unlock_data(keyend, sizeof(keyend));
      unlock_data(keyring, sizeof(keyring));
      unlock_code(@KeyboardWatcher, PtrUInt(@KeyboardWatcherEND) - PtrUInt(@KeyboardWatcher));
      unlock_data(quit, sizeof(quit));
      unlock_code(@AllegroQuitCallback, PtrUInt(@AllegroQuitCallbackEND) - PtrUInt(@AllegroQuitCallback));
    {$ENDIF}
    allegro_exit
  end;
