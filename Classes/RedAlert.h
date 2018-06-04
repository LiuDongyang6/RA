#ifndef __RED_ALERT_H__
#define __RED_ALERT_H__
#include"RAObject\RABuilding.h"
#include"RAUtility.h"
#include"RAObject\RASoldier.h"
#include"RAObject\RAHostileObject.h"
class RedAlert
{
public:
	static RedAlert* getInstance();
	void selectedSoldiersMove(Touch* touch);
	void initAll();
	bool onTouchBegan(Touch* touch, Event* event);
	void onTouchMoved(Touch* touch, Event* event);
	void onTouchEnded(Touch* touch, Event* event);
private:
	static RedAlert* instance_;
	Layer* selectBox;
	RedAlert() {}
};


#endif // !__RED_ALERT_H__


