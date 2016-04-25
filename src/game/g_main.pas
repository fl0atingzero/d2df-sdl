(* Copyright (C)  DooM 2D:Forever Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *)
{$MODE DELPHI}
unit g_main;

interface

procedure Main();
procedure Init();
procedure Release();
procedure Update();
procedure Draw();
procedure KeyPress(K: Word);
procedure CharPress(C: Char);

var
  GameDir: string;
  DataDir: string;
  MapsDir: string;
  ModelsDir: string;
  GameWAD: string;

implementation

uses
  SDL2, GL, GLExt, wadreader, e_log, g_window,
  e_graphics, e_input, g_game, g_console, g_gui,
  e_sound, g_options, g_sound, g_player,
  g_weapons, SysUtils, g_triggers, MAPDEF, g_map,
  MAPSTRUCT, g_menu, g_language, g_net, utils, conbuf;

var
  charbuff: Array [0..15] of Char;

procedure Main();
var
  sdlflags: LongWord;
begin
  e_InitWritelnDriver();

  GetDir(0, GameDir);
  MapsDir := GameDir + '/maps/';
  DataDir := GameDir + '/data/';
  ModelsDir := DataDir + 'models/';
  GameWAD := DataDir + 'Game.wad';

  e_InitLog(GameDir + '/' + LOG_FILENAME, WM_NEWFILE);

  e_WriteLog('Read config file', MSG_NOTIFY);
  g_Options_Read(GameDir + '/' + CONFIG_FILENAME);

{$IFDEF HEADLESS}
  conbufDumpToStdOut := true;
{$ENDIF}
  e_WriteToStdOut := False; //{$IFDEF HEADLESS}True;{$ELSE}False;{$ENDIF}

  //GetSystemDefaultLCID()

  //e_WriteLog('Read language file', MSG_NOTIFY);
  //g_Language_Load(DataDir + gLanguage + '.txt');
  e_WriteLog(gLanguage, MSG_NOTIFY);
  g_Language_Set(gLanguage);

{$IFDEF HEADLESS}
  sdlflags := SDL_INIT_TIMER or $00004000;
{$ELSE}
 {$IFDEF USE_SDLMIXER}
  sdlflags := SDL_INIT_EVERYTHING;
 {$ELSE}
  sdlflags := SDL_INIT_JOYSTICK or SDL_INIT_TIMER or SDL_INIT_VIDEO;
 {$ENDIF}
{$ENDIF}
  if SDL_Init(sdlflags) < 0 then
    raise Exception.Create('SDL: Init failed: ' + SDL_GetError());

{$IFDEF HEADLESS}
  SDL_StartTextInput();
{$ENDIF}

  e_WriteLog('Entering SDLMain', MSG_NOTIFY);

{$WARNINGS OFF}
  SDLMain();
{$WARNINGS ON}

{$IFDEF HEADLESS}
  SDL_StopTextInput();
{$ENDIF}

  e_WriteLog('Releasing SDL', MSG_NOTIFY);
  SDL_Quit();
end;

procedure Init();
var
  a: Integer;
begin
  Randomize;

  e_WriteLog('Init Input', MSG_NOTIFY);
  e_InitInput();

  if (e_JoysticksAvailable > 0) then
    e_WriteLog('Input: Joysticks available.', MSG_NOTIFY)
  else
    e_WriteLog('Input: No Joysticks.', MSG_NOTIFY);

  if (not gNoSound) then
  begin
    e_WriteLog('Initializing sound system', MSG_NOTIFY);
    e_InitSoundSystem({$IFDEF HEADLESS}True{$ELSE}False{$ENDIF});
  end;

  e_WriteLog('Init game', MSG_NOTIFY);
  g_Game_Init();

  for a := 0 to 15 do charbuff[a] := ' ';
end;

procedure Release();
begin
  e_WriteLog('Releasing engine', MSG_NOTIFY);
  e_ReleaseEngine();

  e_WriteLog('Releasing Input', MSG_NOTIFY);
  e_ReleaseInput();

  if not gNoSound then
  begin
    e_WriteLog('Releasing FMOD', MSG_NOTIFY);
    e_ReleaseSoundSystem();
  end;
end;

procedure Update();
begin
  g_Game_Update();
end;

procedure Draw();
begin
  g_Game_Draw();
end;

function Translit(S: String): String;
var
  i: Integer;
