#pragma once

struct BITMAPFILEHEADER{
    uint8_t Char1;
    uint8_t Char2;
    uint32_t Size;
    uint16_t Reserved1;
    uint16_t Reserved2;
    uint32_t PArrayLoc;
};

struct BITMAPV5HEADER{
    uint32_t        Size;
    int32_t         Width;
    int32_t         Height;
    uint16_t        Planes;
    uint16_t        BitCount;
    uint32_t        Compression;
    uint32_t        SizeImage;
    int32_t         XPelsPerMeter;
    int32_t         YPelsPerMeter;
    uint32_t        ClrUsed;
    uint32_t        ClrImportant;
    uint32_t        RedMask;
    uint32_t        GreenMask;
    uint32_t        BlueMask;
    uint32_t        AlphaMask;
    uint32_t        CSType;
    uint8_t         Endpoints[36];
    uint32_t        GammaRed;
    uint32_t        GammaGreen;
    uint32_t        GammaBlue;
    uint32_t        Intent;
    uint32_t        ProfileData;
    uint32_t        ProfileSize;
    uint32_t        Reserved;
};

template <typename T>
void printHex(T value);

void readUINT32(const std::vector<unsigned char>& data, uint32_t* value, size_t offset);

void readINT32(const std::vector<unsigned char>& data, int32_t* value, size_t offset);

void readUINT16(const std::vector<unsigned char>& data, uint16_t* value, size_t offset);

void readUINT8(const std::vector<unsigned char>& data, uint8_t* value, size_t offset);

unsigned char* parseBMP(const std::string& path, unsigned int channels);