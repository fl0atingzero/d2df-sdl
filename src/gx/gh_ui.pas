(* coded by Ketmar // Invisible Vector <ketmar@ketmar.no-ip.org>
 * Understanding is not required. Only obedience.
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *)
{$INCLUDE ../shared/a_modes.inc}
{$M+}
unit gh_ui;

interface

uses
  SysUtils, Classes,
  GL, GLExt, SDL2,
  gh_ui_common,
  sdlcarcass, glgfx,
  xparser;


// ////////////////////////////////////////////////////////////////////////// //
type
  TUIControlClass = class of TUIControl;

  TUIControl = class
  public
    type TActionCB = procedure (me: TUIControl; uinfo: Integer);

  private
    mParent: TUIControl;
    mId: AnsiString;
    mX, mY: Integer;
    mWidth, mHeight: Integer;
    mFrameWidth, mFrameHeight: Integer;
    mEnabled: Boolean;
    mCanFocus: Boolean;
    mChildren: array of TUIControl;
    mFocused: TUIControl; // valid only for top-level controls
    mGrab: TUIControl; // valid only for top-level controls
    mEscClose: Boolean; // valid only for top-level controls
    mEatKeys: Boolean;
    mDrawShadow: Boolean;

  private
    scis: TScissorSave;
    scallowed: Boolean;

  protected
    function getEnabled (): Boolean;
    procedure setEnabled (v: Boolean); inline;

    function getFocused (): Boolean; inline;
    procedure setFocused (v: Boolean); inline;

    function isMyChild (ctl: TUIControl): Boolean;

    function findFirstFocus (): TUIControl;
    function findLastFocus (): TUIControl;

    function findNextFocus (cur: TUIControl): TUIControl;
    function findPrevFocus (cur: TUIControl): TUIControl;

    procedure activated (); virtual;
    procedure blurred (); virtual;

    //WARNING! do not call scissor functions outside `.draw*()` API!
    // set scissor to this rect (in local coords)
    procedure setScissor (lx, ly, lw, lh: Integer);
    // reset scissor to whole control
    procedure resetScissor (fullArea: Boolean); inline; // "full area" means "with frame"

    // DO NOT USE!
    // set scissor to this rect (in global coords)
    procedure setScissorGLInternal (x, y, w, h: Integer);

  public
    actionCB: TActionCB;

  private
    mDefSize: TLaySize; // default size
    mMaxSize: TLaySize; // maximum size
    mFlex: Integer;
    mHoriz: Boolean;
    mCanWrap: Boolean;
    mLineStart: Boolean;
    mHGroup: AnsiString;
    mVGroup: AnsiString;
    mAlign: Integer;
    mExpand: Boolean;
    mLayDefSize: TLaySize;
    mLayMaxSize: TLaySize;

  public
    // layouter interface
    function getDefSize (): TLaySize; inline; // default size; <0: use max size
    //procedure setDefSize (const sz: TLaySize); inline; // default size; <0: use max size
    function getMargins (): TLayMargins; inline;
    function getMaxSize (): TLaySize; inline; // max size; <0: set to some huge value
    //procedure setMaxSize (const sz: TLaySize); inline; // max size; <0: set to some huge value
    function getFlex (): Integer; inline; // <=0: not flexible
    function isHorizBox (): Boolean; inline; // horizontal layout for children?
    procedure setHorizBox (v: Boolean); inline; // horizontal layout for children?
    function canWrap (): Boolean; inline; // for horizontal boxes: can wrap children? for child: `false` means 'nonbreakable at *next* ctl'
    procedure setCanWrap (v: Boolean); inline; // for horizontal boxes: can wrap children? for child: `false` means 'nonbreakable at *next* ctl'
    function isLineStart (): Boolean; inline; // `true` if this ctl should start a new line; ignored for vertical boxes
    procedure setLineStart (v: Boolean); inline; // `true` if this ctl should start a new line; ignored for vertical boxes
    function getAlign (): Integer; inline; // aligning in non-main direction: <0: left/up; 0: center; >0: right/down
    procedure setAlign (v: Integer); inline; // aligning in non-main direction: <0: left/up; 0: center; >0: right/down
    function getExpand (): Boolean; inline; // expanding in non-main direction: `true` will ignore align and eat all available space
    procedure setExpand (v: Boolean); inline; // expanding in non-main direction: `true` will ignore align and eat all available space
    function getHGroup (): AnsiString; inline; // empty: not grouped
    procedure setHGroup (const v: AnsiString); inline; // empty: not grouped
    function getVGroup (): AnsiString; inline; // empty: not grouped
    procedure setVGroup (const v: AnsiString); inline; // empty: not grouped

    procedure setActualSizePos (constref apos: TLayPos; constref asize: TLaySize); inline;

    procedure layPrepare (); virtual; // called before registering control in layouter

  public
    property flex: Integer read mFlex write mFlex;
    property flDefaultSize: TLaySize read mDefSize write mDefSize;
    property flMaxSize: TLaySize read mMaxSize write mMaxSize;
    property flHoriz: Boolean read isHorizBox write setHorizBox;
    property flCanWrap: Boolean read canWrap write setCanWrap;
    property flLineStart: Boolean read isLineStart write setLineStart;
    property flAlign: Integer read getAlign write setAlign;
    property flExpand: Boolean read getExpand write setExpand;
    property flHGroup: AnsiString read getHGroup write setHGroup;
    property flVGroup: AnsiString read getVGroup write setVGroup;

  protected
    function parsePos (par: TTextParser): TLayPos;
    function parseSize (par: TTextParser): TLaySize;
    function parseBool (par: TTextParser): Boolean;
    function parseAnyAlign (par: TTextParser): Integer;
    function parseHAlign (par: TTextParser): Integer;
    function parseVAlign (par: TTextParser): Integer;
    function parseOrientation (const prname: AnsiString; par: TTextParser): Boolean;
    procedure parseTextAlign (par: TTextParser; var h, v: Integer);
    procedure parseChildren (par: TTextParser); // par should be on '{'; final '}' is eaten

  public
    // par is on property data
    // there may be more data in text stream, don't eat it!
    // return `true` if property name is valid and value was parsed
    // return `false` if property name is invalid; don't advance parser in this case
    // throw on property data errors
    function parseProperty (const prname: AnsiString; par: TTextParser): Boolean; virtual;

    // par should be on '{'; final '}' is eaten
    procedure parseProperties (par: TTextParser);

  public
    constructor Create ();
    constructor Create (ax, ay, aw, ah: Integer);
    destructor Destroy (); override;

    // `sx` and `sy` are screen coordinates
    procedure drawControl (gx, gy: Integer); virtual;

    // called after all children drawn
    procedure drawControlPost (gx, gy: Integer); virtual;

    procedure draw (); virtual;

    function topLevel (): TUIControl; inline;

    // returns `true` if global coords are inside this control
    function toLocal (var x, y: Integer): Boolean;
    function toLocal (gx, gy: Integer; out x, y: Integer): Boolean; inline;
    procedure toGlobal (var x, y: Integer);
    procedure toGlobal (lx, ly: Integer; out x, y: Integer); inline;

    // x and y are global coords
    function controlAtXY (x, y: Integer): TUIControl;

    function mouseEvent (var ev: THMouseEvent): Boolean; virtual; // returns `true` if event was eaten
    function keyEvent (var ev: THKeyEvent): Boolean; virtual; // returns `true` if event was eaten

    function prevSibling (): TUIControl;
    function nextSibling (): TUIControl;
    function firstChild (): TUIControl; inline;
    function lastChild (): TUIControl; inline;

    procedure appendChild (ctl: TUIControl); virtual;

  public
    property id: AnsiString read mId;
    property x0: Integer read mX;
    property y0: Integer read mY;
    property height: Integer read mHeight;
    property width: Integer read mWidth;
    property enabled: Boolean read getEnabled write setEnabled;
    property parent: TUIControl read mParent;
    property focused: Boolean read getFocused write setFocused;
    property escClose: Boolean read mEscClose write mEscClose;
    property eatKeys: Boolean read mEatKeys write mEatKeys;
  end;


  TUITopWindow = class(TUIControl)
  private
    mTitle: AnsiString;
    mDragging: Boolean;
    mDragStartX, mDragStartY: Integer;
    mWaitingClose: Boolean;
    mInClose: Boolean;
    mFreeOnClose: Boolean; // default: false
    mDoCenter: Boolean; // after layouting

  protected
    procedure activated (); override;
    procedure blurred (); override;

  public
    closeCB: TActionCB; // called after window was removed from ui window list

  public
    constructor Create (const atitle: AnsiString; ax, ay: Integer; aw: Integer=-1; ah: Integer=-1);

    function parseProperty (const prname: AnsiString; par: TTextParser): Boolean; override;

    procedure centerInScreen ();

    // `sx` and `sy` are screen coordinates
    procedure drawControl (gx, gy: Integer); override;
    procedure drawControlPost (gx, gy: Integer); override;

    function keyEvent (var ev: THKeyEvent): Boolean; override; // returns `true` if event was eaten
    function mouseEvent (var ev: THMouseEvent): Boolean; override; // returns `true` if event was eaten

  public
    property freeOnClose: Boolean read mFreeOnClose write mFreeOnClose;
  end;


  TUISimpleText = class(TUIControl)
  private
    type
      PItem = ^TItem;
      TItem = record
        title: AnsiString;
        centered: Boolean;
        hline: Boolean;
      end;
  private
    mItems: array of TItem;

  public
    constructor Create (ax, ay: Integer);
    destructor Destroy (); override;

    procedure appendItem (const atext: AnsiString; acentered: Boolean=false; ahline: Boolean=false);

    procedure drawControl (gx, gy: Integer); override;

    function mouseEvent (var ev: THMouseEvent): Boolean; override;
    function keyEvent (var ev: THKeyEvent): Boolean; override;
  end;


  TUICBListBox = class(TUIControl)
  private
    type
      PItem = ^TItem;
      TItem = record
        title: AnsiString;
        varp: PBoolean;
        actionCB: TActionCB;
      end;
  private
    mItems: array of TItem;
    mCurIndex: Integer;

  public
    constructor Create (ax, ay: Integer);
    destructor Destroy (); override;

    procedure appendItem (const atext: AnsiString; bv: PBoolean; aaction: TActionCB=nil);

    procedure drawControl (gx, gy: Integer); override;

    function mouseEvent (var ev: THMouseEvent): Boolean; override;
    function keyEvent (var ev: THKeyEvent): Boolean; override;
  end;

  // ////////////////////////////////////////////////////////////////////// //
  TUIBox = class(TUIControl)
  private
    mHasFrame: Boolean;
    mCaption: AnsiString;

  public
    constructor Create (ahoriz: Boolean);

    function parseProperty (const prname: AnsiString; par: TTextParser): Boolean; override;

    procedure drawControl (gx, gy: Integer); override;

    function mouseEvent (var ev: THMouseEvent): Boolean; override;
    function keyEvent (var ev: THKeyEvent): Boolean; override;
  end;

  TUIHBox = class(TUIBox)
  public
    procedure AfterConstruction (); override; // so it will be correctly initialized when created from parser
  end;

  TUIVBox = class(TUIBox)
  public
    procedure AfterConstruction (); override; // so it will be correctly initialized when created from parser
  end;

  // ////////////////////////////////////////////////////////////////////// //
  TUISpan = class(TUIControl)
  public
    procedure AfterConstruction (); override; // so it will be correctly initialized when created from parser

    function parseProperty (const prname: AnsiString; par: TTextParser): Boolean; override;

    procedure drawControl (gx, gy: Integer); override;
  end;

  // ////////////////////////////////////////////////////////////////////// //
  TUILine = class(TUIControl)
  public
    function parseProperty (const prname: AnsiString; par: TTextParser): Boolean; override;

    procedure drawControl (gx, gy: Integer); override;
  end;

  TUIHLine = class(TUILine)
  public
    procedure AfterConstruction (); override; // so it will be correctly initialized when created from parser
  end;

  TUIVLine = class(TUILine)
  public
    procedure AfterConstruction (); override; // so it will be correctly initialized when created from parser
  end;

  // ////////////////////////////////////////////////////////////////////// //
  TUITextLabel = class(TUIControl)
  private
    mText: AnsiString;
    mHAlign: Integer; // -1: left; 0: center; 1: right; default: left
    mVAlign: Integer; // -1: top; 0: center; 1: bottom; default: center

  public
    constructor Create (const atext: AnsiString);

    procedure AfterConstruction (); override; // so it will be correctly initialized when created from parser

    function parseProperty (const prname: AnsiString; par: TTextParser): Boolean; override;

    procedure drawControl (gx, gy: Integer); override;

    function mouseEvent (var ev: THMouseEvent): Boolean; override;
    function keyEvent (var ev: THKeyEvent): Boolean; override;
  end;


