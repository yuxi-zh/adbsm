#pragma once

#include <cstdint>

#ifndef FRAME_SIZE
#define FRAME_SIZE 4096
#endif

struct Frame {
  //
  uint8_t field[FRAME_SIZE];
};