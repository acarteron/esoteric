
#include <math.h>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "constants.h"
#include "fileutils.h"
#include "stringutils.h"
#include "debug.h"
#include "hw-rg350.h"
#include "hw-cpu.h"
#include "hw-power.h"
#include "hw-clock.h"

HwRg350::HwRg350() : IHardware() {
    TRACE("enter");
    this->BLOCK_DEVICE = "/sys/block/mmcblk1/size";
    this->INTERNAL_MOUNT_DEVICE = "/dev/mmcblk0";
    this->EXTERNAL_MOUNT_DEVICE = "/dev/mmcblk1p1";
    this->EXTERNAL_MOUNT_FORMAT = "auto";
    this->EXTERNAL_MOUNT_POINT = EXTERNAL_CARD_PATH;

    this->clock_ = (IClock *)new RTC();
    this->soundcard_ = (ISoundcard *)new AlsaSoundcard("default", "PCM");
    this->cpu_ = JZ4770Factory::getCpu();
    this->power_ = (IPower *)new JzPower();

    this->ledMaxBrightness_ = FileUtils::fileExists(LED_MAX_BRIGHTNESS_PATH) ? FileUtils::fileReader(LED_MAX_BRIGHTNESS_PATH) : "0";
    this->pollBacklight = FileUtils::fileExists(BACKLIGHT_PATH);

    this->getBacklightLevel();
    this->getKeepAspectRatio();
    this->resetKeymap();

    TRACE(
        "brightness: %i, volume : %i",
        this->getBacklightLevel(),
        this->soundcard_->getVolume());
}
HwRg350::~HwRg350() {
    delete this->clock_;
    delete this->cpu_;
    delete this->soundcard_;
    delete this->power_;
    this->ledOff();
}

bool HwRg350::getTVOutStatus() { return 0; };
std::string HwRg350::getTVOutMode() { return "OFF"; }
void HwRg350::setTVOutMode(std::string mode) {
    std::string val = mode;
    if (val != "NTSC" && val != "PAL") val = "OFF";
}

void HwRg350::ledOn(int flashSpeed) {
    TRACE("enter");
    try {
        int limited = constrain(flashSpeed, 0, atoi(ledMaxBrightness_.c_str()));
        std::string trigger = triggerToString(LedAllowedTriggers::TIMER);
        TRACE("mode : %s - for %i", trigger.c_str(), limited);
        FileUtils::fileWriter(LED_TRIGGER_PATH, trigger);
        FileUtils::fileWriter(LED_DELAY_ON_PATH, limited);
        FileUtils::fileWriter(LED_DELAY_OFF_PATH, limited);
    } catch (std::exception e) {
        ERROR("LED error : '%s'", e.what());
    } catch (...) {
        ERROR("Unknown error");
    }
    TRACE("exit");
}
void HwRg350::ledOff() {
    TRACE("enter");
    try {
        std::string trigger = triggerToString(LedAllowedTriggers::NONE);
        TRACE("mode : %s", trigger.c_str());
        FileUtils::fileWriter(LED_TRIGGER_PATH, trigger);
        FileUtils::fileWriter(LED_BRIGHTNESS_PATH, ledMaxBrightness_);
    } catch (std::exception e) {
        ERROR("LED error : '%s'", e.what());
    } catch (...) {
        ERROR("Unknown error");
    }
    TRACE("exit");
    return;
}

int HwRg350::getBacklightLevel() {
    TRACE("enter");
    if (this->pollBacklight) {
        int level = 0;
        //force  scale 0 - 100
        std::string result = FileUtils::fileReader(BACKLIGHT_PATH);
        if (result.length() > 0) {
            level = ceil(atoi(StringUtils::trim(result).c_str()) / 2.55);
        }
        this->backlightLevel_ = level;
    }
    TRACE("exit : %i", this->backlightLevel_);
    return this->backlightLevel_;
}
int HwRg350::setBacklightLevel(int val) {
    TRACE("enter - %i", val);
    // wrap it
    if (val <= 0)
        val = 100;
    else if (val > 100)
        val = 0;
    if (val == this->backlightLevel_)
        return val;

    int deviceVal = (int)(val * (255.0f / 100));
    TRACE("device value : %i", deviceVal);

    if (FileUtils::fileWriter(BACKLIGHT_PATH, deviceVal)) {
        TRACE("success");
    } else {
        ERROR("Couldn't update backlight value to : %i", deviceVal);
    }
    this->backlightLevel_ = val;
    return this->backlightLevel_;
}

bool HwRg350::getKeepAspectRatio() {
    TRACE("enter");
    if (FileUtils::fileExists(ASPECT_RATIO_PATH)) {
        std::string result = FileUtils::fileReader(ASPECT_RATIO_PATH);
        TRACE("raw result : '%s'", result.c_str());
        if (result.length() > 0) {
            result = result[0];
            result = StringUtils::toLower(result);
        }
        this->keepAspectRatio_ = ("y" == result);
    }
    TRACE("exit : %i", this->keepAspectRatio_);
    return this->keepAspectRatio_;
}
bool HwRg350::setKeepAspectRatio(bool val) {
    TRACE("enter - %i", val);
    std::string payload = val ? "Y" : "N";
    if (FileUtils::fileWriter(ASPECT_RATIO_PATH, payload)) {
        TRACE("success");
    } else {
        ERROR("Couldn't update aspect ratio value to : '%s'", payload.c_str());
    }
    this->keepAspectRatio_ = val;
    return this->keepAspectRatio_;
}

std::string HwRg350::getDeviceType() { return "RG-350"; }

bool HwRg350::setScreenState(const bool &enable) {
    TRACE("enter : %s", (enable ? "on" : "off"));
    const char *path = SCREEN_BLANK_PATH.c_str();
    const char *blank = enable ? "0" : "1";
    return this->writeValueToFile(path, blank);
}

std::string HwRg350::systemInfo() {
    TRACE("append - command /usr/bin/system_info");
    if (FileUtils::fileExists("/usr/bin/system_info")) {
        return FileUtils::execute("/usr/bin/system_info") + "\n";
    }
    return IHardware::systemInfo();
}

std::string HwRg350::triggerToString(LedAllowedTriggers t) {
    TRACE("mode : %i", t);
    switch (t) {
        case TIMER:
            return "timer";
            break;
        default:
            return "none";
            break;
    };
}

void HwRg350::resetKeymap() {
    this->writeValueToFile(ALT_KEYMAP_FILE, "N");
}
