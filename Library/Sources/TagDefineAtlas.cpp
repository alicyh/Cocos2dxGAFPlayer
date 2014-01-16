#include "GAFPrecompiled.h"
#include "TagDefineAtlas.h"

#include "GAFStream.h"
#include "GAFAsset.h"

#include "GAFTextureAtlas.h"
#include "GAFTextureAtlasElement.h"
#include "PrimitiveDeserializer.h"

void TagDefineAtlas::read(GAFStream* in, GAFAsset* ctx)
{
    GAFTextureAtlas* txAtlas = GAFTextureAtlas::create();

    txAtlas->setScale(in->readFloat());

    unsigned char atlasesCount = in->readUByte();

    for (unsigned char i = 0; i < atlasesCount; ++i)
    {
        GAFTextureAtlas::AtlasInfo ai;
        GAFTextureAtlas::AtlasInfo::Source aiSource;

        ai.id = in->readU32();

        unsigned char sources = in->readUByte();

        for (unsigned char j = 0; j < sources; ++j)
        {
            in->readString(&aiSource.source);
            aiSource.csf = in->readFloat();
        }

        txAtlas->pushAtlasInfo(ai);
    }

    unsigned int elementsCount = in->readU32();

    for (unsigned int i = 0; i < elementsCount; ++i)
    {
        GAFTextureAtlasElement* element = GAFTextureAtlasElement::create();

        PrimiriveDeserializer::deserialize(in, &element->pivotPoint);
        CCPoint origin;
        PrimiriveDeserializer::deserialize(in, &origin);
        element->scale = in->readFloat();

        //! TODO: Optimize this to read CCRect
        float width = in->readFloat();
        float height = in->readFloat();

        element->atlasIdx = in->readU32();
        element->elementAtlasIdx = in->readU32();

        element->bounds.origin = origin;
        element->bounds.size = CCSize(width, height);

        txAtlas->pushElement(element->elementAtlasIdx, element);
    }

    ctx->pushTextureAtlas(txAtlas);
}
