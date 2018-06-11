#ifndef __RA_HOSTILE_OBJECT_H__
#define __RA_HOSTILE_OBJECT_H__
#include"RAObject.h"
USING_NS_CC;

class RAHostileObject : public RAObject
{
public:
	RAHostileObject(int id) :RAObject(id) {};
	static RAHostileObject* Appear(int id, Point location);
};
#endif // !__RA_HOSTILE_OBJECT_H__
