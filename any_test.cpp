#include "any.h"
#include <cassert>
int main() {
    Any b;
    {
        Any a = 1;
        assert(a.has_value());
        assert(!b.has_value());
        assert(any_cast<int>(a) == 1);
        b = a;
    }
    assert(b.has_value() && any_cast<int>(b) == 1);
    bool throws_re = false;
    try {
        any_cast<float>(b);
    } catch (bad_any_cast& e) {
        throws_re = true;
    }
    assert(throws_re);
}