#include "AudioManager.hpp"
#include "SdlError.hpp"
#include <SDL2/SDL_audio.h>
#include <cmath>
#include <cstdint>
#include <span>

void AudioManager::AudioCallback(void *userdata, uint8_t *stream, int len) {
  auto frequency = *static_cast<double *>(userdata);
  // NOLINTNEXTLINE(*-reinterpret-cast)
  auto *buffer = reinterpret_cast<int16_t *>(stream);
  const int length = len / 2; // 16-bit samples
  double phase = 0.0;
  static constexpr double TWO_PI = 2 * M_PI;
  std::span<int16_t> buffSpan(buffer, length);
  const double phaseInc = TWO_PI * frequency / SAMPLE_RATE_HZ;
  for (auto &elem : buffSpan) {
    elem = (Sint16)(AMPLITUDE * sin(phase));
    phase += phaseInc;
    if (phase > TWO_PI) {
      phase -= TWO_PI;
    }
  }
}

AudioManager::AudioManager() {
  SDL_AudioSpec want;
  SDL_zero(want);
  want.freq = SAMPLE_RATE_HZ;
  want.format = AUDIO_S16SYS;
  want.channels = 1;
  want.samples = SAMPLES;
  want.callback = AudioManager::AudioCallback;

  double frequency = FREQUENCY_HZ;
  want.userdata = &frequency;

  SDL_AudioSpec have;
  SDL_AudioDeviceID audio_device =
      SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
  if (audio_device == 0) {
    throw SdlError();
  }
  PausePlayback();
}

// NOLINTNEXTLINE(readability*const)
void AudioManager::UnpausePlayback() { SDL_PauseAudioDevice(_audioDevice, 0); }

// NOLINTNEXTLINE(readability*const)
void AudioManager::PausePlayback() { SDL_PauseAudioDevice(_audioDevice, 1); }