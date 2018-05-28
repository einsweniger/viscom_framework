//
// Created by bone on 08.05.18.
//

#include "BassHandler.h"

#include <cmath>

#include <bass.h>
#include <enh/gfx/gl/GLTexture.h>

#include "app/util.h"


namespace minuseins::audio {
BassHandler::BassHandler(int device, DWORD freq, DWORD flags) {
  if (!BASS_Init(device, freq, flags, 0, 0)) {
    print_problem("BASS_Init");
  }
  outputDevice_ = BASS_GetDevice();
  if (static_cast<DWORD>(-1) == outputDevice_) {
    print_problem("BASS_GetDevice");
  }
  if (!BASS_RecordInit(device)) {
    print_problem("BASS_RecordInit");
  }
  inputDevice_ = BASS_RecordGetDevice();
  if (static_cast<DWORD>(-1) == inputDevice_) {
    print_problem("BASS_RecordGetDevice");
  }
}

void BassHandler::pause() {
  if (!BASS_ChannelPause(output_)) {
    print_problem("Bass_ChannelPause");
  }
}

void BassHandler::play() { BASS_ChannelPlay(output_, false); }

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
  QWORD len =
      BASS_ChannelGetLength(output_, BASS_POS_BYTE);  // the length in bytes
  double time =
      BASS_ChannelBytes2Seconds(output_, len);  // the length in seconds
  return {len, time};
}
void BassHandler::update(DWORD length) {
  if (!BASS_Update(length)) {
    // print_problem("BASS_Update");  //
  }
}

bool BassHandler::get_fft(float *samples) {
  if (!output_) return false;

  unsigned int len = 0;

  switch (FFT_SIZE *
          2)  // for 256 fft, only 128 values will contain DC in our case
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
      // fprintf( stderr, "BASS invalid fft window size\n" );
      break;
  }
  // first parameter can be a HCHANNEL, HMUSIC, HSTREAM, or HRECORD
  const DWORD numBytes =
      BASS_ChannelGetData(output_, samples, len | BASS_DATA_FFT_REMOVEDC);
  if (static_cast<DWORD>(-1) == numBytes) {
    print_problem("get_fft::BASS_ChannelGetData");
    return false;
  }
  return true;
}

HRECORD BassHandler::startRecording(DWORD freq, DWORD channels, DWORD flags) {
  recorder_ = BASS_RecordStart(freq, channels, flags, nullptr, nullptr);
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
  if (outputDevice_) {
    if (output_) {
      BASS_StreamFree(output_);
      output_ = 0;
    }
  }

  if (inputDevice_) {
    if (recorder_) {
      BASS_ChannelStop(recorder_);
      recorder_ = 0;
    }
    BASS_RecordFree();
  }
}

void BassHandler::set_position(double time) {
  QWORD position = BASS_ChannelSeconds2Bytes(output_, time);
  BASS_ChannelSetPosition(output_, position, BASS_POS_BYTE);
}

double BassHandler::get_time() {
  auto position = BASS_ChannelGetPosition(output_, BASS_POS_BYTE);
  return BASS_ChannelBytes2Seconds(output_, position);
}

void BassHandler::print_problem(const std::string &calledFn) {
  fprintf(stderr, "%s failed: %02d\n", calledFn.c_str(), BASS_ErrorGetCode());
}

constexpr uint32_t color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return r | (g << 8) | (b << 16) | (a << 24);
}

constexpr std::array<unsigned int, 256 * 3> createPalette() {
  std::array<unsigned int, 256 * 3> colors{};
  const uint8_t offset = 128;
  for (unsigned int i = 0; i < 256; i++) {
    uint8_t idx = static_cast<uint8_t>(i);
    colors[255 * 0 + idx] = color32(0, 0, idx, (idx >> 1));  // 0,0,1,1..127,127
    colors[255 * 1 + idx] =
        color32(idx, 0, idx, (idx >> 1) | offset);  // 128,128..255,255
    colors[255 * 2 + idx] = color32(255, idx, 0, 255);
  }
  return colors;
}

static std::array<int, img_height + 2> createLogLookup() {
  const float stepsize = FFT_SIZE / std::log((float)(img_height + 1));
  std::array<int, img_height + 2> logTab{};
  for (size_t i = 0; i < img_height; ++i) {
    logTab[1 + img_height - i] =
        static_cast<int>(stepsize * std::log((float)img_height - i + 1));
  }
  logTab[0] = logTab[1];
  logTab[img_height + 1] = logTab[img_height];
  return logTab;
}

BassDecoder::BassDecoder(unsigned int stream)
    : stream(stream),
      sample_len{BASS_ChannelSeconds2Bytes(stream, 1.0 / samples_per_second)},
      sample_count{
          (size_t)((double)BASS_ChannelGetLength(stream, BASS_POS_BYTE) /
                   (double)sample_len)},
      palette(createPalette()),
      logLookup(createLogLookup()) {
  BASS_ChannelPlay(stream, false);
  BASS_ChannelSetPosition(stream, current_idx * sample_len, BASS_POS_BYTE);
  BASS_ChannelGetData(stream, &fftData[0], BASS_DATA_FFT2048);
  auto texDescr = viscom::enh::TextureDescriptor{
      sizeof(uint32_t), gl::GL_RGBA8, gl::GL_RGBA, gl::GL_UNSIGNED_BYTE};
  auto texCount = sample_count / FFT_SIZE;
  for (size_t i = 0; i < texCount + 2; i++) {
    textures.push_back(std::make_unique<viscom::enh::GLTexture>(
        img_height, FFT_SIZE, texDescr, nullptr));
  }
  result = std::vector<uint32_t>(img_height * FFT_SIZE * textures.size());
  fftOutput = result.begin();
}

void BassDecoder::step() {
  if (current_idx >= sample_count) {
    if (wrote_textures) return;
    wrote_textures = true;

    //            int idx = 0;
    //            for(auto& tex : textures) {
    //                auto name = "tex" + std::to_string(idx) + ".png";
    //                tex->SaveTextureToFile(name);
    //                idx++;
    //            }
  }

  float intensity =
      500.f * (1.f * fftData[FFT_SIZE - logLookup[img_y + 0] - 1] +
               6.f * fftData[FFT_SIZE - logLookup[img_y + 1] - 1] +
               1.f * fftData[FFT_SIZE - logLookup[img_y + 2] - 1]);
  img_y++;
  if (img_height == img_y) {
    img_y = 0;
    current_idx++;
    BASS_ChannelSetPosition(stream, current_idx * sample_len, BASS_POS_BYTE);
    BASS_ChannelGetData(stream, &fftData[0], BASS_DATA_FFT2048);
    auto texIdx =
        std::distance(result.begin(), fftOutput) / (img_height * FFT_SIZE);
    textures.at(texIdx)->SetData(&result[(img_height * FFT_SIZE) * texIdx]);
  }
  if (intensity > maxIntensity) {
    *fftOutput++ = palette[palette.size() - 1];
    return;
  }
  if (intensity < 0.0f) {
    *fftOutput++ = palette[0];
    return;
  }
  const float intensity_step =
      (palette.size() - 1) / std::log(maxIntensity + 1);
  int palettePos =
      static_cast<int>(intensity_step * log(intensity + 1));  // f2* ~[0,6.91]

  *fftOutput++ = palette[palettePos];
}

size_t BassDecoder::total_rows() {
  auto bytelen = BASS_ChannelGetLength(stream, BASS_POS_BYTE);
  return static_cast<size_t>(row_rate *
                             BASS_ChannelBytes2Seconds(stream, bytelen));
}
}  // namespace minuseins::audio
