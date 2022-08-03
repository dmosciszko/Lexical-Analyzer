#include <iostream>
#include <fstream>
#include <map>
#include "lex.cpp"
using namespace std;
int main(int argc, char * argv[])  {
    int lineNumber = 0;
    LexItem tok;
    ifstream file("../resource/lib/public/ProgAssign/PA1/invid");
    istream* in;
    in = &file;

    bool v;
    bool iconsts;
    bool rconsts;
    bool sconsts;
    bool ids;

    if (!file.is_open()) {
        cout << "CANNOT OPEN THE FILE" << " " << argv[0];
    }

    if (argc == 1) {
        cout << "NO SPECIFIED INPUT FILE NAME FOUND";
        exit(0);
    }

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg[0] != '-' ) {
            cout << "ONLY ONE FILE NAME ALLOWED";
            exit(0);
        }
        if (arg == "-v") {
            v = true;
        }
        else if (arg == "-iconsts") {
            iconsts = true;
        }
        else if (arg == "-rconsts") {
            rconsts = true;
        }
        else if (arg == "-sconsts") {
            sconsts = true;
        }
        else if (arg == "-ids") {
            ids = true;
        }
        else {
            cout << "UNRECOGNIZED FLAG" << " " << arg;
        }
    }
    while((tok = getNextToken(*in, lineNumber)) != DONE && tok != ERR) {

        
       // if (v) {
       //     cout << tok;
       // }
    }
    if (tok == ERR) {
        cout << "Error in line " << lineNumber <<" (" << tok.GetLexeme() << ")";
        exit(0);
    }
}
