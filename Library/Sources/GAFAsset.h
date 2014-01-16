#pragma once

#ifndef __GAF_ASSET_H__
#define __GAF_ASSET_H__

namespace cocos2d
{
    class CCDictionary;
    class CCArray;
}
using namespace cocos2d;

class GAFTextureAtlas;
class GAFAnimatedObject;
class GAFAnimationSequence;

class GAFLoader;

class GAFAsset : public CCObject
{
public:

    typedef std::list<GAFTextureAtlas*> TextureAtlases_t;
    typedef std::map<unsigned int, unsigned int> AnimationMasks_t;
    typedef std::map<unsigned int, unsigned int> AnimationObjects_t;

private:
    TextureAtlases_t        m_textureAtlases;
    AnimationMasks_t        m_animationMasks;
    AnimationObjects_t      m_animationObjects;

private:
    void loadAnimationSequences(CCArray * aSequencesNodes);
    void loadFramesFromConfigDictionary(CCDictionary * aConfigDictionary);
    CCArray * objectStatesFromConfigFrame(CCDictionary * configFrame);
private:
    float _usedAtlasContentScaleFactor;
    float atlasScaleFromAtlasConfig(CCDictionary * anAtlasConfigDictionary);
    int _majorVersion;
    int _minorVersion;
    GAFTextureAtlas * _textureAtlas;
    // dictionary of objects [ObjectId -> AtlasElementName]	
    CCDictionary   * _objects;
    // dictionary of masks [MaskId -> AtlasElementName]	
    CCDictionary   * _masks;
    /// List of GAFActionObject-s representing zones to interact with	
    CCArray        * _interactionObjects;
    /// List of GAFActionObject-s representing zones on which objects can be put
    CCArray        * _standObjects;

    ///< List of GAFAnimationFrame objects
    CCArray        * _animationFrames;
    CCDictionary   * _animationSequences;

    CCDictionary   * _namedParts;

public:
    /// Initializes asset with JSON data
    static GAFAsset * create(const std::string& jsonPath);
    bool  initWithImageData(const std::string& jsonPath);

    bool  initWithGAFFile(const std::string& filePath);

    void                    pushTextureAtlas(GAFTextureAtlas* atlas);
    void                    pushAnimationMask(unsigned int objectId, unsigned int elementAtlasIdRef);
    void                    pushAnimationObjects(unsigned int objectId, unsigned int elementAtlasIdRef);

    GAFAsset();
    ~GAFAsset();
    /// total number of frames in animation
    int animationFramesCount() const;
    GAFTextureAtlas * textureAtlas();
    /// Dictionary of objects [ObjectId -> AtlasElementName]
    CCDictionary   * objects();
    /// Dictionary of masks [MaskId -> AtlasElementName]
    CCDictionary   * masks();
    /// Dictionary of masks [MaskId -> AtlasElementName]
    CCDictionary   * namedParts();
    /// List of GAFAnimationSequences objects	
    CCDictionary   * animationSequences();
    /// get GAFAnimationSequence by name specified in editor
    GAFAnimationSequence   * getSequence(const char * name);
    /// get all of the sequences
    CCDictionary* getSequences() const;
    /// get GAFAnimationSequence by last frame number in sequence	
    GAFAnimationSequence   * getSequenceByLastFrame(int frame);
    /// List of GAFAnimationFrame objects	
    CCArray        * animationFrames();
    static bool isAssetVersionPlayable(const char * version);

    GAFAnimatedObject * createObject();
    GAFAnimatedObject * createObjectAndRun(bool looped = false);

    /// desired content scale factor
    static int desiredCsf();
    /// sets desired content scale factor
    static void setDesiredCsf(int csf);
    /// used content scale factor		
    inline float usedAtlasContentScaleFactor()const
    {
        return _usedAtlasContentScaleFactor;
    }

};


#endif // __GAF_ASSET_H__
