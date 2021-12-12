/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OCldProlog.cpp
 * Author: onkel
 * 
 * Created on December 10, 2021, 10:23 AM
 */

#include <iostream>

#include "ODlgProlog.h"

ODlgProlog::ODlgProlog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Dialog(cobject) {
    
    set_name("ODlgProlog");
        
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

    builder->get_widget< Gtk::ComboBoxText >("combo-backend", m_combo_backend);

    builder->get_widget< Gtk::Button >("btn-quit", m_btn_quit);
    m_btn_quit->signal_clicked().connect(sigc::mem_fun(*this, &ODlgProlog::on_btn_quit));

    
    builder->get_widget< Gtk::Button >("btn-start", m_btn_start);
    m_btn_start->signal_clicked().connect(sigc::mem_fun(*this, &ODlgProlog::on_btn_start));
}

ODlgProlog::ODlgProlog(const ODlgProlog& orig) {
}

ODlgProlog::~ODlgProlog() {
}

void ODlgProlog::on_btn_quit() {
    m_result = 0;
    hide();
}

void ODlgProlog::on_btn_start() {
    m_result = 1;
    hide();
}