begin
  Result := S;
  for i := 1 to Length(Result) do
    case Result[i] of
      '�': Result[i] := 'Q';
      '�': Result[i] := 'W';
      '�': Result[i] := 'E';
      '�': Result[i] := 'R';
      '�': Result[i] := 'T';
      '�': Result[i] := 'Y';
      '�': Result[i] := 'U';
      '�': Result[i] := 'I';
      '�': Result[i] := 'O';
      '�': Result[i] := 'P';
      '�': Result[i] := '['; //Chr(219);
      '�': Result[i] := ']'; //Chr(221);
      '�': Result[i] := 'A';
      '�': Result[i] := 'S';
      '�': Result[i] := 'D';
      '�': Result[i] := 'F';
      '�': Result[i] := 'G';
      '�': Result[i] := 'H';
      '�': Result[i] := 'J';
      '�': Result[i] := 'K';
      '�': Result[i] := 'L';
      '�': Result[i] := ';'; //Chr(186);
      '�': Result[i] := #39; //Chr(222);
      '�': Result[i] := 'Z';
      '�': Result[i] := 'X';
      '�': Result[i] := 'C';
      '�': Result[i] := 'V';
      '�': Result[i] := 'B';
      '�': Result[i] := 'N';
      '�': Result[i] := 'M';
      '�': Result[i] := ','; //Chr(188);
      '�': Result[i] := '.'; //Chr(190);
    end;
end;


function CheckCheat (ct: TStrings_Locale; eofs: Integer=0): Boolean;
var
  ls1, ls2: string;
begin
  ls1 :=          CheatEng[ct];
  ls2 := Translit(CheatRus[ct]);
  if length(ls1) = 0 then ls1 := '~';
  if length(ls2) = 0 then ls2 := '~';
  result :=
    (Copy(charbuff, 17-Length(ls1)-eofs, Length(ls1)) = ls1) or
    (Translit(Copy(charbuff, 17-Length(ls1)-eofs, Length(ls1))) = ls1) or
    (Copy(charbuff, 17-Length(ls2)-eofs, Length(ls2)) = ls2) or
    (Translit(Copy(charbuff, 17-Length(ls2)-eofs, Length(ls2))) = ls2);
  {
  if ct = I_GAME_CHEAT_JETPACK then
  begin
    e_WriteLog('ls1: ['+ls1+']', MSG_NOTIFY);
    e_WriteLog('ls2: ['+ls2+']', MSG_NOTIFY);
    e_WriteLog('bf0: ['+Copy(charbuff, 17-Length(ls1)-eofs, Length(ls1))+']', MSG_NOTIFY);
    e_WriteLog('bf1: ['+Translit(Copy(charbuff, 17-Length(ls1)-eofs, Length(ls1)))+']', MSG_NOTIFY);
    e_WriteLog('bf2: ['+Copy(charbuff, 17-Length(ls2)-eofs, Length(ls2))+']', MSG_NOTIFY);
    e_WriteLog('bf3: ['+Translit(Copy(charbuff, 17-Length(ls2)-eofs, Length(ls2)))+']', MSG_NOTIFY);
  end;
  }
end;


procedure Cheat();
const
  CHEAT_DAMAGE = 500;
label
  Cheated;
var
  s, s2: string;
  c: Char16;
  a: Integer;
