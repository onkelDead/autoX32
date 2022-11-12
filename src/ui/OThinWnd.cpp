/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <gtkmm.h>
#include "OThinWnd.h"
#include "res/autoX32.h"
#include "res/autoX32_css.h"


OThinWnd::OThinWnd() : Gtk::Window() {
    set_name("OMainWnd");
    ui = Gtk::Builder::create_from_string(main_inline_glade);
    
    ui->get_widget<Gtk::Box>("thin-box", m_mainbox);
    m_mainbox->set_valign(Gtk::ALIGN_FILL);
    m_mainbox->set_vexpand(true);
    add(*m_mainbox);

    m_bbox = new Gtk::Box();
    m_bbox->set_orientation(Gtk::ORIENTATION_VERTICAL);

    m_overlay = new Gtk::Overlay();
    m_mainbox->add(*m_overlay); 
    m_playhead = new OPlayHead();
    m_playhead->set_halign(Gtk::ALIGN_START);
    m_playhead->set_hexpand(false);
    m_playhead->set_size_request(1, -1);
    m_playhead->set_margin_start(160);
    
    m_overlay->add_overlay(*m_bbox);
    m_overlay->add_overlay(*m_playhead);
    m_overlay->set_overlay_pass_through(*m_bbox, false);
    
    m_refCssProvider = Gtk::CssProvider::create();
    auto refStyleContext = get_style_context();
    refStyleContext->add_provider(m_refCssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    try {
        m_refCssProvider->load_from_data(autoX32_inline_css);
    } catch (const Gtk::CssProviderError& ex) {
        std::cerr << "CssProviderError, Gtk::CssProvider::load_from_path() failed: "
                << ex.what() << std::endl;
    } catch (const Glib::Error& ex) {
        std::cerr << "Error, Gtk::CssProvider::load_from_path() failed: "
                << ex.what() << std::endl;
    }

    //	auto refStyleContext = get_style_context();
    auto screen = Gdk::Screen::get_default();
    refStyleContext->add_provider_for_screen(Gdk::Screen::get_default(), m_refCssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);    
    
    thin = new OTrackDrawThin(m_project->GetDawTime());
    thin->set_halign(Gtk::ALIGN_FILL);
    thin->set_vexpand(true);
    m_bbox->add(*thin);
    show_all_children(true);
    queue_draw(); 
    
    m_ViewDispatcher.connect(sigc::mem_fun(*this, &OThinWnd::OnUIOperation));

    ApplyWindowSettings();

}

OThinWnd::~OThinWnd() {
    
}

OConfig* OThinWnd::GetConfig() {
    return &m_config;
}


void OThinWnd::ApplyWindowSettings() {
    int width = m_config.get_int(SETTINGS_THIN_WIDTH, 800);
    int height = m_config.get_int(SETTINGS_THIN_HEIGHT, 300);

    set_default_size(width, height);
    move(m_config.get_int(SETTINGS_THIN_LEFT), m_config.get_int(SETTINGS_THIN_TOP));
}

bool OThinWnd::on_delete_event(GdkEventAny *any_event) {

    return Shutdown();
}


bool OThinWnd::Shutdown() {
    bool ret_code = false;

    m_backend->ControllerReset();
    
    m_mixer->Disconnect();
    m_daw->Disconnect();

    int width, height;
    get_size(width, height);

    m_config.set_int(SETTINGS_THIN_WIDTH, width);
    m_config.set_int(SETTINGS_THIN_HEIGHT, height);
    get_position(width, height);
    m_config.set_int(SETTINGS_THIN_LEFT, width);
    m_config.set_int(SETTINGS_THIN_TOP, height);
    return ret_code;
}

void OThinWnd::on_activate() {

    
    if (InitDaw(this)) {
        exit(1);
    }
    
    if (InitMixer(this)) {
        exit(1);
    }
    
    if (InitBackend(this)) {
        exit(1);
    }
    
    StartEngine(this);
}

void OThinWnd::OnSelectTrack() {
    PublishUiEvent(E_OPERATION::select_track, NULL);
}

void OThinWnd::PublishUiEvent(E_OPERATION what, void *with) {
    operation_t *ue = new operation_t;
    ue->event = what;
    ue->context = with;
    PublishUiEvent(ue);
}

void OThinWnd::PublishUiEvent(operation_t *ue) {
    m_queue_operation.push(ue);
    m_ViewDispatcher.emit();
}

void OThinWnd::OnUIOperation() {
    operation_t *op;
    static int phc = 0;
    m_queue_operation.front_pop(&op);
    if (op) {

        switch (op->event) {
            case E_OPERATION::new_pos:
                UpdatePlayhead(false);
                break;
            case E_OPERATION::pos_next:
                if (phc++ > 10) {
                    UpdatePlayhead(false);
                    phc = 0;
                }
                break;
            case E_OPERATION::select_track:
            {
                thin->SetTrackStore(m_project->GetTrackSelected());
                thin->queue_draw();
                m_playhead->set_x_pos(m_playhead->calc_new_pos(m_project->GetDawTime(), m_backend->GetFrame()));
            }
                break;
            case E_OPERATION::unselect_track:
                thin->SetTrackStore(nullptr);
                thin->queue_draw();
                break;
        }
    }
}


void OThinWnd::UpdatePlayhead(bool doCalc) {
    bool up = m_playhead->calc_new_pos(m_project->GetDawTime(), m_backend->GetFrame());
    
    if (up) {
        m_playhead->set_x_pos(0);
    }
}

void OThinWnd::OnLocate(bool complete) {
    if (!complete)
        PublishUiEvent(E_OPERATION::new_pos, NULL);
    else
        PublishUiEvent(E_OPERATION::pos_next, NULL);
}

void OThinWnd::OnUnselectTrack() {
    PublishUiEvent(E_OPERATION::unselect_track, NULL);
}

void OThinWnd::OnEngineCenterThin() {

}