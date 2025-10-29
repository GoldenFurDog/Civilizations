#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <vector>
#include <stdint.h>
#include <string>
#include "civilization.h"

#define MAP_SIZE 1200



class Universe {

    public:
    std::vector<Civilization> Civilizations;

    Universe(uint16_t InitCivNum, uint32_t seed);

};

std::vector<std::vector<uint8_t>>* generateResourceMap(uint32_t seed, uint16_t stage);
void VectorTo24bitBMP(std::vector<std::vector<uint8_t>>* _vector);
void byteToBMPFile(uint8_t* bytes, uint32_t bytes_size, const char* name);
uint8_t* endianConvertB2L(uint32_t bits);

#endif
