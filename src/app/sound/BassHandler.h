//
// Created by bone on 08.05.18.
//

#ifndef VISCOMFRAMEWORK_BASSHANDLER_H
#define VISCOMFRAMEWORK_BASSHANDLER_H

#include <experimental/filesystem>
#include <iostream>

namespace viscom::enh {
    class GLTexture;
}

namespace minuseins::audio {
// constexpr float bpm = 150.0f; /* beats per minute */
constexpr float bpm = 172.0f; /* beats per minute */
constexpr int rpb = 8;        /* rows per beat */
constexpr double row_rate = (double(bpm) / 60) * rpb;

constexpr int default_device = -1;
constexpr int channels = 1;
constexpr int FFT_SIZE = 1024;

namespace fs = std::experimental::filesystem;

constexpr size_t img_height = 128;
constexpr float samples_per_second = 100.f;
struct BassDecoder {
  BassDecoder(unsigned int stream);
  const unsigned int stream;
  const uint64_t sample_len;
  const size_t sample_count;
  const std::array<unsigned int, 256 * 3> palette;
  const std::array<int, img_height + 2> logLookup;

  const float maxIntensity = 500 * 2;
  size_t current_idx = 0;
  size_t img_y = 0;

  std::vector<uint32_t> result;
  std::vector<uint32_t>::iterator fftOutput;
  std::array<float, FFT_SIZE> fftData = std::array<float, FFT_SIZE>{};
  std::vector<std::unique_ptr<viscom::enh::GLTexture>> textures;
  void step();
  size_t total_rows();
  bool wrote_textures = false;
};

class BassHandler {
  static void print_problem(const std::string& calledFn);

 public:
  // last two parameters are for directSound
  BassHandler(int device = default_device, uint32_t freq = 44100,
              uint32_t flags = 0);

  void pause();

  void play();

  void set_row(int row);

  double get_row();

  void set_position(double time);

  double get_time();

  std::tuple<uint64_t, double> get_length();

  void update(uint32_t length = 0);

  bool get_fft(float* samples);

  uint32_t startRecording(uint32_t freq = 44100, uint32_t channels = 1,
                          uint32_t flags = 1);  // BASS_SAMPLE_8BITS

  uint32_t openFile(const fs::path& path,
                    uint32_t flags = 0x20000);  // BASS_STREAM_PRESCAN

  virtual ~BassHandler();

  uint32_t outputDevice_;
  uint32_t inputDevice_;
  uint32_t recorder_;
  uint32_t output_;
};
}  // namespace minuseins::audio

#endif  // VISCOMFRAMEWORK_BASSHANDLER_H
