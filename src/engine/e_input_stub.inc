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

const
  e_MaxKbdKeys = 256;

  // these are apparently used in g_gui and g_game and elsewhere
  IK_INVALID = 0;
  IK_ESCAPE  = 1;
  IK_RETURN  = 2;
  IK_KPRETURN= 3;
  IK_ENTER   = IK_RETURN;
  IK_KPINSERT = 5;
  IK_UP      = 6;
  IK_KPUP    = 7;
  IK_DOWN    = 8;
  IK_KPDOWN  = 9;
  IK_LEFT    = 10;
  IK_KPLEFT  = 11;
  IK_RIGHT   = 12;
  IK_KPRIGHT = 13;
  IK_DELETE  = 14;
  IK_HOME    = 15;
  IK_KPHOME  = 16;
  IK_INSERT  = 17;
  IK_SPACE   = 18;
  IK_SHIFT   = 20;
  IK_ALT     = 21;
  IK_TAB     = 22;
  IK_PAGEUP  = 23;
  IK_KPPAGEUP= 24;
  IK_PAGEDN  = 25;
  IK_KPPAGEDN= 26;
  IK_KP5     = 27;
  IK_NUMLOCK = 28;
  IK_KPDIVIDE= 29;
  IK_KPMULTIPLE= 30;
  IK_KPMINUS = 31;
  IK_KPPLUS  = 32;
  IK_KPENTER = IK_KPRETURN;
  IK_KPDOT   = 34;
  IK_CAPSLOCK= 35;
  IK_RSHIFT  = 36;
  IK_CTRL    = 37;
  IK_RCTRL   = 38;
  IK_RALT    = 39;
  IK_WIN     = 40;
  IK_RWIN    = 41;
  IK_MENU    = 42;
  IK_PRINTSCR= 43;
  IK_SCROLLLOCK= 44;
  IK_LBRACKET= 45;
  IK_RBRACKET= 46;
  IK_SEMICOLON= 47;
  IK_QUOTE   = 48;
  IK_BACKSLASH= 49;
  IK_SLASH   = 50;
  IK_COMMA   = 51;
  IK_DOT     = 52;
  IK_EQUALS  = 53;
  IK_0       = 54;
  IK_1       = 55;
  IK_2       = 56;
  IK_3       = 57;
  IK_4       = 58;
  IK_5       = 59;
  IK_6       = 60;
  IK_7       = 61;
  IK_8       = 62;
  IK_9       = 63;
  IK_F1      = 64;
  IK_F2      = 65;
  IK_F3      = 66;
  IK_F4      = 67;
  IK_F5      = 68;
  IK_F6      = 69;
  IK_F7      = 70;
  IK_F8      = 71;
  IK_F9      = 72;
  IK_F10     = 73;
  IK_F11     = 74;
  IK_F12     = 75;
  IK_END     = 76;
  IK_KPEND   = 77;
  IK_BACKSPACE = 78;
  IK_BACKQUOTE = 79;
  IK_GRAVE     = IK_BACKQUOTE;
  IK_PAUSE   = 81;
  IK_A       = 82;
  IK_B       = 83;
  IK_C       = 84;
  IK_D       = 85;
  IK_E       = 86;
  IK_F       = 87;
  IK_G       = 88;
  IK_H       = 89;
  IK_I       = 90;
  IK_J       = 91;
  IK_K       = 92;
  IK_L       = 93;
  IK_M       = 94;
  IK_N       = 95;
  IK_O       = 96;
  IK_P       = 97;
  IK_Q       = 98;
  IK_R       = 99;
  IK_S       = 100;
  IK_T       = 101;
  IK_U       = 102;
  IK_V       = 103;
  IK_W       = 104;
  IK_X       = 105;
  IK_Y       = 106;
  IK_Z       = 107;
  IK_MINUS   = 108;
  IK_NONUSBACKSLASH = 111;
  IK_SELECT  = 112;
  // TODO: think of something better than this shit
  IK_LASTKEY = e_MaxKbdKeys-1;
