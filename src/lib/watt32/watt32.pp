{$MODE OBJFPC}
{$PACKRECORDS C}

{$MODESWITCH OUT}
{$LONGSTRINGS ON}
{$MACRO ON}

{$IF DEFINED(GO32V2)}
  {$LINKLIB libwatt.a}
  {$DEFINE LibraryLibWattDecl := cdecl}
  {$DEFINE LibraryLibWattImp := cdecl; external}
  {$DEFINE LibraryLibWattVar := cvar; external}
{$ELSE}
  {$ERROR what?}
{$ENDIF}

unit watt32;

interface

  uses ctypes;

  const
    FD_MAXFDSET = 512; (* FD_SETSIZE *)

  type
    (* Watt32 -> sys/wtypes.h -> fd_set *)
    TFDSet = record
      fd_bits: array [0..63] of cuchar;
    end;

    tcp_Socket = record
      undoc: array [0..4469] of cuchar;
    end;

    udp_Socket = record
      undoc: array [0..1739] of cuchar;
    end;

    Psock_type = Pointer;

  var
    my_ip_addr: culong; external name '__w32_my_ip_addr';

  function wattcpCopyright: PChar; LibraryLibWattImp;
  function wattcpVersion: PChar; LibraryLibWattImp;
  function wattcpCapabilities: PChar; LibraryLibWattImp;

  function watt_sock_init (tcp, udp: csize_t): cint; LibraryLibWattImp;
  function sock_init_err: PChar; LibraryLibWattImp;
  procedure sock_exit; LibraryLibWattImp;
  procedure dbug_init; LibraryLibWattImp;
  procedure init_misc; LibraryLibWattImp;
  procedure sock_sig_exit (const msg: PChar; sigint: cint); LibraryLibWattImp;

  function hires_timer (on: cint): cint; LibraryLibWattImp name '_w32_hires_timer';
  procedure init_userSuppliedTimerTick; LibraryLibWattImp;
  procedure userTimerTick (elapsed_time_msec: culong); LibraryLibWattImp;
  procedure init_timer_isr; LibraryLibWattImp name '_w32_init_timer_isr';
  procedure exit_timer_isr; LibraryLibWattImp name '_w32_exit_timer_isr';

  function tcp_tick (s: Psock_type): culong; LibraryLibWattImp;


  function htons (hostshort: cuint16): cuint16; LibraryLibWattImp;
  function htonl (hostlong: cuint32): cuint32; LibraryLibWattImp;
  function ntohs (netshort: cuint16): cuint16; LibraryLibWattImp;
  function ntohl (netlong: cuint32): cuint32; LibraryLibWattImp;

  (* MACRO *)
  function sock_init: cint;
  function fpFD_SET (fdno: cint; var nset: TFDSet): cint;
  function fpFD_CLR (fdno:cint; var nset: TFDSet): cint;
  function fpFD_ZERO (out nset: TFDSet): cint;
  function fpFD_ISSET (fdno: cint;const nset: TFDSet): cint;

implementation

  function sock_init: cint;
  begin
    sock_init := watt_sock_init(sizeof(tcp_Socket), sizeof(udp_Socket))
  end;

  function fpFD_SET (fdno: cint; var nset: TFDSet): cint;
  begin
    if (fdno < 0) or (fdno > FD_MAXFDSET) then
      exit(-1);
    nset.fd_bits[fdno div 8] := nset.fd_bits[fdno div 8] OR (culong(1) shl (fdno and 7));
    fpFD_SET := 0
  end;

  function fpFD_CLR (fdno: cint; var nset: TFDSet): cint;
  begin
    if (fdno < 0) or (fdno > FD_MAXFDSET) Then
      exit(-1);
    nset.fd_bits[fdno div 8] := nset.fd_bits[fdno div 8] AND Cardinal(NOT (culong(1) shl (fdno and 7)));
    fpFD_CLR := 0
  end;

  function fpFD_ZERO (out nset: TFDSet): cint;
    var i :longint;
  begin
    for i := 0 to (512 + 7) div 8 DO
      nset.fd_bits[i] := 0;
    fpFD_ZERO := 0
  end;

  function fpFD_ISSET (fdno: cint; const nset: TFDSet): cint;
  begin
    if (fdno < 0) or (fdno > FD_MAXFDSET) Then
      exit(-1);
    if ((nset.fd_bits[fdno div 8]) and (culong(1) shl (fdno and 7))) > 0 then
      fpFD_ISSET := 1
    else
      fpFD_ISSET := 0
  end;

end.
