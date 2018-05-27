#include"RAUtility.h"
//
//RAUtility
//

std::string RAUtility::RAObjectProperty;

bool RAUtility::containsTouchLocation(Touch* touch, Event* event)
{
	// ��ȡ�¼����󶨵� target 
	auto target = static_cast<Sprite*>(event->getCurrentTarget());

	// ��ȡ��ǰ�����������԰�ť��λ������
	Point locationInNode = target->convertToNodeSpace(touch->getLocation());
	Size s = target->getContentSize();
	Rect rect = Rect(0, 0, s.width, s.height);

	// �����Χ�жϼ��
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