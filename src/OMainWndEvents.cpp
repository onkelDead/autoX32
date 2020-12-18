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
    if (pDialog->m_result) {

        if (ConnectDaw(pDialog->GetArdourHost(), pDialog->GetArdourPort(), pDialog->GetArdourReplyPort())) {
            settings->set_string("daw-host", pDialog->GetArdourHost());
            settings->set_string("daw-port", pDialog->GetArdourPort());
            settings->set_string("daw-reply-port", pDialog->GetArdourReplyPort());
            settings->set_boolean("daw-autoconnect", pDialog->GetArdoutAutoConnect());
        }

        if (ConnectMixer(pDialog->GetX32Host())) {
            settings->set_string("mixer-host", pDialog->GetX32Host());
            settings->set_boolean("mixer-autoconnect", pDialog->GetX32AutoConnect());
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
    if (key_event->keyval == GDK_KEY_Control_L) {
        m_timeview.SetScrollStep(1);
        return true;
    }

    if (key_event->keyval == GDK_KEY_Shift_L) {
        m_timeview.EnableZoom(true);

        return true;
    }

    return Gtk::Window::on_key_press_event(key_event);
}

bool OMainWnd::on_key_release_event(GdkEventKey *key_event) {
    if (key_event->keyval == GDK_KEY_Control_L) {
        m_timeview.SetScrollStep(5);
        return true;
    }
    if (key_event->keyval == GDK_KEY_Shift_L) {
        m_timeview.EnableZoom(false);

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
}

void OMainWnd::on_menu_project_close() {
    if (m_project.GetDirty()) {
        if (!SaveProject())
            return;
    }
    m_project.Close();
    m_trackslayout.RemoveAllTackViews();
}

void OMainWnd::on_btn_teach_clicked() {

    m_btn_teach->set_icon_widget(m_btn_teach->get_active() ? m_img_teach_on : m_img_teach_off);
    m_btn_teach->show_all();
}

void OMainWnd::on_btn_loop_start_clicked() {

    m_timeview.SetLoopStart();
}

void OMainWnd::on_btn_loop_end_clicked() {

    m_timeview.SetLoopEnd();
}

void OMainWnd::on_btn_zoom_loop_clicked() {

    m_timeview.SetZoomLoop();
    m_overview->queue_draw();
}

void OMainWnd::on_button_play_clicked() {

    m_button_play->set_icon_widget(m_button_play->get_active() ? m_img_play_on : m_img_play_off);
    m_button_play->show_all();

    if (lock_play)
        return;

    if (!m_button_play->get_active()) {
        m_project.SetPlaying(false);
        m_trackslayout.StopRecord();
        m_daw.Stop();
    } else {
        m_project.SetPlaying(true);
        m_daw.Play();
    }
}

void OMainWnd::on_button_back_clicked() {

    m_daw.SetPosition(m_project.GetLoopStart(), m_button_play->get_active());

}

void OMainWnd::on_button_test_clicked() {

    m_daw.Test();
}

void OMainWnd::on_btn_lock_playhead_clicked() {

    m_btn_lock_playhead->set_icon_widget(m_btn_lock_playhead->get_active() ? m_img_lock_playhead_on : m_img_lock_playhead_off);
    m_btn_lock_playhead->show_all();
    m_project.LockPlayhead(m_btn_lock_playhead->get_active());
}

void OMainWnd::on_timeline_pos_changed() {

    m_daw.SetPosition(m_project.GetCurrentSample(), m_button_play->get_active());
    m_trackslayout.redraw();
}

void OMainWnd::on_timeline_zoom_changed() {
    m_trackslayout.redraw();
}

void OMainWnd::on_btn_x32_clicked() {
}
