/*
  Copyright 2020 Detlef Urban <onkel@paraair.de>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef OTYPES_H
#define OTYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lo/lo.h"    
    
#ifdef __cplusplus
}
#endif

enum DAW_PATH {
	unknown,
	smpte,
	samples,
	reply,
	play,
	stop
};

enum UI_EVENTS {
	nothing,
    load,
    new_track,
	draw_trackview
};


typedef struct ui_event_type {
    UI_EVENTS what;
    void* with;
} ui_event;

typedef struct daw_time_type {
    int m_maxsamples;
    int m_bitrate;
    float scale;
    int m_viewstart;
    int m_viewend;
} daw_time;

typedef struct daw_range {
    int m_loopstart;
    int m_loopend;
    bool m_dirty;
} daw_range;

typedef struct track_entry {
    track_entry* prev;
    int sample;
    lo_arg val;
    track_entry* next;
} track_entry;


#endif /* OTYPES_H */

