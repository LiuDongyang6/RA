#ifndef _ENDINGSCENE_H_
#define _ENDINGSCENE_H_

#include "cocos2d.h"

class EndingScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene(int resultflag);

	virtual bool init();

	void menuCloseCallback(cocos2d::Ref* pSender);

	void menuHelloCallback(cocos2d::Ref *pSender);

	CREATE_FUNC(EndingScene);
};


#endif // !_ENDINGSCENE_H_

