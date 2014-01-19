#pragma once

#ifndef __GAF_FILTER_DATA__
#define __GAF_FILTER_DATA__

class GAFFilterData
{

};

class GAFBlurFilterData : public GAFFilterData
{
public:
    CCSize blurSize;
    GAFBlurFilterData();
};

#endif // __GAF_FILTER_DATA__
