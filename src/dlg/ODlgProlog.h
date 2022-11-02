/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OCldProlog.h
 * Author: onkel
 *
 * Created on December 10, 2021, 10:23 AM
 */

#ifndef OCLDPROLOG_H
#define OCLDPROLOG_H

#include <gtkmm.h>
#include <gtkmm/dialog.h>
#include "res/autoX32_css.h"


class ODlgProlog : public Gtk::Dialog {
public:
    ODlgProlog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~ODlgProlog();
    
    void SetMidiBackend(int val) {
        m_combo_backend->set_active(val);
    }
    
    int GetMidiBackend() { 
        return m_combo_backend->get_active_row_number();
    }
    
    bool GetResult() { return m_result; }
    
    void SetLoadCache(bool val) {
        m_chk_load_cache->set_active(val);
    }
    
    bool GetLoadCache() {
        return m_chk_load_cache->get_active();
    }
    
private:
    
    void on_btn_quit();
    void on_btn_start();
    
    Glib::RefPtr<Gtk::CssProvider> m_refCssProvider;    
    Gtk::Button* m_btn_quit;    
    Gtk::Button* m_btn_start;
    Gtk::ComboBoxText* m_combo_backend;
    Gtk::CheckButton* m_chk_load_cache;

    bool m_result = 0;
    
};

#endif /* OCLDPROLOG_H */

