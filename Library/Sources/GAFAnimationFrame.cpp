#include "GAFPrecompiled.h"
#include "GAFAnimationFrame.h"
#include "GAFSubobjectState.h"
#include "GAFTextureAtlas.h"

GAFAnimationFrame::GAFAnimationFrame()
{

}

GAFAnimationFrame::~GAFAnimationFrame()
{
    GAF_RELEASE_ARRAY(GAFAnimationFrame::SubobjectStates_t, m_subObjectStates);
}

const GAFAnimationFrame::SubobjectStates_t& GAFAnimationFrame::getObjectStates() const
{
    return m_subObjectStates;
}

void GAFAnimationFrame::pushObjectState(GAFSubobjectState* state)
{
    m_subObjectStates.push_back(state);
    state->addRef();
}

