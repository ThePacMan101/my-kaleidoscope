#include <string>
#include "lexer.hpp"

namespace lexer {

static std::string identifier_str;
static double num_val;
static int last_char = ' ';

inline static void skip_whitespaces(){
    while(isspace(last_char))
        last_char = getchar();
}

inline static bool is_identifier_char(const char & c){ // identifier: [a-zA-Z_][a-zA-Z0-9_]*
    return isalnum(c) || c == '_';
}

inline static int identifier() { 
    identifier_str = last_char;
    while(is_identifier_char(last_char = getchar()))
        identifier_str += last_char;

    if(identifier_str == "def"){
        return tok_def;
    }
    if(identifier_str == "extern"){
        return tok_extern;
    }
    if(identifier_str == "if"){
        return tok_if;
    }
    if(identifier_str == "then"){
        return tok_then;
    }
    if(identifier_str == "else"){
        return tok_else;
    }
    return tok_identifier;
}

inline static int number(){ // number: [0-9.]+
    std::string num_str;
    //e.g. parsing 123.456

    // parses 123
    while(isdigit(last_char)){
        num_str += last_char;
        last_char = getchar();
    }
    // parses .
    if(last_char == '.'){
        num_str += last_char;
        last_char = getchar();
    }
    // parses 456
    while(isdigit(last_char)){
        num_str += last_char;
        last_char = getchar();
    }
    num_val = strtod(num_str.c_str(),0);
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
        
    // At this point returns the character as its ascii value
    int this_char = last_char;
    last_char = getchar();
    return this_char;
}

double get_num_val(){
    return num_val;
}

std::string get_identifier_str(){
    return identifier_str;
}


}