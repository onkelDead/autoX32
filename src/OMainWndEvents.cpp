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

#include <iostream>
#include <gtkmm.h>
#include "OMainWnd.h"
#include "OConnectDlg.h"
#include "OTrackDlg.h"

#include "embedded/connectDlg.h"
#include "embedded/trackdlg.h"
#include "embedded/main.h"
#include "ODlgPrefs.h"

void OMainWnd::on_menu_file_connection() {
    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_string(connectDlg_inline_glade);
    OConnectDlg *pDialog = nullptr;
    builder->get_widget_derived("connect-dlg", pDialog);

    // fill dialog with data
    pDialog->SetArdourHost(m_config.get_string(SETTINGS_DAW_HOST));
    pDialog->SetArdourPort(m_config.get_string(SETTINGS_DAW_PORT));
    pDialog->SetArdourReplyPort(m_config.get_string(SETTINGS_DAW__REPLAY_PORT));
    pDialog->SetArdourAutoConnect(m_config.get_boolean(SETTINGS_DAW_AUTOCONNECT));
    pDialog->SetX32Host(m_config.get_string(SETTINGS_MIXER_HOST));
    pDialog->SetX32AutoConnect(m_config.get_boolean(SETTINGS_MIXER_AUTOCONNECT));
    pDialog->run();
    if (pDialog->GetResult()) {

        if (ConnectDaw(pDialog->GetArdourHost(), pDialog->GetArdourPort(), pDialog->GetArdourReplyPort())) {
            m_config.set_string(SETTINGS_DAW_HOST, pDialog->GetArdourHost().c_str());
            m_config.set_string(SETTINGS_DAW_PORT, pDialog->GetArdourPort().c_str());
            m_config.set_string(SETTINGS_DAW__REPLAY_PORT, pDialog->GetArdourReplyPort().c_str());
            m_config.set_boolean(SETTINGS_DAW_AUTOCONNECT, pDialog->GetArdoutAutoConnect());
        } else {
            Gtk::MessageDialog dialog(*this, "Failed to connect to Ardour.",
                    false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
            dialog.run();
            return;
        }

        if (ConnectMixer(pDialog->GetX32Host())) {
            m_config.set_string(SETTINGS_MIXER_HOST, pDialog->GetX32Host().c_str());
            m_config.set_boolean(SETTINGS_MIXER_AUTOCONNECT, pDialog->GetX32AutoConnect());
        } else {
            Gtk::MessageDialog dialog(*this, "Failed to connect to Behringer X32.",
                    false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
            dialog.run();
        }
    }
}

void OMainWnd::on_menu_file_exit() {

    if (!Shutdown())
        this->hide();
}

void OMainWnd::on_menu_file_about() {

    m_Dialog.show();
}

void OMainWnd::on_about_dialog_response(int response_id) {

    switch (response_id) {
        case Gtk::RESPONSE_CLOSE:
        case Gtk::RESPONSE_CANCEL:
        case Gtk::RESPONSE_DELETE_EVENT:
            m_Dialog.hide();

            break;
        default:
            break;
    }
}

bool OMainWnd::on_key_press_event(GdkEventKey *key_event) {

    return Gtk::Window::on_key_press_event(key_event);
}

bool OMainWnd::on_key_release_event(GdkEventKey *key_event) {
    if (key_event->keyval == GDK_KEY_space) {
        //        m_button_play->set_active(!m_button_play->get_active());
        //        return true;
    }

    if (key_event->keyval == GDK_KEY_Home) {
        on_button_back_clicked();
        return true;
    }

    if (key_event->keyval == GDK_KEY_t) {
        m_btn_teach->set_active(!m_btn_teach->get_active());
        return true;
    }
    if (key_event->keyval == GDK_KEY_z) {
        on_btn_zoom_loop_clicked();
        return true;
    }

    if (key_event->keyval == GDK_KEY_s) {
        on_btn_loop_start_clicked();
        return true;
    }

    if (key_event->keyval == GDK_KEY_e) {
        on_btn_loop_end_clicked();
        return true;
    }

    return Gtk::Window::on_key_release_event(key_event);
}

bool OMainWnd::on_delete_event(GdkEventAny *any_event) {

    return Shutdown();
}

void OMainWnd::on_menu_project_new() {

    std::string location;

    if (SelectProjectLocation(true)) {

        NewProject();
        show_all_children(true);
    }
}

void OMainWnd::on_menu_project_open() {
    if (SelectProjectLocation(false)) {
        if (m_project.GetDirty()) {
            if (!SaveProject())
                return;
        }
        m_project.Close();
        m_trackslayout.RemoveAllTackViews();
        OpenProject(m_project.GetProjectLocation());
        m_project.AddRecentProject(m_project.GetProjectLocation());
        // TODO: reimplement
        // settings->set_string_array(SETTINGS_RECENT_PROJECTS, m_project.m_recent_projects);
        UpdateMenuRecent();
        show_all_children(true);
    }
}

void OMainWnd::on_menu_recent(std::string location) {
    if (m_project.GetDirty()) {
        SaveProject();
    }
    m_project.Close();
    m_trackslayout.RemoveAllTackViews();

    OpenProject(location);
    m_project.AddRecentProject(m_project.GetProjectLocation());
    // TODO: reimplement
    // settings->set_string_array(SETTINGS_RECENT_PROJECTS, m_project.m_recent_projects);
    UpdateMenuRecent();
    UpdateDawTime(false);
    show_all_children(true);
}

void OMainWnd::on_menu_project_save() {
    if (m_project.GetProjectLocation().length() == 0) {
        on_menu_project_new();
    }
    m_project.Save();
    m_project.AddRecentProject(m_project.GetProjectLocation());
    // TODO: remimplement
    //settings->set_string_array(SETTINGS_RECENT_PROJECTS, m_project.m_recent_projects);
    UpdateMenuRecent();
}

void OMainWnd::on_menu_project_save_as() {
    if (!SelectProjectLocation(true)) {
        return;
    }
    m_project.Save();
    m_project.AddRecentProject(m_project.GetProjectLocation());
    // TODO: reimplement
    //settings->set_string_array(SETTINGS_RECENT_PROJECTS, m_project.m_recent_projects);
    UpdateMenuRecent();
    set_title("autoX32 - [" + m_project.GetProjectLocation() + "]");
}

void OMainWnd::on_menu_project_close() {
    if (m_project.GetDirty()) {
        if (!SaveProject())
            return;
    }
    m_project.Close();
    m_trackslayout.RemoveAllTackViews();
    set_title("autoX32 - [untitled]");

}

void OMainWnd::on_menu_prefs() {
    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_string(main_inline_glade);
    ODlgPrefs *pDialog = nullptr;
    builder->get_widget_derived("dlg-prefs", pDialog);

    // fill dialog with data
    pDialog->SetShowTrackPath(m_config.get_boolean(SETTINGS_SHOW_PATH_ON_TRACK));
    pDialog->SetSmoothScreen(m_config.get_boolean(SETTING_SMOOTH_SCREEN));

    pDialog->run();
    if (pDialog->m_result) {
        m_config.set_boolean(SETTINGS_SHOW_PATH_ON_TRACK, pDialog->GetShowTrackPath());
        m_config.set_boolean(SETTING_SMOOTH_SCREEN, pDialog->GetSmoothScreen());
    }
    //queue_draw();
}

void OMainWnd::on_menu_layout() {
    m_trackslayout.EditLayout();
}

void OMainWnd::on_menu_expand_all() {
    m_trackslayout.ExpandCollapseAll(true);
}

void OMainWnd::on_menu_collapse_all() {
    m_trackslayout.ExpandCollapseAll(false);
}

void OMainWnd::on_menu_reset_all() {
    m_trackslayout.ResetAll();
}

void OMainWnd::on_menu_fit() {
    m_trackslayout.FitView(m_mainbox->get_height() 
        - m_tool_box->get_height() 
        - m_timebox->get_height() 
        - m_overview->get_height()
        - m_status_box->get_height());
}

void OMainWnd::on_btn_teach_clicked() {

    m_btn_teach->set_icon_widget(m_btn_teach->get_active() ? m_img_teach_on : m_img_teach_off);
    m_btn_teach->show_all();
    if (m_btn_teach->get_active()) {
        m_backend->ControllerShowTeachOn();
    } else {
        m_backend->ControllerShowTeachOff();
        m_trackslayout.StopTeach();

    }


}

void OMainWnd::on_btn_loop_start_clicked() {

    m_timeview->SetLoopStart();
    m_daw.SetRange(m_timeview->GetLoopStart(), m_timeview->GetLoopEnd());
}

void OMainWnd::on_btn_loop_end_clicked() {

    m_timeview->SetLoopEnd();
    m_daw.SetRange(m_timeview->GetLoopStart(), m_timeview->GetLoopEnd(), m_project.GetPlaying());
}

void OMainWnd::on_btn_zoom_loop_clicked() {
    m_timeview->SetZoomLoop();
    UpdateDawTime(true);
    m_overview->queue_draw();
}

void OMainWnd::on_button_play_clicked() {

    m_button_play->set_icon_widget(m_button_play->get_active() ? m_img_play_on : m_img_play_off);
    m_button_play->show_all();

    m_last_playhead_update = 0;

    if (!m_button_play->get_active()) {
        m_project.SetPlaying(false);
        if (m_project.GetProjectLocation() != "" && m_project.GetDirty())
            m_project.Save();
        m_trackslayout.StopRecord();
        if (!m_lock_play)
            //m_daw.Stop();
            m_backend->Stop();
        if (!m_shot_refresh && m_config.get_boolean(SETTING_SMOOTH_SCREEN))
            this->get_window()->thaw_updates();
        m_shot_refresh = 0;
    } else {
        m_project.SetPlaying(true);
        if (!m_lock_play)
            //m_daw.Play();
            m_backend->Play();
        if (m_config.get_boolean(SETTING_SMOOTH_SCREEN))
            this->get_window()->freeze_updates();
    }
}

void OMainWnd::on_button_back_clicked() {
    m_backend->Locate(m_project.GetLoopStart());
    UpdatePlayhead();
}

void OMainWnd::on_button_test_clicked() {

}

void OMainWnd::on_timeline_pos_changed() {
    m_lock_daw_time_event = true;
    //m_daw.SetPosition(m_timer->GetPosMillis() * 48, m_button_play->get_active());
    m_backend->Locate(m_timeview->GetClickMillis());
    // UpdatePlayhead();
}

void OMainWnd::on_timeline_zoom_changed() {
    m_timeview->queue_draw();
    m_trackslayout.redraw();
}

void OMainWnd::OnOverViewEvent() {
    UpdateDawTime(true);
    m_overview->queue_draw();
    m_trackslayout.queue_draw();
}

void OMainWnd::notify_overview() {
    m_OverViewDispatcher.emit();
}

void OMainWnd::UpdateDawTime(bool redraw) {
    if (!m_lock_daw_time) {
        m_lock_daw_time = true;
        UpdatePlayhead();
        m_timeview->UpdateDawTime(redraw);
        m_lock_daw_time = false;
    }
}

void OMainWnd::UpdatePlayhead() {
    daw_time* dt = m_project.GetDawTime();
    gint pos = ((GetPosMillis()) - dt->m_viewstart) * dt->scale;
    m_playhead->set_x_pos(pos);
    m_overview->SetPos(GetPosMillis());
}


void OMainWnd::TrackViewUp(std::string path) {
    m_trackslayout.TrackUp(path);
    m_project.SetDirty();
}

void OMainWnd::TrackViewDown(std::string path) {
    m_trackslayout.TrackDown(path);
    m_project.SetDirty();
}

void OMainWnd::TrackViewHide(std::string path) {
    m_trackslayout.TrackHide(path, false);
    m_project.SetDirty();
}
