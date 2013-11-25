/*
Copyright 2010,2011,2012,2013 Jun WAKO <wakojun@gmail.com>

This software is licensed with a Modified BSD License.
All of this is supposed to be Free Software, Open Source, DFSG-free,
GPL-compatible, and OK to use in both free and proprietary applications.
Additions and corrections to this file are welcome.


Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

* Neither the name of the copyright holders nor the names of
  contributors may be used to endorse or promote products derived
  from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdbool.h>
#include <util/delay.h>
#include "ps2.h"
#include "debug.h"


/*
 * PS/2 protocol busywait version
 */

#define WAIT(stat, us, err) do { \
    if (!wait_##stat(us)) { \
        ps2_error = err; \
        goto ERROR; \
    } \
} while (0)

uint8_t ps2_error = PS2_ERR_NONE;


void ps2_host_init(void)
{
    // POR(150-2000ms) plus BAT(300-500ms) may take 2.5sec([3]p.20)
    _delay_ms(2500);

    inhibit();
}

uint8_t ps2_host_send(uint8_t data)
{
    uint8_t res = 0;
    bool parity = true;
    ps2_error = PS2_ERR_NONE;
    /* terminate a transmission if we have */
    inhibit();
    _delay_us(200); // at least 100us

    /* start bit [1] */
    data_lo();
    clock_hi();
    WAIT(clock_lo, 20000, 10);   // may take 15ms at most until device starts clocking
    /* data [2-9] */
    for (uint8_t i = 0; i < 8; i++) {
        _delay_us(15);
        if (data&(1<<i)) {
            parity = !parity;
            data_hi();
        } else {
            data_lo();
        }
        WAIT(clock_hi, 50, 2);
        WAIT(clock_lo, 50, 3);
    }
    /* parity [10] */
    _delay_us(15);
    if (parity) { data_hi(); } else { data_lo(); }
    WAIT(clock_hi, 50, 4);
    WAIT(clock_lo, 50, 5);
    /* stop bit [11] */
    _delay_us(15);
    data_hi();
    /* ack [12] */
    WAIT(data_lo, 50, 6);
    WAIT(clock_lo, 50, 7);

    /* wait for idle state */
    WAIT(clock_hi, 50, 8);
    WAIT(data_hi, 50, 9);

    res = ps2_host_recv_response();
ERROR:
    inhibit();
    return res;
}

/* receive data when host want else inhibit communication */
uint8_t ps2_host_recv_response(void)
{
    // TODO:
    // Command might take 20ms to response([3]p.21)
    // TrackPoint might take 25ms ([5]2.7)
    uint8_t data = 0;
    uint8_t try = 200;
    while (try-- && (data = ps2_host_recv())) ;
    return data;
}

/* send LED state to keyboard */
void ps2_host_set_led(uint8_t led)
{
    ps2_host_send(0xED);
    ps2_host_send(led);
}


/* called after start bit comes */
uint8_t ps2_host_recv(void)
{
    uint8_t data = 0;
    bool parity = true;
    ps2_error = PS2_ERR_NONE;

    /* release lines(idle state) */
    idle();

    /* start bit [1] */
    WAIT(clock_lo, 100, 1); // TODO: this is enough?
    WAIT(data_lo, 1, 2);
    WAIT(clock_hi, 50, 3);

    /* data [2-9] */
    for (uint8_t i = 0; i < 8; i++) {
        WAIT(clock_lo, 50, 4);
        if (data_in()) {
            parity = !parity;
            data |= (1<<i);
        }
        WAIT(clock_hi, 50, 5);
    }

    /* parity [10] */
    WAIT(clock_lo, 50, 6);
    if (data_in() != parity) {
        ps2_error = PS2_ERR_PARITY;
        goto ERROR;
    }
    WAIT(clock_hi, 50, 7);

    /* stop bit [11] */
    WAIT(clock_lo, 50, 8);
    WAIT(data_hi, 1, 9);
    WAIT(clock_hi, 50, 10);

    inhibit();
    return data;
ERROR:
    if (ps2_error > PS2_ERR_STARTBIT3) {
        printf("x%02X\n", ps2_error);
    }
    inhibit();
    return 0;
}
