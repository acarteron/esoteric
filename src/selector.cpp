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

//for browsing the filesystem
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>

#include "messagebox.h"
#include "linkapp.h"
#include "selector.h"
#include "filelister.h"
#include "debug.h"
#include "stringutils.h"

const std::string PREVIEWS_DIR = ".previews";

Selector::Selector(Esoteric *app, LinkApp *link, const std::string &selectorDir) : Dialog(app) {
	TRACE("enter : %s", selectorDir.c_str());
	this->link = link;
	loadAliases();
	selRow = 0;
	if (selectorDir.empty())
		dir = link->getSelectorDir();
	else
		dir = selectorDir;

}

void Selector::resolve(int selection) {
	TRACE("enter - selection : %i", selection);

	// just set up the file by id and bail back
	FileLister fl(dir, link->getSelectorBrowser());
	if (!this->link->getSelectorBrowser()) {
		fl.addExclude("..");
	}
	std::vector<std::string> titles;
	this->prepare(&fl, &titles);
	int selected = constrain(selection, 0, fl.size() - 1);
	file = fl[selected];

	TRACE("exit - file : '%s'", file.c_str());
}

int Selector::exec(int startSelection) {
	TRACE("enter - startSelection : %i", startSelection);

	// does the link have a backdrop that takes precendence over the background
	if ((*app->sc)[link->getBackdrop()] != NULL) 
		(*app->sc)[link->getBackdrop()]->blit(this->bg,0,0);

	bool close = false, result = true, inputAction = false;
	std::vector<std::string> screens, titles;

	TRACE("starting selector");
	FileLister fl(dir, link->getSelectorBrowser());
	if (!this->link->getSelectorBrowser()) {
		fl.addExclude("..");
	}

	// do we have screen shots?
	// if we do, they will live under this path, or this dir/screenshots
	this->screendir = link->getSelectorScreens();
	this->tickStart = SDL_GetTicks();
	this->animation = 0;
	this->firstElement = 0;

	uint32_t i, iY, padding = 6;
	uint32_t rowHeight = app->font->getHeight() + 1;
	uint32_t halfRowHeight = (rowHeight / 2);

	this->numRows = ((app->listRect.h - 2) / rowHeight) - 1;

	drawTopBar(this->bg, link->getTitle(), link->getDescription(), link->getIconPath());
	drawBottomBar(this->bg);

	this->bg->box(app->listRect, app->skin->colours.listBackground);

	app->ui->drawButton(this->bg, "a", app->tr["Select"],
	app->ui->drawButton(this->bg, "b", app->tr["Exit"], 
	app->ui->drawButton(this->bg, "x", app->tr["Favourite"])));

	this->prepare(&fl, &titles, &screens);
	int selected = constrain(startSelection, 0, fl.size() - 1);

	// moved surfaces out to prevent reloading on loop
	Surface *iconGoUp = app->sc->skinRes("imgs/go-up.png");
	Surface *iconFolder = app->sc->skinRes("imgs/folder.png");
	Surface *iconFile = app->sc->skinRes("imgs/file.png");
	app->sc->defaultAlpha = false;

	// kick off the chooser loop
	while (!close) {
		int currentUniversalIndex = selected;
		int currentFileIndex = selected - fl.dirCount();

		// bang the background in
		this->bg->blit(app->screen, 0, 0);

		if (!fl.size()) {
			MessageBox mb(app, app->tr["This directory is empty"]);
			mb.setAutoHide(1);
			mb.setBgAlpha(0);
			mb.exec();
		} else {
			//Selection wrap test
			if (selected >= firstElement + numRows) firstElement = selected - numRows;
			if (selected < firstElement) firstElement = selected;

			if  (-1 == app->skin->previewWidth  && selected >= 0) {
				// we are doing a full background thing
				std::string screenPath = screens.at(selected);
				if (!screenPath.empty()) {
					// line it up with the text
					SDL_Rect bgArea {
						0, 
						app->skin->menuTitleBarHeight, 
						app->getScreenWidth(), 
						app->getScreenHeight() - app->skin->menuInfoBarHeight - app->skin->menuTitleBarHeight };

					// only stretch it once if possible
					if (!app->sc->exists(screenPath)) {
						TRACE("1st load - stretching screen path : %s", screenPath.c_str());
						(*app->sc)[screenPath]->softStretch(
							app->getScreenWidth(), 
							app->getScreenHeight() - app->skin->menuInfoBarHeight - app->skin->menuTitleBarHeight, 
							true, 
							true);
					}

					(*app->sc)[screenPath]->blit(
						app->screen, 
						bgArea, 
						HAlignCenter | VAlignMiddle, 
						255);
				}
			}
			// draw files & Directories
			iY = app->listRect.y + 1;
			for (i = firstElement; i < fl.size() && i <= firstElement + numRows; i++, iY += rowHeight) {
				if (i == selected) {
					// selected item highlight
					app->screen->box(
						app->listRect.x, 
						iY, 
						app->listRect.w, 
						rowHeight, 
						app->skin->colours.selectionBackground);
				}
				if (fl.isDirectory(i)) {
					if (fl[i] == "..")
						iconGoUp->blit(
							app->screen, 
							app->listRect.x + 10, 
							iY + halfRowHeight, 
							HAlignCenter | VAlignMiddle);
					else
						iconFolder->blit(
							app->screen, 
							app->listRect.x + 10, 
							iY + halfRowHeight, 
							HAlignCenter | VAlignMiddle);
				} else {
					iconFile->blit(
						app->screen, 
						app->listRect.x + 10, 
						iY + halfRowHeight, 
						HAlignCenter | VAlignMiddle);
				}
				app->screen->write(
					app->font, 
					titles[i], 
					app->listRect.x + 21, 
					iY + halfRowHeight, 
					VAlignMiddle);
			}

			// screenshot logic
			if (app->skin->previewWidth > 0 && selected >= 0) {
				// we're in the files section and there's some art to deal with
				if (!screens[selected].empty()) {

					std::string screenPath = screens.at(selected);
					if (!app->sc->exists(screenPath)) {
						TRACE("1st load windowed - stretching screen path : %s", screenPath.c_str());
						(*app->sc)[screenPath]->softStretch(
							app->skin->previewWidth - 3 * padding, 
							app->listRect.h - 3 * padding, 
							true, 
							true);
					}

					app->screen->box(
						app->getScreenWidth() - animation, 
						app->listRect.y, 
						app->skin->previewWidth, 
						app->listRect.h, 
						app->skin->colours.titleBarBackground);

					(*app->sc)[screens[selected]]->blit(
						app->screen, 
						{	app->getScreenWidth() - animation + padding, 
							app->listRect.y + padding, 
							app->skin->previewWidth - 2 * padding, 
							app->listRect.h - 2 * padding
						}, 
						HAlignCenter | VAlignMiddle, 
						220);

					if (animation < app->skin->previewWidth) {
						animation = UI::intTransition(0, app->skin->previewWidth, tickStart, 110);
						app->screen->flip();
						continue;
					}

				} else {
					if (animation > 0) {
						// we only come in here if we had a screenshot before
						// and we need to clean it up
						app->screen->box(
							app->getScreenWidth() - animation, 
							app->listRect.y, 
							app->skin->previewWidth, 
							app->listRect.h, 
							app->skin->colours.titleBarBackground);
			
						animation = app->skin->previewWidth - UI::intTransition(0, app->skin->previewWidth, tickStart, 80);
						app->screen->flip();
						continue;
					}
				}
			}
			app->screen->clearClipRect();
			app->ui->drawScrollBar(numRows, fl.size(), firstElement, app->listRect);
			app->screen->flip();
		}

		// handle input
		do {
			inputAction = app->inputManager->update();
			if (inputAction) this->tickStart = SDL_GetTicks();

			if ( (*app->inputManager)[UP] ) {
				selected -= 1;
				if (selected < 0) selected = fl.size() - 1;
			} else if ( (*app->inputManager)[DOWN] ) {
				selected += 1;
				if (selected >= fl.size()) selected = 0;
			} else if ( (*app->inputManager)[LEFT] ) {
				selected -= numRows;
				if (selected < 0) selected = 0;
			} else if ( (*app->inputManager)[RIGHT] ) {
				selected += numRows;
				if (selected >= fl.size()) selected = fl.size() - 1;
			} else if ((*app->inputManager)[SECTION_PREV]) {
				selected = 0;
			} else if ((*app->inputManager)[SECTION_NEXT]) {
				selected = fl.size() -1;
			} else if ((*app->inputManager)[PAGEUP]) {
				// loop thru the titles collection until first char doesn't match
				char currentStartChar = titles.at(selected)[0];
				int offset = 0;
				bool found = false;
				for(std::vector<std::string>::iterator current = titles.begin() + selected; current != titles.begin(); current--) {
					--offset;
					if (currentStartChar != (*current)[0]) {
						selected += offset + 1;
						found = true;
						break;
					}
				}
				if (!found) selected = fl.size() -1;
			} else if ((*app->inputManager)[PAGEDOWN]) {
				// reverse loop thru the titles collection until first char doesn't match
				char currentStartChar = titles.at(selected)[0];
				int offset = 0;
				bool found = false;
				for(std::vector<std::string>::iterator current = titles.begin() + selected; current != titles.end(); current++) {
					++offset;
					if (currentStartChar != (*current)[0]) {
						selected += offset - 1;
						found = true;
						break;
					}
				}
				if (!found) selected = 0;
			} else if ( (*app->inputManager)[SETTINGS] ) {
				close = true;
				result = false;
			} else if ( (*app->inputManager)[CANCEL] && link->getSelectorBrowser()) {
				std::string::size_type p = this->dir.rfind("/", this->dir.size() - 2);
				this->dir = this->dir.substr(0, p + 1);
				selected = 0;
				this->firstElement = 0;
				this->prepare(&fl, &titles, &screens);
			} else if ( (*app->inputManager)[CONFIRM] ) {
				// file selected or dir selected
				if (fl.isFile(selected)) {
					this->file = fl[selected];
					close = true;
				} else {
					std::string localPath = this->dir + fl[selected];
					TRACE("local path is now set to : '%s'", localPath.c_str());
					this->dir = FileUtils::resolvePath(localPath);
					selected = 0;
					this->firstElement = 0;
					this->prepare(&fl, &titles, &screens);
				}
			} else if ( (*app->inputManager)[INC] ) {
				// favourite
				if (fl.isFile(selected)) {
					file = fl[selected];
					TRACE("making favourite : %s", file.c_str());
					std::string backdrop;
					if (!screens[selected - 1].empty()) {
						TRACE("adding screen : '%s'", screens.at(selected - 1).c_str());
						backdrop = screens.at(selected - 1);
					}
					this->link->makeFavourite(this->dir, file, backdrop);
				}
			}
		} while (!inputAction);
	}

	this->app->sc->defaultAlpha = true;
	this->freeScreenshots(&screens);

	TRACE("exit : %i", result ? (int)selected : -1);
	return result ? (int)selected : -1;
}

