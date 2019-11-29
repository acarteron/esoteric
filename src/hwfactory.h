#ifndef _HWFACTORY_
#define _HWFACTORY_

#include <unistd.h>
#include <sys/statvfs.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <errno.h>

#include "utilities.h"
#include "constants.h"
#include "debug.h"

class IHardware {
    protected:

        int16_t curMMCStatus;

        const string BLOCK_DEVICE;
        const string EXTERNAL_MOUNT_DEVICE;
        const string EXTERNAL_MOUNT_POINT;
        const string EXTERNAL_MOUNT_FORMAT = "auto";

    public:
        IHardware();
        virtual bool getTVOutStatus() = 0;
        virtual void setTVOut(string TVOut) = 0;

        virtual void setPerformanceMode(std::string alias) = 0;
        virtual std::string getPerformanceMode() = 0;
        virtual uint32_t setCPUSpeed(uint32_t mhz) = 0;

        virtual void ledOn(int flashSpeed = 250) = 0;
        virtual void ledOff() = 0;

        /*!
        Reads the current battery state and returns a number representing it's level of charge
        @return A number representing battery charge. 0 means fully discharged. 
        5 means fully charged. 6 represents charging.
        */
        virtual int getBatteryLevel() = 0;

        virtual int getVolumeLevel() = 0;
        virtual int setVolumeLevel(int val) = 0;

        virtual int getBacklightLevel() = 0;
        virtual int setBacklightLevel(int val) = 0;

        string mountSd() {
            TRACE("enter");
            string command = "mount -t " + EXTERNAL_MOUNT_FORMAT + " " + EXTERNAL_MOUNT_DEVICE + " " + EXTERNAL_MOUNT_POINT + " 2>&1";
            string result = exec(command.c_str());
            TRACE("result : %s", result.c_str());
            system("sleep 1");
            this->checkUDC();
            return result;
        }

        string umountSd() {
            sync();
            string command = "umount -fl " + EXTERNAL_MOUNT_POINT + " 2>&1";
            string result = exec(command.c_str());
            system("sleep 1");
            this->checkUDC();
            return result;
        }

        void checkUDC() {
            TRACE("enter");
            curMMCStatus = MMC_ERROR;
            unsigned long size;
            TRACE("reading " + BLOCK_DEVICE);
            std::ifstream fsize(BLOCK_DEVICE.c_str(), std::ios::in | std::ios::binary);
            if (fsize >> size) {
                if (size > 0) {
                    // ok, so we're inserted, are we mounted
                    TRACE("size was : %lu, reading /proc/mounts", size);
                    std::ifstream procmounts( "/proc/mounts" );
                    if (!procmounts) {
                        curMMCStatus = MMC_ERROR;
                        WARNING("couldn't open /proc/mounts");
                    } else {
                        std::string line;
                        std::size_t found;
                        curMMCStatus = MMC_UNMOUNTED;
                        while (std::getline(procmounts, line)) {
                            if ( !(procmounts.fail() || procmounts.bad()) ) {
                                found = line.find("mcblk1");
                                if (found != std::string::npos) {
                                    curMMCStatus = MMC_MOUNTED;
                                    TRACE("inserted && mounted because line : %s", line.c_str());
                                    break;
                                }
                            } else {
                                curMMCStatus = MMC_ERROR;
                                WARNING("error reading /proc/mounts");
                                break;
                            }
                        }
                        procmounts.close();
                    }
                } else {
                    curMMCStatus = MMC_MISSING;
                    TRACE("not inserted");
                }
            } else {
                curMMCStatus = MMC_ERROR;
                WARNING("error, no card");
            }
            fsize.close();
            TRACE("exit - %i",  curMMCStatus);
        }

        void formatSdCard() {
            // TODO :: implement me
        }

