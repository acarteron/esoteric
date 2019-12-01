#include "renderer.h"
#include "menu.h"
#include "constants.h"
#include "linkapp.h"
#include "rtc.h"
#include "debug.h"
#include "utilities.h"

uint8_t Renderer::getVolumeMode(uint8_t vol) {
	TRACE("getVolumeMode - enter : %i", vol);
	if (!vol) return VOLUME_MODE_MUTE;
	else if (vol > 0 && vol < 20) return VOLUME_MODE_PHONES;
	return VOLUME_MODE_NORMAL;
}

Renderer::Renderer(GMenu2X *gmenu2x) : 
    iconBrightness {
		gmenu2x->sc->skinRes("imgs/brightness/0.png"),
		gmenu2x->sc->skinRes("imgs/brightness/1.png"),
		gmenu2x->sc->skinRes("imgs/brightness/2.png"),
		gmenu2x->sc->skinRes("imgs/brightness/3.png"),
		gmenu2x->sc->skinRes("imgs/brightness/4.png"),
		gmenu2x->sc->skinRes("imgs/brightness.png"),
	}, iconBattery {
		gmenu2x->sc->skinRes("imgs/battery/0.png"),
		gmenu2x->sc->skinRes("imgs/battery/1.png"),
		gmenu2x->sc->skinRes("imgs/battery/2.png"),
		gmenu2x->sc->skinRes("imgs/battery/3.png"),
		gmenu2x->sc->skinRes("imgs/battery/4.png"),
		gmenu2x->sc->skinRes("imgs/battery/5.png"),
		gmenu2x->sc->skinRes("imgs/battery/ac.png"),
	}, iconVolume {
		gmenu2x->sc->skinRes("imgs/mute.png"),
		gmenu2x->sc->skinRes("imgs/phones.png"),
		gmenu2x->sc->skinRes("imgs/volume.png"),
	} {

	this->finished_ = false;
	this->gmenu2x = gmenu2x;
    this->rtc.refresh();

	this->prevBackdrop = gmenu2x->skin->wallpaper;
	this->currBackdrop = prevBackdrop;

	iconSD = gmenu2x->sc->skinRes("imgs/sd1.png");
	iconManual = gmenu2x->sc->skinRes("imgs/manual.png");
	iconCPU = gmenu2x->sc->skinRes("imgs/cpu.png");

	brightnessIcon = 5;
	batteryIcon = 3;
    currentVolumeMode = VOLUME_MODE_MUTE;

	helpers.clear();

	this->timerId_ = SDL_AddTimer(this->interval_, callback, this);
	this->locked_ = false;

	this->pollHW();

}

Renderer::~Renderer() {
    TRACE("~Renderer");
	this->quit();
}

void Renderer::quit() {
	this->finished_ = true;
    if (this->timerId_ > 0) {
		SDL_SetTimer(0, NULL);
        SDL_RemoveTimer(this->timerId_);
		this->interval_ = 0;
		this->timerId_ = 0;
    }
}

uint32_t Renderer::callback(uint32_t interval, void * data) {
	TRACE("enter");
	Renderer * me = static_cast<Renderer*>(data);
	if (me->finished_) {
		return (0);
	}
	me->pollHW();
	return interval;
}

