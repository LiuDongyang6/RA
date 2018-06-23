#ifndef __RESOURCE_UI_H__
#define __RESOURCE_UI_H__
#include"RAObject/RAObject.h"
class RAResourceUI
{
public:
	static void init();
	static TextAtlas* capital_;
	static TextAtlas* power_;
	static void changeCapital(int capital);
	static void changePower(int power);
	static Widget* ResourceUI;
};

#endif // ! __RESOURCE_UI_H__
