#pragma once

#ifndef __GAF_ANIMATED_OBJECT_H__
#define __GAF_ANIMATED_OBJECT_H__

#include "GAFAnimation.h"
#include "GAFCollections.h"

class GAFAnimation;
class GAFSprite;
class GAFAnimator;
class GAFAnimatedObject;
class GAFAsset;

enum PCAnimationFPSType
{
    kGAFAnimationFPSType_15 = 15,
    kGAFAnimationFPSType_30 = 30,
    kGAFAnimationFPSType_60 = 60
};

enum GAFAnimatedObjectControlFlags
{
    kGAFAnimatedObjectControl_None = 0,
    // If specified, state of controlled object will be changed every frame (like it is by default) and then
    // animatedObject:didDisplayFrameWithSubobject: will be called
    kGAFAnimatedObjectControl_ApplyState = 1 << 0
};

/// @class GAFFramePlayedDelegate
/// You can get notification when particular frame of any GAFAnimatedObject is played.
/// To do this you have to inherit GAFFramePlayedDelegate and call setFramePlayedDelegate
/// method of your GAFAnimatedObject

class GAFFramePlayedDelegate
{
public:
    /// Callback function, called by GAF.
    /// @param object - selected animated object
    /// @param frame - frame number that is just played

    virtual void onFramePlayed(GAFAnimatedObject * object, int frame);
};

class GAFAnimatedObjectControlDelegate
{
public:
    virtual void onFrameDisplayed(GAFAnimatedObject * object, const GAFSprite * subobject) = 0;
};

class GAFAnimatedObject : public CCLayer, public GAFAnimation
{
protected:
    GAFAnimatedObject();
private:
    GAFAsset * _asset;

    SubObjects_t m_subObjects;
    SubObjects_t m_masks;

    CaptureObjects_t m_capturedObjects;
    bool _animationsSelectorScheduled;
    GAFFramePlayedDelegate * _framePlayedDelegate;
    GAFAnimatedObjectControlDelegate * _controlDelegate;

    int numberOfGlobalFramesForOneAnimationFrame();
    int _extraFramesCounter;
    PCAnimationFPSType _FPSType;

public:
    ~GAFAnimatedObject();
    static GAFAnimatedObject * create(GAFAsset * anAsset);
    static GAFAnimatedObject * createAndRun(const std::string& gafPath, bool looped = false);

    bool init(GAFAsset * anAsset);
    void processAnimations(float dt);
    CCPoint pupilCoordinatesWithXSemiaxis(float anXSemiaxis, float anYSemiaxis, CCPoint aCenter, CCPoint anExternalPoint);
    GAFSprite * subObjectForInnerObjectId(unsigned int anInnerObjectId);
    void removeAllSubObjects();
    
    void instantiateObject(const AnimationObjects_t& objs, const AnimationMasks_t& masks, const AnimationFrames_t& frames);

    void setSubobjectsVisible(bool visible);

    const SubObjects_t& getSubojects() const;
    const SubObjects_t& getMasks() const;

    void animatorDidPlayedFrame(GAFAnimator * anAnimator, int aFrameNo);

    virtual void start();
    virtual void stop();
    virtual void processAnimation();
    /// Takes control over subobject, which means that every frame control delegate will be notified to decide
    /// what to do with captured external object
    /// @note it supports only objects for now, DOES NOT SUPPORT MASKS
    /// @param subobject name taken from "namedParts" section in config
    /// @param controlFlags flags specifying what played will do with subobjects controlled externally
    /// @returns YES if control was successfully taken and all future
    bool captureControlOverSubobjectNamed(const char * aName, GAFAnimatedObjectControlFlags aControlFlags);
    /// Releases control over subobject captured earlier	
    void releaseControlOverSubobjectNamed(const char * aName);

    void setFramePlayedDelegate(GAFFramePlayedDelegate * delegate);
    void setControlDelegate(GAFAnimatedObjectControlDelegate * delegate);
    CCRect realBoundingBoxForCurrentFrame();

    unsigned int objectIdByObjectName(const std::string& aName);

}; // GAFAnimatedObject

#endif // __GAF_ANIMATED_OBJECT_H__
