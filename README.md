Locked - Wrap a data object and an associated mutex, and only allow access to                         
         the contained data object when the mutex is locked.

Usage

Declare a global object using the Locked template:

class MyData { ... };

Locked<MyData> gData;

This creates an object that contains a MyData object and (by default)
a std::mutex.

To access the data, create a Locked<MyData>::pointer or const_pointer:

void func(...)
{
    Locked<MyData>::pointer data(gData);

    data->method(args, ...);
}

The Locked<MyData>::pointer constructor will lock the mutex, and its
destructor will unlock it.  Creating a Locked<MyData>::pointer or
const_pointer is the only way to access the MyData object that is
contained inside the Locked<MyData> object.

A different type of mutex can be specified as a second template
parameter.  In this case, it is often helpful to create a typedef
to keep the name shorter.  For example, to use a C++17 shared_mutex:

typedef Locked<MyData, std::shared_mutex> LockedData;
LockedData gData;

void func(...)
{
    LockedData::const_pointer data(gData);

    data->method(...);
}

When using a std::shared_mutex (or any mutex type that has a lock_shared()
method), the Locked<...>::const_pointer object will call lock_shared()
in its constructor and unlock_shared() in its destructor, obtaining a
read lock.
