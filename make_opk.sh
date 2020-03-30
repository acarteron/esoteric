#!/bin/bash

APP_NAME="esoteric";
OPK_EXTENSION=".opk";
ASSETS_PATH="dist/RG350/esoteric";
argCount=$#
OPK_NAME="${APP_NAME}${OPK_EXTENSION}"
SUPPORTED_PLATFORMS=("gcw0" "retrofw")

# ----------------------------- #
echo "Enter make_opk.sh"

function makePackage () {

    if [[ $# -lt 1 ]]; then
	echo "make_opk needs a target"
	exit 1
    fi
    ASSETS_PATH="dist/$1"
    if [ ! -d ${ASSETS_PATH} ]; then
	echo "Can't find ${ASSETS_PATH}, You need to run a make dist first"
	exit 1
    fi

    # if (( ${argCount} > 1 )); then
    # 	echo "we got a name override : $2"
    # 	OPK_NAME="${2}"
    # fi
    # if (( ${argCount} > 2 )); then
    # 	echo "we got a version override : $3"
    # 	VERSION="${3}"
    # fi

    echo "Making opk : ${OPK_NAME}"

    FLIST="${ASSETS_PATH}/*"

    for myPlatform in ${SUPPORTED_PLATFORMS[*]}; do
	# create default.${myPlatform}.desktop
	cat > default.${myPlatform}.desktop <<EOF
[Desktop Entry]
Name=350teric
Comment=Esoteric App Launcher
Exec=${APP_NAME}
Terminal=false
Type=Application
StartupNotify=true
Icon=logo
Categories=applications;
X-OD-Manual=esoteric.man.txt
Version=${VERSION}
EOF

	FLIST="${FLIST} default.${myPlatform}.desktop"

    done

    # create opk
    if [ -f ${OPK_NAME} ]; then
	echo "removing already existing file : ${OPK_NAME}"
	rm -f ${OPK_NAME}
    fi

    mksquashfs ${FLIST} ${OPK_NAME} -all-root -no-xattrs -noappend -no-exports

    cat default.gcw0.desktop
    rm -f default.*.desktop

    echo "opk created at : ${OPK_NAME}"
    exit 0
}


function showHelp () {
    echo "Usage:"
	echo "    -p                 Make a package when \${target} = ['rg350']."
	echo "    -t target          Set \${target} = [${validTargets[@]}]" 
}

# ----------------------------------- #

if [ $# -eq 0 ]; then
	showHelp
	exit 0
fi


requestedTarget=""
doPackage="false"
target=""
doTarget="false"


validTargets=("RG350" "RG350VGA")

while getopts ":bcd:hiprst:uvV" opt; do

  case ${opt} in
	p )
		echo "package flag set"
		doPackage="true"
		;;
	t )
		echo "target flag set"
		requestedTarget=$OPTARG
		doTarget="true"
	  	;;
    \? )
      	showHelp
      	;;
    : )
      	echo "Invalid option: $OPTARG requires an argument" 1>&2
	  	showHelp
      	;;
  esac
done
shift $((OPTIND -1))



if [ ${doTarget} == "true" ]; then
    echo "checking ${requestedTarget} is a valid target"
    for myTarget in ${validTargets[*]}; do
	echo "checking ${myTarget} against ${requestedTarget}"
	if [ ${myTarget} == ${requestedTarget} ]; then
	    echo "setting target successfully to : ${myTarget}"
	    target=${myTarget}
	    break
	fi
    done
    if [[ -z ${target} ]]; then
	echo "${requestedTarget} is not a valid build target"
	echo "valid build targets are : [all ${validTargets[*]}]"
	exit 1
    fi
fi


if [ ${doPackage} == "true" ] && [[ ! -z ${target} ]]; then
	makePackage ${target}
fi
