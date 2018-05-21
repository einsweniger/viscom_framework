//
// Created by bone on 20.05.18.
//

#ifndef VISCOMFRAMEWORK_TRACKER_H
#define VISCOMFRAMEWORK_TRACKER_H

namespace viscom {
    class ApplicationNodeImplementation;
}

namespace minuseins::gui {

    struct Tracker {
        Tracker(viscom::ApplicationNodeImplementation *appImpl);

        bool Draw(bool* p_open);
        viscom::ApplicationNodeImplementation* appImpl;
        unsigned int row_count = 7000;
        bool track_active_row = true;
        float texture_height = 1024;
        std::unique_ptr<audio::BassDecoder> decoder;
        void draw_table();

        void draw_table_content();

    };
}

#endif //VISCOMFRAMEWORK_TRACKER_H
