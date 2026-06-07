#pragma once
#include <iostream>
#include <memory>

namespace logger {

namespace detail {
    template <typename T>    
    concept Nullable = std::is_constructible_v<T,std::nullptr_t>;
}

template <detail::Nullable returntype> 
returntype error(const char* str){
    std::cerr << "[ERROR]: " << str << ".\n";
    return nullptr;
}

}