void Renderer::render() {

	if (this->locked_) 
		return;
	this->locked_ = true;

    int x = 0;
    int y = 0;
    int ix = 0;
    int iy = 0;

    TRACE("setting the clearing box");
	gmenu2x->screen->box(
		(SDL_Rect){ 0, 0, gmenu2x->config->resolutionX(), gmenu2x->config->resolutionY() }, 
		(RGBAColor){0, 0, 0, 255});

	// do a background image or a background colour 
	if ((*gmenu2x->sc)[currBackdrop]) {
		(*gmenu2x->sc)[currBackdrop]->blit(gmenu2x->screen,0,0);
	} else {
		gmenu2x->screen->box(
			(SDL_Rect){ 0, 0, gmenu2x->config->resolutionX(), gmenu2x->config->resolutionY() }, 
			gmenu2x->skin->colours.background);
	}

	// info bar
	TRACE("infoBar test");
	if (gmenu2x->skin->sectionInfoBarVisible) {
		if (gmenu2x->skin->sectionBar == Skin::SB_TOP || gmenu2x->skin->sectionBar == Skin::SB_BOTTOM) {
			TRACE("infobar needs drawing");

			SDL_Rect infoBarRect;
			switch(gmenu2x->skin->sectionBar) {
				case Skin::SB_TOP:
					infoBarRect = (SDL_Rect) { 
						0, 
						gmenu2x->config->resolutionY() - gmenu2x->skin->sectionInfoBarSize, 
						gmenu2x->config->resolutionX(), 
						gmenu2x->skin->sectionInfoBarSize 
					};
					break;
				case Skin::SB_BOTTOM:
					infoBarRect = (SDL_Rect) { 
						0, 
						0, 
						gmenu2x->config->resolutionX(), 
						gmenu2x->skin->sectionInfoBarSize 
					};
					break;
				default:
					break;
			};

			// do we have an image
			if (!gmenu2x->skin->sectionInfoBarImage.empty()) {
				TRACE("infoBar has an image : %s", gmenu2x->skin->sectionInfoBarImage.c_str());
				if ((*gmenu2x->sc)[gmenu2x->skin->sectionInfoBarImage]->raw->h != infoBarRect.h || (*gmenu2x->sc)[gmenu2x->skin->sectionInfoBarImage]->raw->w != gmenu2x->config->resolutionX()) {
					TRACE("infoBar image is being scaled");
					(*gmenu2x->sc)[gmenu2x->skin->sectionInfoBarImage]->softStretch(
						gmenu2x->config->resolutionX(), 
						infoBarRect.h);
				}
				(*gmenu2x->sc)[gmenu2x->skin->sectionInfoBarImage]->blit(
					gmenu2x->screen, 
					infoBarRect);
			} else {
				TRACE("infoBar has no image, going for a simple box");
				gmenu2x->screen->box(
					infoBarRect, 
					gmenu2x->skin->colours.infoBarBackground);
			}

			int btnX = 6;
			int btnY = infoBarRect.y + (infoBarRect.h / 2);
			btnX = gmenu2x->ui->drawButton(gmenu2x->screen, "select", gmenu2x->tr["edit"], btnX, btnY);
			btnX = gmenu2x->ui->drawButton(gmenu2x->screen, "start", gmenu2x->tr["config"], btnX, btnY);
			btnX = gmenu2x->ui->drawButton(gmenu2x->screen, "a", gmenu2x->tr["run"], btnX, btnY);
			btnX = gmenu2x->ui->drawButton(gmenu2x->screen, "x", gmenu2x->tr["fave"], btnX, btnY);

			/*
			gmenu2x->screen->write(
				gmenu2x->font, 
				"\u00AB info bar \u00BB", 
				infoBarRect.w / 2, 
				infoBarRect.y + (infoBarRect.h / 2),
				HAlignCenter | VAlignMiddle);
			*/
		}
	}
		
	// SECTIONS
	TRACE("sections");
	if (gmenu2x->skin->sectionBar) {

		// do we have an image
		if (!gmenu2x->skin->sectionTitleBarImage.empty()) {
			TRACE("sectionBar has an image : %s", gmenu2x->skin->sectionTitleBarImage.c_str());
			if ((*gmenu2x->sc)[gmenu2x->skin->sectionTitleBarImage]->raw->h != gmenu2x->sectionBarRect.h || (*gmenu2x->sc)[gmenu2x->skin->sectionTitleBarImage]->raw->w != gmenu2x->config->resolutionX()) {
				TRACE("sectionBar image is being scaled");
				(*gmenu2x->sc)[gmenu2x->skin->sectionTitleBarImage]->softStretch(gmenu2x->config->resolutionX(), gmenu2x->sectionBarRect.h);
			}
			(*gmenu2x->sc)[gmenu2x->skin->sectionTitleBarImage]->blit(
				gmenu2x->screen, 
				gmenu2x->sectionBarRect);
		} else {
			TRACE("sectionBar has no image, going for a simple box");
			gmenu2x->screen->box(gmenu2x->sectionBarRect, gmenu2x->skin->colours.titleBarBackground);
		}

		x = gmenu2x->sectionBarRect.x; 
		y = gmenu2x->sectionBarRect.y;

        TRACE("checking mode");
		// we're in section text mode....
		if (!gmenu2x->skin->showSectionIcons && (gmenu2x->skin->sectionBar == Skin::SB_TOP || gmenu2x->skin->sectionBar == Skin::SB_BOTTOM)) {
            TRACE("section text mode");
            string sectionName = gmenu2x->menu->selSection();

            TRACE("section text mode - writing title");
			gmenu2x->screen->write(
				gmenu2x->fontSectionTitle, 
				"\u00AB " + gmenu2x->tr.translate(sectionName) + " \u00BB", 
				gmenu2x->sectionBarRect.w / 2, 
				gmenu2x->sectionBarRect.y + (gmenu2x->sectionBarRect.h / 2),
				HAlignCenter | VAlignMiddle);

            TRACE("section text mode - checking clock");
			if (gmenu2x->skin->showClock) {	

                TRACE("section text mode - writing clock");
                string clockTime = rtc.getClockTime(true);
                TRACE("section text mode - got clock time : %s", clockTime.c_str());
				gmenu2x->screen->write(
					gmenu2x->fontSectionTitle, 
					clockTime, 
					4, 
					gmenu2x->sectionBarRect.y + (gmenu2x->sectionBarRect.h / 2),
					HAlignLeft | VAlignMiddle);
			}

		} else {
            //TRACE("icon mode");
			for (int i = gmenu2x->menu->firstDispSection(); i < gmenu2x->menu->getSections().size() && i < gmenu2x->menu->firstDispSection() + gmenu2x->menu->sectionNumItems(); i++) {
				if (gmenu2x->skin->sectionBar == Skin::SB_LEFT || gmenu2x->skin->sectionBar == Skin::SB_RIGHT) {
					y = (i - gmenu2x->menu->firstDispSection()) * gmenu2x->skin->sectionTitleBarSize;
				} else {
					x = (i - gmenu2x->menu->firstDispSection()) * gmenu2x->skin->sectionTitleBarSize;
				}

                //TRACE("icon mode - got x and y");
				if (gmenu2x->menu->selSectionIndex() == (int)i) {
                    //TRACE("icon mode - applying highlight");
					gmenu2x->screen->box(
						x, 
						y, 
						gmenu2x->skin->sectionTitleBarSize, 
						gmenu2x->skin->sectionTitleBarSize, 
						gmenu2x->skin->colours.selectionBackground);
                }
                //TRACE("icon mode - blit");
				(*gmenu2x->sc)[gmenu2x->menu->getSectionIcon(i)]->blit(
					gmenu2x->screen, 
					{x, y, gmenu2x->skin->sectionTitleBarSize, gmenu2x->skin->sectionTitleBarSize}, 
					HAlignCenter | VAlignMiddle);
			}
		}
	}

	// LINKS
	//TRACE("links");
	gmenu2x->screen->setClipRect(gmenu2x->linksRect);
	gmenu2x->screen->box(gmenu2x->linksRect, gmenu2x->skin->colours.listBackground);

	int i = gmenu2x->menu->firstDispRow() * gmenu2x->skin->numLinkCols;

	if (gmenu2x->skin->numLinkCols == 1) {
		//TRACE("column mode : %i", gmenu2x->menu->sectionLinks()->size());
		// LIST
        ix = gmenu2x->linksRect.x;
		for (y = 0; y < gmenu2x->skin->numLinkRows && i < gmenu2x->menu->sectionLinks()->size(); y++, i++) {
			iy = gmenu2x->linksRect.y + y * gmenu2x->linkHeight;

			// highlight selected link
			if (i == (uint32_t)gmenu2x->menu->selLinkIndex())
				gmenu2x->screen->box(
					ix, 
					iy, 
					gmenu2x->linksRect.w, 
					gmenu2x->linkHeight, 
					gmenu2x->skin->colours.selectionBackground);

			int padding = 36;
			if (gmenu2x->skin->linkDisplayMode == Skin::ICON_AND_TEXT || gmenu2x->skin->linkDisplayMode == Skin::ICON) {
				//TRACE("theme uses icons");
				(*gmenu2x->sc)[gmenu2x->menu->sectionLinks()->at(i)->getIconPath()]->blit(
					gmenu2x->screen, 
					{ix, iy, padding, gmenu2x->linkHeight}, 
					HAlignCenter | VAlignMiddle);
			} else {
				padding = 4;
			}
				
			if (gmenu2x->skin->linkDisplayMode == Skin::ICON_AND_TEXT || gmenu2x->skin->linkDisplayMode == Skin::TEXT) {
				//TRACE("adding : %s", gmenu2x->menu->sectionLinks()->at(i)->getDisplayTitle().c_str());
				int localXpos = ix + gmenu2x->linkSpacing + padding;
				int localAlignTitle = VAlignMiddle;
				int totalFontHeight = gmenu2x->fontTitle->getHeight() + gmenu2x->font->getHeight();
				TRACE("total Font Height : %i, linkHeight: %i", totalFontHeight, gmenu2x->linkHeight);

				if (gmenu2x->skin->sectionBar == Skin::SB_BOTTOM || gmenu2x->skin->sectionBar == Skin::SB_TOP || gmenu2x->skin->sectionBar == Skin::SB_OFF) {
					TRACE("HITTING MIDDLE ALIGN");
					localXpos = gmenu2x->linksRect.w / 2;
					if (totalFontHeight >= gmenu2x->linkHeight) {
						localAlignTitle = HAlignCenter | VAlignTop;
					} else {
						localAlignTitle = HAlignCenter | VAlignMiddle;
					}
				}
				gmenu2x->screen->write(
					gmenu2x->fontTitle, 
					gmenu2x->tr.translate(gmenu2x->menu->sectionLinks()->at(i)->getDisplayTitle()), 
					localXpos, 
					iy + (gmenu2x->fontTitle->getHeight() / 2), 
					localAlignTitle);
					
				if (totalFontHeight < gmenu2x->linkHeight) {
					gmenu2x->screen->write(
						gmenu2x->font, 
						gmenu2x->tr.translate(gmenu2x->menu->sectionLinks()->at(i)->getDescription()), 
						ix + gmenu2x->linkSpacing + padding, 
						iy + gmenu2x->linkHeight - (gmenu2x->linkSpacing / 2), 
						VAlignBottom);
				}
			}
		}
	} else {
		//TRACE("row mode : %i", gmenu2x->menu->sectionLinks()->size());
        int ix, iy = 0;
		for (y = 0; y < gmenu2x->skin->numLinkRows; y++) {
			for (x = 0; x < gmenu2x->skin->numLinkCols && i < gmenu2x->menu->sectionLinks()->size(); x++, i++) {

				string title = gmenu2x->tr.translate(gmenu2x->menu->sectionLinks()->at(i)->getDisplayTitle());
				int textWidth = gmenu2x->font->getTextWidth(title);
				/*
		                TRACE("SCALE::TEXT-WIDTH: %i, TEXT-LENGTH: %i, LINK-WIDTH: %i", 
							textWidth, 
							title.size(), 
							linkWidth);
                */
				if (textWidth > gmenu2x->linkWidth) {
					int wrapFactor = textWidth / gmenu2x->linkWidth;
					int wrapMax = title.size() / wrapFactor;
					title = splitInLines(title, wrapMax);
					//TRACE("SCALE::WRAP::number of wraps needed: %i, wrap at max chars: %i, title: %s", wrapFactor, wrapMax, title.c_str());
				}

				// calc cell x && y
				ix = gmenu2x->linksRect.x + (x * gmenu2x->linkWidth)  + (x + 1) * gmenu2x->linkSpacing;
				iy = gmenu2x->linksRect.y + (y * gmenu2x->linkHeight) + (y + 1) * gmenu2x->linkSpacing;

				gmenu2x->screen->setClipRect({ix, iy, gmenu2x->linkWidth, gmenu2x->linkHeight});

				// selected link highlight
				if (i == (uint32_t)gmenu2x->menu->selLinkIndex()) {
					gmenu2x->screen->box(
						ix, 
						iy, 
						gmenu2x->linkWidth, 
						gmenu2x->linkHeight, 
						gmenu2x->skin->colours.selectionBackground);
				}

				if (gmenu2x->skin->linkDisplayMode == Skin::ICON) {
					//TRACE("adding icon and text : %s", title.c_str());
					(*gmenu2x->sc)[gmenu2x->menu->sectionLinks()->at(i)->getIconPath()]->blit(
						gmenu2x->screen, 
						{ix + 2, iy + 2, gmenu2x->linkWidth - 4, gmenu2x->linkHeight - 4}, 
						HAlignCenter | VAlignMiddle);
				} else if (gmenu2x->skin->linkDisplayMode == Skin::ICON_AND_TEXT) {
					(*gmenu2x->sc)[gmenu2x->menu->sectionLinks()->at(i)->getIconPath()]->blit(
						gmenu2x->screen, 
						{ix + 2, iy, gmenu2x->linkWidth - 4, gmenu2x->linkHeight}, 
						HAlignCenter | VAlignTop);

					gmenu2x->screen->write(gmenu2x->font, 
						title, 
						ix + (gmenu2x->linkWidth / 2), 
						iy + gmenu2x->linkHeight,
						HAlignCenter | VAlignBottom);
				} else {
					//TRACE("adding text only : %s", title.c_str());
					gmenu2x->screen->write(gmenu2x->font, 
						title, 
						ix + (gmenu2x->linkWidth / 2), 
						iy + (gmenu2x->linkHeight / 2), 
						HAlignCenter | VAlignMiddle);
				}

			}
		}
	}
	//TRACE("done");
	gmenu2x->screen->clearClipRect();

	gmenu2x->ui->drawScrollBar(gmenu2x->skin->numLinkRows, 
		gmenu2x->menu->sectionLinks()->size() / gmenu2x->skin->numLinkCols + ((gmenu2x->menu->sectionLinks()->size() % gmenu2x->skin->numLinkCols==0) ? 0 : 1), 
		gmenu2x->menu->firstDispRow(), 
		gmenu2x->linksRect);

	currBackdrop = gmenu2x->skin->wallpaper;
	if (gmenu2x->menu->selLink() != NULL && gmenu2x->menu->selLinkApp() != NULL && !gmenu2x->menu->selLinkApp()->getBackdropPath().empty() && gmenu2x->sc->addImage(gmenu2x->menu->selLinkApp()->getBackdropPath()) != NULL) {
		TRACE("setting currBackdrop to : %s", gmenu2x->menu->selLinkApp()->getBackdropPath().c_str());
		currBackdrop = gmenu2x->menu->selLinkApp()->getBackdropPath();
	}

	//Background has changed flip it and return out quickly
	if (prevBackdrop != currBackdrop) {
		INFO("New backdrop: %s", currBackdrop.c_str());
		gmenu2x->sc->del(prevBackdrop);
		prevBackdrop = currBackdrop;
		// input.setWakeUpInterval(1);

		this->locked_ = false;
		return;
	}

	/* 
	 *
	 * helper icon section
	 * 
	 */
	if (gmenu2x->skin->sectionBar) {
		// tray helper icons
		int helperHeight = 20;
		int maxItemsPerRow = 0;
		if (gmenu2x->sectionBarRect.w > gmenu2x->sectionBarRect.h) {
			maxItemsPerRow = (int)(gmenu2x->sectionBarRect.h / (float)helperHeight);
		} else {
			maxItemsPerRow = (int)(gmenu2x->sectionBarRect.w / (float)helperHeight);
		}
		int rootXPos = gmenu2x->sectionBarRect.x + gmenu2x->sectionBarRect.w - 18;
		int rootYPos = gmenu2x->sectionBarRect.y + gmenu2x->sectionBarRect.h - 18;
		TRACE("hitting up the helpers");

		helpers.push_back(iconVolume[currentVolumeMode]);
		helpers.push_back(iconBattery[batteryIcon]);
		if (gmenu2x->hw->getCardStatus() == IHardware::MMC_MOUNTED) {
			helpers.push_back(iconSD);
		}
		helpers.push_back(iconBrightness[brightnessIcon]);
		if (gmenu2x->menu->selLink() != NULL) {
			if (gmenu2x->menu->selLinkApp() != NULL) {
				if (!gmenu2x->menu->selLinkApp()->getManualPath().empty()) {
					// Manual indicator
					helpers.push_back(iconManual);
				}
				if (gmenu2x->menu->selLinkApp()->clock() != gmenu2x->config->cpuMenu()) {
					// CPU indicator
					helpers.push_back(iconCPU);
				}
			}
		}
		TRACE("layoutHelperIcons");
		int * xPosPtr = & rootXPos;
		int * yPosPtr = & rootYPos;
		layoutHelperIcons(helpers, gmenu2x->screen, helperHeight, xPosPtr, yPosPtr, maxItemsPerRow);
		TRACE("helpers.clear()");
		helpers.clear();

		if (gmenu2x->skin->showClock) {
			if (gmenu2x->skin->sectionBar == Skin::SB_TOP || gmenu2x->skin->sectionBar == Skin::SB_BOTTOM) {
				if (gmenu2x->skin->showSectionIcons) {
					// grab the new x offset and write the clock
					string time = rtc.getClockTime(true);
					gmenu2x->screen->write(
						gmenu2x->fontSectionTitle, 
						time, 
						*(xPosPtr) - (gmenu2x->fontSectionTitle->getTextWidth(time) / 2), 
						gmenu2x->sectionBarRect.y + (gmenu2x->sectionBarRect.h / 2),
						VAlignMiddle);
				}
			} else {
				// grab the new y offset and write the clock
				gmenu2x->screen->write(
					gmenu2x->fontSectionTitle, 
					rtc.getClockTime(true), 
					gmenu2x->sectionBarRect.x + 4, 
					*(yPosPtr),
					HAlignLeft | VAlignTop);
			}
		}
	} // gmenu2x->skin->sectionBar

    TRACE("flip"); 
	gmenu2x->screen->flip();
	this->locked_ = false;
    TRACE("exit");
}

