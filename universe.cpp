#include "universe.h"
#include <stdint.h>
#include <random>
#include <fstream>
#include <iostream>
#include <time.h>
#include <string>

#define INIT_CIV_NUM 20
#define SEED 0x24123400

const uint32_t sTIME = time(NULL);

Universe::Universe(uint16_t InitCivNum, uint32_t seed) {
    std::vector<std::vector<uint8_t>>* _resource_map = generateResourceMap(seed, 64);
    VectorTo24bitBMP(_resource_map);
}

/*
std::vector<std::vector<uint8_t>>* generateResourceMap(uint32_t seed) {

    std::vector<std::vector<uint8_t>> Map2, Map1;
    static std::vector<std::vector<uint8_t>> Map0;

    std::mt19937 gen(seed);
    std::uniform_int_distribution<uint8_t> dist_0_255(0, 255);

    Map2.resize(MAP_SIZE + 2);
    for (int i = 0; i < MAP_SIZE + 2; i ++) {
        Map2[i].resize(MAP_SIZE + 2);
        for (int j = 0; j < MAP_SIZE + 2; j ++) {
            Map2[i][j] = dist_0_255(gen);
        }
    }

    Map1.resize(MAP_SIZE + 1);
    for (int i = 0; i < MAP_SIZE + 1; i ++) {
        Map1[i].resize(MAP_SIZE + 1);
        for (int j = 0; j < MAP_SIZE + 1; j ++) {
            Map1[i][j] = (uint8_t)((Map2[i][j] + Map2[i + 1][j] + Map2[i][j + 1] + Map2[i + 1][j + 1]) / 4);
        }
    }

    Map0.resize(MAP_SIZE);
    for (int i = 0; i < MAP_SIZE; i ++) {
        Map0[i].resize(MAP_SIZE);
        for (int j = 0; j < MAP_SIZE; j ++) {
            Map0[i][j] = (uint8_t)((Map1[i][j] + Map1[i + 1][j] + Map1[i][j + 1] + Map1[i + 1][j + 1]) / 4);
        }
    }

    // delete this in relese
    for (int i = 0; i < MAP_SIZE; i ++) {
        for (int j = 0; j < MAP_SIZE; j ++) {
            std::cout << (uint16_t) Map0[i][j] << " ";
        }
        std::cout << std::endl;
    }
    //

    return &Map0;

}
*/

std::vector<std::vector<uint8_t>>* generateResourceMap(uint32_t seed, uint16_t stage) {
    if (stage == 0) {return nullptr;}
    std::vector<std::vector<std::vector<uint8_t>>> Maps;
    static std::vector<std::vector<uint8_t>> Map0;

    std::mt19937 gen(seed);
    std::uniform_int_distribution<uint8_t> dist_0_255(0, 255);

    Map0.resize(MAP_SIZE);
    for (uint32_t i = 0; i < MAP_SIZE; i ++) {
        Map0[i].resize(MAP_SIZE);
    }

    Maps.resize(stage);
    for (uint16_t i = 0; i < stage; i ++) {
        Maps[i].resize(MAP_SIZE + stage - i);
        for (uint32_t j = 0; j < MAP_SIZE + stage - i; j ++) {
            Maps[i][j].resize(MAP_SIZE + stage - i);
        }
    }

    for (uint32_t i = 0; i < MAP_SIZE + stage; i ++) {
        for (uint32_t j = 0; j < MAP_SIZE + stage; j ++) {
            Maps[0][i][j] = (uint8_t) (dist_0_255(gen) * dist_0_255(gen) / 256);
        }
    }

    for (uint16_t n = 1; n < stage; n ++) {
        for (uint32_t i = 0; i < MAP_SIZE + stage - n; i ++) {
            for (uint32_t j = 0; j < MAP_SIZE + stage - n; j ++) {
                Maps[n][i][j] = (uint8_t) ((Maps[n - 1][i][j] + Maps[n - 1][i + 1][j] + Maps[n - 1][i][j + 1] + Maps[n - 1][i + 1][j + 1]) / 4);
            }
        }
    }

    for (uint32_t i = 0; i < MAP_SIZE; i ++) {
        for (uint16_t j = 0; j < MAP_SIZE; j ++) {
            Map0[i][j] = (uint8_t) ((Maps[stage - 1][i][j] + Maps[stage - 1][i + 1][j] + Maps[stage - 1][i][j + 1] + Maps[stage - 1][i + 1][j + 1]) / 4);
        }
    }
    
    return &Map0;
}

void VectorTo24bitBMP(std::vector<std::vector<uint8_t>>* _vector) {
    if (_vector == nullptr) {return;}
    uint32_t bmp_size = 54 + ((4 - MAP_SIZE * 3 % 4) % 4 + MAP_SIZE * 3) * MAP_SIZE;

    std::cout << bmp_size << std::endl;
    //uint16_t complement = (4 - MAP_SIZE * 3 % 4) % 4;

    //(*_vector)
    uint8_t* bmp = (uint8_t*) malloc(bmp_size);
    for (uint32_t i = 0; i < bmp_size; i ++) {
        bmp[i] = 0x00;
    }

    bmp[0] = 0x42;
    bmp[1] = 0x4d;

    uint8_t* bit_psize = endianConvertB2L(bmp_size);
    uint8_t* bit_pwidth = endianConvertB2L(MAP_SIZE);
    for (int i = 0; i < 4; i ++) {
        bmp[2 + i] = bit_psize[i];

        /*
        bmp[6 + i] = 0x00;
        bmp[10 + i] = 0x00;
        bmp[14 + i] = 0x00;
        bmp[18 + i] = 0x00;
        bmp[22 + i] = 0x00;
        */
        bmp[18 + i] = bmp[22 + i] = bit_pwidth[i];

    }
    bmp[10] = 0x36;
    bmp[14] = 0x28;
    bmp[26] = 0x01;
    bmp[28] = 0x18;

    uint32_t index = 54;

    for (int i = MAP_SIZE - 1; i >= 0; i --) {
        for (int j = 0; j < MAP_SIZE; j ++) {
            bmp[index] = bmp[index + 1] = bmp[index + 2] = 256 - (*_vector)[i][j];
            index += 3;
        }
        index += (4 - MAP_SIZE * 3 % 4) % 4;
    }
    
    byteToBMPFile(bmp, bmp_size, "resourcemap");

    free(bmp);
    return;
}

void byteToBMPFile(uint8_t* bytes, uint32_t bytes_size, const char* name) {
    std::ofstream BMPFile;
    BMPFile.open(name + std::to_string(sTIME) + ".bmp", std::ios::out | std::ios::binary);

    if (BMPFile.is_open()) {
        for (int i = 0; i < bytes_size; i ++) {
            BMPFile << bytes[i];
        }
        BMPFile.close();
    } else {
        std::cout << "Unable to open the file." << std::endl;
    }
    return;
}

uint8_t* endianConvertB2L(uint32_t bits) {
    static uint8_t result[4];

    for (int i = 0; i < 4; i ++) {
        result[i] = (bits & (0xFF << i * 8)) >> i * 8;
    }
    return result;
}

void loop() {

}

int main() {
    uint16_t seed = 0;

    Universe* _universe = new Universe(INIT_CIV_NUM, seed);
    


    delete _universe;
    return 0;
}
