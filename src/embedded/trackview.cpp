char trackview_inline_glade[] = 
"<?xml version='1.0' encoding='UTF-8'?>"
"<!-- Generated with glade 3.39.0 -->"
"<interface>"
"  <requires lib='gtk+' version='3.20'/>"
"  <object class='GtkBox' id='track-box'>"
"    <property name='visible'>True</property>"
"    <property name='can-focus'>False</property>"
"    <property name='hexpand'>True</property>"
"    <property name='vexpand'>True</property>"
"    <child>"
"      <object class='GtkExpander' id='track-expander'>"
"        <property name='name'>track-expander</property>"
"        <property name='width-request'>160</property>"
"        <property name='visible'>True</property>"
"        <property name='can-focus'>True</property>"
"        <property name='tooltip-text' translatable='yes'>Expand / Collapse track</property>"
"        <child>"
"          <object class='GtkBox' id='track-control'>"
"            <property name='name'>track_control</property>"
"            <property name='visible'>True</property>"
"            <property name='can-focus'>False</property>"
"            <property name='vexpand'>True</property>"
"            <property name='orientation'>vertical</property>"
"            <property name='baseline-position'>bottom</property>"
"            <child>"
"              <object class='GtkToolbar' id='track-bar'>"
"                <property name='name'>OTrackToolbar</property>"
"                <property name='visible'>True</property>"
"                <property name='can-focus'>False</property>"
"                <property name='halign'>start</property>"
"                <property name='valign'>start</property>"
"                <property name='toolbar-style'>both-horiz</property>"
"                <property name='icon_size'>2</property>"
"                <child>"
"                  <object class='GtkToggleToolButton' id='track-rec'>"
"                    <property name='name'>TrackRecordButton</property>"
"                    <property name='visible'>True</property>"
"                    <property name='can-focus'>False</property>"
"                    <property name='tooltip-text' translatable='yes'>Enable Record</property>"
"                    <property name='valign'>start</property>"
"                    <property name='icon-name'>media-record</property>"
"                  </object>"
"                  <packing>"
"                    <property name='expand'>False</property>"
"                    <property name='homogeneous'>False</property>"
"                  </packing>"
"                </child>"
"                <child>"
"                  <object class='GtkToolButton' id='track-edit'>"
"                    <property name='visible'>True</property>"
"                    <property name='can-focus'>False</property>"
"                    <property name='tooltip-text' translatable='yes'>Edit track</property>"
"                    <property name='label' translatable='yes'>toolbutton</property>"
"                    <property name='icon-name'>mail-message-new</property>"
"                  </object>"
"                  <packing>"
"                    <property name='expand'>False</property>"
"                    <property name='homogeneous'>True</property>"
"                  </packing>"
"                </child>"
"                <child>"
"                  <object class='GtkToolButton' id='track-up'>"
"                    <property name='name'>track-up</property>"
"                    <property name='visible'>True</property>"
"                    <property name='can-focus'>False</property>"
"                    <property name='tooltip-text' translatable='yes'>Move track up</property>"
"                    <property name='label' translatable='yes'>toolbutton</property>"
"                    <property name='icon-name'>go-up</property>"
"                  </object>"
"                  <packing>"
"                    <property name='expand'>False</property>"
"                    <property name='homogeneous'>True</property>"
"                  </packing>"
"                </child>"
"                <child>"
"                  <object class='GtkToolButton' id='track-down'>"
"                    <property name='visible'>True</property>"
"                    <property name='can-focus'>False</property>"
"                    <property name='tooltip-text' translatable='yes'>Move track down</property>"
"                    <property name='label' translatable='yes'>toolbutton</property>"
"                    <property name='icon-name'>go-down</property>"
"                  </object>"
"                  <packing>"
"                    <property name='expand'>False</property>"
"                    <property name='homogeneous'>True</property>"
"                  </packing>"
"                </child>"
"              </object>"
"              <packing>"
"                <property name='expand'>True</property>"
"                <property name='fill'>True</property>"
"                <property name='position'>0</property>"
"              </packing>"
"            </child>"
"            <child>"
"              <object class='GtkBox' id='box-sizer'>"
"                <property name='visible'>True</property>"
"                <property name='can-focus'>False</property>"
"                <property name='orientation'>vertical</property>"
"                <child>"
"                  <placeholder/>"
"                </child>"
"              </object>"
"              <packing>"
"                <property name='expand'>False</property>"
"                <property name='fill'>True</property>"
"                <property name='position'>1</property>"
"              </packing>"
"            </child>"
"          </object>"
"        </child>"
"        <child type='label'>"
"          <object class='GtkLabel' id='track-label'>"
"            <property name='visible'>True</property>"
"            <property name='can-focus'>False</property>"
"            <property name='label' translatable='yes'>expander</property>"
"          </object>"
"        </child>"
"      </object>"
"      <packing>"
"        <property name='expand'>False</property>"
"        <property name='fill'>True</property>"
"        <property name='position'>0</property>"
"      </packing>"
"    </child>"
"    <child>"
"      <placeholder/>"
"    </child>"
"  </object>"
"</interface>"
;
