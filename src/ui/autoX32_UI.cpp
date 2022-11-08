/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <gtkmm.h>
#include <X11/Xlib.h>   
#include "OMainWnd.h"
#include "OThinWnd.h"


int main_ui(int argc, char** argv) {
    XInitThreads();
    auto app = Gtk::Application::create(argc, argv);

    OMainWnd window;

    app->signal_activate().connect(sigc::mem_fun(window, &OMainWnd::on_activate));
    
    return app->run((Gtk::Window&)window);
}

int main_thin(int argc, char** argv) {
    XInitThreads();
    auto app = Gtk::Application::create();

    OThinWnd window;
    
    app->signal_activate().connect(sigc::mem_fun(window, &OThinWnd::on_activate));

    return app->run((Gtk::Window&)window);    
}