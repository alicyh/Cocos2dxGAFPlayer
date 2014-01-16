#include "GAFPrecompiled.h"
#include "TagDefineAnimationFrames.h"

#include "GAFStream.h"
#include "GAFAsset.h"


void TagDefineAnimationFrames::read(GAFStream* in, GAFAsset* ctx)
{
    unsigned int count = in->readU32();

    for (unsigned int i = 0; i < count; ++i)
    {
        unsigned int frameNumber = in->readU32();

    }
}
