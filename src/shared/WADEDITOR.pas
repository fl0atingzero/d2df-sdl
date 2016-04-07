unit WADEDITOR;

{
-----------------------------------
WADEDITOR.PAS ������ �� 26.08.08

��������� ����� ������ 1
-----------------------------------
}

interface

uses WADSTRUCT;

type
  SArray = array of ShortString;

  TWADEditor_1 = class(TObject)
   private
    FResData:   Pointer;
    FResTable:  packed array of TResourceTableRec_1;
    FHeader:    TWADHeaderRec_1;
    FDataSize:  LongWord;
    FOffset:    LongWord;
    FFileName:  string;
    FWADOpened: Byte;
    FLastError: Integer;
    FVersion:   Byte;
    function LastErrorString(): string;
    function GetResName(ResName: string): Char16;
   public
    constructor Create();
    destructor Destroy(); override;
    procedure FreeWAD();
    function  ReadFile(FileName: string): Boolean;
    function  ReadMemory(Data: Pointer; Len: LongWord): Boolean;
    function HaveResource(Section, Resource: string): Boolean;
    function HaveSection(Section: string): Boolean;
    function GetResource(Section, Resource: string; var pData: Pointer;
                         var Len: Integer): Boolean;
    function GetSectionList(): SArray;
    function GetResourcesList(Section: string): SArray;

    property GetLastError: Integer read FLastError;
    property GetLastErrorStr: string read LastErrorString;
    property GetResourcesCount: Word read FHeader.RecordsCount;
    property GetVersion: Byte read FVersion;
  end;

const
  DFWAD_NOERROR                = 0;
  DFWAD_ERROR_WADNOTFOUND      = -1;
  DFWAD_ERROR_CANTOPENWAD      = -2;
  DFWAD_ERROR_RESOURCENOTFOUND = -3;
  DFWAD_ERROR_FILENOTWAD       = -4;
  DFWAD_ERROR_WADNOTLOADED     = -5;
  DFWAD_ERROR_READRESOURCE     = -6;
  DFWAD_ERROR_READWAD          = -7;
  DFWAD_ERROR_WRONGVERSION     = -8;


 procedure g_ProcessResourceStr(ResourceStr: String; var FileName,
                                SectionName, ResourceName: String); overload;
 procedure g_ProcessResourceStr(ResourceStr: String; FileName,
                                SectionName, ResourceName: PString); overload;

implementation

uses
  SysUtils, BinEditor, ZLib;

const
  DFWAD_OPENED_NONE   = 0;
  DFWAD_OPENED_FILE   = 1;
  DFWAD_OPENED_MEMORY = 2;

procedure DecompressBuf(const InBuf: Pointer; InBytes: Integer;
  OutEstimate: Integer; out OutBuf: Pointer; out OutBytes: Integer);
var
  strm: TZStreamRec;
  P: Pointer;
  BufInc: Integer;
begin
  FillChar(strm, sizeof(strm), 0);
  BufInc := (InBytes + 255) and not 255;
  if OutEstimate = 0 then
    OutBytes := BufInc
  else
    OutBytes := OutEstimate;
  GetMem(OutBuf, OutBytes);
  try
    strm.next_in := InBuf;
    strm.avail_in := InBytes;
    strm.next_out := OutBuf;
    strm.avail_out := OutBytes;
    inflateInit_(strm, zlib_version, sizeof(strm));
    try
      while inflate(strm, Z_FINISH) <> Z_STREAM_END do
      begin
        P := OutBuf;
        Inc(OutBytes, BufInc);
        ReallocMem(OutBuf, OutBytes);
        strm.next_out := PByteF(PChar(OutBuf) + (PChar(strm.next_out) - PChar(P)));
        strm.avail_out := BufInc;
      end;
    finally
      inflateEnd(strm);
    end;
    ReallocMem(OutBuf, strm.total_out);
    OutBytes := strm.total_out;
  except
    FreeMem(OutBuf);
    raise
  end;
end;

