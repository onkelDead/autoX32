/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "OMainWnd.h"
#include <regex>

void OMainWnd::OnTrackUpdate(IOTrackStore* ts) {
    if (ts->GetView())
        PublishUiEvent(E_OPERATION::draw_trackview, ((OTrackView*)ts->GetView()));
    else 
        std::cout << "not select" << std::endl;
}

void OMainWnd::OnTrackNew(IOTrackStore* ts) {
    PublishUiEvent(E_OPERATION::new_track, ts);
}
