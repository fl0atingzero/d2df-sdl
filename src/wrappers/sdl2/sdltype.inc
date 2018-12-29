//types from SDLtype_s.h / SDL_stdinc.h
type

  TSDL_Bool = (SDL_FALSE,SDL_TRUE);

  DWord = LongWord;

  PUInt8Array = ^TUInt8Array;
  PUInt8 = ^UInt8;
  PPUInt8 = ^PUInt8;
  UInt8 = Byte;
  TUInt8Array = array [0..MAXINT shr 1] of UInt8;

  PUInt16 = ^UInt16;
  UInt16 = word;

  PSInt8 = ^SInt8;
  SInt8 = Shortint;

  PSInt16 = ^SInt16;
  SInt16 = smallint;

  PUInt32 = ^UInt32;
  UInt32 = Cardinal;

  PSInt32 = ^SInt32;
  SInt32 = LongInt;

  PFloat = ^Float;
  PInt = ^LongInt;

  PShortInt = ^ShortInt;

  PUInt64 = ^UInt64;
  UInt64 = QWord;

  PInt64 = ^Int64;
//  Int64 = record
//    hi: UInt32;
//    lo: UInt32;
//  end;

  PSInt64 = ^SInt64;
  SInt64 = Int64;

  {$IFNDEF WIN64}
    size_t = UInt32;
  {$ELSE}
    size_t = UInt64;
  {$ENDIF}

  Float = Single;
