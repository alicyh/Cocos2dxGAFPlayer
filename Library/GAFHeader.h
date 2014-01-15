#pragma once

class GAFHeader
{
public:

    enum Compression
    {
        __CompressionDefault = 0, //! Internal
        CompressedNone = 0x00474146,
        CompressedZip = 0x00474143,
    };

public:
    Compression     compression;
    unsigned short  version;
    unsigned int    fileLenght;
    //CCRect          frameSize;
};