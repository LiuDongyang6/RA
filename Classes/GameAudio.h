#ifndef __UTIL_GAMEAUDIO_H__
#define __UTIL_GAMEAUDIO_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

class GameAudio
{
private:

	bool isEffectOn;               //音效是否打开
	bool isBgmOn;                  //音乐是否打开

	std::string currentBgm;
	CocosDenshion::SimpleAudioEngine * audioEngine;       //音频引擎

public:

    static GameAudio * getInstance();

	void preloadBgm(const char * pszFilePath);

	void preloadEffect(const char * pszFilePath);

    void playEffect(const char * name);

    void stopEffect();

    void setEffectOn(bool val);

    bool getEffectOn();

    void playBgm(const char * name);

    void stopBgm();

    void setBgmOn(bool val);

    bool getBgmOn();
private:
	//构造函数
	GameAudio();

};


#endif