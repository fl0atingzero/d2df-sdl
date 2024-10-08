(* Copyright (C) 2016 - The Doom2D.org team & involved community members <http://www.doom2d.org>.
 * This file is part of Doom2D Forever.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation, version 3 of
 * the License ONLY.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *)

// compiler options, common for all game modules
{$MODE OBJFPC}

{$MODESWITCH ADVANCEDRECORDS+}
{$MODESWITCH ALLOWINLINE+}
{$MODESWITCH ANSISTRINGS+} // Allow use of ansistrings.
{$MODESWITCH AUTODEREF+} // Automatic (silent) dereferencing of typed pointers.
{$MODESWITCH CLASS+}
{$MODESWITCH CLASSICPROCVARS+} // Use classical procedural variables.
{$MODESWITCH DEFAULTPARAMETERS+} // Allow use of default parameter values.
{$MODESWITCH DUPLICATELOCALS-} // Allow local variables in class methods to have the same names as properties of the class.
{$MODESWITCH EXCEPTIONS+}
{$MODESWITCH HINTDIRECTIVE+} // Support the hint directives (deprecated, platform etc.)
{$MODESWITCH INITFINAL+} // Allow use of Initialization and Finalization
{.$MODESWITCH ISOUNARYMINUS-} // Unary minus as required by ISO pascal.
{$MODESWITCH MACPROCVARS-} // Use mac-style procedural variables.
{$MODESWITCH NESTEDCOMMENTS-}
{$MODESWITCH NESTEDPROCVARS+}
{$MODESWITCH OBJPAS+}
{$MODESWITCH OUT+} // Allow use of the out parameter type.
{$MODESWITCH PCHARTOSTRING+}
{$MODESWITCH POINTERTOPROCVAR+} // Allow silent conversion of pointers to procedural variables.
{$MODESWITCH PROPERTIES+}
{$MODESWITCH REPEATFORWARD+} // Implementation and Forward declaration must match completely.
{$MODESWITCH RESULT+}
{$MODESWITCH TYPEHELPERS+} // Allow the use of type helpers.
{$MODESWITCH UNICODESTRINGS-}


{$ASSERTIONS ON}
{$BITPACKING OFF}
{$BOOLEVAL OFF}
{$COPERATORS ON}
{$EXTENDEDSYNTAX ON}
{$IFDEF CPU32}
  {$FPUTYPE SSE}
{$ENDIF CPU32}
{$GOTO ON}
{$IEEEERRORS OFF}
{$INLINE ON}
{$LONGSTRINGS ON}
{$MACRO OFF}
{$OBJECTCHECKS OFF}
{$OVERFLOWCHECKS OFF}
{$POINTERMATH ON}
{$RANGECHECKS OFF}
{$SAFEFPUEXCEPTIONS OFF}
{$SCOPEDENUMS ON} // this may be changed later
{$SMARTLINK ON}
{$TYPEDADDRESS ON}
{$TYPEINFO ON}
{$VARSTRINGCHECKS OFF}

{$S-} // disable stack checking
{$MMX-} // get lost, mmx

{$IF DEFINED(D2F_DEBUG)}
  {$STACKFRAMES ON}
  {$HINTS OFF}
{$ELSE}
  {$STACKFRAMES OFF}
  {$HINTS OFF}
  {$DEFINE D2F_MORE_OPTIM}
{$ENDIF}
{$WARNINGS ON}
{$NOTES ON}

{$IF DEFINED(D2F_DEBUG_OPTIM) or DEFINED(D2F_MORE_OPTIM)}
  {$OPTIMIZATION DEADVALUES}
  {$OPTIMIZATION CONSTPROP}
  {$OPTIMIZATION DEADSTORE}
{$ENDIF}

{$IFDEF WIN32}
  {$IFNDEF MSWINDOWS}
    {$DEFINE MSWINDOWS}
  {$ENDIF}
{$ENDIF}

{$IFDEF MSWINDOWS}
  {$IFNDEF WINDOWS}
    {$DEFINE WINDOWS}
  {$ENDIF WINDOWS}
{$ENDIF}
