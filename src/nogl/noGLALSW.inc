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
      x, y, a: Integer;
    end;
    stack_ptr: Integer;
    vpx, vpy: Integer;
    pointSize: Integer;
    matrixMode: GLenum;

  function AddTexture: Integer;
    var i: Integer;
  begin
    for i := 1 to High(tex) do
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
    assert((i = 0) or tex[i].used); (* free unallocated texture *)
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
    if name = GL_EXTENSIONS then (* separated by space *)
      result := 'GL_ARB_texture_non_power_of_two'
    else
      result := nil
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
    ASSERT(size >= 0);
    if size <= 1.0 then pointSize := ceil(size)
    else pointSize := floor(size)
  end;

  procedure glLineWidth(width: GLfloat);
  begin
    (* width > 1 used in rare cases, not critical *)
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
      i, j, k, w, h, x0, y0, x1, y1, offx, offy, tmp, s0, t0, s1, t1, angle: Integer;
      oldx0, oldy0, oldx1, oldy1: cint;
      flipv, fliph: Boolean;
      draw_sprite_proc: procedure (bmp, sprite: Allegro.PBITMAP; x, y: cint); cdecl;
      rotate_sprite_proc: procedure (bmp, sprite: Allegro.PBITMAP; x, y: cint; a: cint32); cdecl;
  begin
    assert(cmds.mode <> GL_INVALID_ENUM);
    assert(Length(cmds.v) mod ValPerVertex = 0);
    assert(Length(cmds.c) mod ValPerColor = 0);
    assert(Length(cmds.t) mod ValPerCoord = 0);

    offx := vpx + stack[stack_ptr].x;
    offy := vpy + stack[stack_ptr].y;
    angle := stack[stack_ptr].a;

    case cmds.mode of
    GL_POINTS:
      begin
        (* implement case for texture coords? *)
        if pointSize = 1 then
        begin
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
        end
        else if pointSize > 1 then
        begin
           x0 := offx - pointSize div 2;
           y0 := offy - pointSize div 2;
           x1 := offx - (pointSize - 1) div 2;
           y1 := offy - (pointSize - 1) div 2;
           if Length(cmds.c) <> 0 then
           begin
             assert(Length(cmds.c) * 2 = Length(cmds.v)); (* not enough colors *)
             for i := 0 to Length(cmds.v) div 2 - 1 do
             begin
               w := cmds.v[i * 2 + 0];
               h := cmds.v[i * 2 + 1];
               rectfill(sdl2allegro_screen, x0 + w, y0 + h, x1 + w, y1 + h, cmds.c[i])
             end
           end
           else
           begin
             for i := 0 to Length(cmds.v) div 2 - 1 do
             begin
               w := cmds.v[i * 2 + 0];
               h := cmds.v[i * 2 + 1];
               rectfill(sdl2allegro_screen, x0 + w, y0 + h, x1 + w, y1 + h, ccol)
             end
           end
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
              fliph := not fliph;
            end;
            if t1 < t0 then
            begin
              tmp := t0;
              t0 := t1;
              t1 := tmp;
              flipv := not flipv;
            end;

            if fliph then
            begin
              tmp := s0;
              s0 := w - s1;
              s1 := w - tmp;
            end;
            if flipv then
            begin
              tmp := t0;
              t0 := h - t1;
              t1 := h - tmp;
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

            if flipv and fliph then
              rotate_sprite_proc := Allegro.rotate_sprite_v_flip (* ??? *)
            else if flipv then
              rotate_sprite_proc := Allegro.rotate_sprite_v_flip
            else if fliph then
              rotate_sprite_proc := Allegro.rotate_sprite (* ??? *)
            else
              rotate_sprite_proc := Allegro.rotate_sprite;

            oldx0 := 0; oldy0 := 0; oldx1 := 0; oldy1 := 0;
            get_clip_rect(sdl2allegro_screen, oldx0, oldy0, oldx1, oldy1);
            set_clip_rect(sdl2allegro_screen, max(oldx0, offx + x0), max(oldy0, offy + y0), min(oldx1, offx + x1), min(oldy1, offy + y1));

            if angle = 0 then
              for j := 0 to (y1 - y0 + h - 1) div h - 1 do
                for k := 0 to (x1 - x0 + w - 1) div w - 1 do
                  draw_sprite_proc(sdl2allegro_screen, tex[ctex].bmp, offx + x0 + k * w - s0, offy + y0 + j * h - t0)
            else
              for j := 0 to (y1 - y0 + h - 1) div h - 1 do
                for k := 0 to (x1 - x0 + w - 1) div w - 1 do
                  rotate_sprite_proc(sdl2allegro_screen, tex[ctex].bmp, offx + x0 + k * w - s0, offy + y0 + j * h - t0, angle);

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
    if matrixMode <> GL_MODELVIEW then Exit;
    with stack[stack_ptr] do
    begin
      x := 0;
      y := 0;
      (* TODO Rotation and scale *)
    end
  end;

  procedure glMatrixMode(mode: GLenum);
  begin
    (* GL_PROJECTION -> verify or ignore *)
    (* GL_MODELVIEW -> apply *)
    ASSERT((mode = GL_PROJECTION) or (mode = GL_MODELVIEW));
    matrixMode := mode;
  end;

  procedure glLoadMatrixd(const m: PGLdouble);
  begin
    if matrixMode <> GL_MODELVIEW then Exit;

    (*
    e_LogWritefln('glLoadMatrix:', []);
    e_LogWritefln('| %s %s %s %s |', [m[0], m[1], m[2], m[3]]);
    e_LogWritefln('| %s %s %s %s |', [m[4], m[5], m[6], m[7]]);
    e_LogWritefln('| %s %s %s %s |', [m[8], m[9], m[10], m[11]]);
    e_LogWritefln('| %s %s %s %s |', [m[12], m[13], m[14], m[15]]);
    *)
    with stack[stack_ptr] do
    begin
      x := Trunc(m[3]);
      y := Trunc(m[7]);
      ASSERT(m[11] = 0);
      (* TODO Rotation and Scale *)
    end
  end;

  procedure glPushMatrix;
  begin
    if matrixMode <> GL_MODELVIEW then Exit;
    stack[stack_ptr + 1] := stack[stack_ptr];
    INC(stack_ptr);
  end;

  procedure glPopMatrix;
  begin
    if matrixMode <> GL_MODELVIEW then Exit;
    DEC(stack_ptr)
  end;

  procedure glTranslatef(x, y, z: GLfloat);
  begin
    if matrixMode <> GL_MODELVIEW then Exit;
    ASSERT(z = 0); (* 3D not supported *)
    stack[stack_ptr].x += Trunc(x);
    stack[stack_ptr].y += Trunc(y);
  end;

  procedure glRotatef(angle, x, y, z: GLfloat);
  begin
    if matrixMode <> GL_MODELVIEW then Exit;
    ASSERT(x = 0); (* 3D not supported *)
    ASSERT(y = 0); (* 3D not supported *)
    // angle 360deg == 256 with conversion to fixed point 16.16
    stack[stack_ptr].a += floor(angle * z * 0.71111) * 65536
  end;

  procedure glScalef(x, y, z: GLfloat);
  begin
    if matrixMode <> GL_MODELVIEW then Exit;
    (* 3D not supported, but z can be any *)
    (* TODO Scale *)
  end;

  procedure glViewport(x, y: GLint; width, height: GLsizei);
  begin
    vpx := x; vpy := y;
    set_clip_rect(sdl2allegro_screen, x, y, x + width, y + height);
  end;

  procedure glScissor(x, y: GLint; width, height: GLsizei);
  begin
    //set_clip_rect(sdl2allegro_screen, x, y, width, height)
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
    var i, j, adr: Integer; p: PByte; color, trans: cint;
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
    tex[ctex].bmp := create_system_bitmap(width, height);
    if tex[ctex].bmp = nil then
      tex[ctex].bmp := create_bitmap(width, height);
    assert(tex[ctex].bmp <> nil);

    if pixels = nil then exit;

    p := pixels;
    if format = GL_RGBA then
    begin
      if DEFAULT_DEPTH <= 8 then
        trans := 0
      else
        trans := makeacol(255, 0, 255, 0);

      for j := 0 to height - 1 do
        for i := 0 to width - 1 do
        begin
          adr := j * width * 4 + i * 4;
          if p[adr + 3] = 0 then
            color := trans
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
    var i, j, adr: Integer; p: PByte; color, trans: Cint;
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
      if DEFAULT_DEPTH <= 8 then
        trans := 0
      else
        trans := makeacol(255, 0, 255, 0);

      for j := 0 to height - 1 do
        for i := 0 to width - 1 do
        begin
          adr := j * width * 4 + i * 4;
          if p[adr + 3] = 0 then
            color := trans
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
