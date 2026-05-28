#include <string>
#include "lexer.hpp"

namespace lexer {

static std::string IdentifierStr;
static double NumVal;
static int last_char = ' ';

inline static void skip_whitespaces(){
    while(isspace(last_char))
        last_char = getchar();
}

inline static bool is_identifier_char(const char & c){ // identifier: [a-zA-Z_][a-zA-Z0-9_]*
    return isalnum(c) || c == '_';
}

inline static int identifier() { 
    IdentifierStr = last_char;
    while(is_identifier_char(last_char = getchar()))
        IdentifierStr += last_char;

    if(IdentifierStr == "def"){
        return tok_def;
    }
    if(IdentifierStr == "extern"){
        return tok_extern;
    }
    return tok_identifier;
}

inline static int number(){ // number: [0-9.]+
    std::string num_str;
    do{
        num_str += last_char;
        last_char = getchar();
    }while(isdigit(last_char) || last_char == '.');

    NumVal = strtod(num_str.c_str(),0);
    return tok_number;
}

inline static void skip_comment(){
    do last_char = getchar();
    while(last_char != EOF && last_char != '\n' && last_char != '\r');
}

inline static int eof(){
    return tok_eof;
}

int get_tok() {
    skip_whitespaces();
    
    if(last_char == '#'){
        skip_comment();
        if(last_char != EOF)      // keep skipping   
                return get_tok(); // new comments  
    } 

    if(isalpha(last_char) || last_char == '_' ) return identifier();

    if(isdigit(last_char) || last_char == '.') return number();

    if(last_char == EOF) return eof();
        
    // At this point, we have a special chararcter
    int this_char = last_char;
    last_char = getchar();
    return this_char;
}

}