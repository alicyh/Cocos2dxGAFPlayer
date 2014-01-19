#include "GAFPrecompiled.h"
#include "GAFTextureAtlas.h"
#include "GAFTextureAtlasElement.h"
#include "GAFAsset.h"
#include "cocoa/CCInteger.h"
#include "GAFJSONData.h"

#if CC_ENABLE_CACHE_TEXTURE_DATA
#include "textures/CCTextureCache.h"
#endif

static const char * kAtlasesKey = "atlases";
static const char * kElementsKey = "elements";
static const char * kSourcesKey = "sources";
static const char * kSourceKey = "source";
static const char * kCSFKey = "csf";
static const char * kTextureAtlasKey = "textureAtlas";

GAFTextureAtlas::GAFTextureAtlas()
:
_loaded(false),
_images(NULL),
_textures(NULL),
_elements(NULL),
m_scale(1.f)
{
}

GAFTextureAtlas::~GAFTextureAtlas()
{
    CC_SAFE_RELEASE(_images);
    CC_SAFE_RELEASE(_textures);
    CC_SAFE_RELEASE(_elements);

    GAF_RELEASE_MAP(GAFTextureAtlas::Elements_t, m_elements);
}

GAFTextureAtlas * GAFTextureAtlas::create(const char * aTexturesDirectory, CCDictionary * aTextureAtlasConfigDictionary)
{
    GAFTextureAtlas *pRet = new GAFTextureAtlas();
    if (pRet && pRet->init(aTexturesDirectory, aTextureAtlasConfigDictionary))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

GAFTextureAtlas* GAFTextureAtlas::create()
{
    GAFTextureAtlas *pRet = new GAFTextureAtlas();

    if (pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return NULL;
}

static bool compare_atlases(const void* p1, const void* p2)
{
    CCDictionary* a1 = (CCDictionary*)p1;
    CCDictionary* a2 = (CCDictionary*)p2;
    CCInteger * id1 = (CCInteger*)a1->objectForKey("id");
    CCInteger * id2 = (CCInteger*)a2->objectForKey("id");
    CCAssert(id1 && id2, "id parameters must be valid");
    return id1->getValue() < id2->getValue();
}

static bool compareAtlasesById(const GAFTextureAtlas::AtlasInfo& ai1, const GAFTextureAtlas::AtlasInfo& ai2)
{
    return ai1.id < ai2.id;
}

bool GAFTextureAtlas::init()
{
    return true;
}


void GAFTextureAtlas::loadImages(const std::string& dir)
{
    std::stable_sort(m_atlasInfos.begin(), m_atlasInfos.end(), compareAtlasesById);

    CC_SAFE_RELEASE(_images);
    _images = new CCArray();

    if (!m_atlasInfos.empty())
    {
        for (unsigned int i = 0; i < m_atlasInfos.size(); ++i)
        {
            AtlasInfo& info = m_atlasInfos[i];

            std::string source;

            for (unsigned int j = 0; j < info.m_sources.size(); ++j)
            {
                AtlasInfo::Source& aiSource = info.m_sources[j];
                if (1.f == aiSource.csf)
                {
                    source = aiSource.source;
                }

                if (aiSource.csf == GAFAsset::desiredCsf())
                {
                    source = aiSource.source;
                    break;
                }
            }

            CCImage* image = new CCImage();
            const char * path = CCFileUtils::sharedFileUtils()->fullPathFromRelativeFile(source.c_str(), dir.c_str());
            image->initWithImageFile(path);
            _images->addObject(image);
            image->release();
        }
    }
}

bool GAFTextureAtlas::init(const char * aTexturesDirectory, CCDictionary * aTextureAtlasConfigDictionary)
{
    if (!aTexturesDirectory || !aTextureAtlasConfigDictionary)
    {
        CCLOG("parameters should not equal to nil");
        return false;
    }
    CCArray * atlasesInfo = (CCArray *)aTextureAtlasConfigDictionary->objectForKey(kAtlasesKey);

    std::sort(atlasesInfo->data->arr,
        atlasesInfo->data->arr + atlasesInfo->data->num,
        compare_atlases);

    CC_SAFE_RELEASE(_images);
    _images = new CCArray();

    if (atlasesInfo)
    {
        for (unsigned int i = 0; i < atlasesInfo->count(); ++i)
        {
            CCDictionary * atlasInfo = (CCDictionary*)atlasesInfo->objectAtIndex(i);
            int desiredCsf = GAFAsset::desiredCsf();
            CCArray * sources = (CCArray*)atlasInfo->objectForKey(kSourcesKey);
            std::string source;
            for (unsigned int j = 0; j < sources->count(); ++j)
            {
                CCDictionary * csfdict = (CCDictionary *)sources->objectAtIndex(j);
                CCNumber * scsf = (CCNumber*)csfdict->objectForKey(kCSFKey);
                int csf = scsf->getIntValue();
                CCString * s = (CCString*)csfdict->objectForKey(kSourceKey);
                if (1 == csf)
                {
                    source = s->getCString();
                }
                if (csf == desiredCsf)
                {
                    source = s->getCString();
                    break;
                }
            }
            CCImage * image = new CCImage();
            const char * path = CCFileUtils::sharedFileUtils()->fullPathFromRelativeFile(source.c_str(), aTexturesDirectory);
            image->initWithImageFile(path);
            _images->addObject(image);
            image->release();
        }
    }

    loadElementsFromAnimationConfigDictionary(aTextureAtlasConfigDictionary);
    return true;
}

bool GAFTextureAtlas::loadElementsFromAnimationConfigDictionary(CCDictionary * aConfigDictionary)
{
    CCArray   * nElements = (CCArray*)aConfigDictionary->objectForKey(kElementsKey);

    if (nElements)
    {
        int s = nElements->count();

        CC_SAFE_RELEASE(_elements);
        _elements = CCDictionary::create();
        CC_SAFE_RETAIN(_elements);

        for (int i = 0; i < s; ++i)
        {
            CCObject * nElement = nElements->objectAtIndex(i);
            CCDictionary * dict = dynamic_cast<CCDictionary*>(nElement);
            if (dict)
            {
                /*GAFTextureAtlasElement * element = GAFTextureAtlasElement::create(dict);
                if (element)
                {
                    _elements->setObject(element, element->name);
                }*/
            }
            else
            {
                CCLOGERROR("Error when parsing texture atlas element JSON. Atlas element not of CCDictionary type");
            }
        }
    }
    else
    {
        CCLOGERROR("Error when parsing texture atlas element JSON.");
        return false;
    }
    _loaded = true;
    return true;
}


CCDictionary * GAFTextureAtlas::elements()
{
    return _elements;
}

CCImage     * GAFTextureAtlas::image()
{
    if (_images && _images->count() > 0)
    {
        return (CCImage*)_images->objectAtIndex(0);
    }
    return NULL;
}

CCArray     * GAFTextureAtlas::images()
{
    return _images;
}

CCTexture2D * GAFTextureAtlas::texture()
{
    if (_textures && _textures->count() > 0)
    {
        return (CCTexture2D*)_textures->objectAtIndex(0);
    }
    return NULL;
}

CCArray * GAFTextureAtlas::textures()
{
    if (!_textures)
    {
        _textures = CCArray::createWithCapacity(_images->count());
        for (unsigned int i = 0; i < _images->count(); ++i)
        {
            CCTexture2D * texture = new CCTexture2D();
            CCImage * image = (CCImage*)_images->objectAtIndex(i);
            texture->initWithImage(image);
            _textures->addObject(texture);
#if CC_ENABLE_CACHE_TEXTURE_DATA
            VolatileTexture::addCCImage(texture, image);
#endif
            texture->release();
        }
        _textures->retain();
    }
    return _textures;
}

void GAFTextureAtlas::setScale(float val)
{
    m_scale = val;
}

float GAFTextureAtlas::getScale() const
{
    return m_scale;
}

void GAFTextureAtlas::pushAtlasInfo(const AtlasInfo& ai)
{
    m_atlasInfos.push_back(ai);
}

void GAFTextureAtlas::pushElement(unsigned int idx, GAFTextureAtlasElement* el)
{
    m_elements[idx] = el;
}

const GAFTextureAtlas::Elements_t& GAFTextureAtlas::getElements() const
{
    return m_elements;
}