// ////////////////////////////////////////////////////////////////////////// //
function uiMouseEvent (ev: THMouseEvent): Boolean;
function uiKeyEvent (ev: THKeyEvent): Boolean;
procedure uiDraw ();


// ////////////////////////////////////////////////////////////////////////// //
procedure uiAddWindow (ctl: TUIControl);
procedure uiRemoveWindow (ctl: TUIControl); // will free window if `mFreeOnClose` is `true`
function uiVisibleWindow (ctl: TUIControl): Boolean;


// ////////////////////////////////////////////////////////////////////////// //
// do layouting
procedure uiLayoutCtl (ctl: TUIControl);


// ////////////////////////////////////////////////////////////////////////// //
var
  gh_ui_scale: Single = 1.0;


implementation

uses
  gh_flexlay,
  utils;


// ////////////////////////////////////////////////////////////////////////// //
var
  knownCtlClasses: array of record
    klass: TUIControlClass;
    name: AnsiString;
  end = nil;


procedure registerCtlClass (aklass: TUIControlClass; const aname: AnsiString);
begin
  assert(aklass <> nil);
  assert(Length(aname) > 0);
  SetLength(knownCtlClasses, Length(knownCtlClasses)+1);
  knownCtlClasses[High(knownCtlClasses)].klass := aklass;
  knownCtlClasses[High(knownCtlClasses)].name := aname;
end;


function findCtlClass (const aname: AnsiString): TUIControlClass;
var
  f: Integer;
begin
  for f := 0 to High(knownCtlClasses) do
  begin
    if (strEquCI1251(aname, knownCtlClasses[f].name)) then
    begin
      result := knownCtlClasses[f].klass;
      exit;
    end;
  end;
  result := nil;
end;


// ////////////////////////////////////////////////////////////////////////// //
type
  TFlexLayouter = specialize TFlexLayouterBase<TUIControl>;

procedure uiLayoutCtl (ctl: TUIControl);
var
  lay: TFlexLayouter;
begin
  if (ctl = nil) then exit;
  lay := TFlexLayouter.Create();
  try
    lay.setup(ctl);
    //lay.layout();

    //writeln('============================'); lay.dumpFlat();

    //writeln('=== initial ==='); lay.dump();

    //lay.calcMaxSizeInternal(0);
    {
    lay.firstPass();
    writeln('=== after first pass ===');
    lay.dump();

    lay.secondPass();
    writeln('=== after second pass ===');
    lay.dump();
    }

    lay.layout();
    //writeln('=== final ==='); lay.dump();

    if (ctl.mParent = nil) and (ctl is TUITopWindow) and (TUITopWindow(ctl).mDoCenter) then
    begin
      TUITopWindow(ctl).centerInScreen();
    end;

  finally
    FreeAndNil(lay);
  end;
end;


// ////////////////////////////////////////////////////////////////////////// //
var
  uiTopList: array of TUIControl = nil;


function uiMouseEvent (ev: THMouseEvent): Boolean;
var
  f, c: Integer;
  lx, ly: Integer;
  ctmp: TUIControl;
begin
  ev.x := trunc(ev.x/gh_ui_scale);
  ev.y := trunc(ev.y/gh_ui_scale);
  ev.dx := trunc(ev.dx/gh_ui_scale); //FIXME
  ev.dy := trunc(ev.dy/gh_ui_scale); //FIXME
  if (Length(uiTopList) = 0) then result := false else result := uiTopList[High(uiTopList)].mouseEvent(ev);
  if not result and (ev.press) then
  begin
    for f := High(uiTopList) downto 0 do
    begin
      if uiTopList[f].toLocal(ev.x, ev.y, lx, ly) then
      begin
        result := true;
        if uiTopList[f].mEnabled and (f <> High(uiTopList)) then
        begin
          uiTopList[High(uiTopList)].blurred();
          ctmp := uiTopList[f];
          ctmp.mGrab := nil;
          for c := f+1 to High(uiTopList) do uiTopList[c-1] := uiTopList[c];
          uiTopList[High(uiTopList)] := ctmp;
          ctmp.activated();
          result := ctmp.mouseEvent(ev);
        end;
        exit;
      end;
    end;
  end;
