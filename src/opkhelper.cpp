#include <opk.h>
#include <list>
#include <string.h>

#include "opkhelper.h"
#include "desktopfile.h"
#include "utilities.h"
#include "debug.h"
#include "constants.h"

// TODO :: fix me to a constants file?
const string OPK_PLATFORM = "gcw0";

myOpk::myOpk() {
    this->fileName_ = "";
    this->fullPath_ = "";
    this->metadata_ = "";
    this->category_ = "";
    this->name_ = "";
    this->comment_ = "";
    this->exec_ = "";
    this->manual_ = "";
    this->icon_ = "";
    this->selectorDir_ = "";
    this->selectorFilter_ = "";
    this->type_ = "";
    this->mimeType_ = "";
    this->needsDownscaling_ = false;
    this->startupNotify_ = false;
    this->terminal_ = false;
}

std::string myOpk::params() {
     return "-m " + this->metadata_ + " \"" + this->fullPath_ + "\"";
}

void myOpk::constrain() {
    TRACE("enter");
    if (!this->exec_.empty() && this->selectorDir().empty()) {
		for (auto token : myOpk::LauncherTokens) {
			if (this->exec_.find(token) != this->exec_.npos) {
				TRACE("exec takes a token");
				this->selectorDir(EXTERNAL_CARD_PATH);
                this->exec_ = "";
				break;
			}
		}
    }
    TRACE("exit");
}

bool myOpk::load(OPK * opk) {
                
    const char *key, *val;
    size_t lkey, lval;
    int ret;
                
    while ((ret = opk_read_pair(opk, &key, &lkey, &val, &lval))) {
        if (ret < 0) {
            ERROR("Unable to read meta-data");
            return false;
        }

        // skip empty values
        if (0 == lval) 
            continue;

        char buf[lval + 1];
        sprintf(buf, "%.*s", (int)lval, val);
        string value = buf;
        
        char keyBuf[lkey + 1];
        sprintf(keyBuf, "%.*s", (int)lkey, key);
        const string & myKey = keyBuf;
        std::string loweredKey = toLower(myKey);

        TRACE("extracted - raw key : %s - lowered key : %s - value : %s", keyBuf, loweredKey.c_str(), value.c_str());

        if (loweredKey == "categories") {
            string temp = value;
            std::size_t pos = temp.find(';');
            if (pos != temp.npos)
                temp = temp.substr(0, pos);
            this->category(temp);
        } else if (loweredKey == "name") {
            this->name(value);
        } else if (loweredKey == "comment") {
            this->comment(value);
        } else if (loweredKey == "terminal") {
            this->terminal(!strncmp(val, "true", lval));
        } else if (loweredKey == "x-od-manual") {
            this->manual(value);
        } else if (loweredKey == "icon") {
            this->icon(value);
        } else if (loweredKey == "exec") {
            this->exec(value);
        } else if (loweredKey == "selectordir") {
            this->selectorDir(value);
        } else if (loweredKey == "selectorfilter") {
            this->selectorFilter(value);
        } else if (loweredKey == "type") {
            this->type(value);
        } else if (loweredKey == "startupnotify") {
            this->startupNotify(!strncmp(val, "true", lval));
        } else if (loweredKey == "x-od-needsdownscaling") {
            this->needsDownscaling(!strncmp(val, "true", lval));
        } else if (loweredKey == "mimetype") {
            this->mimeType(value);
        } else if (loweredKey == "x-od-needsgsensor") {
            // TODO :: dont eat it
        } else if (loweredKey == "x-od-needsjoystick") {
            // TODO :: dont eat it
        } else if (loweredKey == "version") {
            // TODO :: dont eat it
        } else {
            WARNING("Unrecognized OPK link option: '%s'", loweredKey.c_str());
        }
    }
    this->constrain();
    return true;
}

// accessors
std::string myOpk::fileName() const { return this->fileName_; }
void myOpk::fileName(std::string val) { this->fileName_ = val; }

