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

#ifndef IOMAINWND_H
#define IOMAINWND_H

#include <giomm/settingsschemasource.h>
#include <giomm/settings.h>


#include "OTypes.h"
#include "OscCmd.h"
#include "IOTrackView.h"

class IOMainWnd {
public:

    virtual ~IOMainWnd() {
    }
    virtual void notify_daw(DAW_PATH) = 0;
    virtual void notify_jack(JACK_EVENT) = 0;
    virtual void notify_mixer(OscCmd*) = 0;
    virtual void notify_overview() = 0;
    virtual void remove_track(IOTrackView*) = 0;
    virtual Gio::Settings* GetSettings() = 0;
    virtual void SelectTrack(std::string, bool) = 0;
    virtual gint GetPosMillis() = 0;
    virtual void TrackViewUp(std::string) = 0;
    virtual void TrackViewDown(std::string) = 0;

private:

};

#endif /* IOMAINWND_H */

