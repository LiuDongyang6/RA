#include"RAObject.h"

USING_NS_CC;


//
//RAObject
//
bool RAObject::sufferAttack(int damage)
{
	hp_ -= damage;
	return toBeOrNotToBe();
}

bool RAObject::toBeOrNotToBe()//this should be called after getting attacked
{
	if (hp_ < 0)
	{
		annihilation();
		return false;
	}
	else return true;
}

bool RAObject::annihilation()
{
	removeAllChildrenWithCleanup(true);
	removeFromParentAndCleanup(true);
	RAPlayer::resumePower(power_cost_);
	RAPlayer::resumeCapital(capital_cost_);
	//��֪�Ƿ���Ҫ�ֶ�ɾ������
	return true;
}

bool RAObject::initWithFile(const std::string& filename)
{
	Sprite::initWithSpriteFrameName(filename);
	hp_ = original_hp_;
	RAPlayer::consumePower(power_cost_) ;
	RAPlayer::consumeCapital(capital_cost_);
	return true;
}




//
//RA
//
namespace RA {
	bool containsTouchLocation(Touch* touch, Event* event)
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
}







