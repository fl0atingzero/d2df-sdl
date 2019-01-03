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
      3: (androidio: TAndroidIO);
      {$ENDIF}
      {$IFDEF WINDOWS}
      3: (windowsio: TWindowsIO);
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
      text: array[0..SDL_TEXTINPUTEVENT_TEXT_SIZE] of Char;   // The input text
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

  uses e_Log, SysUtils;

  const
    maxKeyBuffer = 64;

  var
    __crt0_argv: PPchar; cvar; external;
    myargv: array [0..255] of Pchar;
    keyring: array [0..maxKeyBuffer - 1] of Integer;
    keybeg, keyend: Integer;

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
    if not IsFullKeyboard then
    begin
      keyring[keyend] := scancode;
      keyend := (keyend + 1) mod maxKeyBuffer
    end
  end;

  /// MACRO ///

//from "sdl_pixels.h"

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
    result := nil
  end;

  function SDL_RWFromConstMem(const mem: Pointer; size: SInt32): PSDL_RWops;
  begin
    result := nil
  end;

(********** KEYBOARD **********)

  function SDL_GetScancodeName(scancode: TSDL_ScanCode): PAnsiChar;
  begin
    result := ''
  end;

  function SDL_IsTextInputActive: TSDL_Bool;
  begin
    result := SDL_FALSE
  end;

  procedure SDL_StartTextInput;
  begin
  end;

  procedure SDL_StopTextInput;
  begin
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

  function SDL_GetPerformanceCounter: UInt64;
  begin
    e_LogWriteln('SDL_GetPerformanceCounter');
    result := 1
  end;

  function SDL_GetPerformanceFrequency: UInt64;
  begin
    e_LogWriteln('SDL_GetPerformanceFrequency');
    result := 1
  end;

  procedure SDL_Delay(ms: UInt32);
  begin
    e_LogWriteln('SDL_Delay');
    //allegro_rest(ms)
  end;

  function SDL_GetTicks: UInt32;
  begin
    e_LogWriteln('SDL_GetTicks');
    result := 1
  end;

(********** DISPLAY MODES **********)

  function SDL_GetDesktopDisplayMode(displayIndex: SInt32; mode: PSDL_DisplayMode): SInt32;
  begin
    e_LogWritefln('SDL_GetDesktopDisplayMode %s', [displayIndex]);
    result := -1;
    mode.format := SDL_PIXELFORMAT_UNKNOWN; (* FIXIT *)
    mode.refresh_rate := 0;
    mode.driverdata := nil;
    if get_desktop_resolution(@mode.w, @mode.h) = 0 then
      result := 0
  end;

  function SDL_GetClosestDisplayMode(displayIndex: SInt32; const mode: PSDL_DisplayMode; closest: PSDL_DisplayMode): PSDL_DisplayMode;
  begin
    e_LogWritefln('SDL_GetClosestDisplayMode %s', [displayIndex]);
    result := nil;
  end;

  function SDL_GetNumDisplayModes(displayIndex: SInt32): SInt32;
    var m: PGFX_MODE_LIST;
  begin
    e_LogWritefln('SDL_GetNumDisplayModes %s', [displayIndex]);
    result := -1;
    m := get_gfx_mode_list(GFX_AUTODETECT);
    if m <> nil then
    begin
      result := m.num_modes;
      destroy_gfx_mode_list(m);
    end
  end;

  function SDL_GetDisplayMode(displayIndex: SInt32; modeIndex: SInt32; mode: PSDL_DisplayMode): SInt32;
    var m: PGFX_MODE_LIST;
  begin
    e_LogWritefln('SDL_GetDisplayMode %s %s', [displayIndex, modeIndex]);
    result := -1;
    m := get_gfx_mode_list(GFX_AUTODETECT);
    if (m <> nil) and (modeIndex >= 0) and (modeIndex < m.num_modes) then
    begin
      mode.format := SDL_PIXELFORMAT_UNKNOWN; (* FIX IT *)
      mode.w := m.mode[modeIndex].width;
      mode.h := m.mode[modeIndex].height;
      mode.refresh_rate := 0;
      mode.driverdata := nil;
      destroy_gfx_mode_list(m);
      result := 0
    end
  end;

  function SDL_GetDisplayDPI(displayIndex: SInt32; ddpi, hdpi, vdpi: PFloat): SInt32;
  begin
    result := -1
  end;

