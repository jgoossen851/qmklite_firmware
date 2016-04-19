#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "musical_notes.h"
#include "song_list.h"

#ifndef AUDIO_H
#define AUDIO_H

typedef union {
    uint8_t raw;
    struct {
        bool    enable :1;
        uint8_t level  :7;
    };
} audio_config_t;

void audio_toggle(void);
void audio_on(void);
void audio_off(void);

void play_sample(uint8_t * s, uint16_t l, bool r);
void play_note(double freq, int vol);
void stop_note(double freq);
void stop_all_notes(void);
void init_notes(void);
void play_notes(float (*np)[][2], uint8_t n_count, bool n_repeat, float n_rest);

void set_timbre(float timbre);
void set_tempo(float tempo);
void increase_tempo(uint8_t tempo_change);
void decrease_tempo(uint8_t tempo_change);

#define SCALE (int []){ 0 + (12*0), 2 + (12*0), 4 + (12*0), 5 + (12*0), 7 + (12*0), 9 + (12*0), 11 + (12*0), \
						0 + (12*1), 2 + (12*1), 4 + (12*1), 5 + (12*1), 7 + (12*1), 9 + (12*1), 11 + (12*1), \
						0 + (12*2), 2 + (12*2), 4 + (12*2), 5 + (12*2), 7 + (12*2), 9 + (12*2), 11 + (12*2), \
						0 + (12*3), 2 + (12*3), 4 + (12*3), 5 + (12*3), 7 + (12*3), 9 + (12*3), 11 + (12*3), \
						0 + (12*4), 2 + (12*4), 4 + (12*4), 5 + (12*4), 7 + (12*4), 9 + (12*4), 11 + (12*4), }

// These macros are used to allow play_notes to play an array of indeterminate
// length. This works around the limitation of C's sizeof operation on pointers.
// The global float array for the song must be used here.
#define NOTE_ARRAY_SIZE(x) ((int)(sizeof(x) / (sizeof(x[0]))))
#define PLAY_NOTE_ARRAY(note_array, note_repeat, note_rest_style) play_notes(&note_array, NOTE_ARRAY_SIZE((note_array)), (note_repeat), (note_rest_style));

void play_goodbye_tone(void);
void play_startup_tone(void);

#define VIBRATO_LUT (float []) { \
1.00090714186239, \
1.00181152169061, \
1.00270955652027, \
1.00359767896099, \
1.00447235162891, \
1.00533008160601, \
1.00616743486158, \
1.00698105056935, \
1.00776765525194, \
1.00852407668313, \
1.0092472574777, \
1.00993426829815, \
1.01058232060837, \
1.01118877890462, \
1.01175117235612, \
1.01226720578933, \
1.01273476995269, \
1.01315195100182, \
1.0135170391489, \
1.01382853642434, \
1.01408516350345, \
1.01428586555648, \
1.0144298170856, \
1.0145164257189, \
1.01454533493752, \
1.0145164257189, \
1.0144298170856, \
1.01428586555648, \
1.01408516350345, \
1.01382853642434, \
1.0135170391489, \
1.01315195100182, \
1.01273476995269, \
1.01226720578933, \
1.01175117235612, \
1.01118877890462, \
1.01058232060837, \
1.00993426829815, \
1.0092472574777, \
1.00852407668313, \
1.00776765525194, \
1.00698105056935, \
1.00616743486158, \
1.00533008160601, \
1.00447235162891, \
1.00359767896099, \
1.00270955652027, \
1.00181152169061, \
1.00090714186239, \
1, \
0.999093680298157, \
0.998191753986265, \
0.997297765337276, \
0.996415217934032, \
0.995547561242821, \
0.99469817754036, \
0.993870369236874, \
0.993067346634376, \
0.992292216155724, \
0.991547969076588, \
0.990837470789065, \
0.990163450622494, \
0.989528492243954, \
0.988935024658062, \
0.988385313823004, \
0.98788145489731, \
0.987425365129624, \
0.987018777401739, \
0.986663234433381, \
0.986360083655655, \
0.986110472758728, \
0.985915345918143, \
0.985775440703176, \
0.985691285669809, \
0.985663198640188, \
0.985691285669809, \
0.985775440703176, \
0.985915345918143, \
0.986110472758728, \
0.986360083655655, \
0.986663234433381, \
0.987018777401739, \
0.987425365129624, \
0.98788145489731, \
0.988385313823004, \
0.988935024658062, \
0.989528492243954, \
0.990163450622494, \
0.990837470789065, \
0.991547969076588, \
0.992292216155724, \
0.993067346634376, \
0.993870369236874, \
0.99469817754036, \
0.99554756124282, \
0.996415217934032, \
0.997297765337276, \
0.998191753986265, \
0.999093680298157, \
1, \
}
#define VIBRATO_LUT_LENGTH NOTE_ARRAY_SIZE(VIBRATO_LUT)

#endif