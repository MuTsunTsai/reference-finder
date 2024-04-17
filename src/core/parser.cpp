/* ***************************************************************************

Parser: a simple expression evaluator for ReferenceFinder
Originally a yacc/bison grammar, reimplemented due to portability issues.

Author: C.A.Furuti

Subject to same distribution conditions of ReferenceFinder, by R.J.Lang

Copyright (c) 2003 Carlos A. Furuti www.progonos.com/furuti

*************************************************************************** */

#include <cmath>        // sin, cos, tan
#include <cstdlib>      // strtod
#include <iterator>
#include <sstream>

#include "parser.h"

// some systems don't define those in math.h
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_E
#define M_E 2.7182818284590452354
#endif


// symbol table
std::map <std::string, Parser::Value*> Parser::ids;
// variables whose value is pending 
std::map <std::string, Parser::Value> Parser::pendingVals;

/*** Ordinary recursive descent predictive parser ***/

/* value: number|var|function(expr)|(expr)
 */
Parser::Status Parser::value (double &result) {
  Status status;
  switch (nextToken) {
  case Lexer::numberTk: // constant
    result = std::strtod (lexer -> token ().c_str (), 0);
    nextToken = lexer -> next ();
    return status;
  case Lexer::wordTk: { // identifier
    Value *p = ids [lexer -> token ()];
    if (! p)
      return Status (Status::unknownId, lexer -> token ());

    switch (p -> type) {
    case Value::number: // simple variable
      result = p -> num;
      nextToken = lexer -> next ();
      break;
    case Value::text: { // text variable
      Value pending;
      std::map<std::string, Value>::iterator i 
        = pendingVals.find (lexer -> token ());
      if (i != pendingVals.end ()) {
        // found - we already needed it in this same expression
        if (i -> second.type == Value::null)
          // we still didn't know it - an evaluation cycle
          return Status (Status::recursiveEval, lexer -> token ());
        else
          // we already know its value, so save a parsing call
          result = i -> second.num;
      } else {
        // evaluate it in a nested parser...
        pendingVals [lexer -> token ()] = Value (Value::null);
        Parser pNested;
        status = pNested.internalEvaluate (p -> str, result);
        // ... and save its value
        pendingVals [lexer -> token ()] = Value (result);
      }
      nextToken = lexer -> next ();
      return status;
    }
    default: 
      { // function call
        double r1; // parameter's value
        std::string varName, varValue;
        
        if ((nextToken = lexer -> next ()) != '(')
          return Status (Status::tokenExpected, "(");

#if 1
        /* The pseudofunction set(variable, "subexpression") allows an
           expression to redefine any variable. It's useful for
           interactive testing, but not essential in ReferenceFinder,
           which updates variables via setVariable()
         */
        if (p -> type == Value::fset) {
          if ((nextToken = lexer -> next ()) != Lexer::wordTk)
            return Status (Status::varExpected);
          varName = lexer -> token ();
          if ((nextToken = lexer -> next ()) != ',')
            return Status (Status::tokenExpected, ",");
          if ((nextToken = lexer -> next ()) != Lexer::stringTk)
            return Status (Status::stringExpected);
          varValue = lexer -> token ();
          if ((nextToken = lexer -> next ()) != '"')
            return Status (Status::tokenExpected, "\"");
          if ((nextToken = lexer -> next ()) != ')')
            return Status (Status::tokenExpected, ")");
          nextToken = lexer -> next ();
          result = 0;
          setVariable (varName, varValue);
          return status;
        }
#endif        
        nextToken = lexer -> next ();
        if (! (status = expression (r1)).isOK ())
          return status;
        if (nextToken != ')')
          return Status (Status::tokenExpected, ")");
        nextToken = lexer -> next ();
        switch (p -> type) {
        case Value::fsqrt: 
          if (r1 < 0)
            return Status (Status::illegalParameter, "negative");
          result = std::sqrt (r1); break;
        case Value::fsin: result = std::sin (r1); break;
        case Value::fcos: result = std::cos (r1); break;
        case Value::ftan: 
          if (std::cos (r1) == 0) 
            // TODO: should test against an appropriate epsilon
            return Status (Status::illegalParameter, "tan(0)");
          result = std::tan (r1); break;
        case Value::fdeg2rad: result = M_PI * r1 / 180; break;
        default: return Status (Status::cantHappen);
        }
      }
      break;
    }
    return status;
  }
  case '(': // ( <expression> )
    nextToken = lexer -> next ();
    double r;
    if (! (status = expression (r)).isOK ())
      return status;
    if (nextToken != ')')
      return Status (Status::tokenExpected, ")");
    nextToken = lexer -> next ();
    result = r;
    return status;
  default: // ???
    return Status (Status::illegalWord, lexer -> token ());
  }
}

