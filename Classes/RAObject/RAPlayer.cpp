#include"RAPlayer.h"
#include"RASoldier.h"
#include"ResourceUI/ResourceUI.h"
int RAPlayer::power_;
int RAPlayer::capital_;
int RAPlayer::unit_counter_ = 1;
Widget* RAPlayer::current_UI_ = 0;
std::vector<RASoldier*> RAPlayer::selected_soldiers_;
std::list<RASoldier*> RAPlayer::all_soldiers_;
std::list<RAObject*> RAPlayer::enemies;
std::unordered_map<int, RAObject*> RAPlayer::master_table_;
int RAPlayer::edge;

void RAPlayer::consumeCapital(int quantity)
{
	capital_ -= quantity;
	NotificationCenter::getInstance()->postNotification("RESOURCE_CHANGE",NULL);
	RAResourceUI::changeCapital(capital_);
}
void RAPlayer::consumePower(int quantity)
{
	power_ -= quantity;
	NotificationCenter::getInstance()->postNotification("RESOURCE_CHANGE",NULL);
	RAResourceUI::changePower(power_);
}
void RAPlayer::resumeCapital(int quantity)
{
	consumeCapital(-1 * quantity);
}
void RAPlayer::resumePower(int quantity)
{
	consumePower(-1 * quantity);
}