        string getDiskFree(const char *path) {
            TRACE("enter - %s", path);
            string df = "N/A";
            struct statvfs b;

            if (statvfs(path, &b) == 0) {
                TRACE("read statvfs ok");
                // Make sure that the multiplication happens in 64 bits.
                uint32_t freeMiB = ((uint64_t)b.f_bfree * b.f_bsize) / (1024 * 1024);
                uint32_t totalMiB = ((uint64_t)b.f_blocks * b.f_frsize) / (1024 * 1024);
                TRACE("raw numbers - free: %lu, total: %lu, block size: %lu", b.f_bfree, b.f_blocks, b.f_bsize);
                std::stringstream ss;
                if (totalMiB >= 10000) {
                    ss << (freeMiB / 1024) << "." << ((freeMiB % 1024) * 10) / 1024 << " / "
                    << (totalMiB / 1024) << "." << ((totalMiB % 1024) * 10) / 1024 << " GiB";
                } else {
                    ss << freeMiB << " / " << totalMiB << " MiB";
                }
                std::getline(ss, df);
            } else WARNING("statvfs failed with error '%s'.", strerror(errno));
            TRACE("exit");
            return df;
        }
};

class HwRg350 : IHardware {
    private:

		enum LedAllowedTriggers {
			NONE = 0, 
			TIMER
		};

		std::string ledMaxBrightness_;
        std::string performanceMode_ = "ondemand";
        int volumeLevel_ = 0;
        int backlightLevel_ = 0;
		const string LED_PREFIX = "/sys/class/leds/power/";
		const string LED_MAX_BRIGHTNESS_PATH = LED_PREFIX + "max_brightness";
		const string LED_BRIGHTNESS_PATH = LED_PREFIX + "brightness";
		const string LED_DELAY_ON_PATH = LED_PREFIX + "delay_on";
		const string LED_DELAY_OFF_PATH = LED_PREFIX + "delay_off";
		const string LED_TRIGGER_PATH = LED_PREFIX + "trigger";
        const string GET_VOLUME_PATH = "/usr/bin/alsa-getvolume default PCM";
        const string SET_VOLUME_PATH = "/usr/bin/alsa-setvolume default PCM "; // keep trailing space
        const string BACKLIGHT_PATH = "/sys/class/backlight/pwm-backlight/brightness";

		string triggerToString(LedAllowedTriggers t) {
            TRACE("mode : %i", t);
            switch(t) {
                case TIMER:
                    return "timer";
                    break;
                default:
                    return "none";
                    break;
            };
        }
    
    protected:
        const string BLOCK_DEVICE = "/sys/block/mmcblk1/size";
        const string EXTERNAL_MOUNT_DEVICE = "/dev/mmcblk1p1";
        const string EXTERNAL_MOUNT_POINT = EXTERNAL_CARD_PATH;
        const string EXTERNAL_MOUNT_FORMAT = "auto";

     public:
        HwRg350() {
            TRACE("enter");
            this->ledMaxBrightness_ = fileReader(LED_MAX_BRIGHTNESS_PATH);
            this->getBacklightLevel();
            this->getVolumeLevel();
            TRACE(
                "brightness - max : %s, current : %i, volume : %i", 
                ledMaxBrightness_.c_str(), 
                this->backlightLevel_, 
                this->volumeLevel_
            );
        }

        bool getTVOutStatus() { return 0; };
        void setTVOut(string TVOut) { return; };

