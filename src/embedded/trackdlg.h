static char trackdlg_inline_glade[] = 
"<?xml version='1.0' encoding='UTF-8'?>"
"<!-- Generated with glade 3.22.2 -->"
"<interface>"
"  <requires lib='gtk+' version='3.20'/>"
"  <object class='GtkDialog' id='track-dlg'>"
"    <property name='can_focus'>False</property>"
"    <property name='type_hint'>dialog</property>"
"    <child type='titlebar'>"
"      <placeholder/>"
"    </child>"
"    <child internal-child='vbox'>"
"      <object class='GtkBox'>"
"        <property name='can_focus'>False</property>"
"        <property name='orientation'>vertical</property>"
"        <property name='spacing'>2</property>"
"        <child internal-child='action_area'>"
"          <object class='GtkButtonBox'>"
"            <property name='can_focus'>False</property>"
"            <property name='layout_style'>end</property>"
"            <child>"
"              <object class='GtkButton' id='btn-cancel'>"
"                <property name='label' translatable='yes'>Cancel</property>"
"                <property name='visible'>True</property>"
"                <property name='can_focus'>True</property>"
"                <property name='receives_default'>True</property>"
"              </object>"
"              <packing>"
"                <property name='expand'>True</property>"
"                <property name='fill'>True</property>"
"                <property name='position'>0</property>"
"              </packing>"
"            </child>"
"            <child>"
"              <object class='GtkButton' id='btn-ok'>"
"                <property name='label' translatable='yes'>Ok</property>"
"                <property name='visible'>True</property>"
"                <property name='can_focus'>True</property>"
"                <property name='receives_default'>True</property>"
"              </object>"
"              <packing>"
"                <property name='expand'>True</property>"
"                <property name='fill'>True</property>"
"                <property name='position'>1</property>"
"              </packing>"
"            </child>"
"          </object>"
"          <packing>"
"            <property name='expand'>False</property>"
"            <property name='fill'>False</property>"
"            <property name='position'>0</property>"
"          </packing>"
"        </child>"
"        <child>"
"          <object class='GtkBox'>"
"            <property name='visible'>True</property>"
"            <property name='can_focus'>False</property>"
"            <property name='orientation'>vertical</property>"
"            <child>"
"              <object class='GtkBox' id='bx-name'>"
"                <property name='visible'>True</property>"
"                <property name='can_focus'>False</property>"
"                <child>"
"                  <object class='GtkLabel'>"
"                    <property name='visible'>True</property>"
"                    <property name='can_focus'>False</property>"
"                    <property name='halign'>start</property>"
"                    <property name='xpad'>8</property>"
"                    <property name='label' translatable='yes'>Track name:</property>"
"                    <property name='ellipsize'>start</property>"
"                  </object>"
"                  <packing>"
"                    <property name='expand'>True</property>"
"                    <property name='fill'>True</property>"
"                    <property name='position'>0</property>"
"                  </packing>"
"                </child>"
"                <child>"
"                  <object class='GtkEntry' id='txt-name'>"
"                    <property name='visible'>True</property>"
"                    <property name='can_focus'>True</property>"
"                  </object>"
"                  <packing>"
"                    <property name='expand'>False</property>"
"                    <property name='fill'>True</property>"
"                    <property name='position'>1</property>"
"                  </packing>"
"                </child>"
"              </object>"
"              <packing>"
"                <property name='expand'>False</property>"
"                <property name='fill'>True</property>"
"                <property name='position'>0</property>"
"              </packing>"
"            </child>"
"            <child>"
"              <object class='GtkBox' id='bx-name1'>"
"                <property name='visible'>True</property>"
"                <property name='can_focus'>False</property>"
"                <child>"
"                  <object class='GtkLabel' id='lbl-path'>"
"                    <property name='visible'>True</property>"
"                    <property name='can_focus'>False</property>"
"                    <property name='halign'>start</property>"
"                    <property name='xpad'>8</property>"
"                    <property name='label' translatable='yes'>Track path:</property>"
"                    <property name='ellipsize'>start</property>"
"                  </object>"
"                  <packing>"
"                    <property name='expand'>True</property>"
"                    <property name='fill'>True</property>"
"                    <property name='position'>0</property>"
"                  </packing>"
"                </child>"
"                <child>"
"                  <object class='GtkEntry' id='txt-path'>"
"                    <property name='visible'>True</property>"
"                    <property name='can_focus'>True</property>"
"                    <property name='editable'>False</property>"
"                  </object>"
"                  <packing>"
"                    <property name='expand'>False</property>"
"                    <property name='fill'>True</property>"
"                    <property name='position'>1</property>"
"                  </packing>"
"                </child>"
"              </object>"
"              <packing>"
"                <property name='expand'>False</property>"
"                <property name='fill'>True</property>"
"                <property name='position'>1</property>"
"              </packing>"
"            </child>"
"            <child>"
"              <object class='GtkBox' id='bx-name2'>"
"                <property name='visible'>True</property>"
"                <property name='can_focus'>False</property>"
"                <child>"
"                  <object class='GtkLabel' id='lbl-path1'>"
"                    <property name='visible'>True</property>"
"                    <property name='can_focus'>False</property>"
"                    <property name='halign'>start</property>"
"                    <property name='xpad'>8</property>"
"                    <property name='label' translatable='yes'>Track color:</property>"
"                    <property name='ellipsize'>start</property>"
"                  </object>"
"                  <packing>"
"                    <property name='expand'>True</property>"
"                    <property name='fill'>True</property>"
"                    <property name='position'>0</property>"
"                  </packing>"
"                </child>"
"                <child>"
"                  <object class='GtkColorButton' id='btn-color'>"
"                    <property name='visible'>True</property>"
"                    <property name='can_focus'>True</property>"
"                    <property name='receives_default'>True</property>"
"                  </object>"
"                  <packing>"
"                    <property name='expand'>False</property>"
"                    <property name='fill'>True</property>"
"                    <property name='position'>1</property>"
"                  </packing>"
"                </child>"
"              </object>"
"              <packing>"
"                <property name='expand'>False</property>"
"                <property name='fill'>True</property>"
"                <property name='position'>2</property>"
"              </packing>"
"            </child>"
"          </object>"
"          <packing>"
"            <property name='expand'>False</property>"
"            <property name='fill'>True</property>"
"            <property name='position'>1</property>"
"          </packing>"
"        </child>"
"      </object>"
"    </child>"
"  </object>"
"</interface>"
;
