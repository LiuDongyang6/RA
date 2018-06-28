#ifndef __RED_ALERT_H__
#define __RED_ALERT_H__
#include"RAObject\RABuilding.h"
#include"RAUtility.h"
#include"RAObject\RASoldier.h"
class RedAlert
{
public:
	static RedAlert* getInstance();
	void selectedSoldiersMove(Touch* touch);
	void initAll(int num);
	void initCreateWiki();
	bool onTouchBegan(Touch* touch, Event* event);
	bool OilOnTouBegan(Touch* touch, Event* event);
	void onTouchMoved(Touch* touch, Event* event);
	void onTouchEnded(Touch* touch, Event* event);
	static void HostileObjectAppear(int id,Point location,int count);
	static void catcher (std::string message);
	static void victory();
	static void defeat();
private:
	static RedAlert* instance_;
	Layer* selectBox;
	RedAlert() {}
};


#endif // !__RED_ALERT_H__


