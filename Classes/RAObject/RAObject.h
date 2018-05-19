#ifndef __RA_OBJECT__
#define __RA_OBJECT__

#include "cocos2d.h"
#include"RAPlayer.h"
#include"cocostudio\CocoStudio.h"
#include"ui\CocosGUI.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;

class RAObject : public cocos2d::Sprite
{
public:
	RAObject(int hp) :hp_(hp), original_hp_(hp) {}
	~RAObject()override {}
	virtual bool initWithFile(const std::string& filename)override;
	virtual bool sufferAttack(int damage);//return survive or not
	virtual bool annihilation();
protected:
	bool toBeOrNotToBe();
	int hp_;
	const int original_hp_;
};

template<typename T>
class RAConstructButton: public Ref
{
public:
	void onTouchEnded(Touch* touch, Event* type)
	{
		auto point = touch->getLocation();
		auto object = T::create();
		object->setPosition(point);
		Director::getInstance()->getRunningScene()->getChildByTag(1)->addChild(object);
	}
	static void initButton(Widget* UI, std::string& name)
	{
		Button* button = (Button*)Helper::seekWidgetByName(UI, name);
		button->setTouchEnabled(0);
		auto listener = EventListenerTouchOneByOne::create();
		RAConstructButton<T>* p = 0;
		listener->onTouchBegan = [](Touch* touch, Event* event) {
			return (RA::containsTouchLocation(touch, event));
		};
		listener->onTouchEnded = CC_CALLBACK_2(RAConstructButton<T>::onTouchEnded, p);
		Director::getInstance()->getEventDispatcher()->
			addEventListenerWithSceneGraphPriority(listener, button);
	}
};

template<typename T>
void RAConstructButton<T>::initButton(Widget* UI, std::string& name);

namespace RA
{
	bool containsTouchLocation(Touch* touch, Event* event);
}

#endif // __RA_SOLDIER__
