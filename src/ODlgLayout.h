/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ODlgLayout.h
 * Author: onkel
 *
 * Created on December 4, 2021, 11:58 AM
 */

#ifndef ODLGLAYOUT_H
#define ODLGLAYOUT_H

#include <gtkmm.h>
#include <gtkmm/dialog.h>
#include "IOTracksLayout.h"
#include "IOTrackView.h"


class ODlgLayoutTrack : public Gtk::Box {
public:
    ODlgLayoutTrack(IOTracksLayout* parent, IOTrackView* view);    
    virtual ~ODlgLayoutTrack();
    
    void SetCollapsed(bool colapse);
    
protected:
    Glib::RefPtr<Gtk::CssProvider> m_refCssProvider;    
    Glib::RefPtr<Gtk::Builder> ui;  
    
private:
    Gtk::Label* m_label;
    Gtk::Label* m_name;
    Gtk::CheckButton* m_check;
    Gtk::CheckButton* m_expand;
};

class ODlgLayout : public Gtk::Dialog {
public:
    ODlgLayout(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~ODlgLayout();

    void on_btn_ok_clicked();
    void on_btn_collapse();
    void on_btn_expand();
    
    void AddTrack(IOTracksLayout* parent, IOTrackView* view);
    
    bool m_result;
    
protected:
    Glib::RefPtr<Gtk::CssProvider> m_refCssProvider;    
    Glib::RefPtr<Gtk::Builder> ui;
    
private:
    Gtk::Button* m_btn_ok;    
    Gtk::Button* m_btn_collapse;    
    Gtk::Button* m_btn_expand;    
    Gtk::Box* m_view;
    
    std::map<std::string, ODlgLayoutTrack*> m_expanders;

};


#endif /* ODLGLAYOUT_H */

