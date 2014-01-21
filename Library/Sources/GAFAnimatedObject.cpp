#include "GAFPrecompiled.h"
#include "GAFAsset.h"
#include "GAFAnimatedObject.h"
#include "GAFTextureAtlas.h"
#include "GAFTextureAtlasElement.h"
#include "GAFAnimationFrame.h"
#include "GAFSubobjectState.h"
#include "GAFSpriteWithAlpha.h"
#include "GAFStencilMaskSprite.h"
#include "GAFFilterData.h"

#ifdef max
#undef max
#endif

static const char * kGAFBlurFilterName = "Fblur";
static float const kAnimationFPS = 30.0;   // we keep this number 'almost' synchronized with web. The reason it's
// not 31 fps is that we will have some animation artifacts when running
// on 30/60 fps device.

static CCAffineTransform GAF_CGAffineTransformCocosFormatFromFlashFormat(CCAffineTransform aTransform)
{
    CCAffineTransform transform = aTransform;
    transform.b = -transform.b;
    transform.c = -transform.c;
    transform.ty = -transform.ty;
    return transform;
}

GAFAnimatedObject::GAFAnimatedObject()
:
_asset(NULL),
_extraFramesCounter(0),
_framePlayedDelegate(NULL),
_controlDelegate(NULL)
{
}

GAFAnimatedObject::~GAFAnimatedObject()
{
    CC_SAFE_RELEASE(_asset);

    GAF_SAFE_RELEASE_MAP(SubObjects_t, m_subObjects);
    GAF_SAFE_RELEASE_MAP(SubObjects_t, m_masks);
}

GAFAnimatedObject * GAFAnimatedObject::create(GAFAsset * anAsset)
{
    GAFAnimatedObject * ret = new GAFAnimatedObject();
    if (ret && ret->init(anAsset))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_RELEASE(ret);
    return NULL;
}

GAFAnimatedObject * GAFAnimatedObject::createAndRun(const std::string& gafPath, bool looped)
{
    GAFAsset * asset = GAFAsset::create(gafPath);
    return asset->createObjectAndRun(looped);
}

bool GAFAnimatedObject::init(GAFAsset * anAsset)
{
    if (!anAsset)
    {
        return false;
    }

    if (!GAFAnimation::init(anAsset))
    {
        return false;
    }

    if (_asset != anAsset)
    {
        CC_SAFE_RELEASE(_asset);
        _asset = anAsset;
        CC_SAFE_RETAIN(_asset);
    }

    GAF_SAFE_RELEASE_MAP(SubObjects_t, m_subObjects);
    GAF_SAFE_RELEASE_MAP(SubObjects_t, m_masks);

    _FPSType = kGAFAnimationFPSType_30;
    _extraFramesCounter = 0;
    _animationsSelectorScheduled = false;
    return true;
}

unsigned int GAFAnimatedObject::objectIdByObjectName(const std::string& aName)
{
    const NamedParts_t& np = _asset->getNamedParts();

    NamedParts_t::const_iterator it = np.find(aName);

    if (it != np.end())
    {
        return it->second;
    }

    return IDNONE;
}

