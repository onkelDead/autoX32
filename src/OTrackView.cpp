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

#include <libgen.h>
#include "OTrackStore.h"
#include "OTrackView.h"
#include "OTrackDlg.h"
#include "OResource.h"
#include "OX32.h"

#include "embedded/trackview.h"
#include "embedded/trackdlg.h"

OTrackView::OTrackView(IOMainWnd* wnd) : Gtk::Box(), ui{Gtk::Builder::create_from_string(trackview_inline_glade)}
{
    m_parent = wnd;
    set_name("OTrackView");

    ui->get_widget < Gtk::Box > ("track-box", m_box);
    ui->get_widget < Gtk::Box > ("track-control", m_boxcontrol);
    ui->get_widget < Gtk::Toolbar > ("track-bar", m_toolbar);
    ui->get_widget < Gtk::Label > ("track-label", m_label);

    ui->get_widget < Gtk::ToggleToolButton > ("track-rec", m_btn_x32_rec);
    ui->get_widget < Gtk::ToggleToolButton > ("track-touch", m_btn_x32_touch);

    m_boxcontrol->set_name("OTrackControl");
    m_toolbar->set_name("OTrackControl");
    
    m_img_rec_off.set(Gdk::Pixbuf::create_from_inline(-1, (unsigned char*)rec_off_inline, FALSE));
    m_img_rec_on.set(Gdk::Pixbuf::create_from_inline(-1, (unsigned char*)rec_on_inline, FALSE));
    m_img_touch_off.set(Gdk::Pixbuf::create_from_inline(-1, (unsigned char*)touch_off_inline, FALSE));
    m_img_touch_on.set(Gdk::Pixbuf::create_from_inline(-1, (unsigned char*)touch_on_inline, FALSE));
    m_btn_x32_rec->signal_clicked().connect(sigc::mem_fun(*this, &OTrackView::on_button_x32_rec_clicked));
    m_btn_x32_rec->set_icon_widget(m_img_rec_off);
    m_btn_x32_rec->show_all();
    m_btn_x32_touch->signal_clicked().connect(sigc::mem_fun(*this, &OTrackView::on_button_x32_touch_clicked));
    m_btn_x32_touch->set_icon_widget(m_img_touch_off);
    m_btn_x32_touch->show_all();
    
    m_trackdraw = new OTrackDraw(wnd);
    m_trackdraw->set_halign(Gtk::ALIGN_FILL);
    m_trackdraw->set_hexpand(true);
    m_box->add(*m_trackdraw);
    m_label->set_text("TEST");
    m_box->set_vexpand(false);
    m_box->set_valign(Gtk::ALIGN_START);

    menu_popup_rename.set_label("Edit...");
    menu_popup_remove.set_label("Remove...");
    menu_popup.append(menu_popup_rename);
    menu_popup.append(menu_popup_remove);
    
    menu_popup_rename.signal_activate().connect(sigc::mem_fun(this, &OTrackView::on_menu_popup_edit));
    
    set_valign(Gtk::ALIGN_START);
    add(*m_box);
    show_all_children(true);
    
    BindRemove(wnd);
}

OTrackView::~OTrackView() {
    m_box->remove(*m_trackdraw);
    delete m_trackdraw;
}

void OTrackView::on_button_x32_rec_clicked() {
    m_btn_x32_rec->set_icon_widget(m_btn_x32_rec->get_active() ? m_img_rec_on : m_img_rec_off);
    m_btn_x32_rec->show_all();

    m_trackdraw->SetRecord(m_btn_x32_rec->get_active());

}

void OTrackView::on_button_x32_touch_clicked() {
    m_btn_x32_touch->set_icon_widget(m_btn_x32_touch->get_active() ? m_img_touch_on : m_img_touch_off);
    m_btn_x32_touch->show_all();

    m_trackdraw->SetTouch(m_btn_x32_touch->get_active());

}


void OTrackView::SetTrackStore(OTrackStore* trackstore) {
    m_trackdraw->SetTrackStore(trackstore);
}

OscCmd* OTrackView::GetCmd() {

    return m_trackdraw->GetCmd();
}

void OTrackView::SetDawTime(daw_time* dt) {

    m_trackdraw->m_daw_time = dt;
}

void OTrackView::UpdateConfig() {
    m_label->set_text(m_trackdraw->GetCmd()->m_name);
}

void OTrackView::SetRecord(bool val) {
    m_btn_x32_rec->set_active(val);
}

void OTrackView::SetTouch(bool val) {
    m_btn_x32_touch->set_active(val);
}

bool OTrackView::GetRecord() {
    return m_btn_x32_rec->get_active();
}

bool OTrackView::GetTouch() {
    return m_btn_x32_touch->get_active();
}

bool OTrackView::on_button_press_event(GdkEventButton* event) {
    if (event->button == 3) {
        menu_popup.show_all();
        menu_popup.popup(3, event->time);
        return true;
    }
    return false;
}

void OTrackView::on_menu_popup_edit() {
    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_string(trackdlg_inline_glade);
    OTrackDlg *pDialog = nullptr;
    builder->get_widget_derived("track-dlg", pDialog);
    pDialog->SetName(m_trackdraw->GetCmd()->m_name);
    pDialog->SetPath(m_trackdraw->GetCmd()->m_path);
    pDialog->SetColor(m_trackdraw->GetCmd()->m_color);
    pDialog->run();
    if (pDialog->m_result) {
        m_trackdraw->GetCmd()->m_name = pDialog->GetName();
        m_trackdraw->GetCmd()->m_color = pDialog->GetColor();
        UpdateConfig();
    }
}

void OTrackView::on_menu_popup_remove() {
    Gtk::MessageDialog dialog("Are you sure to remove this track from project?",
            false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
    int result = dialog.run();
    if (result == Gtk::RESPONSE_OK) {    
        m_parent->remove_track(this);
    }
}

void OTrackView::BindRemove(IOMainWnd* wnd) {
    menu_popup_remove.signal_activate().connect(sigc::mem_fun(*this, &OTrackView::on_menu_popup_remove));
    
}
