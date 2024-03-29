/*
 * Copyright (C) 2011, Triple Oxygen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "libgame.h"
#include "_8k8bpcm.h"

void wait_keypress(void);

typedef struct {
    char    *audio_data;
    int     audio_size;
} audio_data_desc;

res_entry_t resources[] = {
    {
        .filename = "8k8bpcm.wav",
        .res_data = _8k8bpcm_data
    },
    {
        .filename = "TAEND",
        .res_data = NULL
    }
};

void wait_keypress(void)
{
    key_data_t keys, no_keys;

    get_keys(&no_keys);

    while (1) {
        get_keys(&keys);
        if (keys.key2 != no_keys.key2)
            break;
    }
}

int main(void)
{
    int type;
    audio_data_desc ri;

    libgame_init();

    _dprintf("\n");
    _dprintf("Starting audio test.\n");

    res_init(0, &resources);

    type = res_get("8k8bpcm.wav", &ri);
    res_play(type, 1, &ri);

    _dprintf("Playing a mono 8 bit / 8000 Hz PCM wave file.\n");
    _dprintf("Press any key to next sample.\n");
    wait_keypress();

    res_stop(type);

    return 0;
}
