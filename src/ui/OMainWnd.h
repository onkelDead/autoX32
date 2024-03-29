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

#ifndef SRC_OMAINWND_H_
#define SRC_OMAINWND_H_

#include <queue>
#include <giomm/settingsschemasource.h>
#include <gtkmm/widget.h>

#include "OEngine.h"

#include "IOMainWnd.h"
#include "IOMessageHandler.h"
#include "IODawHandler.h"
#include "IOJackHandler.h"
#include "res/OResource.h"

#include "OConfig.h"
#include "ODlgProlog.h"
#include "OTrackDlg.h"
#include "OX32.h"
#include "ODAW.h"
#include "OJack.h"
#include "OAlsa.h"
#include "OProject.h"
#include "OTimeView.h"
#include "OTracksLayout.h"
#include "OOverView.h"
#include "OPlayHead.h"
#include "OQueue.h"
#include "OTimer.h"

#define PACKAGE_STRING "autoX32"
#define PACKAGE_VERSION "0.7"
#define PACKAGE_BUGREPORT "onkel@paraair.de"

class OMainWnd : public Gtk::Window, public OEngine, IOMainWnd {
public:
    OMainWnd();
    virtual ~OMainWnd();

    void on_activate();
    
    /// overrides
    bool on_key_press_event(GdkEventKey *key_event) override;
    bool on_key_release_event(GdkEventKey *key_event) override;
    bool on_delete_event(GdkEventAny *any_event) override;

    /// menu events
    void on_menu_file_connection();
    void on_menu_file_exit();
    void on_menu_file_about();

    void on_menu_prefs();
    void on_menu_layout();
    void on_menu_expand_all();
    void on_menu_collapse_all();
    void on_menu_reset_all();
    void on_menu_fit();
    

    /// button/UI events
    void on_button_play_clicked();
    void on_button_home_clicked();
    void on_button_end_clicked();
    void on_button_test_clicked();
    void on_btn_teach_clicked();
    void on_btn_loop_start_clicked();
    void on_btn_loop_end_clicked();
    void on_btn_zoom_loop_clicked();

    void on_timeline_zoom_changed();
    void on_timeline_pos_changed();

    /// thread events
    void OnOverViewEvent();
    void OnUIOperation();
    void GetTrackConfig(IOTrackStore* trackstore);
    
    void notify_overview();
    void remove_track(std::string path);

    virtual void OnDawReply();
    virtual void OnProjectLoad();
    virtual void OnProjectClose();
    virtual void OnTrackUpdate(IOTrackStore*);
    virtual void OnTrackNew(IOTrackStore*);
    virtual void OnLocate(bool);
    virtual void OnPlay();
    virtual void OnStop();
    virtual void OnTeach(bool);
    virtual void OnUnselectTrack();
    virtual void OnSelectTrack();
    virtual void OnTrackRec();
    virtual void OnMarkerStart();
    virtual void OnMarkerEnd();
    virtual void OnDropTrack();
    virtual void OnCenterThin();
    
    /// operations
    void ApplyWindowSettings();
    bool ConnectMixer(std::string);
    
    bool ConnectDaw(std::string host, std::string port, std::string reply_port);

    gint GetPosFrame();
    
    void NewProject();    
    void NewProject(std::string);

    std::string GetProjectLocation();
    bool SaveProject();
    bool SelectProjectLocation(bool);
    bool Shutdown();
    void CloseProject();

    void UpdateDawTime(bool redraw);
    void UpdatePlayhead(bool doCalc);
    bool PlayTrackEntry(IOTrackStore* trackstore, track_entry* entry);
    
    void SelectTrackUI();
    void SelectTrackDraw(std::string path);
    
    void EditTrack(std::string);
    void TrackViewUp(std::string path);    
    void TrackViewDown(std::string path);
    void TrackViewHide(std::string path);

    void PublishUiEvent(E_OPERATION, void *);


