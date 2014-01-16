#pragma once

#ifndef __GAF_TEXTURE_ATLAS_ELEMENT__
#define __GAF_TEXTURE_ATLAS_ELEMENT__

namespace cocos2d
{
    class CCDictionary;
}

using namespace cocos2d;

class GAFTextureAtlasElement : public CCObject
{
public:
    std::string name;
    CCPoint     pivotPoint;
    CCRect      bounds;
    float       scale;
    unsigned int atlasIdx;

    unsigned int elementAtlasIdx;

    static GAFTextureAtlasElement*   create();
    bool                             init();

    static GAFTextureAtlasElement * create(CCDictionary * aDictionary);
    bool initWithDictionary(CCDictionary * aDictionary);
private:
    GAFTextureAtlasElement();
}; // GAFTextureAtlasElement

#endif // __GAF_TEXTURE_ATLAS_ELEMENT__