        std::string getPerformanceMode() { 
            TRACE("enter");
            this->performanceMode_ = "ondemand";
            if (fileExists("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor")) {
                this->performanceMode_ = fileReader("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
            }
            TRACE("exit - %s", this->performanceMode_.c_str());
            return full_trim(this->performanceMode_);
        };
        void setPerformanceMode(std::string alias) { 
            TRACE("enter - desired : %s", alias.c_str());
            std::string desired = toLower(alias);
            if (desired != this->performanceMode_) {
                TRACE("update needed : current %s vs. desired %s", this->performanceMode_.c_str(), desired.c_str());
                if (fileExists("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor")) {
                    procWriter("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", desired);
                    this->performanceMode_ = desired;
                }
            } else {
                TRACE("nothing to do");
            }
            TRACE("exit");
        };
        uint32_t setCPUSpeed(uint32_t mhz) { return mhz; };

        void ledOn(int flashSpeed = 250) {
            TRACE("enter");
            int limited = constrain(flashSpeed, 0, atoi(ledMaxBrightness_.c_str()));
            string trigger = triggerToString(LedAllowedTriggers::TIMER);
            TRACE("mode : %s - for %i", trigger.c_str(), limited);
            procWriter(LED_TRIGGER_PATH, trigger);
            procWriter(LED_DELAY_ON_PATH, limited);
            procWriter(LED_DELAY_OFF_PATH, limited);
            TRACE("exit");
        };
        void ledOff() { 
            TRACE("enter");
            string trigger = triggerToString(LedAllowedTriggers::NONE);
            TRACE("mode : %s", trigger.c_str());
            procWriter(LED_TRIGGER_PATH, trigger);
            procWriter(LED_BRIGHTNESS_PATH, ledMaxBrightness_);
            TRACE("exit");
            return;
        };

        int getBatteryLevel() { 
            int online, result = 0;
            sscanf(fileReader("/sys/class/power_supply/usb/online").c_str(), "%i", &online);
            if (online) {
                result = 6;
            } else {
                int battery_level = 0;
                sscanf(fileReader("/sys/class/power_supply/battery/capacity").c_str(), "%i", &battery_level);
                TRACE("raw battery level - %i", battery_level);
                if (battery_level >= 100) result = 5;
                else if (battery_level > 80) result = 4;
                else if (battery_level > 60) result = 3;
                else if (battery_level > 40) result = 2;
                else if (battery_level > 20) result = 1;
                result = 0;
            }
            TRACE("scaled battery level : %i", result);
            return result;
        };

        int getVolumeLevel() { 
            TRACE("enter");
            int vol = -1;
            std::string result = exec(GET_VOLUME_PATH.c_str());
            if (result.length() > 0) {
                vol = atoi(trim(result).c_str());
            }
            // scale 0 - 31, turn to percent
            vol = vol * 100 / 31;
            this->volumeLevel_ = vol;
            TRACE("exit : %i", this->volumeLevel_);
            return this->volumeLevel_;
        };
        int setVolumeLevel(int val) { 
            TRACE("enter - %i", val);
            if (val < 0) val = 100;
            else if (val > 100) val = 0;
            if (val == this->volumeLevel_) 
                return val;
            int rg350val = (int)(val * (31.0f/100));
            TRACE("rg350 value : %i", rg350val);
            std::stringstream ss;
            std::string cmd;
            ss << SET_VOLUME_PATH << rg350val;
            std::getline(ss, cmd);
            TRACE("cmd : %s", cmd.c_str());
            std::string result = exec(cmd.c_str());
            TRACE("result : %s", result.c_str());
            this->volumeLevel_ = val;
            return val;
        };

        int getBacklightLevel() { 
            TRACE("enter");
            int level = 0;
            //force  scale 0 - 5
            string result = fileReader(BACKLIGHT_PATH);
            if (result.length() > 0) {
                level = atoi(trim(result).c_str()) / 51;
            }
            this->backlightLevel_ = level;
            TRACE("exit : %i", this->backlightLevel_);
            return this->backlightLevel_;
        };
        int setBacklightLevel(int val) { 
            TRACE("enter - %i", val);
            // wrap it
            if (val <= 0) val = 100;
            else if (val > 100) val = 0;
            int rg350val = (int)(val * (255.0f/100));
            TRACE("rg350 value : %i", rg350val);
            // save a write
            if (rg350val == this->backlightLevel_) 
                return val;

            if (procWriter(BACKLIGHT_PATH, rg350val)) {
                TRACE("success");
            } else {
                ERROR("Couldn't update backlight value to : %i", rg350val);
            }
            this->backlightLevel_ = val;
            return this->backlightLevel_;	
        };
};

class HwGeneric : IHardware {
     public:
        bool getTVOutStatus() { return 0; };
        void setTVOut(string TVOut) { return; };

        void setPerformanceMode(std::string alias) { return; };
        std::string getPerformanceMode() { return "default"; };
        uint32_t setCPUSpeed(uint32_t mhz) { return mhz; };

        void ledOn(int flashSpeed = 250) { return; };
        void ledOff() { return; };

        int getBatteryLevel() { return 100; };
        int getVolumeLevel() { return 100; };
        int setVolumeLevel(int val) { return val; };;

        int getBacklightLevel() { return 100; };
        int setBacklightLevel(int val) { return val; };
};

class HwFactory {
    static IHardware * GetHardware() {
        #ifdef TARGET_RG350
        return (IHardware*)new HwRg350();
        #else
        return (IHardware*)new HwGeneric();
        #endif
    }
};

#endif // _HWFACTORY_