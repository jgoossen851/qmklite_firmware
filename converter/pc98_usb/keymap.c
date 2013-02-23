/*
Copyright 2012 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "keycode.h"
#include "util.h"
#include "keymap.h"




/* PC-9801-98-S02   Raku Raku keyboard(Luckyboard) Normal Mode
  ,---------------------------------------------------------------.
  | 60| 61| 62| 63| 64| 65| 66| 67| 68| 69| 6A| 6B| 36| 37| 3F| 3E|
  `---------------------------------------------------------------'
  ,---------------------------------------------------------------.
  |    00| 01| 02| 03| 04| 05|  58|  71| 06| 07| 08| 09| 0A|    0E|
  |---------------------------------------------------------------|
  |   0F| 10| 11| 12| 13| 14|    3A     | 15| 16| 17| 18| 19|   1C|
  |---------------------------------------------------------------|
  |  74| 20| 21| 22| 23| 24|  3B  |  3C  | 25| 26| 27| 28| 29|    |
  |---------------------------------------------------------------|
  | 70| 2A| 2B| 2C| 2D| 2E| 38|  3D   | 39| 2F| 30| 31| 32| 33| 70|
  `---------------------------------------------------------------'
          | 73|   51|   5B|   59|       34|   5A|   35| xx|
          `-----------------------------------------------'
  xx: 74 35 F4 B5
*/
#define KEYMAP( \
    K60, K61, K62, K63, K64, K65, K66, K67, K68, K69, K6A, K6B, K36, K37, K3F, K3E, \
      K00,  K01, K02, K03, K04, K05,  K58, K71,   K06, K07, K08, K09, K0A,    K0E,  \
      K0F,  K10, K11, K12, K13, K14,     K3A,     K15, K16, K17, K18, K19,    K1C,  \
      K74, K20, K21, K22, K23, K24,   K3B,  K3C,   K25, K26, K27, K28, K29,         \
      K70,K2A, K2B, K2C, K2D, K2E, K38,  K3D,  K39, K2F, K30, K31, K32, K33,        \
               K73,  K51, K5B, K59,        K34,        K5A,  K35                    \
) { \
    { KC_##K00, KC_##K01, KC_##K02, KC_##K03, KC_##K04, KC_##K05, KC_##K06, KC_##K07 }, \
    { KC_##K08, KC_##K09, KC_##K0A, KC_NO,    KC_NO,    KC_NO,    KC_##K0E, KC_##K0F }, \
    { KC_##K10, KC_##K11, KC_##K12, KC_##K13, KC_##K14, KC_##K15, KC_##K16, KC_##K17 }, \
    { KC_##K18, KC_##K19, KC_NO,    KC_NO,    KC_##K1C, KC_NO,    KC_NO,    KC_NO    }, \
    { KC_##K20, KC_##K21, KC_##K22, KC_##K23, KC_##K24, KC_##K25, KC_##K26, KC_##K27 }, \
    { KC_##K28, KC_##K29, KC_##K2A, KC_##K2B, KC_##K2C, KC_##K2D, KC_##K2E, KC_##K2F }, \
    { KC_##K30, KC_##K31, KC_##K32, KC_##K33, KC_##K34, KC_##K35, KC_##K36, KC_##K37 }, \
    { KC_##K38, KC_##K39, KC_##K3A, KC_##K3B, KC_##K3C, KC_##K3D, KC_##K3E, KC_##K3F }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_##K51, KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_NO,    KC_##K5A, KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_##K60, KC_##K61, KC_##K62, KC_##K63, KC_##K64, KC_##K65, KC_##K66, KC_##K67 }, \
    { KC_##K68, KC_##K69, KC_##K6A, KC_##K6B, KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_##K70, KC_NO,    KC_NO,    KC_##K73, KC_##K74, KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }  \
}




// Assign Fn key(0-7) to a layer to which switch with the Fn key pressed.
static const uint8_t PROGMEM fn_layer[] = {
    2,              // Fn0
    3,              // Fn1
    4,              // Fn2
    0,              // Fn3
    0,              // Fn4
    0,              // Fn5
    0,              // Fn6
    0               // Fn7
};

// Assign Fn key(0-7) to a keycode sent when release Fn key without use of the layer.
// See layer.c for details.
static const uint8_t PROGMEM fn_keycode[] = {
    KC_NO,          // Fn0
    KC_SCLN,        // Fn1
    KC_SLSH,        // Fn2
    KC_NO,          // Fn3
    KC_NO,          // Fn4
    KC_NO,          // Fn5
    KC_NO,          // Fn6
    KC_NO           // Fn7
};


static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
  ,---------------------------------------------------------------.
  | 60| 61| 62| 63| 64| 65| 66| 67| 68| 69| 6A| 6B| 36| 37| 3F| 3E|
  `---------------------------------------------------------------'
  ,---------------------------------------------------------------.
  |    00| 01| 02| 03| 04| 05|  58|  71| 06| 07| 08| 09| 0A|    0E|
  |---------------------------------------------------------------|
  |   0F| 10| 11| 12| 13| 14|    3A     | 15| 16| 17| 18| 19|   1C|
  |---------------------------------------------------------------|
  |  74| 20| 21| 22| 23| 24|  MINS|   EQL| 25| 26| 27| 28| 29|    |
  |---------------------------------------------------------------|
  | 70| 2A| 2B| 2C| 2D| 2E| 38|  3D   | 39| 2F| 30| 31| 32| 33| 70|
  `---------------------------------------------------------------'
          | 73|   51|   5B|   59|       34|   5A|   35| xx|
          `-----------------------------------------------'
    */
    KEYMAP(
    PAUS,COPY,  F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9, F10, F11, F12, F13, F14,
      ESC,    1,   2,   3,   4,   5,    NO, NO,     6,   7,   8,   9,   0,   BSPC,
      TAB,    Q,   W,   E,   R,   T,      UP,       Y,   U,   I,   O,   P,   ENT,
      LCTL,  A,   S,   D,   F,   G,  MINS,  EQL,     H,   J,   K,   L,SCLN,
      LSFT, Z,   X,   C,   V,   B, INS, DOWN,  DEL,   N,   M,COMM, DOT,SLSH,
              LGUI, LALT, LCTL, LSFT,      SPC,      SPC,   RALT
    ),
};


uint8_t keymap_get_keycode(uint8_t layer, uint8_t row, uint8_t col)
{
    return pgm_read_byte(&keymaps[(layer)][(row)][(col)]);
}

uint8_t keymap_fn_layer(uint8_t index)
{
    return pgm_read_byte(&fn_layer[index]);
}

uint8_t keymap_fn_keycode(uint8_t index)
{
    return pgm_read_byte(&fn_keycode[index]);
}


#if 0
/* PC-9801-98-S02   Raku Raku keyboard(Luckyboard) M-siki mode
  ,---------------------------------------------------------------.
  | 60| 61| 62| 63| 64| 65| 66| 67| 68| 69| 6A| 6B| 36| 37| 3F| 3E|
  `---------------------------------------------------------------'
  ,---------------------------------------------------------------.
  |    00| 01| 02| 03| 04| 05| NUM|CAPS| 06| 07| 08| 09| 0A|    0E|
  |---------------------------------------------------------------|
  |   0F| 10| 25| 20| 23| 2B|    3A     | 2F| 15| 13| 11| 19|   1C|
  |---------------------------------------------------------------|
  |  74| 12| 16| 17| 1D| 18|  3B  |  3C  | 24| 1E| 14| 2E| 22|    |
  |---------------------------------------------------------------|
  | 70| xx| 2A| 2C| xx| xx| 38|  3D   | 39| 21| 29| 1F| xx| 2D| 70|
  `---------------------------------------------------------------'
          | 73|   51|   xx|   xx|       34|   xx|   35| xx|
          `-----------------------------------------------'
*/

#define KEYMAP_M( \
    K60, K61, K62, K63, K64, K65, K66, K67, K68, K69, K6A, K6B, K36, K37, K3F, K3E, \
      K00,  K01, K02, K03, K04, K05,              K06, K07, K08, K09, K0A,    K0E,  \
      K0F,  K10, K25, K23, K20, K2B,     K3A,     K2F, K15, K13, K11, K19,    K1C,  \
      K74, K12, K16, K17, K1D, K18,   K3B,  K3C,   K24, K1E, K14, K2E, K22,         \
      K70,     K2A, K2C,           K38,  K3D,  K39, K21, K29, K1F,      K2D,        \
               K73,  K51,                  K34,              K35                    \
) { \
    { KC_##K00, KC_##K01, KC_##K02, KC_##K03, KC_##K04, KC_##K05, KC_##K06, KC_##K07 }, \
    { KC_##K08, KC_##K09, KC_##K0A, KC_NO,    KC_NO,    KC_NO,    KC_##K0E, KC_##K0F }, \
    { KC_##K10, KC_##K11, KC_##K12, KC_##K13, KC_##K14, KC_##K15, KC_##K16, KC_##K17 }, \
    { KC_##K18, KC_##K19, KC_NO,    KC_NO,    KC_##K1C, KC_##K1D, KC_##K1E, KC_##K1F }, \
    { KC_##K20, KC_##K21, KC_##K22, KC_##K23, KC_##K24, KC_##K25, KC_NO,    KC_NO    }, \
    { KC_NO,    KC_##K29, KC_##K2A, KC_##K2B, KC_##K2C, KC_##K2D, KC_##K2E, KC_##K2F }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_##K34, KC_##K35, KC_##K36, KC_##K37 }, \
    { KC_##K38, KC_##K39, KC_##K3A, KC_##K3B, KC_##K3C, KC_##K3D, KC_##K3E, KC_##K3F }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_##K51, KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_##K60, KC_##K61, KC_##K62, KC_##K63, KC_##K64, KC_##K65, KC_##K66, KC_##K67 }, \
    { KC_##K68, KC_##K69, KC_##K6A, KC_##K6B, KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_##K70, KC_NO,    KC_NO,    KC_##K73, KC_##K74, KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }  \
}
#endif
