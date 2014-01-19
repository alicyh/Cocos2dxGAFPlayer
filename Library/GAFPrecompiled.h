#pragma once

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <stack>

#include <assert.h>
#include <algorithm>

#include <cocos2d.h>
//#include <cocos-ext.h>

#define GAF_SAFE_RELEASE_MAP(__type, __instance)\
for (__type::iterator i = __instance.begin(), e = __instance.end(); i != e; ++i) {\
i->second->release(); \
} __instance.clear();

#define GAF_SAFE_RELEASE_ARRAY(__type, __instance)\
for (__type::iterator i = __instance.begin(), e = __instance.end(); i != e; ++i) {\
(*i)->release(); \
} __instance.clear();

#define GAF_RELEASE_ARRAY(__type, __instance)\
for (__type::iterator i = __instance.begin(), e = __instance.end(); i != e; ++i) {\
delete *i; \
} __instance.clear();

#define GAF_RELEASE_MAP(__type, __instance)\
for (__type::iterator i = __instance.begin(), e = __instance.end(); i != e; ++i) {\
delete i->second; \
} __instance.clear();


enum 
{
    IDNONE = UINT_MAX
};

using namespace cocos2d;