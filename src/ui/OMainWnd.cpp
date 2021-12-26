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

#include "res/autoX32.h"

static void mixer_callback(OscCmd* cmd, void* user_data) {
    ((OMainWnd*)user_data)->notify_mixer(cmd);
}


void check_ardour_recent(void* user_Data) {
    FILE* file_recent;
    char name[256];
    char path[256];
    int dummy;

    OMainWnd* mainWnd = (OMainWnd*) user_Data;

    file_recent = fopen("/home/onkel/.config/ardour6/recent", "r");
    if (file_recent != NULL) {
        dummy = fscanf(file_recent, "%s", name);
        dummy = fscanf(file_recent, "%s", path);
        fclose(file_recent);
        strncat(path, "/autoX32", 32);
        if (strncmp(path, mainWnd->GetProjectLocation().data(), strlen(path))) {
            mainWnd->CloseProject();
            
            if (access(path, F_OK)) {
                printf("project don't exists\n");
                if (mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP) != 0) {
                    perror("mkdir() error");
                    return;
                }
                mainWnd->NewProject(path);
            }
            mainWnd->OpenProject(path);
        }
    }
}


OMainWnd::OMainWnd() : Gtk::Window()
{

    set_name("OMainWnd");
    ui = Gtk::Builder::create_from_string(main_inline_glade);
    set_icon(Gdk::Pixbuf::create_from_inline(sizeof(autoX32_inline), autoX32_inline, false));
    m_project.SetTracksLayout(&m_trackslayout);

    ApplyWindowSettings(); 
    
    create_view();
    create_menu();
    m_timeview->SetRange(m_project.GetTimeRange());
    m_timeview->SetDawTime(m_project.GetDawTime());

    this->add_events(Gdk::KEY_PRESS_MASK);
    this->add_events(Gdk::KEY_RELEASE_MASK);

    create_about_dlg();

    m_JackDispatcher.connect(sigc::mem_fun(*this, &OMainWnd::OnJackEvent));

    m_DawDispatcher.connect(sigc::mem_fun(*this, &OMainWnd::OnDawEvent));

    m_MixerDispatcher.connect(sigc::mem_fun(*this, &OMainWnd::OnMixerEvent));

    m_OverViewDispatcher.connect(sigc::mem_fun(*this, &OMainWnd::OnOverViewEvent));

    m_ViewDispatcher.connect(sigc::mem_fun(*this, &OMainWnd::OnViewEvent));

    show_all_children(true);
    queue_draw();
    m_x32 = new OX32();

    m_lock_play = false;
    m_lock_daw_time = false;
    m_lock_daw_time_event = false;

}

OMainWnd::~OMainWnd() {
    if (m_x32)
        delete m_x32;
    if (m_timer) {
        m_timer->stop();
        while (m_timer->isRunning());
        delete m_timer;
    }
    if (m_timeview)
        delete m_timeview;
    delete m_bbox;
}

void OMainWnd::on_activate() {

    if (!SetupBackend()) {
        exit(1);
    }    
    AutoConnect();
}

OConfig* OMainWnd::GetConfig() {
    return &m_config;
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
                //TODO: reimplement
                m_config.set_string_array(SETTINGS_RECENT_PROJECTS, m_project.m_recent_projects);
                UpdateMenuRecent();
                return true;
            } else {
                return false;
            }
        } else {
            m_project.Save();
            m_project.AddRecentProject(m_project.GetProjectLocation());
            //TODO: reimplement
            m_config.set_string_array(SETTINGS_RECENT_PROJECTS, m_project.m_recent_projects);
            UpdateMenuRecent();
            return true;
        }
    }
    return true;
}

bool OMainWnd::Shutdown() {
    bool ret_code = false;
    if (m_project.GetDirty()) {
        if (!SaveProject())
            return ret_code;
    }

    m_x32->Disconnect();
    m_daw.Disconnect();

    int width, height;
    get_size(width, height);

    m_config.set_int(SETTINGS_WINDOW_WIDTH, width);
    m_config.set_int(SETTINGS_WINDOW_HEIGHT, height);
    get_position(width, height);
    m_config.set_int(SETTINGS_WINDOW_LEFT, width);
    m_config.set_int(SETTINGS_WINDOW_TOP, height);
    return ret_code;
}

void OMainWnd::ApplyWindowSettings() {
    int width = m_config.get_int(SETTINGS_WINDOW_WIDTH, 400);
    int height = m_config.get_int(SETTINGS_WINDOW_HEIGHT, 300);
    
    set_default_size(width, height);
    move(m_config.get_int(SETTINGS_WINDOW_LEFT), m_config.get_int(SETTINGS_WINDOW_TOP));
}

