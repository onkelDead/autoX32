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

enum JACK_EVENT {
    MTC_COMPLETE,
    MTC_QUARTER_FRAME,
    MTC_QUARTER_FRAME_SEC,
    MTC_QUARTER_FRAME_SEC1,
    MMC_PLAY,
    MMC_STOP,
    MMC_LOCATE,
    MMC_RESET, 
    CTL_PLAY,
    CTL_STOP,
    CTL_TEACH_ON,
    CTL_TEACH_OFF,
    CTL_TEACH_MODE, 
    CTL_TOUCH_RELEASE,
    CTL_COMMAND,
    CTL_LOOP_SET,
    CTL_LOOP_CLEAR,
    CTL_TOGGLE_LOOP,
    CTL_HOME,
    CTL_FADER,
    CTL_PREV_TRACK,
    CTL_NEXT_TRACK,
    CTL_SCRUB_ON,
    CTL_SCRUB_OFF,
    CTL_JUMP_BACKWARD,
    CTL_JUMP_FORWARD,
    CTL_WHEEL_MODE,
};

enum DAW_PATH {
    unknown,
    samples,
    reply
};

enum UI_EVENTS {
    nothing,
    load,
    new_track,
    draw_trackview,
    new_pos,
    play,
    stop,
    touch_on,
    touch_off,
    touch_release,
    home,
    next_track,
    prev_track,
    jump_forward,
    jump_backward,
    conf_track,
};

typedef struct ui_event_type {
    UI_EVENTS what;
    void* with;
} ui_event;

typedef struct daw_time_type {
    int m_maxmillis;
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
    int time;
    lo_arg val;
    track_entry* next;
} track_entry;

typedef struct track_layout {
    bool m_expanded;
    int m_height;
    bool m_visible;
    int m_index;    
} track_layout;



#endif /* OTYPES_H */

