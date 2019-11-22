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
#include <fstream>
#include <sstream>
#include "link.h"
#include "menu.h"
#include "selector.h"
#include "debug.h"
#include "fonthelper.h"

Link::Link(GMenu2X *gmenu2x_, LinkAction action)
	: Button(gmenu2x_->ts, true)
	, gmenu2x(gmenu2x_)
{
	this->action = action;
	edited = false;
	iconPath = gmenu2x->skin->getSkinFilePath("icons/generic.png");
	padding = 4;

}

void Link::run() {
	this->action();
}

const string &Link::getTitle() {
	return title;
}

void Link::setTitle(const string &title) {
	this->title = title;
	edited = true;
}

const string &Link::getDescription() {
	return description;
}

void Link::setDescription(const string &description) {
	this->description = description;
	edited = true;
}

const string &Link::getIcon() {
	return icon;
}

void Link::setIcon(const string &icon) {
	this->icon = icon;

	if (icon.compare(0, 5, "skin:") == 0)
		this->iconPath = gmenu2x->skin->getSkinFilePath(icon.substr(5, string::npos));
	else
		this->iconPath = icon;

	edited = true;
}

const string &Link::searchIcon() {
	if (!gmenu2x->skin->getSkinFilePath(iconPath).empty()) {
		iconPath = gmenu2x->skin->getSkinFilePath(iconPath);
	}	else if (!fileExists(iconPath)) {
		iconPath = gmenu2x->skin->getSkinFilePath("icons/generic.png");
	} else
		iconPath = gmenu2x->skin->getSkinFilePath("icons/generic.png");
	return iconPath;
}

const string &Link::getIconPath() {
	if (iconPath.empty()) searchIcon();
	return iconPath;
}

void Link::setIconPath(const string &icon) {
	if (fileExists(icon))
		iconPath = icon;
	else
		iconPath = gmenu2x->skin->getSkinFilePath("icons/generic.png");
}