// checks for screen shots etc
void Selector::prepare(FileLister *fl, std::vector<std::string> *titles, std::vector<std::string> *screens) {
	TRACE("enter");

	if (!FileUtils::dirExists(this->dir)) {
		if (this->link->getSelectorBrowser()) {
			this->dir = FileUtils::firstExistingDir(this->dir);
		} else {
			// we have problems,
			// it's not a real dir and we can't browse dirs
			ERROR("Directory doesn't exist : '%s'", this->dir.c_str());
			return;
		}
	}

	if (this->dir.length() > 0) {
		// force a trailing slash
		if (0 != this->dir.compare(dir.length() -1, 1, "/")) {
			this->dir += "/";
		}
	} else this->dir = "/";
	fl->setPath(this->dir, false);

	TRACE("setting filter");
	std::string filter = link->getSelectorFilter();
	fl->setFilter(filter);
	TRACE("filter : %s", filter.c_str());

	TRACE("calling browse");
	fl->browse();

	TRACE("found %i files and dirs", fl->size());
	this->numDirs = fl->dirCount();
	this->numFiles = fl->fileCount();
	TRACE("number of files: %i", this->numFiles);
	TRACE("number of dirs: %i", this->numDirs);

	if (NULL != screens) {
		TRACE("clearing the screens collection");
		this->freeScreenshots(screens);
		screens->resize(this->numDirs + this->numFiles);
	}
	TRACE("resetting the titles");
	titles->clear();
	titles->resize(this->numDirs + this->numFiles);

	std::string::size_type pos;
	std::string realPath = FileUtils::resolvePath(fl->getPath());
	if (realPath.length() > 0) {
		if (0 != realPath.compare(realPath.length() -1, 1, "/")) {
			realPath += "/";
		}
	} else realPath = "/";
	TRACE("realPath: '%s'", realPath.c_str());

	std::string previewsDir = realPath + PREVIEWS_DIR;
	TRACE("checking for previews dir at : '%s'", previewsDir.c_str());
	bool previewsDirExists = FileUtils::dirExists(previewsDir);
	if (!previewsDirExists) {
		TRACE("didn't find previews dir in current path, trying up one level...");
		previewsDir = FileUtils::resolvePath(realPath + "../") + "/" + PREVIEWS_DIR;
		TRACE("checking for previews dir at : '%s'", previewsDir.c_str());
		previewsDirExists = FileUtils::dirExists(previewsDir);
	}
	TRACE("previews folder exists: %i",  previewsDirExists);

	// put all the dirs into titles first
	for (uint32_t i = 0; i < this->numDirs; i++) {
		std::string dirName = fl->getDirectories()[i];
		titles->at(i) = dirName;
		this->handleImages(realPath, dirName, i, previewsDirExists, previewsDir, titles, screens);
	}

	// then loop thru all the files
	for (uint32_t i = 0; i < this->numFiles; i++) {
		int fileIndex = i + this->numDirs;
		std::string fname = fl->getFiles()[i];
		std::string noext = FileUtils::fileBaseName(fname);

		// and push into titles, via alias lookup
		titles->at(fileIndex) = getAlias(noext, fname);
		this->handleImages(realPath, noext, fileIndex, previewsDirExists, previewsDir, titles, screens);

	}
	TRACE("exit");
}

