includedir = $(prefix)/include
datadir = $(prefix)/share/autoX32/
desktopdir       = $(prefix)/share/applications
pixmapsdir       = $(prefix)/share/pixmaps
autoX32kdir = src
bin_PROGRAMS = autoX32 check_TrackStore check_OscCmd

autoX32_SOURCES = \
    src/ui/OMainWnd.cpp \
    src/ui/OThinWnd.cpp \
    src/ui/OMainWndUI.cpp \
    src/ui/OMainWndEvents.cpp \
    src/ui/OMainWndProcess.cpp \
    src/ui/OMainWndProcessBackend.cpp \
    src/ui/OTimeView.cpp \
    src/ui/OTimeDraw.cpp \
    src/ui/OTracksLayout.cpp \
    src/ui/OTrackView.cpp \
    src/ui/OTrackDrawBase.cpp \
    src/ui/OTrackDraw.cpp \
    src/ui/OTrackDrawThin.cpp \
    src/ui/OOverView.cpp \
    src/ui/OTrackSizer.cpp \
    src/ui/OCustomWidget.cpp \
    src/ui/OPlayHead.cpp \
    src/dlg/OTrackDlg.cpp \
    src/dlg/OConnectDlg.cpp \
    src/dlg/ODlgPrefs.cpp \
    src/OTrackStore.cpp \
    src/OProject.cpp \
    src/OService.cpp \
    src/OEngine.cpp \
    src/backend/OscMessage.cpp \
    src/backend/OscValue.cpp \
    src/backend/OscCache.cpp \
    src/backend/ODAW.cpp \
    src/backend/OX32.cpp \
    src/backend/OCallbackHandler.cpp \
    src/autoX32.cpp \
    src/autoX32_service.cpp \
    src/ui/autoX32_UI.cpp \
    src/backend/OJack.cpp \
    src/backend/OAlsa.cpp \
    src/backend/OBackend.cpp \
    src/backend/OMidiMtc.cpp \
    src/dlg/ODlgLayout.cpp \
    src/dlg/ODlgProlog.cpp \
    src/res/autoX32_css.cpp \
    src/embedded/connectDlg.cpp \
    src/embedded/main.cpp \
    src/embedded/timeview.cpp \
    src/embedded/trackview.cpp \
    src/embedded/trackdlg.cpp \
    src/utils/OTimer.cpp \
    src/utils/OQueue.cpp \
    src/utils/OConfig.cpp

noinst_HEADERS = \
    src/ui/OMainWnd.h \
    src/ui/OOverView.h \
    src/ui/OTracksLayout.h \
    src/ui/OTrackView.h \
    src/ui/OTimeDraw.h \
    src/ui/OTrackDraw.h \
    src/ui/OOverView.h \
    src/ui/OTrackSizer.h \
    src/ui/OTimeView.h \
    src/ui/OCustomWidget.h \
    src/ui/OPlayHead.h \
    src/OTrackStore.h \
    src/backend/ODAW.h \
    src/OProject.h \
    src/OEngine.h \
    src/dlg/OTrackDlg.h \
    src/dlg/OConnectDlg.h \
    src/dlg/ODlgPrefs.h \
    src/backend/OX32.h \
    src/backend/OJack.h \
    src/backend/OAlsa.h \
    src/backend/OMidiMtc.h \
    src/dlg/ODlgLayout.h \
    src/dlg/ODlgProlog.h \
    src/utils/OQueue.h \
    src/utils/OTimer.h \
    src/utils/OConfig.h

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

    
autoX32_CPPFLAGS = -std=c++2a -Wno-deprecated-enum-enum-conversion -Isrc -Isrc/backend -Isrc/ui -Isrc/dlg -Isrc/utils ${GTK3_CFLAGS} ${GTKMM3_CFLAGS} ${XML2_CFLAGS} ${LO_CFLAGS} ${JACK} ${ALSA_CFLAGS} ${CONFIG_CFLAGS} -DPKGDATADIR='"$(datadir)"'
autoX32_LDADD = ${GTKMM3_LIBS} ${XML2_LIBS} ${LO_LIBS} ${JACK} ${ALSA_LIBS} ${CONFIG_LIBS} -ljack -lpthread -lX11

TESTS = check_TrackStore check_OscCmd
check_PROGRAMS = check_TrackStore check_OscCmd

check_TrackStore_SOURCES = tests/test_trackstore.cpp 
check_TrackStore_CPPFLAGS = -g -O0 -std=c++2a -Isrc -Isrc/backend ${CHECK_FLAGS} ${GTK3_CFLAGS} ${GTKMM3_CFLAGS} ${XML2_CFLAGS}
check_TrackStore_LDADD = src/autoX32-OTrackStore.o src/backend/autoX32-OscMessage.o src/backend/autoX32-OscValue.o src/backend/autoX32-OscCache.o ${GTKMM3_LIBS} ${XML2_LIBS}

check_OscCmd_SOURCES = tests/test_OscCmd.cpp
check_OscCmd_CPPFLAGS = -g -O0 -std=c++2a -Isrc -Isrc/ui -Isrc/utils -Isrc/backend \
    src/backend/autoX32-OscMessage.o \
    src/backend/autoX32-OscValue.o \
    ${CHECK_FLAGS} ${GTK3_CFLAGS} ${GTKMM3_CFLAGS} ${XML2_CFLAGS} ${JACK} ${CONFIG_CFLAGS}

check_OscCmd_LDADD = \
    src/backend/autoX32-OscMessage.o \
    src/backend/autoX32-OscValue.o \
    src/backend/autoX32-OscCache.o \
    src/backend/autoX32-OCallbackHandler.o \
    src/backend/autoX32-OX32.o \
    src/backend/autoX32-ODAW.o \
    src/backend/autoX32-OJack.o \
    src/backend/autoX32-OBackend.o \
    src/utils/autoX32-OConfig.o \
    src/utils/autoX32-OQueue.o \
    src/backend/autoX32-OMidiMtc.o \
    src/utils/autoX32-OTimer.o \
    src/autoX32-OProject.o \
    src/autoX32-OTrackStore.o \
    ${GTKMM3_LIBS} ${LO_LIBS} ${XML2_LIBS} ${CONFIG_LIBS} -lpthread -ljack