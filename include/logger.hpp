#pragma once
#include <iostream>
#include <memory>

namespace logger {

template <typename T>
std::unique_ptr<T> error(const char* str){
    std::cerr << "[ERROR]: " << str << ".\n";
    return nullptr;
}

}