#ifndef __RA_UTILITY_H__
#define __RA_UTILITY_H__
#include<cocos2d.h>
#include<json\json.h>
USING_NS_CC;


class RAUtility
{
public:
	static bool containsTouchLocation(Touch* touch, Event* event);
	//object≈‰÷√ Ù–‘
	static std::string RAObjectProperty;
	//
	static Json::Value RAgetProperty(const int id, const char* property);
};
#endif // !__RA_UTILITY_H__
