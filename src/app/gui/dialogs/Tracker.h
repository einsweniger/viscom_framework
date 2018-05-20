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
        std::unique_ptr<viscom::enh::GLTexture> tex;
        std::unique_ptr<audio::BassDecoder> decoder;
        void draw_table();

        void draw_table_content();

        void draw_value_selectable(tracker::Track& track, tracker::Key& key, size_t active_row);

        void draw_empty_selectable(tracker::Track track, size_t row, tracker::interpolation_type type);
    };
}

#endif //VISCOMFRAMEWORK_TRACKER_H
