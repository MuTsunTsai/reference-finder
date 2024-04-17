/* ***************************************************************************

Lexer: a trivial lexical analyzer for ReferenceFinder
Originally a lex ruleset, reimplemented due to portability issues.

Author: C.A.Furuti

Subject to same distribution conditions of ReferenceFinder, by R.J.Lang

Copyright (c) 2003 Carlos A. Furuti www.progonos.com/furuti

*************************************************************************** */

#ifndef LEXER_H

#define LEXER_H

#include <string>

class Lexer {
 public:
  // token classes, other characters returned directly; 0 == eof
  enum {numberTk = 257, wordTk = 258, stringTk = 259};
  /* Currently only reads from a string. If generalized, should
     be constructed from a stream
  */
  Lexer (const std::string txt) {
    text = txt;
    lastK = -1;
    lexemeStart = current = 0;
    lexeme.erase ();
  }
  // get next token
  int next ();
  // get latest lexeme
  const std::string token () {
    return lexeme;
  }
 private:
  /* copy of input source. For most cases extern input remains unchanged
     during analysis so this could really be a pointer. Also, in the remote
     chance this class is generalized for other input sources, this
     member would not be used.
   */
  std::string text;
  /* index of lexeme's start. Currently unused, could be used for smarter
     error recovery (presently RF's parser bails out at first error) */
  std::size_t lexemeStart;
  std::size_t current; // next reading position

  std::string lexeme; // current token
  int lastK, // pushback buffer
    lastR;  // last character read
  void add (int k = -1) {
    lexeme += (k == - 1 ? lastR : char(k));
  }
  int nextK () { // get next character
    int k;
    if (lastK >= 0) { // there's a pushback available
      k = lastK;
      lastK = -1;
    } else
      /* TODO: check and read from other sources */
      k = current < text.length () ? text.at (current++) : 0;
    return lastR = k;
  }
  void pushB () { // unreads a character
    lastK = lastR; 
  }
};

#endif
