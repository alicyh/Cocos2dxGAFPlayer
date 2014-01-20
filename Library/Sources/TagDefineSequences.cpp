#include "GAFPrecompiled.h"
#include "TagDefineSequences.h"

#include "GAFStream.h"
#include "GAFAsset.h"

void TagDefineSequences::read(GAFStream* in, GAFAsset* ctx)
{
    std::string id;
    in->readString(&id);
    int start = in->readU16();
    int end = in->readU16();

    ctx->pushAnimationSequence(id, start, end);
}
