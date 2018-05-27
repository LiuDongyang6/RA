#include"RedAlert.h"
RedAlert* RedAlert::instance_ = NULL;
RedAlert* RedAlert::getinstance()
{
	if (instance_ == NULL)
		instance_ = new RedAlert();
	return instance_;
}
void RedAlert::initAll()
{
	RAUtility::RAObjectProperty=FileUtils::getInstance()->getStringFromFile("RAObjectProperty.json");
	//initial create_function wiki
	auto &wiki = RAConstructButton::CreateWiki;
	wiki.insert({ 0,RABase::create });
	wiki.insert({{1,RAPowerStation::create } });

	auto framecache = SpriteFrameCache::getInstance();
	framecache->addSpriteFramesWithFile("Buildings.plist");
	//
	RAMap::init();
}