(*********** WINDOW MANAGEMENT **********)

  function SDL_CreateWindow(const title: PAnsiChar; x: SInt32; y: SInt32; w: SInt32; h: SInt32; flags: UInt32): PSDL_Window;
    var window: PSDL_Window; mode: Integer;
  begin
    e_LogWritefln('SDL_CreateWindow %s %s %s %s %s %u', [title, x, y, w, h, flags]);
    result := nil;

    if (flags and (SDL_WINDOW_FULLSCREEN or SDL_WINDOW_FULLSCREEN_DESKTOP)) <> 0 then
      mode := GFX_AUTODETECT_FULLSCREEN
    else
      mode := GFX_AUTODETECT_WINDOWED;
    mode := GFX_SAFE;

    if set_gfx_mode(mode, w, h, 0, 0) = 0 then
    begin
      new(window);
      set_window_title(title);
      if sdl2allegro_screen <> nil then
        destroy_bitmap(sdl2allegro_screen);
      sdl2allegro_screen := create_video_bitmap(w, h);
      if sdl2allegro_screen = nil then
        sdl2allegro_screen := create_system_bitmap(w, h);
      if sdl2allegro_screen = nil then
        sdl2allegro_screen := create_bitmap(w, h);
      assert(sdl2allegro_screen <> nil);
      window.w := w;
      window.h := h;
      window.mode := mode;
      result := window
    end
  end;

  function SDL_SetWindowFullscreen(window: PSDL_Window; flags: UInt32): SInt32;
    var mode: Integer;
  begin
    e_LogWritefln('SDL_SetWindowFullscreen %u', [flags]);
    result := -1;
    if window = nil then exit;

    if (flags and (SDL_WINDOW_FULLSCREEN or SDL_WINDOW_FULLSCREEN_DESKTOP)) <> 0 then
      mode := GFX_AUTODETECT_FULLSCREEN
    else
      mode := GFX_AUTODETECT_WINDOWED;
    mode := GFX_SAFE;

    if set_gfx_mode(mode, window.w, window.h, 0, 0) = 0 then
    begin
(* 
     if sdl2allegro_screen <> nil then
        destroy_bitmap(sdl2allegro_screen);
*)
      sdl2allegro_screen := create_video_bitmap(window.w, window.h);
      if sdl2allegro_screen = nil then
        sdl2allegro_screen := create_system_bitmap(window.w, window.h);
      if sdl2allegro_screen = nil then
        sdl2allegro_screen := create_bitmap(window.w, window.h);
      assert(sdl2allegro_screen <> nil);
      window.mode := mode;
      result := 0
    end
  end;

  procedure SDL_SetWindowSize(window: PSDL_Window; w: SInt32; h: SInt32);
  begin
    e_LogWritefln('SDL_SetWindowSize %s %s', [w, h]);
    if window = nil then exit;
    if set_gfx_mode(window.mode, w, h, 0, 0) = 0 then
    begin
      if sdl2allegro_screen <> nil then
        destroy_bitmap(sdl2allegro_screen);
      if sdl2allegro_screen = nil then
        sdl2allegro_screen := create_system_bitmap(w, h);
      if sdl2allegro_screen = nil then
        sdl2allegro_screen := create_bitmap(w, h);
      sdl2allegro_screen := create_video_bitmap(w, h);
      assert(sdl2allegro_screen <> nil);
      window.w := w;
      window.h := h;
    end
  end;

  procedure SDL_DestroyWindow(window: PSDL_Window);
  begin
    e_LogWriteln('SDL_DestroyWindow');
    if window = nil then exit;
    if sdl2allegro_screen <> nil then
      destroy_bitmap(sdl2allegro_screen);
    sdl2allegro_screen := nil;
    dispose(window)
  end;

  procedure SDL_GetWindowSize(window: PSDL_Window; w: PInt; h: PInt);
  begin
    e_LogWriteln('SDL_GetWindowSize');
    if window = nil then exit;
    if w <> nil then
      w^ := window.w;
    if h <> nil then
      h^ := window.h;
  end;

  procedure SDL_RestoreWindow(window: PSDL_Window);
  begin
    e_LogWriteln('SDL_RestoreWindow');
    if window = nil then exit;
    (* stub *)
  end;

  function SDL_SetWindowGammaRamp(window: PSDL_Window; const red: PUInt16; const green: PUInt16; const blue: PUInt16): SInt32;
  begin
    e_LogWriteln('SDL_SetWindowGammaRamp');
    result := -1;
    if window = nil then exit;
    result := 0
  end;

  function SDL_GetWindowGammaRamp(window: PSDL_Window; red: PUInt16; green: PUInt16; blue: PUInt16): SInt32;
  begin
    e_LogWriteln('SDL_GetWindowGammaRamp');
    result := -1;
    if window = nil then exit;
    result := 0
  end;

