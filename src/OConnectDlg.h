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

#ifndef OCONNECTDLG_H
#define OCONNECTDLG_H

#include <gtkmm.h>
#include <gtkmm/builder.h>

class OConnectDlg : public Gtk::Dialog {
public:
    OConnectDlg(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~OConnectDlg();

    void on_btn_cancel_clicked();
    void on_btn_ok_clicked();
    
    void SetArdourHost(std::string);
    void SetArdourPort(std::string);
    void SetArdourReplyPort(std::string);
    void SetArdourAutoConnect(bool);
    std::string GetArdourHost();
    std::string GetArdourPort();
    std::string GetArdourReplyPort();
    bool GetArdoutAutoConnect();
    
    void SetX32Host(std::string);
    void SetX32AutoConnect(bool);
    std::string GetX32Host();
    bool GetX32AutoConnect();
    
    bool m_result;
    
protected:
    Glib::RefPtr<Gtk::CssProvider> m_refCssProvider;    
    Glib::RefPtr<Gtk::Builder> ui;
    
private:
    Gtk::Button* m_btn_cancel;
    Gtk::Button* m_btn_ok;

    Gtk::Entry* m_txt_ardour_host;
    Gtk::Entry* m_txt_ardour_port;
    Gtk::Entry* m_txt_ardour_reply_port;
    Gtk::CheckButton* m_chk_ardour_autoconnect;
    Gtk::Entry* m_txt_x32_host;
    Gtk::CheckButton* m_chk_x32_autoconnect;
};

#endif /* OCONNECTDLG_H */

