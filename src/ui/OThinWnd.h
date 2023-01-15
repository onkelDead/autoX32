/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OThinWnd.h
 * Author: onkel
 *
 * Created on November 6, 2022, 10:30 AM
 */

#ifndef OTHINWND_H
#define OTHINWND_H

#include <giomm/settingsschemasource.h>

#include "IOThinWnd.h"
#include "OEngine.h"
#include "OOverView.h"
#include "OPlayHead.h"
#include "OTrackDrawThin.h"

#include "res/OResource.h"

#define PACKAGE_STRING "autoX32"
#define PACKAGE_VERSION "0.7"
#define PACKAGE_BUGREPORT "onkel@paraair.de"

class OThinWnd : public Gtk::Window, public OEngine, IOTimeView, IOThinWnd {
public:
    OThinWnd();
    virtual ~OThinWnd();

    OConfig* GetConfig();
    void ApplyWindowSettings();

    void on_activate();
    bool on_delete_event(GdkEventAny *any_event) override;
    bool Shutdown();

    virtual void on_timedraw_pos_changed();   
    virtual void on_timedraw_zoom_changed() {}
    
    void SelectTrackDraw(std::string path);
    void PublishUiEvent(E_OPERATION, void *);
    void UpdatePlayhead(bool doCalc);

    virtual void OnProjectLoad();
    
    virtual void OnLocate(bool);
    virtual void OnUnselectTrack();
    virtual void OnSelectTrack();
    virtual void OnCenterThin();
protected:
    Glib::RefPtr<Gtk::CssProvider> m_refCssProvider;

private:
    Glib::RefPtr<Gtk::Builder> ui;

    Gtk::Box *m_mainbox;
    Gtk::Overlay *m_overlay;
    OPlayHead *m_playhead;
    Gtk::ScrolledWindow *m_scroll;
    Gtk::Box *m_scrolledview;
    Gtk::Box *m_bbox;

    OTrackDrawThin *thin;

    OQueue<operation_t*> m_queue_operation;
    Glib::Dispatcher m_ViewDispatcher;

    void PublishUiEvent(operation_t *);
    void OnUIOperation();

};


#endif /* OTHINWND_H */

