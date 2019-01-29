implementation

  uses Allegro, Math, SDL2, e_Log;

  const
    GL_INVALID_ENUM = $0500;

  const
    ValPerVertex = 2;
    ValPerColor = 1; (* colors stored in one integer *)
    ValPerCoord = 2;
    StackSize = 16;

  type
    TArrayFloat = array of GLfloat;
    TArrayInteger = array of Integer;

    TCmds = record
      mode: GLenum;
      v: TArrayInteger;
      c: TArrayInteger;
      t: TArrayFloat;
    end;

    TArrayTexture = array of record
      used: Boolean;
      bmp: PBITMAP;
    end;

  var
    cmds: TCmds;
    tex: TArrayTexture;
    ctex: Integer;
    ccol: Integer;
    clearColor: cint;
    stack: array [0..StackSize - 1] of record
      x, y: Integer;
    end;
    stack_ptr: Integer;

  function AddTexture: Integer;
    var i: Integer;
  begin
    for i := 0 to High(tex) do
      if not tex[i].used then
      begin
        tex[i].used := true;
        tex[i].bmp := nil;
        result := i;
        exit
      end;
    i :=  Length(tex);
    SetLength(tex, i + 1);
    tex[i].used := true;
    tex[i].bmp := nil;
    result := i
  end;

  procedure RemoveTexture(i: Integer);
  begin
    assert(i >= 0);
    assert(i <= High(tex));
    assert(tex[i].used);
    tex[i].used := false;
    if tex[i].bmp <> nil then
      destroy_bitmap(tex[i].bmp);
    tex[i].bmp := nil
  end;

  procedure Addi (var x: TArrayInteger; f: Integer);
    var i: Integer;
  begin
    i := Length(x);
    SetLength(x, i + 1);
    x[i] := f;
  end;

  procedure Addf (var x: TArrayFloat; f: GLfloat);
    var i: Integer;
  begin
    i := Length(x);
    SetLength(x, i + 1);
    x[i] := f;
  end;

  (** Open GL **)

  procedure glEnable(cap: GLenum);
  begin
  end;

  procedure glDisable(cap: GLenum);
  begin
  end;

  function glIsEnabled(cap: GLenum): GLboolean;
  begin
    result := 0
  end;

  function glGetString(name: GLenum): PChar;
  begin
    result := ''
  end;

  procedure glClearColor(red, green, blue, alpha: GLclampf);
  begin
    clearColor := makeacol(floor(red * 255), floor(green * 255), floor(blue * 255), floor(alpha * 255));
  end;

  procedure glClear(mask: GLbitfield);
  begin
    if (mask and GL_COLOR_BUFFER_BIT) <> 0 then
      clear_to_color(sdl2allegro_screen, clearColor)
  end;

  procedure glAlphaFunc(func: GLenum; ref: GLclampf);
  begin
  end;

  procedure glBlendFunc(sfactor, dfactor: GLenum);
  begin
  end;

  procedure glPointSize(size: GLfloat);
  begin
  end;

  procedure glLineWidth(width: GLfloat);
  begin
  end;

  procedure glGetIntegerv(pname: GLenum; params: PGLint);
  begin
    params^ := 0
  end;

  procedure glFlush;
  begin
  end;

  procedure glFinish;
  begin
  end;

  procedure glBegin(mode: GLenum);
  begin
    assert(cmds.mode = GL_INVALID_ENUM);
    assert((mode = GL_POINTS) or (mode = GL_LINES) or (mode = GL_QUADS));
    cmds.mode := mode;
    SetLength(cmds.v, 0);
    SetLength(cmds.c, 0);
    SetLength(cmds.t, 0);
  end;

  procedure glEnd;
    var
      i, j, k, w, h, x0, y0, x1, y1, offx, offy, tmp, s0, t0, s1, t1: Integer;
      oldx0, oldy0, oldx1, oldy1: cint;
      flipv, fliph: Boolean;
      draw_sprite_proc: procedure (bmp, sprite: Allegro.PBITMAP; x, y: cint); cdecl;
  begin
    assert(cmds.mode <> GL_INVALID_ENUM);
    assert(Length(cmds.v) mod ValPerVertex = 0);
    assert(Length(cmds.c) mod ValPerColor = 0);
    assert(Length(cmds.t) mod ValPerCoord = 0);

    offx := stack[stack_ptr].x;
    offy := stack[stack_ptr].y;

    case cmds.mode of
    GL_POINTS:
      begin
         (* implement case for texture coords? *)
         if Length(cmds.c) <> 0 then
         begin
           assert(Length(cmds.c) * 2 = Length(cmds.v)); (* not enough colors *)
           for i := 0 to Length(cmds.v) div 2 - 1 do
             putpixel(sdl2allegro_screen, offx + cmds.v[i * 2], offy + cmds.v[i * 2 + 1], cmds.c[i])
         end
         else
         begin
           for i := 0 to Length(cmds.v) div 2 - 1 do
             putpixel(sdl2allegro_screen, offx + cmds.v[i * 2], offy + cmds.v[i * 2 + 1], ccol)
         end
      end;
    GL_LINES:
      begin
        assert(Length(cmds.v) mod 4 = 0); (* broken line *)
        (* implement case for texture coords? *)
        if Length(cmds.c) <> 0 then
        begin
          assert(Length(cmds.c) * 2 = Length(cmds.v));
          for i := 0 to Length(cmds.v) div 4 - 1 do
            fastline(sdl2allegro_screen, offx + cmds.v[i * 4], offy + cmds.v[i * 4 + 1], offx + cmds.v[i * 4 + 2], offy + cmds.v[i * 4 + 3], cmds.c[i * 2])
        end
        else
        begin
          for i := 0 to Length(cmds.v) div 4 - 1 do
            fastline(sdl2allegro_screen, offx + cmds.v[i * 4], offy + cmds.v[i * 4 + 1], offx + cmds.v[i * 4 + 2], offy + cmds.v[i * 4 + 3], ccol)
        end
      end;
    GL_QUADS:
      begin
        ASSERT(Length(cmds.v) mod 8 = 0); (* broken quad *)
        if Length(cmds.t) <> 0 then
        begin
          ASSERT(Length(cmds.t) = Length(cmds.v)); (* not enough texture coords *)
          ASSERT(ctex >= 0);
          ASSERT(ctex <= High(tex));
          ASSERT(tex[ctex].bmp <> nil);
          for i := 0 to Length(cmds.v) div 8 - 1 do
          begin
            flipv := False; fliph := False;
            x0 := cmds.v[i * 8 + 0]; y0 := cmds.v[i * 8 + 1];
            x1 := cmds.v[i * 8 + 4]; y1 := cmds.v[i * 8 + 5];
            if x1 < x0 then
            begin
              tmp := x0;
              x0 := x1;
              x1 := tmp;
              fliph := not fliph
            end;
            if y1 < y0 then
            begin
              tmp := y0;
              y0 := y1;
              y1 := tmp;
              flipv := not flipv
            end;

            w := tex[ctex].bmp.w;
            h := tex[ctex].bmp.h;
            s0 := Trunc(cmds.t[i * 8 + 0] * w);
            t0 := Trunc(cmds.t[i * 8 + 1] * h);
            s1 := Trunc(cmds.t[i * 8 + 4] * w);
            t1 := Trunc(cmds.t[i * 8 + 5] * h);

            if s1 < s0 then
            begin
              tmp := s0;
              s0 := s1;
              s1 := tmp;
              fliph := not fliph
            end;
            if t1 < t0 then
            begin
              tmp := t0;
              t0 := t1;
              t1 := tmp;
              flipv := not flipv
            end;

            s0 := s0 mod w;
            t0 := t0 mod h;
            s1 := s1 mod w;
            t1 := t1 mod h;

            if flipv and fliph then
              draw_sprite_proc := Allegro.draw_sprite_vh_flip
            else if flipv then
              draw_sprite_proc := Allegro.draw_sprite_v_flip
            else if fliph then
              draw_sprite_proc := Allegro.draw_sprite_h_flip
            else
              draw_sprite_proc := Allegro.draw_sprite;

            oldx0 := 0; oldy0 := 0; oldx1 := 0; oldy1 := 0;
            get_clip_rect(sdl2allegro_screen, oldx0, oldy0, oldx1, oldy1);
            set_clip_rect(sdl2allegro_screen, max(oldx0, offx + x0), max(oldy0, offy + y0), min(oldx1, offx + x1), min(oldy1, offy + y1));

            for j := 0 to (y1 - y0 + h - 1) div h - 1 do
              for k := 0 to (x1 - x0 + w - 1) div w - 1 do
                draw_sprite_proc(sdl2allegro_screen, tex[ctex].bmp, offx + x0 + k * w - s0, offy + y0 + j * h - t0);
                //blit(tex[ctex].bmp, sdl2allegro_screen, 0, 0, offx + x0 + k * w - s0, offy + y0 + j * h - t0, w, h);

            set_clip_rect(sdl2allegro_screen, oldx0, oldy0, oldx1, oldy1);

            //rect(sdl2allegro_screen, offx + x0, offy + y0, offx + x1, offy + y1, makecol(255, 0, 0));
            //rect(sdl2allegro_screen, offx + oldx0, offy + oldy0, offx + oldx1, offy + oldx1, makecol(0, 255, 0));
          end
        end
        else if Length(cmds.c) <> 0 then
        begin
          assert(Length(cmds.c) * 2 = Length(cmds.v)); (* not enough colors *)
          for i := 0 to Length(cmds.v) div 8 - 1 do
            rectfill(sdl2allegro_screen, offx + cmds.v[i * 8], offy + cmds.v[i * 8 + 1], offx + cmds.v[i * 8 + 4], offy + cmds.v[i * 8 + 5], cmds.c[i * 4])
        end
        else
        begin
          for i := 0 to Length(cmds.v) div 8 - 1 do
            rectfill(sdl2allegro_screen, offx + cmds.v[i * 8], offy + cmds.v[i * 8 + 1], offx + cmds.v[i * 8 + 4], offy + cmds.v[i * 8 + 5], ccol)
        end
      end;
    else
      assert(false)
    end;

    SetLength(cmds.v, 0);
    SetLength(cmds.c, 0);
    SetLength(cmds.t, 0);
    cmds.mode := GL_INVALID_ENUM;
  end;

  procedure glVertex2f(x, y: GLfloat);
  begin
    Addi(cmds.v, ceil(x));
    Addi(cmds.v, ceil(y))
  end;

  procedure glVertex2i(x, y: GLint);
  begin
    Addi(cmds.v, x);
    Addi(cmds.v, y)
  end;

  procedure glColor4f(red, green, blue, alpha: GLfloat);
  begin
    ccol := makeacol(floor(red * 255), floor(green * 255), floor(blue * 255), floor(alpha * 255));
    Addi(cmds.c, ccol)
  end;

  procedure glColor4ub(red, green, blue, alpha: GLubyte);
  begin
    ccol := makeacol(red, green, blue, alpha);
    Addi(cmds.c, ccol)
  end;

  procedure glColor3ub(red, green, blue: GLubyte);
  begin
    ccol := makecol(red, green, blue);
    Addi(cmds.c, ccol)
  end;

  procedure glTexCoord2f(s, t: GLfloat);
  begin
    Addf(cmds.t, s);
    Addf(cmds.t, t);
  end;

  procedure glTexCoord2i(s, t: GLint);
  begin
    Addf(cmds.t, s);
    Addf(cmds.t, t);
  end;

  procedure glReadPixels(x, y: GLint; width, height: GLsizei; format, atype: GLenum; pixels: Pointer);
  begin
  end;

  procedure glLoadIdentity;
  begin
    with stack[stack_ptr] do
    begin
      x := 0;
      y := 0;
    end
  end;

  procedure glMatrixMode(mode: GLenum);
  begin
  end;

  procedure glLoadMatrixd(const m: PGLdouble);
  begin
    //m[x,y]
  end;

  procedure glPushMatrix;
  begin
    stack[stack_ptr + 1] := stack[stack_ptr];
    INC(stack_ptr);
  end;

  procedure glPopMatrix;
  begin
    DEC(stack_ptr)
  end;

  procedure glTranslatef(x, y, z: GLfloat);
  begin
    ASSERT(z = 0); (* 3D not supported *)
    stack[stack_ptr].x := Trunc(x);
    stack[stack_ptr].y := Trunc(y);
  end;

  procedure glRotatef(angle, x, y, z: GLfloat);
  begin
    //ASSERT(z = 0); (* 3D not supported *)
    (* TODO Rotation *)
  end;

  procedure glScalef(x, y, z: GLfloat);
  begin
    //ASSERT(z = 1); (* 3D not supported *)
    (* TODO Scale *)
  end;

  procedure glViewport(x, y: GLint; width, height: GLsizei);
  begin
  end;

  procedure glScissor(x, y: GLint; width, height: GLsizei);
  begin
  end;

  procedure glStencilMask(mask: GLuint);
  begin
  end;

  procedure glStencilFunc(func: GLenum; ref: GLint; mask: GLuint);
  begin
  end;

  procedure glStencilOp(fail, zfail, zpass: GLenum);
  begin
  end;

  procedure glColorMask(red, green, blue, alpha: GLboolean);
  begin
  end;

  procedure glBindTexture(target: GLenum; texture: GLuint);
  begin
    assert(target = GL_TEXTURE_2D);
    ctex := texture;
  end;

  procedure glGenTextures(n: GLsizei; textures: PGLuint);
    var i: Integer;
  begin
    for i := 0 to n - 1 do
      textures[i] := AddTexture
  end;

  procedure glTexEnvi(target: GLenum; pname: GLenum; param: GLint);
  begin
  end;

  procedure glTexParameterf(target: GLenum; pname: GLenum; param: GLfloat);
  begin
  end;

  procedure glTexParameteri(target: GLenum; pname: GLenum; param: GLint);
  begin
  end;

  procedure glTexImage2D(target: GLenum; level, internalformat: GLint; width, height: GLsizei; border: GLint; format, atype: GLenum; const pixels: Pointer);
    var i, j, adr: Integer; p: PByte; color: cint;
  begin
    assert(target = GL_TEXTURE_2D);
    assert(level = 0);
    assert((internalformat = GL_RGBA) or (internalformat = GL_RGB));
    assert((format = GL_RGBA) or (format = GL_RGB));
    assert(border = 0);
    assert(atype = GL_UNSIGNED_BYTE);

    assert(ctex >= 0);
    assert(ctex <= High(tex));
    assert(tex[ctex].used);

    if tex[ctex].bmp <> nil then
      destroy_bitmap(tex[ctex].bmp);
    // Video bitmap can lead to bad textures under dos
    //tex[ctex].bmp := create_video_bitmap(width, height);
    //if tex[ctex].bmp = nil then
      tex[ctex].bmp := create_system_bitmap(width, height);
    if tex[ctex].bmp = nil then
      tex[ctex].bmp := create_bitmap(width, height);
    assert(tex[ctex].bmp <> nil);

    if pixels = nil then exit;

    p := pixels;
    if format = GL_RGBA then
    begin
      for j := 0 to height - 1 do
        for i := 0 to width - 1 do
        begin
          adr := j * width * 4 + i * 4;
          if p[adr + 3] <> $FF then
            color := 0
          else
            color := makeacol(p[adr], p[adr + 1], p[adr + 2], p[adr + 3]);
          putpixel(tex[ctex].bmp, i, j, color)
        end
    end
    else
    begin
      for j := 0 to height - 1 do
        for i := 0 to width - 1 do
        begin
          adr := j * width * 3 + i * 3;
          putpixel(tex[ctex].bmp, i, j, makecol(p[adr], p[adr + 1], p[adr + 2]))
        end
    end
  end;

  procedure glTexSubImage2D(target: GLenum; level, xoffset, yoffset: GLint; width, height: GLsizei; format, atype: GLenum; const pixels: Pointer);
    var i, j, adr: Integer; p: PByte; color: Cint;
  begin
    assert(target = GL_TEXTURE_2D);
    assert(level = 0);
    assert((format = GL_RGBA) or (format = GL_RGB));
    assert(atype = GL_UNSIGNED_BYTE);

    assert(ctex >= 0);
    assert(ctex <= High(tex));
    assert(tex[ctex].used);
    
    assert(xoffset = 0);
    assert(yoffset = 0);

    if pixels = nil then exit;

    p := pixels;
    if format = GL_RGBA then
    begin
      for j := 0 to height - 1 do
        for i := 0 to width - 1 do
        begin
          adr := j * width * 4 + i * 4;
          if p[adr + 3] <> $FF then
            color := 0
          else
            color := makeacol(p[adr], p[adr + 1], p[adr + 2], p[adr + 3]);
          putpixel(tex[ctex].bmp, i, j, color)
        end
    end
    else
    begin
      for j := 0 to height - 1 do
        for i := 0 to width - 1 do
        begin
          adr := j * width * 3 + i * 3;
          putpixel(tex[ctex].bmp, i, j, makecol(p[adr], p[adr + 1], p[adr + 2]))
        end
    end
  end;

  procedure glDeleteTextures(n: GLsizei; const textures: PGLuint);
    var i: Integer;
  begin
    for i := 0 to n - 1 do
      RemoveTexture(textures[i])
  end;

  procedure nogl_Init;
  begin
    cmds.mode := GL_INVALID_ENUM
  end;

  procedure nogl_Quit;
  begin
  end;

initialization
