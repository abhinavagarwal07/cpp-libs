
template <class T>
class Optional {
public:
    Optional(const T& o): obj_(o){}
    constexpr T* operator->() noexcept {
        obj_.get();
    }

private:
    std::unique_ptr<T> obj_;
};