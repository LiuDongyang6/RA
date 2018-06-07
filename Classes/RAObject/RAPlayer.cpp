#include"RAPlayer.h"
#include"RASoldier.h"
int RAPlayer::power_ = 200;
int RAPlayer::capital_ = 10000;
int RAPlayer::unit_counter_ = 1;
Widget* RAPlayer::current_UI_ = 0;
std::unordered_set<RASoldier*> RAPlayer::selected_soldiers_;
std::unordered_set<RASoldier*> RAPlayer::all_soldiers_;

void RAPlayer::consumeCapital(int quantity)
{
	capital_ -= quantity;
	NotificationCenter::getInstance()->postNotification("RESOURCE_CHANGE",NULL);
}
void RAPlayer::consumePower(int quantity)
{
	power_ -= quantity;
	NotificationCenter::getInstance()->postNotification("RESOURCE_CHANGE",NULL);
}
void RAPlayer::resumeCapital(int quantity)
{
	consumeCapital(-1 * quantity);
}
void RAPlayer::resumePower(int quantity)
{
	consumePower(-1 * quantity);
}