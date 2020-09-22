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
 * Layout is topologically equivalent to a 10x6 matrix:
 *
 * ┌                                           ┐
 * │                      (D R5) (E R5) (F R5) │
 * │ (A R4) (B R4) (C R4) (D R4) (E R4) (F R4) │
 * │ (A R3) (B R3) (C R3) (D R3) (E R3) (F R3) │
 * │ (A R2) (B R2) (C R2) (D R2) (E R2) (F R2) │
 * │ (A R1) (B R1) (C R1) (D R1) (E R1) (F R1) │
 * │ (A L1) (B L1) (C L1) (D L1) (E L1) (F L1) │
 * │ (A L2) (B L2) (C L2) (D L2) (E L2) (F L2) │
 * │ (A L3) (B L3) (C L3) (D L3) (E L3) (F L3) │
 * │ (A L4) (B L4) (C L4) (D L4) (E L4) (F L4) │
 * │                      (D L5) (E L5) (F L5) │
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

const u8 cols[] = {
	21, // A
	20, // B
	19, // C
	4,  // D
	3,  // E
	2,  // F
};

const u8 rows[] = {
        10, // R5
        16, // R4
        14, // R3
        15, // R2
        18, // R1
	5,  // L1
	6,  // L2
	7,  // L3
	8,  // L4
	9,  // L5
};

static const u8 layout0[] = {
	'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l',
	'm', 'n', 'o', 'p', 'q', 'r',
	's', 't', 'u', 'v', 'w', 'x',
	'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9',
	'~', '!', '@', '#', '$', '%',
	'^', '&', '*', '(', ')', '-',
	'+', '[', ']', '{', '}', '/',
	',', ';', '.', '"', '`', '=',
};

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