end;


function uiKeyEvent (ev: THKeyEvent): Boolean;
begin
  ev.x := trunc(ev.x/gh_ui_scale);
  ev.y := trunc(ev.y/gh_ui_scale);
  if (Length(uiTopList) = 0) then result := false else result := uiTopList[High(uiTopList)].keyEvent(ev);
  if (ev.release) then begin result := true; exit; end;
end;


procedure uiDraw ();
var
  f: Integer;
  ctl: TUIControl;
begin
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  try
    glLoadIdentity();
    glScalef(gh_ui_scale, gh_ui_scale, 1);
    for f := 0 to High(uiTopList) do
    begin
      ctl := uiTopList[f];
      ctl.draw();
      if (f <> High(uiTopList)) then darkenRect(ctl.x0, ctl.y0, ctl.width, ctl.height, 128);
    end;
  finally
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  end;
end;


procedure uiAddWindow (ctl: TUIControl);
var
  f, c: Integer;
begin
  if (ctl = nil) then exit;
  ctl := ctl.topLevel;
  if not (ctl is TUITopWindow) then exit; // alas
  for f := 0 to High(uiTopList) do
  begin
    if (uiTopList[f] = ctl) then
    begin
      if (f <> High(uiTopList)) then
      begin
        uiTopList[High(uiTopList)].blurred();
        for c := f+1 to High(uiTopList) do uiTopList[c-1] := uiTopList[c];
        uiTopList[High(uiTopList)] := ctl;
        ctl.activated();
      end;
      exit;
    end;
  end;
  if (Length(uiTopList) > 0) then uiTopList[High(uiTopList)].blurred();
  SetLength(uiTopList, Length(uiTopList)+1);
  uiTopList[High(uiTopList)] := ctl;
  ctl.activated();
end;


procedure uiRemoveWindow (ctl: TUIControl);
var
  f, c: Integer;
begin
  if (ctl = nil) then exit;
  ctl := ctl.topLevel;
  if not (ctl is TUITopWindow) then exit; // alas
  for f := 0 to High(uiTopList) do
  begin
    if (uiTopList[f] = ctl) then
    begin
      ctl.blurred();
      for c := f+1 to High(uiTopList) do uiTopList[c-1] := uiTopList[c];
      SetLength(uiTopList, Length(uiTopList)-1);
      if (ctl is TUITopWindow) then
      begin
        try
          if assigned(TUITopWindow(ctl).closeCB) then TUITopWindow(ctl).closeCB(ctl, 0);
        finally
          if (TUITopWindow(ctl).mFreeOnClose) then FreeAndNil(ctl);
        end;
      end;
      exit;
    end;
  end;
end;


function uiVisibleWindow (ctl: TUIControl): Boolean;
var
  f: Integer;
begin
  result := false;
  if (ctl = nil) then exit;
  ctl := ctl.topLevel;
  if not (ctl is TUITopWindow) then exit; // alas
  for f := 0 to High(uiTopList) do
  begin
    if (uiTopList[f] = ctl) then begin result := true; exit; end;
  end;
end;


// ////////////////////////////////////////////////////////////////////////// //
constructor TUIControl.Create ();
begin
  mParent := nil;
  mX := 0;
  mY := 0;
  mWidth := 64;
  mHeight := 8;
  mFrameWidth := 0;
  mFrameHeight := 0;
  mEnabled := true;
  mCanFocus := true;
  mChildren := nil;
  mFocused := nil;
  mGrab := nil;
  mEscClose := false;
  mEatKeys := false;
  scallowed := false;
  mDrawShadow := false;
  actionCB := nil;
  // layouter interface
  //mDefSize := TLaySize.Create(64, 8); // default size
  mDefSize := TLaySize.Create(0, 0); // default size
  mMaxSize := TLaySize.Create(-1, -1); // maximum size
  mFlex := 0;
  mHoriz := true;
  mCanWrap := false;
  mLineStart := false;
  mHGroup := '';
  mVGroup := '';
  mAlign := -1; // left/top
  mExpand := false;
end;


constructor TUIControl.Create (ax, ay, aw, ah: Integer);
begin
  Create();
  mX := ax;
  mY := ay;
  mWidth := aw;
  mHeight := ah;
end;


destructor TUIControl.Destroy ();
var
  f, c: Integer;
begin
  if (mParent <> nil) then
  begin
    setFocused(false);
    for f := 0 to High(mParent.mChildren) do
    begin
      if (mParent.mChildren[f] = self) then
      begin
        for c := f+1 to High(mParent.mChildren) do mParent.mChildren[c-1] := mParent.mChildren[c];
        SetLength(mParent.mChildren, Length(mParent.mChildren)-1);
      end;
    end;
  end;
  for f := 0 to High(mChildren) do
  begin
    mChildren[f].mParent := nil;
    mChildren[f].Free();
  end;
  mChildren := nil;
end;


// ////////////////////////////////////////////////////////////////////////// //
function TUIControl.getDefSize (): TLaySize; inline; begin result := mLayDefSize; end;
function TUIControl.getMaxSize (): TLaySize; inline; begin result := mLayMaxSize; end;
function TUIControl.getFlex (): Integer; inline; begin result := mFlex; end;
function TUIControl.isHorizBox (): Boolean; inline; begin result := mHoriz; end;
procedure TUIControl.setHorizBox (v: Boolean); inline; begin mHoriz := v; end;
function TUIControl.canWrap (): Boolean; inline; begin result := mCanWrap; end;
procedure TUIControl.setCanWrap (v: Boolean); inline; begin mCanWrap := v; end;
function TUIControl.isLineStart (): Boolean; inline; begin result := mLineStart; end;
procedure TUIControl.setLineStart (v: Boolean); inline; begin mLineStart := v; end;
function TUIControl.getAlign (): Integer; inline; begin result := mAlign; end;
procedure TUIControl.setAlign (v: Integer); inline; begin mAlign := v; end;
function TUIControl.getExpand (): Boolean; inline; begin result := mExpand; end;
procedure TUIControl.setExpand (v: Boolean); inline; begin mExpand := v; end;
function TUIControl.getHGroup (): AnsiString; inline; begin result := mHGroup; end;
procedure TUIControl.setHGroup (const v: AnsiString); inline; begin mHGroup := v; end;
function TUIControl.getVGroup (): AnsiString; inline; begin result := mVGroup; end;
procedure TUIControl.setVGroup (const v: AnsiString); inline; begin mVGroup := v; end;

function TUIControl.getMargins (): TLayMargins; inline;
begin
  result := TLayMargins.Create(mFrameHeight, mFrameWidth, mFrameHeight, mFrameWidth);
end;

procedure TUIControl.setActualSizePos (constref apos: TLayPos; constref asize: TLaySize); inline; begin
  //writeln(self.className, '; pos=', apos.toString, '; size=', asize.toString);
  if (mParent <> nil) then
  begin
    mX := apos.x;
    mY := apos.y;
  end;
  mWidth := asize.w;
  mHeight := asize.h;
end;

procedure TUIControl.layPrepare ();
begin
  mLayDefSize := mDefSize;
  mLayMaxSize := mMaxSize;
  if (mLayMaxSize.w >= 0) then mLayMaxSize.w += mFrameWidth*2;
  if (mLayMaxSize.h >= 0) then mLayMaxSize.h += mFrameHeight*2;
end;


// ////////////////////////////////////////////////////////////////////////// //
function TUIControl.parsePos (par: TTextParser): TLayPos;
var
  ech: AnsiChar = ')';
begin
  if (par.eatDelim('[')) then ech := ']' else par.expectDelim('(');
  result.x := par.expectInt();
  par.eatDelim(','); // optional comma
  result.y := par.expectInt();
  par.eatDelim(','); // optional comma
  par.expectDelim(ech);
