#include"RedAlert.h"
#include<fstream>
RedAlert* RedAlert::instance_ = NULL;
RedAlert* RedAlert::getInstance()
{
	if (instance_ == NULL)
		instance_ = new RedAlert();
	return instance_;
}
void RedAlert::initAll(int num)
{
	RAUtility::RAObjectProperty=FileUtils::getInstance()->getStringFromFile("RAObjectProperty.json");
	//initial create_function wiki
	initCreateWiki();
	//initial building sprite frame
	auto framecache = SpriteFrameCache::getInstance();
	framecache->addSpriteFramesWithFile("Buildings.plist");
	framecache->addSpriteFramesWithFile("OilField/OilField.plist");
	framecache->addSpriteFramesWithFile("effects/effects.plist");
	//
	RAMap::init(1);
	//initial selectBox
	selectBox = LayerColor::create(Color4B(145, 150, 134, 255));
	selectBox->setOpacity(70);
	selectBox->retain();
	//initial on touch
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(RedAlert::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(RedAlert::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(RedAlert::onTouchEnded, this);
	Director::getInstance()->getEventDispatcher()->
		addEventListenerWithSceneGraphPriority(listener, RAMap::getMap()->getChildByTag(1));
	//
	auto OilListener= EventListenerTouchOneByOne::create();
	OilListener->onTouchBegan = CC_CALLBACK_2(RedAlert::OilOnTouBegan, this);
	OilListener->setSwallowTouches(true);
	Director::getInstance()->getEventDispatcher()->
		addEventListenerWithSceneGraphPriority(OilListener, RAMap::getMap()->getChildByTag(2));
}
void RedAlert::initCreateWiki()
{
	//initial create_function wiki
	auto &wiki = RAConstructButton::CreateWiki;
	wiki.insert({ 0,RABase::create });
	wiki.insert({ 1,RAPowerStation::create });
	wiki.insert({ 2,RABarrack::create });
	wiki.insert({ 3,RAFairy::create });
	wiki.insert({ 4,RAAssassin::create });
	wiki.insert({ 5,RALancer::create });
	wiki.insert({ 6,RAGeneral::create });
	wiki.insert({ 7,RAAtomicBomb::create });
	wiki.insert({ 8,RABlackMagician::create });
	wiki.insert({ 9,RABomber::create });
	wiki.insert({ 10,RAEngineer::create });
	wiki.insert({ 11,RAWinterSoldier::create });
	wiki.insert({ 12,RAWitch::create });
	wiki.insert({ 13,RAWizzard::create });
	wiki.insert({ 14,RAOilField::create });
	wiki.insert({ 15,RADefendingBase::create });
	wiki.insert({ 16,RANuclearSilo::create });
	wiki.insert({ 17,RAManhattan::create });

}
void RedAlert::selectedSoldiersMove(Touch* touch)
{
	auto point = RAUtility::getPositionInMap(touch->getLocation());
	for (auto soldier : RAPlayer::selected_soldiers_)
	{
		soldier->runTo(point);
	}
}
bool RedAlert::OilOnTouBegan(Touch* touch, Event* event)
{
	auto point =RAUtility::getPositionInMap(touch->getLocation());
	auto pos = RAMap::cannotBuildOil(point, 4);
	if (pos!= Point(-1000.0f, -1000.0f))
	{
		const auto& soldiers = RAPlayer::selected_soldiers_;
		if ((soldiers.size() == 1) && ((*soldiers.begin())->getId() == 10))
		{
			auto engineer=static_cast<RAEngineer*>(*soldiers.begin());
			engineer->runToBuildOilField(point);
			return true;
		}
	}
	return false;
}
bool RedAlert::onTouchBegan(Touch* touch, Event* event)
{
	selectBox->setPosition(RAUtility::getPositionInMap(touch->getLocation()));
	selectBox->setContentSize(Size(0, 0));
	RAMap::getMap()->addChild(selectBox);
	
	return true;
}
void RedAlert::onTouchMoved(Touch* touch, Event* event)
{
	selectBox->setContentSize(Size(touch->getLocation() - touch->getStartLocation()));
}
void RedAlert::onTouchEnded(Touch* touch, Event* event)
{
	//unmoved
	if (touch->getStartLocation().distance(touch->getLocation())<10)
		selectedSoldiersMove(touch);
	//moved
	else
	{
		auto position = selectBox->getPosition();
		auto size = selectBox->getContentSize();
		auto rect = Rect(MIN(position.x,position.x+size.width),MIN(position.y, position.y + size.height),abs(size.width),abs(size.height));
		//尽管移动了，但范围内没有我方单位
		//则不会清空
		bool cleared=0;
		for (auto soldier : RAPlayer::all_soldiers_)
		{
			auto position = soldier->getPosition();
			if (rect.containsPoint(soldier->getPosition()))
			{
				if (!cleared)
				{
					RAPlayer::selected_soldiers_.clear();
					cleared = 1;
				}
				RAPlayer::selected_soldiers_.push_back(soldier);
			}
		}
	}
	RAMap::getMap()->removeChild(selectBox);
}
void RedAlert::HostileObjectAppear(int id, Point location,int count)
{
	RAConstructButton::LaunchTest(id);
	auto object=RAConstructButton::CreateWiki[id](location);
	object->setCount(count);
	RAPlayer::master_table_.insert({ count,object });
	object->changeControl(0);
}
void RedAlert::catcher(std::string message)
{
	using namespace std;
	char instruction_kind = message[0];
	int aim = stoi(message.substr(1, 6));
	message.erase(0,7);
	switch (instruction_kind)
	{
	case 'a':
	{
		if (RAPlayer::master_table_.count(aim)!=0)
		{
			RAPlayer::master_table_[aim]->annihilation();
		}
		break;
	}
	case 'b':
	{
		if (RAPlayer::master_table_.count(aim) == 0)
		{
			int id = stoi(message.substr(0, 3));
			message.erase(0, 3);
			float coords[2];
			for (int i = 0; i != 2; ++i)
			{
				char length = message[0];
				coords[i] = RAUtility::stof(message.substr(1, length));
				message.erase(0, length + 1);
			}
			Point location(coords);
			HostileObjectAppear(id, location, aim);
		}
		break;
	}
	default:
	{
		RAPlayer::master_table_[aim]->followInstruction(message,instruction_kind);
		std::ofstream os("arecords.txt",std::ofstream::app);
		os << message << ' ' << instruction_kind << endl;
	}
		break;
	}
}