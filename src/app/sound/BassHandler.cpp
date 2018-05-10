//
// Created by bone on 08.05.18.
//

#include "BassHandler.h"
namespace minuseins::audio {
    BassHandler::BassHandler(int device, DWORD freq,
                                               DWORD flags) {
        if(!BASS_Init(device, freq, flags, 0,0)) {
            print_problem("BASS_Init");
        }
        outputDevice_ = BASS_GetDevice();
        if(-1 == outputDevice_) {
            print_problem("BASS_GetDevice");
        }
        if( !BASS_RecordInit( device ) )
        {
            print_problem("BASS_RecordInit");
        }
        inputDevice_ = BASS_RecordGetDevice();
        if(-1 == inputDevice_) {
            print_problem("BASS_RecordGetDevice");
        }
    }

    void BassHandler::pause() {
        BASS_ChannelPause(output_);
    }

    void BassHandler::play() {
        BASS_ChannelPlay(output_, false);
    }

    void BassHandler::set_row(int row) {
        QWORD position = BASS_ChannelSeconds2Bytes(output_, row / row_rate);
        BASS_ChannelSetPosition(output_, position, BASS_POS_BYTE);
    }

    double BassHandler::get_row() {
        QWORD pos = BASS_ChannelGetPosition(output_, BASS_POS_BYTE);
        double time = BASS_ChannelBytes2Seconds(output_, pos);
        return time * row_rate;
    }

    std::tuple<QWORD, double> BassHandler::get_length() {
        QWORD len=BASS_ChannelGetLength(output_, BASS_POS_BYTE); // the length in bytes
        double time=BASS_ChannelBytes2Seconds(output_, len); // the length in seconds
        return {len, time};
    }
    void BassHandler::update(DWORD length) {
        BASS_Update(length);
    }

    bool BassHandler::get_fft(float *samples) {
        if (!output_)
            return false;

        unsigned int len = 0;

        switch( FFT_SIZE*2 ) // for 256 fft, only 128 values will contain DC in our case
        {
            case 256:
                len = BASS_DATA_FFT256;
                break;
            case 512:
                len = BASS_DATA_FFT512;
                break;
            case 1024:
                len = BASS_DATA_FFT1024;
                break;
            case 2048:
                len = BASS_DATA_FFT2048;
                break;
            case 4096:
                len = BASS_DATA_FFT4096;
                break;
            case 8192:
                len = BASS_DATA_FFT8192;
                break;
            case 16384:
                len = BASS_DATA_FFT16384;
                break;
            default:
                //fprintf( stderr, "BASS invalid fft window size\n" );
                break;
        }
        //first parameter can be a HCHANNEL, HMUSIC, HSTREAM, or HRECORD
        const int numBytes = BASS_ChannelGetData( output_, samples, len | BASS_DATA_FFT_REMOVEDC );
        if( numBytes <= 0 )
            return false;

        return true;
    }

    HRECORD BassHandler::startRecording(DWORD freq,
                                                                                  DWORD channels,
                                                                                  DWORD flags) {
        recorder_ = BASS_RecordStart( freq, channels, flags, nullptr, nullptr );
        if (!recorder_) {
            print_problem("BASS_RecordStart");
            return 0;
        }
        return recorder_;
    }

    HSTREAM BassHandler::openFile(const std::experimental::filesystem::path &path,
                                                                            DWORD flags) {
        output_ = BASS_StreamCreateFile(false, path.c_str(), 0, 0, flags);
        if (!output_) {
            print_problem("BASS_StreamCreateFile");
            return 0;
        }

        return output_;

    }

    BassHandler::~BassHandler() {
        if(outputDevice_) {
            if(output_) {
                BASS_StreamFree(output_);
                output_ = 0;
            }
        }

        if(inputDevice_) {
            if(recorder_) {
                BASS_ChannelStop(recorder_);
                recorder_ = 0;
            }
            BASS_RecordFree();
        }
    }
    
}
