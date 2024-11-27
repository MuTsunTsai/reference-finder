/*******************************************************************************
File:         Version.h
Project:      ReferenceFinder
Purpose:      Version macros used in About box and Info.plist on Mac
Author:       Robert J. Lang
Modified by:  Mu-Tsun Tsai
Created:      2006-05-07
Copyright:    ©2006-2007 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#pragma once

/*****
Define version macros as symbols and strings.
NOTE: Do not edit these directly, as these values now sync with package.json automatically.
*****/

#define APP_FAMILY ReferenceFinder
enum {
	VERSION_MAJOR = 4,
	VERSION_MINOR = 4,
	VERSION_BUGFIX = 2,
	VERSION_BUILD = 20240815
};

/******************************************************************************/

/* Two-stage construction of string from symbol */
#define MACRO_STRINGIZE(x) #x
#define MACRO_MAKE_STR(x) MACRO_STRINGIZE(x)

/* Name of the application family */
#define APP_FAMILY_STR MACRO_MAKE_STR(APP_FAMILY)

/* Name of the executable as string */
#define APP_V_NAME_STR (APP_FAMILY_STR " " MACRO_MAKE_STR(VERSION_MAJOR))
#define APP_V_M_B_NAME_STR (APP_FAMILY_STR " " MACRO_MAKE_STR(VERSION_MAJOR) "." MACRO_MAKE_STR(VERSION_MINOR) "." MACRO_MAKE_STR(VERSION_BUGFIX))

/* Build code as string (used in About box) */
#define BUILD_CODE_STR MACRO_MAKE_STR(VERSION_BUILD)
