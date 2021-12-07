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

#ifndef IMAGES_H
#define IMAGES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "embedded/go_start.h"
#include "embedded/lock_playhead_off.h"
#include "embedded/lock_playhead_on.h"
#include "embedded/loop_end.h"
#include "embedded/loop_start.h"
#include "embedded/play_off.h"
#include "embedded/play_on.h"
#include "embedded/rec_off.h"
#include "embedded/rec_on.h"
#include "embedded/teach_off.h"
#include "embedded/teach_on.h"
#include "embedded/zoom_loop.h"

    
#define SETTING_SMOOTH_SCREEN           "smooth-screen"    
#define SETTINGS_DAW_HOST               "daw-host"    
#define SETTINGS_DAW_PORT               "daw-port"
#define SETTINGS_DAW__REPLAY_PORT       "daw-reply-port"
#define SETTINGS_DAW_AUTOCONNECT        "daw-autoconnect"
#define SETTINGS_MIXER_HOST             "mixer-host"
#define SETTINGS_MIXER_AUTOCONNECT      "mixer-autoconnect"
    
#define SETTINGS_WINDOW_WIDTH           "window-width"    
#define SETTINGS_WINDOW_HEIGHT          "window-height"    
#define SETTINGS_WINDOW_TOP             "window-top"    
#define SETTINGS_WINDOW_LEFT            "window-left"    
    
#define SETTINGS_SHOW_PATH_ON_TRACK     "show-path-on-track"
    
#define SETTINGS_RECENT_PROJECTS        "recent-projects"    
    
#ifdef __cplusplus
}
#endif

#endif /* IMAGES_H */

