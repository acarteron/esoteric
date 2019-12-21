#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <SDL.h>
#include "hw/ihardware.h"

class ScreenManager {
public:
	ScreenManager(IHardware *hw_);
	~ScreenManager();
	void resetScreenTimer();
	void setScreenTimeout(unsigned int seconds);
	bool isAsleep();

private:
	void addScreenTimer();
	void removeScreenTimer();
	void setScreenBlanking(bool state);
	void enableScreen();
	void disableScreen();

	static ScreenManager *instance;
	IHardware *hw;
	bool screenState;
	unsigned int screenTimeout;
	unsigned int timeout_startms;
	SDL_TimerID screenTimer;

	bool asleep;
	friend Uint32 screenTimerCallback(Uint32 timeout, void *d);
};

#endif