void OMainWnd::AutoConnect() {
    std::string reply_port = m_config.get_string(SETTINGS_DAW__REPLAY_PORT);
    if (m_config.get_boolean(SETTINGS_MIXER_AUTOCONNECT)) {
        ConnectMixer(m_config.get_string(SETTINGS_MIXER_HOST));
    }

    if (m_config.get_boolean(SETTINGS_DAW_AUTOCONNECT)) {
        ConnectDaw(m_config.get_string(SETTINGS_DAW_HOST)
                , m_config.get_string(SETTINGS_DAW_PORT)
                , reply_port);
    }
    m_backend->Start();
}

bool OMainWnd::ConnectMixer(std::string host) {
    if (m_x32->IsConnected()) {
        m_x32->Disconnect();
    }
    if (!m_x32->Connect(host)) {
        m_project.SetMixer(m_x32);
        m_lbl_x32->set_label("X32: connected");
        m_x32->SetMixerCallback(mixer_callback, this);
        return true;
    }
    m_project.SetMixer(NULL);
    m_lbl_x32->set_label("X32: disconnected");
    return false;
}

bool OMainWnd::ConnectDaw(std::string ip, std::string port, std::string replyport) {
    if (!m_daw.Connect(ip.data(), port.data(), replyport.data(), this)) {

        m_button_play->set_sensitive(true);
        m_lbl_ardour->set_label("Ardour: connected");
        m_timer = new OTimer(check_ardour_recent, 5000, this);
        m_timer->start();
        return true;
    }
    m_lbl_ardour->set_label("Ardour: disconnected");
    return false;
}

void OMainWnd::NewProject(std::string path) {
    m_project.SetProjectLocation(path);
    m_project.New();
}

void OMainWnd::NewProject() {

    m_project.New();
}

void OMainWnd::OpenProject(std::string location) {
    m_project.Load(location);

    set_title("autoX32 - [" + location + "]");

    std::map<std::string, IOTrackStore*> tracks = m_project.GetTracks();

    for (gint i = 0; i < tracks.size(); i++) {
        for (std::map<std::string, IOTrackStore*>::iterator it = tracks.begin(); it != tracks.end(); ++it) {
            if (it->second->GetLayout()->m_index == i) {
                OTrackView* trackview = new OTrackView(this, m_project.GetDawTime());
                trackview->SetTrackStore(it->second);
                trackview->UpdateConfig();
                m_trackslayout.AddTrack(trackview, it->second->GetLayout()->m_visible);
            }
        }
    }
    UpdateDawTime(false);
    on_btn_zoom_loop_clicked();
    m_project.UpdatePos(GetPosMillis(), true);
}

std::string OMainWnd::GetProjectLocation() {
    return m_project.GetProjectLocation();
}

void OMainWnd::SetProjectLocation(std::string location) {
    m_project.SetProjectLocation(location);
}

void OMainWnd::CloseProject() {
    m_trackslayout.RemoveAllTackViews();
 
    m_project.Close();
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
    printf("remove %s\n", cmd->GetPath().data());
    m_trackslayout.RemoveTrackView(cmd->GetPath());
    m_project.RemoveCommand(cmd);
}

void OMainWnd::SelectTrack(std::string path, bool selected) {
    m_trackslayout.SelectTrack(path, selected);
    if (selected) {
        m_backend->ControllerShowLevel(m_trackslayout.GetSelectedTrackValue());
        m_backend->ControllerShowLCDName(m_trackslayout.GetSelectedTrackName());
    }
}

bool OMainWnd::SetupBackend() {

    ODlgProlog *pDialog = nullptr;
    ui->get_widget_derived("dlg-prolog", pDialog);
    
    pDialog->set_icon(get_icon());
    
    pDialog->SetMidiBackend(GetConfig()->get_int(SETTINGS_MIDI_BACKEND));
    
    pDialog->run();
    if (!pDialog->GetResult()) {
        return 0;
    }
    
    GetConfig()->set_int(SETTINGS_MIDI_BACKEND, pDialog->GetMidiBackend());
    
    switch(pDialog->GetMidiBackend()) {
        case 0:
            m_backend = new OAlsa();
            break;
        case 1:
            m_backend = new OJack();
            break;
        default:
            return 0;
    }

    m_backend->Connect(this);

    return 1;
}

gint OMainWnd::GetPosMillis() {
    return m_backend->GetMillis();
}

void OMainWnd::UpdatePos(gint current, bool jump) {
    trackview_entry* tv = m_trackslayout.GetTrackHead();
    while (tv) {
        IOTrackStore* trackstore = tv->item->GetTrackStore();
        track_entry* entry = trackstore->UpdatePlayhead(current, jump);
        if (entry) {
            m_project.PlayTrackEntry(trackstore, entry);
            if (tv->item->GetSelected())
                m_backend->ControllerShowLevel(entry->val.f);
        }
        tv = tv->next;
    }
}