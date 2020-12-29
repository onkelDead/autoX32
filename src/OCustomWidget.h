/*
 * OCustomWidget.h
 *
 *  Created on: Dec 28, 2020
 *      Author: onkel
 */

#ifndef SRC_OCUSTOMWIDGET_H_
#define SRC_OCUSTOMWIDGET_H_

#include <gtkmm/widget.h>

class OCustomWidget: public Gtk::Widget {
public:
	OCustomWidget();
	virtual ~OCustomWidget();

protected:

    //Overrides:
    Gtk::SizeRequestMode get_request_mode_vfunc() const override;
//    void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const override;
//    void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const override;
//    void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const override;
//    void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const override;
    virtual void on_size_allocate(Gtk::Allocation& allocation) override;
    virtual void on_map() override;
    virtual void on_unmap() override;
    virtual void on_realize() override;
    virtual void on_unrealize() override;


    Glib::RefPtr<Gdk::Window> m_refGdkWindow;

};

#endif /* SRC_OCUSTOMWIDGET_H_ */