void Renderer::layoutHelperIcons(vector<Surface*> icons, Surface *target, int helperHeight, int * rootXPosPtr, int * rootYPosPtr, int iconsPerRow) {
	TRACE("enter - rootXPos = %i, rootYPos = %i", *rootXPosPtr, *rootYPosPtr);
	int iconCounter = 0;
	int currentXOffset = 0;
	int currentYOffset = 0;
	int rootXPos = *rootXPosPtr;
	int rootYPos = *rootYPosPtr;

	for(std::vector<Surface*>::iterator it = icons.begin(); it != icons.end(); ++it) {
		TRACE("blitting");
		(*it)->blit(
			gmenu2x->screen, 
			rootXPos - (currentXOffset * (helperHeight - 2)), 
			rootYPos - (currentYOffset * (helperHeight - 2))
		);
		if (++iconCounter % iconsPerRow == 0) {
			++currentXOffset;
			currentYOffset = 0;
		} else {
			++currentYOffset;
		}
	};
	*rootXPosPtr = rootXPos - (currentXOffset * (helperHeight - 2));
	*rootYPosPtr = rootYPos - (currentXOffset * (helperHeight - 2));
	TRACE("exit - rootXPos = %i, rootYPos = %i", *rootXPosPtr, *rootYPosPtr);
}

void Renderer::pollHW() {
	// if we're going to draw helpers, get their latest value
	TRACE("section bar test");
	if (this->gmenu2x->skin->sectionBar) {
		TRACE("section bar exists in skin settings");
		TRACE("updating helper icon status");
		this->batteryIcon = this->gmenu2x->hw->getBatteryLevel();
		if (this->batteryIcon > 5) this->batteryIcon = 6;

		this->brightnessIcon = this->gmenu2x->hw->getBacklightLevel();
		if (this->brightnessIcon > 4 || this->iconBrightness[this->brightnessIcon] == NULL) 
			this->brightnessIcon = 5;

		int currentVolume = this->gmenu2x->hw->getVolumeLevel();
		this->currentVolumeMode = this->getVolumeMode(currentVolume);
        this->rtc.refresh();
		TRACE("helper icon status updated");
    }
}

