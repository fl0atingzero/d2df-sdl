{$INCLUDE ../shared/a_modes.inc}
{$SCOPEDENUMS OFF}

interface

  {$I sdltype.inc}
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
    TSDL_Window = record end;

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

  /// MACRO ///
  function SDL_BITSPERPIXEL(X: Cardinal): Cardinal;

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
  /// MACRO ///

  function SDL_BITSPERPIXEL(X: Cardinal): Cardinal;
  begin
    Result := (X shr 8) and $FF;
  end;

  /// FUNCTIONS ////

  function SDL_SetWindowGammaRamp(window: PSDL_Window; const red: PUInt16; const green: PUInt16; const blue: PUInt16): SInt32;
  begin
    result := 0
  end;

  function SDL_GetWindowGammaRamp(window: PSDL_Window; red: PUInt16; green: PUInt16; blue: PUInt16): SInt32;
  begin
    result := 0
  end;


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
  



  function SDL_GetScancodeName(scancode: TSDL_ScanCode): PAnsiChar;
  begin
    result := ''
  end;




  function SDL_GetPerformanceCounter: UInt64;
  begin
    result := 0
  end;

  function SDL_GetPerformanceFrequency: UInt64;
  begin
    result := 0
  end;



  function SDL_RWFromFile(const _file: PAnsiChar; const mode: PAnsiChar): PSDL_RWops;
  begin
    result := nil
  end;

  function SDL_RWFromConstMem(const mem: Pointer; size: SInt32): PSDL_RWops;
  begin
    result := nil
  end;




  function SDL_GetDisplayDPI(displayIndex: SInt32; ddpi, hdpi, vdpi: PFloat): SInt32;
  begin
    result := 0
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

  function SDL_GetNumTouchDevices: SInt32;
  begin
    result := 0
  end;



  function SDL_GetDesktopDisplayMode(displayIndex: SInt32; mode: PSDL_DisplayMode): SInt32;
  begin
    result := 0
  end;

  function SDL_GetClosestDisplayMode(displayIndex: SInt32; const mode: PSDL_DisplayMode; closest: PSDL_DisplayMode): PSDL_DisplayMode;
  begin
    result := nil
  end;

  procedure SDL_DestroyWindow(window: PSDL_Window);
  begin
  end;

  procedure SDL_GL_DeleteContext(context: TSDL_GLContext);
  begin
  end;

  procedure SDL_RestoreWindow(window: PSDL_Window);
  begin
  end;

  procedure SDL_SetWindowSize(window: PSDL_Window; w: SInt32; h: SInt32);
  begin
  end;

  function SDL_CreateWindow(const title: PAnsiChar; x: SInt32; y: SInt32; w: SInt32; h: SInt32; flags: UInt32): PSDL_Window;
  begin
    result := nil
  end;

  function SDL_SetWindowFullscreen(window: PSDL_Window; flags: UInt32): SInt32;
  begin
    result := 0
  end;

  function SDL_GL_MakeCurrent(window: PSDL_Window; context: TSDL_GLContext): SInt32;
  begin
    result := 0
  end;

  procedure SDL_GetWindowSize(window: PSDL_Window; w: PInt; h: PInt);
  begin
  end;

  function SDL_GetNumDisplayModes(displayIndex: SInt32): SInt32;
  begin
    result := 0
  end;

  function SDL_GetDisplayMode(displayIndex: SInt32; modeIndex: SInt32; mode: PSDL_DisplayMode): SInt32;
  begin
    result := 0
  end;

  function SDL_ShowCursor(toggle: SInt32): SInt32;
  begin
    result := 0
  end;

  procedure SDL_Delay(ms: UInt32);
  begin
  end;

  procedure SDL_GL_SwapWindow(window: PSDL_Window);
  begin
  end;

  function SDL_GL_CreateContext(window: PSDL_Window): TSDL_GLContext;
  begin
    result := nil
  end;

  function SDL_GetTicks: UInt32;
  begin
    result := 0
  end;

  function SDL_PushEvent(event: PSDL_Event): SInt32;
  begin
    result := 0
  end;

  function SDL_PollEvent(event: PSDL_Event): SInt32;
  begin
    result := 0
  end;

  function SDL_GL_SetSwapInterval(interval: SInt32): SInt32;
  begin
    result := 0
  end;

  function SDL_GL_SetAttribute(attr: TSDL_GLattr; value: SInt32): SInt32;
  begin
    result := 0
  end;

  function SDL_GL_GetAttribute(attr: TSDL_GLattr; value: PInt): SInt32;
  begin
    result := 0
  end;

  function SDL_GetError: PAnsiChar;
  begin
    result := ''
  end;

  function SDL_Init(flags: UInt32): SInt32;
  begin
    result := 0
  end;

  procedure SDL_Quit();
  begin
  end;

  function SDL_SetHint( const name: PChar; const value: PChar) : boolean;
  begin
    result := false
  end;
