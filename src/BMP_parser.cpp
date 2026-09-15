#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <cmath>

#include <BMP_parser.hpp>

template <typename T>
void printHex(T value){
    std::cout << "0x" << std::hex << std::setw(sizeof(value) * 2) << std::setfill('0')
              << std::uppercase << static_cast<int>(value) << '\n';
    std::cout << std::dec; //reset ostream flags
}//prints any non-floating-point value in hex form

void readUINT32(const std::vector<unsigned char>& data, uint32_t* value, size_t offset){
    uint8_t p1 = data[offset];
    uint8_t p2 = data[offset + 1];
    uint8_t p3 = data[offset + 2];
    uint8_t p4 = data[offset + 3];

    *value = p1 | (p2 << 8)
                | (p3 << 16)
                | (p4 << 24);
}//specific to 32 bit unsigned int

void readINT32(const std::vector<unsigned char>& data, int32_t* value, size_t offset){
    int8_t p1 = data[offset];
    int8_t p2 = data[offset + 1];
    int8_t p3 = data[offset + 2];
    int8_t p4 = data[offset + 3];

    *value = p1 | (p2 << 8)
                | (p3 << 16)
                | (p4 << 24);
}//specific to 32 bit signed int

void readUINT16(const std::vector<unsigned char>& data, uint16_t* value, size_t offset){
    unsigned char p1 = data[offset];
    uint8_t p2 = data[offset + 1];

    *value = p1 | (p2 << 8);
}//specific to 16 bit unsigned int

void readUINT8(const std::vector<unsigned char>& data, uint8_t* value, size_t offset){
    *value = data[offset];
}//specific to 8 bit unsigned int

void printDetails(const BITMAPFILEHEADER& fh, const BITMAPV5HEADER& v5) {
    //Print BITMAPFILEHEADER info
    std::cout << "File header data:\n"
              << "\tFile type: " << fh.Char1 << fh.Char2 << '\n'
              << "\tFile size: " << fh.Size << " bytes" << '\n'
              << "\tPixel array located at an offset of " << fh.PArrayLoc << " bytes" << '\n';

    //Print BITMAPV5HEADER info
    std::string comp = (v5.Compression == 0) ? "BI_RGB" :
                       (v5.Compression == 1) ? "BI_RLE8" :
                       (v5.Compression == 2) ? "BI_RLE4" :
                       (v5.Compression == 3) ? "BI_BITFIELDS" :
                       (v5.Compression == 4) ? "BI_JPEG" :
                                               "BI_PNG";
    std::cout << "V5 header data:\n"
              << "\tHeader size: " << v5.Size << " bytes\n"
              << "\tImage width: " << v5.Width << " pixels\n"
              << "\tImage height: " << v5.Height << " pixels\n"
              << "\tNumber of planes: " << v5.Planes << '\n'
              << "\tBits per pixel: " << v5.BitCount << '\n'
              << "\tCompression method: " << comp << '\n'
              << "\tImage size: " << v5.SizeImage << " bytes\n"
              << "\tHorizontal pixels per meter: " << v5.XPelsPerMeter << '\n'
              << "\tVertical pixels per meter: " << v5.YPelsPerMeter << '\n'
              << "\tNumber of color indexes used from color table: " << v5.ClrUsed << '\n'
              << "\tNumber of important color indexes: " << v5.ClrImportant << '\n'
              << "\tRed mask: " << v5.RedMask << '\n'
              << "\tGreen mask: " << v5.GreenMask << '\n'
              << "\tBlue mask: " << v5.BlueMask << '\n'
              << "\tAlpha mask: " << v5.AlphaMask << '\n'
              << "\tColor space: " << v5.CSType << '\n'
              << "\tEndpoints ignored\n"
              << "\tGamma red ignored\n"
              << "\tGamma green ignored\n"
              << "\tGamma blue ignored\n"
              << "\tIntent doesn't matter\n"
              << "\tProfile data located at byte offset " << v5.ProfileData << '\n'
              << "\tProfile data is " << v5.ProfileSize << " bytes\n";
}

unsigned char* parseBMP(const std::string& path, unsigned int channels){
    std::ifstream fileIn;
    fileIn.open(path, std::ios::binary);
    if(!fileIn.is_open()){
        std::cout << "Failed to open file at path: " << path << '\n';
        return nullptr;
    }

    std::vector<unsigned char> data(
        (std::istreambuf_iterator<char>(fileIn)),
        std::istreambuf_iterator<char>()
    );

    fileIn.close();

    BITMAPFILEHEADER fh;//read BITMAPFILEHEADER
    readUINT8(data, &fh.Char1, 0);
    readUINT8(data, &fh.Char2, 1);
    readUINT32(data, &fh.Size, 2);
    readUINT16(data, &fh.Reserved1, 6);
    readUINT16(data, &fh.Reserved2, 8);
    readUINT32(data, &fh.PArrayLoc, 10);

    BITMAPV5HEADER v5;//read BITMAPHEADERV5 
    readUINT32(data, &v5.Size, 14);
    readINT32(data, &v5.Width, 18);
    readINT32(data, &v5.Height, 22);
    readUINT16(data, &v5.Planes, 26);
    readUINT16(data, &v5.BitCount, 28);
    readUINT32(data, &v5.Compression, 30);
    readUINT32(data, &v5.SizeImage, 34);
    readINT32(data, &v5.XPelsPerMeter, 38);
    readINT32(data, &v5.YPelsPerMeter, 42);
    readUINT32(data, &v5.ClrUsed, 46);
    readUINT32(data, &v5.ClrImportant, 50);
    readUINT32(data, &v5.RedMask, 54);
    readUINT32(data, &v5.GreenMask, 58);
    readUINT32(data, &v5.BlueMask, 62);
    readUINT32(data, &v5.AlphaMask, 66);
    readUINT32(data, &v5.CSType, 70);
    for(uint8_t i = 0; i < 36; i++){
        readUINT8(data, &v5.Endpoints[i], 74+i);
    }
    readUINT32(data, &v5.GammaRed, 110);
    readUINT32(data, &v5.GammaGreen, 114);
    readUINT32(data, &v5.GammaBlue, 118);
    readUINT32(data, &v5.Intent, 122);
    readUINT32(data, &v5.ProfileData, 126);
    readUINT32(data, &v5.ProfileSize, 130);
    readUINT32(data, &v5.Reserved, 134);

    //prepare for getting raw pixels by calculating row and padding size in bytes and create an array for pixels
 
    float rowSize = std::round((v5.BitCount * v5.Width) / 32.0f) * 4.0f;
    unsigned int padSize = ((v5.BitCount / 8) * v5.Width) % 4;
    unsigned int trueRowSize = rowSize - padSize;

    unsigned char* pixelsOut = new unsigned char[(static_cast<int>(rowSize) - padSize) * v5.Height];
    for(size_t i = 0; i < v5.Height; i++){
        for(size_t j = 0; j < rowSize; j++){
            if(rowSize - j > padSize){
                pixelsOut[j + (trueRowSize * i)] = data[fh.PArrayLoc + (j + (rowSize * i))];
            }
        }
    }
    
    return pixelsOut;
}