std::string myOpk::fullPath() const { return this->fullPath_; }
void myOpk::fullPath(std::string val) { this->fullPath_ = val; }

std::string myOpk::metadata() const { return this->metadata_; }
void myOpk::metadata(std::string val) { this->metadata_ = val; }

std::string myOpk::category() const { return this->category_; }
void myOpk::category(std::string val) { this->category_ = val; }

std::string myOpk::name() const { return this->name_; }
void myOpk::name(std::string val) { this->name_ = val; }

std::string myOpk::comment() const { return this->comment_; }
void myOpk::comment(std::string val) { this->comment_ = val; }

std::string myOpk::exec() const { return this->exec_; }
void myOpk::exec(std::string val) { this->exec_ = val; }

std::string myOpk::manual() const { return this->manual_; }
void myOpk::manual(std::string val) { this->manual_ = val; }

std::string myOpk::icon() const { return this->icon_; }
void myOpk::icon(std::string val) { this->icon_ = val; }

std::string myOpk::selectorDir() const { return this->selectorDir_; }
void myOpk::selectorDir(std::string val) { this->selectorDir_ = val; }

std::string myOpk::selectorFilter() const { return this->selectorFilter_; }
void myOpk::selectorFilter(std::string val) { this->selectorFilter_ = val; }

std::string myOpk::type() const { return this->type_; }
void myOpk::type(std::string val) { this->type_ = val; }

std::string myOpk::mimeType() const { return this->mimeType_; }
void myOpk::mimeType(std::string val) { this->mimeType_ = val; }

bool myOpk::needsDownscaling() const { return this->needsDownscaling_; }
void myOpk::needsDownscaling(bool val) { this->needsDownscaling_ = val; }

bool myOpk::startupNotify() const { return this->startupNotify_; }
void myOpk::startupNotify(bool val) { this->startupNotify_ = val; }

bool myOpk::terminal() const { return this->terminal_; }
void myOpk::terminal(bool val) { this->terminal_ = val; }

std::list<myOpk> * OpkHelper::ToOpkList(const std::string & path) {
    TRACE("enter : %s", path.c_str());

    if (!fileExists(path)) {
        return NULL;
    }

    TRACE("opening opk : %s", path.c_str());
    struct OPK *opk = opk_open(path.c_str());
    if (!opk) {
        ERROR("Unable to open OPK %s", path.c_str());
        return NULL;
    }
    TRACE("opened opk successfully");

    std::list<myOpk> * results = new std::list<myOpk>();

    //TRACE("meta outer loop");
    for (;;) {
        unsigned int i;
        bool has_metadata = false;
        const char *name;

        //TRACE("meta inner loop");
        for (;;) {
            std::string::size_type pos;
            int ret = opk_open_metadata(opk, &name);
            if (ret < 0) {
                ERROR("Error while loading meta-data for %s", path.c_str());
                break;
            } else if (!ret) {
                break;
            }

            /* Strip .desktop */
            string metadata = name;
            TRACE("extracted metadata : %s", metadata.c_str());
            pos = metadata.rfind('.');
            string platform = metadata.substr(0, pos);
            pos = platform.rfind('.');
            platform = platform.substr(pos + 1);

            //TRACE("resolved meta data to : %s", platform.c_str());
            if (platform == OPK_PLATFORM || platform == "all") {
                TRACE("metadata matches platform : %s", OPK_PLATFORM.c_str());

                myOpk opkFile;
                opkFile.metadata(metadata);
                opkFile.fileName(base_name(path));
                opkFile.fullPath(path);
                if (opkFile.load(opk)) {
                    results->push_back(opkFile);
                } else {
                    WARNING("Couldn't load opk : %s (%s)", name, path.c_str());
                }
            }
        }

        if (!has_metadata)
             break;
    }

    opk_close(opk);
    TRACE("closed opk");
    return results;

}