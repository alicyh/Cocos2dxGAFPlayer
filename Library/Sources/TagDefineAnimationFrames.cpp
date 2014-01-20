#include "GAFPrecompiled.h"
#include "TagDefineAnimationFrames.h"

#include "GAFStream.h"
#include "GAFAsset.h"

#include "PrimitiveDeserializer.h"

#include "GAFSubobjectState.h"
#include "GAFAnimationFrame.h"
#include "GAFFilterData.h"

void TagDefineAnimationFrames::read(GAFStream* in, GAFAsset* ctx)
{
    unsigned int count = in->readU32();

    typedef std::map<unsigned int, GAFSubobjectState*> States_t;
    States_t currentStates;

    assert(!ctx->getAnimationObjects().empty());

    for (AnimationObjects_t::const_iterator i = ctx->getAnimationObjects().begin(), e = ctx->getAnimationObjects().end(); i != e; ++i)
    {
        unsigned int objectId = i->first;
        GAFSubobjectState *state = new GAFSubobjectState();
        state->initEmpty(objectId);

        currentStates[objectId] = state;
    }

    for (unsigned int i = 0; i < count; ++i)
    {
        unsigned int frameNumber = in->readU32();

        if ((frameNumber - 1) == i)
        {
            unsigned int numObjects = in->readU32();

            typedef std::list<GAFSubobjectState*> StatesList_t;
            StatesList_t statesList;

            for (unsigned int j = 0; j < numObjects; ++j)
            {
                GAFSubobjectState* state = extractState(in);

                statesList.push_back(state);
            }

            for (StatesList_t::iterator it = statesList.begin(), ie = statesList.end(); it != ie; ++it)
            {
                GAFSubobjectState* st = *it;

                GAFSubobjectState* ps = currentStates[st->objectIdRef];

                if (ps)
                {
                    ps->release();
                }

                currentStates[st->objectIdRef] = st;
            }
        }

        GAFAnimationFrame* frame = new GAFAnimationFrame();

        for (States_t::iterator it = currentStates.begin(), ie = currentStates.end(); it != ie; ++it)
        {
            frame->pushObjectState(it->second);
        }

        ctx->pushAnimationFrame(frame);
    }
}

GAFSubobjectState* TagDefineAnimationFrames::extractState(GAFStream* in)
{
    GAFSubobjectState* state = new GAFSubobjectState();

    float ctx[7];

    char hasColorTransform = in->readUByte();
    char hasMasks = in->readUByte();
    char hasEffect = in->readUByte();

    state->objectIdRef = in->readU32();
    state->zIndex = in->readS32();
    state->colorMults()[GAFCTI_A] = in->readFloat();

    PrimiriveDeserializer::deserialize(in, &state->affineTransform);

    if (hasColorTransform)
    {
        in->readNBytesOfT(ctx, sizeof(float)* 7);

        float* ctxOff = state->colorOffsets();
        float* ctxMul = state->colorMults();

        ctxOff[GAFCTI_A] = ctx[0];

        ctxMul[GAFCTI_R] = ctx[1];
        ctxOff[GAFCTI_R] = ctx[2];

        ctxMul[GAFCTI_G] = ctx[3];
        ctxOff[GAFCTI_G] = ctx[4];

        ctxMul[GAFCTI_B] = ctx[5];
        ctxOff[GAFCTI_B] = ctx[6];
    }
    else
    {
        state->ctxMakeIdentity();
    }

    if (hasEffect)
    {
        unsigned char effects = in->readUByte();

        for (unsigned int e = 0; e < effects; ++e)
        {
            GAFFilterType type = (GAFFilterType)in->readU32();

            if (type == GFT_Blur)
            {
                CCSize p;
                PrimiriveDeserializer::deserialize(in, &p);
                GAFBlurFilterData* blurFilter = new GAFBlurFilterData();
                blurFilter->blurSize = p;
                state->pushFilter(type, blurFilter);
            }
        }
    }

    if (hasMasks)
    {
        state->maskObjectIdRef = in->readU32();
    }

    return state;
}