end;

function TUIControl.parseSize (par: TTextParser): TLaySize;
var
  ech: AnsiChar = ')';
begin
  if (par.eatDelim('[')) then ech := ']' else par.expectDelim('(');
  result.w := par.expectInt();
  par.eatDelim(','); // optional comma
  result.h := par.expectInt();
  par.eatDelim(','); // optional comma
  par.expectDelim(ech);
end;

function TUIControl.parseBool (par: TTextParser): Boolean;
begin
  result :=
    par.eatIdOrStrCI('true') or
    par.eatIdOrStrCI('yes') or
    par.eatIdOrStrCI('tan');
  if not result then
  begin
    if (not par.eatIdOrStrCI('false')) and (not par.eatIdOrStrCI('no')) and (not par.eatIdOrStrCI('ona')) then
    begin
      par.error('boolean value expected');
    end;
  end;
end;

function TUIControl.parseAnyAlign (par: TTextParser): Integer;
begin
       if (par.eatIdOrStrCI('left')) or (par.eatIdOrStrCI('top')) then result := -1
  else if (par.eatIdOrStrCI('right')) or (par.eatIdOrStrCI('bottom')) then result := 1
  else if (par.eatIdOrStrCI('center')) then result := 0
  else par.error('invalid align value');
end;

function TUIControl.parseHAlign (par: TTextParser): Integer;
begin
       if (par.eatIdOrStrCI('left')) then result := -1
  else if (par.eatIdOrStrCI('right')) then result := 1
  else if (par.eatIdOrStrCI('center')) then result := 0
  else par.error('invalid horizontal align value');
end;

function TUIControl.parseVAlign (par: TTextParser): Integer;
begin
       if (par.eatIdOrStrCI('top')) then result := -1
  else if (par.eatIdOrStrCI('bottom')) then result := 1
  else if (par.eatIdOrStrCI('center')) then result := 0
  else par.error('invalid vertical align value');
end;

procedure TUIControl.parseTextAlign (par: TTextParser; var h, v: Integer);
var
  wasH: Boolean = false;
  wasV: Boolean = false;
begin
  while true do
  begin
    if (par.eatIdOrStrCI('left')) then
    begin
      if wasH then par.error('too many align directives');
      wasH := true;
      h := -1;
      continue;
    end;
    if (par.eatIdOrStrCI('right')) then
    begin
      if wasH then par.error('too many align directives');
      wasH := true;
      h := 1;
      continue;
    end;
    if (par.eatIdOrStrCI('hcenter')) then
    begin
      if wasH then par.error('too many align directives');
      wasH := true;
      h := 0;
      continue;
    end;
    if (par.eatIdOrStrCI('top')) then
    begin
      if wasV then par.error('too many align directives');
      wasV := true;
      v := -1;
      continue;
    end;
    if (par.eatIdOrStrCI('bottom')) then
    begin
      if wasV then par.error('too many align directives');
      wasV := true;
      v := 1;
      continue;
    end;
    if (par.eatIdOrStrCI('vcenter')) then
    begin
      if wasV then par.error('too many align directives');
      wasV := true;
      v := 0;
      continue;
    end;
    if (par.eatIdOrStrCI('center')) then
    begin
      if wasV or wasH then par.error('too many align directives');
      wasV := true;
      wasH := true;
      h := 0;
      v := 0;
      continue;
    end;
    break;
  end;
  if not wasV and not wasH then par.error('invalid align value');
end;

function TUIControl.parseOrientation (const prname: AnsiString; par: TTextParser): Boolean;
begin
  if (strEquCI1251(prname, 'orientation')) or (strEquCI1251(prname, 'orient')) then
  begin
         if (par.eatIdOrStrCI('horizontal')) or (par.eatIdOrStrCI('horiz')) then mHoriz := true
    else if (par.eatIdOrStrCI('vertical')) or (par.eatIdOrStrCI('vert')) then mHoriz := false
    else par.error('`horizontal` or `vertical` expected');
    result := true;
  end
  else
  begin
    result := false;
  end;
end;

// par should be on '{'; final '}' is eaten
procedure TUIControl.parseProperties (par: TTextParser);
var
  pn: AnsiString;
begin
  if (not par.eatDelim('{')) then exit;
  while (not par.eatDelim('}')) do
  begin
    if (not par.isIdOrStr) then par.error('property name expected');
    pn := par.tokStr;
    par.skipToken();
    par.eatDelim(':'); // optional
    if not parseProperty(pn, par) then par.errorfmt('invalid property name ''%s''', [pn]);
    par.eatDelim(','); // optional
  end;
end;

// par should be on '{'
procedure TUIControl.parseChildren (par: TTextParser);
var
  cc: TUIControlClass;
  ctl: TUIControl;
begin
  par.expectDelim('{');
  while (not par.eatDelim('}')) do
  begin
    if (not par.isIdOrStr) then par.error('control name expected');
    cc := findCtlClass(par.tokStr);
    if (cc = nil) then par.errorfmt('unknown control name: ''%s''', [par.tokStr]);
    //writeln('children for <', par.tokStr, '>: <', cc.className, '>');
    par.skipToken();
    par.eatDelim(':'); // optional
    ctl := cc.Create();
    //writeln('  mHoriz=', ctl.mHoriz);
    try
      ctl.parseProperties(par);
    except
      FreeAndNil(ctl);
      raise;
    end;
    //writeln(': ', ctl.mDefSize.toString);
    appendChild(ctl);
    par.eatDelim(','); // optional
  end;
end;


function TUIControl.parseProperty (const prname: AnsiString; par: TTextParser): Boolean;
begin
  result := true;
  if (strEquCI1251(prname, 'id')) then begin mId := par.expectStrOrId(true); exit; end; // allow empty strings
  if (strEquCI1251(prname, 'flex')) then begin flex := par.expectInt(); exit; end;
  // sizes
  if (strEquCI1251(prname, 'defsize')) or (strEquCI1251(prname, 'size')) then begin mDefSize := parseSize(par); exit; end;
  if (strEquCI1251(prname, 'maxsize')) then begin mMaxSize := parseSize(par); exit; end;
  if (strEquCI1251(prname, 'defwidth')) or (strEquCI1251(prname, 'width')) then begin mDefSize.w := par.expectInt(); exit; end;
  if (strEquCI1251(prname, 'defheight')) or (strEquCI1251(prname, 'height')) then begin mDefSize.h := par.expectInt(); exit; end;
  if (strEquCI1251(prname, 'maxwidth')) then begin mMaxSize.w := par.expectInt(); exit; end;
  if (strEquCI1251(prname, 'maxheight')) then begin mMaxSize.h := par.expectInt(); exit; end;
  // flags
  if (strEquCI1251(prname, 'wrap')) then begin mCanWrap := parseBool(par); exit; end;
  if (strEquCI1251(prname, 'linestart')) then begin mLineStart := parseBool(par); exit; end;
  if (strEquCI1251(prname, 'expand')) then begin mExpand := parseBool(par); exit; end;
  // align
  if (strEquCI1251(prname, 'align')) then begin mAlign := parseAnyAlign(par); exit; end;
  if (strEquCI1251(prname, 'hgroup')) then begin mHGroup := par.expectStrOrId(true); exit; end; // allow empty strings
  if (strEquCI1251(prname, 'vgroup')) then begin mVGroup := par.expectStrOrId(true); exit; end; // allow empty strings
  // other
  if (strEquCI1251(prname, 'canfocus')) then begin mCanFocus := parseBool(par); exit; end;
  if (strEquCI1251(prname, 'enabled')) then begin mEnabled := parseBool(par); exit; end;
  if (strEquCI1251(prname, 'disabled')) then begin mEnabled := not parseBool(par); exit; end;
  if (strEquCI1251(prname, 'escclose')) then begin mEscClose := not parseBool(par); exit; end;
  if (strEquCI1251(prname, 'eatkeys')) then begin mEatKeys := not parseBool(par); exit; end;
  result := false;
end;


// ////////////////////////////////////////////////////////////////////////// //
procedure TUIControl.activated ();
begin
end;


procedure TUIControl.blurred ();
begin
  mGrab := nil;
end;


function TUIControl.topLevel (): TUIControl; inline;
begin
  result := self;
  while (result.mParent <> nil) do result := result.mParent;
