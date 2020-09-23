/*
  main.cpp - Main loop for Arduino sketches
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>
#include "Keyboard.h"

typedef uint8_t u8;

/**
 * Keyboard layout (col row):
 *
 * ┌────┐┌────┐┌────┐┌────┐┌────┐┌────┐ ┌────┐┌────┐┌────┐┌────┐┌────┐┌────┐
 * │A L1││B L1││C L1││D L1││E L1││F L1│ │F R1││E R1││D R1││C R1││B R1││A R1│
 * └────┘└────┘└────┘└────┘└────┘└────┘ └────┘└────┘└────┘└────┘└────┘└────┘
 * ┌────┐┌────┐┌────┐┌────┐┌────┐┌────┐ ┌────┐┌────┐┌────┐┌────┐┌────┐┌────┐
 * │A L2││B L2││C L2││D L2││E L2││F L2│ │F R2││E R2││D R2││C R2││B R2││A R2│
 * └────┘└────┘└────┘└────┘└────┘└────┘ └────┘└────┘└────┘└────┘└────┘└────┘
 * ┌────┐┌────┐┌────┐┌────┐┌────┐┌────┐ ┌────┐┌────┐┌────┐┌────┐┌────┐┌────┐
 * │A L3││B L3││C L3││D L3││E L3││F L3│ │F R3││E R3││D R3││C R3││B R3││A R3│
 * └────┘└────┘└────┘└────┘└────┘└────┘ └────┘└────┘└────┘└────┘└────┘└────┘
 * ┌────┐┌────┐┌────┐┌────┐┌────┐┌────┐ ┌────┐┌────┐┌────┐┌────┐┌────┐┌────┐
 * │A L4││B L4││C L4││D L4││E L4││F L4│ │F R4││E R4││D R4││C R4││B R4││A R4│
 * └────┘└────┘└────┘└────┘└────┘└────┘ └────┘└────┘└────┘└────┘└────┘└────┘
 *                   ┌────┐┌────┐┌────┐ ┌────┐┌────┐┌────┐
 *                   │D L5││E L5││F L5│ │F R5││E R5││D R5│
 *                   └────┘└────┘└────┘ └────┘└────┘└────┘
 */

/**
 * ┌                                           ┐
 * │ (A L1) (B L1) (C L1) (D L1) (E L1) (F L1) │
 * │ (A L2) (B L2) (C L2) (D L2) (E L2) (F L2) │
 * │ (A L3) (B L3) (C L3) (D L3) (E L3) (F L3) │
 * │ (A L4) (B L4) (C L4) (D L4) (E L4) (F L4) │
 * │                      (D L5) (E L5) (F L5) │
 * │ (A R1) (B R1) (C R1) (D R1) (E R1) (F R1) │
 * │ (A R2) (B R2) (C R2) (D R2) (E R2) (F R2) │
 * │ (A R3) (B R3) (C R3) (D R3) (E R3) (F R3) │
 * │ (A R4) (B R4) (C R4) (D R4) (E R4) (F R4) │
 * │                      (D R5) (E R5) (F R5) │
 * └                                           ┘
 */

/**
 * Arduino Pro Micro pinout:
 *
 *     ┌─────────────┐
 *     ┥ 1       RAW ┝
 *     ┥ 0       GND ┝
 *     ┥ GND     RST ┝
 *     ┥ GND     VCC ┝
 *   F ┥ 2        21 ┝ A
 *   E ┥ 3        20 ┝ B
 *   D ┥ 4        19 ┝ C
 *  L1 ┥ 5        18 ┝ R1
 *  L2 ┥ 6        15 ┝ R2
 *  L3 ┥ 7        14 ┝ R3
 *  L4 ┥ 8        16 ┝ R4
 *  L5 ┥ 9        10 ┝ R5
 *     └─────────────┘
 */

