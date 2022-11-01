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
#include "res/autoX32.h"
#include "res/trackdlg.h"

void OMainWnd::OnTimer(void* user_data)  {
    m_backend->ReconnectPorts();
    OnJackEvent();
}

OMainWnd::OMainWnd() : Gtk::Window() {

    set_name("OMainWnd");
    ui = Gtk::Builder::create_from_string(main_inline_glade);
    set_icon(Gdk::Pixbuf::create_from_inline(sizeof (autoX32_inline), autoX32_inline, false));

    ApplyWindowSettings();

    create_view();
    create_menu();
    
    m_timeview->SetRange(m_project->GetTimeRange());
    m_timeview->SetDawTime(m_project->GetDawTime());

    this->add_events(Gdk::KEY_PRESS_MASK);
    this->add_events(Gdk::KEY_RELEASE_MASK);

    create_about_dlg();

    m_DawDispatcher.connect(sigc::mem_fun(*this, &OMainWnd::OnDawEvent));

    m_MessageDispatcher.connect(sigc::mem_fun(*this, &OMainWnd::OnMessageEvent));

    m_OverViewDispatcher.connect(sigc::mem_fun(*this, &OMainWnd::OnOverViewEvent));

    m_ViewDispatcher.connect(sigc::mem_fun(*this, &OMainWnd::OnUIOperation));

    show_all_children(true);
    queue_draw();
    m_mixer->SetMessageHandler(this);

    m_lock_play = false;
    m_lock_daw_time = false;
    m_lock_daw_time_event = false;

}

OMainWnd::~OMainWnd() {
    m_daw->StopSessionMonitor();
    StopEngine();
    if (m_timeview)
        delete m_timeview;
    delete m_bbox;
}

void OMainWnd::on_activate() {

    if (!SetupBackend()) {
        exit(1);
    }
    AutoConnect();
    
    m_mixer->Start();
        
    StartEngine(this);
}

OConfig* OMainWnd::GetConfig() {
    return &m_config;
}