end;


function TUIControl.getEnabled (): Boolean;
var
  ctl: TUIControl;
begin
  result := false;
  if (not mEnabled) or (mWidth < 1) or (mHeight < 1) then exit;
  ctl := mParent;
  while (ctl <> nil) do
  begin
    if (not ctl.mEnabled) or (ctl.mWidth < 1) or (ctl.mHeight < 1) then exit;
    ctl := ctl.mParent;
  end;
  result := true;
end;


procedure TUIControl.setEnabled (v: Boolean); inline;
begin
  if (mEnabled = v) then exit;
  mEnabled := v;
  if not v and focused then setFocused(false);
end;


function TUIControl.getFocused (): Boolean; inline;
begin
  if (mParent = nil) then result := (Length(uiTopList) > 0) and (uiTopList[High(uiTopList)] = self) else result := (topLevel.mFocused = self);
end;


procedure TUIControl.setFocused (v: Boolean); inline;
var
  tl: TUIControl;
begin
  tl := topLevel;
  if not v then
  begin
    if (tl.mFocused = self) then
    begin
      tl.blurred();
      tl.mFocused := tl.findNextFocus(self);
      if (tl.mFocused = self) then tl.mFocused := nil;
    end;
    exit;
  end;
  if (not mEnabled) or (not mCanFocus) then exit;
  if (tl.mFocused <> self) then
  begin
    if (tl.mFocused <> nil) then tl.mFocused.blurred();
    tl.mFocused := self;
    if (tl.mGrab <> self) then tl.mGrab := nil;
    activated();
  end;
end;


function TUIControl.isMyChild (ctl: TUIControl): Boolean;
begin
  result := true;
  while (ctl <> nil) do
  begin
    if (ctl.mParent = self) then exit;
    ctl := ctl.mParent;
  end;
  result := false;
end;


// returns `true` if global coords are inside this control
function TUIControl.toLocal (var x, y: Integer): Boolean;
var
  ctl: TUIControl;
begin
  ctl := self;
  while (ctl <> nil) do
  begin
    Dec(x, ctl.mX);
    Dec(y, ctl.mY);
    ctl := ctl.mParent;
  end;
  result := (x >= 0) and (y >= 0) and (x < mWidth) and (y < mHeight);
end;

function TUIControl.toLocal (gx, gy: Integer; out x, y: Integer): Boolean; inline;
begin
  x := gx;
  y := gy;
  result := toLocal(x, y);
end;

procedure TUIControl.toGlobal (var x, y: Integer);
var
  ctl: TUIControl;
begin
  ctl := self;
  while (ctl <> nil) do
  begin
    Inc(x, ctl.mX);
    Inc(y, ctl.mY);
    ctl := ctl.mParent;
  end;
end;

procedure TUIControl.toGlobal (lx, ly: Integer; out x, y: Integer); inline;
begin
  x := lx;
  y := ly;
  toGlobal(x, y);
end;


// x and y are global coords
function TUIControl.controlAtXY (x, y: Integer): TUIControl;
var
  lx, ly: Integer;
  f: Integer;
begin
  result := nil;
  if (not mEnabled) or (mWidth < 1) or (mHeight < 1) then exit;
  if not toLocal(x, y, lx, ly) then exit;
  for f := High(mChildren) downto 0 do
  begin
    result := mChildren[f].controlAtXY(x, y);
    if (result <> nil) then exit;
  end;
  result := self;
end;


function TUIControl.prevSibling (): TUIControl;
var
  f: Integer;
begin
  if (mParent <> nil) then
  begin
    for f := 1 to High(mParent.mChildren) do
    begin
      if (mParent.mChildren[f] = self) then begin result := mParent.mChildren[f-1]; exit; end;
    end;
  end;
  result := nil;
end;

function TUIControl.nextSibling (): TUIControl;
var
  f: Integer;
begin
  if (mParent <> nil) then
  begin
    for f := 0 to High(mParent.mChildren)-1 do
    begin
      if (mParent.mChildren[f] = self) then begin result := mParent.mChildren[f+1]; exit; end;
    end;
  end;
  result := nil;
end;

function TUIControl.firstChild (): TUIControl; inline;
begin
  if (Length(mChildren) <> 0) then result := mChildren[0] else result := nil;
end;

function TUIControl.lastChild (): TUIControl; inline;
begin
  if (Length(mChildren) <> 0) then result := mChildren[High(mChildren)] else result := nil;
end;


function TUIControl.findFirstFocus (): TUIControl;
var
  f: Integer;
begin
  result := nil;
  if enabled then
  begin
    for f := 0 to High(mChildren) do
    begin
      result := mChildren[f].findFirstFocus();
      if (result <> nil) then exit;
    end;
    if mCanFocus then result := self;
  end;
end;


function TUIControl.findLastFocus (): TUIControl;
var
  f: Integer;
begin
  result := nil;
  if enabled then
  begin
    for f := High(mChildren) downto 0 do
    begin
      result := mChildren[f].findLastFocus();
      if (result <> nil) then exit;
    end;
    if mCanFocus then result := self;
  end;
end;


function TUIControl.findNextFocus (cur: TUIControl): TUIControl;
begin
  result := nil;
  if enabled then
  begin
    if not isMyChild(cur) then cur := nil;
    if (cur = nil) then begin result := findFirstFocus(); exit; end;
    result := cur.findFirstFocus();
    if (result <> nil) and (result <> cur) then exit;
    while true do
    begin
      cur := cur.nextSibling;
      if (cur = nil) then break;
      result := cur.findFirstFocus();
      if (result <> nil) then exit;
    end;
    result := findFirstFocus();
  end;
end;


function TUIControl.findPrevFocus (cur: TUIControl): TUIControl;
begin
  result := nil;
  if enabled then
  begin
    if not isMyChild(cur) then cur := nil;
    if (cur = nil) then begin result := findLastFocus(); exit; end;
    //FIXME!
    result := cur.findLastFocus();
    if (result <> nil) and (result <> cur) then exit;
    while true do
    begin
      cur := cur.prevSibling;
      if (cur = nil) then break;
      result := cur.findLastFocus();
      if (result <> nil) then exit;
    end;
    result := findLastFocus();
  end;
end;


procedure TUIControl.appendChild (ctl: TUIControl);
begin
  if (ctl = nil) then exit;
  if (ctl.mParent <> nil) then exit;
  SetLength(mChildren, Length(mChildren)+1);
  mChildren[High(mChildren)] := ctl;
  ctl.mParent := self;
  Inc(ctl.mX, mFrameWidth);
  Inc(ctl.mY, mFrameHeight);
  if (ctl.mWidth > 0) and (ctl.mHeight > 0) and
     (ctl.mX+ctl.mWidth > mFrameWidth) and (ctl.mY+ctl.mHeight > mFrameHeight) then
  begin
    if (mWidth+mFrameWidth < ctl.mX+ctl.mWidth) then mWidth := ctl.mX+ctl.mWidth+mFrameWidth;
    if (mHeight+mFrameHeight < ctl.mY+ctl.mHeight) then mHeight := ctl.mY+ctl.mHeight+mFrameHeight;
  end;
  //if (mFocused = nil) and ctl.mEnabled and ctl.mCanFocus and (ctl.mWidth > 0) and (ctl.mHeight > 0) then mFocused := ctl;
end;


// ////////////////////////////////////////////////////////////////////////// //
procedure TUIControl.setScissorGLInternal (x, y, w, h: Integer);
begin
  if not scallowed then exit;
  x := trunc(x*gh_ui_scale);
  y := trunc(y*gh_ui_scale);
  w := trunc(w*gh_ui_scale);
  h := trunc(h*gh_ui_scale);
  scis.combineRect(x, y, w, h);
end;

procedure TUIControl.setScissor (lx, ly, lw, lh: Integer);
var
  gx, gy: Integer;
  //ox, oy, ow, oh: Integer;
begin
  if not scallowed then exit;
  //ox := lx; oy := ly; ow := lw; oh := lh;
  if not intersectRect(lx, ly, lw, lh, 0, 0, mWidth, mHeight) then
  begin
    //writeln('oops: <', self.className, '>: old=(', ox, ',', oy, ')-[', ow, ',', oh, ']');
    glScissor(0, 0, 0, 0);
    exit;
  end;
  toGlobal(lx, ly, gx, gy);
  setScissorGLInternal(gx, gy, lw, lh);