void Selector::handleImages(std::string realPath, std::string noext, int fileIndex, bool previewsDirExists, std::string previewsDir, std::vector<std::string> *titles, std::vector<std::string> *screens) {
    
	if (NULL == screens) return;

	std::string realdir;
    // are we looking for screen shots
    if (!this->screendir.empty() && 0 != this->app->skin->previewWidth) {
        if (this->screendir[0] == '.') {
            // allow "." as "current directory", therefore, relative paths
            realdir = realPath + this->screendir + "/";
        } else
            realdir = FileUtils::resolvePath(screendir) + "/";

        // INFO("Searching for screen '%s%s.png'", realdir.c_str(), noext.c_str());
        if (FileUtils::fileExists(realdir + noext + ".jpg")) {
            screens->at(fileIndex) = realdir + noext + ".jpg";
        } else if (FileUtils::fileExists(realdir + noext + ".png")) {
            screens->at(fileIndex) = realdir + noext + ".png";
        }
    }
    if (screens->at(fileIndex).empty()) {
        // fallback - always search for filename.png and jpg in a .previews folder
        // either inside the current path, or one level higher
        if (previewsDirExists && 0 != app->skin->previewWidth) {
            if (FileUtils::fileExists(previewsDir + "/" + noext + ".png"))
                screens->at(fileIndex) = previewsDir + "/" + noext + ".png";
            else if (FileUtils::fileExists(previewsDir + "/" + noext + ".jpg"))
                screens->at(fileIndex) = previewsDir + "/" + noext + ".jpg";
        }
    }
}

void Selector::freeScreenshots(std::vector<std::string> *screens) {
	for (uint32_t i = 0; i < screens->size(); i++) {
		if (!screens->at(i).empty())
			app->sc->del(screens->at(i));
	}
	screens->clear();
}

void Selector::loadAliases() {
	TRACE("enter");
	aliases.clear();
	std::string aliasFile = link->getAliasFile();
	if (!aliasFile.empty() && FileUtils::fileExists(aliasFile)) {
		TRACE("alias file found at : %s", aliasFile.c_str());
		std::string line;
		std::ifstream infile (aliasFile.c_str(), std::ios_base::in);
		while (getline(infile, line, '\n')) {
			std::string::size_type position = line.find("=");
			std::string name = StringUtils::trim(line.substr(0,position));
			std::string value = StringUtils::trim(line.substr(position+1));
			aliases[name] = value;
		}
		infile.close();
	}
	TRACE("exit : loaded %zu aliases", aliases.size());
}

std::string Selector::getAlias(const std::string &key, const std::string &fname) {
	//TRACE("enter");
	if (aliases.empty()) return fname;
	std::unordered_map<std::string, std::string>::iterator i = aliases.find(key);
	if (i == aliases.end())
		return fname;
	else
		return i->second;
}
