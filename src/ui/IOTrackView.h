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

#ifndef IOTRACKVIEW_H
#define IOTRACKVIEW_H

#include "OscCmd.h"
#include "IOTrackStore.h"

class IOTrackView {
public:
    virtual OscCmd* GetCmd() = 0;
    virtual IOTrackStore* GetTrackStore() = 0;
    virtual void Resize(bool) = 0;
    virtual void ExpandCollapse(bool) = 0;
    virtual void Reset() = 0;
    virtual void SetHeight(gint) = 0;
};



#endif /* IOTRACKVIEW_H */
