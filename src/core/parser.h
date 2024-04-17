/* ***************************************************************************

Parser: a simple expression evaluator for ReferenceFinder

Author: C.A.Furuti

Subject to same distribution conditions of ReferenceFinder, by R.J.Lang

Copyright (c) 2003, 2006 Carlos A. Furuti www.progonos.com/furuti

*************************************************************************** */

#ifndef PARSER_H

#define PARSER_H

#include <string>
#include <iostream>
#include <map>
#include <set>
#include "lexer.h"

/* Expression parser.
   Features:
   - simple operators +, - [unary/binary], *, /, ^
   - functions sin [sine], cos [cosine], tan [tangent], sqrt [square root],
     deg2rad [degrees to radians]
   - variables (with names like "a", "A", "total_sum1"); may be set
     programatically or by the 'set(varName, "expression")' pseudofunction
   - lazy evaluation (e.g., 'set(a,"1")', 'set(b,"a+2")' and 'set(c,"3*b")'
     may be defined in any order)

   This is neither thread-safe nor intended for high-performance, repeated
   evaluation; in particular, no fancy optimization or bytecode compilation
   are implemented or planned.
 */
class Parser {
public:
  // Exception-like status condition
  struct Status {
    typedef enum {
      none,  // expression ok
      opExpected, // operator expected
      unknownId, // unknown name
      notAFunction, // function unknown
      extraInput, // extra input at end of text
      emptyInput, // empty text
      illegalWord, // illegal input
      zeroDivide, // division by zero
      illegalParameter, // illegal function parameter
      varExpected, // variable expected
      stringExpected, // quote-delimited string expected
      recursiveEval, // variable defined recursively
      tokenExpected, // constant lexeme expected
      cantHappen  // internal error
    } ErrType;
  
    Status(ErrType code = none, 
     std::string msg = "") : mErrType(code), mMessage (msg) {
    }
    bool isOK() {
      return mErrType == none;
    }
    std::string toString () const;
    ErrType GetErr() const;
    friend std::ostream& operator<<(std::ostream &s, const Status &val);
    private:
      ErrType mErrType;
      std::string mMessage;
  };

  // a variable or intermediate value
  struct Value {
    // identifier category
    enum Type {null, number, text, fsqrt, fsin, fcos, ftan, fdeg2rad, fset};
    Value (Type t = null, double val = 0) : type (t), num (val) {}
    Value (Type t, std::string val) : type (t), str (val) {}
    Value (std::string val) : type (text), str (val) {}
    Value (double val) : type (number), num (val) {}
    friend std::ostream& operator<<(std::ostream &s,
            const Parser::Value &val);
    Type type;    // category
    double num;  // numerical value, if <type> == num
    std::string str;  // value expression, if <type> == text
  };

  // parse string and evaluate expression
  Status evaluate (const std::string &text, double &result, 
       bool useDefault = false, 
       double defaultValue = 0);
  // updates or creates numerical variable
  static void setVariable (const std::string &name, double val = 0, 
         bool overwrite = true);
  // updates or creates variable given string
  static void setVariable (const std::string &name, const std::string &val,
         bool overwrite = true);
  // updates or creates variable given value
  static void setVariable (const std::string &name, const Value &val,
         bool overwrite = true);
  // retrieves a variable's value; throws an exception if not found
  static Value getVariable (const std::string &name) throw (Status);

 private:
  // parse string and evaluate expression
  Status internalEvaluate (std::string text, double &result, 
         bool useDefault = false, 
         double defaultValue = 0);
  static void initIds();

  Lexer *lexer;
  Status value (double &result);
  Status signedExpr (double &result);
  Status factor (double &);
  Status term (double &);
  Status expression (double &);
  int nextToken; // current token

  /* Identifier dictionary, shared by all instances. In ReferenceFinder
     there's at most one Parser active at any time, and there's no need to
     generalize this.
  */
  
  // identifier table
  static std::map <std::string, Value*> ids;

  /* Store variable names used while parsing a string. Used for:
     - detecting cyclically defined variables, like a=c+1, b=2*a, c=sqrt(a)
     - trivially optimizing multiple uses of a variable; e.g., in a=7,
       b=sqrt(5 + a), c=b+sin(b), when evaluating c, parse a and b only
       once
   */
  static std::map <std::string, Value> pendingVals;
};

#endif
