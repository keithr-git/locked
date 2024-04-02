//
// Locked - Wrap a data object and an associated mutex, and only allow access to
//          the contained data object when the mutex is locked.
//
// Copyright 2012,2024 Keith Reynolds.
//
// This program is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software
// Foundation, either version 2.1 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE.  See the GNU Lesser General Public License for
// more details.
//
// You should have received a copy of the GNU Lesser General
// Public License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//
// Usage
//
// Declare a global object using the Locked template:
//
// class MyData { ... };
//
// Locked<MyData> gData;
//
// This creates an object that contains a MyData object and (by default)
// a std::mutex.
//
// To access the data, create a Locked<MyData>::pointer or const_pointer:
//
// void func(...)
// {
//     Locked<MyData>::pointer data(gData);
//
//     data->method(args, ...);
// }
//
// The Locked<MyData>::pointer constructor will lock the mutex, and its
// destructor will unlock it.  Creating a Locked<MyData>::pointer or
// const_pointer is the only way to access the MyData object that is
// contained inside the Locked<MyData> object.
//
// A different type of mutex can be specified as a second template
// parameter.  In this case, it is often helpful to create a typedef
// to keep the name shorter.  For example, to use a C++17 shared_mutex:
//
// typedef Locked<MyData, std::shared_mutex> LockedData;
// LockedData gData;
//
// void func(...)
// {
//     LockedData::const_pointer data(gData);
//
//     data->method(...);
// }
//
// When using a std::shared_mutex (or any mutex type that has a lock_shared()
// method), the Locked<...>::const_pointer object will call lock_shared()
// in its constructor and unlock_shared() in its destructor, obtaining a
// read lock.
//
#ifndef LOCKED_HPP
#define LOCKED_HPP

#include <mutex>
#include <type_traits>

template <typename T, typename MUTEX = std::mutex>
class Locked
{
    MUTEX       mutex_;
    T           data_;

    struct check_lock_shared
    {
        template <typename U, U>
        class check
        { };

        template <typename U>
        static char f(check<void (U::*)(), &U::lock_shared>*);
        template <typename U>
        static long f(...);
    };

    typedef typename std::remove_reference<MUTEX>::type mutex_type;

    enum {
        has_lock_shared =
            (sizeof(check_lock_shared::template f<mutex_type>(0)) == sizeof(char)),
    };

    //
    // Base class for pointer and const_pointer.
    // U is either T for pointer or const T for const_pointer.
    //
    template <typename U>
    class pointer_base
    {
    protected:
        Locked&         locked_;

        inline pointer_base(Locked& Locked)
            : locked_(Locked)
        { }

    public:
        inline U* operator->() const
        {
            return &locked_.data_;
        }

        inline U& operator*() const
        {
            return locked_.data_;
        }

        inline operator U*() const
        {
            return &locked_.data_;
        }

    private:
        pointer_base(const pointer_base&) = delete;
        pointer_base& operator=(const pointer_base&) = delete;
    }; // class pointer_base

public:
    template <typename... Args>
    inline Locked(Args&&... args)
        : data_(std::forward<Args>(args)...)
    { }

    template <typename... Args>
    inline Locked(MUTEX& mutex, Args&&... args)
        : mutex_(mutex), data_(std::forward<Args>(args)...)
    { }

    class pointer
        : public pointer_base<T>
    {
    public:
        inline pointer(Locked& Locked)
            : pointer_base<T>(Locked)
        {
            this->locked_.mutex_.lock();
        }

        inline ~pointer()
        {
            this->locked_.mutex_.unlock();
        }
    }; // class pointer

    //
    // Use lock_shared() and unlock_shared() if the mutex class has them,
    // otherwise use lock() and unlock().
    //
    class const_pointer
        : public pointer_base<const T>
    {
    public:
        template <typename L = Locked,
                    typename std::enable_if<L::has_lock_shared>::type* = nullptr>
        inline const_pointer(Locked& Locked)
            : pointer_base<const T>(Locked)
        {
            this->locked_.mutex_.lock_shared();
        }

        template <typename L = Locked,
                    typename std::enable_if<! L::has_lock_shared>::type* = nullptr>
        inline const_pointer(Locked& Locked)
            : pointer_base<const T>(Locked)
        {
            this->locked_.mutex_.lock();
        }

        template <typename L = Locked,
                    typename std::enable_if<L::has_lock_shared>::type* = nullptr>
        inline void unlock()
        {
            this->locked_.mutex_.unlock_shared();
        }

        template <typename L = Locked,
                    typename std::enable_if<! L::has_lock_shared>::type* = nullptr>
        inline void unlock()
        {
            this->locked_.mutex_.unlock();
        }

        inline ~const_pointer()
        {
            unlock();
        }
    }; // class const_pointer
}; // class Locked

#endif // LOCKED_HPP
