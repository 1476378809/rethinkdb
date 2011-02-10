#ifndef __CONTAINERS_UNIQUE_PTR_HPP__
#define __CONTAINERS_UNIQUE_PTR_HPP__

#include "debug.hpp"

/* We can't use C++0x's real unique pointer because we have to support GCC 4.1, so instead we have
this hack. It's like an auto_ptr, but with two differences:
 - Instead of NULLing itself when it loses ownership, it sets a bit that indicates it's no longer
   valid. That makes it impossible to accidentally use it after losing ownership.
 - It cheats on const-ness. It has a copy constructor that takes a (const unique_ptr_t &) but then
   modifies it.
*/

template<class T>
class unique_ptr_t {

    template<class T2> friend class unique_ptr_t;

    mutable T *ptr;
#ifndef NDEBUG
    mutable bool ok;
#endif

public:
    unique_ptr_t(T *p) throw () : ptr(p) {
        DEBUG_ONLY(ok = true);
    }

    unique_ptr_t() throw () : ptr(NULL) {
        DEBUG_ONLY(ok = true);
    }

    unique_ptr_t(const unique_ptr_t &other) throw () : ptr(NULL) {
        DEBUG_ONLY(ok = false);
        *this = other;
    }

    template<class T2>
    unique_ptr_t(const unique_ptr_t<T2> &other) throw () : ptr(NULL) {
        DEBUG_ONLY(ok = false);
        *this = other;
    }

    unique_ptr_t &operator=(const unique_ptr_t &other) throw () {
#ifndef NDEBUG
        if (other.ok) reset(other.release());
        else reset_invalid();
#else
        reset(other.release());
#endif
        return *this;
    }

    template<class T2>
    unique_ptr_t &operator=(const unique_ptr_t<T2> &other) throw () {
#ifndef NDEBUG
        if (other.ok) reset(other.release());
        else reset_invalid();
#else
        reset(other.release());
#endif
        return *this;
    }

    T *get() const throw () {
        rassert(ok);
        return ptr;
    }
    T *operator->() const throw () { return get(); }
    T &operator*() const throw () { return *get(); }
    operator bool() const throw () { return bool(get()); }

    void reset_invalid() {
        if (ptr) {
            rassert(ok);
            delete ptr;
        }
    }

    void reset(T *x = NULL) {
        reset_invalid();
        DEBUG_ONLY(ok = true);
        ptr = x;
    };

    T *release() const throw () {
        T *p = get();
        ptr = NULL;
        DEBUG_ONLY(ok = false);
        return p;
    }

    ~unique_ptr_t() {
        reset_invalid();
    }
};

template<class T, class U>
unique_ptr_t<T> static_pointer_cast(const unique_ptr_t<U> &ptr) {
    return static_cast<T*>(ptr.release());
}

#endif /* __CONTAINERS_UNIQUE_PTR_HPP__ */
