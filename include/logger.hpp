#pragma once
#include <iostream>
#include <memory>

namespace logger {

namespace detail {
    template <typename T>    
    concept Nullable = std::is_constructible_v<T,std::nullptr_t>;
}

template <detail::Nullable returntype> 
returntype error(std::string_view str){
    std::cerr << "[ERROR]: " << str << ".\n";
    return nullptr;
}

}