    /// application settings
    OConfig* GetConfig();
    
    bool GetSensitive() { return m_sensitive; }

protected:
    Glib::RefPtr<Gtk::CssProvider> m_refCssProvider;

private:

    Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;

    Glib::RefPtr<Gtk::Builder> ui;

    // private menu members
    Gtk::Box *m_mainbox;
    Gtk::MenuBar* m_menubar;
    Glib::RefPtr<Gtk::MenuItem> m_connection;
    Glib::RefPtr<Gtk::MenuItem> m_quit_m;
    Glib::RefPtr<Gtk::MenuItem> m_project_new;
    Glib::RefPtr<Gtk::MenuItem> m_project_open;
    Glib::RefPtr<Gtk::MenuItem> m_project_save;
    Glib::RefPtr<Gtk::MenuItem> m_project_save_as;
    Glib::RefPtr<Gtk::MenuItem> m_project_close;
    Glib::RefPtr<Gtk::MenuItem> m_layout;
    Glib::RefPtr<Gtk::MenuItem> m_expand_all;
    Glib::RefPtr<Gtk::MenuItem> m_collapse_all;
    Glib::RefPtr<Gtk::MenuItem> m_reset_all;
    Glib::RefPtr<Gtk::MenuItem> m_view_fit;
    Glib::RefPtr<Gtk::MenuItem> m_prefs;
    Glib::RefPtr<Gtk::Menu> m_recents;
    Glib::RefPtr<Gtk::MenuItem> m_about;

    // private tool bar members
    Gtk::Box *m_Toolbox;
    Gtk::ToggleToolButton* m_button_play;
    Gtk::Image m_img_play_on;
    Gtk::Image m_img_play_off;
    Gtk::ToolButton* m_button_back;
    Gtk::ToolButton* m_button_end;
    Gtk::Image m_img_back;
    Gtk::Image m_img_end;
    Gtk::ToggleToolButton *m_btn_teach;
    Gtk::Image m_img_teach_on;
    Gtk::Image m_img_teach_off;
    Gtk::ToolButton *m_btn_loop_start;
    Gtk::Image m_img_loop_start;
    Gtk::ToolButton *m_btn_loop_end;
    Gtk::Image m_img_loop_end;
    Gtk::ToolButton *m_btn_zoom_loop;
    Gtk::Image m_img_zoom_loop;

    Gtk::MenuBar *m_menu_box;
    Gtk::Toolbar *m_tool_box;
    Gtk::Box *m_timebox;
    OTimeView *m_timeview = nullptr;
    Gtk::Box *m_status_box;

    Gtk::Box *m_tracksbox;
    Gtk::ScrolledWindow *m_scroll;
    Gtk::Viewport *m_viewport;
    Gtk::Box *m_scrolledview;
    OTracksLayout m_trackslayout;
    OOverView* m_overview;

    Gtk::Box *m_bbox;
    Gtk::Box *m_statusbox;
    Gtk::Label *m_lbl_status;
    Gtk::Label *m_lbl_ardour;
    Gtk::Label *m_lbl_x32;

    Gtk::Overlay *m_overlay;
    OPlayHead *m_playhead;

    bool m_lock_play;
    bool m_lock_daw_time;
    uint8_t m_shot_refresh = false;

    /// prevent loop back daw-event when position changed from here
    bool m_lock_daw_time_event;

    /// dialogs
    Gtk::AboutDialog m_Dialog;

    Glib::Dispatcher m_ViewDispatcher;
    Glib::Dispatcher m_OverViewDispatcher;
    OQueue<operation_t*> m_queue_operation;

    int m_last_playhead_update = 0;
    int m_last_pos_update = 0;

    void create_view();
    void create_menu();

    void create_about_dlg();

    void on_about_dialog_response(int response_id);
	void PublishUiEvent(operation_t *);
        
    bool m_sensitive = true;
};



#endif /* SRC_OMAINWND_H_ */
