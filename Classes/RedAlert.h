#ifndef __RED_ALERT_H__
#define __RED_ALERT_H__
#include"RAObject\RABuilding.h"
#include"RAUtility.h"
#include"RAObject\RASoldier.h"
class RedAlert
{
public:
	static RedAlert* getinstance();
	void initAll();
private:
	static RedAlert* instance_;
	RedAlert() {}
};


#endif // !__RED_ALERT_H__