(********** OPENGL MANAGEMENT **********)

  function SDL_GL_MakeCurrent(window: PSDL_Window; context: TSDL_GLContext): SInt32;
  begin
    e_LogWriteln('SDL_GL_MakeCurrent');
    result := 0
  end;

  procedure SDL_GL_DeleteContext(context: TSDL_GLContext);
  begin
    e_LogWriteln('SDL_GL_DeleteContext');
  end;

  procedure SDL_GL_SwapWindow(window: PSDL_Window);
  begin
    e_LogWriteln('SDL_GL_SwapWindow');
    assert(sdl2allegro_screen <> nil);
    show_video_bitmap(sdl2allegro_screen);
  end;

  function SDL_GL_CreateContext(window: PSDL_Window): TSDL_GLContext;
  begin
    e_LogWriteln('SDL_GL_CreateContext');
    result := window
  end;

  function SDL_GL_SetSwapInterval(interval: SInt32): SInt32;
  begin
    e_LogWritefln('SDL_GL_SetSwapInterval %s', [interval]);
    result := 0
  end;

  function SDL_GL_SetAttribute(attr: TSDL_GLattr; value: SInt32): SInt32;
  begin
    e_LogWritefln('SDL_GL_SetAttribute %s %s', [attr, value]);
    result := -1;
  end;

  function SDL_GL_GetAttribute(attr: TSDL_GLattr; value: PInt): SInt32;
  begin
    e_LogWritefln('SDL_GL_GetAttribute %s', [attr]);
    value^ := 0; result := -1
  end;

