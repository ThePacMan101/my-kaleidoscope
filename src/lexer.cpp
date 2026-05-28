// #include <string>

// enum Token {
//     tok_eof=-1,
//     tok_def=-2,
//     tok_extern=-3,
//     tok_identifier=-4,
//     tok_number=-5,
// };

// static std::string IdentifierStr;
// static double NumVal;

// static int get_tok() {
//     static int last_char = ' ';
//     while(isspace(last_char))
//         last_char = getchar();

    
//     if(isalpha(last_char)){
//         IdentifierStr = last_char;
//         while(isalnum(last_char = getchar()))
//             IdentifierStr += last_char;

//         if(IdentifierStr == "def"){
//             return tok_def;
//         }
//         if(IdentifierStr == "extern"){
//             return tok_extern;
//         }
//         return tok_identifier;
//     }

//     if(isdigit(last_char) || last_char == '.'){
//         std::string num_str;
//         do{
//             num_str += last_char;
//             last_char = getchar();
//         }while(isdigit(last_char) || last_char == '.');

//         NumVal = strtod(num_str.c_str(), 0);
//         return tok_number;
//     }

//     if(last_char == '#'){
//         do last_char = getchar();
//         while(last_char != EOF && last_char != '\n' && last_char != '\r');

//         if(last_char != EOF)
//             return get_tok();
//     }

//     if(last_char == EOF)
//         return tok_eof;

//     int this_char = last_char;
//     last_char = getchar();
//     return this_char;
// }