/* signedExpr: [+-]?value
 */
Parser::Status Parser::signedExpr (double &result) {
  int tokOp = nextToken;
  if (nextToken == '-' || nextToken == '+') {
    double r1;
    Status status;
    nextToken = lexer -> next ();
    if (! (status = value (r1)).isOK ())
      return status;
    result = tokOp == '-' ? -r1 : r1;
    return status;
  } else
    return value (result);
}

/* factor: signedExpr (^ factor)?
 */
Parser::Status Parser::factor (double &result) {
  double r1, r2;
  Status status;
  if (! (status = signedExpr (r1)).isOK ())
    return status;
  int tokOp = nextToken;
  if (tokOp == '^') {
    nextToken = lexer -> next ();
    if (! (status = factor (r2)).isOK ())
      return status;
    else { // TODO: should test for r1 < 0 and fractionary r2
      result = std::pow (r1, r2);
      return status;
    }
  } else {
    result = r1;
    return status;
  }
}

/* term: factor ((/|*) factor)*
 */
Parser::Status Parser::term (double &result) {
  double r1, r2;
  Status status;
  if (! (status = factor (r1)).isOK ())
    return status;
  else {
    while (1)
      switch (nextToken) {
      case '*':
        nextToken = lexer -> next ();
        if (! (status = factor (r2)).isOK ())
          return status;
        else
          r1 *= r2;
        break;
      case '/':
        nextToken = lexer -> next ();
        if (! (status = factor (r2)).isOK ())
          return status;
        if (r2 == 0) // should test against epsilon
          return Status (Status::zeroDivide);
        result = r1 /= r2;
        break;
      default:
        result = r1;
        return status;
      }
    result = r1;
    return status;
  }
}

/* expression: term ([+-] term)*
 */
Parser::Status Parser::expression (double &result) {
  double r1, r2;
  Status status;
  if (! (status = term (r1)).isOK ())
    return status;
  while (true)
    switch (nextToken) {
    case '+':
      nextToken = lexer -> next ();
      if (! (status = term (r2)).isOK ())
        return status;
      else
        r1 += r2;
      break;
    case '-':
      nextToken = lexer -> next ();
      if (! (status = term (r2)).isOK ())
        return status;
      else
        r1 -= r2;
      break;
    default:
      result = r1;
      return status;
    }
  result = r1;
  return status;
}

/* internalEvaluate: parse and evaluate simple expression
   Parameters:
   str: text to parse
   result: result if no error found
   useDefault: use default if empty text
   defaultValue: default value if text is empty
   Returns:
   evaluation status
*/
Parser::Status Parser::internalEvaluate (std::string str, double &result,
                                         bool useDefault, 
                                         double defaultValue) {
  //  std::cerr << "Evaluate <" << str << ">\n";
  initIds ();
  lexer = new Lexer (str);
  if (! (nextToken = lexer -> next ())) { // already end of text
    if (useDefault) {
      result = defaultValue;
      return Status ();
    }
    else
      return Status (Status::emptyInput);
  }  
  double r;
  Status status = expression (r);
  if (nextToken && status.isOK ())
    status = Status (Status::extraInput);
  if (status.isOK ()) {
    result = r;
    // std::cerr << ">> " << result << "\n";
  }
  return status;
}

/* evaluate: user-callable expression evaluator
   Parameters:
   str: text to parse
   result: result if no error found
   useDefault: use default if empty text
   defaultValue: default value if text is empty
   Returns:
   evaluation status
*/
Parser::Status Parser::evaluate (const std::string &str, double &result,
                                 bool useDefault, 
                                 double defaultValue) {
  pendingVals.clear ();
  return internalEvaluate (str, result, useDefault, defaultValue);
}

