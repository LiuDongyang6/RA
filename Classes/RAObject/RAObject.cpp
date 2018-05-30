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
	//˳���ܵߵ�

	//remove children��ᵼ��construct button��������������ֹͣ�۲���ģʽ
	removeAllChildrenWithCleanup(true);
	//�ָ���Դ��������Ϣ
	RAPlayer::resumePower(power_cost_);
	//2��3�ߵ���delete�Լ�����power_cost_���Ϊδ֪
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