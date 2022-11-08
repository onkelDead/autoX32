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
    MMC_PLAY,
    MMC_STOP,
    MMC_RESET, 
    CTL_PLAY,
    CTL_STOP,
    CTL_TEACH_PRESS,
    CTL_TEACH_RELEASE,
    CTL_TEACH_MODE, 
    CTL_TOUCH_RELEASE,
    CTL_LOOP_START,
    CTL_LOOP_END, 
    CTL_HOME,
    CTL_END,
    CTL_FADER,
    CTL_PREV_TRACK,
    CTL_NEXT_TRACK,
    CTL_DROP_TRACK,
    CTL_KNOB,
    CTL_SCRUB_ON,
    CTL_SCRUB_OFF,
    CTL_STEP_MODE,
    CTL_WHEEL_LEFT,
    CTL_WHEEL_RIGHT,
    CTL_WHEEL_MODE,
    CTL_UNSELECT,
    CTL_TOGGLE_SOLO, 
    CTL_TOGGLE_REC,
    CTL_MARKER_PRESS,
    CTL_MARKER_RELEASE,
    CTL_LOOP,
    CTL_SHUTDOWN,
    CTL_SAVE,
    CTL_CENTER_THIN,
};

enum DAW_PATH {
    unknown,
    samples,
    session, 
    reply
};

enum E_OPERATION {
    nothing,
    new_track,
    draw_trackview,
    new_pos,
    play,
    stop,
    teach,
    select_track,
    unselect_track,
    toggle_rec, 
    toggle_recview,
    marker_start,
    marker_end,
    drop_track,
};

typedef struct ui_event_type {
    E_OPERATION event;
    void* context;
} operation_t;

typedef struct daw_time_type {
    int m_maxframes;
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
    size_t m_index;    
} track_layout;

typedef struct {
    uint8_t len;
    uint8_t buf[64];
} ctl_command;


#endif /* OTYPES_H */

