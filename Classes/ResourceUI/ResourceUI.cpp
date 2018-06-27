#include"ResourceUI.h"

TextAtlas* RAResourceUI::capital_;
TextAtlas* RAResourceUI::power_;
Widget* RAResourceUI::ResourceUI;
void RAResourceUI::init()
{
	ResourceUI = GUIReader::getInstance()->widgetFromJsonFile("ResourceUI/NewUi_1.ExportJson");
	ResourceUI->setPosition(Point(1410,780));
	ResourceUI->setAnchorPoint(Vec2(0.5, 0.5));
	ResourceUI->setScale(1.5);
	capital_ = (TextAtlas*)Helper::seekWidgetByTag(ResourceUI, 3);
	capital_->setString(std::to_string(RAPlayer::getCapital()));
	power_ = (TextAtlas*)Helper::seekWidgetByTag(ResourceUI, 1);
	power_->setString(std::to_string(RAPlayer::getPower()));
}
void RAResourceUI::changeCapital(int capital)
{
	capital_->setString(std::to_string(capital));
}
void RAResourceUI::changePower(int power)
{
	power_->setString(std::to_string(power));
}