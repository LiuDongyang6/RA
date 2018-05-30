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
	//顺序不能颠倒

	//remove children后会导致construct button析构函数触发，停止观察者模式
	removeAllChildrenWithCleanup(true);
	//恢复资源，发布消息
	RAPlayer::resumePower(power_cost_);
	//2，3颠倒会delete自己，则power_cost_会变为未知
	removeFromParentAndCleanup(true);
	return true;
}

bool RAObject::initWithSpriteFrameName(const std::string& filename)
{
	Sprite::initWithSpriteFrameName(filename);
	RAPlayer::consumePower(power_cost_) ;
	RAPlayer::consumeCapital(capital_cost_);
	return true;
}

//
//RAConstructButton
//
typedef Sprite*(*CreateType)();
std::unordered_map<int, CreateType> RAConstructButton::CreateWiki;