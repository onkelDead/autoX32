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

#include <gtkmm.h>
#include "OMainWnd.h"
#include "ODlgProlog.h"
#include "embedded/main.h"

 int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv, AUTOX32_SCHEMA_ID);

    int midi_backend = 0;
    
    OMainWnd* window;

    Glib::RefPtr<Gio::Settings> settings;    
    GSettingsSchemaSource *source = g_settings_schema_source_get_default();
   
    GSettingsSchema *schema = g_settings_schema_source_lookup(source, AUTOX32_SCHEMA_ID, true);
    if (schema) {
        settings = Gio::Settings::create(AUTOX32_SCHEMA_ID);
        midi_backend = settings->get_int(SETTINGS_MIDI_BACKEND);
    }
    
    g_settings_schema_unref(schema);    
    
    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_string(main_inline_glade);    
    ODlgProlog *pDialog = nullptr;
    builder->get_widget_derived("dlg-prolog", pDialog);
    
    pDialog->SetMidiBackend(midi_backend);
    
    pDialog->run();
    if (!pDialog->GetResult()) {
        return 0;
    }
    
    midi_backend = pDialog->GetMidiBackend();
    
    window = new OMainWnd();
    
    if (window->SetupBackend(midi_backend)) {
        Gtk::MessageDialog dialog(*window, "Failed to start backend.",
                false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
        dialog.run();   
        delete window;
        return 1;
    }

    window->AutoConnect();
    
    
    int result = app->run((Gtk::Window&)*window);
    delete window;
    return result;
}