void GAFAnimatedObject::instantiateObject(const AnimationObjects_t& objs, const AnimationMasks_t& masks, const AnimationFrames_t& frames)
{
    for (AnimationObjects_t::const_iterator i = objs.begin(), e = objs.end(); i != e; ++i)
    {
        GAFTextureAtlas* atlas = _asset->textureAtlas();
        const GAFTextureAtlas::Elements_t& elementsMap = atlas->getElements();
        CCSpriteFrame * spriteFrame = NULL;

        unsigned int atlasElementIdRef = i->second;

        GAFTextureAtlas::Elements_t::const_iterator elIt = elementsMap.find(atlasElementIdRef); //! Search for atlas element by its xref

        assert(elIt != elementsMap.end());

        const GAFTextureAtlasElement* txElemet = NULL;

        if (elIt != elementsMap.end())
        {
            txElemet = elIt->second;

            if (atlas->textures()->count() >= txElemet->atlasIdx + 1)
            {
                CCTexture2D * texture = (CCTexture2D *)atlas->textures()->objectAtIndex(txElemet->atlasIdx);
                spriteFrame = CCSpriteFrame::createWithTexture(texture, txElemet->bounds);
            }
            else
            {
                CCLOGERROR("Cannot add sub object with Id: %d, atlas with idx: %d not found.", atlasElementIdRef, txElemet->atlasIdx);
            }
        }

        if (spriteFrame && txElemet)
        {
            GAFSpriteWithAlpha *sprite = new GAFSpriteWithAlpha();
            sprite->initWithSpriteFrame(spriteFrame);

            sprite->objectIdRef = atlasElementIdRef;

            sprite->setVisible(false);
            CCPoint pt = CCPointMake(0 - (0 - (txElemet->pivotPoint.x / sprite->getContentSize().width)),
                0 + (1 - (txElemet->pivotPoint.y / sprite->getContentSize().height)));
            sprite->setAnchorPoint(pt);

            if (txElemet->scale != 1.0f)
            {
                sprite->setAtlasScale(1.0f / txElemet->scale);
            }
            // visual studio compile fix
            ccBlendFunc blend = { GL_ONE, GL_ONE_MINUS_SRC_ALPHA };
            sprite->setBlendFunc(blend);
            m_subObjects[i->first] = sprite;
        }
        else
        {
            assert(false);
            CCLOGERROR("Cannot add subnode with AtlasElementRef: %d, not found in atlas(es). Ignoring.", atlasElementIdRef);
        }

    }

    for (AnimationMasks_t::const_iterator i = masks.begin(), e = masks.end(); i != e; ++i)
    {
        GAFTextureAtlas* atlas = _asset->textureAtlas();
        const GAFTextureAtlas::Elements_t& elementsMap = atlas->getElements();

        unsigned int atlasElementIdRef = i->second;

        GAFTextureAtlas::Elements_t::const_iterator elIt = elementsMap.find(atlasElementIdRef); //! Search for atlas element by it's xref

        assert(elIt != elementsMap.end());

        const GAFTextureAtlasElement* txElemet = NULL;

        if (elIt != elementsMap.end())
        {
            txElemet = elIt->second;

            CCSpriteFrame * spriteFrame = CCSpriteFrame::createWithTexture(atlas->texture(), txElemet->bounds);

            if (spriteFrame)
            {
                GAFStencilMaskSprite *mask = new GAFStencilMaskSprite();
                mask->initWithSpriteFrame(spriteFrame);
                mask->objectIdRef = i->first;
                CCPoint pt = CCPointMake(0 - (0 - (txElemet->pivotPoint.x / mask->getContentSize().width)),
                    0 + (1 - (txElemet->pivotPoint.y / mask->getContentSize().height)));

                mask->setAnchorPoint(pt);
                if (txElemet->scale != 1.0f)
                {
                    mask->setAtlasScale(1.0f / txElemet->scale);
                }

                m_masks[i->first] = mask;
                addChild(mask);
            }
        }
    }
}

bool GAFAnimatedObject::captureControlOverSubobjectNamed(const char * aName, GAFAnimatedObjectControlFlags aControlFlags)
{
    unsigned int objectId = objectIdByObjectName(aName);
    if (IDNONE == objectId)
    {
        return false;
    }

    CaptureObjects_t::const_iterator cpoIt = m_capturedObjects.find(objectId);

    if (cpoIt != m_capturedObjects.end())
    {
        return false;
    }

    m_capturedObjects[objectId] = aControlFlags;
    return true;
}

void GAFAnimatedObject::releaseControlOverSubobjectNamed(const char * aName)
{
    unsigned int objectId = objectIdByObjectName(aName);
    if (objectId != IDNONE)
    {
        CaptureObjects_t::const_iterator cpoIt = m_capturedObjects.find(objectId);

        if (cpoIt != m_capturedObjects.end())
        {
            m_capturedObjects.erase(cpoIt);
        }
    }
}

void GAFAnimatedObject::removeAllSubObjects()
{
    for (SubObjects_t::iterator i = m_subObjects.begin(), e = m_subObjects.end(); i != e; ++i)
    {
        GAFSprite* sprite = i->second;
        sprite->setVisible(false);
        sprite->removeFromParentAndCleanup(true);
    }

    GAF_SAFE_RELEASE_MAP(SubObjects_t, m_subObjects);
}

void GAFAnimatedObject::setSubobjectsVisible(bool visible)
{
    for (SubObjects_t::iterator i = m_subObjects.begin(), e = m_subObjects.end(); i != e; ++i)
    {
        GAFSprite* sprite = i->second;
        sprite->setVisible(visible);
    }
}

void GAFAnimatedObject::processAnimations(float dt)
{
    if (++_extraFramesCounter >= numberOfGlobalFramesForOneAnimationFrame())
    {
        _extraFramesCounter = 0;
        if (!isDone() && isAnimationRunning())
        {
            step();
        }
        if (_framePlayedDelegate)
        {
            _framePlayedDelegate->onFramePlayed(this, currentFrameIndex());
        }
    }
}

