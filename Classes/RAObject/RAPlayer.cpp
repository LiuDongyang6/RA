#include"RAPlayer.h"
int RAPlayer::power_ = 200;
int RAPlayer::capital_ = 10000;
Widget* RAPlayer::current_UI_ = 0;


void RAPlayer::consumeCapital(int quantity)
{
	NotificationCenter::getInstance()->postNotification("RESOURCE_CHANGE",NULL);
	capital_ -= quantity;
}
void RAPlayer::consumePower(int quantity)
{
	NotificationCenter::getInstance()->postNotification("RESOURCE_CHANGE",NULL);
	power_-=quantity;
}
void RAPlayer::resumeCapital(int quantity)
{
	consumeCapital(-1 * quantity);
}
void RAPlayer::resumePower(int quantity)
{
	consumePower(-1 * quantity);
}