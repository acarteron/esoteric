# 350teric Changelog

**1.0.11** (*Codename: *)

* Added
  * Quick launch the last game on 1st boot
    * Hold 'a' to bypass
    * Toggle on / off in Settings menu
  * Fast cache load option
    * Doesn't take the delta, just loads current state
    * Used automatically when in a quick launch mode

* Fixed
  * LED lower power boot loop

* Improved
  * Setup wrapped in crash handler and dialog

**1.0.10** (*Codename: I can't believe it's not battery*)

* Added
  * Accurate battery support
  * Try / Catch support
  * Toggle battery icons or percentage in Skin Settings ui
  * Hidden files and folders toggle for explorer

* Fixed

* Improved
  * HW abstraction extends to batteries now
  * Refactored UI transition code
  * Refactored String Helper code
  * Refactored File readers and writers

**1.0.9** (*Codename: The smell of dissapointment*)

* Added
  * Support for key combo's in input handler
    * Can be set via input.conf for any action
    * format is : action=combo,key_code_1,key_code_2,key_code_n
    * eg. power=combo,9,308 (this is for GKD350h)
  * Power button brings up power off / restart dialog on RG350, PG2
  * L1 & B on GKD350h brings up power off / restart dialog
  * New Device setting, Default CPU speed to launch apps at if nothing in the desktop file

* Fixed
  * Directory copy installer notification
  * File browser filters for empty string and no extension

* Improved
  * Cleaner session restore
  * More defensive 'save last selection' restore
  * Surface collection error checking
  * Only remove boot marker on clean exit from esoteric
  * Refactored Cpu code, added factory for determining JZ4770O capabilities
  * About reports cpu info

* Currently thwarted by....
  * GKD350h overclock registers....
  * closer but not happening still, and smells of closed source sadness

**1.0.8** (*Codename: Don't worry, it's just a background check*)

* Added
  * Backgrounds for favourites
    * Fullscreen or
    * Best fit in the link view space

* Fixed
  * Save last selection
  * Potential crash after making a favourite if no icon was created
  * Empty directory handling in browser

* Improved
  * Stay in rom browser after making a favourite
  * Add custom value in ui for preview width if it doesn't match a step of 5
  * Background image scaling and placing
  * File handlimg routines and path resolvers
  * Includes and ifdef's

**1.0.7** (*Codename: Meet the family*)

* Added
  * RG350
    * ALSA Mixer
    * Console apps in general
  * GKD350H
    * All standard opk / launcher functionality
    * Key mappings
    * Power off and reboot
    * Screen off timer
    * Power off timer
    * Screen brightness
    * Real Time Clock setting
    * Battery level reporting
    * Volume level adjust and reporting
  * PocketGo 2
    * All standard opk / launcher functionality
    * CPU performance guvernor (on demand / performance)
    * Key mappings
    * Power off and reboot
    * Screen off timer
    * Power off timer
    * Battery level reporting
    * Brightness control
    * Led control
    * Volume level adjust and reporting

* Fixed
  * Default clock speed reporting for non overclocked rg350
  * Allow zero volumes in device menu
  * Screen tearing on rg350 and gkd350h

* Improved
  * Tear down of the app on link launch

**1.0.6** (*Codename: Lay some skin on me*)

* Added
  * Seperate device specific input configs
  * Keyboard nav arrows
  * Skin settings for preview image size

* Fixed
  * Restore session handling
  * Set screen w & h back to config after determined once
  * Fullpaths for gkd350h poweroff and reboot
  * Manual file filter extension typo

* Improved
  * Overclocking detection and guard conditions
  * Performance mode detection and guard conditions
  * Upgrade

**1.0.5** (*Codename: TLOE: Links Hiding*)

* Added
  * Hide / show links
  * CPU underclock option for launcher, exposed in Device
  * CPU underclock for links
  * Default the date to build date if it's not set
  * ALSA mixer launcher
  * CPU freq or governor mode shown in About
  * Manual

* Fixed
  * Crash viewing logs from apps w/ control codes
  * Force vsync in any sdl version
  * Reset alt keymap file on launch. thanks @nebuleon
  * SDL_ShowCursor(SDL_DISABLE); comes after SDL Video setup now to avoid potential side effect
  * UI race condition removing an OPK

* Improved
  * Performance in HW polling
  * CPU scaling to hw layer
  * Clock types abstracted
  * Only set SDL Fullscreen flag if we're on unknown w/h

**1.0.4** (*Codename: Fight the power!*)

* Added
  * Lock for selector browsing when you edit a link
  * Auto Power off
    * Toggleable
    * Only activates when running on battery

* Fixed
  * Some links weren't editable that should be
  * Save config after hiding section
  * possible dangling slash in config file for external apps path
  * crash when info bar is smaller than icons

* Improved
  * Video initialisation is portable across devices
  * Refactored screen resolution consumers
  * Refactored inputManager
  * Refactored screenManager and powerManager to a common interface
  * screenManager now uses hw abstraction layer for blanking
  * powerManager now uses hw abstraction layer for power off

**1.0.3** (*Codename: Is that garlic?*)

* Added
  * Key repeat
  * More skinning docs in templates
  * Linux HW abstraction layer

* Fixed
  * Screen sleep if nothing happens after boot

* Improved
  * Optimised surface collection
  * Build scripts
  * HW abstraction layer layout
  * key bindings for linux HW layer

**1.0.1** (*Codename: I wanna die before i g-g-g-g-get old*)

* Fixed
  * button stutter repeats
  * polling for selected overlay image every render

**1.0.0** (*Codename: Boys don't cry (AKA: I must be new here).*)

* Renamed fork to 350teric
* Added
  * opk code
  * opk cache
  * favourites
  * previews
  * many new layout options
  * loader option
  * device menu
  * section (un)hiding
  * battery friendly rendering
  * internal stuff
