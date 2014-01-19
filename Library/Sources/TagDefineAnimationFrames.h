#pragma once

#include "DefinitionTagBase.h"

class GAFSubobjectState;

class TagDefineAnimationFrames : public DefinitionTagBase
{
private:
    GAFSubobjectState* extractState(GAFStream* in);
public:

    virtual void read(GAFStream*, GAFAsset*);

};