begin
  if (not gGameOn) or (not gCheats) or ((gGameSettings.GameType <> GT_SINGLE) and
    (gGameSettings.GameMode <> GM_COOP) and (not gDebugMode))
    or g_Game_IsNet then Exit;

  s := 'SOUND_GAME_RADIO';

  //
  if CheckCheat(I_GAME_CHEAT_GODMODE) then
  begin
    if gPlayer1 <> nil then gPlayer1.GodMode := not gPlayer1.GodMode;
    if gPlayer2 <> nil then gPlayer2.GodMode := not gPlayer2.GodMode;
    goto Cheated;
  end;
  // RAMBO
  if CheckCheat(I_GAME_CHEAT_WEAPONS) then
  begin
    if gPlayer1 <> nil then gPlayer1.AllRulez(False);
    if gPlayer2 <> nil then gPlayer2.AllRulez(False);
    goto Cheated;
  end;
  // TANK
  if CheckCheat(I_GAME_CHEAT_HEALTH) then
  begin
    if gPlayer1 <> nil then gPlayer1.AllRulez(True);
    if gPlayer2 <> nil then gPlayer2.AllRulez(True);
    goto Cheated;
  end;
  // IDDQD
  if CheckCheat(I_GAME_CHEAT_DEATH) then
  begin
    if gPlayer1 <> nil then gPlayer1.Damage(CHEAT_DAMAGE, 0, 0, 0, HIT_TRAP);
    if gPlayer2 <> nil then gPlayer2.Damage(CHEAT_DAMAGE, 0, 0, 0, HIT_TRAP);
    s := 'SOUND_MONSTER_HAHA';
    goto Cheated;
  end;
  //
  if CheckCheat(I_GAME_CHEAT_DOORS) then
  begin
    g_Triggers_OpenAll();
    goto Cheated;
  end;
  // GOODBYE
  if CheckCheat(I_GAME_CHEAT_NEXTMAP) then
  begin
    if gTriggers <> nil then
      for a := 0 to High(gTriggers) do
        if gTriggers[a].TriggerType = TRIGGER_EXIT then
        begin
          gExitByTrigger := True;
          g_Game_ExitLevel(gTriggers[a].Data.MapName);
          Break;
        end;
    goto Cheated;
  end;
  //
  s2 := Copy(charbuff, 15, 2);
  if CheckCheat(I_GAME_CHEAT_CHANGEMAP, 2) and (s2[1] >= '0') and (s2[1] <= '9') and (s2[2] >= '0') and (s2[2] <= '9') then
  begin
    if g_Map_Exist(MapsDir+gGameSettings.WAD+':\MAP'+s2) then
    begin
      c := 'MAP00';
      c[3] := s2[1];
      c[4] := s2[2];
      g_Game_ExitLevel(c);
    end;
    goto Cheated;
  end;
  //
  if CheckCheat(I_GAME_CHEAT_FLY) then
  begin
    gFly := not gFly;
    goto Cheated;
  end;
  // BULLFROG
  if CheckCheat(I_GAME_CHEAT_JUMPS) then
  begin
    VEL_JUMP := 30-VEL_JUMP;
    goto Cheated;
  end;
  // FORMULA1
  if CheckCheat(I_GAME_CHEAT_SPEED) then
  begin
    MAX_RUNVEL := 32-MAX_RUNVEL;
    goto Cheated;
  end;
  // CONDOM
  if CheckCheat(I_GAME_CHEAT_SUIT) then
  begin
    if gPlayer1 <> nil then gPlayer1.GiveItem(ITEM_SUIT);
    if gPlayer2 <> nil then gPlayer2.GiveItem(ITEM_SUIT);
    goto Cheated;
  end;
  //
  if CheckCheat(I_GAME_CHEAT_AIR) then
  begin
    if gPlayer1 <> nil then gPlayer1.GiveItem(ITEM_OXYGEN);
    if gPlayer2 <> nil then gPlayer2.GiveItem(ITEM_OXYGEN);
    goto Cheated;
  end;
  // PURELOVE
  if CheckCheat(I_GAME_CHEAT_BERSERK) then
  begin
    if gPlayer1 <> nil then gPlayer1.GiveItem(ITEM_MEDKIT_BLACK);
    if gPlayer2 <> nil then gPlayer2.GiveItem(ITEM_MEDKIT_BLACK);
    goto Cheated;
  end;
  //
  if CheckCheat(I_GAME_CHEAT_JETPACK) then
  begin
    if gPlayer1 <> nil then gPlayer1.GiveItem(ITEM_JETPACK);
    if gPlayer2 <> nil then gPlayer2.GiveItem(ITEM_JETPACK);
    goto Cheated;
  end;
  // CASPER
  if CheckCheat(I_GAME_CHEAT_NOCLIP) then
  begin
    if gPlayer1 <> nil then gPlayer1.SwitchNoClip;
    if gPlayer2 <> nil then gPlayer2.SwitchNoClip;
    goto Cheated;
  end;
  //
  if CheckCheat(I_GAME_CHEAT_NOTARGET) then
  begin
    if gPlayer1 <> nil then gPlayer1.NoTarget := not gPlayer1.NoTarget;
    if gPlayer2 <> nil then gPlayer2.NoTarget := not gPlayer2.NoTarget;
    goto Cheated;
  end;
  // INFERNO
  if CheckCheat(I_GAME_CHEAT_NORELOAD) then
  begin
    if gPlayer1 <> nil then gPlayer1.NoReload := not gPlayer1.NoReload;
    if gPlayer2 <> nil then gPlayer2.NoReload := not gPlayer2.NoReload;
    goto Cheated;
  end;
  if CheckCheat(I_GAME_CHEAT_AIMLINE) then
  begin
    gAimLine := not gAimLine;
    goto Cheated;
  end;
  if CheckCheat(I_GAME_CHEAT_AUTOMAP) then
  begin
    gShowMap := not gShowMap;
    goto Cheated;
  end;
  Exit;

