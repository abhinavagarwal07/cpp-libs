// TODO: small object opt for small objects that are nothrow_move_constructible
#include <iostream>
#include <ctime>
#include <cstdint>
#include <iostream>
#include <memory>
struct Obj {
    virtual std::unique_ptr<Obj> clone() const = 0;
    virtual const std::type_info& type() const noexcept= 0;
    virtual void* get() noexcept = 0;
    virtual ~Obj() {}
};

template <class T>
struct ObjC: public Obj {
    ObjC(const T& o): obj_(o) {}
    ObjC(T&& o): obj_(std::move(o)) {}
    std::unique_ptr<Obj> clone() const override {
        auto ptr = new ObjC<T>(obj_);
        return std::unique_ptr<Obj>(ptr);
    }
    const std::type_info& type() const noexcept override {
        return typeid(T);
    };
    void* get() noexcept override {
        return &obj_;
    }
    T& get_obj() {
        return obj_;
    }
private:
    T obj_;
};
class bad_cast{};
class bad_any_cast : public bad_cast{};
class Any {
public:
    Any() = default;
    template <class T>
    Any(T t) {
        obj_ = std::make_unique<ObjC<T>>(std::move(t));
    }
    Any(const Any& o) {
        obj_ = o.obj_->clone();
    }
    const std::type_info& type() const noexcept {
        return obj_->type();
    }
    friend void swap(Any& a, Any& b) noexcept {
        using std::swap;
        swap(a.obj_, b.obj_);
    }
    Any& operator=(Any o) noexcept {
        using std::swap;
        swap(o, *this);
        return *this;
    }
    void reset() noexcept {
        obj_.reset();
    }
    bool has_value() const noexcept {
        return (bool)obj_;
    }
    template <class T>
    friend T& any_cast(Any& o) {
        // type checking via dynamic cast
        auto ptr = dynamic_cast<ObjC<T>*>(o.obj_.get());
        if (!ptr)
            throw bad_any_cast();
        return ptr->get_obj();
        // ALTER
//        if (o.obj_->type() != typeid(T)) {
//            throw bad_any_cast();
//        }
//        return *static_cast<T*>(o.obj_->get());
    }
private:
    std::unique_ptr<Obj> obj_;
};
