#include <SDL2/SDL_audio.h>
class AudioManager {
public:
  AudioManager(const AudioManager &) = default;
  AudioManager(AudioManager &&) = delete;
  AudioManager &operator=(const AudioManager &) = default;
  AudioManager &operator=(AudioManager &&) = delete;

  AudioManager();

  void UnpausePlayback();
  void PausePlayback();

  ~AudioManager();

private:
  static void AudioCallback(void *userdata, uint8_t *stream, int len);

  constexpr static int SAMPLE_RATE_HZ = 44100;
  constexpr static int AMPLITUDE = 28000;
  constexpr static double FREQUENCY_HZ = 440;
  constexpr static int SAMPLES = 2048;
  constexpr static int SAMPLE_SIZE = 16;

  SDL_AudioDeviceID _audioDevice = 0;
};