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

#include "embedded/autoX32.h"
#include "embedded/autoX32_css.h"

namespace sigc {
    SIGC_FUNCTORS_HAVE_RESULT_TYPE
}

void OMainWnd::create_about_dlg() {
    // create about dialog
    m_Dialog.set_transient_for(*this);

    m_Dialog.set_logo(Gdk::Pixbuf::create_from_inline(-1, autoX32_inline, FALSE));

    m_Dialog.set_program_name(PACKAGE_STRING);
    m_Dialog.set_version(PACKAGE_VERSION);
    m_Dialog.set_copyright("Copyright 2020 Detlef Urban");
    m_Dialog.set_comments("Behringer X32 Automation Recorder");
    m_Dialog.set_license("LGPL");

    m_Dialog.set_website("http://www.paraair.de/autoX32");
    m_Dialog.set_website_label("http://www.paraair.de");

    std::vector<Glib::ustring> list_authors;
    list_authors.push_back("Detlef Urban (" PACKAGE_BUGREPORT ")");
    m_Dialog.set_authors(list_authors);

    m_Dialog.signal_response().connect(
            sigc::mem_fun(*this, &OMainWnd::on_about_dialog_response));

}

void OMainWnd::UpdateMenuRecent(){
    while (m_recents->get_children().size() > 0) {
        Gtk::MenuItem* recent = (Gtk::MenuItem*)m_recents->get_children().at(0);
        m_recents->remove(*recent);
        delete recent;
    }
    for (std::vector<std::string>::iterator it = m_project.m_recent_projects.begin(); it != m_project.m_recent_projects.end(); ++it) {
        Gtk::MenuItem* m_recent = new Gtk::MenuItem();
        m_recent->set_label(*it);
        m_recents->add(*m_recent);
        m_recent->signal_activate().connect(sigc::bind<>(sigc::mem_fun(*this, &OMainWnd::on_menu_recent), *it));
    }
    if (m_project.m_recent_projects.size() > 0) {
        m_recents->set_sensitive(true);
    }
}

void OMainWnd::create_menu() {

    m_about = Glib::RefPtr<Gtk::MenuItem>::cast_dynamic(ui->get_object("menu_help_about"));
    m_about->signal_activate().connect(sigc::mem_fun(this, &OMainWnd::on_menu_file_about));

    m_quit_m = Glib::RefPtr<Gtk::MenuItem>::cast_dynamic(ui->get_object("menu_file_exit"));
    m_quit_m->signal_activate().connect(sigc::mem_fun(this, &OMainWnd::on_menu_file_exit));

    m_connection = Glib::RefPtr<Gtk::MenuItem>::cast_dynamic(ui->get_object("connect_m"));
    m_connection->signal_activate().connect(sigc::mem_fun(this, &OMainWnd::on_menu_file_connection));

    m_project_new = Glib::RefPtr<Gtk::MenuItem>::cast_dynamic(ui->get_object("menu_project_new"));
    m_project_new->signal_activate().connect(sigc::mem_fun(this, &OMainWnd::on_menu_project_new));

    m_project_open = Glib::RefPtr<Gtk::MenuItem>::cast_dynamic(ui->get_object("menu_project_open"));
    m_project_open->signal_activate().connect(sigc::mem_fun(this, &OMainWnd::on_menu_project_open));

    m_project_save = Glib::RefPtr<Gtk::MenuItem>::cast_dynamic(ui->get_object("menu_project_save"));
    m_project_save->signal_activate().connect(sigc::mem_fun(this, &OMainWnd::on_menu_project_save));

    m_project_save_as = Glib::RefPtr<Gtk::MenuItem>::cast_dynamic(ui->get_object("menu_project_save_as"));
    m_project_save_as->signal_activate().connect(sigc::mem_fun(this, &OMainWnd::on_menu_project_save_as));

    m_project_close = Glib::RefPtr<Gtk::MenuItem>::cast_dynamic(ui->get_object("menu_project_close"));
    m_project_close->signal_activate().connect(sigc::mem_fun(this, &OMainWnd::on_menu_project_close));

    m_recents = Glib::RefPtr<Gtk::Menu>::cast_dynamic(ui->get_object("menu-recent"));

    UpdateMenuRecent();
}

