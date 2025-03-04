## Demangle

The demangle library allows you to get the name of a type in c++ as a string.
Support for clang, gcc and msvc. Other compilers return the mangled name from `typeid` as a fallback.

See main.cpp for an example.

Possible output (depending on compiler and standard library):

```text
Commontype<int16, int32> = int
int& = int&
int* = int*
int* const = int* const
const int& = int const&
const int* const = int const* const
std::string = std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >
```
