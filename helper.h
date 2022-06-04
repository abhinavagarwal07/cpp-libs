#pragma once

#include <iostream>

template <class... T>
void print(T&&...) {
    std::cerr << __PRETTY_FUNCTION__ << "\n";
}

template <class F, class E>
bool check_throws(F&& f, E) {
    try {
        f();
    } catch (E e) {
        return true;
    }
    return false;
}