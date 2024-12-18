#pragma once

class SoundTimer {
public:
  void Tick();
  void SetTimer(int val);

private:
  void StartSound();
  void StopSound();
  int _counter;
};