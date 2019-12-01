#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include "utilities.h"
#include "constants.h"

static const std::string CONFIG_FILE_NAME = "gmenunx.conf";
static const int CONFIG_CURRENT_VERSION = 1;
static const std::string EXTERNAL_LAUNCHER_PATH = EXTERNAL_CARD_PATH + "/ROMS";
static const std::string HOME_DIR = "/media/home";
static const std::string APP_EXTERNAL_PATH = EXTERNAL_CARD_PATH + "/APPS";

class Config {

public:

    Config(std::string const &prefix);
    ~Config();
    void changePath(std::string const &prefix) { this->prefix = prefix; }

    // accessors
    std::string skin() const { return this->skin_; }
    void skin(std::string val) { 
        if (val != this->skin_) {
            this->skin_ = val;
            this->isDirty = true;
        }
     }
    std::string externalAppPath() const { return this->externalAppPath_; }
    void externalAppPath(std::string val) { 
        if (val != this->externalAppPath_) {
            if (dirExists(val)) {
                this->externalAppPath_ = val;
                this->isDirty = true;
            }
        }
     }
    std::string performance() const { return this->performance_; }
    void performance(std::string val) {
        if (val != this->performance_) {
            this->performance_ = val;
            this->isDirty = true;
        }
    }
    std::string tvOutMode() const { return this->tvOutMode_; }
    void tvOutMode(std::string val) {
        if (val != this->tvOutMode_) {
            this->tvOutMode_ = val;
            this->isDirty = true;
        }
    }
    std::string lang() const { return this->lang_; }
    void lang(std::string val) {
        if (val != this->lang_) {
            this->lang_ = val;
            this->isDirty = true;
        }
    }
    std::string batteryType() const { return this->batteryType_; }
    void batteryType(std::string val) {
        if (val != this->batteryType_) {
            this->batteryType_ = val;
            this->isDirty = true;
        }
    }
    std::string sectionFilter() const { return this->sectionFilter_; }
    void sectionFilter(std::string val) {
        if (val != this->sectionFilter_) {
            this->sectionFilter_ = val;
            this->isDirty = true;
        }
    }
    std::string launcherPath() const { return this->launcherPath_; }
    void launcherPath(std::string val) {
        if (val != this->launcherPath_) {
            this->launcherPath_ = val;
            this->isDirty = true;
        }
    }

    int buttonRepeatRate() const { return this->buttonRepeatRate_; }
    void buttonRepeatRate(int val) {
        if (val != this->buttonRepeatRate_) {
            this->buttonRepeatRate_ = val;
            this->isDirty = true;
        }
    }
    int resolutionX() const { return this->resolutionX_; }
    void resolutionX(int val) {
        if (val != this->resolutionX_) {
            this->resolutionX_ = val;
            this->isDirty = true;
        }
    }
    int resolutionY() const { return this->resolutionY_; }
    void resolutionY(int val) {
        if (val != this->resolutionY_) {
            this->resolutionY_ = val;
            this->isDirty = true;
        }
    }
    int backlightLevel() const { return this->backlightLevel_; }
    void backlightLevel(int val) {
        if (val != this->backlightLevel_) {
            this->backlightLevel_ = val;
            this->isDirty = true;
        }
    }
    int minBattery() const { return this->minBattery_; }
    void minBattery(int val) {
        if (val != this->minBattery_) {
            this->minBattery_ = val;
            this->isDirty = true;
        }
    }
    int maxBattery() const { return this->maxBattery_; }
    void maxBattery(int val) {
        if (val != this->maxBattery_) {
            this->maxBattery_ = val;
            this->isDirty = true;
        }
    }
    int backlightTimeout() const { return this->backlightTimeout_; }
    void backlightTimeout(int val) {
        if (val != this->backlightTimeout_) {
            this->backlightTimeout_ = val;
            this->isDirty = true;
        }
    }
    int videoBpp() const { return this->videoBpp_; }
    void videoBpp(int val) {
        if (val != this->videoBpp_) {
            this->videoBpp_ = val;
            this->isDirty = true;
        }
    }
    int cpuMin() const { return this->cpuMin_; }
    void cpuMin(int val) {
        if (val != this->cpuMin_) {
            this->cpuMin_ = val;
            this->isDirty = true;
        }
    }
    int cpuMax() const { return this->cpuMax_; }
    void cpuMax(int val) {
        if (val != this->cpuMax_) {
            this->cpuMax_ = val;
            this->isDirty = true;
        }
    }
    int cpuMenu() const { return this->cpuMenu_; }
    void cpuMenu(int val) {
        if (val != this->cpuMenu_) {
            this->cpuMenu_ = val;
            this->isDirty = true;
        }
    }
    int globalVolume() const { return this->globalVolume_; }
    void globalVolume(int val) {
        if (val != this->globalVolume_) {
            this->globalVolume_ = val;
            this->isDirty = true;
        }
    }
    int link() const { return this->link_; }
    void link(int val) {
        if (val != this->link_) {
            this->link_ = val;
            this->isDirty = true;
        }
    }
    int section() const { return this->section_; }
    void section(int val) {
        if (val != this->section_) {
            this->section_ = val;
            this->isDirty = true;
        }
    }
    int saveSelection() const { return this->saveSelection_; }
    void saveSelection(int val) {
        if (val != this->saveSelection_) {
            this->saveSelection_ = val;
            this->isDirty = true;
        }
    }
    int powerTimeout() const { return this->powerTimeout_; }
    void powerTimeout(int val) {
        if (val != this->powerTimeout_) {
            this->powerTimeout_ = val;
            this->isDirty = true;
        }
    }
    int outputLogs() const { return this->outputLogs_; }
    void outputLogs(int val) {
        if (val != this->outputLogs_) {
            this->outputLogs_ = val;
            this->isDirty = true;
        }
    }
    int version() const { return this->version_; }
    void version(int val) {
        if (val != this->version_) {
            this->version_ = val;
            this->isDirty = true;
        }
    }

    std::string configFile() { return this->prefix + CONFIG_FILE_NAME; }
    bool loadConfig();
    std::string toString();
    std::string now();
    bool save();
    bool remove();

    static bool configExistsUnderPath(const std::string & path);

    // TODO :: cache this
    int halfX() { return this->resolutionX() / 2; }
    int halfY() { return this->resolutionY() / 2; }

private:

    std::string prefix;
    bool isDirty;

    std::string externalAppPath_;// = APP_EXTERNAL_PATH;
    std::string skin_; //="Default"
    std::string performance_; //="On demand"
    std::string tvOutMode_; //="NTSC"
    std::string lang_; //=""
    std::string batteryType_; //="BL-5B"
    std::string sectionFilter_; //="applications,foo"
    std::string launcherPath_; //="/media/sdcard/ROMS"

    int buttonRepeatRate_; //=10
    int resolutionX_; //=320
    int resolutionY_; //=240
    int backlightLevel_; //=70
    int minBattery_; //=0
    int maxBattery_; //=5
    int backlightTimeout_; //=30
    int videoBpp_; //=32
    int cpuMin_; //=342
    int cpuMax_; //=996
    int cpuMenu_; //=600
    int globalVolume_; //=60
    int link_; //=1
    int section_; //=1
    int saveSelection_; //=1
    int powerTimeout_; //=10
    int outputLogs_; //=1
    int version_; //=1

    void reset();
    bool fromFile();
    void constrain();

};

#endif