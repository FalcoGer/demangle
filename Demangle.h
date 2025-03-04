#pragma once

#include <string>
#include <type_traits>
#include <typeinfo>

#if defined(__GNUG__) || defined(__clang__)

#  include <cstdlib>
#  include <cxxabi.h>
#  include <memory>

namespace Demangler
{

// NOLINTNEXTLINE (readability-identifier-naming) // name "Detail_" is okay.
namespace Detail_
{
    // this is required because top level const gets dropped from function parameters
    // this results in "int* const" to be displayed as "int*"
    // this wrapper stops that
    template <typename T>
    class Wrap
    {};

    class FreeDeleter
    {
      public:
        // NOLINTNEXTLINE (readability-identifier-naming) // name "ptr" is okay.
        void operator() (void* const ptr) const
        {
            if (ptr != nullptr)
            {
                // NOLINTNEXTLINE(cppcoreguidelines-no-malloc, cppcoreguidelines-owning-memory) // This is for RAII.
                std::free(ptr);
            }
        }
    };

}    // namespace Detail_

template <typename T>
auto demangle() -> std::string
{
    const auto* const                                 ptr_name = typeid(std::type_identity_t<Detail_::Wrap<T>>).name();

    int                                               status {0};

    // std::free is not indicated *addressable*. Only addressable functions in the std namespace can have their
    // addresses taken
    // const std::unique_ptr<char, decltype(&std::free)> DEMANGLED
    //   {abi::__cxa_demangle(ptr_name, nullptr, nullptr, &status), &std::free};

    const std::unique_ptr<char, Detail_::FreeDeleter> DEMANGLED {
      abi::__cxa_demangle(ptr_name, nullptr, nullptr, &status)
    };
    if (status == 0)
    {
        auto                  ret = std::string {DEMANGLED.get()};
        constexpr std::size_t LEN_OF_WRAP_T_NAME_START {std::string("Demangler::Detail_::Wrap<").length()};
        constexpr std::size_t LEN_OF_WRAP_T_CLOSING_ANGLE_BRKT {1};
        return {std::next(ret.begin(), LEN_OF_WRAP_T_NAME_START), std::prev(ret.end(), LEN_OF_WRAP_T_CLOSING_ANGLE_BRKT)};
    }
    return std::string(typeid(T).name());
}

template <typename T>
auto demangle(const T& value) -> std::string
{
    const auto* const ptr_name = typeid(value).name();    // dropping const and ref here is OK

    int               status {0};
    const std::unique_ptr<char, Detail_::FreeDeleter> DEMANGLED {
      abi::__cxa_demangle(ptr_name, nullptr, nullptr, &status)
    };
    if (status == 0)
    {
        return std::string {DEMANGLED.get()};
    }
    return std::string(typeid(value).name());
}

#elif defined(_MSC_VER)

#  include <dbghelp.h>    // requires /link Dbghelp.lib
#  include <Windows.h>

namespace Demangler
{

namespace Detail_
{
    template <typename T>
    class Wrap
    {};
}
template <typename T>
auto demangle() -> std::string
{
    const auto* const ptr_name = typeid(std::type_identity_t<Detail_::Wrap<T>>).name();
    std::string       result {ptr_name};
    // Allocate buffer to hold the demangled name
    char              buffer[1'024];
    if (UnDecorateSymbolName(ptr_name, buffer, sizeof(buffer), UNDNAME_COMPLETE))
    {
        result = std::string(buffer);
        constexpr std::size_t LEN_OF_WRAP_T_NAME_START {std::string("class Demangler::Detail_::Wrap<").length()};
        constexpr std::size_t LEN_OF_WRAP_T_CLOSING_ANGLE_BRKT {1};
        result = std::string {
          std::next(result.begin(), LEN_OF_WRAP_T_NAME_START), std::prev(result.end(), LEN_OF_WRAP_T_CLOSING_ANGLE_BRKT)
        };
    }
    return result;
}

template <typename T>
auto demangle(const T& value) -> std::string
{
    const auto* const ptr_name = typeid(value).name();

    // Allocate buffer to hold the demangled name
    char              buffer[1'024];
    if (UnDecorateSymbolName(ptr_name, buffer, sizeof(buffer), UNDNAME_COMPLETE))
    {
        return std::string(buffer);
    }

    return std::string(ptr_name);
}

#else

namespace Demangler
{

template <typename T>
auto demangle() -> std::string
{
    const auto* const ptr_name = typeid(std::type_identity_t<T>).name();
    return std::string(ptr_name);
}

template <typename T>
auto demangle(const T& value) -> std::string
{
    const auto* const ptr_name = typeid(value).name();
    return std::string(ptr_name);
}

#endif

}    // namespace Demangler
