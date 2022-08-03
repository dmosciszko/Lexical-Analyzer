#include "lex.h"
#include <fstream>
#include <regex>
using namespace std;

LexItem id_or_kw(const string& lexeme, int linenum) {
    map<string,Token>::iterator it = kwmap.find(lexeme);
        //std::cout << it -> first << ": " << it -> second << endl;
        
        if (it != kwmap.end())  {
            return (LexItem(it -> second, lexeme, linenum));
        }
        return (LexItem(IDENT, lexeme, linenum));
    }
ostream& operator<<(ostream& out, const LexItem& tok)   {
    Token token = tok.GetToken();
    int num = tok.GetLinenum();
    out << tokenPrint[token];
    if (token == IDENT || token == ICONST || token == RCONST || token == SCONST || token == ERR)    {
        out <<" (" << tok.GetLexeme()<< ")";
    }
    out << " Line #: " << tok.GetLinenum() << endl;
    return out;
}

LexItem prevTok;
LexItem currTok;

LexItem getNextToken(istream& in, int& linenum) {
    enum TokState { START, INID, INSTRING, ININT, INREAL, INCOMMENT};
    int lexstate = START;
    string lexeme = "";
    char ch;
    prevTok = LexItem(DONE, lexeme, linenum);
    while (in.get(ch))  {
        cout << ch;
        switch(lexstate)    {
            case START:
                if (ch == '\n') {
                    linenum++;
                }
                if (in.peek() == EOF) {
                    if (prevTok.GetToken() != END) {
                        return LexItem(ERR, prevTok.GetLexeme(), prevTok.GetLinenum());                       
                    }
                    return LexItem(DONE, lexeme, linenum);
                }
                if (isspace(ch)) {
                    continue;
                }
                lexeme += ch;

                if (ch == '!')  {
                    lexstate = INCOMMENT;
                    continue;
                }

                if (isalpha(ch)) {
                    lexstate = INID;
                    continue;
                }

                if (isdigit(ch)) {
                    lexstate = ININT;
                    continue;
                }

                if (ch == '.') {
                    lexstate = INREAL;
                    continue;
                }

                if ((ch == '\"') || (ch == '\'')) {
                    lexstate = INSTRING;
                    continue;
                }

                if (ch == '+') {
                    lexstate = PLUS;
                    continue;
                }

                if (ch == '-') {
                    lexstate = MINUS;
                    continue;
                }

                if (ch == '*') {
                    lexstate = MULT;
                    continue;
                }

                if (ch == '(') {
                    lexstate = LPAREN;
                    continue;
                }

                if (ch == ')') {
                    lexstate = RPAREN;
                    continue;
                }

                if (ch == '<') {
                    lexstate = LTHAN;
                    continue;
                }

                if (ch == ',') {
                    lexstate = COMA;
                    continue;
                }

                if (ch == ':') {
                    lexstate = COLON;
                    continue;
                }

                if (ch == '=') {
                    if (in.get() == '=') {
                        lexstate = EQUAL;
                    }
                    else {
                        lexstate = ASSOP;
                        in.putback(ch);
                    }
                    continue;
                }
                if (ch == '/') {
                    if (in.get() == '/') {
                        lexstate = CONCAT;
                    }
                    else {
                        lexstate = DIV;
                        in.putback(ch);
                    }
                    continue;
                }
                



                //error case (cannot be resolved to a state)
                return(LexItem(ERR, lexeme, linenum));

            case ININT:
                if (prevTok.GetToken() == ERR) {
                    return LexItem(ERR, prevTok.GetLexeme(), prevTok.GetLinenum());
                }

               else if (regex_match(lexeme + ch, regex("[0-9]+"))) {
                    lexeme += ch;
                }
               else if (ch == '.')  {
                    lexstate = INREAL;
                    in.putback(ch);
                    continue;
                }
                else {
                    lexstate = START;
                    in.putback(ch);
                    currTok = LexItem(ICONST, lexeme, linenum);
                    prevTok = currTok;
                    return currTok;
                }
                break;

            case INSTRING:
                if (prevTok == ERR) {
                    return LexItem(ERR, prevTok.GetLexeme(), prevTok.GetLinenum());
                }
                // IF UNCLOSED STRING:
                if (regex_match(lexeme + ch, regex("[\"\'][ -~]*"))) {
                    //if you find new line in string, error
                    if ((ch == '\\') && (in.peek() == 'n')) {
                        return LexItem(ERR, prevTok.GetLexeme(), prevTok.GetLinenum());
                    }
                }
                // IF CLOSED STRING:
                if (regex_match(lexeme + ch, regex("[\"\'][ -~]*[\"\']"))) {
                    lexstate = START;
                    currTok = LexItem(SCONST, lexeme, linenum);
                    prevTok = currTok;
                    return currTok;
                }
                break;

            case INID:
                if (!isalnum(ch)) {
                    lexstate = START;
                    in.putback(ch);
                    return LexItem(ERR, lexeme, linenum);
                }
                if (isalnum(ch)) {
                    lexeme += ch;
                }
                if (lexeme == "program") {
                    if (prevTok.GetToken() != ERR) {
                        return LexItem(ERR, lexeme, linenum);
                    }
                    currTok = LexItem(PROGRAM, lexeme, linenum);
                }
                else if (lexeme == "print") {
                    currTok = LexItem(PRINT, lexeme, linenum);
                }
                else if (lexeme == "if") {
                    currTok = LexItem(IF, lexeme, linenum);
                }
                else if (lexeme == "then") {
                    currTok = LexItem(THEN, lexeme, linenum);
                }
                else if (lexeme == "read") {
                    currTok = LexItem(READ, lexeme, linenum);
                }
                else if (lexeme == "real") {
                    currTok = LexItem(REAL, lexeme, linenum);
                }
                else if (lexeme == "char") {
                    currTok = LexItem(CHAR, lexeme, linenum);
                }
                else {
                    currTok = LexItem(IDENT, lexeme, linenum);
                }
                prevTok = currTok;
                break;
            case INREAL:
                if (isdigit(ch)) {
                    lexeme += ch;
                }
                else {
                    if (lexeme[lexeme.length()-1] == '.') {
                        return LexItem(ERR, prevTok.GetLexeme(), prevTok.GetLinenum());
                    }
                    lexstate = START;
                    in.putback(ch);
                    currTok = LexItem(RCONST, lexeme, linenum);
                    prevTok = currTok;
                    return currTok;
                }
                break;

            case INCOMMENT:
                if (ch == '\n') {
                    linenum++;
                    lexstate = START;
                }
                continue;
        }
    }
}
