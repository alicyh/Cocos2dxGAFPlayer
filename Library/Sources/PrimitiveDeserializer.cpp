#include "GAFPrecompiled.h"
#include "PrimitiveDeserializer.h"

#include "GAFStream.h"

void PrimiriveDeserializer::deserialize(GAFStream* in, CCPoint* out)
{
    out->x = in->readFloat();
    out->y = in->readFloat();
}

void PrimiriveDeserializer::deserialize(GAFStream* in, CCRect* out)
{
    deserialize(in, &out->origin);
    deserialize(in, &out->size);
}

void PrimiriveDeserializer::deserialize(GAFStream* in, CCAffineTransform* out)
{
    in->readNBytesOfT(out, sizeof(CCAffineTransform));
}

void PrimiriveDeserializer::deserialize(GAFStream* in, CCSize* out)
{
    out->width = in->readFloat();
    out->height = in->readFloat();
}
