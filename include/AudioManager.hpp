#include <SDL2/SDL_audio.h>
class AudioManager {
public:
  AudioManager();

private:
  constexpr static int SAMPLE_RATE_HZ = 44100;
  constexpr static int AMPLITUDE = 28000;
  constexpr static double FREQUENCY = 440;
  SDL_AudioDeviceID _audioDevice = 0;
};