procedure g_ProcessResourceStr(ResourceStr: String; var FileName,
                               SectionName, ResourceName: String);
var
  a, i: Integer;

begin
  for i := Length(ResourceStr) downto 1 do
    if ResourceStr[i] = ':' then
      Break;

  FileName := Copy(ResourceStr, 1, i-1);

  for a := i+1 to Length(ResourceStr) do
    if (ResourceStr[a] = '\') or (ResourceStr[a] = '/') then Break;

  ResourceName := Copy(ResourceStr, a+1, Length(ResourceStr)-Abs(a));
  SectionName := Copy(ResourceStr, i+1, Length(ResourceStr)-Length(ResourceName)-Length(FileName)-2);
end;

procedure g_ProcessResourceStr(ResourceStr: AnsiString; FileName,
                               SectionName, ResourceName: PAnsiString);
var
  a, i, l1, l2: Integer;

begin
  for i := Length(ResourceStr) downto 1 do
    if ResourceStr[i] = ':' then
      Break;

  if FileName <> nil then
    begin
      FileName^ := Copy(ResourceStr, 1, i-1);
      l1 := Length(FileName^);
    end
  else
    l1 := 0;

  for a := i+1 to Length(ResourceStr) do
    if (ResourceStr[a] = '\') or (ResourceStr[a] = '/') then Break;

  if ResourceName <> nil then
    begin
      ResourceName^ := Copy(ResourceStr, a+1, Length(ResourceStr)-Abs(a));
      l2 := Length(ResourceName^);
    end
  else
    l2 := 0;

  if SectionName <> nil then
    SectionName^ := Copy(ResourceStr, i+1, Length(ResourceStr)-l2-l1-2);
end;


{ TWADEditor_1 }

constructor TWADEditor_1.Create();
begin
 FResData := nil;
 FResTable := nil;
 FDataSize := 0;
 FOffset := 0;
 FHeader.RecordsCount := 0;
 FFileName := '';
 FWADOpened := DFWAD_OPENED_NONE;
 FLastError := DFWAD_NOERROR;
 FVersion := DFWAD_VERSION;
end;

destructor TWADEditor_1.Destroy();
begin
 FreeWAD();

 inherited;
end;

procedure TWADEditor_1.FreeWAD();
begin
 if FResData <> nil then FreeMem(FResData);
 FResTable := nil;
 FDataSize := 0;
 FOffset := 0;
 FHeader.RecordsCount := 0;
 FFileName := '';
 FWADOpened := DFWAD_OPENED_NONE;
 FLastError := DFWAD_NOERROR;
 FVersion := DFWAD_VERSION;
end;

function TWADEditor_1.GetResName(ResName: string): Char16;
begin
 ZeroMemory(@Result[0], 16);
 if ResName = '' then Exit;

 ResName := Trim(UpperCase(ResName));
 if Length(ResName) > 16 then SetLength(ResName, 16);

 CopyMemory(@Result[0], @ResName[1], Length(ResName));
end;

function TWADEditor_1.HaveResource(Section, Resource: string): Boolean;
var
  a: Integer;
  CurrentSection: string;
begin
 Result := False;

 if FResTable = nil then Exit;

 CurrentSection := '';
 Section := AnsiUpperCase(Section);
 Resource := AnsiUpperCase(Resource);

 for a := 0 to High(FResTable) do
 begin
  if FResTable[a].Length = 0 then
  begin
   CurrentSection := FResTable[a].ResourceName;
   Continue;
  end;

  if (FResTable[a].ResourceName = Resource) and
     (CurrentSection = Section) then
  begin
   Result := True;
   Break;
  end;
 end;
end;

function TWADEditor_1.HaveSection(Section: string): Boolean;
var
  a: Integer;
begin
 Result := False;

 if FResTable = nil then Exit;
 if Section = '' then
 begin
  Result := True;
  Exit;
 end;

 Section := AnsiUpperCase(Section);

 for a := 0 to High(FResTable) do
  if (FResTable[a].Length = 0) and (FResTable[a].ResourceName = Section) then
  begin
   Result := True;
   Exit;
  end;
end;

function TWADEditor_1.GetResource(Section, Resource: string;
  var pData: Pointer; var Len: Integer): Boolean;
var
  a: LongWord;
  i: Integer;
  WADFile: File;
  CurrentSection: string;
  TempData: Pointer;
  OutBytes: Integer;
begin
 Result := False;

 CurrentSection := '';

 if FWADOpened = DFWAD_OPENED_NONE then
 begin
  FLastError := DFWAD_ERROR_WADNOTLOADED;
  Exit;
 end;

 Section := UpperCase(Section);
 Resource := UpperCase(Resource);

 i := -1;
 for a := 0 to High(FResTable) do
 begin
  if FResTable[a].Length = 0 then
  begin
   CurrentSection := FResTable[a].ResourceName;
   Continue;
  end;

  if (FResTable[a].ResourceName = Resource) and
     (CurrentSection = Section) then
  begin
   i := a;
   Break;
  end;
 end;

 if i = -1 then
 begin
  FLastError := DFWAD_ERROR_RESOURCENOTFOUND;
  Exit;
 end;

 if FWADOpened = DFWAD_OPENED_FILE then
 begin
  try
   AssignFile(WADFile, FFileName);
   Reset(WADFile, 1);

   Seek(WADFile, FResTable[i].Address+6+
        LongWord(SizeOf(TWADHeaderRec_1)+SizeOf(TResourceTableRec_1)*Length(FResTable)));
   TempData := GetMemory(FResTable[i].Length);
   BlockRead(WADFile, TempData^, FResTable[i].Length);
   DecompressBuf(TempData, FResTable[i].Length, 0, pData, OutBytes);
   FreeMem(TempData);

   Len := OutBytes;

   CloseFile(WADFile);
  except
   FLastError := DFWAD_ERROR_CANTOPENWAD;
   CloseFile(WADFile);
   Exit;
  end;
 end
  else
 begin
  TempData := GetMemory(FResTable[i].Length);
  CopyMemory(TempData, Pointer(LongWord(FResData)+FResTable[i].Address+6+
             LongWord(SizeOf(TWADHeaderRec_1)+SizeOf(TResourceTableRec_1)*Length(FResTable))),
             FResTable[i].Length);
  DecompressBuf(TempData, FResTable[i].Length, 0, pData, OutBytes);
  FreeMem(TempData);

  Len := OutBytes;
 end;

 FLastError := DFWAD_NOERROR;
 Result := True;
end;

function TWADEditor_1.GetResourcesList(Section: string): SArray;
var
  a: Integer;
  CurrentSection: Char16;
begin
 Result := nil;

 if FResTable = nil then Exit;
 if Length(Section) > 16 then Exit;

 CurrentSection := '';

 for a := 0 to High(FResTable) do
 begin
  if FResTable[a].Length = 0 then
  begin
   CurrentSection := FResTable[a].ResourceName;
   Continue;
  end;

  if CurrentSection = Section then
  begin
   SetLength(Result, Length(Result)+1);
   Result[High(Result)] := FResTable[a].ResourceName;
  end;
 end;
end;

function TWADEditor_1.GetSectionList(): SArray;
var
  i: DWORD;
begin
 Result := nil;

 if FResTable = nil then Exit;

 if FResTable[0].Length <> 0 then
 begin
  SetLength(Result, 1);
  Result[0] := '';
 end;

 for i := 0 to High(FResTable) do
  if FResTable[i].Length = 0 then
  begin
   SetLength(Result, Length(Result)+1);
   Result[High(Result)] := FResTable[i].ResourceName;
  end;
end;

function TWADEditor_1.LastErrorString(): string;
begin
 case FLastError of
  DFWAD_NOERROR: Result := '';
  DFWAD_ERROR_WADNOTFOUND: Result := 'DFWAD file not found';
  DFWAD_ERROR_CANTOPENWAD: Result := 'Can''t open DFWAD file';
  DFWAD_ERROR_RESOURCENOTFOUND: Result := 'Resource not found';
  DFWAD_ERROR_FILENOTWAD: Result := 'File is not DFWAD';
  DFWAD_ERROR_WADNOTLOADED: Result := 'DFWAD file is not loaded';
  DFWAD_ERROR_READRESOURCE: Result := 'Read resource error';
  DFWAD_ERROR_READWAD: Result := 'Read DFWAD error';
  else Result := 'Unknown DFWAD error';
 end;
end;

function TWADEditor_1.ReadFile(FileName: string): Boolean;
var
  WADFile: File;
  Signature: array[0..4] of Char;
  a: Integer;
begin
 FreeWAD();

 Result := False;

 if not FileExists(FileName) then
 begin
  FLastError := DFWAD_ERROR_WADNOTFOUND;
  Exit;
 end;

 FFileName := FileName;

 AssignFile(WADFile, FFileName);

 try
  Reset(WADFile, 1);
 except
  FLastError := DFWAD_ERROR_CANTOPENWAD;
  Exit;
 end;

 try
  BlockRead(WADFile, Signature, 5);
  if Signature <> DFWAD_SIGNATURE then
  begin
   FLastError := DFWAD_ERROR_FILENOTWAD;
   CloseFile(WADFile);
   Exit;
  end;

  BlockRead(WADFile, FVersion, 1);
  if FVersion <> DFWAD_VERSION then
  begin
    FLastError := DFWAD_ERROR_WRONGVERSION;
    CloseFile(WADFile);
    Exit;
  end;

  BlockRead(WADFile, FHeader, SizeOf(TWADHeaderRec_1));
  SetLength(FResTable, FHeader.RecordsCount);
  if FResTable <> nil then
  begin
   BlockRead(WADFile, FResTable[0], SizeOf(TResourceTableRec_1)*FHeader.RecordsCount);

   for a := 0 to High(FResTable) do
    if FResTable[a].Length <> 0 then
     FResTable[a].Address := FResTable[a].Address-6-(LongWord(SizeOf(TWADHeaderRec_1)+
                             SizeOf(TResourceTableRec_1)*Length(FResTable)));
  end;

  CloseFile(WADFile);
 except
  FLastError := DFWAD_ERROR_READWAD;
  CloseFile(WADFile);
  Exit;
 end;

 FWADOpened := DFWAD_OPENED_FILE;
 FLastError := DFWAD_NOERROR;
 Result := True;
end;

function TWADEditor_1.ReadMemory(Data: Pointer; Len: LongWord): Boolean;
var
  Signature: array[0..4] of Char;
  a: Integer;
begin
 FreeWAD();

 Result := False;

 CopyMemory(@Signature[0], Data, 5);
 if Signature <> DFWAD_SIGNATURE then
 begin
  FLastError := DFWAD_ERROR_FILENOTWAD;
  Exit;
 end;

 CopyMemory(@FVersion, Pointer(LongWord(Data)+5), 1);
 if FVersion <> DFWAD_VERSION then
 begin
   FLastError := DFWAD_ERROR_WRONGVERSION;
   Exit;
 end;

 CopyMemory(@FHeader, Pointer(LongWord(Data)+6), SizeOf(TWADHeaderRec_1));

 SetLength(FResTable, FHeader.RecordsCount);
 if FResTable <> nil then
 begin
  CopyMemory(@FResTable[0], Pointer(LongWord(Data)+6+SizeOf(TWADHeaderRec_1)),
             SizeOf(TResourceTableRec_1)*FHeader.RecordsCount);

  for a := 0 to High(FResTable) do
   if FResTable[a].Length <> 0 then
    FResTable[a].Address := FResTable[a].Address-6-(LongWord(SizeOf(TWADHeaderRec_1)+
                            SizeOf(TResourceTableRec_1)*Length(FResTable)));
 end;

 GetMem(FResData, Len);
 CopyMemory(FResData, Data, Len);

 FWADOpened := DFWAD_OPENED_MEMORY;
 FLastError := DFWAD_NOERROR;

 Result := True;
end;

end.