CCPoint GAFAnimatedObject::pupilCoordinatesWithXSemiaxis(float anXSemiaxis, float anYSemiaxis, CCPoint aCenter, CCPoint anExternalPoint)
{
    CCPoint rePoint = CCPointZero;
    float const firstConst = (anYSemiaxis * anYSemiaxis);
    float const secondConst = firstConst / (anXSemiaxis * anXSemiaxis);
    float length = std::numeric_limits<float>::max();
    float tmpLenght = length;
    float xStart = -anXSemiaxis;
    float xEnd = 0;

    if (((anExternalPoint.x - aCenter.x) * (anExternalPoint.x - aCenter.x)) / (anXSemiaxis * anXSemiaxis) +
        ((anExternalPoint.y - aCenter.y) * (anExternalPoint.y - aCenter.y)) / (anYSemiaxis * anYSemiaxis) < (1.0 + FLT_EPSILON))
    {
        return anExternalPoint;
    }
    if (anExternalPoint.x > aCenter.x)
    {
        xStart = 0;
        xEnd = anXSemiaxis;
    }
    for (float i = xStart; i <= xEnd; i += 0.5f)
    {
        float y = sqrtf(firstConst - secondConst * (i * i));
        if (anExternalPoint.y > aCenter.y)
        {
            y = -y;
        }
        float dX = anExternalPoint.x - (aCenter.x + i);
        float dY = anExternalPoint.y - (aCenter.y - y);
        tmpLenght = sqrtf(dX * dX + dY * dY);
        if (tmpLenght < length)
        {
            length = tmpLenght;
            rePoint = CCPointMake(aCenter.x + i, aCenter.y - y);
        }
    }
    return rePoint;
}

GAFSprite * GAFAnimatedObject::subObjectForInnerObjectId(unsigned int anInnerObjectId)
{
    for (SubObjects_t::iterator i = m_subObjects.begin(), e = m_subObjects.end(); i != e; ++i)
    {
        GAFSprite* anim = i->second;
        if (anim->objectIdRef == anInnerObjectId)
        {
            return anim;
        }
    }
    return NULL;
}

const SubObjects_t& GAFAnimatedObject::getSubojects() const
{
    return m_subObjects;
}

const SubObjects_t& GAFAnimatedObject::getMasks() const
{
    return m_masks;
}

void GAFAnimatedObject::start()
{
    GAFAnimation::start();
    removeAllSubObjects();
    instantiateObject(_asset->getAnimationObjects(), _asset->getAnimationMasks(), _asset->getAnimationFrames());

    schedule(SEL_SCHEDULE(&GAFAnimatedObject::processAnimations));
    _animationsSelectorScheduled = true;
}

void GAFAnimatedObject::stop()
{
    GAFAnimation::stop();

    unschedule(SEL_SCHEDULE(&GAFAnimatedObject::processAnimations));
    _animationsSelectorScheduled = false;
}

int GAFAnimatedObject::numberOfGlobalFramesForOneAnimationFrame()
{
    float globalFPS = roundf(1.0f / (float)CCDirector::sharedDirector()->getAnimationInterval());

    if (globalFPS > (float)_FPSType - FLT_EPSILON)
    {
        return (int)roundf(globalFPS / (float)_FPSType);
    }
    else
    {
        return 1;
    }
}

