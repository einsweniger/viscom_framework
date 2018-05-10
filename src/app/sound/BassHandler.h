//
// Created by bone on 08.05.18.
//

#ifndef VISCOMFRAMEWORK_BASSHANDLER_H
#define VISCOMFRAMEWORK_BASSHANDLER_H

#include <experimental/filesystem>
#include <bass.h>
#include <iostream>

namespace minuseins::audio {
    //constexpr float bpm = 150.0f; /* beats per minute */
    constexpr float bpm = 172.0f; /* beats per minute */
    constexpr int rpb = 8; /* rows per beat */
    constexpr double row_rate = (double(bpm) / 60) * rpb;

    constexpr int freq = 44100;
    constexpr int default_device = -1;
    constexpr int channels = 1;
    constexpr int FFT_SIZE = 1024;

    namespace fs = std::experimental::filesystem;
    namespace bass {


    }
    class BassHandler {
        static void print_problem(const std::string& calledFn) {
            fprintf(stderr, "%s failed: %08X\n",calledFn.c_str(), BASS_ErrorGetCode());
        }
    public:
        //last two parameters are for directSound
        BassHandler(int device = default_device, DWORD freq = 44100, DWORD flags = 0);

        void pause();

        void play();

        void set_row(int row);

        double get_row();

        void set_position(double time) {
            QWORD position = BASS_ChannelSeconds2Bytes(output_,time);
            BASS_ChannelSetPosition(output_, position, BASS_POS_BYTE);
        }

        double get_time() {
            auto position = BASS_ChannelGetPosition(output_, BASS_POS_BYTE);
            return BASS_ChannelBytes2Seconds(output_, position);
        }

        std::tuple<QWORD, double> get_length();

        void update(DWORD length = 0);

        bool get_fft( float * samples );

        HRECORD startRecording(DWORD freq = 44100, DWORD channels = 1, DWORD flags = BASS_SAMPLE_8BITS);

        HSTREAM openFile(const fs::path& path, DWORD flags = BASS_STREAM_PRESCAN);

        virtual ~BassHandler();

        DWORD outputDevice_;
        DWORD inputDevice_;
        HRECORD recorder_;
        HSTREAM output_;
    };
}

#endif //VISCOMFRAMEWORK_BASSHANDLER_H
