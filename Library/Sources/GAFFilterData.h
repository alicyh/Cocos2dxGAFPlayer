#pragma once

#ifndef __GAF_FILTER_DATA__
#define __GAF_FILTER_DATA__

class GAFFilterData : public CCObject
{

};

class GAFBlurFilterData : public GAFFilterData
{
public:
    static GAFBlurFilterData * create(float _blurX, float _blurY);

    CCSize blurSize;
protected:
    GAFBlurFilterData();
};

#endif // __GAF_FILTER_DATA__