Cheated:
  g_Sound_PlayEx(s);
end;

procedure KeyPress(K: Word);
var
  Msg: g_gui.TMessage;
begin
  case K of
    IK_PAUSE: // <Pause/Break>:
      begin
        if (g_ActiveWindow = nil) then
          g_Game_Pause(not gPause);
      end;

    IK_BACKQUOTE: // <`/~/�/�>:
      begin
        g_Console_Switch();
      end;

    IK_ESCAPE: // <Esc>:
      begin
        if gChatShow then
        begin
          g_Console_Chat_Switch();
          Exit;
        end;

        if gConsoleShow then
          g_Console_Switch()
        else
          if g_ActiveWindow <> nil then
            begin
              Msg.Msg := WM_KEYDOWN;
              Msg.WParam := IK_ESCAPE;
              g_ActiveWindow.OnMessage(Msg);
            end
          else
            if gState <> STATE_FOLD then
              if gGameOn
              or (gState = STATE_INTERSINGLE)
              or (gState = STATE_INTERCUSTOM)
              then
                g_Game_InGameMenu(True)
              else
                if (gExit = 0) and (gState <> STATE_SLIST) then
                  begin
                    if gState <> STATE_MENU then
                      if NetMode <> NET_NONE then
                      begin
                        g_Game_StopAllSounds(True);
                        g_Game_Free;
                        gState := STATE_MENU;
                        Exit;
                      end;

                    g_GUI_ShowWindow('MainMenu');
                    g_Sound_PlayEx('MENU_OPEN');
                  end;
      end;

    IK_F2, IK_F3, IK_F4, IK_F5, IK_F6, IK_F7, IK_F10:
      begin // <F2> .. <F6> � <F12>
        if gGameOn and (not gConsoleShow) and (not gChatShow) then
        begin
          while g_ActiveWindow <> nil do
            g_GUI_HideWindow(False);

          if (not g_Game_IsNet) then
            g_Game_Pause(True);

          case K of
            IK_F2:
              g_Menu_Show_SaveMenu();
            IK_F3:
              g_Menu_Show_LoadMenu();
            IK_F4:
              g_Menu_Show_GameSetGame();
            IK_F5:
              g_Menu_Show_OptionsVideo();
            IK_F6:
              g_Menu_Show_OptionsSound();
            IK_F7:
              g_Menu_Show_EndGameMenu();
            IK_F10:
              g_Menu_Show_QuitGameMenu();
          end;
        end;
      end;

    else
      begin
        gJustChatted := False;
        if gConsoleShow or gChatShow then
          g_Console_Control(K)
        else
          if g_ActiveWindow <> nil then
          begin
            Msg.Msg := WM_KEYDOWN;
            Msg.WParam := K;
            g_ActiveWindow.OnMessage(Msg);
          end
          else
          begin
            if (gState = STATE_MENU) then
            begin
              g_GUI_ShowWindow('MainMenu');
              g_Sound_PlayEx('MENU_OPEN');
            end;
          end;
      end;
  end;
end;

procedure CharPress(C: Char);
var
  Msg: g_gui.TMessage;
  a: Integer;
begin
  if (not gChatShow) and ((C = '`') or (C = '~') or (C = '�') or (C = '�')) then
    Exit;

  if gConsoleShow or gChatShow then
    g_Console_Char(C)
  else
    if g_ActiveWindow <> nil then
    begin
      Msg.Msg := WM_CHAR;
      Msg.WParam := Ord(C);
      g_ActiveWindow.OnMessage(Msg);
    end
    else
    begin
      for a := 0 to 14 do charbuff[a] := charbuff[a+1];
      charbuff[15] := UpCase1251(C);
      Cheat();
    end;
end;

end.
