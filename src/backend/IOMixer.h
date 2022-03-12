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

#ifndef IOX32_H
#define IOX32_H

typedef void(*MixerCallback)(OscCmd*, void*);

class IOMixer {
public:
    
    virtual int Connect(std::string) = 0;
    virtual int Disconnect() = 0;

    virtual void SetMixerCallback(MixerCallback, void*) = 0;
    
    virtual int IsConnected() = 0;
    
    virtual void SendFloat(std::string, float) = 0;
    virtual void SendInt(std::string, int ) = 0;
    virtual void Send(std::string) = 0;
};

#endif /* IOX32_H */
