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
#include "embedded/autoX32_css.h"
#include "embedded/main.h"
#include "ODlgLayout.h"

ODlgLayout::ODlgLayout(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Dialog(cobject) {
    set_name("ODlgLayout");
    set_title("autoX32 - Layout");

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

    auto screen = Gdk::Screen::get_default();
    refStyleContext->add_provider_for_screen(Gdk::Screen::get_default(), m_refCssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    builder->get_widget< Gtk::Button >("btn-l-ok", m_btn_ok);
    m_btn_ok->signal_clicked().connect(sigc::mem_fun(*this, &ODlgLayout::on_btn_ok_clicked));

    builder->get_widget< Gtk::Button >("btn-collapse-all", m_btn_collapse);
    builder->get_widget< Gtk::Button >("btn-expand-all", m_btn_expand);
    m_btn_collapse->signal_clicked().connect(sigc::mem_fun(*this, &ODlgLayout::on_btn_collapse));
    m_btn_expand->signal_clicked().connect(sigc::mem_fun(*this, &ODlgLayout::on_btn_expand));    

    builder->get_widget< Gtk::Box >("view", m_view);
}

ODlgLayout::~ODlgLayout() {
}

void ODlgLayout::on_btn_ok_clicked() {
    m_result = true;
    hide();
}

void ODlgLayout::on_btn_collapse() {
    for (std::map<std::string, ODlgLayoutTrack*>::iterator it = m_expanders.begin(); it != m_expanders.end(); ++it) {
        it->second->SetCollapsed(false);
    }
}

void ODlgLayout::on_btn_expand() {
    for (std::map<std::string, ODlgLayoutTrack*>::iterator it = m_expanders.begin(); it != m_expanders.end(); ++it) {
        it->second->SetCollapsed(true);
    }
}

void ODlgLayout::AddTrack(IOTracksLayout* parent, IOTrackView* view) {
    ODlgLayoutTrack *track = new ODlgLayoutTrack(parent, view);
    track->set_hexpand(true);
    track->set_vexpand(false);
    track->show_now();

    m_view->add(*track);
    m_view->show_all_children(true);
    m_expanders[view->GetCmd()->GetPath()] = track;
}

ODlgLayoutTrack::ODlgLayoutTrack(IOTracksLayout* parent, IOTrackView* view) : Gtk::Box(), ui{Gtk::Builder::create_from_string(main_inline_glade)}
{
    set_name("ODlgLayoutTrack");

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

    this->set_homogeneous(false);
    set_vexpand(true);

    m_label = new Gtk::Label();
    m_label->set_name("OTrackLayoutLabel");
    m_label->set_text(view->GetCmd()->GetPath());
    m_label->set_hexpand(true);
    m_label->set_alignment(0.0, 0.0);
    m_label->set_halign(Gtk::ALIGN_FILL);

    m_name = new Gtk::Label();
    m_name->set_name("OTrackLayoutName");
    m_name->set_text(view->GetCmd()->GetName());
    m_name->set_hexpand(true);
    m_name->set_alignment(0.0, 0.0);
    m_name->set_halign(Gtk::ALIGN_FILL);

    Gtk::Box *m_sb = new Gtk::Box();
    m_sb->set_homogeneous(true);

    m_check = new Gtk::CheckButton();
    m_check->set_vexpand(false);
    m_check->set_hexpand(false);
    m_check->set_active(view->GetTrackStore()->GetLayout()->m_visible);
    m_check->set_margin_left(8);
    m_check->set_margin_right(8);
    m_check->set_tooltip_text("Show/Hide Track");
    m_check->signal_toggled().connect(sigc::bind<>(sigc::mem_fun(*parent, &IOTracksLayout::on_hide_toggle), view, m_check));

    m_expand = new Gtk::CheckButton();
    m_expand->set_vexpand(false);
    m_expand->set_hexpand(false);
    m_expand->set_active(view->GetTrackStore()->GetLayout()->m_expanded);
    m_expand->set_margin_left(8);
    m_expand->set_margin_right(8);
    m_expand->set_tooltip_text("Expand/Collapse Track");
    m_expand->signal_toggled().connect(sigc::bind<>(sigc::mem_fun(*parent, &IOTracksLayout::on_expand_toggle), view, m_expand));

    add(*m_expand);
    add(*m_check);
    m_sb->add(*m_name);
    m_sb->add(*m_label);
    add(*m_sb);

    show_all_children(true);
}

ODlgLayoutTrack::~ODlgLayoutTrack() {
}

void ODlgLayoutTrack::SetCollapsed(bool colapse) {
    m_expand->set_active(colapse);
}