end;

procedure TUIControl.resetScissor (fullArea: Boolean); inline;
begin
  if not scallowed then exit;
  if (fullArea) then
  begin
    setScissor(0, 0, mWidth, mHeight);
  end
  else
  begin
    setScissor(mFrameWidth, mFrameHeight, mWidth-mFrameWidth*2, mHeight-mFrameHeight*2);
  end;
end;


// ////////////////////////////////////////////////////////////////////////// //
procedure TUIControl.draw ();
var
  f: Integer;
  gx, gy: Integer;
begin
  if (mWidth < 1) or (mHeight < 1) then exit;
  toGlobal(0, 0, gx, gy);
  //conwritefln('[%s]: (%d,%d)-(%d,%d)  (%d,%d)', [ClassName, mX, mY, mWidth, mHeight, x, y]);

  scis.save(true); // scissoring enabled
  try
    scallowed := true;
    resetScissor(true); // full area
    drawControl(gx, gy);
    resetScissor(false); // client area
    for f := 0 to High(mChildren) do mChildren[f].draw();
    resetScissor(true); // full area
    drawControlPost(gx, gy);
  finally
    scis.restore();
    scallowed := false;
  end;
end;

procedure TUIControl.drawControl (gx, gy: Integer);
begin
  if (mParent = nil) then darkenRect(gx, gy, mWidth, mHeight, 64);
end;

procedure TUIControl.drawControlPost (gx, gy: Integer);
begin
  // shadow
  if mDrawShadow and (mWidth > 0) and (mHeight > 0) then
  begin
    setScissorGLInternal(gx+8, gy+8, mWidth, mHeight);
    darkenRect(gx+mWidth, gy+8, 8, mHeight, 128);
    darkenRect(gx+8, gy+mHeight, mWidth-8, 8, 128);
  end;
end;


// ////////////////////////////////////////////////////////////////////////// //
function TUIControl.mouseEvent (var ev: THMouseEvent): Boolean;
var
  ctl: TUIControl;
begin
  result := false;
  if not mEnabled then exit;
  if (mParent = nil) then
  begin
    if (mGrab <> nil) then
    begin
      result := mGrab.mouseEvent(ev);
      if (ev.release) then mGrab := nil;
      exit;
    end;
  end;
  if (mWidth < 1) or (mHeight < 1) then exit;
  ctl := controlAtXY(ev.x, ev.y);
  if (ctl <> nil) and (ctl <> self) then
  begin
    if (ctl <> topLevel.mFocused) then ctl.setFocused(true);
    result := ctl.mouseEvent(ev);
  end
  else if (ctl = self) and assigned(actionCB) then
  begin
    actionCB(self, 0);
  end;
end;


function TUIControl.keyEvent (var ev: THKeyEvent): Boolean;
var
  ctl: TUIControl;
begin
  result := false;
  if not mEnabled then exit;
  if (topLevel.mFocused <> self) and isMyChild(topLevel.mFocused) and topLevel.mFocused.mEnabled then result := topLevel.mFocused.keyEvent(ev);
  if (mParent = nil) then
  begin
    if (ev = 'S-Tab') then
    begin
      result := true;
      ctl := findPrevFocus(mFocused);
      if (ctl <> mFocused) then
      begin
        mGrab := nil;
        mFocused := ctl;
      end;
      exit;
    end;
    if (ev = 'Tab') then
    begin
      result := true;
      ctl := findNextFocus(mFocused);
      if (ctl <> mFocused) then
      begin
        mGrab := nil;
        mFocused := ctl;
      end;
      exit;
    end;
    if mEscClose and (ev = 'Escape') then
    begin
      result := true;
      uiRemoveWindow(self);
      exit;
    end;
  end;
  if mEatKeys then result := true;
end;


// ////////////////////////////////////////////////////////////////////////// //
constructor TUITopWindow.Create (const atitle: AnsiString; ax, ay: Integer; aw: Integer=-1; ah: Integer=-1);
begin
  inherited Create(ax, ay, aw, ah);
  mFrameWidth := 8;
  mFrameHeight := 8;
  mTitle := atitle;
  if (mWidth < mFrameWidth*2+3*8) then mWidth := mFrameWidth*2+3*8;
  if (mHeight < mFrameHeight*2) then mHeight := mFrameHeight*2;
  if (Length(mTitle) > 0) then
  begin
    if (mWidth < Length(mTitle)*8+mFrameWidth*2+3*8) then mWidth := Length(mTitle)*8+mFrameWidth*2+3*8;
  end;
  mDragging := false;
  mDrawShadow := true;
  mWaitingClose := false;
  mInClose := false;
  closeCB := nil;
end;


function TUITopWindow.parseProperty (const prname: AnsiString; par: TTextParser): Boolean;
begin
  if (strEquCI1251(prname, 'title')) or (strEquCI1251(prname, 'caption')) then
  begin
    mTitle := par.expectStrOrId(true);
    result := true;
    exit;
  end;
  if (strEquCI1251(prname, 'children')) then
  begin
    parseChildren(par);
    result := true;
    exit;
  end;
  if (strEquCI1251(prname, 'position')) then
  begin
         if (par.eatIdOrStrCI('default')) then mDoCenter := false
    else if (par.eatIdOrStrCI('center')) then mDoCenter := true
    else par.error('`center` or `default` expected');
    result := true;
    exit;
  end;
  if (parseOrientation(prname, par)) then begin result := true; exit; end;
  result := inherited parseProperty(prname, par);
end;


procedure TUITopWindow.centerInScreen ();
begin
  if (mWidth > 0) and (mHeight > 0) then
  begin
    mX := trunc((gScrWidth/gh_ui_scale-mWidth)/2);
    mY := trunc((gScrHeight/gh_ui_scale-mHeight)/2);
  end;
end;


procedure TUITopWindow.drawControl (gx, gy: Integer);
begin
  fillRect(gx, gy, mWidth, mHeight, TGxRGBA.Create(0, 0, 128));
end;


procedure TUITopWindow.drawControlPost (gx, gy: Integer);
const r = 255;
const g = 255;
const b = 255;
var
  tx: Integer;
begin
  if mDragging then
  begin
    drawRectUI(mX+4, mY+4, mWidth-8, mHeight-8, TGxRGBA.Create(r, g, b));
  end
  else
  begin
    drawRectUI(mX+3, mY+3, mWidth-6, mHeight-6, TGxRGBA.Create(r, g, b));
    drawRectUI(mX+5, mY+5, mWidth-10, mHeight-10, TGxRGBA.Create(r, g, b));
    setScissor(mFrameWidth, 0, 3*8, 8);
    fillRect(mX+mFrameWidth, mY, 3*8, 8, TGxRGBA.Create(0, 0, 128));
    drawText8(mX+mFrameWidth, mY, '[ ]', TGxRGBA.Create(r, g, b));
    if mInClose then drawText8(mX+mFrameWidth+7, mY, '#', TGxRGBA.Create(0, 255, 0))
    else drawText8(mX+mFrameWidth+7, mY, '*', TGxRGBA.Create(0, 255, 0));
  end;
  if (Length(mTitle) > 0) then
  begin
    setScissor(mFrameWidth+3*8, 0, mWidth-mFrameWidth*2-3*8, 8);
    tx := (mX+3*8)+((mWidth-3*8)-Length(mTitle)*8) div 2;
    fillRect(tx-3, mY, Length(mTitle)*8+3+2, 8, TGxRGBA.Create(0, 0, 128));
    drawText8(tx, mY, mTitle, TGxRGBA.Create(r, g, b));
  end;
  inherited drawControlPost(gx, gy);
end;


procedure TUITopWindow.activated ();
begin
  if (mFocused = nil) or (mFocused = self) then
  begin
    mFocused := findFirstFocus();
    if (mFocused <> nil) and (mFocused <> self) then mFocused.activated();
  end;
  inherited;
end;


procedure TUITopWindow.blurred ();
begin
  mDragging := false;
  mWaitingClose := false;
  mInClose := false;
  inherited;
end;


