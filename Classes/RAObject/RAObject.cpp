#include"RAObject.h"

USING_NS_CC;

int RAPlayer::power_ = 200;
int RAPlayer::capital_ = 10000;
Widget* RAPlayer::current_UI_ = 0;

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
	RAPlayer::power() += power_cost_;
	RAPlayer::capital() += capital_cost_;
	//不知是否需要手动删除对象？
	return true;
}

bool RAObject::initWithFile(const std::string& filename)
{
	Sprite::initWithFile(filename);
	hp_ = original_hp_;
	RAPlayer::power() -= power_cost_;
	RAPlayer::capital() -= capital_cost_;
	return true;
}




//
//RA
//
namespace RA {
	bool containsTouchLocation(Touch* touch, Event* event)
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
}







