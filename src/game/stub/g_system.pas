(* Copyright (C) 2016 - The Doom2D.org team & involved community members <http://www.doom2d.org>.
 * This file is part of Doom2D Forever.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation, version 3 of
 * the License ONLY.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *)

{$INCLUDE ../../shared/a_modes.inc}
unit g_system;

interface

  uses utils;

  // use signed type as a result because timer can be not monotonic
  function sys_GetTicks (): Int64; inline;

  (* --- Utils --- *)
  procedure sys_Delay (ms: Integer); inline;
  procedure sys_YieldTimeSlice (); inline;

  (* --- Graphics --- *)
  function sys_GetDisplayModes (bpp: Integer): SSArray;
  function sys_SetDisplayMode (w, h, bpp: Integer; fullscreen, maximized: Boolean): Boolean;
  procedure sys_EnableVSync (yes: Boolean);
  procedure sys_Repaint;

  (* --- Input --- *)
  function sys_HandleEvents (): Boolean;
  procedure sys_RequestQuit;

  (* --- Init --- *)
  procedure sys_Init;
  procedure sys_Final;

implementation

  uses SysUtils;

  (* --------- Utils --------- *)

  function sys_GetTicks (): Int64;
  begin
    Result := Int64(GetTickCount64());
  end;

  procedure sys_Delay (ms: Integer);
  begin
    Sleep(ms);
  end;

  procedure sys_YieldTimeSlice ();
  begin
  {$IFNDEF WINDOWS}
    // TODO: Is this enough for other platforms? Needs testing.
    ThreadSwitch();
  {$ELSE}
    Sleep(1);
  {$ENDIF}
  end;

  (* --------- Graphics --------- *)

  procedure sys_Repaint;
  begin
  end;

  procedure sys_EnableVSync (yes: Boolean);
  begin
  end;

  function sys_GetDisplayModes (bpp: Integer): SSArray;
  begin
    Result := nil
  end;

  function sys_SetDisplayMode (w, h, bpp: Integer; fullscreen, maximized: Boolean): Boolean;
  begin
    Result := True
  end;

  (* --------- Input --------- *)

  function sys_HandleEvents (): Boolean;
  begin
    Result := False
  end;

  procedure sys_RequestQuit;
  begin
  end;

  (* --------- Init --------- *)

  procedure sys_Init;
  begin
  end;

  procedure sys_Final;
  begin
  end;

end.
