includedir = $(prefix)/include
datadir = $(prefix)/share/autoX32/
desktopdir       = $(prefix)/share/applications
pixmapsdir       = $(prefix)/share/pixmaps
autoX32kdir = src
bin_PROGRAMS = autoX32

autoX32_SOURCES = \
    src/OMainWndEvents.cpp \
    src/OMainWnd.cpp \
    src/OMainWndUI.cpp \
    src/OMainWndProcess.cpp \
    src/OOverView.cpp \
    src/OscCmd.cpp \
    src/OTimeView.cpp \
    src/OTracksLayout.cpp \
    src/OTrackEditDlg.cpp \
    src/OTrackDlg.cpp \
    src/OConnectDlg.cpp \
    src/OTrackView.cpp \
    src/OTrackStore.cpp \
    src/OTimeDraw.cpp \
    src/OTrackDraw.cpp \
    src/OProject.cpp \
    src/ODAW.cpp \
    src/OX32.cpp \
    src/OTimer.cpp \
    src/autoX32.cpp

noinst_HEADERS = \
    src/OTimeDraw.h \
    src/OMainWnd.h \
    src/OOverView.h \
    src/OTrackDraw.h \
    src/OTimeView.h \
    src/OTrackStore.h \
    src/ODAW.h \
    src/OscCmd.h \
    src/OProject.h \
    src/OTrackEditDlg.h \
    src/OTrackDlg.h \
    src/OConnectDlg.h \
    src/OX32.h \
    src/OTimer.h

if DEBUG
    AM_CFLAGS = -g3 -O0
    AM_CXXFLAGS = -g3 -O0
else
    AM_CFLAGS = -O2
    AM_CXXFLAGS = -O2
endif

data_DATA = data/media/autoX32.png

dist_data_DATA = data/media/autoX32.png


desktop_DATA = data/autoX32.desktop
pixmaps_DATA     = data/media/autoX32.png
    
autoX32_CPPFLAGS = -std=c++11 ${GTK3_CFLAGS} ${GTKMM3_CFLAGS} ${XML2_CFLAGS} ${LO_CFLAGS} -DPKGDATADIR='"$(datadir)"' 
autoX32_LDADD = ${GTKMM3_LIBS} ${XML2_LIBS} ${LO_LIBS} -lpthread

gsettings_SCHEMAS = de.paraair.autoX32.gschema.xml
@GSETTINGS_RULES@

EXTRA_DIST = $(gsettings_SCHEMAS) \
	     $(pixmaps_DATA) \
	     $(desktop_DATA)

