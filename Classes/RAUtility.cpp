#include"RAUtility.h"
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