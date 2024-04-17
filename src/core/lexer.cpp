/* ***************************************************************************

Lexer: a trivial lexical analyzer for ReferenceFinder
Originally a lex ruleset, reimplemented due to portability issues.

Author: C.A.Furuti

Subject to same distribution conditions of ReferenceFinder, by R.J.Lang

Copyright (c) 2003 Carlos A. Furuti www.progonos.com/furuti

*************************************************************************** */

#include <cctype>
#include <iostream> // debug only
#include "lexer.h"

int Lexer::next () {
  /* Status map for DFA:
     <0>\.<1>[0-9]<2>[0-9]*[eE]<4>[+-]?<5>[0-9]<6>[0-9]*<3>
          <1>.<-1>
                  <2>.<3>
                               <4>[0-9]<6>
                               <4>.<-1>
                                       <5>.<-1>
     <0>"(\\ | \" | [^"])*"<stringTk>
     <0>[0-9]<7>\.<2>
             <7>[eE]<4>
             <7>[0-9]*<3>
     <3><numberTk>
     <0>[A-Za-z]<8>[A-Za-z0-9_]*<wordTk>
     <0>.<other>
   */
  register int state = 0, k;
  lexemeStart = current;
  lexeme.erase ();
  while (1) {
#if 0 // debugging only
    std::cerr << "LX " << state << " " << lastR << " '" << 
      static_cast<char>(lastR) << "' <" << text << ">\n";
#endif
    switch (state) {
    case 0:
      do {
	k = nextK ();
      } while (std::isspace (k));
      if (! k)
	return 0;
      else 
	if (k == '.') {
	  add (); state = 1;
	} else
	  if (std::isdigit (k)) {
	    add (); state = 7;
	  } else
	    if (std::isalpha (k)) {
	      add (); state = 8;
	    } else
	      if (k == '"') {
		bool escaped = false;
		while ((k = nextK ())) {
		  if (escaped) {
		    add (k);
		    escaped = false;
		  } else
		    if (k == '\\')
		      escaped = true;
		    else
		      if (k == '"') {
			/* we pushback it, so the closing quote is a separate
                           token - therefore the parser must expect _2_ tokens,
                           '"'[^"]* and '"'. It's a bit strange, but otherwise
                           we'd need to introduce error reporting in the lexer,
                           for dealing with unterminated strings. */
                        pushB ();
			break;
                      } else
			add ();
		}
		return stringTk;
	      } else
		return k;
      break;
    case 1:
      if (std::isdigit (k = nextK ())) {
	add (); state = 2;
      } else {
	pushB ();
	state = -1;
      }
      break;
    case 2:
      k = nextK ();
      if (k == 'e' || k == 'E') {
	add (); state = 4;
      } else
	if (! std::isdigit (k))
	  state = 3;
	else
	  add ();
      break;
    case 3:
      pushB ();
      return numberTk;
    case 4:
      k = nextK ();
      if (k == '+' || k == '-') {
	add (); state = 5;
      } else
	if (std::isdigit (k)) {
	  add (); state = 6;
	} else
	  state = -1;
      break;
    case 5:
      if (std::isdigit (k = nextK ())) {
	add (); state = 6;
      } else
	state = -1;
      break;
    case 6:
      if (! std::isdigit (k = nextK ()))
	state = 3;
      else
	add ();
      break;
    case 7:
      k = nextK ();
      if (k == '.') {
	add (); state = 2;
      } else
	if (k == 'e' || k == 'E') {
	  add (); state = 4;
	} else
	  if (std::isdigit (k))
	    add ();
	  else
	    state = 3;
      break;
    case 8:
      while (std::isalnum (k = nextK ()) || k == '_')
	add ();
      pushB ();
      return wordTk;
    case -1:
      //      current = lexemeStart + 1;
      return lexeme [0];
    default:
      std::cerr << "INTERNAL LEXER ERROR\n";
      return 0;
    }
  }
}
