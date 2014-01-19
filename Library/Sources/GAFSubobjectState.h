#pragma once

#ifndef __GAF_SUBOBJECT_STATE__
#define __GAF_SUBOBJECT_STATE__

#include "GAFCollections.h"

namespace cocos2d
{
    class CCDictionary;
}

using namespace cocos2d;

enum GAFColorTransformIndex
{
    GAFCTI_R,
    GAFCTI_G,
    GAFCTI_B,
    GAFCTI_A,
};

class GAFSubobjectState //: public CCObject
{
private:
    Filters_t       m_filters;
    CCDictionary *  _filters;
    float           _colorMults[4];
    float           _colorOffsets[4];
public:

    unsigned int objectIdRef;
    unsigned int maskObjectIdRef;

    std::string objectId;
    int zIndex;
    std::string maskObjectId;
    CCAffineTransform affineTransform;

    static GAFSubobjectState * createWithStateDictionary(CCDictionary * dict, const char * objectId);
    static GAFSubobjectState * createEmptyWithObjectId(const char * objectId);

    static GAFSubobjectState* createEmpty(unsigned int objectIdRef);

    static GAFSubobjectState* create();
    bool init();

    bool initWithStateDictionary(CCDictionary * dict, const char * objectId);
    bool initEmptyWinthObjectId(const char * objectId);

    bool initEmpty(unsigned int objectIdRef);

    ~GAFSubobjectState();
    GAFSubobjectState();

    inline CCDictionary * filters() const
    {
        return _filters;
    }
    inline float * colorMults()
    {
        return &_colorMults[0];
    }
    inline float * colorOffsets()
    {
        return &_colorOffsets[0];
    }

    inline const float * colorMults() const 
    {
        return &_colorMults[0];
    }

    inline const float * colorOffsets() const 
    {
        return &_colorOffsets[0];
    }

    inline bool isVisisble() const
    {
        return _colorMults[GAFCTI_A] != 0;
    }

    void                ctxMakeIdentity();

    void                pushFilter(GAFFilterType type, GAFFilterData* filter);
    const Filters_t&    getFilters() const;

}; // GAFSubobjectState

#endif // __GAF_SUBOBJECT_STATE__