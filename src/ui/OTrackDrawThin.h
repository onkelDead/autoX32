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
#include "OTrackDrawBase.h"
#include "IOTrackStore.h"
#include "IOTimeView.h"
#include "IOThinWnd.h"

class OTrackDrawThin : public OTrackDrawBase {
public:
    OTrackDrawThin(IOThinWnd* wnd, daw_time*);
    virtual ~OTrackDrawThin();
    void SetSignalPosChange(IOTimeView *t);
    int m_click_frame = 0;

protected:
    //Overrides:
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr);
    virtual bool on_button_press_event(GdkEventButton* event);
    
private:
    IOThinWnd* m_parent;
    sigc::signal<void> signal_pos_changed;
    daw_time *m_daw_time;

    gint m_left = 0;
    gint m_right = 0;
    gint m_width = 0;

};

#endif /* OTRACKDRAWTHIN_H */

