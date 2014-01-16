#pragma once

#ifndef __GAF_ANIMATION_FRAME__
#define __GAF_ANIMATION_FRAME__

namespace cocos2d
{
    class CCDictionary;
    class CCArray;
}

class GAFTextureAtlas;
class GAFSubobjectState;

using namespace cocos2d;

class GAFAnimationFrame : public CCObject
{
public:
    typedef std::list<GAFSubobjectState*> SubobjectStates_t;
private:
    SubobjectStates_t       m_subObjectStates;

    CCArray * _initialInstructions;
    CCArray * _objectsStates;
public:
    GAFAnimationFrame();
    ~GAFAnimationFrame();
    //static GAFAnimationFrame * create(CCDictionary * aDictionary, GAFTextureAtlas * aTextureAtlas);
    //bool init(CCDictionary * aDictionary, GAFTextureAtlas * aTextureAtlas);
    CCArray * objectStates();
    void setObjectStates(CCArray * states);
};

#endif // __GAF_ANIMATION_FRAME__
