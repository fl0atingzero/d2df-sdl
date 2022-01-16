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
{$INCLUDE ../../shared/a_modes.inc}
unit r_panel;

interface

  uses g_panel, MAPDEF; // TPanel + TDFColor

  procedure r_Panel_Draw (constref p: TPanel; hasAmbient: Boolean; constref ambColor: TDFColor);
  procedure r_Panel_DrawShadowVolume (constref p: TPanel; lightX, lightY: Integer; radius: Integer);

implementation

  uses
    {$INCLUDE ../nogl/noGLuses.inc}
    SysUtils, Classes, Math,
    r_graphics, g_options, r_animations, r_textures,
    g_base, g_basic, g_map
  ;

  // TODO: remove WITH operator

  procedure r_Panel_Draw (constref p: TPanel; hasAmbient: Boolean; constref ambColor: TDFColor);
    var xx, yy: Integer; NoTextureID, TextureID, FramesID: DWORD; NW, NH: Word; Texture: Cardinal; IsAnim: Boolean;
  begin
    if {p.Enabled and} (p.FCurTexture >= 0) and (p.Width > 0) and (p.Height > 0) and (p.Alpha < 255) {and g_Collide(X, Y, Width, Height, sX, sY, sWidth, sHeight)} then
    begin
      Texture := p.TextureIDs[p.FCurTexture].Texture;
      IsAnim := p.TextureIDs[p.FCurTexture].Anim;
      if IsAnim then
      begin
        if p.TextureIDs[p.FCurTexture].AnTex <> nil then
        begin
          FramesID := Textures[Texture].FramesID;
          for xx := 0 to p.Width div p.TextureWidth - 1 do
            for yy := 0 to p.Height div p.TextureHeight - 1 do
              r_AnimationState_Draw(FramesID, p.TextureIDs[p.FCurTexture].AnTex, p.X + xx * p.TextureWidth, p.Y + yy * p.TextureHeight, TMirrorType.None);
        end
      end
      else
      begin
        TextureID := Textures[Texture].TextureID; // GL texture
        case TextureID of
          LongWord(TEXTURE_SPECIAL_WATER): e_DrawFillQuad(p.X, p.Y, p.X + p.Width - 1, p.Y + p.Height - 1, 0, 0, 255, 0, TBlending.Filter);
          LongWord(TEXTURE_SPECIAL_ACID1): e_DrawFillQuad(p.X, p.Y, p.X + p.Width - 1, p.Y + p.Height - 1, 0, 230, 0, 0, TBlending.Filter);
          LongWord(TEXTURE_SPECIAL_ACID2): e_DrawFillQuad(p.X, p.Y, p.X + p.Width - 1, p.Y + p.Height - 1, 230, 0, 0, 0, TBlending.Filter);
          LongWord(TEXTURE_NONE):
            if g_Texture_Get('NOTEXTURE', NoTextureID) then
            begin
              e_GetTextureSize(NoTextureID, @NW, @NH);
              e_DrawFill(NoTextureID, p.X, p.Y, p.Width div NW, p.Height div NH, 0, False, False);
            end
            else
            begin
              xx := p.X + (p.Width div 2);
              yy := p.Y + (p.Height div 2);
              e_DrawFillQuad(p.X, p.Y, xx, yy, 255, 0, 255, 0);
              e_DrawFillQuad(xx, p.Y, p.X + p.Width - 1, yy, 255, 255, 0, 0);
              e_DrawFillQuad(p.X, yy, xx, p.Y + p.Height - 1, 255, 255, 0, 0);
              e_DrawFillQuad(xx, yy, p.X + p.Width - 1, p.Y + p.Height - 1, 255, 0, 255, 0);
            end;
        else
          if not p.movingActive then
            e_DrawFill(TextureID, p.X, p.Y, p.Width div p.TextureWidth, p.Height div p.TextureHeight, p.Alpha, True, p.Blending, hasAmbient)
          else
            e_DrawFillX(TextureID, p.X, p.Y, p.Width, p.Height, p.Alpha, True, p.Blending, g_dbg_scale, hasAmbient);
          if hasAmbient then
            e_AmbientQuad(p.X, p.Y, p.Width, p.Height, ambColor.r, ambColor.g, ambColor.b, ambColor.a);
        end
      end
    end
  end;

  procedure r_Panel_DrawShadowVolume (constref p: TPanel; lightX, lightY: Integer; radius: Integer);
    var Texture: Cardinal;

    procedure extrude (x: Integer; y: Integer);
    begin
      glVertex2i(x + (x - lightX) * 500, y + (y - lightY) * 500);
      //e_WriteLog(Format('  : (%d,%d)', [x + (x - lightX) * 300, y + (y - lightY) * 300]), MSG_WARNING);
    end;

    procedure drawLine (x0: Integer; y0: Integer; x1: Integer; y1: Integer);
    begin
      // does this side facing the light?
      if ((x1 - x0) * (lightY - y0) - (lightX - x0) * (y1 - y0) >= 0) then exit;
      //e_WriteLog(Format('lightpan: (%d,%d)-(%d,%d)', [x0, y0, x1, y1]), MSG_WARNING);
      // this edge is facing the light, extrude and draw it
      glVertex2i(x0, y0);
      glVertex2i(x1, y1);
      extrude(x1, y1);
      extrude(x0, y0);
    end;

  begin
    if radius < 4 then exit;
    if p.Enabled and (p.FCurTexture >= 0) and (p.Width > 0) and (p.Height > 0) and (p.Alpha < 255) {and g_Collide(X, Y, Width, Height, sX, sY, sWidth, sHeight)} then
    begin
      if not p.TextureIDs[p.FCurTexture].Anim then
      begin
        Texture := p.TextureIDs[p.FCurTexture].Texture;
        case Textures[Texture].TextureID of
          LongWord(TEXTURE_SPECIAL_WATER): exit;
          LongWord(TEXTURE_SPECIAL_ACID1): exit;
          LongWord(TEXTURE_SPECIAL_ACID2): exit;
          LongWord(TEXTURE_NONE): exit;
        end;
      end;
      if (p.X + p.Width < lightX - radius) then exit;
      if (p.Y + p.Height < lightY - radius) then exit;
      if (p.X > lightX + radius) then exit;
      if (p.Y > lightY + radius) then exit;
      //e_DrawFill(TextureIDs[FCurTexture].Tex, X, Y, Width div TextureWidth, Height div TextureHeight, Alpha, True, Blending);
      glBegin(GL_QUADS);
        drawLine(p.x,           p.y,            p.x + p.width, p.y); // top
        drawLine(p.x + p.width, p.y,            p.x + p.width, p.y + p.height); // right
        drawLine(p.x + p.width, p.y + p.height, p.x,           p.y + p.height); // bottom
        drawLine(p.x,           p.y + p.height, p.x,           p.y); // left
      glEnd;
    end
  end;

end.