void OMainWnd::create_view() {

    Gtk::MenuBar *menubar = NULL;
    Gtk::Toolbar *toolbar = NULL;
    Gtk::Box *status  = NULL;

    ui->get_widget<Gtk::Box>("main-box", m_mainbox);
    
    ui->get_widget<Gtk::MenuBar>("main-menu", menubar);
    m_mainbox->add(*menubar);
    
    ui->get_widget<Gtk::Toolbar>("toolbar", toolbar);
    m_mainbox->add(*toolbar);
    
    Gtk::Box *bbox = new Gtk::Box();
    bbox->set_orientation(Gtk::ORIENTATION_VERTICAL);
    
    ui->get_widget<Gtk::Box>("time-box", m_timebox);
    bbox->add(*m_timebox);
    
    ui->get_widget<Gtk::ScrolledWindow>("scrolled-view", m_scroll);
    bbox->add(*m_scroll);
    
    m_overlay = new Gtk::Overlay();
    m_playhead = new OPlayHead();
    m_playhead->set_halign(Gtk::ALIGN_START);
    m_playhead->set_hexpand(false);
    m_playhead->set_size_request(1, -1);
    m_playhead->set_margin_start(160);
    
    m_overlay->add_overlay(*bbox);
    m_overlay->add_overlay(*m_playhead);
    m_overlay->set_overlay_pass_through(*bbox, false);
    m_mainbox->add(*m_overlay);    
    
    ui->get_widget<Gtk::Box>("box-status", status);
    m_mainbox->add(*status);    
    
    add(*m_mainbox);
    
    m_timeview.set_vexpand(false);
    m_timeview.set_valign(Gtk::ALIGN_START);
    m_timebox->add(m_timeview);

    ui->get_widget<Gtk::Box>("scroll-view", m_scrolledview);
    m_trackslayout.set_vexpand(true);
    m_trackslayout.set_valign(Gtk::ALIGN_FILL);
    m_scrolledview->add(m_trackslayout);

    Gtk::Box *box;
    ui->get_widget<Gtk::Box>("box-overview", box);
    m_overview = new OOverView(this);
    m_overview->m_daw_time = m_project.GetDawTime();

    box->add(*m_overview);


    // setup toolbar buttons
    ui->get_widget < Gtk::ToggleToolButton > ("play-button", m_button_play);
    ui->get_widget < Gtk::ToolButton > ("back-button", m_button_back);
    ui->get_widget < Gtk::ToggleToolButton > ("teach-button", m_btn_teach);
    ui->get_widget < Gtk::ToolButton > ("loop-start", m_btn_loop_start);
    ui->get_widget < Gtk::ToolButton > ("loop-end", m_btn_loop_end);
    ui->get_widget < Gtk::ToolButton > ("zoom-loop", m_btn_zoom_loop);
    ui->get_widget < Gtk::ToggleToolButton > ("lock-playhead", m_btn_lock_playhead);

    m_img_play_off.set(Gdk::Pixbuf::create_from_inline(-1, play_off_inline, FALSE));
    m_img_play_on.set(Gdk::Pixbuf::create_from_inline(-1, play_on_inline, FALSE));
    m_img_teach_off.set(Gdk::Pixbuf::create_from_inline(-1, teach_off_inline, FALSE));
    m_img_teach_on.set(Gdk::Pixbuf::create_from_inline(-1, teach_on_inline, FALSE));
    m_img_loop_start.set(Gdk::Pixbuf::create_from_inline(-1, loop_start_inline, FALSE));
    m_img_loop_end.set(Gdk::Pixbuf::create_from_inline(-1, loop_end_inline, FALSE));
    m_img_zoom_loop.set(Gdk::Pixbuf::create_from_inline(-1, zoom_loop_inline, FALSE));
    m_img_lock_playhead_off.set(Gdk::Pixbuf::create_from_inline(-1, lock_playhead_off_inline, FALSE));
    m_img_lock_playhead_on.set(Gdk::Pixbuf::create_from_inline(-1, lock_playhead_on_inline, FALSE));
    m_img_back.set(Gdk::Pixbuf::create_from_inline(-1, go_start_inline, FALSE));

    m_button_play->signal_clicked().connect(sigc::mem_fun(*this, &OMainWnd::on_button_play_clicked));
    m_button_play->set_icon_widget(m_img_play_off);
    m_button_play->show_all();

    m_btn_teach->signal_clicked().connect(sigc::mem_fun(*this, &OMainWnd::on_btn_teach_clicked));
    m_btn_teach->set_icon_widget(m_img_teach_off);
    m_btn_teach->show_all();

    m_btn_loop_start->signal_clicked().connect(sigc::mem_fun(*this, &OMainWnd::on_btn_loop_start_clicked));
    m_btn_loop_start->set_icon_widget(m_img_loop_start);
    m_btn_loop_start->show_all();

    m_btn_loop_end->signal_clicked().connect(sigc::mem_fun(*this, &OMainWnd::on_btn_loop_end_clicked));
    m_btn_loop_end->set_icon_widget(m_img_loop_end);
    m_btn_loop_end->show_all();

    m_btn_zoom_loop->signal_clicked().connect(sigc::mem_fun(*this, &OMainWnd::on_btn_zoom_loop_clicked));
    m_btn_zoom_loop->set_icon_widget(m_img_zoom_loop);
    m_btn_zoom_loop->show_all();

    m_btn_lock_playhead->signal_clicked().connect(sigc::mem_fun(*this, &OMainWnd::on_btn_lock_playhead_clicked));
    m_btn_lock_playhead->set_icon_widget(m_img_lock_playhead_off);
    m_btn_lock_playhead->show_all();

    m_button_back->signal_clicked().connect(sigc::mem_fun(*this, &OMainWnd::on_button_back_clicked));
    m_button_back->set_icon_widget(m_img_back);
    m_button_back->show_all();

    m_timeview.signal_pos_changed.connect(sigc::mem_fun(*this, &OMainWnd::on_timeline_pos_changed));
    m_timeview.signal_zoom_changed.connect(sigc::mem_fun(*this, &OMainWnd::on_timeline_zoom_changed));

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
}
