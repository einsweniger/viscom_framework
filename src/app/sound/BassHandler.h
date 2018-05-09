//
// Created by bone on 08.05.18.
//

#ifndef VISCOMFRAMEWORK_BASSHANDLER_H
#define VISCOMFRAMEWORK_BASSHANDLER_H

#include <experimental/filesystem>
#include <iostream>

namespace minuseins::audio {
    constexpr float bpm = 150.0f; /* beats per minute */
    constexpr int rpb = 8; /* rows per beat */
    constexpr double row_rate = (double(bpm) / 60) * rpb;

    constexpr int freq = 44100;
    constexpr int default_device = -1;
    constexpr int channels = 1;
    constexpr int FFT_SIZE = 1024;

    namespace fs = std::experimental::filesystem;
    namespace bass {
#include <bass.h>

    }
    class BassHandler {
        static void print_problem(const std::string& calledFn) {
            fprintf(stderr, "%s failed: %08X\n",calledFn.c_str(), bass::BASS_ErrorGetCode());
        }
    public:
        //last two parameters are for directSound
        BassHandler(int device = default_device, bass::DWORD freq = 44100, bass::DWORD flags = 0);

        void pause();

        void play();

        void set_row(int row);

        double get_row();

        void set_position(double time) {
            bass::QWORD position = bass::BASS_ChannelSeconds2Bytes(output_,time);
            bass::BASS_ChannelSetPosition(output_, position, BASS_POS_BYTE);
        }

        double get_position() {
            auto position = bass::BASS_ChannelGetPosition(output_, BASS_POS_BYTE);
            return bass::BASS_ChannelSeconds2Bytes(output_, position);
        }

        std::tuple<bass::QWORD, double> get_length();

        void update(bass::DWORD length = 0);

        bool get_fft( float * samples );

        bass::HRECORD startRecording(bass::DWORD freq = 44100, bass::DWORD channels = 1, bass::DWORD flags = BASS_SAMPLE_8BITS);

        bass::HSTREAM openFile(const fs::path& path, bass::DWORD flags = BASS_STREAM_PRESCAN);

        virtual ~BassHandler();

        bass::DWORD outputDevice_;
        bass::DWORD inputDevice_;
        bass::HRECORD recorder_;
        bass::HSTREAM output_;
    };
}

#endif //VISCOMFRAMEWORK_BASSHANDLER_H