/* initIds: make sure identifier table has built-ins
   There should be a static constructor in C++.
*/
void Parser::initIds() {
  static bool firstTime = true;
  if (firstTime) {
    // Initialize variable/function table
    ids ["w"] = new Value (1);
    ids ["h"] = new Value (1);
    ids ["d"] = new Value ("sqrt(w*w+h*h)");
    ids ["r2"] = new Value (std::sqrt (2.0));
    ids [std::string("phi")] = new Value (0.5 * (std::sqrt (5.0) - 1));
    ids [std::string("Phi")] = new Value (0.5 * (std::sqrt (5.0) + 1));
    ids [std::string("pi")] = new Value (M_PI);
    ids [std::string("e")] = new Value (M_E);
    ids [std::string ("sqrt")] = new Value (Value::fsqrt, "square root");
    ids [std::string ("sin")] = new Value (Value::fsin, "sine");
    ids [std::string ("cos")] = new Value (Value::fcos, "cosine");
    ids [std::string ("tan")] = new Value (Value::ftan, "tangent");
    ids [std::string ("deg2rad")] = new Value (Value::fdeg2rad, 
                                               "degrees to radians");
    ids [std::string ("set")] = new Value (Value::fset, "assignment");
    firstTime = false;
  }
}

/* setVariable: create or update variable from number
   Parameters:
   name: symbol name
   val: symbol value
   overwrite: if false, don't change existing variable
*/
void Parser::setVariable (const std::string &name, double val, bool overwrite) {
  initIds ();
  Value *p = ids [name];
  if (! p) 
    ids [name] = new Value (val);
  else
    if (overwrite) {
      p -> type = Value::number; // killing a function definition is allowed
      p -> num = val;
    }
}

/* setVariable: create or update variable from text expression
   Parameters:
   name: symbol name
   val: symbol value
   overwrite: if false, don't change existing variable
*/
void Parser::setVariable (const std::string &name,
                          const std::string &val, bool overwrite) {
  initIds ();
  Value *p = ids [name];
  if (! p) 
    ids [name] = new Value (val);
  else
    if (overwrite) {
      p -> type = Value::text; // killing a function definition is allowed
      p -> str = val;
    }
}

/* setVariable: create or update variable from existing value
   Parameters:
   name: symbol name
   val: symbol value
   overwrite: if false, don't change existing variable
*/
void Parser::setVariable (const std::string &name,
                          const Value &val, bool overwrite) {
  initIds ();
  Value *p = ids [name];
  if (! p) 
    ids [name] = new Value (val);
  else
    if (overwrite)
      *p = val;
}

/* Could return Value * and return 0 if not found, but I want to return
   a copy to be sure no one changes the returned value */
Parser::Value Parser::getVariable(const std::string &name) throw (Status) {
  initIds ();
  Value *p = ids [name];
  if (! p)
    throw Status (Status::unknownId, name);
  return *p;
}

std::ostream & operator<<(std::ostream &s, const Parser::Value &val) {
  switch (val.type) {
  case Parser::Value::number:
    s << val.num;
    break;
  case Parser::Value::text:
    s << val.str;
    break;
  default:
    s << "Function [" << val.str << "]";
  }
  return s;
}

/* convenience error output
   Parameters:
   s: output stream
   st: status info
*/
std::ostream &operator<<(std::ostream &s, 
                         const Parser::Status &st) {
  return s << st.toString ();
}

/* Convert status to string.
   Returns: status in user-readable format
*/
std::string Parser::Status::toString () const {
  switch (mErrType) {
  case Parser::Status::none:
    return "";
  case Parser::Status::opExpected: 
    return "operator +,-,*,/,^ expected";
  case Parser::Status::tokenExpected:
    return  "expected \"" + mMessage + "\"";
  case Parser::Status::unknownId: 
    return "unknown name: \"" + mMessage + "\"";
  case Parser::Status::notAFunction:
    return "identifier is not a function: \"" + mMessage + "\"";
  case Parser::Status::extraInput:
    return "extra input at end of text";
  case Parser::Status::emptyInput: 
    return "empty text";
  case Parser::Status::illegalWord:
    return "neither number, name nor operator: \"" + mMessage + "\"";
  case Parser::Status::zeroDivide:
    return "division by zero";
  case Parser::Status::illegalParameter:
    return "parameter of function out of range (" + mMessage + ")";
  case Parser::Status::varExpected: 
    return "variable name expected";
  case Parser::Status::stringExpected: 
    return "quote-delimited text expected";
  case Parser::Status::recursiveEval: 
    return "variable \"" + mMessage + "\" defined recursively";
  case Parser::Status::cantHappen: 
    return "internal error";
  default: return "illegal error code " + mErrType;
  }
}

/*****
Return the status code
*****/
Parser::Status::ErrType Parser::Status::GetErr() const
{
  return mErrType;
}

