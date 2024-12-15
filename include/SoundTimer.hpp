#pragma once

class SoundTimer {
public:
  void Tick();

private:
  void StartSound();
  void StopSound();
  int _counter;
};