void GAFAnimatedObject::processAnimation()
{
    GAFAnimationFrame *currentFrame = _asset->getAnimationFrames()[_currentFrameIndex];
    setSubobjectsVisible(false);

    const GAFAnimationFrame::SubobjectStates_t& states = currentFrame->getObjectStates();
    {
        size_t statesCount = states.size();
        for (size_t i = 0; i < statesCount; ++i)
        {
            const GAFSubobjectState *state = states[i];
            {
                GAFSpriteWithAlpha *subObject = NULL;

                SubObjects_t::iterator sboIt = m_subObjects.find(state->objectIdRef);

                if (sboIt != m_subObjects.end())
                {
                    subObject = static_cast<GAFSpriteWithAlpha*>(sboIt->second);
                }

                if (subObject)
                {
                    // Validate sprite type (w/ or w/o filter)
                    GAFBlurFilterData *blurFilter = NULL;
                    const Filters_t& filters = state->getFilters();

                    if (!filters.empty())
                    {
                        Filters_t::const_iterator flIt = filters.find(GFT_Blur);
                        if (flIt != filters.end())
                        {
                            blurFilter = static_cast<GAFBlurFilterData*>(flIt->second);
                        }
                    }
                    CCPoint prevAP = subObject->getAnchorPoint();
                    CCSize  prevCS = subObject->getContentSize();

                    if (blurFilter)
                    {
                        subObject->setBlurRadius(CCSizeMake(blurFilter->blurSize.width / 4, blurFilter->blurSize.height / 4));
                    }
                    else
                    {
                        subObject->setBlurRadius(CCSizeZero);
                    }

                    CCSize newCS = subObject->getContentSize();
                    CCPoint newAP = CCPointMake(((prevAP.x - 0.5) * prevCS.width) / newCS.width + 0.5,
                        ((prevAP.y - 0.5) * prevCS.height) / newCS.height + 0.5);
                    subObject->setAnchorPoint(newAP);

                    if (state->maskObjectIdRef == IDNONE)
                    {
                        if (!subObject->getParent())
                        {
                            addChild(subObject);
                        }
                    }
                    else
                    {
                        if (subObject->getParent())
                        {
                            removeChild(subObject, false);
                        }
                        GAFStencilMaskSprite * mask = NULL;

                        if (!m_masks.empty())
                        {
                            mask = static_cast<GAFStencilMaskSprite *>(m_masks[state->maskObjectIdRef]);
                        }

                        if (mask)
                        {
                            mask->addMaskedObject(subObject);
                        }
                    }

                    bool subobjectCaptured = false;
                    GAFAnimatedObjectControlFlags controlFlags = kGAFAnimatedObjectControl_None;

                    CaptureObjects_t::const_iterator cpoIt = m_capturedObjects.find(state->objectIdRef);
                    if (cpoIt != m_capturedObjects.end())
                    {
                        subobjectCaptured = true;
                        controlFlags = (GAFAnimatedObjectControlFlags)cpoIt->second;
                    }

                    if (!subobjectCaptured ||
                        (subobjectCaptured && (controlFlags & kGAFAnimatedObjectControl_ApplyState)))
                    {
                        CCAffineTransform stateTransform = state->affineTransform;
                        float csf = _asset->usedAtlasContentScaleFactor();
                        stateTransform.tx *= csf;
                        stateTransform.ty *= csf;
                        CCAffineTransform t = GAF_CGAffineTransformCocosFormatFromFlashFormat(state->affineTransform);
                        subObject->setExternaTransform(t);
                        if (subObject->getZOrder() != state->zIndex)
                        {
                            subObject->setZOrder(state->zIndex);
                            GAFStencilMaskSprite::updateMaskContainerOf(subObject);
                        }
                        subObject->setVisible(state->isVisisble());
                        subObject->setColorTransform(state->colorMults(), state->colorOffsets());
                    }
                }
                else
                {
                    GAFSprite * mask = NULL;
                    if (!m_masks.empty())
                    {
                        mask = m_masks[state->objectIdRef];
                    }
                    if (mask)
                    {
                        mask->setExternaTransform(GAF_CGAffineTransformCocosFormatFromFlashFormat(state->affineTransform));

                        if (mask->getZOrder() != state->zIndex)
                        {
                            mask->setZOrder(state->zIndex);
                            GAFStencilMaskSprite::updateMaskContainerOf(subObject);
                        }
                    }
                }
            }
        }
    }

    if (_controlDelegate)
    {
        size_t statesCount = states.size();
        for (size_t i = 0; i < statesCount; ++i)
        {

            const GAFSubobjectState *state = states[i];

            SubObjects_t::iterator sboIt = m_subObjects.find(state->objectIdRef);

            if (sboIt != m_subObjects.end())
            {
                const GAFSpriteWithAlpha *subObject = static_cast<const GAFSpriteWithAlpha*>(m_subObjects[state->objectIdRef]);

                CaptureObjects_t::const_iterator cpoIt = m_capturedObjects.find(state->objectIdRef);

                bool subobjectCaptured = cpoIt != m_capturedObjects.end();
                if (subobjectCaptured && _controlDelegate)
                {
                    _controlDelegate->onFrameDisplayed(this, subObject);
                }
            }
            else
            {
                // Masks cannot be captured right now
            }
        }
    }
}

void GAFAnimatedObject::setFramePlayedDelegate(GAFFramePlayedDelegate * delegate)
{
    _framePlayedDelegate = delegate;
}

void GAFAnimatedObject::setControlDelegate(GAFAnimatedObjectControlDelegate * delegate)
{
    _controlDelegate = delegate;
}

static CCRect GAFCCRectUnion(const CCRect& src1, const CCRect& src2)
{
    CCRect result;

    float x1 = std::min(src1.getMinX(), src2.getMinX());
    float y1 = std::min(src1.getMinY(), src2.getMinY());
    float x2 = std::max(src1.getMaxX(), src2.getMaxX());
    float y2 = std::max(src1.getMaxY(), src2.getMaxY());

    result.origin = ccp(x1, x2);
    result.size = CCSizeMake(x2 - x1, y2 - y1);
    return result;
}

CCRect GAFAnimatedObject::realBoundingBoxForCurrentFrame()
{
    CCRect result = CCRectZero;

    for (SubObjects_t::iterator i = m_subObjects.begin(), e = m_subObjects.end(); i != e; ++i)
    {
        GAFSprite* anim = i->second;
        if (anim->isVisible())
        {
            CCRect bb = anim->boundingBox();
            result = GAFCCRectUnion(result, bb);
        }
    }

    return CCRectApplyAffineTransform(result, nodeToParentTransform());
}

