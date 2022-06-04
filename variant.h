#pragma once

#include <cstddef>
#include <algorithm>

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

template <class... Ts>
class variant {
public:
    variant(): index_(-1) {}
    template <class V>
    variant(V v) {
        index_ = GetIndex_v<V, Ts...>;
        if (index_ != -1) {
            new (buf_) V(std::move(v));
        } else {
            // throw ?
        }
    }
    auto index() const noexcept {return index_;}
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
    T get() const {
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
auto get(const variant<Ts...>& var) {
    using T = typename variant_alternative<index, variant<Ts...>>::type;
    if (var.index() != index) {
        throw bad_variant_access();
    }
    return var.template get<T>();
}