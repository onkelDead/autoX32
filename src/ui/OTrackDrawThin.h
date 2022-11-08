/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OTrackDrawThin.h
 * Author: onkel
 *
 * Created on November 6, 2022, 4:06 PM
 */

#ifndef OTRACKDRAWTHIN_H
#define OTRACKDRAWTHIN_H

#include <gtkmm.h>
#include <gtkmm/widget.h>
#include "OCustomWidget.h"
#include "IOTrackStore.h"

class OTrackDrawThin : public OCustomWidget {
public:
    OTrackDrawThin(daw_time *daw_time);
    virtual ~OTrackDrawThin();
    void SetTrackStore(IOTrackStore*);

protected:
    //Overrides:
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr);
private:
    daw_time *m_daw_time;
    IOTrackStore *m_trackstore = nullptr;
    gint m_left = 0;
    gint m_right = 0;
    gint m_width = 0;
    void GetColorByIndex(const Cairo::RefPtr<Cairo::Context> &cr, int index);
	float GetHeight(lo_arg it, char t);

};

#endif /* OTRACKDRAWTHIN_H */

