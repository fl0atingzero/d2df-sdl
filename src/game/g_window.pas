(* Copyright (C)  Doom 2D: Forever Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License ONLY.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *)
{$INCLUDE ../shared/a_modes.inc}
unit g_window;

interface

  procedure ProcessLoading (forceUpdate: Boolean=false);

implementation

  uses
    {$IFDEF ENABLE_RENDER}
      r_render,
    {$ENDIF}
    {$IFNDEF HEADLESS}
      g_system,
    {$ENDIF}
    e_sound, g_net
  ;

  procedure ProcessLoading (forceUpdate: Boolean = False);
    var update: Boolean;
  begin
    {$IFDEF HEADLESS}
      update := True;
    {$ELSE}
      update := sys_HandleInput() = False;
    {$ENDIF}
    if update then
    begin
      {$IFDEF ENABLE_RENDER}
        r_Render_DrawLoading(forceUpdate);
      {$ENDIF}
      e_SoundUpdate();
      if NetMode = NET_SERVER then
        g_Net_Host_Update
      else if (NetMode = NET_CLIENT) and (NetState <> NET_STATE_AUTH) then
        g_Net_Client_UpdateWhileLoading
    end
  end;

end.
