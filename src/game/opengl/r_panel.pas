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
    g_base, g_basic
  ;

  // TODO: remove WITH operator

  procedure r_Panel_Draw (constref p: TPanel; hasAmbient: Boolean; constref ambColor: TDFColor);
    var xx, yy: Integer; NoTextureID: DWORD; NW, NH: Word;
  begin
    with p do
    begin
      if {Enabled and} (FCurTexture >= 0) and (Width > 0) and (Height > 0) and (Alpha < 255) {and g_Collide(X, Y, Width, Height, sX, sY, sWidth, sHeight)} then
      begin
        if TextureIDs[FCurTexture].Anim then
        begin
          if TextureIDs[FCurTexture].AnTex = nil then
            Exit;
          for xx := 0 to Width div TextureWidth - 1 do
            for yy := 0 to Height div TextureHeight - 1 do
              r_Animation_Draw(TextureIDs[FCurTexture].AnTex, X + xx * TextureWidth, Y + yy * TextureHeight, TMirrorType.None);
        end
        else
        begin
          case TextureIDs[FCurTexture].Tex of
            LongWord(TEXTURE_SPECIAL_WATER): e_DrawFillQuad(X, Y, X + Width - 1, Y + Height - 1, 0, 0, 255, 0, TBlending.Filter);
            LongWord(TEXTURE_SPECIAL_ACID1): e_DrawFillQuad(X, Y, X + Width - 1, Y + Height - 1, 0, 230, 0, 0, TBlending.Filter);
            LongWord(TEXTURE_SPECIAL_ACID2): e_DrawFillQuad(X, Y, X + Width - 1, Y + Height - 1, 230, 0, 0, 0, TBlending.Filter);
            LongWord(TEXTURE_NONE):
              if g_Texture_Get('NOTEXTURE', NoTextureID) then
              begin
                e_GetTextureSize(NoTextureID, @NW, @NH);
                e_DrawFill(NoTextureID, X, Y, Width div NW, Height div NH, 0, False, False);
              end
              else
              begin
                xx := X + (Width div 2);
                yy := Y + (Height div 2);
                e_DrawFillQuad(X, Y, xx, yy, 255, 0, 255, 0);
                e_DrawFillQuad(xx, Y, X + Width - 1, yy, 255, 255, 0, 0);
                e_DrawFillQuad(X, yy, xx, Y + Height - 1, 255, 255, 0, 0);
                e_DrawFillQuad(xx, yy, X + Width - 1, Y + Height - 1, 255, 0, 255, 0);
              end;
            else
            begin
              if not movingActive then
                e_DrawFill(TextureIDs[FCurTexture].Tex, X, Y, Width div TextureWidth, Height div TextureHeight, Alpha, True, Blending, hasAmbient)
              else
                e_DrawFillX(TextureIDs[FCurTexture].Tex, X, Y, Width, Height, Alpha, True, Blending, g_dbg_scale, hasAmbient);
              if hasAmbient then
                e_AmbientQuad(X, Y, Width, Height, ambColor.r, ambColor.g, ambColor.b, ambColor.a);
            end
          end
        end
      end
    end
  end;

  procedure r_Panel_DrawShadowVolume (constref p: TPanel; lightX, lightY: Integer; radius: Integer);

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
    with p do
    begin
      if radius < 4 then exit;
      if Enabled and (FCurTexture >= 0) and (Width > 0) and (Height > 0) and (Alpha < 255) {and g_Collide(X, Y, Width, Height, sX, sY, sWidth, sHeight)} then
      begin
        if not TextureIDs[FCurTexture].Anim then
        begin
          case TextureIDs[FCurTexture].Tex of
            LongWord(TEXTURE_SPECIAL_WATER): exit;
            LongWord(TEXTURE_SPECIAL_ACID1): exit;
            LongWord(TEXTURE_SPECIAL_ACID2): exit;
            LongWord(TEXTURE_NONE): exit;
          end;
        end;
        if (X + Width < lightX - radius) then exit;
        if (Y + Height < lightY - radius) then exit;
        if (X > lightX + radius) then exit;
        if (Y > lightY + radius) then exit;
        //e_DrawFill(TextureIDs[FCurTexture].Tex, X, Y, Width div TextureWidth, Height div TextureHeight, Alpha, True, Blending);
        glBegin(GL_QUADS);
          drawLine(x,         y,          x + width, y); // top
          drawLine(x + width, y,          x + width, y + height); // right
          drawLine(x + width, y + height, x,         y + height); // bottom
          drawLine(x,         y + height, x,         y); // left
        glEnd;
      end
    end
  end;

end.
