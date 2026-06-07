#pragma once
#include <iostream>
#include <memory>

namespace logger {

/* Must have nullable return type */
template <typename returntype> 
returntype error(const char* str){
    std::cerr << "[ERROR]: " << str << ".\n";
    return nullptr;
}

}