#include"RAUtility.h"
#include"TiledMap\tiled_map.h"
//
//RAUtility
//

std::string RAUtility::RAObjectProperty;

bool RAUtility::containsTouchLocation(Touch* touch, Event* event)
{
	// 获取事件所绑定的 target 
	auto target = static_cast<Sprite*>(event->getCurrentTarget());

	// 获取当前点击点所在相对按钮的位置坐标
	Point locationInNode = target->convertToNodeSpace(touch->getLocation());
	Size s = target->getContentSize();
	Rect rect = Rect(0, 0, s.width, s.height);

	// 点击范围判断检测
	if (rect.containsPoint(locationInNode))
		return true;
	return false;
}

Json::Value RAUtility::RAgetProperty(const int id, const char* property)
{
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(RAObjectProperty, root, false))
	{
		return root[id][property];
	}
	else
	{
		cocos2d::log("unable to open RAObjectProperty");
		return -1;
	}
}

Point RAUtility::getPositionInMap(Point point)
{
	point -= RAMap::getMap()->getPosition();
	return point;
}

Point RAUtility::getBottumHalfPoint(Sprite* sprite)
{
	auto halfHeight = (sprite->getContentSize().height) / 2;
	Vec2 halfPoint = sprite->getPosition() - Vec2(0, halfHeight);
	return halfPoint;
}

float RAUtility::stof(std::string str)
{
	char* chs = new char[str.size()];
	for (int i = 0; i != str.size(); ++i)
	{
		chs[i] = str[i];
	}
	float f = *(float*)(chs);
	delete chs;
	return f;

}
std::string RAUtility::ftostr(float f)
{
	std::string str;
	char* ch = (char*)(&f);
	for (int i = 0; i != sizeof(f); ++i)
	{
		str.push_back(ch[i]);
	}
	return str;
}

std::string RAUtility::coortostr(Point point)
{
	std::string str;
	std::string temp = ftostr(point.x);
	//caution
	//length is ACSII
	char length=temp.size();
	str.push_back(length);
	str.append(temp);
	temp = ftostr(point.y);
	length = temp.size();
	str.push_back(length);
	str.append(temp);
	return str;
}