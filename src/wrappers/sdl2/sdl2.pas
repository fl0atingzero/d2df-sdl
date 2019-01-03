unit sdl2;

{$IF DEFINED(USE_SDL2ALLEGRO)}
  {$INCLUDE sdl2allegro.inc}
{$ELSE}
  {$INCLUDE sdl2stub.inc}
{$ENDIF}

end.