#define LAYOUT( K00, K01, K02, K03, K04, K05 \
	      , K10, K11, K12, K13, K14, K15 \
	      , K20, K21, K22, K23, K24, K25 \
	      , K30, K31, K32, K33, K34, K35 \
	      ,                K43, K44, K45 \
	      , K50, K51, K52, K53, K54, K55 \
	      , K60, K61, K62, K63, K64, K65 \
	      , K70, K71, K72, K73, K74, K75 \
	      , K80, K81, K82, K83, K84, K85 \
	      , K90, K91, K92              ) \
{ K00, K01, K02, K03, K04, K05 \
, K10, K11, K12, K13, K14, K15 \
, K20, K21, K22, K23, K24, K25 \
, K30, K31, K32, K33, K34, K35 \
,                K43, K44, K45 \
, K55, K54, K53, K52, K51, K50 \
, K65, K64, K63, K62, K61, K60 \
, K75, K74, K73, K72, K71, K70 \
, K85, K84, K83, K82, K81, K80 \
,                K92, K91, K90 }

const u8 cols[] = {
	21, // A
	20, // B
	19, // C
	4,  // D
	3,  // E
	2,  // F
};

const u8 rows[] = {
	5,  // L1
	6,  // L2
	7,  // L3
	8,  // L4
	9,  // L5
	18, // R1
	15, // R2
	14, // R3
	16, // R4
	10, // R5
};

static const u8 layout0[] = LAYOUT(
	KEY_ESC       , KEY_1       , KEY_2        , KEY_3       , KEY_4       , KEY_5        ,
	KEY_TAB       , KEY_Q       , KEY_W        , KEY_E       , KEY_R       , KEY_T        ,
	KEY_LEFT_CTRL , KEY_A       , KEY_S        , KEY_D       , KEY_F       , KEY_G        ,
	KEY_LEFT_SHIFT, KEY_Z       , KEY_X        , KEY_C       , KEY_V       , KEY_B        ,
	KEY_NULL      , KEY_NULL    , KEY_NULL     , KEY_LEFT_ALT, KEY_LEFT_GUI, KEY_SPACE    ,

	KEY_6         , KEY_7       , KEY_8        , KEY_9       , KEY_0       , KEY_BACKSPACE,
	KEY_Y         , KEY_U       , KEY_I        , KEY_O       , KEY_P       , KEY_BACKSLASH,
	KEY_H         , KEY_J       , KEY_K        , KEY_L       , KEY_COLON   , KEY_QUOTE    ,
	KEY_N         , KEY_M       , KEY_COMMA    , KEY_DOT     , KEY_SLASH   , KEY_RETURN   ,
	KEY_SPACE     , KEY_LBRACKET, KEY_RBRACKET', KEY_NULL    , KEY_NULL    , KEY_NULL     ,
);

const u8 ncols = sizeof(cols) / sizeof(cols[0]);
const u8 nrows = sizeof(rows) / sizeof(rows[0]);

u8 state[ncols * nrows] = {
	HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH,
	HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH,
	HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH,
	HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH,
	HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH,
	HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH,
};

int main(void)
{
	init();
	usb_device_attach();

#ifdef DEBUG
	while (!Serial);
	Serial.begin(9600);
	Serial.println("Hi");
#endif

	for (u8 i = 0; i < ncols; i++) pinMode(cols[i], INPUT_PULLUP);
	for (u8 i = 0; i < nrows; i++) pinMode(rows[i], OUTPUT);
	for (u8 i = 0; i < nrows; i++) digitalWrite(rows[i], HIGH);
	u8 row = 0, last;
	for (;;) {
		last = row;
		row = row == nrows - 1 ? 0 : row + 1;
		digitalWrite(rows[last], HIGH);
		digitalWrite(rows[row], LOW);
		for (u8 col = 0; col < ncols; col++) {
			u8 value = digitalRead(cols[col]);
			if (state[row * ncols + col] != value) {
				state[row * ncols + col] = value;
				u8 k = layout0[row * ncols + col];
#ifdef DEBUG
				Serial.print(row);
				Serial.print(' ');
				Serial.print(col);
				Serial.print(": ");
				Serial.print(value);
				Serial.print(' ');
				Serial.print((char)k);
				Serial.println();
#endif
				if (value == LOW)
					Keyboard.press(k);
				else
					Keyboard.release(k);
			}
		}
		if (serialEventRun) serialEventRun();
	}
        
	return 0;
}
