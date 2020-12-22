/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ODlgPrefs.h
 * Author: onkel
 *
 * Created on December 22, 2020, 10:03 AM
 */

#ifndef ODLGPREFS_H
#define ODLGPREFS_H

#include <gtkmm.h>

class ODlgPrefs : public Gtk::Dialog {
public:
    ODlgPrefs(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~ODlgPrefs();
    
    void on_btn_cancel_clicked();
    void on_btn_ok_clicked();
    
    void SetShowTrackPath(bool);
    bool GetShowTrackPath();
    
    bool m_result;

protected:
    Glib::RefPtr<Gtk::CssProvider> m_refCssProvider;    
    Glib::RefPtr<Gtk::Builder> ui;
    
private:
    Gtk::Button* m_btn_cancel;
    Gtk::Button* m_btn_ok;
    
    Gtk::CheckButton* m_chk_showtrackpath;

};

#endif /* ODLGPREFS_H */

