//
// Created by bone on 25.04.18.
//

#ifndef VISCOMFRAMEWORK_OVERLAY_H
#define VISCOMFRAMEWORK_OVERLAY_H

namespace minuseins::gui {
    struct Overlay {
        static const size_t data_size = 90;
        int histIdx = 0;
        float histData[data_size] = { 0 };
        float DISTANCE = 10.0f;
        int corner = 0;

        void drawOverlay(bool* p_open);
        void UpdateFrame(double currentTime, double elapsedTime);
    };
}


#endif //VISCOMFRAMEWORK_OVERLAY_H
