/*
Copyright 2011 Jun Wako <wakojun@gmail.com>
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

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "debug_USART.h"
#include <stdio.h>

//#define DO_DEBUG_USART

#define BV(bit)               (1 << bit)
#define set_bit(sfr, bit)     (_SFR_BYTE(sfr) |= BV(bit))  // old sbi()
#define clear_bit(sfr, bit)   (_SFR_BYTE(sfr) &= ~BV(bit)) // old cbi()
#define toggle_bit(sfr, bit)  (_SFR_BYTE(sfr) ^= BV(bit))

#define set_val(sfr, val)     (_SFR_BYTE(sfr) |= val)
#define clear_val(sfr, val)   (_SFR_BYTE(sfr) &= ~val)

/*
 * test
 * PORTD: ポートDデータレジスタ(pullup)
 * DDRD: ポートD方向レジスタ(1:output, 0:input)
 * PIND: ポートD入力レジスタ(読み取り専用)
 *
 * http://www.cqpub.co.jp/hanbai/books/37/37301/37301_4-1.pdf
 *             MCUCR
 * DDxn PORTxn PUD I/O    pullup status
 * -------------------------------------
 * 0    0      X   input  no     hi-z
 * 0    1      0   input  yes    外部が L に引き込まれているとPxnはソース電源となる
 * 0    1      1   input  no     hi-z
 * -------------------------------------
 * 1    0      X   output no     L シンク
 * 1    1      X   output no     H ソース
 * -------------------------------------
 *
 * http://playground.arduino.cc/Code/Keypad#Download
 * http://playground.arduino.cc/uploads/Code/keypad.zip
 */

/*
 * Keymap for bskbb14 mod
 *
 * Pin usage:
 *   ROW: PD0-1,PD4-7
 *   COL: PB0-5
 */
#ifndef DEBOUNCE
#   define DEBOUNCE	10
#endif
static uint8_t debouncing = DEBOUNCE;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

#ifdef MATRIX_HAS_GHOST
static bool matrix_has_ghost_in_row(uint8_t row);
#endif
static matrix_row_t read_cols(void);
static void init_cols(void);
static void unselect_rows(void);
static void select_row(uint8_t row);


inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

/*
 *   ROW: PD0-1,PD4-7
 *   COL: PB0-5
 */
void matrix_init(void)
{
#ifdef DO_DEBUG_USART
    initUSART();
#endif
    // initialize rows
    unselect_rows();

    init_cols();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
        matrix_debouncing[i] = 0;
    }
}

#ifdef DO_DEBUG_USART
#include <string.h>
char lastbuf[80];
#endif

uint8_t matrix_scan(void)
{
#ifdef DO_DEBUG_USART
    char buf[80];
#endif
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        select_row(i);
        _delay_us(30);  // without this wait read unstable value.
        matrix_row_t cols = read_cols();

#ifdef DO_DEBUG_USART
        if (cols) {
            sprintf(buf, "%d%02x ", i, cols);
            if (strcmp(buf, lastbuf) != 0) {
                printString(buf);
            }
            strcpy(lastbuf, buf);
        }
#endif
        if (matrix_debouncing[i] != cols) {
            matrix_debouncing[i] = cols;
            if (debouncing) {
                debug("bounce!: "); debug_hex(debouncing); debug("\n");
            }
            debouncing = DEBOUNCE;
        }
        unselect_rows();
    }

    if (debouncing) {
        if (--debouncing) {
            _delay_ms(1);
        } else {
            for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
                matrix[i] = matrix_debouncing[i];
            }
        }
    }

    return 1;
}

bool matrix_is_modified(void)
{
    if (debouncing) return false;
    return true;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 01234567\n");
    for (uint8_t row = 0; row < matrix_rows(); row++) {
        phex(row); print(": ");
        pbin_reverse(matrix_get_row(row));
#ifdef MATRIX_HAS_GHOST
        if (matrix_has_ghost_in_row(row)) {
            print(" <ghost");
        }
#endif
        print("\n");
    }
}

#ifdef MATRIX_HAS_GHOST
inline
static bool matrix_has_ghost_in_row(uint8_t row)
{
    // no ghost exists in case less than 2 keys on
    if (((matrix[row] - 1) & matrix[row]) == 0)
        return false;

    // ghost exists in case same state as other row
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        if (i != row && (matrix[i] & matrix[row]))
            return true;
    }
    return false;
}
#endif

/*
 *   ROW: PD0-1,PD4-7 (PD2 RXD, PD3 TXD)
 *   COL: PB0-5
 */
#define ROW_MASK 0b11110011
#define COL_MASK 0b00111111

#define ROW_PIN PIND
#define ROW_DDR DDRD
#define ROW_PORT PORTD

#define COL_PIN PINB
#define COL_DDR DDRB
#define COL_PORT PORTB

inline
static matrix_row_t read_cols(void)
{
    return ((~COL_PIN) & COL_MASK);
}

inline
static void init_cols(void)
{
    // initialize columns to input with pull-up(DDR:0, PORT:1)
    set_val(COL_DDR, COL_MASK);
    set_val(COL_PORT, COL_MASK);
    clear_val(COL_DDR, COL_MASK);
}

inline
static void unselect_rows(void)
{
    // Hi-Z(DDR:0, PORT:0) to unselect
    set_val(ROW_DDR, ROW_MASK);
    clear_val(ROW_PORT, ROW_MASK);
    clear_val(ROW_DDR, ROW_MASK);
}

unsigned char row_bitmask[] = {
    0b00000001,
    0b00000010,
    0b00010000,
    0b00100000,
    0b01000000,
    0b10000000,
};

inline
static void select_row(uint8_t row)
{
    // Output low(DDR:1, PORT:0) to select
    if (row >= 0 && row < 6) {
        set_val(ROW_DDR, row_bitmask[row]);
        clear_val(ROW_PORT, row_bitmask[row]);
    }
}
