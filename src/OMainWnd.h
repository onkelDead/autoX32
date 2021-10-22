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
#include "OResource.h"

#include "OX32.h"
#include "ODAW.h"
#include "OJack.h"
#include "OProject.h"
#include "OTimeView.h"
#include "OTracksLayout.h"
#include "OOverView.h"
#include "OPlayHead.h"
#include "OQueue.h"

#define PACKAGE_STRING "autoX32"
#define PACKAGE_VERSION "0.1"
#define PACKAGE_BUGREPORT "onkel@paraair.de"

#define AUTOX32_SCHEMA_ID "de.paraair.autoX32"

class OMainWnd : public Gtk::Window, IOMainWnd {
public:
    OMainWnd();
    virtual ~OMainWnd();

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

    /// button/UI events
    void on_button_play_clicked();
    void on_button_back_clicked();
    void on_button_test_clicked();
    void on_btn_teach_clicked();
    void on_btn_loop_start_clicked();
    void on_btn_loop_end_clicked();
    void on_btn_zoom_loop_clicked();
    void on_btn_lock_playhead_clicked();
    void on_btn_cut_clicked();

    void on_timeline_zoom_changed();
    void on_timeline_pos_changed();

    /// thread events
    void OnJackEvent();
    void notify_jack(JACK_EVENT);
    void OnDawEvent();
    void notify_daw(DAW_PATH);
    void OnMixerEvent();
    void notify_mixer(OscCmd*);
    void OnOverViewEvent();
    void OnViewEvent();
    void notify_overview();
    void remove_track(IOTrackView*);

    /// operations
    void ApplyWindowSettings();
    void AutoConnect();
    bool ConnectMixer(std::string);

    bool ConnectDaw(std::string host, std::string port, std::string reply_port);

    bool SetupJackClient();

    gint GetPosMillis();

    void NewProject();
    void OpenProject(std::string location);
    bool SaveProject();
    bool SelectProjectLocation(bool);
    bool Shutdown();

    void UpdateDawTime(bool redraw);
    void UpdatePlayhead();

    void SelectTrack(std::string, bool);

    /// application settings
    Gio::Settings* GetSettings();


protected:
    Glib::RefPtr<Gtk::CssProvider> m_refCssProvider;

private:

    Glib::RefPtr<Gio::Settings> settings;

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
    Glib::RefPtr<Gtk::MenuItem> m_prefs;
    Glib::RefPtr<Gtk::Menu> m_recents;
    Glib::RefPtr<Gtk::MenuItem> m_about;

    Gtk::Box *m_Toolbox;
    Gtk::ToggleToolButton* m_button_play;
    Gtk::Image m_img_play_on;
    Gtk::Image m_img_play_off;
    Gtk::ToolButton* m_button_back;
    Gtk::Image m_img_back;
    Gtk::ToggleToolButton *m_btn_teach;
    Gtk::Image m_img_teach_on;
    Gtk::Image m_img_teach_off;
    Gtk::ToolButton *m_btn_loop_start;
    Gtk::Image m_img_loop_start;
    Gtk::ToolButton *m_btn_loop_end;
    Gtk::Image m_img_loop_end;
    Gtk::ToolButton *m_btn_zoom_loop;
    Gtk::Image m_img_zoom_loop;
    Gtk::ToggleToolButton *m_btn_lock_playhead;
    Gtk::Image m_img_lock_playhead_on;
    Gtk::Image m_img_lock_playhead_off;
    Gtk::ToggleToolButton *m_btn_cut;
    Gtk::Image m_img_cut_on;
    Gtk::Image m_img_cut_off;

    Gtk::Box *m_timebox;
    OTimeView *m_timeview = nullptr;

    Gtk::Box *m_tracksbox;
    Gtk::ScrolledWindow *m_scroll;
    Gtk::Viewport *m_viewport;
    Gtk::Box *m_scrolledview;
    OTracksLayout m_trackslayout;
    OOverView* m_overview;

    Gtk::Box *m_statusbox;
    Gtk::Label *m_lbl_status;
    Gtk::Label *m_lbl_ardour;
    Gtk::Label *m_lbl_x32;

    Gtk::Overlay *m_overlay;
    OPlayHead *m_playhead;

    bool m_lock_play;
    bool m_lock_daw_time;

    /// prevent loop back daw-event when position changed from here
    bool m_lock_daw_time_event;

    /// dialogs
    Gtk::AboutDialog m_Dialog;

    Glib::Dispatcher m_DawDispatcher;
    std::queue<DAW_PATH> my_dawqueue;
    Glib::Dispatcher m_JackDispatcher;
    std::queue<JACK_EVENT> m_jackqueue;

    Glib::Dispatcher m_MixerDispatcher;
    std::queue<OscCmd*> my_mixerqueue;
    Glib::Dispatcher m_ViewDispatcher;
    Glib::Dispatcher m_OverViewDispatcher;
    OQueue m_new_ts_queue;

    /// objects
    OProject m_project;
    OX32* m_x32 = nullptr;
    ODAW m_daw;
    OJack m_jack;

    void TimerEvent(void*);
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
