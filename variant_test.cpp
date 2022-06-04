#include <cassert>
#include "variant.h"
#include "helper.h"

struct Visitor1 {
    static int visited;
    void operator()(int) const {
        visited = 1;
    }
    void operator()(float) const {
        visited = 2;
    }
};


struct Visitor2 {

};

int Visitor1::visited = 0;
int main() {
    using var_t = variant<int, float>;
    var_t v = 0xf1ea, v2;
    assert(get<0>(v) == 0xf1ea);
    assert(check_throws([&v](){ get<1>(v); }, bad_variant_access()));
    static_assert(std::is_same_v<variant_alternative_t<0, var_t>, int>);
    static_assert(std::is_same_v<variant_alternative_t<1, var_t>, float>);
    using std::swap;
    swap(v, v2);
    assert(get<0>(v2) == 0xf1ea);
    assert(check_throws([&v](){ get<0>(v); }, bad_variant_access()));
    v = v2;
    assert(get<0>(v) == 0xf1ea);
    static_assert(holds_alternative_v<var_t, float>);
    static_assert(!holds_alternative_v<var_t, char>);

    visit(Visitor1{}, v);
    assert(Visitor1::visited == 1);
    v = 1.02f;

    visit(Visitor1{}, v);
    assert(Visitor1::visited == 2);

//    visit(Visitor2{}, v); // does not compile
}