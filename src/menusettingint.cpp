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
#include "menusettingint.h"
#include "esoteric.h"
#include "utilities.h"

#include <sstream>

using std::string;
using std::stringstream;
using fastdelegate::MakeDelegate;

MenuSettingInt::MenuSettingInt(Esoteric *app, const string &title, const string &description, int *value, int def, int min, int max, int delta)
	: MenuSetting(app, title, description) {

	_value = value;
	originalValue = *value;
	this->def = def;
	this->min = min;
	this->max = max;
	this->delta = delta;
	setValue(evalIntConf(value, def, min, max));

	//Delegates
	ButtonAction actionInc = MakeDelegate(this, &MenuSettingInt::inc);
	ButtonAction actionDec = MakeDelegate(this, &MenuSettingInt::dec);

	btn = new IconButton(app, "skin:imgs/buttons/select.png", app->tr["Default"]);
	btn->setAction(MakeDelegate(this, &MenuSettingInt::setDefault));
	buttonBox.add(btn);

	btn = new IconButton(app, "skin:imgs/buttons/left.png");
	btn->setAction(actionDec);
	buttonBox.add(btn);

	btn = new IconButton(app, "skin:imgs/buttons/right.png", app->tr["Change"]);
	btn->setAction(actionInc);
	buttonBox.add(btn);
}

void MenuSettingInt::draw(int y) {
	MenuSetting::draw(y);
	app->screen->write( app->font, strvalue, 155, y+app->font->getHalfHeight(), VAlignMiddle );
}

uint32_t MenuSettingInt::manageInput() {
	if ( app->input[LEFT ] ) dec();
	if ( app->input[RIGHT] ) inc();
	if ( app->input[DEC] ) setValue(value() - 10 * delta);
	if ( app->input[INC] ) setValue(value() + 10 * delta);
	if ( app->input[MENU] ) setDefault();
	return 0; // SD_NO_ACTION
}

void MenuSettingInt::inc() {
	setValue(value() + delta);
}

void MenuSettingInt::dec() {
	setValue(value() - delta);
}

void MenuSettingInt::setValue(int value) {
	*_value = constrain(value,min,max);
	stringstream ss;
	ss << *_value;
	strvalue = "";
	ss >> strvalue;
}

void MenuSettingInt::setDefault() {
	setValue(def);
}

int MenuSettingInt::value() {
	return *_value;
}

bool MenuSettingInt::edited() {
	return originalValue != value();
}
