/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OMainWndProcessBacckend.cpp
 * Author: onkel
 *
 * Created on 13. März 2022, 17:01
 */

#include "OMainWnd.h"

void OMainWnd::OnLocate(bool partial) {
    if (!partial) {
        PublishUiEvent(E_OPERATION::new_pos, NULL);
    }
    else {
        PublishUiEvent(E_OPERATION::pos_next, NULL);
    }
}

void OMainWnd::OnPlay() {
    PublishUiEvent(E_OPERATION::play, NULL);
}

void OMainWnd::OnStop() {
    PublishUiEvent(E_OPERATION::stop, NULL);
}

void OMainWnd::OnTeach(bool val) {
    PublishUiEvent(E_OPERATION::teach, NULL);
}

void OMainWnd::OnUnselectTrack() {
    PublishUiEvent(E_OPERATION::unselect_track, NULL);
}

void OMainWnd::OnSelectTrack() {
    PublishUiEvent(E_OPERATION::select_track, NULL);
}

void OMainWnd::OnTrackRec() {
    PublishUiEvent(E_OPERATION::toggle_rec, NULL);
}

void OMainWnd::OnMarkerStart() {
    PublishUiEvent(E_OPERATION::marker_start, NULL);
}

void OMainWnd::OnMarkerEnd() {
    PublishUiEvent(E_OPERATION::marker_end, NULL);
}

void OMainWnd::OnDropTrack() {
    PublishUiEvent(E_OPERATION::drop_track, NULL);
}

void OMainWnd::OnCenterThin() {
    on_btn_zoom_loop_clicked();
}

void OMainWnd::OnTrackUpdate(IOTrackStore* ts) {
    if (ts->GetView())
        PublishUiEvent(E_OPERATION::draw_trackview, ((OTrackView*)ts->GetView()));
}

void OMainWnd::OnTrackNew(IOTrackStore* ts) {
    PublishUiEvent(E_OPERATION::new_track, ts);
}
