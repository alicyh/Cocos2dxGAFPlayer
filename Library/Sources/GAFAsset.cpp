#include "GAFPrecompiled.h"
#include "GAFAsset.h"
#include "GAFData.h"
#include "GAFTextureAtlas.h"
#include "GAFTextureAtlasElement.h"
#include "GAFAnimationFrame.h"
#include "GAFSubobjectState.h"
#include "GAFInteractionObject.h"
#include "GAFActionObject.h"
#include "GAFAnimationSequence.h"
#include "GAFAnimatedObject.h"

#include "GAFLoader.h"

static float _currentDeviceScale = 1.0f;
static bool  _makeExactScaleForObject = false;
static float  _desiredCsf = 1.f;

float GAFAsset::desiredCsf()
{
    return CC_CONTENT_SCALE_FACTOR();
}

void GAFAsset::setDesiredCsf(float csf)
{
    _desiredCsf = csf;
}

GAFAnimatedObject * GAFAsset::createObject()
{
    if (!m_currentTextureAtlas)
    {
        return NULL;
    }

    return GAFAnimatedObject::create(this);
}

GAFAnimatedObject * GAFAsset::createObjectAndRun(bool looped)
{
    GAFAnimatedObject * res = createObject();
    if (res)
    {
        res->setLooped(looped);
        res->start();
    }
    return res;
}

GAFAsset::GAFAsset()
:
m_currentTextureAtlas(NULL)
{
}

GAFAsset::~GAFAsset()
{
    GAF_RELEASE_ARRAY(TextureAtlases_t, m_textureAtlases);
    GAF_RELEASE_ARRAY(AnimationFrames_t, m_animationFrames);
}

bool GAFAsset::isAssetVersionPlayable(const char * version)
{
    return true;
}

GAFAsset* GAFAsset::create(const std::string& gafFilePath)
{
    GAFAsset * ret = new GAFAsset();
    if (ret && ret->initWithGAFFile(gafFilePath))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_RELEASE(ret);
    return NULL;
}

bool GAFAsset::initWithGAFFile(const std::string& filePath)
{
    GAFLoader* loader = new GAFLoader();

    std::string fullfilePath = CCFileUtils::sharedFileUtils()->fullPathForFilename(filePath.c_str());

    bool isLoaded = loader->loadFile(fullfilePath, this);

    if (m_textureAtlases.empty())
    {
        return false;
    }

    if (isLoaded)
    {
        float atlasScale = m_textureAtlases[0]->getScale();

        m_currentTextureAtlas = m_textureAtlases[0];

        const unsigned int count = m_textureAtlases.size();

        for (unsigned int i = 1; i < count; ++i)
        {
            float as = m_textureAtlases[i]->getScale();
            if (fabs(atlasScale - _currentDeviceScale) > fabs(as - _currentDeviceScale))
            {
                m_currentTextureAtlas = m_textureAtlases[i];
                atlasScale = as;
            }
        }

        _usedAtlasContentScaleFactor = atlasScale;

        if (m_currentTextureAtlas)
        {
            m_currentTextureAtlas->loadImages(fullfilePath);
        }
    }

    delete loader;

    return isLoaded;
}

GAFTextureAtlas* GAFAsset::textureAtlas()
{
    return m_currentTextureAtlas;
}

int GAFAsset::animationFramesCount() const
{
    return m_animationFrames.size();
}

const GAFAnimationSequence* GAFAsset::getSequence(const std::string& name) const
{
    AnimationSequences_t::const_iterator it = m_animationSequences.find(name);

    if (it != m_animationSequences.end())
    {
        return &it->second;
    }

    return NULL;
}

const GAFAnimationSequence * GAFAsset::getSequenceByLastFrame(int frame) const
{
    if (m_animationSequences.empty())
    {
        return NULL;
    }

    for (AnimationSequences_t::const_iterator i = m_animationSequences.begin(), e = m_animationSequences.end(); i != e; ++i)
    {
        if (i->second.endFrameNo == frame)
        {
            return &i->second;
        }
    }

    return NULL;
}

void GAFAsset::pushTextureAtlas(GAFTextureAtlas* atlas)
{
    m_textureAtlases.push_back(atlas);
}

void GAFAsset::pushAnimationMask(unsigned int objectId, unsigned int elementAtlasIdRef)
{
    m_animationMasks[objectId] = elementAtlasIdRef;
}

void GAFAsset::pushAnimationObjects(unsigned int objectId, unsigned int elementAtlasIdRef)
{
    m_animationObjects[objectId] = elementAtlasIdRef;
}

void GAFAsset::pushAnimationFrame(GAFAnimationFrame* frame)
{
    m_animationFrames.push_back(frame);
}

const AnimationObjects_t& GAFAsset::getAnimationObjects() const
{
    return m_animationObjects;
}

const AnimationMasks_t& GAFAsset::getAnimationMasks() const
{
    return m_animationMasks;
}

const AnimationFrames_t& GAFAsset::getAnimationFrames() const
{
    return m_animationFrames;
}

void GAFAsset::pushAnimationSequence(const std::string nameId, int start, int end)
{
    GAFAnimationSequence seq;
    seq.name = nameId;
    seq.startFrameNo = start;
    seq.endFrameNo = end;

    m_animationSequences[nameId] = seq;
}

const AnimationSequences_t& GAFAsset::getAnimationSequences() const
{
    return m_animationSequences;
}

void GAFAsset::pushNamedPart(unsigned int objectIdRef, const std::string& name)
{
    m_namedParts[name] = objectIdRef;
}

const NamedParts_t& GAFAsset::getNamedParts() const
{
    return m_namedParts;
}
