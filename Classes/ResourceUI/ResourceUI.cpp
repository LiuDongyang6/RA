#include"ResourceUI.h"

TextAtlas* RAResourceUI::capital_;
TextAtlas* RAResourceUI::power_;
Widget* RAResourceUI::ResourceUI;
void RAResourceUI::init()
{
	ResourceUI = GUIReader::getInstance()->widgetFromJsonFile("ResourceUI/NewUi_1.ExportJson");
	ResourceUI->setPosition(Point(-3, 600));
	capital_ = (TextAtlas*)Helper::seekWidgetByTag(ResourceUI, 1);
	capital_->setString(std::to_string(RAPlayer::getCapital()));
	power_ = (TextAtlas*)Helper::seekWidgetByTag(ResourceUI, 3);
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