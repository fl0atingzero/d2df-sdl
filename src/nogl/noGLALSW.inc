implementation

  uses Allegro, Math, SDL2, e_Log;

  const
    GL_INVALID_ENUM = $0500;

  const
    ValPerVertex = 2;
    ValPerColor = 4;
    ValPerCoord = 2;

  type
    TArrayFloat = array of Integer;

    TCmds = record
      mode: GLenum;
      v, c, t: TArrayFloat;
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
      destroy_bitmap(tex[i].bmp)
  end;

  procedure Add (var x: TArrayFloat; f: Integer);
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
    var color: Integer;
  begin
    color := makeacol(floor(red * 255), floor(green * 255), floor(blue * 255), floor(alpha * 255));
    clear_to_color(sdl2allegro_screen, color)
  end;

  procedure glClear(mask: GLbitfield);
  begin
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
    var i, x, y, w, h: Integer;
  begin
    assert(cmds.mode <> GL_INVALID_ENUM);
    assert(Length(cmds.v) mod ValPerVertex = 0);
    assert(Length(cmds.c) mod ValPerColor = 0);
    assert(Length(cmds.t) mod ValPerCoord = 0);

    case cmds.mode of
    GL_POINTS:
      begin
         (* implement case for texture coords? *)
         if Length(cmds.c) <> 0 then
         begin
           assert(Length(cmds.c) * 2 = Length(cmds.v)); (* not enough colors *)
           for i := 0 to Length(cmds.v) div 2 - 1 do
             putpixel(sdl2allegro_screen, cmds.v[i * 2], cmds.v[i * 2 + 1], cmds.c[i])
         end
         else
         begin
           for i := 0 to Length(cmds.v) div 2 - 1 do
             putpixel(sdl2allegro_screen, cmds.v[i * 2], cmds.v[i * 2 + 1], ccol)
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
            fastline(sdl2allegro_screen, cmds.v[i * 4], cmds.v[i * 4 + 1], cmds.v[i * 4 + 2], cmds.v[i * 4 + 3], cmds.c[i * 2])
        end
        else
        begin
          for i := 0 to Length(cmds.v) div 4 - 1 do
            fastline(sdl2allegro_screen, cmds.v[i * 4], cmds.v[i * 4 + 1], cmds.v[i * 4 + 2], cmds.v[i * 4 + 3], ccol)
        end
      end;
    GL_QUADS:
      begin
        assert(Length(cmds.v) mod 8 = 0); (* broken quad *)
        if Length(cmds.t) <> 0 then
        begin
          assert(Length(cmds.t) = Length(cmds.v)); (* not enough texture coords *)
          assert(ctex >= 0);
          assert(ctex <= High(tex));
          for i := 0 to Length(cmds.v) div 8 - 1 do
          begin
            x := cmds.v[i * 8];
            y := cmds.v[i * 8 + 1];
            w := abs(cmds.v[i * 4 + 5] - x);
            h := abs(cmds.v[i * 4 + 6] - y);
            //e_LogWriteFLn('Textured Quad %s %s', [w, h]);
            draw_sprite(sdl2allegro_screen, tex[ctex].bmp, x, y);
            //rect(sdl2allegro_screen, x, y, w, h, makecol(255, 0, 0))
          end
        end
        else if Length(cmds.c) <> 0 then
        begin
          assert(Length(cmds.c) * 2 = Length(cmds.v)); (* not enough colors *)
          for i := 0 to Length(cmds.v) div 8 - 1 do
            rectfill(sdl2allegro_screen, cmds.v[i * 8], cmds.v[i * 8 + 1], cmds.v[i * 4 + 5], cmds.v[i * 4 + 6], cmds.c[i * 4])
        end
        else
        begin
          for i := 0 to Length(cmds.v) div 8 - 1 do
            rectfill(sdl2allegro_screen, cmds.v[i * 8], cmds.v[i * 8 + 1], cmds.v[i * 4 + 5], cmds.v[i * 4 + 6], ccol)
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
    Add(cmds.v, ceil(x));
    Add(cmds.v, ceil(y))
  end;

  procedure glVertex2i(x, y: GLint);
  begin
    Add(cmds.v, x);
    Add(cmds.v, y)
  end;

  procedure glColor4f(red, green, blue, alpha: GLfloat);
  begin
    ccol := makeacol(floor(red * 255), floor(green * 255), floor(blue * 255), floor(alpha * 255));
    Add(cmds.c, ccol)
  end;

  procedure glColor4ub(red, green, blue, alpha: GLubyte);
  begin
    ccol := makeacol(red, green, blue, alpha);
    Add(cmds.c, ccol)
  end;

  procedure glColor3ub(red, green, blue: GLubyte);
  begin
    ccol := makecol(red, green, blue);
    Add(cmds.c, ccol)
  end;

  procedure glTexCoord2f(s, t: GLfloat);
  begin
    Add(cmds.t, floor(s));
    Add(cmds.t, floor(t));
  end;

  procedure glTexCoord2i(s, t: GLint);
  begin
    Add(cmds.t, s);
    Add(cmds.t, t);
  end;

  procedure glReadPixels(x, y: GLint; width, height: GLsizei; format, atype: GLenum; pixels: Pointer);
  begin
  end;

  procedure glLoadIdentity;
  begin
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
  end;

  procedure glPopMatrix;
  begin
  end;

  procedure glTranslatef(x, y, z: GLfloat);
  begin
  end;

  procedure glRotatef(angle, x, y, z: GLfloat);
  begin
  end;

  procedure glScalef(x, y, z: GLfloat);
  begin
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
    var i, j, adr: Integer; p: PByte;
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
          putpixel(tex[ctex].bmp, i, height - j, makeacol(p[adr], p[adr + 1], p[adr + 2], p[adr + 3]))
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
    var i, j, adr: Integer; p: PByte;
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
          putpixel(tex[ctex].bmp, i, height - j, makeacol(p[adr], p[adr + 1], p[adr + 2], p[adr + 3]))
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
