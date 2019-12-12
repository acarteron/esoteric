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

#ifndef _ESOTERIC_H_
#define _ESOTERIC_H_

#include "surfacecollection.h"
#include "iconbutton.h"
#include "translator.h"
#include "FastDelegate.h"
#include "utilities.h"
#include "touchscreen.h"
#include "inputmanager.h"
#include "screenmanager.h"
#include "surface.h"
#include "fonthelper.h"
#include "skin.h"
#include "config.h"
#include "ui.h"
#include "hwfactory.h"

#ifdef HAVE_LIBOPK
#include "opkcache.h"
#endif

#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <tr1/unordered_map>

using std::string;
using std::vector;
using fastdelegate::FastDelegate0;

typedef FastDelegate0<> MenuAction;
typedef std::tr1::unordered_map<string, string, std::hash<string> > ConfStrHash;
typedef std::tr1::unordered_map<string, int, std::hash<string> > ConfIntHash;

struct MenuOption {
	string text;
	MenuAction action;
};

class Menu;
class UI;

#ifdef HAVE_LIBOPK
class OpkCache;
#endif

class Esoteric {
private:

	bool needsInstalling;
	std::string exe_path; //!< Contains the working directory of Esoteric
	std::string writeable_path; // Contains the assets path

	/*!
	Starts the scanning of the nand and sd filesystems, searching for gpe and gpu files and creating the links in 2 dedicated sections.
	*/
	void linkScanner();
	/*!
	Performs the actual scan in the given path and populates the files vector with the results. The creation of the links is not performed here.
	@see scanner
	*/
	// void scanPath(string path, vector<string> *files);

	/*!
	Displays a selector and launches the specified executable file
	*/
	void explorer();

	std::string lastSelectorDir;
	int lastSelectorElement;
	void readTmp();

	void initFont();
	void showManual();

	void mountSdDialog();
	void umountSdDialog();

	void doInstall();
	void doUnInstall();
	void doUpgrade();
	bool doInitialSetup();

	void deviceMenu();
	void skinMenu();
	uint32_t onChangeSkin();

public:
	Esoteric();
	~Esoteric();
	void quit();
	static void quit_all(int err);
	void releaseScreen();

	bool f200 = true;
	uint32_t linkWidth, linkHeight, linkSpacing = 4;
	SDL_Rect listRect, linksRect, sectionBarRect;

	const std::string &getExePath();
	const std::string &getWriteablePath();
	const std::string &getReadablePath();

	ScreenManager screenManager;
	InputManager input;
	Touchscreen ts;

	SurfaceCollection *sc;
	Translator tr;
	Surface *screen, *bg;
	FontHelper *font = NULL, *fontTitle = NULL, *fontSectionTitle = NULL; 

	//Status functions
	void main();
	void settings();
	void restartDialog(bool showDialog = false);
	void poweroffDialog();
	void resetSettings();
	void cpuSettings();


	void skinColors();
	void initLayout();
	void initMenu();

	void about();
	void viewLog();
	//void batteryLogger();
	void performanceMenu();
	void contextMenu();
	void changeWallpaper();

	void setInputSpeed();

	void writeConfig();
	void writeSkinConfig();
	void writeTmp(int selelem=-1, const std::string &selectordir = "");

	void addLink();
	void editLink();
	void deleteLink();
	void addSection();
	void hideSection();
	void renameSection();
	void deleteSection();

	void setWallpaper(const std::string &wallpaper = "");
	void updateAppCache(std::function<void(std::string)> callback = nullptr);
	void cacheChanged(const DesktopFile & file, const bool & added);

	Menu* menu;
	Skin* skin;
	Config* config;
	UI* ui;
	IHardware * hw;

	#ifdef HAVE_LIBOPK
	OpkCache* cache;
	#endif

};

#endif