(********** EVENTS **********)

  function SDL_PushEvent(event: PSDL_Event): SInt32;
  begin
    e_LogWriteln('SDL_PushEvent');
    result := 0
  end;

  function allegro_to_sdl_scancode (x: Integer): Integer;
  begin
    x := x and $7F;
    case x of
      KEY_A..KEY_Z: result := SDL_SCANCODE_A + (x - KEY_A);
      KEY_0..KEY_0: result := SDL_SCANCODE_0 + (x - KEY_0);
      KEY_0_PAD: result := SDL_SCANCODE_KP_0;
      KEY_1_PAD..KEY_9_PAD: result := SDL_SCANCODE_KP_1 + (x - KEY_1_PAD);
      KEY_F1..KEY_F12: result := SDL_SCANCODE_F1 + (x - KEY_F1);
      KEY_ESC: result := SDL_SCANCODE_ESCAPE;
      KEY_TILDE: result := SDL_SCANCODE_GRAVE;
      KEY_MINUS: result := SDL_SCANCODE_MINUS;
      KEY_EQUALS: result := SDL_SCANCODE_EQUALS;
      KEY_BACKSPACE: result := SDL_SCANCODE_BACKSPACE;
      KEY_TAB: result := SDL_SCANCODE_TAB;
      KEY_OPENBRACE: result := SDL_SCANCODE_LEFTBRACKET;
      KEY_CLOSEBRACE: result := SDL_SCANCODE_RIGHTBRACKET;
      KEY_ENTER: result := SDL_SCANCODE_RETURN;
      KEY_COLON: result := SDL_SCANCODE_SEMICOLON;
      KEY_QUOTE: result := SDL_SCANCODE_APOSTROPHE;
      KEY_BACKSLASH: result := SDL_SCANCODE_BACKSLASH;
      KEY_BACKSLASH2: result := SDL_SCANCODE_NONUSHASH; (* ??? *)
      KEY_COMMA: result := SDL_SCANCODE_COMMA;
      (*KEY_STOP: result :=;*) (* ??? *)
      KEY_SLASH: result := SDL_SCANCODE_SLASH;
      KEY_SPACE: result := SDL_SCANCODE_SPACE;
      KEY_INSERT: result := SDL_SCANCODE_INSERT;
      KEY_DEL: result := SDL_SCANCODE_DELETE;
      KEY_HOME: result := SDL_SCANCODE_HOME;
      KEY_END: result := SDL_SCANCODE_END;
      KEY_PGUP: result := SDL_SCANCODE_PAGEUP;
      KEY_PGDN: result := SDL_SCANCODE_PAGEDOWN;
      KEY_LEFT: result := SDL_SCANCODE_LEFT;
      KEY_RIGHT: result := SDL_SCANCODE_RIGHT;
      KEY_UP: result := SDL_SCANCODE_UP;
      KEY_DOWN: result := SDL_SCANCODE_DOWN;
      KEY_SLASH_PAD: result := SDL_SCANCODE_KP_DIVIDE;
      KEY_ASTERISK: result := SDL_SCANCODE_KP_MULTIPLY;
      KEY_MINUS_PAD: result := SDL_SCANCODE_KP_MINUS;
      KEY_PLUS_PAD: result := SDL_SCANCODE_KP_PLUS;
      KEY_DEL_PAD: result := SDL_SCANCODE_KP_PERIOD;
      KEY_ENTER_PAD: result := SDL_SCANCODE_KP_ENTER;
      KEY_PRTSCR: result := SDL_SCANCODE_PRINTSCREEN;
      KEY_PAUSE: result := SDL_SCANCODE_PAUSE;
      (*KEY_ABNT_C1: result :=;*) (* ??? *)
      KEY_YEN: result := SDL_SCANCODE_INTERNATIONAL3;
      KEY_KANA: result := SDL_SCANCODE_LANG3; (* ??? *)
      (*KEY_CONVERT: result :=;*) (* ??? *)
      (*KEY_NOCONVERT: result := ;*) (* ??? *)
      KEY_AT: result := SDL_SCANCODE_KP_AT;
      (*KEY_CIRCUMFLEX: result :=;*) (* ??? *)
      KEY_COLON2: result := SDL_SCANCODE_KP_COLON;
      (*KEY_KANJI: result :=;*) (* ??? *)
      KEY_EQUALS_PAD: result := SDL_SCANCODE_KP_EQUALS;
      (*KEY_BACKQUOTE: result :=;*) (* ??? *)
      KEY_SEMICOLON: result := SDL_SCANCODE_SEMICOLON;
      KEY_COMMAND: result := SDL_SCANCODE_LGUI;
      (*KEY_UNKNOWN1: result :=;*)
      (*KEY_UNKNOWN2: result :=;*)
      (*KEY_UNKNOWN3: result :=;*)
      (*KEY_UNKNOWN4: result :=;*)
      (*KEY_UNKNOWN5: result :=;*)
      (*KEY_UNKNOWN6: result :=;*)
      (*KEY_UNKNOWN7: result :=;*)
      (*KEY_UNKNOWN8: result :=;*)
      KEY_LSHIFT: result := SDL_SCANCODE_LSHIFT;
      KEY_RSHIFT: result := SDL_SCANCODE_RSHIFT;
      KEY_LCONTROL: result := SDL_SCANCODE_LCTRL;
      KEY_RCONTROL: result := SDL_SCANCODE_RCTRL;
      KEY_ALT: result := SDL_SCANCODE_LALT;
      KEY_ALTGR: result := SDL_SCANCODE_RALT;
      KEY_LWIN: result := SDL_SCANCODE_LGUI;
      KEY_RWIN: result := SDL_SCANCODE_RGUI;
      KEY_MENU: result := SDL_SCANCODE_MODE; (* ??? *)
      KEY_SCRLOCK: result := SDL_SCANCODE_SCROLLLOCK;
      KEY_NUMLOCK: result := SDL_SCANCODE_NUMLOCKCLEAR;
      KEY_CAPSLOCK: result := SDL_SCANCODE_CAPSLOCK;
    else result := SDL_SCANCODE_UNKNOWN
    end
  end;

  function SDL_PollEvent(event: PSDL_Event): SInt32;
    var alscan: Integer;
  begin
    e_LogWriteln('SDL_PollEvent');
    poll_keyboard;
    result := 0;
    if not IsEmptyKeyboard then
    begin
      alscan := NextScancode;
      if alscan and $80 = 0 then
        event.type_ := SDL_KEYDOWN
      else
        event.type_ := SDL_KEYDOWN;
      event.key.timestamp := 0;
      event.key.windowID := 0;
      (* df not use it?
      if alscan and $80 = 0 then
        event.key.state := SDL_PRESSED
      else
        event.key.state := SDL_RELEASED;
      *)
      event.key.keysym.scancode := allegro_to_sdl_scancode(alscan);
      event.key.keysym.sym := 0; (* df not use it? *)
      event.key.keysym._mod := 0; (* df not use it? *)
      result := 1
    end
  end;

