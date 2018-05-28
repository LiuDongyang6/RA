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
	//不知是否需要手动删除对象？
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