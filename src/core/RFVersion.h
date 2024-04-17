/*******************************************************************************
File:         Version.h
Project:      ReferenceFinder
Purpose:      Version macros used in About box and Info.plist on Mac
Author:       Robert J. Lang
Modified by:  
Created:      2006-05-07
Copyright:    ©2006-2007 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _RFVERSION_H_
#define _RFVERSION_H_

/*****
Define version macros as symbols and strings. Nothing below these 5 lines
needs to be changed.
*****/

#define APP_FAMILY ReferenceFinder
#define VERSION_MAJOR 4
#define VERSION_MINOR 0
#define VERSION_BUGFIX 1
#define VERSION_BUILD 20070210

/******************************************************************************/

/* Two-stage construction of string from symbol */
#define MACRO_STRINGIZE(x) #x
#define MACRO_MAKE_STR(x) MACRO_STRINGIZE(x)

/* Name of the application family */
#define APP_FAMILY_STR MACRO_MAKE_STR(APP_FAMILY)

/* Name of the executable as string */
#define APP_V_NAME_STR (APP_FAMILY_STR " " \
  MACRO_MAKE_STR(VERSION_MAJOR))
#define APP_V_M_B_NAME_STR (APP_FAMILY_STR " " \
  MACRO_MAKE_STR(VERSION_MAJOR) "." \
  MACRO_MAKE_STR(VERSION_MINOR) "." \
  MACRO_MAKE_STR(VERSION_BUGFIX))

/* Build code as string (used in About box) */
#define BUILD_CODE_STR MACRO_MAKE_STR(VERSION_BUILD)

/* Symbols needed for Info.plist; ugly multistage construction because
  they need to be symbols, not strings, no quotes, no whitespace */
#define MACRO_PASTE_5(a, b, c, d, e) a##b##c##d##e
#define MACRO_PASTE_5A(a, b, c, d, e) MACRO_PASTE_5(a, b, c, d, e)
#define MAC_CFBUNDLE_SHORT_VERSION_STRING \
  MACRO_PASTE_5A(VERSION_MAJOR, ., VERSION_MINOR, ., VERSION_BUGFIX)
#define MACRO_PASTE_7(a, b, c, d, e, f, g) a##b##c##d##e##f##g
#define MACRO_PASTE_7A(a, b, c, d, e, f, g) MACRO_PASTE_7(a, b, c, d, e, f, g)
#define MAC_CFBUNDLE_VERSION \
  MACRO_PASTE_7A(VERSION_MAJOR, ., VERSION_MINOR, ., VERSION_BUGFIX, \
  ., VERSION_BUILD)

#endif // _RFVERSION_H_
