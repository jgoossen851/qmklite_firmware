/*
Copyright 2013 Jun Wako <wakojun@gmail.com>

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
#ifndef LAYER_SWITCH_H
#define LAYER_SWITCH_H

#include <stdint.h>
#include "keyboard.h"
#include "action.h"


/* overlays are asigned at layer 16-31 */
#define OVERLAY_BIT      0x10
#define OVERLAY_MASK     0x0F


/*
 * Default Layer
 */
/* base layer to fall back */
extern uint8_t default_layer;
void default_layer_set(uint8_t layer);


/*
 * Keymap Layer
 */
extern uint16_t keymap_stat;
/* return current active layer */
uint8_t keymap_get_layer(void);
void keymap_clear(void);
void keymap_set(uint16_t stat);
void keymap_move(uint8_t layer);
void keymap_on(uint8_t layer);
void keymap_off(uint8_t layer);
void keymap_invert(uint8_t layer);
/* bitwise operation */
void keymap_or(uint16_t stat);
void keymap_and(uint16_t stat);
void keymap_xor(uint16_t stat);
void keymap_debug(void);


/*
 * Overlay Layer
 */
extern uint16_t overlay_stat;
/* return current active layer */
uint8_t overlay_get_layer(void);
void overlay_clear(void);
void overlay_set(uint16_t stat);
void overlay_move(uint8_t layer);
void overlay_on(uint8_t layer);
void overlay_off(uint8_t layer);
void overlay_invert(uint8_t layer);
/* bitwise operation */
void overlay_or(uint16_t stat);
void overlay_and(uint16_t stat);
void overlay_xor(uint16_t stat);
void overlay_debug(void);



/* return action depending on current layer status */
action_t layer_switch_get_action(key_t key);

#endif
