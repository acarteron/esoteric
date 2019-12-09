#ifndef PROGRESSBAR_H_
#define PROGRESSBAR_H_

#include <string>
#include "esoteric.h"

class ProgressBar {

private:
	std::string title_, detail_, icon;
	int bgalpha, boxPadding, boxHeight, titleWidth;
	Esoteric *app;
	std::string formatText(const std::string & text);
	void free();
    bool finished_;
    SDL_TimerID timerId_;
    int interval_ = 100;

public:
	ProgressBar(Esoteric *app, const std::string &title, const std::string &icon="");
    ~ProgressBar();
	void exec();
    static uint32_t render(uint32_t interval, void * data);
	void setBgAlpha(bool bgalpha);
    void finished(int millis = 0);
	void updateDetail(const std::string &text);
};

#endif /*PROGRESSBAR_H_*/
