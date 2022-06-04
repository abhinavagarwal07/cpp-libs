#include <memory>
struct bad_optional_access: public std::exception {};
template <class T>
class Optional {
public:
    Optional(){}
    Optional(const T& o): obj_(o){}
    constexpr T* operator->() noexcept {
        obj_.get();
    }
    bool has_value() const noexcept {
        return (bool)obj_;
    }
    T& value() {
        if (!has_value())
            throw bad_optional_access();
        return *obj_;
    }
    template <class U>
    T& value_or(U&& def) {
        if (!has_value())
            return def;
        return *obj_;
    }
private:
    std::unique_ptr<T> obj_;
};