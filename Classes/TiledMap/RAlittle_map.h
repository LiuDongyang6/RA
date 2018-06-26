#ifndef __RALIMAP__H__
#define __RALIMAP__H__

#include "cocos2d.h"

class littleMap
{
public:
	static bool init(int num);

	static cocos2d::Sprite* getLittleMap();

	static void chooseMap(int num);

	static cocos2d::Sprite* little_map;
};



#endif // __AB__
