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
#include "ODlgPrefs.h"
#include "embedded/autoX32_css.h"
#include <ostream>


ODlgPrefs::ODlgPrefs(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Dialog(cobject) 
{
    set_name("ODlgPrefs");
    set_title("autoX32 - Preferences");
    
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
    
    // setup buttons
    builder->get_widget< Gtk::Button >("btn-cancel", m_btn_cancel);
    m_btn_cancel->signal_clicked().connect(sigc::mem_fun(*this, &ODlgPrefs::on_btn_cancel_clicked));
    builder->get_widget< Gtk::Button >("btn-ok", m_btn_ok);
    m_btn_ok->signal_clicked().connect(sigc::mem_fun(*this, &ODlgPrefs::on_btn_ok_clicked));    
    
    builder->get_widget< Gtk::CheckButton >("chk-show-track-path", m_chk_showtrackpath);
    builder->get_widget< Gtk::CheckButton >("chk-smooth-screen", m_chk_smooth_screen);
    
}

ODlgPrefs::~ODlgPrefs() {
    
}

void ODlgPrefs::on_btn_cancel_clicked() {
    m_result = false;
    hide();
}

void ODlgPrefs::on_btn_ok_clicked() {
    m_result = true;
    hide();
}

void ODlgPrefs::SetShowTrackPath(bool val) {
    m_chk_showtrackpath->set_active(val);
}

bool ODlgPrefs::GetShowTrackPath() {
    return m_chk_showtrackpath->get_active();
}

void ODlgPrefs::SetSmoothScreen(bool val) {
    m_chk_smooth_screen->set_active(val);
}

bool ODlgPrefs::GetSmoothScreen() {
    return m_chk_smooth_screen->get_active();
}
