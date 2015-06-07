/*
Copyright 2012,2013 Jun Wako <wakojun@gmail.com>
Copyright 2015 woremacx <info@woremacx.com>

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

/*
 * Keymap for bskbb14 mod
 */
#include <stdint.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "keycode.h"
#include "print.h"
#include "debug.h"
#include "util.h"
#include "keymap.h"

/*
 * Layout: 32key
 * ,----------------------------
 * |Esc |  1|  2|  3|  4|  5|  6|
 * |-----------------------------
 * |Tab |  Q|  W|  E|  R|  T|Fn3|
 * |-----------------------------
 * |Control|  A|  S|  D|  F|   G|
 * |-----------------------------
 * |Shift   |  Z|  X|  C|  V|  B|
 * |-----------------------------
 * |Fn1|Alt|GUI|MHEN|        Fn2|
 * |   |---|   |    |           |
 * |   |Ctl|   |    |           |
 * `----------------------------'
 *
 * Matrix: 6x6
 *    |       0 |       1 |       2 |       3 |       4 |       5 |X
 * ---+---------+---------+---------+---------+---------+---------+
 *  0 |         | Y       | Fn1     | 5       | 6       | Esc     |
 *  1 | B       |         | 4       | 3       | 2       | 1       |
 *  2 | V       | C       | X       | Z       | G       | LAlt    |
 *  3 | Fn2     |         |         | MHEN    | F       | Tab     |
 *  4 | D       | Ctrl2   | T       | S       | A       | Shift   |
 *  5 | Control | GUI     | R       | E       | W       | Q       |
 *  Y
 */
// b:101 -> 1|01 -> Y=1|X=1-1 -> K01
// v:201 -> 2|01 -> Y=2|X=1-1 -> K02
// d:401 -> 4|01 -> Y=4|X=1-1 -> K04
// mhnkn:308 -> 3|08 -> 3|4 -> Y=3|X=4-1 -> K33
#define KEYMAP( \
    K50, K51, K41, K31, K21, K30, K40, \
    K53, K55, K45, K35, K25, K24, K10, \
    K05, K44, K34, K04, K43, K42,      \
    K54, K32, K22, K12, K02, K01,      \
    K20, K52, K14, K15, K33, K03       \
) { \
    { KC_NO,    KC_##K10,  KC_##K20, KC_##K30, KC_##K40, KC_##K50  }, \
    { KC_##K01, KC_NO,     KC_##K21, KC_##K31, KC_##K41, KC_##K51  }, \
    { KC_##K02, KC_##K12,  KC_##K22, KC_##K32, KC_##K42, KC_##K52  }, \
    { KC_##K03, KC_NO,     KC_NO,    KC_##K33, KC_##K43, KC_##K53  }, \
    { KC_##K04, KC_##K14,  KC_##K24, KC_##K34, KC_##K44, KC_##K54  }, \
    { KC_##K05, KC_##K15,  KC_##K25, KC_##K35, KC_##K45, KC_##K55  }, \
}

static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer 0: Default Layer
     * ,----------------------------
     * |Esc |  1|  2|  3|  4|  5|  6|
     * |-----------------------------
     * |Tab |  Q|  W|  E|  R|  T|Fn3|
     * |-----------------------------
     * |Control|  A|  S|  D|  F|   G|
     * |-----------------------------
     * |Shift   |  Z|  X|  C|  V|  B|
     * |-----------------------------
     * |Fn1|Alt|GUI|MHEN|        Fn2|
     * |   |---|   |    |           |
     * |   |Ctl|   |    |           |
     * `----------------------------'
     */
    KEYMAP(ESC,   1,   2,   3,   4,   5,   6, \
           TAB,   Q,   W,   E,   R,   T, FN2, \
           LCTL,  A,   S,   D,   F,   G,      \
           LSFT,  Z,   X,   C,   V,   B,      \
           FN0,LALT,LCTRL,LGUI,MHEN,FN1),

    KEYMAP(ESC,   1,   2,   3,   4,   5,   6, \
           TAB,   Q,   W,   E,   R,   T, FN2, \
           LCTL,  A,   S,   D,   F,   G,      \
           LSFT,  Z,   X,   C,   V,   B,      \
           FN0,LALT,LCTRL,LGUI,MHEN,FN1),

    /* Layer 2: space layer
     * ,----------------------------
     * |   \|  ^|  -|  0|  9|  8|  7|
     * |-----------------------------
     * |BS  |  @|  P|  O|  I|  U|Ent|
     * |-----------------------------
     * |Control|  ;|  L|  K|  J|   H|
     * |-----------------------------
     * |Shift   |  /|  .|  ,|  M|  N|
     * |-----------------------------
     * |Fn0|Alt|GUI|MHEN|        Fn2|
     * |   |---|   |    |           |
     * |   |Ctl|   |    |           |
     * `----------------------------'
     */
    KEYMAP(JYEN, EQL,MINS,   0,   9,   8,   7, \
           BSPC,   P,   O,   I,   U,   Y,TRNS, \
           LCTL,SCLN,   L,   K,   J,   H,      \
           LSFT,SLSH, DOT,COMM,   M,   N,      \
           FN0,LALT,LCTRL,LGUI,HENK,TRNS),

    /* Layer 3: symbol layer
     */
    KEYMAP(JYEN, EQL,MINS,  NO,  NO,  NO,  NO, \
           BSPC,RBRC,LBRC,  NO,  NO,  NO, FN2, \
            ENT,NUHS,QUOT,SCLN,  NO,  NO,      \
           LSFT,  RO,SLSH, DOT,COMM,  NO,      \
           FN0,LALT,LCTRL,LGUI,HENK,TRNS),
};


const uint16_t fn_actions[] PROGMEM = {
    [0] = ACTION_DEFAULT_LAYER_SET(0),                // Default layer(not used)
    [1] = ACTION_LAYER_TAP_KEY(2, KC_SPC),         // space
    [2] = ACTION_LAYER_TOGGLE(3),
};

/* translates key to keycode */
uint8_t keymap_key_to_keycode(uint8_t layer, keypos_t key)
{
    return pgm_read_byte(&keymaps[(layer)][(key.row)][(key.col)]);
}

/* translates Fn keycode to action */
action_t keymap_fn_to_action(uint8_t keycode)
{
    return (action_t){ .code = pgm_read_word(&fn_actions[FN_INDEX(keycode)]) };
}

