//
// Created by bone on 08.05.18.
//

#ifndef VISCOMFRAMEWORK_BASSHANDLER_H
#define VISCOMFRAMEWORK_BASSHANDLER_H

#include <experimental/filesystem>
#include <bass.h>
#include <iostream>
#include <enh/gfx/gl/GLTexture.h>

namespace minuseins::audio {
    //constexpr float bpm = 150.0f; /* beats per minute */
    constexpr float bpm = 172.0f; /* beats per minute */
    constexpr int rpb = 8; /* rows per beat */
    constexpr double row_rate = (double(bpm) / 60) * rpb;

    constexpr int default_device = -1;
    constexpr int channels = 1;
    constexpr int FFT_SIZE = 1024;

    namespace fs = std::experimental::filesystem;

    constexpr size_t img_height=128;
    constexpr float samples_per_second = 100.f;
    struct BassDecoder {
        BassDecoder(unsigned int stream);
        const unsigned int stream;
        const unsigned long sample_len;
        const size_t sample_count;
        const std::array<unsigned int, 256*3> palette;
        const std::array<int, img_height+2> logLookup;

        const float maxIntensity = 500 * 2;
        const float intensity_step = (palette.size() - 1) / std::log(maxIntensity + 1);
        size_t current_idx = 0;
        size_t img_y=0;

        std::vector<uint32_t> result = std::vector<uint32_t>(img_height*sample_count);
        std::vector<uint32_t>::iterator fftOutput = result.begin();
        std::array<float, FFT_SIZE> fftData = std::array<float, FFT_SIZE>{};
        std::vector<std::unique_ptr<viscom::enh::GLTexture>> textures;
        void step();
        size_t total_rows();
        bool wrote_textures=false;
    };

    class BassHandler {
        static void print_problem(const std::string& calledFn) {
            fprintf(stderr, "%s failed: %02d\n",calledFn.c_str(), BASS_ErrorGetCode());
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