function TUITopWindow.keyEvent (var ev: THKeyEvent): Boolean;
begin
  result := inherited keyEvent(ev);
  if not getFocused then exit;
  if (ev = 'M-F3') then
  begin
    uiRemoveWindow(self);
    result := true;
    exit;
  end;
end;


function TUITopWindow.mouseEvent (var ev: THMouseEvent): Boolean;
var
  lx, ly: Integer;
begin
  result := false;
  if not mEnabled then exit;
  if (mWidth < 1) or (mHeight < 1) then exit;

  if mDragging then
  begin
    mX += ev.x-mDragStartX;
    mY += ev.y-mDragStartY;
    mDragStartX := ev.x;
    mDragStartY := ev.y;
    if (ev.release) then mDragging := false;
    result := true;
    exit;
  end;

  if toLocal(ev.x, ev.y, lx, ly) then
  begin
    if (ev.press) then
    begin
      if (ly < 8) then
      begin
        if (lx >= mFrameWidth) and (lx < mFrameWidth+3*8) then
        begin
          //uiRemoveWindow(self);
          mWaitingClose := true;
          mInClose := true;
        end
        else
        begin
          mDragging := true;
          mDragStartX := ev.x;
          mDragStartY := ev.y;
        end;
        result := true;
        exit;
      end;
      if (lx < mFrameWidth) or (lx >= mWidth-mFrameWidth) or (ly >= mHeight-mFrameHeight) then
      begin
        mDragging := true;
        mDragStartX := ev.x;
        mDragStartY := ev.y;
        result := true;
        exit;
      end;
    end;

    if (ev.release) then
    begin
      if mWaitingClose and (lx >= mFrameWidth) and (lx < mFrameWidth+3*8) then
      begin
        uiRemoveWindow(self);
        result := true;
        exit;
      end;
      mWaitingClose := false;
      mInClose := false;
    end;

    if (ev.motion) then
    begin
      if mWaitingClose then
      begin
        mInClose := (lx >= mFrameWidth) and (lx < mFrameWidth+3*8);
        result := true;
        exit;
      end;
    end;
  end
  else
  begin
    mInClose := false;
    if (not ev.motion) then mWaitingClose := false;
  end;

  result := inherited mouseEvent(ev);
end;


// ////////////////////////////////////////////////////////////////////////// //
constructor TUISimpleText.Create (ax, ay: Integer);
begin
  mItems := nil;
  inherited Create(ax, ay, 4, 4);
end;


destructor TUISimpleText.Destroy ();
begin
  mItems := nil;
  inherited;
end;


procedure TUISimpleText.appendItem (const atext: AnsiString; acentered: Boolean=false; ahline: Boolean=false);
var
  it: PItem;
begin
  if (Length(atext)*8+3*8+2 > mWidth) then mWidth := Length(atext)*8+3*8+2;
  SetLength(mItems, Length(mItems)+1);
  it := @mItems[High(mItems)];
  it.title := atext;
  it.centered := acentered;
  it.hline := ahline;
  if (Length(mItems)*8 > mHeight) then mHeight := Length(mItems)*8;
end;


procedure TUISimpleText.drawControl (gx, gy: Integer);
var
  f, tx: Integer;
  it: PItem;
  r, g, b: Integer;
begin
  for f := 0 to High(mItems) do
  begin
    it := @mItems[f];
    tx := gx;
    r := 255;
    g := 255;
    b := 0;
    if it.centered then begin b := 255; tx := gx+(mWidth-Length(it.title)*8) div 2; end;
    if it.hline then
    begin
      b := 255;
      if (Length(it.title) = 0) then
      begin
        drawHLine(gx+4, gy+3, mWidth-8, TGxRGBA.Create(r, g, b));
      end
      else if (tx-3 > gx+4) then
      begin
        drawHLine(gx+4, gy+3, tx-3-(gx+3), TGxRGBA.Create(r, g, b));
        drawHLine(tx+Length(it.title)*8, gy+3, mWidth-4, TGxRGBA.Create(r, g, b));
      end;
    end;
    drawText8(tx, gy, it.title, TGxRGBA.Create(r, g, b));
    Inc(gy, 8);
  end;
end;


function TUISimpleText.mouseEvent (var ev: THMouseEvent): Boolean;
var
  lx, ly: Integer;
begin
  result := inherited mouseEvent(ev);
  if not result and toLocal(ev.x, ev.y, lx, ly) then
  begin
    result := true;
  end;
end;


function TUISimpleText.keyEvent (var ev: THKeyEvent): Boolean;
begin
  result := inherited keyEvent(ev);
end;


// ////////////////////////////////////////////////////////////////////////// //
constructor TUICBListBox.Create (ax, ay: Integer);
begin
  mItems := nil;
  mCurIndex := -1;
  inherited Create(ax, ay, 4, 4);
end;


destructor TUICBListBox.Destroy ();
begin
  mItems := nil;
  inherited;
end;


procedure TUICBListBox.appendItem (const atext: AnsiString; bv: PBoolean; aaction: TActionCB=nil);
var
  it: PItem;
begin
  if (Length(atext)*8+3*8+2 > mWidth) then mWidth := Length(atext)*8+3*8+2;
  SetLength(mItems, Length(mItems)+1);
  it := @mItems[High(mItems)];
  it.title := atext;
  it.varp := bv;
  it.actionCB := aaction;
  if (Length(mItems)*8 > mHeight) then mHeight := Length(mItems)*8;
  if (mCurIndex < 0) then mCurIndex := 0;
end;


procedure TUICBListBox.drawControl (gx, gy: Integer);
var
  f, tx: Integer;
  it: PItem;
begin
  for f := 0 to High(mItems) do
  begin
    it := @mItems[f];
    if (mCurIndex = f) then fillRect(gx, gy, mWidth, 8, TGxRGBA.Create(0, 128, 0));
    if (it.varp <> nil) then
    begin
      if it.varp^ then drawText8(gx, gy, '[x]', TGxRGBA.Create(255, 255, 255)) else drawText8(gx, gy, '[ ]', TGxRGBA.Create(255, 255, 255));
      drawText8(gx+3*8+2, gy, it.title, TGxRGBA.Create(255, 255, 0));
    end
    else if (Length(it.title) > 0) then
    begin
      tx := gx+(mWidth-Length(it.title)*8) div 2;
      if (tx-3 > gx+4) then
      begin
        drawHLine(gx+4, gy+3, tx-3-(gx+3), TGxRGBA.Create(255, 255, 255));
        drawHLine(tx+Length(it.title)*8, gy+3, mWidth-4, TGxRGBA.Create(255, 255, 255));
      end;
      drawText8(tx, gy, it.title, TGxRGBA.Create(255, 255, 255));
    end
    else
    begin
      drawHLine(gx+4, gy+3, mWidth-8, TGxRGBA.Create(255, 255, 255));
    end;
    Inc(gy, 8);
  end;
end;


function TUICBListBox.mouseEvent (var ev: THMouseEvent): Boolean;
var
  lx, ly: Integer;
  it: PItem;
begin
  result := inherited mouseEvent(ev);
  if not result and toLocal(ev.x, ev.y, lx, ly) then
  begin
    result := true;
    if (ev = 'lmb') then
    begin
      ly := ly div 8;
      if (ly >= 0) and (ly < Length(mItems)) then
      begin
        it := @mItems[ly];
        if (it.varp <> nil) then
        begin
          mCurIndex := ly;
          it.varp^ := not it.varp^;
          if assigned(it.actionCB) then it.actionCB(self, Integer(it.varp^));
          if assigned(actionCB) then actionCB(self, ly);
        end;
      end;
    end;
  end;
end;


function TUICBListBox.keyEvent (var ev: THKeyEvent): Boolean;
var
  it: PItem;