bool OMainWnd::SaveProject() {
    Gtk::MessageDialog dialog(*this, "Project is modified.\nShall I save your changes?",
            false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
    int result = dialog.run();

    if (result == Gtk::RESPONSE_YES) {
        if (m_daw->GetLocation().length() == 0) {
            if (SelectProjectLocation(true)) {
                NewProject();
                m_project->Save(m_daw->GetLocation());
                return true;
            } else {
                return false;
            }
        } else {
            m_project->Save(m_daw->GetLocation());
            return true;
        }
    }
    return true;
}

bool OMainWnd::Shutdown() {
    bool ret_code = false;
    if (m_project->GetDirty()) {
        if (!SaveProject())
            return ret_code;
    }

    m_backend->ControllerShowActive(false);
    m_backend->ControllerShowRec(false);
    m_backend->ControllerShowTeachMode(false);
    m_backend->ControllerShowTeachOff();
    m_backend->ControllerShowLevel(0.0);
    
    m_mixer->Disconnect();
    m_daw->Disconnect();

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
}

bool OMainWnd::ConnectMixer(std::string host) {
    if (m_mixer->IsConnected()) {
        m_mixer->Disconnect();
    }
    if (!m_mixer->Connect(host)) {
        m_project->SetMixer(m_mixer);
        m_lbl_x32->set_label("X32: connected");
        return true;
    }
    m_project->SetMixer(NULL);
    m_lbl_x32->set_label("X32: disconnected");
    return false;
}

bool OMainWnd::ConnectDaw(std::string ip, std::string port, std::string replyport) {
    if (!m_daw->Connect(ip.data(), port.data(), replyport.data(), this)) {

        m_button_play->set_sensitive(true);
        m_lbl_ardour->set_label("Ardour: connected");
        m_daw->StartSessionMonitor();
        return true;
    }
    m_lbl_ardour->set_label("Ardour: disconnected");
    return false;
}

void OMainWnd::NewProject(std::string path) {
}

void OMainWnd::NewProject() {

}

int OMainWnd::OpenProject(std::string location) {

    if (m_project->Load(location))
        return 1;
    set_title("autoX32 - [" + location + "]");
    std::map<std::string, IOTrackStore*> tracks = m_project->GetTracks();

    for (size_t i = 0; i < tracks.size(); i++) {
        for (std::map<std::string, IOTrackStore*>::iterator it = tracks.begin(); it != tracks.end(); ++it) {
            IOTrackStore* ts = it->second;
            if (ts->GetLayout()->m_index == i) {
                ts->GetMessage()->SetTrackstore(ts);
                OTrackView* trackview = new OTrackView(this, m_project->GetDawTime());
                trackview->SetTrackStore(ts);
                ts->SetView(trackview);
                m_trackslayout.AddTrack(trackview, ts->GetLayout()->m_visible);
                track_entry* e = ts->GetEntryAtPosition(GetPosMillis(), true);
                ts->SetPlayhead(e);
                PlayTrackEntry(ts, e);
                ts->SetName(m_mixer->GetCachedMessage(ts->GetConfigRequestName())->GetVal(0)->GetString());
                ts->SetColor_index(m_mixer->GetCachedMessage(ts->GetConfigRequestColor())->GetVal(0)->GetInteger());
                m_trackslayout.GetTrackview(ts->GetPath())->SetTrackName(ts->GetName());
            }
        }
    }
    
    m_trackslayout.show_all();
    UpdateDawTime(false);
    m_daw->SetRange(m_project->GetTimeRange()->m_loopstart, m_project->GetTimeRange()->m_loopend);
    on_btn_zoom_loop_clicked();
    return 0;
}

void OMainWnd::CloseProject() {
    m_trackslayout.RemoveAllTackViews();

    m_project->Close();
}

bool OMainWnd::SelectProjectLocation(bool n) {
    Gtk::FileChooserDialog m_FileChooserDialog(*this, "Select project folder", n ? Gtk::FileChooserAction::FILE_CHOOSER_ACTION_CREATE_FOLDER : Gtk::FileChooserAction::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    m_FileChooserDialog.set_transient_for(*this);

    m_FileChooserDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    m_FileChooserDialog.add_button("_Select", Gtk::RESPONSE_OK);

    if (m_FileChooserDialog.run() == Gtk::RESPONSE_OK) {
        m_daw->SetLocation(m_FileChooserDialog.get_filename());
        return true;
    }
    return false;
}

void OMainWnd::remove_track(std::string path) {
    printf("remove %s\n", path.data());
    UnselectTrack();
    m_trackslayout.RemoveTrackView(path);
    m_project->RemoveTrack(path);
    m_mixer->ReleaseCacheMessage(path);
}

void OMainWnd::SelectTrackUI(std::string path, bool val) {
    SelectTrack(path, val);
}

void OMainWnd::EditTrack(std::string path) {
    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_string(trackdlg_inline_glade);
    OTrackDlg *pDialog = nullptr;
    builder->get_widget_derived("track-dlg", pDialog);
    pDialog->SetPath(path);

    IOTrackStore* ts = m_project->GetTrack(path);

    IOscMessage* nameMsg = m_mixer->GetCachedMessage(ts->GetConfigRequestName());
    pDialog->SetName(nameMsg->GetVal(0)->GetString());

    pDialog->SetCountEntries(ts->GetCountEntries());

    pDialog->run();
    if (pDialog->GetResult()) {
        m_mixer->SendString(nameMsg->GetPath(), pDialog->GetName());
        m_trackslayout.GetTrackview(path)->SetTrackName(pDialog->GetName());
    }
}

void OMainWnd::ToggleSolo() {

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

    switch (pDialog->GetMidiBackend()) {
        case 0:
            m_backend = new OAlsa();
            break;
        case 1:
            //m_backend = new OJack(GetConfig());
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

bool OMainWnd::PlayTrackEntry(IOTrackStore* trackstore, track_entry* entry) {
    if (entry == nullptr || trackstore == nullptr)
        return false;
    IOscMessage* cmd = trackstore->GetMessage();
    switch (cmd->GetTypes()[0]) {
        case 'f':
            m_mixer->SendFloat(cmd->GetPath(), entry->val.f);
            break;
        case 'i':
            m_mixer->SendInt(cmd->GetPath(), entry->val.i);
            break;
        case 's':
            m_mixer->SendString(cmd->GetPath(), &entry->val.s);
            break;            
    }
    return true;    
}