{$INCLUDE ../shared/a_modes.inc}
{$SCOPEDENUMS OFF}

interface

  uses SDL2;

  const
    MIX_INIT_FLAC        = $00000001;
    MIX_INIT_MOD         = $00000002;
    MIX_INIT_MODPLUG     = $00000004;
    MIX_INIT_MP3         = $00000008;
    MIX_INIT_OGG         = $00000010;
    MIX_INIT_FLUIDSYNTH  = $00000020;

    MIX_DEFAULT_FREQUENCY = 22050;
    MIX_DEFAULT_CHANNELS = 2;
    MIX_MAX_VOLUME       = 128;

{$IFDEF FPC}
   {$IF DEFINED(ENDIAN_LITTLE)}
      MIX_DEFAULT_FORMAT = AUDIO_S16LSB;
   {$ELSEIF DEFINED(ENDIAN_BIG)}
      MIX_DEFAULT_FORMAT = AUDIO_S16MSB;
   {$ELSE}
      {$FATAL Unable to determine endianness.}
   {$IFEND}
{$ENDIF}

  type
    PMix_Chunk = ^TMix_Chunk;
    TMix_Chunk = record
      allocated: Integer;
      abuf: PUInt8;
      alen: UInt32;
      volume: UInt8;
    end;

    PMix_Music = ^TMix_Music;
    TMix_Music = record end;

    TMix_Fading = (MIX_NO_FADING, MIX_FADING_OUT, MIX_FADING_IN);

    TMix_MusicType = (MUS_NONE,
                    MUS_CMD,
                    MUS_WAV,
                    MUS_MOD,
                    MUS_MID,
                    MUS_OGG,
                    MUS_MP3,
                    MUS_MP3_MAD,
                    MUS_FLAC,
                    MUS_MODPLUG);

  TMix_Channel_Finished = procedure(channel: Integer); cdecl;

  (** procedures **)

  function Mix_GetMusicType(music: TMix_Music): TMix_MusicType;
  function Mix_Init(flags: Integer): Integer;
  function Mix_OpenAudio(frequency: Integer; format: UInt16; channels: Integer; chunksize: Integer): Integer;
  function Mix_GetError: PAnsiChar;
  function Mix_QuerySpec(frequency: PInt; format: PUInt16; channels: PInt): Integer;
  function Mix_GetNumChunkDecoders: Integer;
  function Mix_GetChunkDecoder(index: Integer): PAnsiChar;
  function Mix_GetNumMusicDecoders: Integer;
  function Mix_GetMusicDecoder(index: Integer): PAnsiChar;
  function Mix_AllocateChannels(numchans: Integer): Integer;
  procedure Mix_ChannelFinished(channel_finished: TMix_Channel_Finished);
  function Mix_LoadMUS(_file: PAnsiChar): PMix_Music;
  function Mix_LoadMUS_RW(src: PSDL_RWops; freesrc: Integer): PMix_Music;
  function Mix_LoadWAV(_file: PAnsiChar): PMix_Chunk;
  function Mix_LoadWAV_RW(src: PSDL_RWops; freesrc: Integer): PMix_Chunk;

  function Mix_PlayChannel(channel: Integer; chunk: PMix_Chunk; loops: Integer): Integer;
  function Mix_Volume(channel: Integer; volume: Integer): Integer;
  function Mix_HaltMusic: Integer;
  function Mix_PlayMusic(music: PMix_Music; loops: Integer): Integer;
  function Mix_SetPanning(channel: Integer; left: UInt8; right: UInt8): Integer;
  procedure Mix_FreeChunk(chunk: PMix_Chunk);
  procedure Mix_FreeMusic(music: PMix_Music);

  function Mix_VolumeMusic(volume: Integer): Integer;
  function Mix_HaltChannel(channel: Integer): Integer;
  procedure Mix_CloseAudio;
  function Mix_PlayingMusic: Integer;
  function Mix_Paused(channel: Integer): Integer;
  procedure Mix_Pause(channel: Integer);
  procedure Mix_Resume(channel: Integer);

  procedure Mix_PauseMusic;
  function Mix_PausedMusic: Integer;
  procedure Mix_ResumeMusic;

implementation

  function Mix_GetMusicType(music: TMix_Music): TMix_MusicType;
  begin
    result := TMix_MusicType.MUS_NONE
  end;

  function Mix_Init(flags: Integer): Integer;
  begin
    result := 0
  end;

  function Mix_OpenAudio(frequency: Integer; format: UInt16; channels: Integer; chunksize: Integer): Integer;
  begin
    result := 0
  end;

  function Mix_GetError: PAnsiChar;
  begin
    result := ''
  end;

  function Mix_QuerySpec(frequency: PInt; format: PUInt16; channels: PInt): Integer;
  begin
    result := 0
  end;

  function Mix_GetNumChunkDecoders: Integer;
  begin
    result := 0
  end;

  function Mix_GetChunkDecoder(index: Integer): PAnsiChar;
  begin
    result := ''
  end;

  function Mix_GetNumMusicDecoders: Integer;
  begin
    result := 0
  end;

  function Mix_GetMusicDecoder(index: Integer): PAnsiChar;
  begin
    result := ''
  end;

  function Mix_AllocateChannels(numchans: Integer): Integer;
  begin
    result := 0
  end;

  procedure Mix_ChannelFinished(channel_finished: TMix_Channel_Finished);
  begin
  end;

  function Mix_LoadMUS(_file: PAnsiChar): PMix_Music;
  begin
    result := nil
  end;

  function Mix_LoadMUS_RW(src: PSDL_RWops; freesrc: Integer): PMix_Music;
  begin
    result := nil
  end;

  function Mix_LoadWAV(_file: PAnsiChar): PMix_Chunk;
  begin
    Result := Mix_LoadWAV_RW(SDL_RWFromFile(_file, 'rb'), 1);
  end;

  function Mix_LoadWAV_RW(src: PSDL_RWops; freesrc: Integer): PMix_Chunk;
  begin
    result := nil
  end;

  function Mix_PlayChannel(channel: Integer; chunk: PMix_Chunk; loops: Integer): Integer;
  begin
    result := 0
  end;

  function Mix_Volume(channel: Integer; volume: Integer): Integer;
  begin
    result := 0
  end;

  function Mix_HaltMusic: Integer;
  begin
    result := 0
  end;

  function Mix_PlayMusic(music: PMix_Music; loops: Integer): Integer;
  begin
    result := 0
  end;

  function Mix_SetPanning(channel: Integer; left: UInt8; right: UInt8): Integer;
  begin
    result := 0
  end;

  procedure Mix_FreeChunk(chunk: PMix_Chunk);
  begin
  end;

  procedure Mix_FreeMusic(music: PMix_Music);
  begin
  end;

  function Mix_VolumeMusic(volume: Integer): Integer;
  begin
    result := 0
  end;

  function Mix_HaltChannel(channel: Integer): Integer;
  begin
    result := 0
  end;

  procedure Mix_CloseAudio;
  begin
  end;

  function Mix_PlayingMusic: Integer;
  begin
    result := 0
  end;

  function Mix_Paused(channel: Integer): Integer;
  begin
    result := 0
  end;

  procedure Mix_Pause(channel: Integer);
  begin
  end;

  procedure Mix_Resume(channel: Integer);
  begin
  end;

  procedure Mix_PauseMusic;
  begin
  end;

  function Mix_PausedMusic: Integer;
  begin
    result := 0
  end;

  procedure Mix_ResumeMusic;
  begin
  end;

