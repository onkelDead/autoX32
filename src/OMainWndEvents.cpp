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
    pDialog->SetArdourHost(settings->get_string("daw-host"));
    pDialog->SetArdourPort(settings->get_string("daw-port"));
    pDialog->SetArdourReplyPort(settings->get_string("daw-reply-port"));
    pDialog->SetArdourAutoConnect(settings->get_boolean("daw-autoconnect"));
    pDialog->SetX32Host(settings->get_string("mixer-host"));
    pDialog->SetX32AutoConnect(settings->get_boolean("mixer-autoconnect"));
    pDialog->run();
    if (pDialog->GetResult()) {

        if (ConnectDaw(pDialog->GetArdourHost(), pDialog->GetArdourPort(), pDialog->GetArdourReplyPort())) {
            settings->set_string("daw-host", pDialog->GetArdourHost());
            settings->set_string("daw-port", pDialog->GetArdourPort());
            settings->set_string("daw-reply-port", pDialog->GetArdourReplyPort());
            settings->set_boolean("daw-autoconnect", pDialog->GetArdoutAutoConnect());
        }
        else {
            Gtk::MessageDialog dialog(*this, "Failed to connect to Ardour.",
                    false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
            dialog.run();
            return;
        }

        if (ConnectMixer(pDialog->GetX32Host())) {
            settings->set_string("mixer-host", pDialog->GetX32Host());
            settings->set_boolean("mixer-autoconnect", pDialog->GetX32AutoConnect());
        }
        else {
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

    if (key_event->keyval == GDK_KEY_f) {
    	m_btn_lock_playhead->set_active(!m_btn_lock_playhead->get_active());
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
        settings->set_string_array("recent-projects", m_project.m_recent_projects);
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
    settings->set_string_array("recent-projects", m_project.m_recent_projects);
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
    settings->set_string_array("recent-projects", m_project.m_recent_projects);
    UpdateMenuRecent();
}

void OMainWnd::on_menu_project_save_as() {
    if (!SelectProjectLocation(true)) {
        return;
    }
    m_project.Save();
    m_project.AddRecentProject(m_project.GetProjectLocation());
    settings->set_string_array("recent-projects", m_project.m_recent_projects);
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
    pDialog->SetShowTrackPath(settings->get_boolean("show-path-on-track"));
    pDialog->SetResolution(settings->get_int("track-resolution"));

    pDialog->run();
    if (pDialog->m_result) {
        settings->set_boolean("show-path-on-track", pDialog->GetShowTrackPath());
        settings->set_int("track-resolution", pDialog->GetResolution());
    }
    //queue_draw();
}

void OMainWnd::on_btn_teach_clicked() {

    m_btn_teach->set_icon_widget(m_btn_teach->get_active() ? m_img_teach_on : m_img_teach_off);
    m_btn_teach->show_all();
}

void OMainWnd::on_btn_loop_start_clicked() {

    m_timeview->SetLoopStart();
}

void OMainWnd::on_btn_loop_end_clicked() {

    m_timeview->SetLoopEnd();
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
        if (m_project.GetProjectLocation() != "") 
            m_project.Save();
        m_trackslayout.StopRecord();
        if (!m_lock_play)
            //m_daw.Stop();
        	m_jack.Stop();
        if (!m_shot_refresh)
            this->get_window()->thaw_updates();
        m_shot_refresh = 0;
    } else {
        m_project.SetPlaying(true);
        if (!m_lock_play)
        	//m_daw.Play();
        	m_jack.Play();
        this->get_window()->freeze_updates();
    }
}

void OMainWnd::on_button_back_clicked() {
    m_jack.Locate(m_project.GetLoopStart());
    UpdatePlayhead();
}

void OMainWnd::on_button_test_clicked() {

}

void OMainWnd::on_btn_lock_playhead_clicked() {

    m_btn_lock_playhead->set_icon_widget(m_btn_lock_playhead->get_active() ? m_img_lock_playhead_on : m_img_lock_playhead_off);
    m_btn_lock_playhead->show_all();
    m_project.LockPlayhead(m_btn_lock_playhead->get_active());
}

void OMainWnd::on_btn_cut_clicked() {
	m_btn_cut->set_icon_widget(m_btn_cut->get_active() ? m_img_cut_on : m_img_cut_off);
	m_btn_cut->show_all();
}

void OMainWnd::on_timeline_pos_changed() {
    m_lock_daw_time_event = true;
    //m_daw.SetPosition(m_timer->GetPosMillis() * 48, m_button_play->get_active());
    m_jack.Locate(m_timeview->GetClickMillis());
    //UpdatePlayhead();
}

void OMainWnd::on_timeline_zoom_changed() {
	m_timeview->queue_draw();
    m_trackslayout.redraw();
}

void OMainWnd::OnOverViewEvent() {
    UpdateDawTime(false);
    m_overview->queue_draw();
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
    if (pos < 0) {
        m_playhead->set_active(false);
        pos = 0;
    } else if (pos >= 0 && pos < 0xffff) {
        if (m_btn_lock_playhead->get_active() && m_button_play->get_active()) {
            gint offset = m_playhead->get_margin_start() - 160 - pos;
            dt->m_viewend -= offset / dt->scale;
            dt->m_viewstart -= offset / dt->scale;
            //queue_draw();
            m_playhead->set_active(true);
        } else {
            m_playhead->set_active(true);
            m_playhead->set_margin_start(160 + pos);
            //queue_draw();
        }
    }
}

