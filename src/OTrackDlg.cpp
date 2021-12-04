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

#include "OTrackDlg.h"

#include "embedded/autoX32_css.h"

OTrackDlg::OTrackDlg(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Dialog(cobject)
{
    set_name("OTrackDlg");
    
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
    
    builder->get_widget< Gtk::Button >("btn-cancel", m_btn_cancel);
    m_btn_cancel->signal_clicked().connect(sigc::mem_fun(*this, &OTrackDlg::on_btn_cancel_clicked));
    builder->get_widget< Gtk::Button >("btn-ok", m_btn_ok);
    m_btn_ok->signal_clicked().connect(sigc::mem_fun(*this, &OTrackDlg::on_btn_ok_clicked));

    builder->get_widget< Gtk::Entry >("txt-name", m_txt_name);
    builder->get_widget< Gtk::Entry >("txt-path", m_txt_path);
    builder->get_widget< Gtk::Label >("lbl-count-entries", m_count_entries);
    builder->get_widget< Gtk::ColorButton >("btn-color", m_btn_color);
    m_btn_color->set_name("OColorButton");
    

}


OTrackDlg::~OTrackDlg() {
}

void OTrackDlg::on_btn_cancel_clicked() {
    m_result = false;
    hide();
}

void OTrackDlg::on_btn_ok_clicked() {
    m_result = true;
    hide();
}

void OTrackDlg::SetName(std::string name) {
    m_txt_name->set_text(name);
}

std::string OTrackDlg::GetName() {
    return m_txt_name->get_text();
}

void OTrackDlg::SetPath(std::string path) {
    m_txt_path->set_text(path);
}

Gdk::RGBA OTrackDlg::GetColor() {
    return m_btn_color->get_rgba();
}

void OTrackDlg::SetColor(Gdk::RGBA color) {
    m_btn_color->set_rgba(color);
}

void OTrackDlg::SetCountEntries(gint count) {
    char tmp[32];
    sprintf(tmp, "%d", count);
    m_count_entries->set_text(tmp);
}

bool OTrackDlg::GetResult() {
	return m_result;
}
