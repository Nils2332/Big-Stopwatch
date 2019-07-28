#pragma once

#include <cstdint>

namespace System {

void ResumeClock();

void PowerDown();

uint8_t nRF905sendtimestamp(uint32_t address, uint16_t from);

uint8_t nRF905send(uint32_t address, uint16_t type, uint16_t payload, uint16_t from);

void print(const char *fmt, ...);

}
