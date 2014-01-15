#pragma once



class GAFStream;

class PrimiriveDeserializer
{
public:
    static void deserialize(GAFStream* in, CCPoint* out);
    static void deserialize(GAFStream* in, CCRect* out);
    static void deserialize(GAFStream* in, CCAffineTransform* out);
    static void deserialize(GAFStream* in, CCSize* out);

};