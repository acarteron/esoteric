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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "messagebox.h"
#include "debug.h"
#include "utilities.h"

using namespace std;

MessageBox::MessageBox(Esoteric *app, const string &text, const string &icon) {
	this->app = app;
	this->text = text;
	this->icon = icon;
	this->autohide = 0;
	this->bgalpha = 200;

	buttons.resize(19);
	buttonLabels.resize(19);
	buttonPositions.resize(19);
	for (uint32_t x = 0; x < buttons.size(); x++) {
		buttons[x] = "";
		buttonLabels[x] = "";
		buttonPositions[x].h = app->font->getHeight();
	}

	//Default enabled button
	buttons[CONFIRM] = "OK";

	//Default labels
	buttonLabels[UP] = "up";
	buttonLabels[DOWN] = "down";
	buttonLabels[LEFT] = "left";
	buttonLabels[RIGHT] = "right";
	buttonLabels[MODIFIER] = "a";

	buttonLabels[CONFIRM] = "a";
	buttonLabels[CANCEL] = "b";
	buttonLabels[MANUAL] = "y";
	buttonLabels[DEC] = "x";
	buttonLabels[INC] = "y";
	buttonLabels[SECTION_PREV] = "l";
	buttonLabels[SECTION_NEXT] = "r";
	buttonLabels[PAGEUP] = "l";
	buttonLabels[PAGEDOWN] = "r";
	buttonLabels[SETTINGS] = "start";
	buttonLabels[MENU] = "select";
	buttonLabels[VOLUP] = "vol+";
	buttonLabels[VOLDOWN] = "vol-";
}

void MessageBox::setButton(int action, const string &btn) {
	buttons[action] = btn;
}

void MessageBox::setAutoHide(int autohide) {
	this->autohide = autohide;
}

void MessageBox::setBgAlpha(int bgalpha) {
	this->bgalpha = bgalpha;
}

string MessageBox::formatText(int box_w_padding, int buttonWidth) {
	int wrap_size = ((app->config->resolutionX() - (box_w_padding / 2)) / app->font->getSize() + 15);
	TRACE("initial wrap size : %i", wrap_size);
	if (wrap_size < buttonWidth) {
		wrap_size = buttonWidth;
	}
	TRACE("final wrap size : %i", wrap_size);

	string wrappedText = splitInLines(this->text, wrap_size);
	TRACE("wrap text : %s", wrappedText.c_str());
	return wrappedText;
}

// can oly be called after creating a message box that has a negative value autohide
void MessageBox::fadeOut(int delay) {
	if (this->autohide >= 0)
		return;
	SDL_Delay(delay);
}

int MessageBox::exec() {
	TRACE("enter");
	int result = -1;

	//Darken background
	app->screen->box(
		(SDL_Rect){ 0, 0, app->config->resolutionX(), app->config->resolutionY() }, 
		(RGBAColor){ 0, 0, 0, bgalpha }
	);
	TRACE("resx : %i", app->config->resolutionX());
	TRACE("text width : %i, size: %i", app->font->getTextWidth(text), app->font->getSize());

	int box_w_padding = 24 + ((*app->sc)[icon] != NULL ? 37 : 0);

	// let's see how big our buttons add up to
	int buttonWidth = 0;
	for (uint32_t i = 0; i < buttons.size(); i++) {
		if (!buttons[i].empty()) {
			TRACE("button width being added for : %s", buttonLabels[i].c_str());
			buttonWidth += buttonLabels[i].length() + 2;
		}
	}
	TRACE("button width : %i", buttonWidth);

	string wrappedText = formatText(box_w_padding, buttonWidth);

	int textWidthPx = app->font->getTextWidth(wrappedText);
	if (textWidthPx + box_w_padding > app->config->resolutionX()) {
		textWidthPx = app->config->resolutionX(); 
	}

	SDL_Rect box;
	box.h = app->font->getTextHeight(wrappedText) * app->font->getHeight() + app->font->getHeight();
	if ((*app->sc)[icon] != NULL && box.h < 40) {
		box.h = 48;
	}
	box.w = textWidthPx + box_w_padding;
	box.x = app->config->halfX() - box.w / 2 - 2;
	box.y = app->config->halfY() - box.h / 2 - 2;

	//outer box
	app->screen->box(box, app->skin->colours.msgBoxBackground);
	
	//draw inner rectangle
	app->screen->rectangle(
			box.x + 2, 
			box.y + 2, 
			box.w - 4, 
			box.h - 4, 
			app->skin->colours.msgBoxBorder);

	//icon+wrapped_text
	if ((*app->sc)[icon] != NULL) {
		(*app->sc)[icon]->blit(
			app->screen, 
			box.x + 24, 
			box.y + 24 , 
			HAlignCenter | VAlignMiddle);
	}

	app->screen->write(
		app->font, 
		wrappedText, 
		box.x+((*app->sc)[icon] != NULL ? 47 : 11), 
		app->config->halfY() - app->font->getHeight()/5, 
		VAlignMiddle, 
		app->skin->colours.fontAlt, 
		app->skin->colours.fontAltOutline);

	if (this->autohide != 0) {
		app->screen->flip();
		if (this->autohide > 0) {
			SDL_Delay(this->autohide);
		}
		return -1;
	}

	//draw buttons rectangle
	app->screen->box(
		box.x, 
		box.y + box.h, 
		box.w, 
		app->font->getHeight(), 
		app->skin->colours.msgBoxBackground);

	int btnX = app->config->halfX() + (box.w / 2) - 6;
	for (uint32_t i = 0; i < buttons.size(); i++) {
		if (!buttons[i].empty()) {
			buttonPositions[i].y = box.y + box.h + app->font->getHalfHeight();
			buttonPositions[i].w = btnX;

			btnX = app->ui->drawButtonRight(
				app->screen, 
				buttonLabels[i], 
				buttons[i], 
				btnX, 
				buttonPositions[i].y);

			buttonPositions[i].x = btnX;
			buttonPositions[i].w = buttonPositions[i].x - btnX - 6;
		}
	}
	app->screen->flip();

	while (result < 0) {
		//touchscreen
		if (app->f200 && app->ts.poll()) {
			for (uint32_t i = 0; i < buttons.size(); i++) {
				if (!buttons[i].empty() && app->ts.inRect(buttonPositions[i])) {
					result = i;
					break;
				}
			}
		}

		bool inputAction = app->input.update();
		if (inputAction) {
			for (uint32_t i = 0; i < buttons.size(); i++) {
				if (!buttons[i].empty() && app->input[i]) {
					result = i;
					break;
				}
			}
		}
	}
	app->input.dropEvents();
	TRACE("exit : %i", result);
	return result;
}
