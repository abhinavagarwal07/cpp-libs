#pragma once

#include <cstddef>
#include <algorithm>
#include <type_traits>
#include "helper.h"

struct bad_variant_access: public std::exception {};

template <size_t a, size_t...as>
constexpr size_t mx() {
    if constexpr (sizeof ...(as) == 0)
        return a;
    else
        return std::max(a, mx<as...>());
}


template <int index, class V, class T1, class... Ts>
struct GetIndexH {
    static constexpr auto f() {
        if constexpr (std::is_same_v<V, T1>)
            return index;
        else if constexpr (sizeof...(Ts) == 0)
            return -1;
        else
            return GetIndexH<index + 1, V, Ts...>::value;
    }
    static constexpr int value = f();
};


template <class V, class... Ts>
int GetIndex_v = GetIndexH<0, V, Ts...>::value;

template <class T>
struct is_variant: std::false_type {};

template <class... Ts>
class variant;
template <class... Ts>
struct is_variant<variant<Ts...>>: std::true_type {};
template <class T>
constexpr bool is_variant_v = is_variant<T>::value;

template <class... Ts>
class variant {
public:
    static constexpr size_t arity = sizeof...(Ts);
    constexpr variant(): index_(-1) {

    }
    variant(const variant& v): index_(v.index_) {
        std::memcpy(buf_, v.buf_, sizeof(buf_));
    }

    // unable to get it working with univ ref somehow
    template <class V>
    constexpr variant(V v) {
        using Vr = V;
        index_ = GetIndex_v<Vr, Ts...>;
        if (index_ != -1) {
            new (buf_) Vr(std::move(v));
        } else {
            // throw ?
        }
    }
    constexpr auto index() const noexcept {return index_;}
    friend void swap(variant<Ts...>& a, variant<Ts...>& b) {
        using std::swap;
        swap(a.index_, b.index_);
        swap(a.buf_, b.buf_);
    }
    variant& operator=(variant o) {
        swap(*this, o);
        return *this;
    }
    template <class T>
    T& get() const {
        return *reinterpret_cast<const T*>(buf_);
    }
private:
    constexpr static size_t MAX_SZ = mx<sizeof(Ts)...>();
    char buf_[MAX_SZ];
    int index_{-1};
};

template <int index, class V>
struct variant_alternative;
template <int index, class V1, class... Vs>
struct variant_alternative<index, variant<V1, Vs...>> {
    static constexpr auto f() noexcept {
        if constexpr (index == 0)
            return std::declval<V1>();
        else return std::declval<typename variant_alternative<index - 1,
                variant<Vs...>>::type>();
    }
    using type = decltype(f());
};

template <int index, class V>
using variant_alternative_t = typename variant_alternative<index, V>::type;

template <size_t index, class... Ts>
decltype(auto) get(const variant<Ts...>& var) {
    using T = typename variant_alternative<index, variant<Ts...>>::type;
    if (var.index() != index) {
        throw bad_variant_access();
    }
    return var.template get<T>();
}

template <class T, class U>
struct holds_variant_alternative;

template <class T, class... Ts, class U>
struct holds_variant_alternative<variant<T, Ts...>, U> {
    static constexpr bool value = std::is_same_v<U, T>
                                  || holds_variant_alternative<variant<Ts...>, U>::value;
};

template <class U>
struct holds_variant_alternative<variant<>, U> : std::false_type {};

template <class T, class U>
constexpr bool holds_alternative_v = holds_variant_alternative<T, U>::value;

template <class T, class... Ts>
constexpr decltype(auto) strip_variant_head_type(variant<T, Ts...>& v) {
    return reinterpret_cast<variant<Ts...>&>(v);
}

template <class Visitor, class Var, class I>
auto visit_h(Visitor&& V, Var& var, I i)
{
    static constexpr size_t var_arity = Var::arity;
    constexpr int idx = i();
    if constexpr (idx < var_arity) {
        if (var.index() == idx) {
            V(get<idx>(var));
        } else {
            visit_h(std::forward<Visitor>(V),
                    var,
                    std::integral_constant<int, idx + 1>());
        }
    }
}
template <class Visitor, class Var>
auto visit(Visitor&& V, Var& var) {
    visit_h(V, var, std::integral_constant<int, 0>());
}