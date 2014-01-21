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

class GAFSubobjectState
{
private:
    Filters_t       m_filters;
    float           _colorMults[4];
    float           _colorOffsets[4];

    unsigned long   m_refCount;

public:

    unsigned int objectIdRef;
    unsigned int maskObjectIdRef;

    int zIndex;
    CCAffineTransform affineTransform;

    bool initEmpty(unsigned int objectIdRef);

    ~GAFSubobjectState();
    GAFSubobjectState();

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


    void                addRef();
    void                release();

}; // GAFSubobjectState

#endif // __GAF_SUBOBJECT_STATE__