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
unit r_map;

interface

  uses g_panel, MAPDEF; // TPanel, TDFColor

  procedure r_Map_LoadTextures;

  procedure r_Map_DrawBack (dx, dy: Integer);
  procedure r_Map_DrawPanels (PanelType: Word; hasAmbient: Boolean; constref ambColor: TDFColor); // unaccelerated
  procedure r_Map_CollectDrawPanels (x0, y0, wdt, hgt: Integer);
  procedure r_Map_DrawPanelShadowVolumes (lightX: Integer; lightY: Integer; radius: Integer);
  procedure r_Map_DrawFlags;

  procedure r_Panel_Draw (constref p: TPanel; hasAmbient: Boolean; constref ambColor: TDFColor);
  procedure r_Panel_DrawShadowVolume (constref p: TPanel; lightX, lightY: Integer; radius: Integer);

implementation

  uses
    {$INCLUDE ../nogl/noGLuses.inc}
    SysUtils, Classes, Math, e_log, wadreader, CONFIG, utils,
    r_graphics, r_animations, r_textures,
    g_base, g_basic, g_game, g_options,
    g_map
  ;

  var
    RenTextures: array of record
      ID: DWORD;
      Width, Height: WORD;
      Anim: Boolean;
    end;

  procedure r_Map_LoadTextures;
    const
      log = True;
    var
      i, n: Integer;
      WadName, ResName: String;
      WAD, WADZ: TWADFile;
      ResData, ReszData: Pointer;
      ResLen, ReszLen: Integer;
      cfg: TConfig;
      TextureResource: String;
      Width, Height: Integer;
      FramesCount: Integer;
      BackAnim: Boolean;
  begin
    if Textures <> nil then
    begin
      n := Length(Textures);
      SetLength(RenTextures, n);
      for i := 0 to n - 1 do
      begin
        // e_LogWritefln('r_Map_LoadTextures: -> [%s] :: [%s]', [Textures[i].FullName, Textures[i].TextureName]);
        RenTextures[i].ID := LongWord(TEXTURE_NONE);
        RenTextures[i].Width := 0;
        RenTextures[i].Height := 0;
        RenTextures[i].Anim := False;
        case Textures[i].TextureName of
          TEXTURE_NAME_WATER: RenTextures[i].ID := LongWord(TEXTURE_SPECIAL_WATER);
          TEXTURE_NAME_ACID1: RenTextures[i].ID := LongWord(TEXTURE_SPECIAL_ACID1);
          TEXTURE_NAME_ACID2: RenTextures[i].ID := LongWord(TEXTURE_SPECIAL_ACID2);
        else
          WadName := g_ExtractWadName(Textures[i].FullName);
          ResName := g_ExtractFilePathName(Textures[i].FullName);
          WAD := TWADFile.Create();
          if WAD.ReadFile(WadName) then
          begin
            if WAD.GetResource(ResName, ResData, ResLen, log) then
            begin
              if IsWadData(ResData, ResLen) then
              begin
                WADz := TWADFile.Create();
                if WADz.ReadMemory(ResData, ResLen) then
                begin
                  if WADz.GetResource('TEXT/ANIM', ReszData, ReszLen) then
                  begin
                    cfg := TConfig.CreateMem(ReszData, ReszLen);
                    FreeMem(ReszData);
                    if cfg <> nil then
                    begin
                      TextureResource := cfg.ReadStr('', 'resource', '');
                      Width := cfg.ReadInt('', 'framewidth', 0);
                      Height := cfg.ReadInt('', 'frameheight', 0);
                      FramesCount := cfg.ReadInt('', 'framecount', 0);
                      // Speed := cfg.ReadInt('', 'waitcount', 0);
                      BackAnim := cfg.ReadBool('', 'backanimation', False);
                      RenTextures[i].Width := Width;
                      RenTextures[i].Height := Height;
                      if TextureResource <> '' then
                      begin
                        if WADz.GetResource('TEXTURES/' + TextureResource, ReszData, ReszLen) then
                        begin
                          if g_Frames_CreateMemory(@RenTextures[i].ID, '', ReszData, ReszLen, Width, Height, FramesCount, BackAnim) then
                            RenTextures[i].Anim := True
                          else
                            e_LogWritefln('r_Map_LoadTextures: failed to create frames object (%s)', [Textures[i].FullName]);
                          FreeMem(ReszData)
                        end
                        else
                          e_LogWritefln('r_Map_LoadTextures: failed to open animation resources (%s)', [Textures[i].FullName])
                      end
                      else
                        e_LogWritefln('r_Map_LoadTextures: failed to animation has no texture resource string (%s)', [Textures[i].FullName]);
                      cfg.Free
                    end
                    else
                      e_LogWritefln('r_Map_LoadTextures: failed to parse animation description (%s)', [Textures[i].FullName])
                  end
                  else
                    e_LogWritefln('r_Map_LoadTextures: failed to open animation description (%s)', [Textures[i].FullName])
                end
                else
                  e_LogWritefln('r_Map_LoadTextures: failed to open animation (%s)', [Textures[i].FullName]);
                WADz.Free
              end
              else
              begin
                if e_CreateTextureMem(ResData, ResLen, RenTextures[i].ID) then
                  e_GetTextureSize(RenTextures[i].ID, @RenTextures[i].Width, @RenTextures[i].Height)
                else
                  e_LogWritefln('r_Map_LoadTextures: failed to create texture (%s)', [Textures[i].FullName])
              end;
              FreeMem(ResData);
            end
            else
              e_LogWritefln('r_Map_LoadTextures: failed to open (%s)', [Textures[i].FullName])
          end
          else
            e_LogWritefln('r_Map_LoadTextures: failed to open %s', [WadName]);
          WAD.Free;
        end
      end
    end
  end;

