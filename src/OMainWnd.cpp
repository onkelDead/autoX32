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

#include <giomm/simpleactiongroup.h>
#include "OMainWnd.h"

#include "OscCmd.h"

#include "embedded/main.h"
#include "OTimer.h"

OMainWnd::OMainWnd() :
Gtk::Window(), ui{Gtk::Builder::create_from_string(main_inline_glade)} , m_x32(0) {

    set_name("OMainWnd");

    GSettingsSchemaSource *source = g_settings_schema_source_get_default();
    GSettingsSchema *schema = g_settings_schema_source_lookup(source, AUTOX32_SCHEMA_ID, true);
    if (schema) {
        settings = Gio::Settings::create(AUTOX32_SCHEMA_ID);
        m_project.m_recent_projects = settings->get_string_array("recent-projects");
    }
    g_settings_schema_unref(schema);

    m_timeview.SetRange(m_project.GetTimeRange());
    m_timeview.SetDawTime(m_project.GetDawTime());
    m_timeview.SetTimer(&m_timer);

    create_view();
    create_menu();

    this->add_events(Gdk::KEY_PRESS_MASK);
    this->add_events(Gdk::KEY_RELEASE_MASK);

    create_about_dlg();

    m_DawDispatcher.connect(sigc::mem_fun(*this, &OMainWnd::OnDawEvent));

    m_MixerDispatcher.connect(sigc::mem_fun(*this, &OMainWnd::OnMixerEvent));

    m_OverViewDispatcher.connect(sigc::mem_fun(*this, &OMainWnd::OnOverViewEvent));

    show_all_children(true);

    m_x32 = new OX32();

    lock_play = false;
    lock_daw_time = false;

    m_timer.setInterval(settings->get_int("track-resolution"));
    m_timer.SetUserData(&m_project);
    m_timer.setFunc(std::bind(&OMainWnd::TimerEvent, this, &m_project));
    
    AutoConnect();

}

OMainWnd::~OMainWnd() {

}

Gio::Settings* OMainWnd::GetSettings() {
    return settings.get();
}

bool OMainWnd::SaveProject() {
    Gtk::MessageDialog dialog(*this, "Project is modified.\nShall I save your changes?",
            false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
    int result = dialog.run();

    if (result == Gtk::RESPONSE_YES) {
        if (m_project.GetProjectLocation().length() == 0) {
            if (SelectProjectLocation(true)) {
                NewProject();
                m_project.Save();
                m_project.AddRecentProject(m_project.GetProjectLocation());
                settings->set_string_array("recent-projects", m_project.m_recent_projects);
                UpdateMenuRecent();
                return true;
            }
        } else {
            m_project.Save();
            m_project.AddRecentProject(m_project.GetProjectLocation());
            settings->set_string_array("recent-projects", m_project.m_recent_projects);
            UpdateMenuRecent();
            return true;
        }
    }
    return false;
}

bool OMainWnd::Shutdown() {
    bool ret_code = false;
    if (m_project.GetDirty()) {
        if (!SaveProject())
            return ret_code;
    }

    m_x32->Disconnect();
    m_daw.disconnect();

    int width, height;
    get_size(width, height);

    settings->set_int("window-width", width);
    settings->set_int("window-height", height);
    get_position(width, height);
    settings->set_int("window-left", width);
    settings->set_int("window-top", height);
    return ret_code;
}

void OMainWnd::LoadSettings() {
    if (settings) {
        set_default_size(settings->get_int("window-width"), settings->get_int("window-height"));
        move(settings->get_int("window-left"), settings->get_int("window-top"));
    }
}

void OMainWnd::AutoConnect() {
    if (settings->get_boolean("mixer-autoconnect")) {
        ConnectMixer(settings->get_string("mixer-host"));
    }

    if (settings && settings->get_boolean("daw-autoconnect")) {
        ConnectDaw(settings->get_string("daw-host")
                , settings->get_string("daw-port")
                , settings->get_string("daw-reply-port"));
    }
}

bool OMainWnd::ConnectMixer(std::string host) {
    if (m_x32->IsConnected()) {
        m_x32->Disconnect();
    }
    if (!m_x32->Connect(host, this)) {
        m_project.SetMixer(m_x32);
        return true;
    }
    m_project.SetMixer(NULL);
    return false;
}

bool OMainWnd::ConnectDaw(std::string ip, std::string port, std::string replyport) {
    if (!m_daw.connect(ip.data(), port.data(), replyport.data(), this)) {
        m_daw.ShortMessage("/refresh");
        m_daw.ShortMessage("/strip/list");
        m_button_play->set_sensitive(true);
        return true;
    }
    return false;
}

void OMainWnd::NewProject() {
    m_project.New();
}

void OMainWnd::OpenProject(std::string location) {
    m_project.Load(location);

    set_title("autoX32 - [" + location + "]");
    
    std::map<std::string, OTrackStore*> tracks = m_project.GetTracks();

    for (std::map<std::string, OTrackStore*>::iterator it = tracks.begin(); it != tracks.end(); ++it) {
        OTrackView* trackview = new OTrackView(this);
        trackview->BindRemove(this);
        trackview->SetDawTime(m_project.GetDawTime());
        trackview->SetTrackStore(it->second);
        trackview->UpdateConfig();
        m_trackslayout.AddTrack(trackview);
    }
    m_project.ProcessPos(NULL, &m_timer);
    UpdateDawTime(false);
    queue_draw();
}

bool OMainWnd::SelectProjectLocation(bool n) {
    Gtk::FileChooserDialog m_FileChooserDialog(*this, "Select project folder", n ? Gtk::FileChooserAction::FILE_CHOOSER_ACTION_CREATE_FOLDER : Gtk::FileChooserAction::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    m_FileChooserDialog.set_transient_for(*this);

    m_FileChooserDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    m_FileChooserDialog.add_button("_Select", Gtk::RESPONSE_OK);

    if (m_FileChooserDialog.run() == Gtk::RESPONSE_OK) {
        m_project.SetProjectLocation(m_FileChooserDialog.get_filename());
        return true;
    }
    return false;
}

void OMainWnd::remove_track(IOTrackView* view) {
    OscCmd* cmd = view->GetCmd();
    printf("remove %s\n", cmd->m_path.data());
    m_trackslayout.RemoveTrackView(cmd->GetPathStr());
    m_project.RemoveCommand(cmd);
}
