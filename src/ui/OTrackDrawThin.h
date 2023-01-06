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
    OTrackDrawThin(IOThinWnd* wnd, daw_time *daw_time);
    virtual ~OTrackDrawThin();
    void SetTrackStore(IOTrackStore*);
    void SetSignalPosChange(IOTimeView *t);
    int m_click_frame = 0;

protected:
    //Overrides:
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr);
    virtual bool on_button_press_event(GdkEventButton* event);
    
private:
    IOThinWnd* m_parent;
    daw_time *m_daw_time;
    sigc::signal<void> signal_pos_changed;

    IOTrackStore *m_trackstore = nullptr;
    gint m_left = 0;
    gint m_right = 0;
    gint m_width = 0;
    void GetColorByIndex(const Cairo::RefPtr<Cairo::Context> &cr, int index);

};

#endif /* OTRACKDRAWTHIN_H */