begin
  result := inherited keyEvent(ev);
  if not getFocused then exit;
  //result := true;
  if (ev = 'Home') or (ev = 'PageUp') then
  begin
    result := true;
    mCurIndex := 0;
  end;
  if (ev = 'End') or (ev = 'PageDown') then
  begin
    result := true;
    mCurIndex := High(mItems);
  end;
  if (ev = 'Up') then
  begin
    result := true;
    if (Length(mItems) > 0) then
    begin
      if (mCurIndex < 0) then mCurIndex := Length(mItems);
      while (mCurIndex > 0) do
      begin
        Dec(mCurIndex);
        if (mItems[mCurIndex].varp <> nil) then break;
      end;
    end
    else
    begin
      mCurIndex := -1;
    end;
  end;
  if (ev = 'Down') then
  begin
    result := true;
    if (Length(mItems) > 0) then
    begin
      if (mCurIndex < 0) then mCurIndex := -1;
      while (mCurIndex < High(mItems)) do
      begin
        Inc(mCurIndex);
        if (mItems[mCurIndex].varp <> nil) then break;
      end;
    end
    else
    begin
      mCurIndex := -1;
    end;
  end;
  if (ev = 'Space') or (ev = 'Enter') then
  begin
    result := true;
    if (mCurIndex >= 0) and (mCurIndex < Length(mItems)) and (mItems[mCurIndex].varp <> nil) then
    begin
      it := @mItems[mCurIndex];
      it.varp^ := not it.varp^;
      if assigned(it.actionCB) then it.actionCB(self, Integer(it.varp^));
      if assigned(actionCB) then actionCB(self, mCurIndex);
    end;
  end;
end;


// ////////////////////////////////////////////////////////////////////////// //
constructor TUIBox.Create (ahoriz: Boolean);
begin
  inherited Create();
  mHoriz := ahoriz;
  mCanFocus := false;
end;


function TUIBox.parseProperty (const prname: AnsiString; par: TTextParser): Boolean;
begin
  if (parseOrientation(prname, par)) then begin result := true; exit; end;
  if (strEquCI1251(prname, 'frame')) then
  begin
    mHasFrame := parseBool(par);
    if (mHasFrame) then begin mFrameWidth := 8; mFrameHeight := 8; end else begin mFrameWidth := 0; mFrameHeight := 0; end;
    result := true;
    exit;
  end;
  if (strEquCI1251(prname, 'title')) or (strEquCI1251(prname, 'caption')) then
  begin
    mCaption := par.expectStrOrId(true);
    mDefSize := TLaySize.Create(Length(mCaption)*8+3, 8);
    result := true;
    exit;
  end;
  if (strEquCI1251(prname, 'children')) then
  begin
    parseChildren(par);
    result := true;
    exit;
  end;
  result := inherited parseProperty(prname, par);
end;


procedure TUIBox.drawControl (gx, gy: Integer);
var
  r, g, b: Integer;
  tx: Integer;
begin
  if focused then begin r := 255; g := 255; b := 255; end else begin r := 255; g := 255; b := 0; end;
  if mHasFrame then
  begin
    // draw frame
    drawRectUI(gx+3, gy+3, mWidth-6, mHeight-6, TGxRGBA.Create(r, g, b));
  end;
  // draw caption
  if (Length(mCaption) > 0) then
  begin
    setScissor(mFrameWidth+1, 0, mWidth-mFrameWidth-2, 8);
    tx := gx+((mWidth-Length(mCaption)*8) div 2);
    if mHasFrame then fillRect(tx-2, gy, Length(mCaption)*8+3, 8, TGxRGBA.Create(0, 0, 128));
    drawText8(tx, gy, mCaption, TGxRGBA.Create(r, g, b));
  end;
end;


function TUIBox.mouseEvent (var ev: THMouseEvent): Boolean;
var
  lx, ly: Integer;
begin
  result := inherited mouseEvent(ev);
  if not result and toLocal(ev.x, ev.y, lx, ly) then
  begin
    result := true;
  end;
end;


//TODO: navigation with arrow keys, according to box orientation
function TUIBox.keyEvent (var ev: THKeyEvent): Boolean;
begin
  result := inherited keyEvent(ev);
end;


// ////////////////////////////////////////////////////////////////////////// //
procedure TUIHBox.AfterConstruction ();
begin
  inherited AfterConstruction();
  mHoriz := true;
end;


// ////////////////////////////////////////////////////////////////////////// //
procedure TUIVBox.AfterConstruction ();
begin
  inherited AfterConstruction();
  mHoriz := false;
  mCanFocus := false;
end;


// ////////////////////////////////////////////////////////////////////////// //
procedure TUISpan.AfterConstruction ();
begin
  inherited AfterConstruction();
  mExpand := true;
  mCanFocus := false;
end;


function TUISpan.parseProperty (const prname: AnsiString; par: TTextParser): Boolean;
begin
  if (parseOrientation(prname, par)) then begin result := true; exit; end;
  result := inherited parseProperty(prname, par);
end;


procedure TUISpan.drawControl (gx, gy: Integer);
begin
end;


// ////////////////////////////////////////////////////////////////////// //
function TUILine.parseProperty (const prname: AnsiString; par: TTextParser): Boolean;
begin
  if (parseOrientation(prname, par)) then begin result := true; exit; end;
  result := inherited parseProperty(prname, par);
end;


procedure TUILine.drawControl (gx, gy: Integer);
begin
  if mHoriz then
  begin
    drawHLine(gx, gy+(mHeight div 2), mWidth, TGxRGBA.Create(255, 255, 255));
  end
  else
  begin
    drawVLine(gx+(mWidth div 2), gy, mHeight, TGxRGBA.Create(255, 255, 255));
  end;
end;


// ////////////////////////////////////////////////////////////////////////// //
procedure TUIHLine.AfterConstruction ();
begin
  mHoriz := true;
  mExpand := true;
  mDefSize.h := 1;
end;


// ////////////////////////////////////////////////////////////////////////// //
procedure TUIVLine.AfterConstruction ();
begin
  mHoriz := false;
  mExpand := true;
  mDefSize.w := 1;
  //mDefSize.h := 8;
end;


// ////////////////////////////////////////////////////////////////////////// //
constructor TUITextLabel.Create (const atext: AnsiString);
begin
  inherited Create();
  mText := atext;
  mDefSize := TLaySize.Create(Length(atext)*8, 8);
end;


procedure TUITextLabel.AfterConstruction ();
begin
  inherited AfterConstruction();
  mHAlign := -1;
  mVAlign := 0;
  mCanFocus := false;
  if (mDefSize.h <= 0) then mDefSize.h := 8;
end;


function TUITextLabel.parseProperty (const prname: AnsiString; par: TTextParser): Boolean;
begin
  if (strEquCI1251(prname, 'title')) or (strEquCI1251(prname, 'caption')) then
  begin
    mText := par.expectStrOrId(true);
    mDefSize := TLaySize.Create(Length(mText)*8, 8);
    result := true;
    exit;
  end;
  if (strEquCI1251(prname, 'textalign')) then
  begin
    parseTextAlign(par, mHAlign, mVAlign);
    result := true;
    exit;
  end;
  result := inherited parseProperty(prname, par);
end;


procedure TUITextLabel.drawControl (gx, gy: Integer);
var
  xpos, ypos: Integer;
begin
  // debug
  fillRect(gx, gy, mWidth, mHeight, TGxRGBA.Create(96, 96, 0));
  drawRectUI(gx, gy, mWidth, mHeight, TGxRGBA.Create(96, 96, 96));

  if (Length(mText) > 0) then
  begin
         if (mHAlign < 0) then xpos := 0
    else if (mHAlign > 0) then xpos := mWidth-Length(mText)*8
    else xpos := (mWidth-Length(mText)*8) div 2;

         if (mVAlign < 0) then ypos := 0
    else if (mVAlign > 0) then ypos := mHeight-8
    else ypos := (mHeight-8) div 2;

    drawText8(gx+xpos, gy+ypos, mText, TGxRGBA.Create(255, 255, 255));
  end;
end;


function TUITextLabel.mouseEvent (var ev: THMouseEvent): Boolean;
var
  lx, ly: Integer;
begin
  result := inherited mouseEvent(ev);
  if not result and toLocal(ev.x, ev.y, lx, ly) then
  begin
    result := true;
  end;
end;


function TUITextLabel.keyEvent (var ev: THKeyEvent): Boolean;
begin
  result := inherited keyEvent(ev);
end;


initialization
  registerCtlClass(TUIHBox, 'hbox');
  registerCtlClass(TUIVBox, 'vbox');
  registerCtlClass(TUISpan, 'span');
  registerCtlClass(TUIHLine, 'hline');
  registerCtlClass(TUIVLine, 'vline');
  registerCtlClass(TUITextLabel, 'label');
end.
