#pragma once

namespace lexer{
    enum Token {
        tok_eof=-1,
        tok_def=-2,
        tok_extern=-3,
        tok_identifier=-4,
        tok_number=-5,
        // any single character valid tokens are themselves,
        // i.e. the token for '+' has its number identifier 
        // in the enum equal to its ascii value
    };
    
    int get_tok();
}