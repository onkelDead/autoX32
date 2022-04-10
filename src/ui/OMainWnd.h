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


#include "IOMainWnd.h"
#include "IOMessageHandler.h"
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

class OMainWnd : public Gtk::Window, IOMainWnd, IOMessageHandler {
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
    void on_menu_project_new();
    void on_menu_project_open();
    void on_menu_project_save();
    void on_menu_project_save_as();
    void on_menu_project_close();
    void on_menu_recent(std::string);

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
    void OnJackEvent();
    void notify_jack(JACK_EVENT);
    void OnDawEvent();
    void notify_daw(DAW_PATH);
    void OnMessageEvent();
    void OnMixerEvent();
    void OnOverViewEvent();
    void OnViewEvent();
    void GetTrackConfig(IOTrackStore* trackstore);

    void notify_overview();
    void remove_track(IOTrackView*);

    /// operations
    void ApplyWindowSettings();
    void AutoConnect();
    bool ConnectMixer(std::string);

    int NewMessageCallback(IOscMessage*);
    int UpdateMessageCallback(IOscMessage*);    
    
    bool ConnectDaw(std::string host, std::string port, std::string reply_port);

    bool SetupBackend();

    gint GetPosMillis();
    
    void UpdatePos(gint current, bool jump);

    void NewProject();    
    void NewProject(std::string);

    void OpenProject(std::string location);
    std::string GetProjectLocation();
    void SetProjectLocation(std::string);
    bool SaveProject();
    bool SelectProjectLocation(bool);
    bool Shutdown();
    void CloseProject();

    void UpdateDawTime(bool redraw);
    void UpdatePlayhead(bool doCalc);
    bool PlayTrackEntry(IOTrackStore* trackstore, track_entry* entry);
    
    void SelectTrack(std::string, bool);
    void EditTrack(std::string);
    void UnselectTrack();
    void ToggleSolo();
    void TrackViewUp(std::string path);    
    void TrackViewDown(std::string path);
    void TrackViewHide(std::string path);

    

    /// application settings
    OConfig* GetConfig();

    OTimer *m_timer = nullptr;

protected:
    Glib::RefPtr<Gtk::CssProvider> m_refCssProvider;

private:

    OConfig m_config;
    //Glib::RefPtr<Gio::Settings> settings;

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

    bool m_teach_mode = false;
    
    /// dialogs
    Gtk::AboutDialog m_Dialog;

    Glib::Dispatcher m_DawDispatcher;
    OQueue<DAW_PATH> my_dawqueue;
    Glib::Dispatcher m_JackDispatcher;
    OQueue<JACK_EVENT> m_jackqueue;
    
    OTimer m_jackTimer;

    Glib::Dispatcher m_MessageDispatcher;
    OQueue<IOscMessage*> my_messagequeue;

    Glib::Dispatcher m_ViewDispatcher;
    Glib::Dispatcher m_OverViewDispatcher;
    OQueue<ui_event*> m_new_ts_queue;

    /// objects
    OProject m_project;
    IOMixer* m_x32 = nullptr;
    ODAW m_daw;
    IOBackend* m_backend = nullptr;

    int m_last_playhead_update = 0;
    int m_last_pos_update = 0;

    void create_view();
    void create_menu();
    void UpdateMenuRecent();

    void create_about_dlg();

    void on_about_dialog_response(int response_id);
	void PublishUiEvent(UI_EVENTS, void *);
	void PublishUiEvent(ui_event *);
};



#endif /* SRC_OMAINWND_H_ */
