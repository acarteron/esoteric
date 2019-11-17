#include <memory>
#include <stdarg.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include <dirent.h>
#include <unistd.h>

#include "debug.h"
#include "config.h"
#include "utilities.h"

#define sync() sync(); system("sync");

using std::ifstream;
using std::ofstream;
using std::string;

Config::Config(string const &prefix) {
    DEBUG("Config::Skin - enter - prefix : %s", prefix.c_str());
    this->prefix = prefix;
    this->isDirty = false;
}

Config::~Config() {
    TRACE("Config::~Config");
}

string Config::toString() {

    vector<string> vec;

    vec.push_back("# gmenunx config file");
    vec.push_back("# lines starting with a # are ignored");

    // strings
    vec.push_back(string_format("skin=\"%s\"", this->skin().c_str()));
    vec.push_back(string_format("performance=\"%s\"", this->performance().c_str()));
    vec.push_back(string_format("tvOutMode=\"%s\"", this->tvOutMode().c_str()));
    vec.push_back(string_format("lang=\"%s\"", this->lang().c_str()));
    vec.push_back(string_format("batteryType=\"%s\"", this->batteryType().c_str()));
    vec.push_back(string_format("sectionFilter=\"%s\"", this->sectionFilter().c_str()));
    vec.push_back(string_format("launcherPath=\"%s\"", this->launcherPath().c_str()));

    // ints
    vec.push_back(string_format("buttonRepeatRate=%i", this->buttonRepeatRate()));
    vec.push_back(string_format("resolutionX=%i", this->resolutionX()));
    vec.push_back(string_format("resolutionY=%i", this->resolutionY()));
    vec.push_back(string_format("videoBpp=%i", this->videoBpp()));

    vec.push_back(string_format("backlightLevel=%i", this->backlightLevel()));
    vec.push_back(string_format("backlightTimeout=%i", this->backlightTimeout()));
    vec.push_back(string_format("powerTimeout=%i", this->powerTimeout()));

    vec.push_back(string_format("minBattery=%i", this->minBattery()));
    vec.push_back(string_format("maxBattery=%i", this->maxBattery()));

    vec.push_back(string_format("cpuMin=%i", this->cpuMin()));
    vec.push_back(string_format("cpuMax=%i", this->cpuMax()));
    vec.push_back(string_format("cpuMenu=%i", this->cpuMenu()));

    vec.push_back(string_format("globalVolume=%i", this->globalVolume()));
    vec.push_back(string_format("outputLogs=%i", this->outputLogs()));

    vec.push_back(string_format("saveSelection=%i", this->saveSelection()));
    vec.push_back(string_format("section=%i", this->section()));
    vec.push_back(string_format("link=%i", this->link()));

    vec.push_back(string_format("version=%i", this->version()));
    
    std::string s;
    for (const auto &piece : vec) s += (piece + "\n");
    return s;
}
    
bool Config::save() {
    TRACE("Config::save - enter");
    if (this->isDirty) {
        string fileName = this->prefix + CONFIG_FILE_NAME;
        TRACE("Config::save - saving to : %s", fileName.c_str());
        std::ofstream config(fileName.c_str());
        if (config.is_open()) {
            config << this->toString();
            config.close();
            sync();
            this->isDirty = false;
        }
    }
    TRACE("Config::save - exit");
    return true;
}

bool Config::loadConfig() {
    TRACE("Config::loadConfig - enter");
    this->reset();
    if (this->fromFile()) {
        this->constrain();
        isDirty = false;
        return true;
    }
    return false;
}

/* Private methods */

void Config::reset() {
    TRACE("Config::reset - enter");

     //strings
    this->skin_ = "Default";
    this->performance_ = "On demand";
    this->tvOutMode_ = "NTSC";
    this->lang_ = "";
    this->batteryType_ = "BL-5B";
    this->sectionFilter_ = "";

    if (dirExists(EXTERNAL_LAUNCHER_PATH)) {
        this->launcherPath(EXTERNAL_LAUNCHER_PATH);
    } else this->launcherPath(HOME_DIR);

    // ints
    this->buttonRepeatRate_ = 10;
    this->resolutionX_ = 320;
    this->resolutionY_ = 240;
    this->videoBpp_ = 32;

    this->powerTimeout_ = 10;
    this->backlightTimeout_ = 30;
    this->backlightLevel_ = 70;

    this->minBattery_ = 0;
    this->maxBattery_ = 5;

    this->cpuMin_ = 342;
    this->cpuMax_ = 996;
    this->cpuMenu_ = 600;

    this->globalVolume_ = 60;
    this->outputLogs_ = 0;

    this->saveSelection_ = 1;
    this->section_ = 1;
    this->link_ = 1;

    this->version_ = CONFIG_CURRENT_VERSION;

    TRACE("Config::reset - exit");
    return;
}

