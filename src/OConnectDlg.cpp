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

#include "OConnectDlg.h"

#include "embedded/autoX32_css.h"

OConnectDlg::OConnectDlg(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Dialog(cobject) 
{
    set_name("OConnectDlg");
    
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
    m_btn_cancel->signal_clicked().connect(sigc::mem_fun(*this, &OConnectDlg::on_btn_cancel_clicked));
    builder->get_widget< Gtk::Button >("btn-ok", m_btn_ok);
    m_btn_ok->signal_clicked().connect(sigc::mem_fun(*this, &OConnectDlg::on_btn_ok_clicked));
    
    // setup controls
    builder->get_widget< Gtk::Entry >("txt-daw-host", m_txt_ardour_host);
    builder->get_widget< Gtk::Entry >("txt-daw-port", m_txt_ardour_port);
    builder->get_widget< Gtk::Entry >("txt-daw-reply-port", m_txt_ardour_reply_port);
    builder->get_widget< Gtk::CheckButton >("chk-daw-connect", m_chk_ardour_autoconnect);
    
    builder->get_widget< Gtk::Entry >("txt-x32-host", m_txt_x32_host);
    builder->get_widget< Gtk::CheckButton >("chk-x32-connect", m_chk_x32_autoconnect);

}

OConnectDlg::~OConnectDlg() {
}

void OConnectDlg::on_btn_cancel_clicked() {
    m_result = false;
    hide();
}

void OConnectDlg::on_btn_ok_clicked() {
    m_result = true;
    hide();
}

void OConnectDlg::SetArdourHost(std::string host) {
    m_txt_ardour_host->set_text(host);
}

void OConnectDlg::SetArdourPort(std::string port) {
    m_txt_ardour_port->set_text(port);
}

void OConnectDlg::SetArdourReplyPort(std::string reply_port) {
    m_txt_ardour_reply_port->set_text(reply_port);
}

void OConnectDlg::SetArdourAutoConnect(bool ac) {
    m_chk_ardour_autoconnect->set_active(ac);
}

std::string OConnectDlg::GetArdourHost() {
    return m_txt_ardour_host->get_text();
}

std::string OConnectDlg::GetArdourPort() {
    return m_txt_ardour_port->get_text();
}

std::string OConnectDlg::GetArdourReplyPort() {
    return m_txt_ardour_reply_port->get_text();
}

bool OConnectDlg::GetArdoutAutoConnect() {
    return m_chk_ardour_autoconnect->get_active();
}

void OConnectDlg::SetX32Host(std::string host) {
    m_txt_x32_host->set_text(host);
}

void OConnectDlg::SetX32AutoConnect(bool ac) {
    m_chk_x32_autoconnect->set_active(ac);
}

std::string OConnectDlg::GetX32Host() {
    return m_txt_x32_host->get_text();
}

bool OConnectDlg::GetX32AutoConnect() {
    return m_chk_x32_autoconnect->get_active();
}