procedure dplClear ();
begin
  if (gDrawPanelList = nil) then gDrawPanelList := TBinHeapPanelDraw.Create() else gDrawPanelList.clear();
end;

// old algo
procedure r_Map_DrawPanels (PanelType: Word; hasAmbient: Boolean; constref ambColor: TDFColor);

  procedure DrawPanels (constref panels: TPanelArray; drawDoors: Boolean=False);
  var
    idx: Integer;
  begin
    if (panels <> nil) then
    begin
      // alas, no visible set
      for idx := 0 to High(panels) do
      begin
        if not (drawDoors xor panels[idx].Door) then
          r_Panel_Draw(panels[idx], hasAmbient, ambColor);
      end;
    end;
  end;

begin
  case PanelType of
    PANEL_WALL:       DrawPanels(gWalls);
    PANEL_CLOSEDOOR:  DrawPanels(gWalls, True);
    PANEL_BACK:       DrawPanels(gRenderBackgrounds);
    PANEL_FORE:       DrawPanels(gRenderForegrounds);
    PANEL_WATER:      DrawPanels(gWater);
    PANEL_ACID1:      DrawPanels(gAcid1);
    PANEL_ACID2:      DrawPanels(gAcid2);
    PANEL_STEP:       DrawPanels(gSteps);
  end;
end;

// new algo
procedure r_Map_CollectDrawPanels (x0, y0, wdt, hgt: Integer);
var
  mwit: PPanel;
  it: TPanelGrid.Iter;
begin
  dplClear();
  it := mapGrid.forEachInAABB(x0, y0, wdt, hgt, GridDrawableMask);
  for mwit in it do if (((mwit^.tag and GridTagDoor) <> 0) = mwit^.Door) then gDrawPanelList.insert(mwit^);
  it.release();
  // list will be rendered in `g_game.DrawPlayer()`
end;

procedure r_Map_DrawPanelShadowVolumes (lightX: Integer; lightY: Integer; radius: Integer);
var
  mwit: PPanel;
  it: TPanelGrid.Iter;
begin
  it := mapGrid.forEachInAABB(lightX-radius, lightY-radius, radius*2, radius*2, (GridTagWall or GridTagDoor));
  for mwit in it do r_Panel_DrawShadowVolume(mwit^, lightX, lightY, radius);
  it.release();
end;

procedure r_Map_DrawBack(dx, dy: Integer);
begin
  if gDrawBackGround and (BackID <> DWORD(-1)) then
    e_DrawSize(BackID, dx, dy, 0, False, False, gBackSize.X, gBackSize.Y)
  else
    e_Clear(GL_COLOR_BUFFER_BIT, 0, 0, 0);
end;

procedure r_Map_DrawFlags();
var
  i, dx: Integer;
  Mirror: TMirrorType;
begin
  if gGameSettings.GameMode <> GM_CTF then
    Exit;

  for i := FLAG_RED to FLAG_BLUE do
    with gFlags[i] do
      if State <> FLAG_STATE_CAPTURED then
      begin
        if State = FLAG_STATE_NONE then
          continue;

        if Direction = TDirection.D_LEFT then
          begin
            Mirror := TMirrorType.Horizontal;
            dx := -1;
          end
        else
          begin
            Mirror := TMirrorType.None;
            dx := 1;
          end;

        r_Animation_Draw(Animation, Obj.X + dx, Obj.Y + 1, Mirror);

        if g_debug_Frames then
        begin
          e_DrawQuad(Obj.X+Obj.Rect.X,
                     Obj.Y+Obj.Rect.Y,
                     Obj.X+Obj.Rect.X+Obj.Rect.Width-1,
                     Obj.Y+Obj.Rect.Y+Obj.Rect.Height-1,
                     0, 255, 0);
        end;
      end;
end;

  procedure r_Panel_Draw (constref p: TPanel; hasAmbient: Boolean; constref ambColor: TDFColor);
    var xx, yy: Integer; NoTextureID, TextureID, FramesID: DWORD; NW, NH: Word; Texture: Cardinal; IsAnim: Boolean; w, h: Integer;
  begin
    if {p.Enabled and} (p.FCurTexture >= 0) and (p.Width > 0) and (p.Height > 0) and (p.Alpha < 255) {and g_Collide(X, Y, Width, Height, sX, sY, sWidth, sHeight)} then
    begin
      Texture := p.TextureIDs[p.FCurTexture].Texture;
      IsAnim := RenTextures[Texture].Anim;
      if IsAnim then
      begin
        if p.TextureIDs[p.FCurTexture].AnTex <> nil then
        begin
          FramesID := RenTextures[Texture].ID;
          w := RenTextures[Texture].Width;
          h := RenTextures[Texture].Height;
          for xx := 0 to p.Width div w - 1 do
            for yy := 0 to p.Height div h - 1 do
              r_AnimationState_Draw(FramesID, p.TextureIDs[p.FCurTexture].AnTex, p.X + xx * w, p.Y + yy * h, TMirrorType.None);
        end
      end
      else
      begin
        TextureID := RenTextures[Texture].ID;
        w := RenTextures[Texture].Width;
        h := RenTextures[Texture].Height;
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
            e_DrawFill(TextureID, p.X, p.Y, p.Width div w, p.Height div h, p.Alpha, True, p.Blending, hasAmbient)
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
      Texture := p.TextureIDs[p.FCurTexture].Texture;
      if not RenTextures[Texture].Anim then
      begin
        case RenTextures[Texture].ID of
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
