/***************************************************************************
 *   Copyright (C) 2006 by Massimiliano Torromeo   *
 *   massimiliano.torromeo@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "menusettingdatetime.h"
#include "esoteric.h"

#include <sstream>
#include <iomanip>

MenuSettingDateTime::MenuSettingDateTime(Esoteric *app, const std::string &title, const std::string &description, std::string *value)
	: MenuSetting(app, title, description) {
	_value = value;
	originalValue = *value;

	selPart = 0;

	sscanf(_value->c_str(), "%d-%d-%d %d:%d", &iyear, &imonth, &iday, &ihour, &iminute);

	this->setYear(iyear);
	this->setMonth(imonth);
	this->setDay(iday);
	this->setHour(ihour);
	this->setMinute(iminute);

	btn = new IconButton(app, "skin:imgs/buttons/left.png");
	btn->setAction(fastdelegate::MakeDelegate(this, &MenuSettingDateTime::leftComponent));
	buttonBox.add(btn);

	btn = new IconButton(app, "skin:imgs/buttons/right.png", app->tr["Component"]);
	btn->setAction(fastdelegate::MakeDelegate(this, &MenuSettingDateTime::rightComponent));
	buttonBox.add(btn);

	btn = new IconButton(app, "skin:imgs/buttons/y.png", app->tr["Decrease"]);
	btn->setAction(fastdelegate::MakeDelegate(this, &MenuSettingDateTime::dec));
	buttonBox.add(btn);

	btn = new IconButton(app, "skin:imgs/buttons/x.png", app->tr["Increase"]);
	btn->setAction(fastdelegate::MakeDelegate(this, &MenuSettingDateTime::inc));
	buttonBox.add(btn);
}

void MenuSettingDateTime::draw(int y) {
	this->y = y;
	MenuSetting::draw(y);
	app->screen->write( app->font, year + "-" + month + "-" + day + " " + hour + ":" + minute, 155, y+app->font->getHalfHeight(), VAlignMiddle );
}

uint32_t MenuSettingDateTime::manageInput() {
	if ((*app->inputManager)[INC]) inc();
	if ((*app->inputManager)[DEC]) dec();
	if ((*app->inputManager)[LEFT]) leftComponent();
	if ((*app->inputManager)[RIGHT]) rightComponent();
	return 0; // SD_NO_ACTION
}

void MenuSettingDateTime::dec() {
	setSelPart(getSelPart()-1);
}

void MenuSettingDateTime::inc() {
	setSelPart(getSelPart()+1);
}

void MenuSettingDateTime::leftComponent() {
	selPart = constrain(selPart-1,0,4);
}

void MenuSettingDateTime::rightComponent() {
	selPart = constrain(selPart + 1,0,4);
}

void MenuSettingDateTime::setYear(int16_t i) {
	iyear = constrain(i, 1970, 2100);
	std::stringstream ss;
	ss << iyear;
	ss >> year;
}

void MenuSettingDateTime::setMonth(int16_t i) {
	imonth = i;
	if (i < 1) imonth = 12;
	else if (i > 12) imonth = 0;
	std::stringstream ss;
	ss << std::setw(2) << std::setfill('0') << imonth;
	ss >> month;
}

void MenuSettingDateTime::setDay(int16_t i) {
	iday = i;
	if (i < 1) iday = 31;
	else if (i > 31) iday = 0;
	std::stringstream ss;
	ss << std::setw(2) << std::setfill('0') << iday;
	ss >> day;
}

void MenuSettingDateTime::setHour(int16_t i) {
	ihour = i;
	if (i < 0) ihour = 23;
	else if (i > 23) ihour = 0;
	std::stringstream ss;
	ss << std::setw(2) << std::setfill('0') << ihour;
	ss >> hour;
}

void MenuSettingDateTime::setMinute(int16_t i) {
	iminute = i;
	if (i < 0) iminute = 59;
	else if (i > 59) iminute = 0;
	std::stringstream ss;
	ss << std::setw(2) << std::setfill('0') << iminute;
	ss >> minute;
}

void MenuSettingDateTime::setSelPart(uint16_t i) {
	switch (selPart) {
		case 1: setMonth(i); break;
		case 2: setDay(i); break;
		case 3: setHour(i); break;
		case 4: setMinute(i); break;
		default: setYear(i); break;
	}

	*_value = year + "-" + month + "-" + day + " " + hour + ":" + minute;
}

std::string MenuSettingDateTime::value() {
	return *_value;
}

uint16_t MenuSettingDateTime::getSelPart() {
	switch (selPart) {
		default: case 0: return iyear;
		case 1: return imonth;
		case 2: return iday;
		case 3: return ihour;
		case 4: return iminute;
	}
}

void MenuSettingDateTime::drawSelected(int y) {
	int x = 155, w = 40;
	switch (selPart) {
		case 1: x += app->font->getTextWidth(year + "-"); w = app->font->getTextWidth(month); break;
		case 2: x += app->font->getTextWidth(year + "-" + month + "-"); w = app->font->getTextWidth(day); break;
		case 3: x += app->font->getTextWidth(year + "-" + month + "-" + day + " "); w = app->font->getTextWidth(hour); break;
		case 4: x += app->font->getTextWidth(year + "-" + month + "-" + day + " " + hour + ":"); w = app->font->getTextWidth(minute); break;
		default: w = app->font->getTextWidth(year); break;
	}
	app->screen->box( x - 2, y, w + 3, app->font->getHeight() + 1, app->skin->colours.selectionBackground);
	app->screen->rectangle( x - 2, y, w + 3, app->font->getHeight() + 1, 0,0,0,255 );

	MenuSetting::drawSelected(y);
}

bool MenuSettingDateTime::edited() {
	return originalValue != *_value;
}