void Config::constrain() {

	evalIntConf( &this->backlightTimeout_, 30, 10, 300);
	evalIntConf( &this->powerTimeout_, 10, 1, 300);
	evalIntConf( &this->outputLogs_, 0, 0, 1 );
	evalIntConf( &this->cpuMax_, 642, 200, 1200 );
	evalIntConf( &this->cpuMin_, 342, 200, 1200 );
	evalIntConf( &this->cpuMenu_, 600, 200, 1200 );
	evalIntConf( &this->globalVolume_, 60, 1, 100 );
	evalIntConf( &this->videoBpp_, 16, 8, 32 );
	evalIntConf( &this->backlightLevel_, 70, 1, 100);
	evalIntConf( &this->minBattery_, 0, 0, 5);
	evalIntConf( &this->maxBattery_, 5, 0, 5);
	evalIntConf( &this->version_, CONFIG_CURRENT_VERSION, 1, 999);

    if (!this->saveSelection()) {
        this->section(0);
        this->link(0);
    }

	if (this->performance() != "Performance") 
		this->performance("On demand");
	if (this->tvOutMode() != "PAL") 
		this->tvOutMode("NTSC");
    if (!dirExists(this->launcherPath())) {
        this->launcherPath(HOME_DIR);
    }

}

bool Config::fromFile() {
    TRACE("Config::fromFile - enter");
    bool result = false;
    string fileName = this->prefix + CONFIG_FILE_NAME;
    TRACE("Config::fromFile - loading config file from : %s", fileName.c_str());

	if (fileExists(fileName)) {
		TRACE("Config::fromFile - config file exists");
		std::ifstream confstream(fileName.c_str(), std::ios_base::in);
		if (confstream.is_open()) {
			string line;
			while (getline(confstream, line, '\n')) {
				line = trim(line);
                if (0 == line.length()) continue;
                if ('#' == line[0]) continue;
				string::size_type pos = line.find("=");
                if (string::npos == pos) continue;
                
				string name = trim(line.substr(0,pos));
				string value = trim(line.substr(pos+1,line.length()));

                if (0 == value.length()) continue;

                TRACE("Config::fromFile - handling kvp - %s = %s", name.c_str(), value.c_str());

                // strings
                if (name == "skin") {
                    this->skin(stripQuotes(value));
                } else if (name == "performance") {
                    this->performance(stripQuotes(value));
                } else if (name == "tvOutMode") {
                    this->tvOutMode(stripQuotes(value));
                } else if (name == "lang") {
                    this->lang(stripQuotes(value));
                } else if (name == "batteryType") {
                    this->batteryType(stripQuotes(value));
                } else if (name == "sectionFilter") {
                    this->sectionFilter(stripQuotes(value));
                } else if (name == "launcherPath") {
                    this->launcherPath(stripQuotes(value));
                } 

                // ints
                else if (name == "buttonRepeatRate") {
                    this->buttonRepeatRate(atoi(value.c_str()));
                } else if (name == "resolutionX") {
                    this->resolutionX(atoi(value.c_str()));
                } else if (name == "resolutionY") {
                    this->resolutionY(atoi(value.c_str()));
                } else if (name == "videoBpp") {
                    this->videoBpp(atoi(value.c_str()));
                } else if (name == "backlightLevel") {
                    this->backlightLevel(atoi(value.c_str()));
                } else if (name == "backlightTimeout") {
                    this->backlightTimeout(atoi(value.c_str()));
                } else if (name == "powerTimeout") {
                    this->powerTimeout(atoi(value.c_str()));
                } else if (name == "minBattery") {
                    this->minBattery(atoi(value.c_str()));
                } else if (name == "maxBattery") {
                    this->maxBattery(atoi(value.c_str()));
                } else if (name == "cpuMin") {
                    this->cpuMin(atoi(value.c_str()));
                } else if (name == "cpuMax") {
                    this->cpuMax(atoi(value.c_str()));
                } else if (name == "cpuMenu") {
                    this->cpuMenu(atoi(value.c_str()));
                } else if (name == "globalVolume") {
                    this->globalVolume(atoi(value.c_str()));
                } else if (name == "outputLogs") {
                    this->outputLogs(atoi(value.c_str()));
                } else if (name == "saveSelection") {
                    this->saveSelection(atoi(value.c_str()));
                } else if (name == "section") {
                    this->section(atoi(value.c_str()));
                } else if (name == "link") {
                    this->link(atoi(value.c_str()));
                } else if (name == "version") {
                    this->version(atoi(value.c_str()));
                }

            };
            confstream.close();
            result = true;
        }
    }
    TRACE("Config::fromFile - exit");
    return result;
}

std::string Config::stripQuotes(std::string const &input) {
    string result = input;
    if (input.at(0) == '"' && input.at(input.length() - 1) == '"') {
        result = input.substr(1, input.length() - 2);
    }
    return result;
}

