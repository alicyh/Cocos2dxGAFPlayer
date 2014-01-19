#pragma once

class GAFTextureAtlas;
class GAFAnimationFrame;
class GAFSprite;
class GAFFilterData;

enum GAFFilterType
{
    GFT_Blur = 1
};

typedef std::vector<GAFTextureAtlas*> TextureAtlases_t;
typedef std::map<unsigned int, unsigned int> AnimationMasks_t;      //! Object id -> Element Atlas Id
typedef std::map<unsigned int, unsigned int> AnimationObjects_t;    //! Object id -> Element Atlas Id
typedef std::vector<GAFAnimationFrame*> AnimationFrames_t;
typedef std::map<unsigned int, GAFSprite*> SubObjects_t;
typedef std::map<GAFFilterType, GAFFilterData*> Filters_t;
