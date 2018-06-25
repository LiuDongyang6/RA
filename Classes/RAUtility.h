#ifndef __RA_UTILITY_H__
#define __RA_UTILITY_H__
#include<cocos2d.h>
#include<json\json.h>
USING_NS_CC;

class RAMap;

class RAUtility
{
public:
	static bool containsTouchLocation(Touch* touch, Event* event);
	//object≈‰÷√ Ù–‘
	static std::string RAObjectProperty;
	//
	static Json::Value RAgetProperty(const int id, const char* property);
	//parameter£∫a point which is the absolute position in GL
	//return the position relative to map
	static Point getPositionInMap(Point point);
	//
	static Point getBottumHalfPoint(Sprite* sprite);
	//
	static float stof(std::string str);
	//
	static std::string ftostr(float);
	//
	static std::string coortostr(Point point);
};
#endif // !__RA_UTILITY_H__