(********** MOUSE **********)

  function SDL_ShowCursor(toggle: SInt32): SInt32;
  begin
    e_LogWritefln('SDL_ShowCursor %s', [toggle]);
    result := 0
  end;

(********** SDL **********)

  function SDL_SetHint( const name: PChar; const value: PChar) : boolean;
  begin
    e_LogWritefln('SDL_SetHint %s %s', [name, value]);
    result := false
  end;

  function SDL_GetError: PAnsiChar;
  begin
    e_LogWritefln('SDL_GetError => %s', [allegro_error]);
    result := allegro_error;
  end;

  function TestGet: AnsiString;
  begin
    result := '+++ Test concat qwertyiop!'
  end;

  procedure Test;
  begin
    // WTF? when 'Test concat qwertyio! ' it not messed!
    //e_LogWriteln('Test concat qwertyiop! ' + ParamStr(0));
    e_LogWriteln('Test concat qwertyiop! ' + TestGet);
  end;

  function SDL_Init(flags: UInt32): SInt32;
  begin
    Test;
    e_LogWritefln('SDL_Init %u', [flags]);
    result := -1;
    __crt0_argv := @myargv[0];
    myargv[0] := PChar(ParamStr(0));
    e_LogWritefln('argv[0] = %s', [myargv[0]]);
    if allegro_init = 0 then
    begin
      e_LogWriteln('SDL_Init inited! ' + ParamStr(0) + ' ');
      install_timer;
      install_keyboard;
      keyboard_lowlevel_callback := KeyboardWatcher;
      set_keyboard_rate(0, 0);
      result := 0
    end
  end;

  procedure SDL_Quit;
  begin
    e_LogWriteln('SDL_Quit');
    remove_keyboard;
    remove_timer;
    allegro_exit
  end;
