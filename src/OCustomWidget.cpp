/*
 * OCustomWidget.cpp
 *
 *  Created on: Dec 28, 2020
 *      Author: onkel
 */

#include "OCustomWidget.h"

OCustomWidget::OCustomWidget() {
	// TODO Auto-generated constructor stub

}

OCustomWidget::~OCustomWidget() {
	// TODO Auto-generated destructor stub
}

Gtk::SizeRequestMode OCustomWidget::get_request_mode_vfunc() const {
	return Gtk::Widget::get_request_mode_vfunc();
}

#if 0
void OCustomWidget::get_preferred_width_vfunc(int &minimum_width,
		int &natural_width) const {
	minimum_width = 30;
	natural_width = 30;
}

void OCustomWidget::get_preferred_height_for_width_vfunc(int /* width */,
		int &minimum_height, int &natural_height) const {
	minimum_height = 30;
	natural_height = 30;
}

void OCustomWidget::get_preferred_height_vfunc(int &minimum_height,
		int &natural_height) const {
	minimum_height = 30;
	natural_height = 30;
}

void OCustomWidget::get_preferred_width_for_height_vfunc(int /* height */,
		int &minimum_width, int &natural_width) const {
	minimum_width = 30;
	natural_width = 30;
}
#endif
void OCustomWidget::on_size_allocate(Gtk::Allocation &allocation) {
	set_allocation(allocation);

	if (m_refGdkWindow) {
		m_refGdkWindow->move_resize(allocation.get_x(), allocation.get_y(),
				allocation.get_width(), allocation.get_height());
	}
}

void OCustomWidget::on_map() {
	Gtk::Widget::on_map();
}

void OCustomWidget::on_unmap() {
	Gtk::Widget::on_unmap();
}

void OCustomWidget::on_realize() {
	set_realized();

	if (!m_refGdkWindow) {

		GdkWindowAttr attributes;
		memset(&attributes, 0, sizeof(attributes));

		Gtk::Allocation allocation = get_allocation();

		attributes.x = allocation.get_x();
		attributes.y = allocation.get_y();
		attributes.width = allocation.get_width();
		attributes.height = allocation.get_height();

		attributes.event_mask = get_events() | GDK_EXPOSURE_MASK
				| GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
				| GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK
				| GDK_SCROLL_MASK;
		attributes.window_type = GDK_WINDOW_CHILD;
		attributes.wclass = GDK_INPUT_OUTPUT;

		m_refGdkWindow = Gdk::Window::create(get_parent_window(), &attributes,
				GDK_WA_X | GDK_WA_Y);
		set_window(m_refGdkWindow);

		m_refGdkWindow->set_user_data(gobj());
	}
}

void OCustomWidget::on_unrealize() {
	m_refGdkWindow.reset();

	Gtk::Widget::on_unrealize();
}

