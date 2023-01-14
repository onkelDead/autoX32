/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OTrackDrawBase.h
 * Author: onkel
 *
 * Created on January 6, 2023, 8:59 AM
 */

#ifndef OTRACKDRAWBASE_H
#define OTRACKDRAWBASE_H

#include <gtkmm.h>
#include <gtkmm/widget.h>
#include <lo/lo.h>

#include "OCustomWidget.h"
#include "IOTrackStore.h"

class OTrackDrawBase : public OCustomWidget {
public:
    OTrackDrawBase();
    OTrackDrawBase(const OTrackDrawBase& orig);
    virtual ~OTrackDrawBase();

    void SetTrackStore(IOTrackStore*);
    IOTrackStore* GetTrackStore();

    IOscMessage* GetMessage();

protected:
    virtual void draw_text(const Cairo::RefPtr<Cairo::Context> &cr, int rectangle_width, int rectangle_height, std::string text);

    float GetHeight(lo_arg it, char t);
    void GetColorByIndex(const Cairo::RefPtr<Cairo::Context> &cr, int index);

    IOTrackStore *m_trackstore = nullptr;

private:
};

#endif /* OTRACKDRAWBASE_H */

