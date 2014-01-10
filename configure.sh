#!/bin/bash

function validate_yn {
    CHOICE=$1;

    if [ x"$CHOICE" == xy -o x"$CHOICE" == xn ]; then
	return 0;
    fi

    echo "$CHOICE is not a valid choice. Exiting... ";
    exit 1;
}

function write_config_h {
    LANGDEF=$1;
    FUZZY=$2;
    SHOW_DATE=$3;
    DEBUG=$4;

    echo "#define LANG $LANGDEF" > src/config.h
    [ $FUZZY == y ] && echo "#define FUZZY" >> src/config.h
    [ $SHOW_DATE == y ] && echo "#define SHOW_DATE" >> src/config.h
    [ $DEBUG == y ] && echo "#define DEBUG" >> src/config.h
}

function write_appinfo_json {
    LANG=$1;
    FUZZY=$2;
    SHOW_DATE=$3;
    DEBUG=$4;

    if [ $DEBUG == y ]; then
	WATCHFACE="false"
    else
	WATCHFACE="true"
    fi

    if [ $LANG == 1 ]; then
	if [ $FUZZY == y ]; then
	    if [ $SHOW_DATE == y ]; then
	        # This UUID is invalid
		UUID="f4180083-f9ad-4bba-98c0-b49f33f9710c"
		NAME="Dansk Fuzzy med dato"
	    else
		UUID="7bded28e-f202-436f-af71-066057a0643f"
		NAME="Dansk Fuzzy"
	    fi
	else
	    if [ $SHOW_DATE == y ]; then
		UUID="b9785296-7dcc-4071-a5ed-215e81669d66"
		NAME="Dansk Tekst med dato"
	    else
		UUID="e38dfc3e-1f96-4c0f-af6f-7860a567eb27"
		NAME="Dansk Tekst"
	    fi
	fi
    else
	if [ $FUZZY == y ]; then
	    if [ $SHOW_DATE == y ]; then
		UUID="21738945-1736-4100-855b-840aed2a060a"
		NAME="Islenskt Fuzzy with Date"
	    else
		UUID="434181c933-2445-1b81-d949-1f310f4d8e"
		NAME="Islenskt Fuzzy"
	    fi
	else
	    if [ $SHOW_DATE == y ]; then
		# This UUID is invalid
		UUID="fc05e6c50a-ae46-9588-4be1-60e47a463d"
		NAME="Islenskur Texti with Date"
	    else
		UUID="a10e86c1ad-7147-dd96-3635-94f80740e9"
		NAME="Islenskur Texti"
	    fi
	fi
    fi

    cat <<EOF > appinfo.json
{
  "uuid": "$UUID",
  "shortName": "$NAME",
  "longName": "$NAME",
  "companyName": "Tanis",
  "versionCode": 1,
  "versionLabel": "2.0.0",
  "watchapp": {
    "watchface": $WATCHFACE
  },
  "appKeys": {},
  "resources": {
    "media": [
	{
	    "type": "font",
	    "name": "FONT_SANSATION_LIGHT_32",
	    "file": "fonts/sansation/Sansation_Light.ttf"
	},
	{
	    "type": "font",
	    "name": "FONT_SANSATION_BOLD_32",
	    "file": "fonts/sansation/Sansation_Bold.ttf"
	}
    ]
  }
}
EOF
}

echo "This script will configure the app, according to your choices."

echo -n "Select 1 for Danish, 2 for Icelandic: "
read LANG

if [ x"$LANG" == x"1" ]; then
    LANGDEF="LANG_DANISH";
elif [ x"$LANG" == x"2" ]; then
    LANGDEF="LANG_ICELANDIC";
else
    echo "$LANG is not a valid choice. Exiting... ";
    exit 1;
fi

echo -n "Use fuzzy time? (y/n): "
read FUZZY
validate_yn "$FUZZY"

echo -n "Show date? (y/n): "
read SHOW_DATE
validate_yn "$SHOW_DATE"

echo -n "Enable debug? (y/n): "
read DEBUG
validate_yn "$DEBUG"

write_config_h $LANGDEF $FUZZY $SHOW_DATE $DEBUG;
write_appinfo_json $LANG $FUZZY $SHOW_DATE $DEBUG;



