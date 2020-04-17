// doctest.h - the lightest feature-rich C++ single-header testing framework for unit tests and TDD
//
// Copyright (c) 2016-2019 Viktor Kirilov
// and a few changes by Eyal Rozenberg
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
// The documentation can be found at the library's page:
// https://github.com/onqtam/doctest/blob/master/doc/markdown/readme.md
//
// =================================================================================================
// =================================================================================================
// =================================================================================================
//
// The library is heavily influenced by Catch - https://github.com/catchorg/Catch2
// which uses the Boost Software License - Version 1.0
// see here - https://github.com/catchorg/Catch2/blob/master/LICENSE.txt
//
// The concept of subcases (sections in Catch) and expression decomposition are from there.
// Some parts of the code are taken directly:
// - stringification - the detection of "ostream& operator<<(ostream&, const T&)" and StringMaker<>
// - the Approx() helper class for floating point comparison
// - colors in the console
// - breaking into a debugger
// - signal / SEH handling
// - timer
// - XmlWriter class - thanks to Phil Nash for allowing the direct reuse (AKA copy/paste)
//
// The expression decomposing templates are taken from lest - https://github.com/martinmoene/lest
// which uses the Boost Software License - Version 1.0
// see here - https://github.com/martinmoene/lest/blob/master/LICENSE.txt
//
// =================================================================================================
// =================================================================================================
// =================================================================================================

#ifndef DOCTEST_LIBRARY_INCLUDED
#define DOCTEST_LIBRARY_INCLUDED

// =================================================================================================
// == VERSION ======================================================================================
// =================================================================================================

#define DOCTEST_VERSION_MAJOR 2
#define DOCTEST_VERSION_MINOR 3
#define DOCTEST_VERSION_PATCH 7
#define DOCTEST_VERSION_STR "2.3.7"

#define DOCTEST_VERSION                                                                            \
    (DOCTEST_VERSION_MAJOR * 10000 + DOCTEST_VERSION_MINOR * 100 + DOCTEST_VERSION_PATCH)

// =================================================================================================
// == COMPILER VERSION =============================================================================
// =================================================================================================

// ideas for the version stuff are taken from here: https://github.com/cxxstuff/cxx_detect

#define DOCTEST_COMPILER(MAJOR, MINOR, PATCH) ((MAJOR)*10000000 + (MINOR)*100000 + (PATCH))

// GCC/Clang and GCC/MSVC are mutually exclusive, but Clang/MSVC are not because of clang-cl...
#if defined(_MSC_VER) && defined(_MSC_FULL_VER)
#if _MSC_VER == _MSC_FULL_VER / 10000
#define DOCTEST_MSVC DOCTEST_COMPILER(_MSC_VER / 100, _MSC_VER % 100, _MSC_FULL_VER % 10000)
#else // MSVC
#define DOCTEST_MSVC                                                                               \
    DOCTEST_COMPILER(_MSC_VER / 100, (_MSC_FULL_VER / 100000) % 100, _MSC_FULL_VER % 100000)
#endif // MSVC
#endif // MSVC
#if defined(__clang__) && defined(__clang_minor__)
#define DOCTEST_CLANG DOCTEST_COMPILER(__clang_major__, __clang_minor__, __clang_patchlevel__)
#elif defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__) &&              \
        !defined(__INTEL_COMPILER)
#define DOCTEST_GCC DOCTEST_COMPILER(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#endif // GCC

#ifndef DOCTEST_MSVC
#define DOCTEST_MSVC 0
#endif // DOCTEST_MSVC
#ifndef DOCTEST_CLANG
#define DOCTEST_CLANG 0
#endif // DOCTEST_CLANG
#ifndef DOCTEST_GCC
#define DOCTEST_GCC 0
#endif // DOCTEST_GCC

// =================================================================================================
// == COMPILER WARNINGS HELPERS ====================================================================
// =================================================================================================

#if DOCTEST_CLANG
#define DOCTEST_PRAGMA_TO_STR(x) _Pragma(#x)
#define DOCTEST_CLANG_SUPPRESS_WARNING_PUSH _Pragma("clang diagnostic push")
#define DOCTEST_CLANG_SUPPRESS_WARNING(w) DOCTEST_PRAGMA_TO_STR(clang diagnostic ignored w)
#define DOCTEST_CLANG_SUPPRESS_WARNING_POP _Pragma("clang diagnostic pop")
#define DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)                                                \
    DOCTEST_CLANG_SUPPRESS_WARNING_PUSH DOCTEST_CLANG_SUPPRESS_WARNING(w)
#else // DOCTEST_CLANG
#define DOCTEST_CLANG_SUPPRESS_WARNING_PUSH
#define DOCTEST_CLANG_SUPPRESS_WARNING(w)
#define DOCTEST_CLANG_SUPPRESS_WARNING_POP
#define DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // DOCTEST_CLANG

#if DOCTEST_GCC
#define DOCTEST_PRAGMA_TO_STR(x) _Pragma(#x)
#define DOCTEST_GCC_SUPPRESS_WARNING_PUSH _Pragma("GCC diagnostic push")
#define DOCTEST_GCC_SUPPRESS_WARNING(w) DOCTEST_PRAGMA_TO_STR(GCC diagnostic ignored w)
#define DOCTEST_GCC_SUPPRESS_WARNING_POP _Pragma("GCC diagnostic pop")
#define DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH(w)                                                  \
    DOCTEST_GCC_SUPPRESS_WARNING_PUSH DOCTEST_GCC_SUPPRESS_WARNING(w)
#else // DOCTEST_GCC
#define DOCTEST_GCC_SUPPRESS_WARNING_PUSH
#define DOCTEST_GCC_SUPPRESS_WARNING(w)
#define DOCTEST_GCC_SUPPRESS_WARNING_POP
#define DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // DOCTEST_GCC

#if DOCTEST_MSVC
#define DOCTEST_MSVC_SUPPRESS_WARNING_PUSH __pragma(warning(push))
#define DOCTEST_MSVC_SUPPRESS_WARNING(w) __pragma(warning(disable : w))
#define DOCTEST_MSVC_SUPPRESS_WARNING_POP __pragma(warning(pop))
#define DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)                                                 \
    DOCTEST_MSVC_SUPPRESS_WARNING_PUSH DOCTEST_MSVC_SUPPRESS_WARNING(w)
#else // DOCTEST_MSVC
#define DOCTEST_MSVC_SUPPRESS_WARNING_PUSH
#define DOCTEST_MSVC_SUPPRESS_WARNING(w)
#define DOCTEST_MSVC_SUPPRESS_WARNING_POP
#define DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // DOCTEST_MSVC

// =================================================================================================
// == COMPILER WARNINGS ============================================================================
// =================================================================================================

DOCTEST_CLANG_SUPPRESS_WARNING_PUSH
DOCTEST_CLANG_SUPPRESS_WARNING("-Wunknown-pragmas")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wnon-virtual-dtor")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wweak-vtables")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wpadded")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wdeprecated")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wmissing-prototypes")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wunused-local-typedef")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wc++98-compat")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wc++98-compat-pedantic")

DOCTEST_GCC_SUPPRESS_WARNING_PUSH
DOCTEST_GCC_SUPPRESS_WARNING("-Wunknown-pragmas")
DOCTEST_GCC_SUPPRESS_WARNING("-Wpragmas")
DOCTEST_GCC_SUPPRESS_WARNING("-Weffc++")
DOCTEST_GCC_SUPPRESS_WARNING("-Wstrict-overflow")
DOCTEST_GCC_SUPPRESS_WARNING("-Wstrict-aliasing")
DOCTEST_GCC_SUPPRESS_WARNING("-Wctor-dtor-privacy")
DOCTEST_GCC_SUPPRESS_WARNING("-Wmissing-declarations")
DOCTEST_GCC_SUPPRESS_WARNING("-Wnon-virtual-dtor")
DOCTEST_GCC_SUPPRESS_WARNING("-Wunused-local-typedefs")
DOCTEST_GCC_SUPPRESS_WARNING("-Wuseless-cast")
DOCTEST_GCC_SUPPRESS_WARNING("-Wnoexcept")
DOCTEST_GCC_SUPPRESS_WARNING("-Wsign-promo")

DOCTEST_MSVC_SUPPRESS_WARNING_PUSH
DOCTEST_MSVC_SUPPRESS_WARNING(4616) // invalid compiler warning
DOCTEST_MSVC_SUPPRESS_WARNING(4619) // invalid compiler warning
DOCTEST_MSVC_SUPPRESS_WARNING(4996) // The compiler encountered a deprecated declaration
DOCTEST_MSVC_SUPPRESS_WARNING(4706) // assignment within conditional expression
DOCTEST_MSVC_SUPPRESS_WARNING(4512) // 'class' : assignment operator could not be generated
DOCTEST_MSVC_SUPPRESS_WARNING(4127) // conditional expression is constant
DOCTEST_MSVC_SUPPRESS_WARNING(4820) // padding
DOCTEST_MSVC_SUPPRESS_WARNING(4625) // copy constructor was implicitly defined as deleted
DOCTEST_MSVC_SUPPRESS_WARNING(4626) // assignment operator was implicitly defined as deleted
DOCTEST_MSVC_SUPPRESS_WARNING(5027) // move assignment operator was implicitly defined as deleted
DOCTEST_MSVC_SUPPRESS_WARNING(5026) // move constructor was implicitly defined as deleted
DOCTEST_MSVC_SUPPRESS_WARNING(4623) // default constructor was implicitly defined as deleted
DOCTEST_MSVC_SUPPRESS_WARNING(4640) // construction of local static object is not thread-safe
// static analysis
DOCTEST_MSVC_SUPPRESS_WARNING(26439) // This kind of function may not throw. Declare it 'noexcept'
DOCTEST_MSVC_SUPPRESS_WARNING(26495) // Always initialize a member variable
DOCTEST_MSVC_SUPPRESS_WARNING(26451) // Arithmetic overflow ...
DOCTEST_MSVC_SUPPRESS_WARNING(26444) // Avoid unnamed objects with custom construction and dtr...
DOCTEST_MSVC_SUPPRESS_WARNING(26812) // Prefer 'enum class' over 'enum'

// 4548 - expression before comma has no effect; expected expression with side - effect
// 4265 - class has virtual functions, but destructor is not virtual
// 4986 - exception specification does not match previous declaration
// 4350 - behavior change: 'member1' called instead of 'member2'
// 4668 - 'x' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
// 4365 - conversion from 'int' to 'unsigned long', signed/unsigned mismatch
// 4774 - format string expected in argument 'x' is not a string literal
// 4820 - padding in structs

// only 4 should be disabled globally:
// - 4514 # unreferenced inline function has been removed
// - 4571 # SEH related
// - 4710 # function not inlined
// - 4711 # function 'x' selected for automatic inline expansion

#define DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN                                 \
    DOCTEST_MSVC_SUPPRESS_WARNING_PUSH                                                             \
    DOCTEST_MSVC_SUPPRESS_WARNING(4548)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4265)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4986)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4350)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4668)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4365)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4774)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4820)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4625)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4626)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(5027)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(5026)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4623)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(5039)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(5045)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(5105)

#define DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END DOCTEST_MSVC_SUPPRESS_WARNING_POP

// =================================================================================================
// == FEATURE DETECTION ============================================================================
// =================================================================================================

// general compiler feature support table: https://en.cppreference.com/w/cpp/compiler_support
// MSVC C++11 feature support table: https://msdn.microsoft.com/en-us/library/hh567368.aspx
// GCC C++11 feature support table: https://gcc.gnu.org/projects/cxx-status.html
// MSVC version table:
// https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B#Internal_version_numbering
// MSVC++ 14.2 (16) _MSC_VER == 1920 (Visual Studio 2019)
// MSVC++ 14.1 (15) _MSC_VER == 1910 (Visual Studio 2017)
// MSVC++ 14.0      _MSC_VER == 1900 (Visual Studio 2015)
// MSVC++ 12.0      _MSC_VER == 1800 (Visual Studio 2013)
// MSVC++ 11.0      _MSC_VER == 1700 (Visual Studio 2012)
// MSVC++ 10.0      _MSC_VER == 1600 (Visual Studio 2010)
// MSVC++ 9.0       _MSC_VER == 1500 (Visual Studio 2008)
// MSVC++ 8.0       _MSC_VER == 1400 (Visual Studio 2005)

#if DOCTEST_MSVC && !defined(DOCTEST_CONFIG_WINDOWS_SEH)
#define DOCTEST_CONFIG_WINDOWS_SEH
#endif // MSVC
#if defined(DOCTEST_CONFIG_NO_WINDOWS_SEH) && defined(DOCTEST_CONFIG_WINDOWS_SEH)
#undef DOCTEST_CONFIG_WINDOWS_SEH
#endif // DOCTEST_CONFIG_NO_WINDOWS_SEH

#if !defined(_WIN32) && !defined(__QNX__) && !defined(DOCTEST_CONFIG_POSIX_SIGNALS) &&             \
        !defined(__EMSCRIPTEN__)
#define DOCTEST_CONFIG_POSIX_SIGNALS
#endif // _WIN32
#if defined(DOCTEST_CONFIG_NO_POSIX_SIGNALS) && defined(DOCTEST_CONFIG_POSIX_SIGNALS)
#undef DOCTEST_CONFIG_POSIX_SIGNALS
#endif // DOCTEST_CONFIG_NO_POSIX_SIGNALS

#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
#if !defined(__cpp_exceptions) && !defined(__EXCEPTIONS) && !defined(_CPPUNWIND)
#define DOCTEST_CONFIG_NO_EXCEPTIONS
#endif // no exceptions
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS

#ifdef DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
#define DOCTEST_CONFIG_NO_EXCEPTIONS
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS

#if defined(DOCTEST_CONFIG_NO_EXCEPTIONS) && !defined(DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS)
#define DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS && !DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS

#if defined(DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN) && !defined(DOCTEST_CONFIG_IMPLEMENT)
#define DOCTEST_CONFIG_IMPLEMENT
#endif // DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#if defined(_WIN32) || defined(__CYGWIN__)
#if DOCTEST_MSVC
#define DOCTEST_SYMBOL_EXPORT __declspec(dllexport)
#define DOCTEST_SYMBOL_IMPORT __declspec(dllimport)
#else // MSVC
#define DOCTEST_SYMBOL_EXPORT __attribute__((dllexport))
#define DOCTEST_SYMBOL_IMPORT __attribute__((dllimport))
#endif // MSVC
#else  // _WIN32
#define DOCTEST_SYMBOL_EXPORT __attribute__((visibility("default")))
#define DOCTEST_SYMBOL_IMPORT
#endif // _WIN32

#ifdef DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#ifdef DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_INTERFACE DOCTEST_SYMBOL_EXPORT
#else // DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_INTERFACE DOCTEST_SYMBOL_IMPORT
#endif // DOCTEST_CONFIG_IMPLEMENT
#else  // DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#define DOCTEST_INTERFACE
#endif // DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL

#define DOCTEST_EMPTY

#if DOCTEST_MSVC
#define DOCTEST_NOINLINE __declspec(noinline)
#define DOCTEST_UNUSED
#define DOCTEST_ALIGNMENT(x)
#else // MSVC
#define DOCTEST_NOINLINE __attribute__((noinline))
#define DOCTEST_UNUSED __attribute__((unused))
#define DOCTEST_ALIGNMENT(x) __attribute__((aligned(x)))
#endif // MSVC

// =================================================================================================
// == FEATURE DETECTION END ========================================================================
// =================================================================================================

// internal macros for string concatenation and anonymous variable name generation
#define DOCTEST_CAT_IMPL(s1, s2) s1##s2
#define DOCTEST_CAT(s1, s2) DOCTEST_CAT_IMPL(s1, s2)
#ifdef __COUNTER__ // not standard and may be missing for some compilers
#define DOCTEST_ANONYMOUS(x) DOCTEST_CAT(x, __COUNTER__)
#else // __COUNTER__
#define DOCTEST_ANONYMOUS(x) DOCTEST_CAT(x, __LINE__)
#endif // __COUNTER__

#define DOCTEST_TOSTR(x) #x

#ifndef DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE
#define DOCTEST_REF_WRAP(x) x&
#else // DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE
#define DOCTEST_REF_WRAP(x) x
#endif // DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE

// not using __APPLE__ because... this is how Catch does it
#ifdef __MAC_OS_X_VERSION_MIN_REQUIRED
#define DOCTEST_PLATFORM_MAC
#elif defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#define DOCTEST_PLATFORM_IPHONE
#elif defined(_WIN32)
#define DOCTEST_PLATFORM_WINDOWS
#else // DOCTEST_PLATFORM
#define DOCTEST_PLATFORM_LINUX
#endif // DOCTEST_PLATFORM

#define DOCTEST_GLOBAL_NO_WARNINGS(var)                                                            \
    DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wglobal-constructors")                              \
    DOCTEST_CLANG_SUPPRESS_WARNING("-Wunused-variable")                                            \
    static int var DOCTEST_UNUSED // NOLINT(fuchsia-statically-constructed-objects,cert-err58-cpp)
#define DOCTEST_GLOBAL_NO_WARNINGS_END() DOCTEST_CLANG_SUPPRESS_WARNING_POP

#ifndef DOCTEST_BREAK_INTO_DEBUGGER
// should probably take a look at https://github.com/scottt/debugbreak
#ifdef DOCTEST_PLATFORM_MAC
#define DOCTEST_BREAK_INTO_DEBUGGER() __asm__("int $3\n" : :)
#elif DOCTEST_MSVC
#define DOCTEST_BREAK_INTO_DEBUGGER() __debugbreak()
#elif defined(__MINGW32__)
DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wredundant-decls")
extern "C" __declspec(dllimport) void __stdcall DebugBreak();
DOCTEST_GCC_SUPPRESS_WARNING_POP
#define DOCTEST_BREAK_INTO_DEBUGGER() ::DebugBreak()
#else // linux
#define DOCTEST_BREAK_INTO_DEBUGGER() ((void)0)
#endif // linux
#endif // DOCTEST_BREAK_INTO_DEBUGGER

// this is kept here for backwards compatibility since the config option was changed
#ifdef DOCTEST_CONFIG_USE_IOSFWD
#define DOCTEST_CONFIG_USE_STD_HEADERS
#endif // DOCTEST_CONFIG_USE_IOSFWD

#ifdef DOCTEST_CONFIG_USE_STD_HEADERS
#include <iosfwd>
#include <cstddef>
#include <ostream>
#else // DOCTEST_CONFIG_USE_STD_HEADERS

#if DOCTEST_CLANG
// to detect if libc++ is being used with clang (the _LIBCPP_VERSION identifier)
#include <ciso646>
#endif // clang

#ifdef _LIBCPP_VERSION
#define DOCTEST_STD_NAMESPACE_BEGIN _LIBCPP_BEGIN_NAMESPACE_STD
#define DOCTEST_STD_NAMESPACE_END _LIBCPP_END_NAMESPACE_STD
#else // _LIBCPP_VERSION
#define DOCTEST_STD_NAMESPACE_BEGIN namespace std {
#define DOCTEST_STD_NAMESPACE_END }
#endif // _LIBCPP_VERSION

// Forward declaring 'X' in namespace std is not permitted by the C++ Standard.
DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4643)

DOCTEST_STD_NAMESPACE_BEGIN // NOLINT (cert-dcl58-cpp)
typedef decltype(nullptr) nullptr_t;
template <class charT>
struct char_traits;
template <>
struct char_traits<char>;
template <class charT, class traits>
class basic_ostream;
typedef basic_ostream<char, char_traits<char>> ostream;
template <class... Types>
class tuple;
#if DOCTEST_MSVC >= DOCTEST_COMPILER(19, 20, 0)
// see this issue on why this is needed: https://github.com/onqtam/doctest/issues/183
template <class _Ty>
class allocator;
template <class _Elem, class _Traits, class _Alloc>
class basic_string;
using string = basic_string<char, char_traits<char>, allocator<char>>;
#endif // VS 2019
DOCTEST_STD_NAMESPACE_END

DOCTEST_MSVC_SUPPRESS_WARNING_POP

#endif // DOCTEST_CONFIG_USE_STD_HEADERS

#ifdef DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS
#include <type_traits>
#endif // DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS

namespace doctest {

DOCTEST_INTERFACE extern bool is_running_in_test;

// A 24 byte string class (can be as small as 17 for x64 and 13 for x86) that can hold strings with length
// of up to 23 chars on the stack before going on the heap - the last byte of the buffer is used for:
// - "is small" bit - the highest bit - if "0" then it is small - otherwise its "1" (128)
// - if small - capacity left before going on the heap - using the lowest 5 bits
// - if small - 2 bits are left unused - the second and third highest ones
// - if small - acts as a null terminator if strlen() is 23 (24 including the null terminator)
//              and the "is small" bit remains "0" ("as well as the capacity left") so its OK
// Idea taken from this lecture about the string implementation of facebook/folly - fbstring
// https://www.youtube.com/watch?v=kPR8h4-qZdk
// TODO:
// - optimizations - like not deleting memory unnecessarily in operator= and etc.
// - resize/reserve/clear
// - substr
// - replace
// - back/front
// - iterator stuff
// - find & friends
// - push_back/pop_back
// - assign/insert/erase
// - relational operators as free functions - taking const char* as one of the params
class DOCTEST_INTERFACE String
{
    static const unsigned len  = 24;      //!OCLINT avoid private static members
    static const unsigned last = len - 1; //!OCLINT avoid private static members

    struct view // len should be more than sizeof(view) - because of the final byte for flags
    {
        char*    ptr;
        unsigned size;
        unsigned capacity;
    };

    union
    {
        char buf[len];
        view data;
    };

    bool isOnStack() const { return (buf[last] & 128) == 0; }
    void setOnHeap();
    void setLast(unsigned in = last);

    void copy(const String& other);

public:
    String();
    ~String();

    // cppcheck-suppress noExplicitConstructor
    String(const char* in);
    String(const char* in, unsigned in_size);

    String(const String& other);
    String& operator=(const String& other);

    String& operator+=(const String& other);
    String  operator+(const String& other) const;

    String(String&& other);
    String& operator=(String&& other);

    char  operator[](unsigned i) const;
    char& operator[](unsigned i);

    // the only functions I'm willing to leave in the interface - available for inlining
    const char* c_str() const { return const_cast<String*>(this)->c_str(); } // NOLINT
    char*       c_str() {
        if(isOnStack())
            return reinterpret_cast<char*>(buf);
        return data.ptr;
    }

    unsigned size() const;
    unsigned capacity() const;

    int compare(const char* other, bool no_case = false) const;
    int compare(const String& other, bool no_case = false) const;
};

DOCTEST_INTERFACE bool operator==(const String& lhs, const String& rhs);
DOCTEST_INTERFACE bool operator!=(const String& lhs, const String& rhs);
DOCTEST_INTERFACE bool operator<(const String& lhs, const String& rhs);
DOCTEST_INTERFACE bool operator>(const String& lhs, const String& rhs);
DOCTEST_INTERFACE bool operator<=(const String& lhs, const String& rhs);
DOCTEST_INTERFACE bool operator>=(const String& lhs, const String& rhs);

DOCTEST_INTERFACE std::ostream& operator<<(std::ostream& s, const String& in);

namespace Color {
    enum Enum
    {
        None = 0,
        White,
        Red,
        Green,
        Blue,
        Cyan,
        Yellow,
        Grey,

        Bright = 0x10,

        BrightRed   = Bright | Red,
        BrightGreen = Bright | Green,
        LightGrey   = Bright | Grey,
        BrightWhite = Bright | White
    };

    DOCTEST_INTERFACE std::ostream& operator<<(std::ostream& s, Color::Enum code);
} // namespace Color

namespace assertType {
    enum Enum
    {
        // macro traits

        is_warn    = 1,
        is_check   = 2 * is_warn,
        is_require = 2 * is_check,

        is_normal      = 2 * is_require,
        is_throws      = 2 * is_normal,
        is_throws_as   = 2 * is_throws,
        is_throws_with = 2 * is_throws_as,
        is_nothrow     = 2 * is_throws_with,

        is_false = 2 * is_nothrow,
        is_unary = 2 * is_false, // not checked anywhere - used just to distinguish the types

        is_eq = 2 * is_unary,
        is_ne = 2 * is_eq,

        is_lt = 2 * is_ne,
        is_gt = 2 * is_lt,

        is_ge = 2 * is_gt,
        is_le = 2 * is_ge,

        // macro types

        DT_WARN    = is_normal | is_warn,
        DT_CHECK   = is_normal | is_check,
        DT_REQUIRE = is_normal | is_require,

        DT_WARN_FALSE    = is_normal | is_false | is_warn,
        DT_CHECK_FALSE   = is_normal | is_false | is_check,
        DT_REQUIRE_FALSE = is_normal | is_false | is_require,

        DT_WARN_THROWS    = is_throws | is_warn,
        DT_CHECK_THROWS   = is_throws | is_check,
        DT_REQUIRE_THROWS = is_throws | is_require,

        DT_WARN_THROWS_AS    = is_throws_as | is_warn,
        DT_CHECK_THROWS_AS   = is_throws_as | is_check,
        DT_REQUIRE_THROWS_AS = is_throws_as | is_require,

        DT_WARN_THROWS_WITH    = is_throws_with | is_warn,
        DT_CHECK_THROWS_WITH   = is_throws_with | is_check,
        DT_REQUIRE_THROWS_WITH = is_throws_with | is_require,
        
        DT_WARN_THROWS_WITH_AS    = is_throws_with | is_throws_as | is_warn,
        DT_CHECK_THROWS_WITH_AS   = is_throws_with | is_throws_as | is_check,
        DT_REQUIRE_THROWS_WITH_AS = is_throws_with | is_throws_as | is_require,

        DT_WARN_NOTHROW    = is_nothrow | is_warn,
        DT_CHECK_NOTHROW   = is_nothrow | is_check,
        DT_REQUIRE_NOTHROW = is_nothrow | is_require,

        DT_WARN_EQ    = is_normal | is_eq | is_warn,
        DT_CHECK_EQ   = is_normal | is_eq | is_check,
        DT_REQUIRE_EQ = is_normal | is_eq | is_require,

        DT_WARN_NE    = is_normal | is_ne | is_warn,
        DT_CHECK_NE   = is_normal | is_ne | is_check,
        DT_REQUIRE_NE = is_normal | is_ne | is_require,

        DT_WARN_GT    = is_normal | is_gt | is_warn,
        DT_CHECK_GT   = is_normal | is_gt | is_check,
        DT_REQUIRE_GT = is_normal | is_gt | is_require,

        DT_WARN_LT    = is_normal | is_lt | is_warn,
        DT_CHECK_LT   = is_normal | is_lt | is_check,
        DT_REQUIRE_LT = is_normal | is_lt | is_require,

        DT_WARN_GE    = is_normal | is_ge | is_warn,
        DT_CHECK_GE   = is_normal | is_ge | is_check,
        DT_REQUIRE_GE = is_normal | is_ge | is_require,

        DT_WARN_LE    = is_normal | is_le | is_warn,
        DT_CHECK_LE   = is_normal | is_le | is_check,
        DT_REQUIRE_LE = is_normal | is_le | is_require,

        DT_WARN_UNARY    = is_normal | is_unary | is_warn,
        DT_CHECK_UNARY   = is_normal | is_unary | is_check,
        DT_REQUIRE_UNARY = is_normal | is_unary | is_require,

        DT_WARN_UNARY_FALSE    = is_normal | is_false | is_unary | is_warn,
        DT_CHECK_UNARY_FALSE   = is_normal | is_false | is_unary | is_check,
        DT_REQUIRE_UNARY_FALSE = is_normal | is_false | is_unary | is_require,
    };
} // namespace assertType

DOCTEST_INTERFACE const char* assertString(assertType::Enum at);
DOCTEST_INTERFACE const char* failureString(assertType::Enum at);
DOCTEST_INTERFACE const char* skipPathFromFilename(const char* file);

struct DOCTEST_INTERFACE TestCaseData
{
    const char* m_file;       // the file in which the test was registered
    unsigned    m_line;       // the line where the test was registered
    const char* m_name;       // name of the test case
    const char* m_test_suite; // the test suite in which the test was added
    const char* m_description;
    bool        m_skip;
    bool        m_may_fail;
    bool        m_should_fail;
    int         m_expected_failures;
    double      m_timeout;
};

struct DOCTEST_INTERFACE AssertData
{
    // common - for all asserts
    const TestCaseData* m_test_case;
    assertType::Enum    m_at;
    const char*         m_file;
    int                 m_line;
    const char*         m_expr;
    bool                m_failed;

    // exception-related - for all asserts
    bool   m_threw;
    String m_exception;

    // for normal asserts
    String m_decomp;

    // for specific exception-related asserts
    bool        m_threw_as;
    const char* m_exception_type;
    const char* m_exception_string;
};

struct DOCTEST_INTERFACE MessageData
{
    String           m_string;
    const char*      m_file;
    int              m_line;
    assertType::Enum m_severity;
};

struct DOCTEST_INTERFACE SubcaseSignature
{
    String      m_name;
    const char* m_file;
    int         m_line;

    bool operator<(const SubcaseSignature& other) const;
};

struct DOCTEST_INTERFACE IContextScope
{
    IContextScope();
    virtual ~IContextScope();
    virtual void stringify(std::ostream*) const = 0;
};

struct ContextOptions //!OCLINT too many fields
{
    std::ostream* cout;        // stdout stream - std::cout by default
    std::ostream* cerr;        // stderr stream - std::cerr by default
    String        binary_name; // the test binary name

    // == parameters from the command line
    String   out;       // output filename
    String   order_by;  // how tests should be ordered
    unsigned rand_seed; // the seed for rand ordering

    unsigned first; // the first (matching) test to be executed
    unsigned last;  // the last (matching) test to be executed

    int abort_after;           // stop tests after this many failed assertions
    int subcase_filter_levels; // apply the subcase filters for the first N levels

    bool success;              // include successful assertions in output
    bool case_sensitive;       // if filtering should be case sensitive
    bool exit;                 // if the program should be exited after the tests are ran/whatever
    bool duration;             // print the time duration of each test case
    bool no_throw;             // to skip exceptions-related assertion macros
    bool no_exitcode;          // if the framework should return 0 as the exitcode
    bool no_run;               // to not run the tests at all (can be done with an "*" exclude)
    bool no_version;           // to not print the version of the framework
    bool no_intro;             // to not print the version of the framework
    bool no_colors;            // if output to the console should be colorized
    bool force_colors;         // forces the use of colors even when a tty cannot be detected
    bool no_breaks;            // to not break into the debugger
    bool no_skip;              // don't skip test cases which are marked to be skipped
    bool gnu_file_line;        // if line numbers should be surrounded with :x: and not (x):
    bool no_path_in_filenames; // if the path to files should be removed from the output
    bool no_line_numbers;      // if source code line numbers should be omitted from the output
    bool no_skipped_summary;   // don't print "skipped" in the summary !!! UNDOCUMENTED !!!

    bool help;             // to print the help
    bool version;          // to print the version
    bool count;            // if only the count of matching tests is to be retrieved
    bool list_test_cases;  // to list all tests matching the filters
    bool list_test_suites; // to list all suites matching the filters
    bool list_reporters;   // lists all registered reporters
};

namespace detail {
#if defined(DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING) || defined(DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS)
    template <bool CONDITION, typename TYPE = void>
    struct enable_if
    {};

    template <typename TYPE>
    struct enable_if<true, TYPE>
    { typedef TYPE type; };
#endif // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING) || DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS

    // clang-format off
    template<class T> struct remove_reference      { typedef T type; };
    template<class T> struct remove_reference<T&>  { typedef T type; };
    template<class T> struct remove_reference<T&&> { typedef T type; };

    template<class T> struct remove_const          { typedef T type; };
    template<class T> struct remove_const<const T> { typedef T type; };
    // clang-format on

    template <typename T>
    struct deferred_false
    // cppcheck-suppress unusedStructMember
    { static const bool value = false; };

    namespace has_insertion_operator_impl {
        typedef char no;
        typedef char yes[2];

        struct any_t
        {
            template <typename T>
            // cppcheck-suppress noExplicitConstructor
            any_t(const DOCTEST_REF_WRAP(T));
        };

        yes& testStreamable(std::ostream&);
        no   testStreamable(no);

        no operator<<(const std::ostream&, const any_t&);

        template <typename T>
        struct has_insertion_operator
        {
            static std::ostream& s;
            static const DOCTEST_REF_WRAP(T) t;
            static const bool value = sizeof(decltype(testStreamable(s << t))) == sizeof(yes);
        };
    } // namespace has_insertion_operator_impl

    template <typename T>
    struct has_insertion_operator : has_insertion_operator_impl::has_insertion_operator<T>
    {};

    DOCTEST_INTERFACE void my_memcpy(void* dest, const void* src, unsigned num);

    DOCTEST_INTERFACE std::ostream* getTlsOss(); // returns a thread-local ostringstream
    DOCTEST_INTERFACE String getTlsOssResult();

    template <bool C>
    struct StringMakerBase
    {
        template <typename T>
        static String convert(const DOCTEST_REF_WRAP(T)) {
            return "{?}";
        }
    };

    template <>
    struct StringMakerBase<true>
    {
        template <typename T>
        static String convert(const DOCTEST_REF_WRAP(T) in) {
            *getTlsOss() << in;
            return getTlsOssResult();
        }
    };

    DOCTEST_INTERFACE String rawMemoryToString(const void* object, unsigned size);

    template <typename T>
    String rawMemoryToString(const DOCTEST_REF_WRAP(T) object) {
        return rawMemoryToString(&object, sizeof(object));
    }

    template <typename T>
    const char* type_to_string() {
        return "<>";
    }
} // namespace detail

template <typename T>
struct StringMaker : public detail::StringMakerBase<detail::has_insertion_operator<T>::value>
{};

template <typename T>
struct StringMaker<T*>
{
    template <typename U>
    static String convert(U* p) {
        if(p)
            return detail::rawMemoryToString(p);
        return "NULL";
    }
};

template <typename R, typename C>
struct StringMaker<R C::*>
{
    static String convert(R C::*p) {
        if(p)
            return detail::rawMemoryToString(p);
        return "NULL";
    }
};

template <typename T>
String toString(const DOCTEST_REF_WRAP(T) value) {
    return StringMaker<T>::convert(value);
}

#ifdef DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
DOCTEST_INTERFACE String toString(char* in);
DOCTEST_INTERFACE String toString(const char* in);
#endif // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
DOCTEST_INTERFACE String toString(bool in);
DOCTEST_INTERFACE String toString(float in);
DOCTEST_INTERFACE String toString(double in);
DOCTEST_INTERFACE String toString(double long in);

DOCTEST_INTERFACE String toString(char in);
DOCTEST_INTERFACE String toString(char signed in);
DOCTEST_INTERFACE String toString(char unsigned in);
DOCTEST_INTERFACE String toString(int short in);
DOCTEST_INTERFACE String toString(int short unsigned in);
DOCTEST_INTERFACE String toString(int in);
DOCTEST_INTERFACE String toString(int unsigned in);
DOCTEST_INTERFACE String toString(int long in);
DOCTEST_INTERFACE String toString(int long unsigned in);
DOCTEST_INTERFACE String toString(int long long in);
DOCTEST_INTERFACE String toString(int long long unsigned in);
DOCTEST_INTERFACE String toString(std::nullptr_t in);

#if DOCTEST_MSVC >= DOCTEST_COMPILER(19, 20, 0)
// see this issue on why this is needed: https://github.com/onqtam/doctest/issues/183
DOCTEST_INTERFACE String toString(const std::string& in);
#endif // VS 2019

class DOCTEST_INTERFACE Approx
{
public:
    explicit Approx(double value);

    Approx operator()(double value) const;

#ifdef DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS
    template <typename T>
    explicit Approx(const T& value,
                    typename detail::enable_if<std::is_constructible<double, T>::value>::type* =
                            static_cast<T*>(nullptr)) {
        *this = Approx(static_cast<double>(value));
    }
#endif // DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS

    Approx& epsilon(double newEpsilon);

#ifdef DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS
    template <typename T>
    typename detail::enable_if<std::is_constructible<double, T>::value, Approx&>::type epsilon(
            const T& newEpsilon) {
        m_epsilon = static_cast<double>(newEpsilon);
        return *this;
    }
#endif //  DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS

    Approx& scale(double newScale);

#ifdef DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS
    template <typename T>
    typename detail::enable_if<std::is_constructible<double, T>::value, Approx&>::type scale(
            const T& newScale) {
        m_scale = static_cast<double>(newScale);
        return *this;
    }
#endif // DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS

    // clang-format off
    DOCTEST_INTERFACE friend bool operator==(double lhs, const Approx & rhs);
    DOCTEST_INTERFACE friend bool operator==(const Approx & lhs, double rhs);
    DOCTEST_INTERFACE friend bool operator!=(double lhs, const Approx & rhs);
    DOCTEST_INTERFACE friend bool operator!=(const Approx & lhs, double rhs);
    DOCTEST_INTERFACE friend bool operator<=(double lhs, const Approx & rhs);
    DOCTEST_INTERFACE friend bool operator<=(const Approx & lhs, double rhs);
    DOCTEST_INTERFACE friend bool operator>=(double lhs, const Approx & rhs);
    DOCTEST_INTERFACE friend bool operator>=(const Approx & lhs, double rhs);
    DOCTEST_INTERFACE friend bool operator< (double lhs, const Approx & rhs);
    DOCTEST_INTERFACE friend bool operator< (const Approx & lhs, double rhs);
    DOCTEST_INTERFACE friend bool operator> (double lhs, const Approx & rhs);
    DOCTEST_INTERFACE friend bool operator> (const Approx & lhs, double rhs);

    DOCTEST_INTERFACE friend String toString(const Approx& in);

#ifdef DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS
#define DOCTEST_APPROX_PREFIX \
    template <typename T> friend typename detail::enable_if<std::is_constructible<double, T>::value, bool>::type

    DOCTEST_APPROX_PREFIX operator==(const T& lhs, const Approx& rhs) { return operator==(double(lhs), rhs); }
    DOCTEST_APPROX_PREFIX operator==(const Approx& lhs, const T& rhs) { return operator==(rhs, lhs); }
    DOCTEST_APPROX_PREFIX operator!=(const T& lhs, const Approx& rhs) { return !operator==(lhs, rhs); }
    DOCTEST_APPROX_PREFIX operator!=(const Approx& lhs, const T& rhs) { return !operator==(rhs, lhs); }
    DOCTEST_APPROX_PREFIX operator<=(const T& lhs, const Approx& rhs) { return double(lhs) < rhs.m_value || lhs == rhs; }
    DOCTEST_APPROX_PREFIX operator<=(const Approx& lhs, const T& rhs) { return lhs.m_value < double(rhs) || lhs == rhs; }
    DOCTEST_APPROX_PREFIX operator>=(const T& lhs, const Approx& rhs) { return double(lhs) > rhs.m_value || lhs == rhs; }
    DOCTEST_APPROX_PREFIX operator>=(const Approx& lhs, const T& rhs) { return lhs.m_value > double(rhs) || lhs == rhs; }
    DOCTEST_APPROX_PREFIX operator< (const T& lhs, const Approx& rhs) { return double(lhs) < rhs.m_value && lhs != rhs; }
    DOCTEST_APPROX_PREFIX operator< (const Approx& lhs, const T& rhs) { return lhs.m_value < double(rhs) && lhs != rhs; }
    DOCTEST_APPROX_PREFIX operator> (const T& lhs, const Approx& rhs) { return double(lhs) > rhs.m_value && lhs != rhs; }
    DOCTEST_APPROX_PREFIX operator> (const Approx& lhs, const T& rhs) { return lhs.m_value > double(rhs) && lhs != rhs; }
#undef DOCTEST_APPROX_PREFIX
#endif // DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS

    // clang-format on

private:
    double m_epsilon;
    double m_scale;
    double m_value;
};

DOCTEST_INTERFACE String toString(const Approx& in);

DOCTEST_INTERFACE const ContextOptions* getContextOptions();

#if !defined(DOCTEST_CONFIG_DISABLE)

namespace detail {
    // clang-format off
#ifdef DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
    template<class T>               struct decay_array       { typedef T type; };
    template<class T, unsigned N>   struct decay_array<T[N]> { typedef T* type; };
    template<class T>               struct decay_array<T[]>  { typedef T* type; };

    template<class T>   struct not_char_pointer              { enum { value = 1 }; };
    template<>          struct not_char_pointer<char*>       { enum { value = 0 }; };
    template<>          struct not_char_pointer<const char*> { enum { value = 0 }; };

    template<class T> struct can_use_op : public not_char_pointer<typename decay_array<T>::type> {};
#endif // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
    // clang-format on

    struct DOCTEST_INTERFACE TestFailureException
    {
    };

    DOCTEST_INTERFACE bool checkIfShouldThrow(assertType::Enum at);

#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
    [[noreturn]]
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS
    DOCTEST_INTERFACE void throwException();

    struct DOCTEST_INTERFACE Subcase
    {
        SubcaseSignature m_signature;
        bool             m_entered = false;

        Subcase(const String& name, const char* file, int line);
        ~Subcase();

        operator bool() const;
    };

    template <typename L, typename R>
    String stringifyBinaryExpr(const DOCTEST_REF_WRAP(L) lhs, const char* op,
                               const DOCTEST_REF_WRAP(R) rhs) {
        return toString(lhs) + op + toString(rhs);
    }

#define DOCTEST_DO_BINARY_EXPRESSION_COMPARISON(op, op_str, op_macro)                              \
    template <typename R>                                                                          \
    DOCTEST_NOINLINE Result operator op(const DOCTEST_REF_WRAP(R) rhs) {                           \
        bool res = op_macro(lhs, rhs);                                                             \
        if(m_at & assertType::is_false)                                                            \
            res = !res;                                                                            \
        if(!res || doctest::getContextOptions()->success)                                          \
            return Result(res, stringifyBinaryExpr(lhs, op_str, rhs));                             \
        return Result(res);                                                                        \
    }

    // more checks could be added - like in Catch:
    // https://github.com/catchorg/Catch2/pull/1480/files
    // https://github.com/catchorg/Catch2/pull/1481/files
#define DOCTEST_FORBIT_EXPRESSION(rt, op)                                                          \
    template <typename R>                                                                          \
    rt& operator op(const R&) {                                                                    \
        static_assert(deferred_false<R>::value,                                                    \
                      "Expression Too Complex Please Rewrite As Binary Comparison!");              \
        return *this;                                                                              \
    }

    struct DOCTEST_INTERFACE Result
    {
        bool   m_passed;
        String m_decomp;

        Result(bool passed, const String& decomposition = String());

        // forbidding some expressions based on this table: https://en.cppreference.com/w/cpp/language/operator_precedence
        DOCTEST_FORBIT_EXPRESSION(Result, &)
        DOCTEST_FORBIT_EXPRESSION(Result, ^)
        DOCTEST_FORBIT_EXPRESSION(Result, |)
        DOCTEST_FORBIT_EXPRESSION(Result, &&)
        DOCTEST_FORBIT_EXPRESSION(Result, ||)
        DOCTEST_FORBIT_EXPRESSION(Result, ==)
        DOCTEST_FORBIT_EXPRESSION(Result, !=)
        DOCTEST_FORBIT_EXPRESSION(Result, <)
        DOCTEST_FORBIT_EXPRESSION(Result, >)
        DOCTEST_FORBIT_EXPRESSION(Result, <=)
        DOCTEST_FORBIT_EXPRESSION(Result, >=)
        DOCTEST_FORBIT_EXPRESSION(Result, =)
        DOCTEST_FORBIT_EXPRESSION(Result, +=)
        DOCTEST_FORBIT_EXPRESSION(Result, -=)
        DOCTEST_FORBIT_EXPRESSION(Result, *=)
        DOCTEST_FORBIT_EXPRESSION(Result, /=)
        DOCTEST_FORBIT_EXPRESSION(Result, %=)
        DOCTEST_FORBIT_EXPRESSION(Result, <<=)
        DOCTEST_FORBIT_EXPRESSION(Result, >>=)
        DOCTEST_FORBIT_EXPRESSION(Result, &=)
        DOCTEST_FORBIT_EXPRESSION(Result, ^=)
        DOCTEST_FORBIT_EXPRESSION(Result, |=)
    };

#ifndef DOCTEST_CONFIG_NO_COMPARISON_WARNING_SUPPRESSION

    DOCTEST_CLANG_SUPPRESS_WARNING_PUSH
    DOCTEST_CLANG_SUPPRESS_WARNING("-Wsign-conversion")
    DOCTEST_CLANG_SUPPRESS_WARNING("-Wsign-compare")
    //DOCTEST_CLANG_SUPPRESS_WARNING("-Wdouble-promotion")
    //DOCTEST_CLANG_SUPPRESS_WARNING("-Wconversion")
    //DOCTEST_CLANG_SUPPRESS_WARNING("-Wfloat-equal")

    DOCTEST_GCC_SUPPRESS_WARNING_PUSH
    DOCTEST_GCC_SUPPRESS_WARNING("-Wsign-conversion")
    DOCTEST_GCC_SUPPRESS_WARNING("-Wsign-compare")
    //DOCTEST_GCC_SUPPRESS_WARNING("-Wdouble-promotion")
    //DOCTEST_GCC_SUPPRESS_WARNING("-Wconversion")
    //DOCTEST_GCC_SUPPRESS_WARNING("-Wfloat-equal")

    DOCTEST_MSVC_SUPPRESS_WARNING_PUSH
    // https://stackoverflow.com/questions/39479163 what's the difference between 4018 and 4389
    DOCTEST_MSVC_SUPPRESS_WARNING(4388) // signed/unsigned mismatch
    DOCTEST_MSVC_SUPPRESS_WARNING(4389) // 'operator' : signed/unsigned mismatch
    DOCTEST_MSVC_SUPPRESS_WARNING(4018) // 'expression' : signed/unsigned mismatch
    //DOCTEST_MSVC_SUPPRESS_WARNING(4805) // 'operation' : unsafe mix of type 'type' and type 'type' in operation

#endif // DOCTEST_CONFIG_NO_COMPARISON_WARNING_SUPPRESSION

    // clang-format off
#ifndef DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#define DOCTEST_COMPARISON_RETURN_TYPE bool
#else // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#define DOCTEST_COMPARISON_RETURN_TYPE typename enable_if<can_use_op<L>::value || can_use_op<R>::value, bool>::type
    inline bool eq(const char* lhs, const char* rhs) { return String(lhs) == String(rhs); }
    inline bool ne(const char* lhs, const char* rhs) { return String(lhs) != String(rhs); }
    inline bool lt(const char* lhs, const char* rhs) { return String(lhs) <  String(rhs); }
    inline bool gt(const char* lhs, const char* rhs) { return String(lhs) >  String(rhs); }
    inline bool le(const char* lhs, const char* rhs) { return String(lhs) <= String(rhs); }
    inline bool ge(const char* lhs, const char* rhs) { return String(lhs) >= String(rhs); }
#endif // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
    // clang-format on

#define DOCTEST_RELATIONAL_OP(name, op)                                                            \
    template <typename L, typename R>                                                              \
    DOCTEST_COMPARISON_RETURN_TYPE name(const DOCTEST_REF_WRAP(L) lhs,                             \
                                        const DOCTEST_REF_WRAP(R) rhs) {                           \
        return lhs op rhs;                                                                         \
    }

    DOCTEST_RELATIONAL_OP(eq, ==)
    DOCTEST_RELATIONAL_OP(ne, !=)
    DOCTEST_RELATIONAL_OP(lt, <)
    DOCTEST_RELATIONAL_OP(gt, >)
    DOCTEST_RELATIONAL_OP(le, <=)
    DOCTEST_RELATIONAL_OP(ge, >=)

#ifndef DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#define DOCTEST_CMP_EQ(l, r) l == r
#define DOCTEST_CMP_NE(l, r) l != r
#define DOCTEST_CMP_GT(l, r) l > r
#define DOCTEST_CMP_LT(l, r) l < r
#define DOCTEST_CMP_GE(l, r) l >= r
#define DOCTEST_CMP_LE(l, r) l <= r
#else // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#define DOCTEST_CMP_EQ(l, r) eq(l, r)
#define DOCTEST_CMP_NE(l, r) ne(l, r)
#define DOCTEST_CMP_GT(l, r) gt(l, r)
#define DOCTEST_CMP_LT(l, r) lt(l, r)
#define DOCTEST_CMP_GE(l, r) ge(l, r)
#define DOCTEST_CMP_LE(l, r) le(l, r)
#endif // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING

    template <typename L>
    // cppcheck-suppress copyCtorAndEqOperator
    struct Expression_lhs
    {
        L                lhs;
        assertType::Enum m_at;

        explicit Expression_lhs(L in, assertType::Enum at)
                : lhs(in)
                , m_at(at) {}

        DOCTEST_NOINLINE operator Result() {
            bool res = !!lhs;
            if(m_at & assertType::is_false) //!OCLINT bitwise operator in conditional
                res = !res;

            if(!res || getContextOptions()->success)
                return Result(res, toString(lhs));
            return Result(res);
        }

        // clang-format off
        DOCTEST_DO_BINARY_EXPRESSION_COMPARISON(==, " == ", DOCTEST_CMP_EQ) //!OCLINT bitwise operator in conditional
        DOCTEST_DO_BINARY_EXPRESSION_COMPARISON(!=, " != ", DOCTEST_CMP_NE) //!OCLINT bitwise operator in conditional
        DOCTEST_DO_BINARY_EXPRESSION_COMPARISON(>,  " >  ", DOCTEST_CMP_GT) //!OCLINT bitwise operator in conditional
        DOCTEST_DO_BINARY_EXPRESSION_COMPARISON(<,  " <  ", DOCTEST_CMP_LT) //!OCLINT bitwise operator in conditional
        DOCTEST_DO_BINARY_EXPRESSION_COMPARISON(>=, " >= ", DOCTEST_CMP_GE) //!OCLINT bitwise operator in conditional
        DOCTEST_DO_BINARY_EXPRESSION_COMPARISON(<=, " <= ", DOCTEST_CMP_LE) //!OCLINT bitwise operator in conditional
        // clang-format on

        // forbidding some expressions based on this table: https://en.cppreference.com/w/cpp/language/operator_precedence
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, &)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, ^)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, |)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, &&)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, ||)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, =)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, +=)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, -=)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, *=)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, /=)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, %=)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, <<=)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, >>=)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, &=)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, ^=)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, |=)
        // these 2 are unfortunate because they should be allowed - they have higher precedence over the comparisons, but the
        // ExpressionDecomposer class uses the left shift operator to capture the left operand of the binary expression...
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, <<)
        DOCTEST_FORBIT_EXPRESSION(Expression_lhs, >>)
    };

#ifndef DOCTEST_CONFIG_NO_COMPARISON_WARNING_SUPPRESSION

    DOCTEST_CLANG_SUPPRESS_WARNING_POP
    DOCTEST_MSVC_SUPPRESS_WARNING_POP
    DOCTEST_GCC_SUPPRESS_WARNING_POP

#endif // DOCTEST_CONFIG_NO_COMPARISON_WARNING_SUPPRESSION

    struct DOCTEST_INTERFACE ExpressionDecomposer
    {
        assertType::Enum m_at;

        ExpressionDecomposer(assertType::Enum at);

        // The right operator for capturing expressions is "<=" instead of "<<" (based on the operator precedence table)
        // but then there will be warnings from GCC about "-Wparentheses" and since "_Pragma()" is problematic this will stay for now...
        // https://github.com/catchorg/Catch2/issues/870
        // https://github.com/catchorg/Catch2/issues/565
        template <typename L>
        Expression_lhs<const DOCTEST_REF_WRAP(L)> operator<<(const DOCTEST_REF_WRAP(L) operand) {
            return Expression_lhs<const DOCTEST_REF_WRAP(L)>(operand, m_at);
        }
    };

    struct DOCTEST_INTERFACE TestSuite
    {
        const char* m_test_suite;
        const char* m_description;
        bool        m_skip;
        bool        m_may_fail;
        bool        m_should_fail;
        int         m_expected_failures;
        double      m_timeout;

        TestSuite& operator*(const char* in);

        template <typename T>
        TestSuite& operator*(const T& in) {
            in.fill(*this);
            return *this;
        }
    };

    typedef void (*funcType)();

    struct DOCTEST_INTERFACE TestCase : public TestCaseData
    {
        funcType m_test; // a function pointer to the test case

        const char* m_type; // for templated test cases - gets appended to the real name
        int m_template_id; // an ID used to distinguish between the different versions of a templated test case
        String m_full_name; // contains the name (only for templated test cases!) + the template type

        TestCase(funcType test, const char* file, unsigned line, const TestSuite& test_suite,
                 const char* type = "", int template_id = -1);

        TestCase(const TestCase& other);

        DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(26434) // hides a non-virtual function
        TestCase& operator=(const TestCase& other);
        DOCTEST_MSVC_SUPPRESS_WARNING_POP

        TestCase& operator*(const char* in);

        template <typename T>
        TestCase& operator*(const T& in) {
            in.fill(*this);
            return *this;
        }

        bool operator<(const TestCase& other) const;
    };

    // forward declarations of functions used by the macros
    DOCTEST_INTERFACE int  regTest(const TestCase& tc);
    DOCTEST_INTERFACE int  setTestSuite(const TestSuite& ts);
    DOCTEST_INTERFACE bool isDebuggerActive();

    template<typename T>
    int instantiationHelper(const T&) { return 0; }

    namespace binaryAssertComparison {
        enum Enum
        {
            eq = 0,
            ne,
            gt,
            lt,
            ge,
            le
        };
    } // namespace binaryAssertComparison

    // clang-format off
    template <int, class L, class R> struct RelationalComparator     { bool operator()(const DOCTEST_REF_WRAP(L),     const DOCTEST_REF_WRAP(R)    ) const { return false;        } };

#define DOCTEST_BINARY_RELATIONAL_OP(n, op) \
    template <class L, class R> struct RelationalComparator<n, L, R> { bool operator()(const DOCTEST_REF_WRAP(L) lhs, const DOCTEST_REF_WRAP(R) rhs) const { return op(lhs, rhs); } };
    // clang-format on

    DOCTEST_BINARY_RELATIONAL_OP(0, eq)
    DOCTEST_BINARY_RELATIONAL_OP(1, ne)
    DOCTEST_BINARY_RELATIONAL_OP(2, gt)
    DOCTEST_BINARY_RELATIONAL_OP(3, lt)
    DOCTEST_BINARY_RELATIONAL_OP(4, ge)
    DOCTEST_BINARY_RELATIONAL_OP(5, le)

    struct DOCTEST_INTERFACE ResultBuilder : public AssertData
    {
        ResultBuilder(assertType::Enum at, const char* file, int line, const char* expr,
                      const char* exception_type = "", const char* exception_string = "");

        void setResult(const Result& res);

        template <int comparison, typename L, typename R>
        DOCTEST_NOINLINE void binary_assert(const DOCTEST_REF_WRAP(L) lhs,
                                            const DOCTEST_REF_WRAP(R) rhs) {
            m_failed = !RelationalComparator<comparison, L, R>()(lhs, rhs);
            if(m_failed || getContextOptions()->success)
                m_decomp = stringifyBinaryExpr(lhs, ", ", rhs);
        }

        template <typename L>
        DOCTEST_NOINLINE void unary_assert(const DOCTEST_REF_WRAP(L) val) {
            m_failed = !val;

            if(m_at & assertType::is_false) //!OCLINT bitwise operator in conditional
                m_failed = !m_failed;

            if(m_failed || getContextOptions()->success)
                m_decomp = toString(val);
        }

        void translateException();

        bool log();
        void react() const;
    };

    namespace assertAction {
        enum Enum
        {
            nothing     = 0,
            dbgbreak    = 1,
            shouldthrow = 2
        };
    } // namespace assertAction

    DOCTEST_INTERFACE void failed_out_of_a_testing_context(const AssertData& ad);

    DOCTEST_INTERFACE void decomp_assert(assertType::Enum at, const char* file, int line,
                                         const char* expr, Result result);

#define DOCTEST_ASSERT_OUT_OF_TESTS(decomp)                                                        \
    do {                                                                                           \
        if(!is_running_in_test) {                                                                  \
            if(failed) {                                                                           \
                ResultBuilder rb(at, file, line, expr);                                            \
                rb.m_failed = failed;                                                              \
                rb.m_decomp = decomp;                                                              \
                failed_out_of_a_testing_context(rb);                                               \
                if(isDebuggerActive() && !getContextOptions()->no_breaks)                          \
                    DOCTEST_BREAK_INTO_DEBUGGER();                                                 \
                if(checkIfShouldThrow(at))                                                         \
                    throwException();                                                              \
            }                                                                                      \
            return;                                                                                \
        }                                                                                          \
    } while(false)

#define DOCTEST_ASSERT_IN_TESTS(decomp)                                                            \
    ResultBuilder rb(at, file, line, expr);                                                        \
    rb.m_failed = failed;                                                                          \
    if(rb.m_failed || getContextOptions()->success)                                                \
        rb.m_decomp = decomp;                                                                      \
    if(rb.log())                                                                                   \
        DOCTEST_BREAK_INTO_DEBUGGER();                                                             \
    if(rb.m_failed && checkIfShouldThrow(at))                                                      \
    throwException()

    template <int comparison, typename L, typename R>
    DOCTEST_NOINLINE void binary_assert(assertType::Enum at, const char* file, int line,
                                        const char* expr, const DOCTEST_REF_WRAP(L) lhs,
                                        const DOCTEST_REF_WRAP(R) rhs) {
        bool failed = !RelationalComparator<comparison, L, R>()(lhs, rhs);

        // ###################################################################################
        // IF THE DEBUGGER BREAKS HERE - GO 1 LEVEL UP IN THE CALLSTACK FOR THE FAILING ASSERT
        // THIS IS THE EFFECT OF HAVING 'DOCTEST_CONFIG_SUPER_FAST_ASSERTS' DEFINED
        // ###################################################################################
        DOCTEST_ASSERT_OUT_OF_TESTS(stringifyBinaryExpr(lhs, ", ", rhs));
        DOCTEST_ASSERT_IN_TESTS(stringifyBinaryExpr(lhs, ", ", rhs));
    }

    template <typename L>
    DOCTEST_NOINLINE void unary_assert(assertType::Enum at, const char* file, int line,
                                       const char* expr, const DOCTEST_REF_WRAP(L) val) {
        bool failed = !val;

        if(at & assertType::is_false) //!OCLINT bitwise operator in conditional
            failed = !failed;

        // ###################################################################################
        // IF THE DEBUGGER BREAKS HERE - GO 1 LEVEL UP IN THE CALLSTACK FOR THE FAILING ASSERT
        // THIS IS THE EFFECT OF HAVING 'DOCTEST_CONFIG_SUPER_FAST_ASSERTS' DEFINED
        // ###################################################################################
        DOCTEST_ASSERT_OUT_OF_TESTS(toString(val));
        DOCTEST_ASSERT_IN_TESTS(toString(val));
    }

    struct DOCTEST_INTERFACE IExceptionTranslator
    {
        IExceptionTranslator();
        virtual ~IExceptionTranslator();
        virtual bool translate(String&) const = 0;
    };

    template <typename T>
    class ExceptionTranslator : public IExceptionTranslator //!OCLINT destructor of virtual class
    {
    public:
        explicit ExceptionTranslator(String (*translateFunction)(T))
                : m_translateFunction(translateFunction) {}

        bool translate(String& res) const override {
#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
            try {
                throw; // lgtm [cpp/rethrow-no-exception]
                // cppcheck-suppress catchExceptionByValue
            } catch(T ex) {                    // NOLINT
                res = m_translateFunction(ex); //!OCLINT parameter reassignment
                return true;
            } catch(...) {} //!OCLINT -  empty catch statement
#endif                      // DOCTEST_CONFIG_NO_EXCEPTIONS
            ((void)res);    // to silence -Wunused-parameter
            return false;
        }

    private:
        String (*m_translateFunction)(T);
    };

    DOCTEST_INTERFACE void registerExceptionTranslatorImpl(const IExceptionTranslator* et);

    template <bool C>
    struct StringStreamBase
    {
        template <typename T>
        static void convert(std::ostream* s, const T& in) {
            *s << toString(in);
        }

        // always treat char* as a string in this context - no matter
        // if DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING is defined
        static void convert(std::ostream* s, const char* in) { *s << String(in); }
    };

    template <>
    struct StringStreamBase<true>
    {
        template <typename T>
        static void convert(std::ostream* s, const T& in) {
            *s << in;
        }
    };

    template <typename T>
    struct StringStream : public StringStreamBase<has_insertion_operator<T>::value>
    {};

    template <typename T>
    void toStream(std::ostream* s, const T& value) {
        StringStream<T>::convert(s, value);
    }

#ifdef DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
    DOCTEST_INTERFACE void toStream(std::ostream* s, char* in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, const char* in);
#endif // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
    DOCTEST_INTERFACE void toStream(std::ostream* s, bool in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, float in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, double in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, double long in);

    DOCTEST_INTERFACE void toStream(std::ostream* s, char in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, char signed in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, char unsigned in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int short in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int short unsigned in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int unsigned in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int long in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int long unsigned in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int long long in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int long long unsigned in);

    // ContextScope base class used to allow implementing methods of ContextScope 
    // that don't depend on the template parameter in doctest.cpp.
    class DOCTEST_INTERFACE ContextScopeBase : public IContextScope {
    protected:
        ContextScopeBase();

        void destroy();
    };

    template <typename L> class ContextScope : public ContextScopeBase
    {
        const L &lambda_;

    public:
        explicit ContextScope(const L &lambda) : lambda_(lambda) {}

        ContextScope(ContextScope &&other) : lambda_(other.lambda_) {}

        void stringify(std::ostream* s) const override { lambda_(s); }

        ~ContextScope() override { destroy(); }
    };

    struct DOCTEST_INTERFACE MessageBuilder : public MessageData
    {
        std::ostream* m_stream;

        MessageBuilder(const char* file, int line, assertType::Enum severity);
        MessageBuilder() = delete;
        ~MessageBuilder();

        template <typename T>
        MessageBuilder& operator<<(const T& in) {
            toStream(m_stream, in);
            return *this;
        }

        bool log();
        void react();
    };
    
    template <typename L>
    ContextScope<L> MakeContextScope(const L &lambda) {
        return ContextScope<L>(lambda);
    }
} // namespace detail

#define DOCTEST_DEFINE_DECORATOR(name, type, def)                                                  \
    struct name                                                                                    \
    {                                                                                              \
        type data;                                                                                 \
        name(type in = def)                                                                        \
                : data(in) {}                                                                      \
        void fill(detail::TestCase& state) const { state.DOCTEST_CAT(m_, name) = data; }           \
        void fill(detail::TestSuite& state) const { state.DOCTEST_CAT(m_, name) = data; }          \
    }

DOCTEST_DEFINE_DECORATOR(test_suite, const char*, "");
DOCTEST_DEFINE_DECORATOR(description, const char*, "");
DOCTEST_DEFINE_DECORATOR(skip, bool, true);
DOCTEST_DEFINE_DECORATOR(timeout, double, 0);
DOCTEST_DEFINE_DECORATOR(may_fail, bool, true);
DOCTEST_DEFINE_DECORATOR(should_fail, bool, true);
DOCTEST_DEFINE_DECORATOR(expected_failures, int, 0);

template <typename T>
int registerExceptionTranslator(String (*translateFunction)(T)) {
    DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wexit-time-destructors")
    static detail::ExceptionTranslator<T> exceptionTranslator(translateFunction);
    DOCTEST_CLANG_SUPPRESS_WARNING_POP
    detail::registerExceptionTranslatorImpl(&exceptionTranslator);
    return 0;
}

} // namespace doctest

// in a separate namespace outside of doctest because the DOCTEST_TEST_SUITE macro
// introduces an anonymous namespace in which getCurrentTestSuite gets overridden
namespace doctest_detail_test_suite_ns {
DOCTEST_INTERFACE doctest::detail::TestSuite& getCurrentTestSuite();
} // namespace doctest_detail_test_suite_ns

namespace doctest {
#else  // DOCTEST_CONFIG_DISABLE
template <typename T>
int registerExceptionTranslator(String (*)(T)) {
    return 0;
}
#endif // DOCTEST_CONFIG_DISABLE

namespace detail {
    typedef void (*assert_handler)(const AssertData&);
    struct ContextState;
} // namespace detail

class DOCTEST_INTERFACE Context
{
    detail::ContextState* p;

    void parseArgs(int argc, const char* const* argv, bool withDefaults = false);

public:
    explicit Context(int argc = 0, const char* const* argv = nullptr);

    ~Context();

    void applyCommandLine(int argc, const char* const* argv);

    void addFilter(const char* filter, const char* value);
    void clearFilters();
    void setOption(const char* option, int value);
    void setOption(const char* option, const char* value);

    bool shouldExit();

    void setAsDefaultForAssertsOutOfTestCases();

    void setAssertHandler(detail::assert_handler ah);

    int run();
};

namespace TestCaseFailureReason {
    enum Enum
    {
        None                     = 0,
        AssertFailure            = 1,   // an assertion has failed in the test case
        Exception                = 2,   // test case threw an exception
        Crash                    = 4,   // a crash...
        TooManyFailedAsserts     = 8,   // the abort-after option
        Timeout                  = 16,  // see the timeout decorator
        ShouldHaveFailedButDidnt = 32,  // see the should_fail decorator
        ShouldHaveFailedAndDid   = 64,  // see the should_fail decorator
        DidntFailExactlyNumTimes = 128, // see the expected_failures decorator
        FailedExactlyNumTimes    = 256, // see the expected_failures decorator
        CouldHaveFailedAndDid    = 512  // see the may_fail decorator
    };
} // namespace TestCaseFailureReason

struct DOCTEST_INTERFACE CurrentTestCaseStats
{
    int    numAssertsCurrentTest;
    int    numAssertsFailedCurrentTest;
    double seconds;
    int    failure_flags; // use TestCaseFailureReason::Enum
};

struct DOCTEST_INTERFACE TestCaseException
{
    String error_string;
    bool   is_crash;
};

struct DOCTEST_INTERFACE TestRunStats
{
    unsigned numTestCases;
    unsigned numTestCasesPassingFilters;
    unsigned numTestSuitesPassingFilters;
    unsigned numTestCasesFailed;
    int      numAsserts;
    int      numAssertsFailed;
};

struct QueryData
{
    const TestRunStats*  run_stats = nullptr;
    const TestCaseData** data      = nullptr;
    unsigned             num_data  = 0;
};

struct DOCTEST_INTERFACE IReporter
{
    // The constructor has to accept "const ContextOptions&" as a single argument
    // which has most of the options for the run + a pointer to the stdout stream
    // Reporter(const ContextOptions& in)

    // called when a query should be reported (listing test cases, printing the version, etc.)
    virtual void report_query(const QueryData&) = 0;

    // called when the whole test run starts
    virtual void test_run_start() = 0;
    // called when the whole test run ends (caching a pointer to the input doesn't make sense here)
    virtual void test_run_end(const TestRunStats&) = 0;

    // called when a test case is started (safe to cache a pointer to the input)
    virtual void test_case_start(const TestCaseData&) = 0;
    // called when a test case is reentered because of unfinished subcases (safe to cache a pointer to the input)
    virtual void test_case_reenter(const TestCaseData&) = 0;
    // called when a test case has ended
    virtual void test_case_end(const CurrentTestCaseStats&) = 0;

    // called when an exception is thrown from the test case (or it crashes)
    virtual void test_case_exception(const TestCaseException&) = 0;

    // called whenever a subcase is entered (don't cache pointers to the input)
    virtual void subcase_start(const SubcaseSignature&) = 0;
    // called whenever a subcase is exited (don't cache pointers to the input)
    virtual void subcase_end() = 0;

    // called for each assert (don't cache pointers to the input)
    virtual void log_assert(const AssertData&) = 0;
    // called for each message (don't cache pointers to the input)
    virtual void log_message(const MessageData&) = 0;

    // called when a test case is skipped either because it doesn't pass the filters, has a skip decorator
    // or isn't in the execution range (between first and last) (safe to cache a pointer to the input)
    virtual void test_case_skipped(const TestCaseData&) = 0;

    // doctest will not be managing the lifetimes of reporters given to it but this would still be nice to have
    virtual ~IReporter();

    // can obtain all currently active contexts and stringify them if one wishes to do so
    static int                         get_num_active_contexts();
    static const IContextScope* const* get_active_contexts();

    // can iterate through contexts which have been stringified automatically in their destructors when an exception has been thrown
    static int           get_num_stringified_contexts();
    static const String* get_stringified_contexts();
};

namespace detail {
    typedef IReporter* (*reporterCreatorFunc)(const ContextOptions&);

    DOCTEST_INTERFACE void registerReporterImpl(const char* name, int prio, reporterCreatorFunc c, bool isReporter);

    template <typename Reporter>
    IReporter* reporterCreator(const ContextOptions& o) {
        return new Reporter(o);
    }
} // namespace detail

template <typename Reporter>
int registerReporter(const char* name, int priority, bool isReporter) {
    detail::registerReporterImpl(name, priority, detail::reporterCreator<Reporter>, isReporter);
    return 0;
}
} // namespace doctest

// if registering is not disabled
#if !defined(DOCTEST_CONFIG_DISABLE)

// common code in asserts - for convenience
#define DOCTEST_ASSERT_LOG_AND_REACT(b)                                                            \
    if(b.log())                                                                                    \
        DOCTEST_BREAK_INTO_DEBUGGER();                                                             \
    b.react()

#ifdef DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS
#define DOCTEST_WRAP_IN_TRY(x) x;
#else // DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS
#define DOCTEST_WRAP_IN_TRY(x)                                                                     \
    try {                                                                                          \
        x;                                                                                         \
    } catch(...) { _DOCTEST_RB.translateException(); }
#endif // DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS

#ifdef DOCTEST_CONFIG_VOID_CAST_EXPRESSIONS
#define DOCTEST_CAST_TO_VOID(x)                                                                    \
    DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wuseless-cast")                                       \
    static_cast<void>(x);                                                                          \
    DOCTEST_GCC_SUPPRESS_WARNING_POP
#else // DOCTEST_CONFIG_VOID_CAST_EXPRESSIONS
#define DOCTEST_CAST_TO_VOID(x) x;
#endif // DOCTEST_CONFIG_VOID_CAST_EXPRESSIONS

// registers the test by initializing a dummy var with a function
#define DOCTEST_REGISTER_FUNCTION(global_prefix, f, decorators)                                    \
    global_prefix DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_ANONYMOUS(_DOCTEST_ANON_VAR_)) =              \
            doctest::detail::regTest(                                                              \
                    doctest::detail::TestCase(                                                     \
                            f, __FILE__, __LINE__,                                                 \
                            doctest_detail_test_suite_ns::getCurrentTestSuite()) *                 \
                    decorators);                                                                   \
    DOCTEST_GLOBAL_NO_WARNINGS_END()

#define DOCTEST_IMPLEMENT_FIXTURE(der, base, func, decorators)                                     \
    namespace {                                                                                    \
        struct der : public base                                                                   \
        {                                                                                          \
            void f();                                                                              \
        };                                                                                         \
        static void func() {                                                                       \
            der v;                                                                                 \
            v.f();                                                                                 \
        }                                                                                          \
        DOCTEST_REGISTER_FUNCTION(DOCTEST_EMPTY, func, decorators)                                 \
    }                                                                                              \
    inline DOCTEST_NOINLINE void der::f()

#define DOCTEST_CREATE_AND_REGISTER_FUNCTION(f, decorators)                                        \
    static void f();                                                                               \
    DOCTEST_REGISTER_FUNCTION(DOCTEST_EMPTY, f, decorators)                                        \
    static void f()

#define DOCTEST_CREATE_AND_REGISTER_FUNCTION_IN_CLASS(f, proxy, decorators)                        \
    static doctest::detail::funcType proxy() { return f; }                                         \
    DOCTEST_REGISTER_FUNCTION(inline const, proxy(), decorators)                                   \
    static void f()

// for registering tests
#define DOCTEST_TEST_CASE(decorators)                                                              \
    DOCTEST_CREATE_AND_REGISTER_FUNCTION(DOCTEST_ANONYMOUS(_DOCTEST_ANON_FUNC_), decorators)

// for registering tests in classes - requires C++17 for inline variables!
#if __cplusplus >= 201703L || (DOCTEST_MSVC >= DOCTEST_COMPILER(19, 12, 0) && _MSVC_LANG >= 201703L)
#define DOCTEST_TEST_CASE_CLASS(decorators)                                                        \
    DOCTEST_CREATE_AND_REGISTER_FUNCTION_IN_CLASS(DOCTEST_ANONYMOUS(_DOCTEST_ANON_FUNC_),          \
                                                  DOCTEST_ANONYMOUS(_DOCTEST_ANON_PROXY_),         \
                                                  decorators)
#else // DOCTEST_TEST_CASE_CLASS
#define DOCTEST_TEST_CASE_CLASS(...)                                                               \
    TEST_CASES_CAN_BE_REGISTERED_IN_CLASSES_ONLY_IN_CPP17_MODE_OR_WITH_VS_2017_OR_NEWER
#endif // DOCTEST_TEST_CASE_CLASS

// for registering tests with a fixture
#define DOCTEST_TEST_CASE_FIXTURE(c, decorators)                                                   \
    DOCTEST_IMPLEMENT_FIXTURE(DOCTEST_ANONYMOUS(_DOCTEST_ANON_CLASS_), c,                          \
                              DOCTEST_ANONYMOUS(_DOCTEST_ANON_FUNC_), decorators)

// for converting types to strings without the <typeinfo> header and demangling
#define DOCTEST_TYPE_TO_STRING_IMPL(...)                                                           \
    template <>                                                                                    \
    inline const char* type_to_string<__VA_ARGS__>() {                                             \
        return "<" #__VA_ARGS__ ">";                                                               \
    }
#define DOCTEST_TYPE_TO_STRING(...)                                                                \
    namespace doctest { namespace detail {                                                         \
            DOCTEST_TYPE_TO_STRING_IMPL(__VA_ARGS__)                                               \
        }                                                                                          \
    }                                                                                              \
    typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

#define DOCTEST_TEST_CASE_TEMPLATE_DEFINE_IMPL(dec, T, iter, func)                                 \
    template <typename T>                                                                          \
    static void func();                                                                            \
    namespace {                                                                                    \
        template <typename Tuple>                                                                  \
        struct iter;                                                                               \
        template <typename Type, typename... Rest>                                                 \
        struct iter<std::tuple<Type, Rest...>>                                                     \
        {                                                                                          \
            iter(const char* file, unsigned line, int index) {                                     \
                doctest::detail::regTest(doctest::detail::TestCase(func<Type>, file, line,         \
                                            doctest_detail_test_suite_ns::getCurrentTestSuite(),   \
                                            doctest::detail::type_to_string<Type>(),               \
                                            int(line) * 1000 + index)                              \
                                         * dec);                                                   \
                iter<std::tuple<Rest...>>(file, line, index + 1);                                  \
            }                                                                                      \
        };                                                                                         \
        template <>                                                                                \
        struct iter<std::tuple<>>                                                                  \
        {                                                                                          \
            iter(const char*, unsigned, int) {}                                                    \
        };                                                                                         \
    }                                                                                              \
    template <typename T>                                                                          \
    static void func()

#define DOCTEST_TEST_CASE_TEMPLATE_DEFINE(dec, T, id)                                              \
    DOCTEST_TEST_CASE_TEMPLATE_DEFINE_IMPL(dec, T, DOCTEST_CAT(id, ITERATOR),                      \
                                           DOCTEST_ANONYMOUS(_DOCTEST_ANON_TMP_))

#define DOCTEST_TEST_CASE_TEMPLATE_INSTANTIATE_IMPL(id, anon, ...)                                 \
    DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_CAT(anon, DUMMY)) =                                         \
        doctest::detail::instantiationHelper(DOCTEST_CAT(id, ITERATOR)<__VA_ARGS__>(__FILE__, __LINE__, 0));\
    DOCTEST_GLOBAL_NO_WARNINGS_END()

#define DOCTEST_TEST_CASE_TEMPLATE_INVOKE(id, ...)                                                 \
    DOCTEST_TEST_CASE_TEMPLATE_INSTANTIATE_IMPL(id, DOCTEST_ANONYMOUS(_DOCTEST_ANON_TMP_), std::tuple<__VA_ARGS__>) \
    typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

#define DOCTEST_TEST_CASE_TEMPLATE_APPLY(id, ...)                                                  \
    DOCTEST_TEST_CASE_TEMPLATE_INSTANTIATE_IMPL(id, DOCTEST_ANONYMOUS(_DOCTEST_ANON_TMP_), __VA_ARGS__) \
    typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

#define DOCTEST_TEST_CASE_TEMPLATE_IMPL(dec, T, anon, ...)                                         \
    DOCTEST_TEST_CASE_TEMPLATE_DEFINE_IMPL(dec, T, DOCTEST_CAT(anon, ITERATOR), anon);             \
    DOCTEST_TEST_CASE_TEMPLATE_INSTANTIATE_IMPL(anon, anon, std::tuple<__VA_ARGS__>)               \
    template <typename T>                                                                          \
    static void anon()

#define DOCTEST_TEST_CASE_TEMPLATE(dec, T, ...)                                                    \
    DOCTEST_TEST_CASE_TEMPLATE_IMPL(dec, T, DOCTEST_ANONYMOUS(_DOCTEST_ANON_TMP_), __VA_ARGS__)

// for subcases
#define DOCTEST_SUBCASE(name)                                                                      \
    if(const doctest::detail::Subcase & DOCTEST_ANONYMOUS(_DOCTEST_ANON_SUBCASE_) DOCTEST_UNUSED = \
               doctest::detail::Subcase(name, __FILE__, __LINE__))

// for grouping tests in test suites by using code blocks
#define DOCTEST_TEST_SUITE_IMPL(decorators, ns_name)                                               \
    namespace ns_name { namespace doctest_detail_test_suite_ns {                                   \
            static DOCTEST_NOINLINE doctest::detail::TestSuite& getCurrentTestSuite() {            \
                DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4640)                                      \
                DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wexit-time-destructors")                \
                static doctest::detail::TestSuite data;                                            \
                static bool                       inited = false;                                  \
                DOCTEST_MSVC_SUPPRESS_WARNING_POP                                                  \
                DOCTEST_CLANG_SUPPRESS_WARNING_POP                                                 \
                if(!inited) {                                                                      \
                    data* decorators;                                                              \
                    inited = true;                                                                 \
                }                                                                                  \
                return data;                                                                       \
            }                                                                                      \
        }                                                                                          \
    }                                                                                              \
    namespace ns_name

#define DOCTEST_TEST_SUITE(decorators)                                                             \
    DOCTEST_TEST_SUITE_IMPL(decorators, DOCTEST_ANONYMOUS(_DOCTEST_ANON_SUITE_))

// for starting a testsuite block
#define DOCTEST_TEST_SUITE_BEGIN(decorators)                                                       \
    DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_ANONYMOUS(_DOCTEST_ANON_VAR_)) =                            \
            doctest::detail::setTestSuite(doctest::detail::TestSuite() * decorators);              \
    DOCTEST_GLOBAL_NO_WARNINGS_END()                                                               \
    typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

// for ending a testsuite block
#define DOCTEST_TEST_SUITE_END                                                                     \
    DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_ANONYMOUS(_DOCTEST_ANON_VAR_)) =                            \
            doctest::detail::setTestSuite(doctest::detail::TestSuite() * "");                      \
    DOCTEST_GLOBAL_NO_WARNINGS_END()                                                               \
    typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

// for registering exception translators
#define DOCTEST_REGISTER_EXCEPTION_TRANSLATOR_IMPL(translatorName, signature)                      \
    inline doctest::String translatorName(signature);                                              \
    DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_ANONYMOUS(_DOCTEST_ANON_TRANSLATOR_)) =                     \
            doctest::registerExceptionTranslator(translatorName);                                  \
    DOCTEST_GLOBAL_NO_WARNINGS_END()                                                               \
    doctest::String translatorName(signature)

#define DOCTEST_REGISTER_EXCEPTION_TRANSLATOR(signature)                                           \
    DOCTEST_REGISTER_EXCEPTION_TRANSLATOR_IMPL(DOCTEST_ANONYMOUS(_DOCTEST_ANON_TRANSLATOR_),       \
                                               signature)

// for registering reporters
#define DOCTEST_REGISTER_REPORTER(name, priority, reporter)                                        \
    DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_ANONYMOUS(_DOCTEST_ANON_REPORTER_)) =                       \
            doctest::registerReporter<reporter>(name, priority, true);                             \
    DOCTEST_GLOBAL_NO_WARNINGS_END() typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

// for registering listeners
#define DOCTEST_REGISTER_LISTENER(name, priority, reporter)                                        \
    DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_ANONYMOUS(_DOCTEST_ANON_REPORTER_)) =                       \
            doctest::registerReporter<reporter>(name, priority, false);                            \
    DOCTEST_GLOBAL_NO_WARNINGS_END() typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

// for logging
#define DOCTEST_INFO(expression)                                                                   \
    DOCTEST_INFO_IMPL(DOCTEST_ANONYMOUS(_DOCTEST_CAPTURE_), DOCTEST_ANONYMOUS(_DOCTEST_CAPTURE_),  \
                      DOCTEST_ANONYMOUS(_DOCTEST_CAPTURE_), expression)

#define DOCTEST_INFO_IMPL(lambda_name, mb_name, s_name, expression)                                \
    DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4626)                                                  \
    auto lambda_name = [&](std::ostream* s_name) {                                                 \
        doctest::detail::MessageBuilder mb_name(__FILE__, __LINE__, doctest::assertType::is_warn); \
        mb_name.m_stream = s_name;                                                                 \
        mb_name << expression;                                                                     \
    };                                                                                             \
    DOCTEST_MSVC_SUPPRESS_WARNING_POP                                                              \
    auto DOCTEST_ANONYMOUS(_DOCTEST_CAPTURE_) = doctest::detail::MakeContextScope(lambda_name)

#define DOCTEST_CAPTURE(x) DOCTEST_INFO(#x " := " << x)

#define DOCTEST_ADD_AT_IMPL(type, file, line, mb, x)                                               \
    do {                                                                                           \
        doctest::detail::MessageBuilder mb(file, line, doctest::assertType::type);                 \
        mb << x;                                                                                   \
        DOCTEST_ASSERT_LOG_AND_REACT(mb);                                                          \
    } while((void)0, 0)

// clang-format off
#define DOCTEST_ADD_MESSAGE_AT(file, line, x) DOCTEST_ADD_AT_IMPL(is_warn, file, line, DOCTEST_ANONYMOUS(_DOCTEST_MESSAGE_), x)
#define DOCTEST_ADD_FAIL_CHECK_AT(file, line, x) DOCTEST_ADD_AT_IMPL(is_check, file, line, DOCTEST_ANONYMOUS(_DOCTEST_MESSAGE_), x)
#define DOCTEST_ADD_FAIL_AT(file, line, x) DOCTEST_ADD_AT_IMPL(is_require, file, line, DOCTEST_ANONYMOUS(_DOCTEST_MESSAGE_), x)
// clang-format on

#define DOCTEST_MESSAGE(x) DOCTEST_ADD_MESSAGE_AT(__FILE__, __LINE__, x)
#define DOCTEST_FAIL_CHECK(x) DOCTEST_ADD_FAIL_CHECK_AT(__FILE__, __LINE__, x)
#define DOCTEST_FAIL(x) DOCTEST_ADD_FAIL_AT(__FILE__, __LINE__, x)

#define DOCTEST_TO_LVALUE(...) __VA_ARGS__ // Not removed to keep backwards compatibility.

#ifndef DOCTEST_CONFIG_SUPER_FAST_ASSERTS

#define DOCTEST_ASSERT_IMPLEMENT_2(assert_type, ...)                                               \
    DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Woverloaded-shift-op-parentheses")                  \
    doctest::detail::ResultBuilder _DOCTEST_RB(doctest::assertType::assert_type, __FILE__,         \
                                               __LINE__, #__VA_ARGS__);                            \
    DOCTEST_WRAP_IN_TRY(_DOCTEST_RB.setResult(                                                     \
            doctest::detail::ExpressionDecomposer(doctest::assertType::assert_type)                \
            << __VA_ARGS__))                                                                       \
    DOCTEST_ASSERT_LOG_AND_REACT(_DOCTEST_RB)                                                      \
    DOCTEST_CLANG_SUPPRESS_WARNING_POP

#define DOCTEST_ASSERT_IMPLEMENT_1(assert_type, ...)                                               \
    do {                                                                                           \
        DOCTEST_ASSERT_IMPLEMENT_2(assert_type, __VA_ARGS__);                                      \
    } while((void)0, 0)

#else // DOCTEST_CONFIG_SUPER_FAST_ASSERTS

// necessary for <ASSERT>_MESSAGE
#define DOCTEST_ASSERT_IMPLEMENT_2 DOCTEST_ASSERT_IMPLEMENT_1

#define DOCTEST_ASSERT_IMPLEMENT_1(assert_type, ...)                                               \
    DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Woverloaded-shift-op-parentheses")                  \
    doctest::detail::decomp_assert(                                                                \
            doctest::assertType::assert_type, __FILE__, __LINE__, #__VA_ARGS__,                    \
            doctest::detail::ExpressionDecomposer(doctest::assertType::assert_type)                \
                    << __VA_ARGS__) DOCTEST_CLANG_SUPPRESS_WARNING_POP

#endif // DOCTEST_CONFIG_SUPER_FAST_ASSERTS

#define DOCTEST_WARN(...) DOCTEST_ASSERT_IMPLEMENT_1(DT_WARN, __VA_ARGS__)
#define DOCTEST_CHECK(...) DOCTEST_ASSERT_IMPLEMENT_1(DT_CHECK, __VA_ARGS__)
#define DOCTEST_REQUIRE(...) DOCTEST_ASSERT_IMPLEMENT_1(DT_REQUIRE, __VA_ARGS__)
#define DOCTEST_WARN_FALSE(...) DOCTEST_ASSERT_IMPLEMENT_1(DT_WARN_FALSE, __VA_ARGS__)
#define DOCTEST_CHECK_FALSE(...) DOCTEST_ASSERT_IMPLEMENT_1(DT_CHECK_FALSE, __VA_ARGS__)
#define DOCTEST_REQUIRE_FALSE(...) DOCTEST_ASSERT_IMPLEMENT_1(DT_REQUIRE_FALSE, __VA_ARGS__)

// clang-format off
#define DOCTEST_WARN_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2(DT_WARN, cond); } while((void)0, 0)
#define DOCTEST_CHECK_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2(DT_CHECK, cond); } while((void)0, 0)
#define DOCTEST_REQUIRE_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2(DT_REQUIRE, cond); } while((void)0, 0)
#define DOCTEST_WARN_FALSE_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2(DT_WARN_FALSE, cond); } while((void)0, 0)
#define DOCTEST_CHECK_FALSE_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2(DT_CHECK_FALSE, cond); } while((void)0, 0)
#define DOCTEST_REQUIRE_FALSE_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2(DT_REQUIRE_FALSE, cond); } while((void)0, 0)
// clang-format on

#define DOCTEST_ASSERT_THROWS_AS(expr, assert_type, message, ...)                                  \
    do {                                                                                           \
        if(!doctest::getContextOptions()->no_throw) {                                              \
            doctest::detail::ResultBuilder _DOCTEST_RB(doctest::assertType::assert_type, __FILE__, \
                                                       __LINE__, #expr, #__VA_ARGS__, message);    \
            try {                                                                                  \
                DOCTEST_CAST_TO_VOID(expr)                                                         \
            } catch(const doctest::detail::remove_const<                                           \
                    doctest::detail::remove_reference<__VA_ARGS__>::type>::type&) {                \
                _DOCTEST_RB.translateException();                                                  \
                _DOCTEST_RB.m_threw_as = true;                                                     \
            } catch(...) { _DOCTEST_RB.translateException(); }                                     \
            DOCTEST_ASSERT_LOG_AND_REACT(_DOCTEST_RB);                                             \
        }                                                                                          \
    } while((void)0, 0)

#define DOCTEST_ASSERT_THROWS_WITH(expr, assert_type, ...)                                         \
    do {                                                                                           \
        if(!doctest::getContextOptions()->no_throw) {                                              \
            doctest::detail::ResultBuilder _DOCTEST_RB(doctest::assertType::assert_type, __FILE__, \
                                                       __LINE__, #expr, "", __VA_ARGS__);          \
            try {                                                                                  \
                DOCTEST_CAST_TO_VOID(expr)                                                         \
            } catch(...) { _DOCTEST_RB.translateException(); }                                     \
            DOCTEST_ASSERT_LOG_AND_REACT(_DOCTEST_RB);                                             \
        }                                                                                          \
    } while((void)0, 0)

#define DOCTEST_ASSERT_NOTHROW(expr, assert_type)                                                  \
    do {                                                                                           \
        doctest::detail::ResultBuilder _DOCTEST_RB(doctest::assertType::assert_type, __FILE__,     \
                                                   __LINE__, #expr);                               \
        try {                                                                                      \
            DOCTEST_CAST_TO_VOID(expr)                                                             \
        } catch(...) { _DOCTEST_RB.translateException(); }                                         \
        DOCTEST_ASSERT_LOG_AND_REACT(_DOCTEST_RB);                                                 \
    } while((void)0, 0)

// clang-format off
#define DOCTEST_WARN_THROWS(expr) DOCTEST_ASSERT_THROWS_WITH(expr, DT_WARN_THROWS, "")
#define DOCTEST_CHECK_THROWS(expr) DOCTEST_ASSERT_THROWS_WITH(expr, DT_CHECK_THROWS, "")
#define DOCTEST_REQUIRE_THROWS(expr) DOCTEST_ASSERT_THROWS_WITH(expr, DT_REQUIRE_THROWS, "")

#define DOCTEST_WARN_THROWS_AS(expr, ...) DOCTEST_ASSERT_THROWS_AS(expr, DT_WARN_THROWS_AS, "", __VA_ARGS__)
#define DOCTEST_CHECK_THROWS_AS(expr, ...) DOCTEST_ASSERT_THROWS_AS(expr, DT_CHECK_THROWS_AS, "", __VA_ARGS__)
#define DOCTEST_REQUIRE_THROWS_AS(expr, ...) DOCTEST_ASSERT_THROWS_AS(expr, DT_REQUIRE_THROWS_AS, "", __VA_ARGS__)

#define DOCTEST_WARN_THROWS_WITH(expr, ...) DOCTEST_ASSERT_THROWS_WITH(expr, DT_WARN_THROWS_WITH, __VA_ARGS__)
#define DOCTEST_CHECK_THROWS_WITH(expr, ...) DOCTEST_ASSERT_THROWS_WITH(expr, DT_CHECK_THROWS_WITH, __VA_ARGS__)
#define DOCTEST_REQUIRE_THROWS_WITH(expr, ...) DOCTEST_ASSERT_THROWS_WITH(expr, DT_REQUIRE_THROWS_WITH, __VA_ARGS__)

#define DOCTEST_WARN_THROWS_WITH_AS(expr, message, ...) DOCTEST_ASSERT_THROWS_AS(expr, DT_WARN_THROWS_WITH_AS, message, __VA_ARGS__)
#define DOCTEST_CHECK_THROWS_WITH_AS(expr, message, ...) DOCTEST_ASSERT_THROWS_AS(expr, DT_CHECK_THROWS_WITH_AS, message, __VA_ARGS__)
#define DOCTEST_REQUIRE_THROWS_WITH_AS(expr, message, ...) DOCTEST_ASSERT_THROWS_AS(expr, DT_REQUIRE_THROWS_WITH_AS, message, __VA_ARGS__)

#define DOCTEST_WARN_NOTHROW(expr) DOCTEST_ASSERT_NOTHROW(expr, DT_WARN_NOTHROW)
#define DOCTEST_CHECK_NOTHROW(expr) DOCTEST_ASSERT_NOTHROW(expr, DT_CHECK_NOTHROW)
#define DOCTEST_REQUIRE_NOTHROW(expr) DOCTEST_ASSERT_NOTHROW(expr, DT_REQUIRE_NOTHROW)

#define DOCTEST_WARN_THROWS_MESSAGE(expr, msg) do { DOCTEST_INFO(msg); DOCTEST_WARN_THROWS(expr); } while((void)0, 0)
#define DOCTEST_CHECK_THROWS_MESSAGE(expr, msg) do { DOCTEST_INFO(msg); DOCTEST_CHECK_THROWS(expr); } while((void)0, 0)
#define DOCTEST_REQUIRE_THROWS_MESSAGE(expr, msg) do { DOCTEST_INFO(msg); DOCTEST_REQUIRE_THROWS(expr); } while((void)0, 0)
#define DOCTEST_WARN_THROWS_AS_MESSAGE(expr, ex, msg) do { DOCTEST_INFO(msg); DOCTEST_WARN_THROWS_AS(expr, ex); } while((void)0, 0)
#define DOCTEST_CHECK_THROWS_AS_MESSAGE(expr, ex, msg) do { DOCTEST_INFO(msg); DOCTEST_CHECK_THROWS_AS(expr, ex); } while((void)0, 0)
#define DOCTEST_REQUIRE_THROWS_AS_MESSAGE(expr, ex, msg) do { DOCTEST_INFO(msg); DOCTEST_REQUIRE_THROWS_AS(expr, ex); } while((void)0, 0)
#define DOCTEST_WARN_THROWS_WITH_MESSAGE(expr, with, msg) do { DOCTEST_INFO(msg); DOCTEST_WARN_THROWS_WITH(expr, with); } while((void)0, 0)
#define DOCTEST_CHECK_THROWS_WITH_MESSAGE(expr, with, msg) do { DOCTEST_INFO(msg); DOCTEST_CHECK_THROWS_WITH(expr, with); } while((void)0, 0)
#define DOCTEST_REQUIRE_THROWS_WITH_MESSAGE(expr, with, msg) do { DOCTEST_INFO(msg); DOCTEST_REQUIRE_THROWS_WITH(expr, with); } while((void)0, 0)
#define DOCTEST_WARN_THROWS_WITH_AS_MESSAGE(expr, with, ex, msg) do { DOCTEST_INFO(msg); DOCTEST_WARN_THROWS_WITH_AS(expr, with, ex); } while((void)0, 0)
#define DOCTEST_CHECK_THROWS_WITH_AS_MESSAGE(expr, with, ex, msg) do { DOCTEST_INFO(msg); DOCTEST_CHECK_THROWS_WITH_AS(expr, with, ex); } while((void)0, 0)
#define DOCTEST_REQUIRE_THROWS_WITH_AS_MESSAGE(expr, with, ex, msg) do { DOCTEST_INFO(msg); DOCTEST_REQUIRE_THROWS_WITH_AS(expr, with, ex); } while((void)0, 0)
#define DOCTEST_WARN_NOTHROW_MESSAGE(expr, msg) do { DOCTEST_INFO(msg); DOCTEST_WARN_NOTHROW(expr); } while((void)0, 0)
#define DOCTEST_CHECK_NOTHROW_MESSAGE(expr, msg) do { DOCTEST_INFO(msg); DOCTEST_CHECK_NOTHROW(expr); } while((void)0, 0)
#define DOCTEST_REQUIRE_NOTHROW_MESSAGE(expr, msg) do { DOCTEST_INFO(msg); DOCTEST_REQUIRE_NOTHROW(expr); } while((void)0, 0)
// clang-format on

#ifndef DOCTEST_CONFIG_SUPER_FAST_ASSERTS

#define DOCTEST_BINARY_ASSERT(assert_type, comp, ...)                                              \
    do {                                                                                           \
        doctest::detail::ResultBuilder _DOCTEST_RB(doctest::assertType::assert_type, __FILE__,     \
                                                   __LINE__, #__VA_ARGS__);                        \
        DOCTEST_WRAP_IN_TRY(                                                                       \
                _DOCTEST_RB.binary_assert<doctest::detail::binaryAssertComparison::comp>(          \
                        __VA_ARGS__))                                                              \
        DOCTEST_ASSERT_LOG_AND_REACT(_DOCTEST_RB);                                                 \
    } while((void)0, 0)

#define DOCTEST_UNARY_ASSERT(assert_type, ...)                                                     \
    do {                                                                                           \
        doctest::detail::ResultBuilder _DOCTEST_RB(doctest::assertType::assert_type, __FILE__,     \
                                                   __LINE__, #__VA_ARGS__);                        \
        DOCTEST_WRAP_IN_TRY(_DOCTEST_RB.unary_assert(__VA_ARGS__))                                 \
        DOCTEST_ASSERT_LOG_AND_REACT(_DOCTEST_RB);                                                 \
    } while((void)0, 0)

#else // DOCTEST_CONFIG_SUPER_FAST_ASSERTS

#define DOCTEST_BINARY_ASSERT(assert_type, comparison, ...)                                        \
    doctest::detail::binary_assert<doctest::detail::binaryAssertComparison::comparison>(           \
            doctest::assertType::assert_type, __FILE__, __LINE__, #__VA_ARGS__, __VA_ARGS__)

#define DOCTEST_UNARY_ASSERT(assert_type, ...)                                                     \
    doctest::detail::unary_assert(doctest::assertType::assert_type, __FILE__, __LINE__,            \
                                  #__VA_ARGS__, __VA_ARGS__)

#endif // DOCTEST_CONFIG_SUPER_FAST_ASSERTS

#define DOCTEST_WARN_EQ(...) DOCTEST_BINARY_ASSERT(DT_WARN_EQ, eq, __VA_ARGS__)
#define DOCTEST_CHECK_EQ(...) DOCTEST_BINARY_ASSERT(DT_CHECK_EQ, eq, __VA_ARGS__)
#define DOCTEST_REQUIRE_EQ(...) DOCTEST_BINARY_ASSERT(DT_REQUIRE_EQ, eq, __VA_ARGS__)
#define DOCTEST_WARN_NE(...) DOCTEST_BINARY_ASSERT(DT_WARN_NE, ne, __VA_ARGS__)
#define DOCTEST_CHECK_NE(...) DOCTEST_BINARY_ASSERT(DT_CHECK_NE, ne, __VA_ARGS__)
#define DOCTEST_REQUIRE_NE(...) DOCTEST_BINARY_ASSERT(DT_REQUIRE_NE, ne, __VA_ARGS__)
#define DOCTEST_WARN_GT(...) DOCTEST_BINARY_ASSERT(DT_WARN_GT, gt, __VA_ARGS__)
#define DOCTEST_CHECK_GT(...) DOCTEST_BINARY_ASSERT(DT_CHECK_GT, gt, __VA_ARGS__)
#define DOCTEST_REQUIRE_GT(...) DOCTEST_BINARY_ASSERT(DT_REQUIRE_GT, gt, __VA_ARGS__)
#define DOCTEST_WARN_LT(...) DOCTEST_BINARY_ASSERT(DT_WARN_LT, lt, __VA_ARGS__)
#define DOCTEST_CHECK_LT(...) DOCTEST_BINARY_ASSERT(DT_CHECK_LT, lt, __VA_ARGS__)
#define DOCTEST_REQUIRE_LT(...) DOCTEST_BINARY_ASSERT(DT_REQUIRE_LT, lt, __VA_ARGS__)
#define DOCTEST_WARN_GE(...) DOCTEST_BINARY_ASSERT(DT_WARN_GE, ge, __VA_ARGS__)
#define DOCTEST_CHECK_GE(...) DOCTEST_BINARY_ASSERT(DT_CHECK_GE, ge, __VA_ARGS__)
#define DOCTEST_REQUIRE_GE(...) DOCTEST_BINARY_ASSERT(DT_REQUIRE_GE, ge, __VA_ARGS__)
#define DOCTEST_WARN_LE(...) DOCTEST_BINARY_ASSERT(DT_WARN_LE, le, __VA_ARGS__)
#define DOCTEST_CHECK_LE(...) DOCTEST_BINARY_ASSERT(DT_CHECK_LE, le, __VA_ARGS__)
#define DOCTEST_REQUIRE_LE(...) DOCTEST_BINARY_ASSERT(DT_REQUIRE_LE, le, __VA_ARGS__)

#define DOCTEST_WARN_UNARY(...) DOCTEST_UNARY_ASSERT(DT_WARN_UNARY, __VA_ARGS__)
#define DOCTEST_CHECK_UNARY(...) DOCTEST_UNARY_ASSERT(DT_CHECK_UNARY, __VA_ARGS__)
#define DOCTEST_REQUIRE_UNARY(...) DOCTEST_UNARY_ASSERT(DT_REQUIRE_UNARY, __VA_ARGS__)
#define DOCTEST_WARN_UNARY_FALSE(...) DOCTEST_UNARY_ASSERT(DT_WARN_UNARY_FALSE, __VA_ARGS__)
#define DOCTEST_CHECK_UNARY_FALSE(...) DOCTEST_UNARY_ASSERT(DT_CHECK_UNARY_FALSE, __VA_ARGS__)
#define DOCTEST_REQUIRE_UNARY_FALSE(...) DOCTEST_UNARY_ASSERT(DT_REQUIRE_UNARY_FALSE, __VA_ARGS__)

#ifdef DOCTEST_CONFIG_NO_EXCEPTIONS

#undef DOCTEST_WARN_THROWS
#undef DOCTEST_CHECK_THROWS
#undef DOCTEST_REQUIRE_THROWS
#undef DOCTEST_WARN_THROWS_AS
#undef DOCTEST_CHECK_THROWS_AS
#undef DOCTEST_REQUIRE_THROWS_AS
#undef DOCTEST_WARN_THROWS_WITH
#undef DOCTEST_CHECK_THROWS_WITH
#undef DOCTEST_REQUIRE_THROWS_WITH
#undef DOCTEST_WARN_THROWS_WITH_AS
#undef DOCTEST_CHECK_THROWS_WITH_AS
#undef DOCTEST_REQUIRE_THROWS_WITH_AS
#undef DOCTEST_WARN_NOTHROW
#undef DOCTEST_CHECK_NOTHROW
#undef DOCTEST_REQUIRE_NOTHROW

#undef DOCTEST_WARN_THROWS_MESSAGE
#undef DOCTEST_CHECK_THROWS_MESSAGE
#undef DOCTEST_REQUIRE_THROWS_MESSAGE
#undef DOCTEST_WARN_THROWS_AS_MESSAGE
#undef DOCTEST_CHECK_THROWS_AS_MESSAGE
#undef DOCTEST_REQUIRE_THROWS_AS_MESSAGE
#undef DOCTEST_WARN_THROWS_WITH_MESSAGE
#undef DOCTEST_CHECK_THROWS_WITH_MESSAGE
#undef DOCTEST_REQUIRE_THROWS_WITH_MESSAGE
#undef DOCTEST_WARN_THROWS_WITH_AS_MESSAGE
#undef DOCTEST_CHECK_THROWS_WITH_AS_MESSAGE
#undef DOCTEST_REQUIRE_THROWS_WITH_AS_MESSAGE
#undef DOCTEST_WARN_NOTHROW_MESSAGE
#undef DOCTEST_CHECK_NOTHROW_MESSAGE
#undef DOCTEST_REQUIRE_NOTHROW_MESSAGE

#ifdef DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS

#define DOCTEST_WARN_THROWS(expr) ((void)0)
#define DOCTEST_CHECK_THROWS(expr) ((void)0)
#define DOCTEST_REQUIRE_THROWS(expr) ((void)0)
#define DOCTEST_WARN_THROWS_AS(expr, ...) ((void)0)
#define DOCTEST_CHECK_THROWS_AS(expr, ...) ((void)0)
#define DOCTEST_REQUIRE_THROWS_AS(expr, ...) ((void)0)
#define DOCTEST_WARN_THROWS_WITH(expr, ...) ((void)0)
#define DOCTEST_CHECK_THROWS_WITH(expr, ...) ((void)0)
#define DOCTEST_REQUIRE_THROWS_WITH(expr, ...) ((void)0)
#define DOCTEST_WARN_THROWS_WITH_AS(expr, with, ...) ((void)0)
#define DOCTEST_CHECK_THROWS_WITH_AS(expr, with, ...) ((void)0)
#define DOCTEST_REQUIRE_THROWS_WITH_AS(expr, with, ...) ((void)0)
#define DOCTEST_WARN_NOTHROW(expr) ((void)0)
#define DOCTEST_CHECK_NOTHROW(expr) ((void)0)
#define DOCTEST_REQUIRE_NOTHROW(expr) ((void)0)

#define DOCTEST_WARN_THROWS_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_CHECK_THROWS_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_REQUIRE_THROWS_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_WARN_THROWS_AS_MESSAGE(expr, ex, msg) ((void)0)
#define DOCTEST_CHECK_THROWS_AS_MESSAGE(expr, ex, msg) ((void)0)
#define DOCTEST_REQUIRE_THROWS_AS_MESSAGE(expr, ex, msg) ((void)0)
#define DOCTEST_WARN_THROWS_WITH_MESSAGE(expr, with, msg) ((void)0)
#define DOCTEST_CHECK_THROWS_WITH_MESSAGE(expr, with, msg) ((void)0)
#define DOCTEST_REQUIRE_THROWS_WITH_MESSAGE(expr, with, msg) ((void)0)
#define DOCTEST_WARN_THROWS_WITH_AS_MESSAGE(expr, with, ex, msg) ((void)0)
#define DOCTEST_CHECK_THROWS_WITH_AS_MESSAGE(expr, with, ex, msg) ((void)0)
#define DOCTEST_REQUIRE_THROWS_WITH_AS_MESSAGE(expr, with, ex, msg) ((void)0)
#define DOCTEST_WARN_NOTHROW_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_CHECK_NOTHROW_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_REQUIRE_NOTHROW_MESSAGE(expr, msg) ((void)0)

#else // DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS

#undef DOCTEST_REQUIRE
#undef DOCTEST_REQUIRE_FALSE
#undef DOCTEST_REQUIRE_MESSAGE
#undef DOCTEST_REQUIRE_FALSE_MESSAGE
#undef DOCTEST_REQUIRE_EQ
#undef DOCTEST_REQUIRE_NE
#undef DOCTEST_REQUIRE_GT
#undef DOCTEST_REQUIRE_LT
#undef DOCTEST_REQUIRE_GE
#undef DOCTEST_REQUIRE_LE
#undef DOCTEST_REQUIRE_UNARY
#undef DOCTEST_REQUIRE_UNARY_FALSE

#endif // DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS

#endif // DOCTEST_CONFIG_NO_EXCEPTIONS

// =================================================================================================
// == WHAT FOLLOWS IS VERSIONS OF THE MACROS THAT DO NOT DO ANY REGISTERING!                      ==
// == THIS CAN BE ENABLED BY DEFINING DOCTEST_CONFIG_DISABLE GLOBALLY!                            ==
// =================================================================================================
#else // DOCTEST_CONFIG_DISABLE

#define DOCTEST_IMPLEMENT_FIXTURE(der, base, func, name)                                           \
    namespace {                                                                                    \
        template <typename DOCTEST_UNUSED_TEMPLATE_TYPE>                                           \
        struct der : public base                                                                   \
        { void f(); };                                                                             \
    }                                                                                              \
    template <typename DOCTEST_UNUSED_TEMPLATE_TYPE>                                               \
    inline void der<DOCTEST_UNUSED_TEMPLATE_TYPE>::f()

#define DOCTEST_CREATE_AND_REGISTER_FUNCTION(f, name)                                              \
    template <typename DOCTEST_UNUSED_TEMPLATE_TYPE>                                               \
    static inline void f()

// for registering tests
#define DOCTEST_TEST_CASE(name)                                                                    \
    DOCTEST_CREATE_AND_REGISTER_FUNCTION(DOCTEST_ANONYMOUS(_DOCTEST_ANON_FUNC_), name)

// for registering tests in classes
#define DOCTEST_TEST_CASE_CLASS(name)                                                              \
    DOCTEST_CREATE_AND_REGISTER_FUNCTION(DOCTEST_ANONYMOUS(_DOCTEST_ANON_FUNC_), name)

// for registering tests with a fixture
#define DOCTEST_TEST_CASE_FIXTURE(x, name)                                                         \
    DOCTEST_IMPLEMENT_FIXTURE(DOCTEST_ANONYMOUS(_DOCTEST_ANON_CLASS_), x,                          \
                              DOCTEST_ANONYMOUS(_DOCTEST_ANON_FUNC_), name)

// for converting types to strings without the <typeinfo> header and demangling
#define DOCTEST_TYPE_TO_STRING(...) typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)
#define DOCTEST_TYPE_TO_STRING_IMPL(...)

// for typed tests
#define DOCTEST_TEST_CASE_TEMPLATE(name, type, ...)                                                \
    template <typename type>                                                                       \
    inline void DOCTEST_ANONYMOUS(_DOCTEST_ANON_TMP_)()

#define DOCTEST_TEST_CASE_TEMPLATE_DEFINE(name, type, id)                                          \
    template <typename type>                                                                       \
    inline void DOCTEST_ANONYMOUS(_DOCTEST_ANON_TMP_)()

#define DOCTEST_TEST_CASE_TEMPLATE_INVOKE(id, ...)                                                 \
    typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

#define DOCTEST_TEST_CASE_TEMPLATE_APPLY(id, ...)                                                  \
    typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

// for subcases
#define DOCTEST_SUBCASE(name)

// for a testsuite block
#define DOCTEST_TEST_SUITE(name) namespace

// for starting a testsuite block
#define DOCTEST_TEST_SUITE_BEGIN(name) typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

// for ending a testsuite block
#define DOCTEST_TEST_SUITE_END typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

#define DOCTEST_REGISTER_EXCEPTION_TRANSLATOR(signature)                                           \
    template <typename DOCTEST_UNUSED_TEMPLATE_TYPE>                                               \
    static inline doctest::String DOCTEST_ANONYMOUS(_DOCTEST_ANON_TRANSLATOR_)(signature)

#define DOCTEST_REGISTER_REPORTER(name, priority, reporter)
#define DOCTEST_REGISTER_LISTENER(name, priority, reporter)

#define DOCTEST_INFO(x) ((void)0)
#define DOCTEST_CAPTURE(x) ((void)0)
#define DOCTEST_ADD_MESSAGE_AT(file, line, x) ((void)0)
#define DOCTEST_ADD_FAIL_CHECK_AT(file, line, x) ((void)0)
#define DOCTEST_ADD_FAIL_AT(file, line, x) ((void)0)
#define DOCTEST_MESSAGE(x) ((void)0)
#define DOCTEST_FAIL_CHECK(x) ((void)0)
#define DOCTEST_FAIL(x) ((void)0)

#define DOCTEST_WARN(...) ((void)0)
#define DOCTEST_CHECK(...) ((void)0)
#define DOCTEST_REQUIRE(...) ((void)0)
#define DOCTEST_WARN_FALSE(...) ((void)0)
#define DOCTEST_CHECK_FALSE(...) ((void)0)
#define DOCTEST_REQUIRE_FALSE(...) ((void)0)

#define DOCTEST_WARN_MESSAGE(cond, msg) ((void)0)
#define DOCTEST_CHECK_MESSAGE(cond, msg) ((void)0)
#define DOCTEST_REQUIRE_MESSAGE(cond, msg) ((void)0)
#define DOCTEST_WARN_FALSE_MESSAGE(cond, msg) ((void)0)
#define DOCTEST_CHECK_FALSE_MESSAGE(cond, msg) ((void)0)
#define DOCTEST_REQUIRE_FALSE_MESSAGE(cond, msg) ((void)0)

#define DOCTEST_WARN_THROWS(expr) ((void)0)
#define DOCTEST_CHECK_THROWS(expr) ((void)0)
#define DOCTEST_REQUIRE_THROWS(expr) ((void)0)
#define DOCTEST_WARN_THROWS_AS(expr, ...) ((void)0)
#define DOCTEST_CHECK_THROWS_AS(expr, ...) ((void)0)
#define DOCTEST_REQUIRE_THROWS_AS(expr, ...) ((void)0)
#define DOCTEST_WARN_THROWS_WITH(expr, ...) ((void)0)
#define DOCTEST_CHECK_THROWS_WITH(expr, ...) ((void)0)
#define DOCTEST_REQUIRE_THROWS_WITH(expr, ...) ((void)0)
#define DOCTEST_WARN_THROWS_WITH_AS(expr, with, ...) ((void)0)
#define DOCTEST_CHECK_THROWS_WITH_AS(expr, with, ...) ((void)0)
#define DOCTEST_REQUIRE_THROWS_WITH_AS(expr, with, ...) ((void)0)
#define DOCTEST_WARN_NOTHROW(expr) ((void)0)
#define DOCTEST_CHECK_NOTHROW(expr) ((void)0)
#define DOCTEST_REQUIRE_NOTHROW(expr) ((void)0)

#define DOCTEST_WARN_THROWS_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_CHECK_THROWS_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_REQUIRE_THROWS_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_WARN_THROWS_AS_MESSAGE(expr, ex, msg) ((void)0)
#define DOCTEST_CHECK_THROWS_AS_MESSAGE(expr, ex, msg) ((void)0)
#define DOCTEST_REQUIRE_THROWS_AS_MESSAGE(expr, ex, msg) ((void)0)
#define DOCTEST_WARN_THROWS_WITH_MESSAGE(expr, with, msg) ((void)0)
#define DOCTEST_CHECK_THROWS_WITH_MESSAGE(expr, with, msg) ((void)0)
#define DOCTEST_REQUIRE_THROWS_WITH_MESSAGE(expr, with, msg) ((void)0)
#define DOCTEST_WARN_THROWS_WITH_AS_MESSAGE(expr, with, ex, msg) ((void)0)
#define DOCTEST_CHECK_THROWS_WITH_AS_MESSAGE(expr, with, ex, msg) ((void)0)
#define DOCTEST_REQUIRE_THROWS_WITH_AS_MESSAGE(expr, with, ex, msg) ((void)0)
#define DOCTEST_WARN_NOTHROW_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_CHECK_NOTHROW_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_REQUIRE_NOTHROW_MESSAGE(expr, msg) ((void)0)

#define DOCTEST_WARN_EQ(...) ((void)0)
#define DOCTEST_CHECK_EQ(...) ((void)0)
#define DOCTEST_REQUIRE_EQ(...) ((void)0)
#define DOCTEST_WARN_NE(...) ((void)0)
#define DOCTEST_CHECK_NE(...) ((void)0)
#define DOCTEST_REQUIRE_NE(...) ((void)0)
#define DOCTEST_WARN_GT(...) ((void)0)
#define DOCTEST_CHECK_GT(...) ((void)0)
#define DOCTEST_REQUIRE_GT(...) ((void)0)
#define DOCTEST_WARN_LT(...) ((void)0)
#define DOCTEST_CHECK_LT(...) ((void)0)
#define DOCTEST_REQUIRE_LT(...) ((void)0)
#define DOCTEST_WARN_GE(...) ((void)0)
#define DOCTEST_CHECK_GE(...) ((void)0)
#define DOCTEST_REQUIRE_GE(...) ((void)0)
#define DOCTEST_WARN_LE(...) ((void)0)
#define DOCTEST_CHECK_LE(...) ((void)0)
#define DOCTEST_REQUIRE_LE(...) ((void)0)

#define DOCTEST_WARN_UNARY(...) ((void)0)
#define DOCTEST_CHECK_UNARY(...) ((void)0)
#define DOCTEST_REQUIRE_UNARY(...) ((void)0)
#define DOCTEST_WARN_UNARY_FALSE(...) ((void)0)
#define DOCTEST_CHECK_UNARY_FALSE(...) ((void)0)
#define DOCTEST_REQUIRE_UNARY_FALSE(...) ((void)0)

#endif // DOCTEST_CONFIG_DISABLE

// clang-format off
// KEPT FOR BACKWARDS COMPATIBILITY - FORWARDING TO THE RIGHT MACROS
#define DOCTEST_FAST_WARN_EQ             DOCTEST_WARN_EQ
#define DOCTEST_FAST_CHECK_EQ            DOCTEST_CHECK_EQ
#define DOCTEST_FAST_REQUIRE_EQ          DOCTEST_REQUIRE_EQ
#define DOCTEST_FAST_WARN_NE             DOCTEST_WARN_NE
#define DOCTEST_FAST_CHECK_NE            DOCTEST_CHECK_NE
#define DOCTEST_FAST_REQUIRE_NE          DOCTEST_REQUIRE_NE
#define DOCTEST_FAST_WARN_GT             DOCTEST_WARN_GT
#define DOCTEST_FAST_CHECK_GT            DOCTEST_CHECK_GT
#define DOCTEST_FAST_REQUIRE_GT          DOCTEST_REQUIRE_GT
#define DOCTEST_FAST_WARN_LT             DOCTEST_WARN_LT
#define DOCTEST_FAST_CHECK_LT            DOCTEST_CHECK_LT
#define DOCTEST_FAST_REQUIRE_LT          DOCTEST_REQUIRE_LT
#define DOCTEST_FAST_WARN_GE             DOCTEST_WARN_GE
#define DOCTEST_FAST_CHECK_GE            DOCTEST_CHECK_GE
#define DOCTEST_FAST_REQUIRE_GE          DOCTEST_REQUIRE_GE
#define DOCTEST_FAST_WARN_LE             DOCTEST_WARN_LE
#define DOCTEST_FAST_CHECK_LE            DOCTEST_CHECK_LE
#define DOCTEST_FAST_REQUIRE_LE          DOCTEST_REQUIRE_LE

#define DOCTEST_FAST_WARN_UNARY          DOCTEST_WARN_UNARY
#define DOCTEST_FAST_CHECK_UNARY         DOCTEST_CHECK_UNARY
#define DOCTEST_FAST_REQUIRE_UNARY       DOCTEST_REQUIRE_UNARY
#define DOCTEST_FAST_WARN_UNARY_FALSE    DOCTEST_WARN_UNARY_FALSE
#define DOCTEST_FAST_CHECK_UNARY_FALSE   DOCTEST_CHECK_UNARY_FALSE
#define DOCTEST_FAST_REQUIRE_UNARY_FALSE DOCTEST_REQUIRE_UNARY_FALSE

#define DOCTEST_TEST_CASE_TEMPLATE_INSTANTIATE DOCTEST_TEST_CASE_TEMPLATE_INVOKE
// clang-format on

// BDD style macros
// clang-format off
#define DOCTEST_SCENARIO(name) DOCTEST_TEST_CASE("  Scenario: " name)
#define DOCTEST_SCENARIO_CLASS(name) DOCTEST_TEST_CASE_CLASS("  Scenario: " name)
#define DOCTEST_SCENARIO_TEMPLATE(name, T, ...)  DOCTEST_TEST_CASE_TEMPLATE("  Scenario: " name, T, __VA_ARGS__)
#define DOCTEST_SCENARIO_TEMPLATE_DEFINE(name, T, id) DOCTEST_TEST_CASE_TEMPLATE_DEFINE("  Scenario: " name, T, id)

#define DOCTEST_GIVEN(name)     DOCTEST_SUBCASE("   Given: " name)
#define DOCTEST_WHEN(name)      DOCTEST_SUBCASE("    When: " name)
#define DOCTEST_AND_WHEN(name)  DOCTEST_SUBCASE("And when: " name)
#define DOCTEST_THEN(name)      DOCTEST_SUBCASE("    Then: " name)
#define DOCTEST_AND_THEN(name)  DOCTEST_SUBCASE("     And: " name)
// clang-format on

// == SHORT VERSIONS OF THE MACROS
#if !defined(DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES)

#define TEST_CASE DOCTEST_TEST_CASE
#define TEST_CASE_CLASS DOCTEST_TEST_CASE_CLASS
#define TEST_CASE_FIXTURE DOCTEST_TEST_CASE_FIXTURE
#define TYPE_TO_STRING DOCTEST_TYPE_TO_STRING
#define TEST_CASE_TEMPLATE DOCTEST_TEST_CASE_TEMPLATE
#define TEST_CASE_TEMPLATE_DEFINE DOCTEST_TEST_CASE_TEMPLATE_DEFINE
#define TEST_CASE_TEMPLATE_INVOKE DOCTEST_TEST_CASE_TEMPLATE_INVOKE
#define TEST_CASE_TEMPLATE_APPLY DOCTEST_TEST_CASE_TEMPLATE_APPLY
#define SUBCASE DOCTEST_SUBCASE
#define TEST_SUITE DOCTEST_TEST_SUITE
#define TEST_SUITE_BEGIN DOCTEST_TEST_SUITE_BEGIN
#define TEST_SUITE_END DOCTEST_TEST_SUITE_END
#define REGISTER_EXCEPTION_TRANSLATOR DOCTEST_REGISTER_EXCEPTION_TRANSLATOR
#define REGISTER_REPORTER DOCTEST_REGISTER_REPORTER
#define REGISTER_LISTENER DOCTEST_REGISTER_LISTENER
#define INFO DOCTEST_INFO
#define CAPTURE DOCTEST_CAPTURE
#define ADD_MESSAGE_AT DOCTEST_ADD_MESSAGE_AT
#define ADD_FAIL_CHECK_AT DOCTEST_ADD_FAIL_CHECK_AT
#define ADD_FAIL_AT DOCTEST_ADD_FAIL_AT
#define MESSAGE DOCTEST_MESSAGE
#define FAIL_CHECK DOCTEST_FAIL_CHECK
#define FAIL DOCTEST_FAIL
#define TO_LVALUE DOCTEST_TO_LVALUE

#define WARN DOCTEST_WARN
#define WARN_FALSE DOCTEST_WARN_FALSE
#define WARN_THROWS DOCTEST_WARN_THROWS
#define WARN_THROWS_AS DOCTEST_WARN_THROWS_AS
#define WARN_THROWS_WITH DOCTEST_WARN_THROWS_WITH
#define WARN_THROWS_WITH_AS DOCTEST_WARN_THROWS_WITH_AS
#define WARN_NOTHROW DOCTEST_WARN_NOTHROW
#define CHECK DOCTEST_CHECK
#define CHECK_FALSE DOCTEST_CHECK_FALSE
#define CHECK_THROWS DOCTEST_CHECK_THROWS
#define CHECK_THROWS_AS DOCTEST_CHECK_THROWS_AS
#define CHECK_THROWS_WITH DOCTEST_CHECK_THROWS_WITH
#define CHECK_THROWS_WITH_AS DOCTEST_CHECK_THROWS_WITH_AS
#define CHECK_NOTHROW DOCTEST_CHECK_NOTHROW
#define REQUIRE DOCTEST_REQUIRE
#define REQUIRE_FALSE DOCTEST_REQUIRE_FALSE
#define REQUIRE_THROWS DOCTEST_REQUIRE_THROWS
#define REQUIRE_THROWS_AS DOCTEST_REQUIRE_THROWS_AS
#define REQUIRE_THROWS_WITH DOCTEST_REQUIRE_THROWS_WITH
#define REQUIRE_THROWS_WITH_AS DOCTEST_REQUIRE_THROWS_WITH_AS
#define REQUIRE_NOTHROW DOCTEST_REQUIRE_NOTHROW

#define WARN_MESSAGE DOCTEST_WARN_MESSAGE
#define WARN_FALSE_MESSAGE DOCTEST_WARN_FALSE_MESSAGE
#define WARN_THROWS_MESSAGE DOCTEST_WARN_THROWS_MESSAGE
#define WARN_THROWS_AS_MESSAGE DOCTEST_WARN_THROWS_AS_MESSAGE
#define WARN_THROWS_WITH_MESSAGE DOCTEST_WARN_THROWS_WITH_MESSAGE
#define WARN_THROWS_WITH_AS_MESSAGE DOCTEST_WARN_THROWS_WITH_AS_MESSAGE
#define WARN_NOTHROW_MESSAGE DOCTEST_WARN_NOTHROW_MESSAGE
#define CHECK_MESSAGE DOCTEST_CHECK_MESSAGE
#define CHECK_FALSE_MESSAGE DOCTEST_CHECK_FALSE_MESSAGE
#define CHECK_THROWS_MESSAGE DOCTEST_CHECK_THROWS_MESSAGE
#define CHECK_THROWS_AS_MESSAGE DOCTEST_CHECK_THROWS_AS_MESSAGE
#define CHECK_THROWS_WITH_MESSAGE DOCTEST_CHECK_THROWS_WITH_MESSAGE
#define CHECK_THROWS_WITH_AS_MESSAGE DOCTEST_CHECK_THROWS_WITH_AS_MESSAGE
#define CHECK_NOTHROW_MESSAGE DOCTEST_CHECK_NOTHROW_MESSAGE
#define REQUIRE_MESSAGE DOCTEST_REQUIRE_MESSAGE
#define REQUIRE_FALSE_MESSAGE DOCTEST_REQUIRE_FALSE_MESSAGE
#define REQUIRE_THROWS_MESSAGE DOCTEST_REQUIRE_THROWS_MESSAGE
#define REQUIRE_THROWS_AS_MESSAGE DOCTEST_REQUIRE_THROWS_AS_MESSAGE
#define REQUIRE_THROWS_WITH_MESSAGE DOCTEST_REQUIRE_THROWS_WITH_MESSAGE
#define REQUIRE_THROWS_WITH_AS_MESSAGE DOCTEST_REQUIRE_THROWS_WITH_AS_MESSAGE
#define REQUIRE_NOTHROW_MESSAGE DOCTEST_REQUIRE_NOTHROW_MESSAGE

#define SCENARIO DOCTEST_SCENARIO
#define SCENARIO_CLASS DOCTEST_SCENARIO_CLASS
#define SCENARIO_TEMPLATE DOCTEST_SCENARIO_TEMPLATE
#define SCENARIO_TEMPLATE_DEFINE DOCTEST_SCENARIO_TEMPLATE_DEFINE
#define GIVEN DOCTEST_GIVEN
#define WHEN DOCTEST_WHEN
#define AND_WHEN DOCTEST_AND_WHEN
#define THEN DOCTEST_THEN
#define AND_THEN DOCTEST_AND_THEN

#define WARN_EQ DOCTEST_WARN_EQ
#define CHECK_EQ DOCTEST_CHECK_EQ
#define REQUIRE_EQ DOCTEST_REQUIRE_EQ
#define WARN_NE DOCTEST_WARN_NE
#define CHECK_NE DOCTEST_CHECK_NE
#define REQUIRE_NE DOCTEST_REQUIRE_NE
#define WARN_GT DOCTEST_WARN_GT
#define CHECK_GT DOCTEST_CHECK_GT
#define REQUIRE_GT DOCTEST_REQUIRE_GT
#define WARN_LT DOCTEST_WARN_LT
#define CHECK_LT DOCTEST_CHECK_LT
#define REQUIRE_LT DOCTEST_REQUIRE_LT
#define WARN_GE DOCTEST_WARN_GE
#define CHECK_GE DOCTEST_CHECK_GE
#define REQUIRE_GE DOCTEST_REQUIRE_GE
#define WARN_LE DOCTEST_WARN_LE
#define CHECK_LE DOCTEST_CHECK_LE
#define REQUIRE_LE DOCTEST_REQUIRE_LE
#define WARN_UNARY DOCTEST_WARN_UNARY
#define CHECK_UNARY DOCTEST_CHECK_UNARY
#define REQUIRE_UNARY DOCTEST_REQUIRE_UNARY
#define WARN_UNARY_FALSE DOCTEST_WARN_UNARY_FALSE
#define CHECK_UNARY_FALSE DOCTEST_CHECK_UNARY_FALSE
#define REQUIRE_UNARY_FALSE DOCTEST_REQUIRE_UNARY_FALSE

// KEPT FOR BACKWARDS COMPATIBILITY
#define FAST_WARN_EQ DOCTEST_FAST_WARN_EQ
#define FAST_CHECK_EQ DOCTEST_FAST_CHECK_EQ
#define FAST_REQUIRE_EQ DOCTEST_FAST_REQUIRE_EQ
#define FAST_WARN_NE DOCTEST_FAST_WARN_NE
#define FAST_CHECK_NE DOCTEST_FAST_CHECK_NE
#define FAST_REQUIRE_NE DOCTEST_FAST_REQUIRE_NE
#define FAST_WARN_GT DOCTEST_FAST_WARN_GT
#define FAST_CHECK_GT DOCTEST_FAST_CHECK_GT
#define FAST_REQUIRE_GT DOCTEST_FAST_REQUIRE_GT
#define FAST_WARN_LT DOCTEST_FAST_WARN_LT
#define FAST_CHECK_LT DOCTEST_FAST_CHECK_LT
#define FAST_REQUIRE_LT DOCTEST_FAST_REQUIRE_LT
#define FAST_WARN_GE DOCTEST_FAST_WARN_GE
#define FAST_CHECK_GE DOCTEST_FAST_CHECK_GE
#define FAST_REQUIRE_GE DOCTEST_FAST_REQUIRE_GE
#define FAST_WARN_LE DOCTEST_FAST_WARN_LE
#define FAST_CHECK_LE DOCTEST_FAST_CHECK_LE
#define FAST_REQUIRE_LE DOCTEST_FAST_REQUIRE_LE

#define FAST_WARN_UNARY DOCTEST_FAST_WARN_UNARY
#define FAST_CHECK_UNARY DOCTEST_FAST_CHECK_UNARY
#define FAST_REQUIRE_UNARY DOCTEST_FAST_REQUIRE_UNARY
#define FAST_WARN_UNARY_FALSE DOCTEST_FAST_WARN_UNARY_FALSE
#define FAST_CHECK_UNARY_FALSE DOCTEST_FAST_CHECK_UNARY_FALSE
#define FAST_REQUIRE_UNARY_FALSE DOCTEST_FAST_REQUIRE_UNARY_FALSE

#define TEST_CASE_TEMPLATE_INSTANTIATE DOCTEST_TEST_CASE_TEMPLATE_INSTANTIATE

#endif // DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES

#if !defined(DOCTEST_CONFIG_DISABLE)

// this is here to clear the 'current test suite' for the current translation unit - at the top
DOCTEST_TEST_SUITE_END();

// add stringification for primitive/fundamental types
namespace doctest { namespace detail {
    DOCTEST_TYPE_TO_STRING_IMPL(bool)
    DOCTEST_TYPE_TO_STRING_IMPL(float)
    DOCTEST_TYPE_TO_STRING_IMPL(double)
    DOCTEST_TYPE_TO_STRING_IMPL(long double)
    DOCTEST_TYPE_TO_STRING_IMPL(char)
    DOCTEST_TYPE_TO_STRING_IMPL(signed char)
    DOCTEST_TYPE_TO_STRING_IMPL(unsigned char)
#if !DOCTEST_MSVC || defined(_NATIVE_WCHAR_T_DEFINED)
    DOCTEST_TYPE_TO_STRING_IMPL(wchar_t)
#endif // not MSVC or wchar_t support enabled
    DOCTEST_TYPE_TO_STRING_IMPL(short int)
    DOCTEST_TYPE_TO_STRING_IMPL(unsigned short int)
    DOCTEST_TYPE_TO_STRING_IMPL(int)
    DOCTEST_TYPE_TO_STRING_IMPL(unsigned int)
    DOCTEST_TYPE_TO_STRING_IMPL(long int)
    DOCTEST_TYPE_TO_STRING_IMPL(unsigned long int)
    DOCTEST_TYPE_TO_STRING_IMPL(long long int)
    DOCTEST_TYPE_TO_STRING_IMPL(unsigned long long int)
}} // namespace doctest::detail

#endif // DOCTEST_CONFIG_DISABLE

DOCTEST_CLANG_SUPPRESS_WARNING_POP
DOCTEST_MSVC_SUPPRESS_WARNING_POP
DOCTEST_GCC_SUPPRESS_WARNING_POP

#endif // DOCTEST_LIBRARY_INCLUDED

#ifndef DOCTEST_SINGLE_HEADER
#define DOCTEST_SINGLE_HEADER
#endif // DOCTEST_SINGLE_HEADER

#if defined(DOCTEST_CONFIG_IMPLEMENT) || !defined(DOCTEST_SINGLE_HEADER)

#ifndef DOCTEST_SINGLE_HEADER
#include "doctest_fwd.h"
#endif // DOCTEST_SINGLE_HEADER

DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wunused-macros")

#ifndef DOCTEST_LIBRARY_IMPLEMENTATION
#define DOCTEST_LIBRARY_IMPLEMENTATION

DOCTEST_CLANG_SUPPRESS_WARNING_POP

DOCTEST_CLANG_SUPPRESS_WARNING_PUSH
DOCTEST_CLANG_SUPPRESS_WARNING("-Wunknown-pragmas")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wpadded")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wweak-vtables")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wmissing-prototypes")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wsign-conversion")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wshorten-64-to-32")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wmissing-variable-declarations")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wswitch")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wswitch-enum")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wcovered-switch-default")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wmissing-noreturn")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wunused-local-typedef")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wdisabled-macro-expansion")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wmissing-braces")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wmissing-field-initializers")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wc++98-compat")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wc++98-compat-pedantic")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wunused-member-function")

DOCTEST_GCC_SUPPRESS_WARNING_PUSH
DOCTEST_GCC_SUPPRESS_WARNING("-Wunknown-pragmas")
DOCTEST_GCC_SUPPRESS_WARNING("-Wpragmas")
DOCTEST_GCC_SUPPRESS_WARNING("-Wconversion")
DOCTEST_GCC_SUPPRESS_WARNING("-Weffc++")
DOCTEST_GCC_SUPPRESS_WARNING("-Wsign-conversion")
DOCTEST_GCC_SUPPRESS_WARNING("-Wstrict-overflow")
DOCTEST_GCC_SUPPRESS_WARNING("-Wstrict-aliasing")
DOCTEST_GCC_SUPPRESS_WARNING("-Wmissing-field-initializers")
DOCTEST_GCC_SUPPRESS_WARNING("-Wmissing-braces")
DOCTEST_GCC_SUPPRESS_WARNING("-Wmissing-declarations")
DOCTEST_GCC_SUPPRESS_WARNING("-Wswitch")
DOCTEST_GCC_SUPPRESS_WARNING("-Wswitch-enum")
DOCTEST_GCC_SUPPRESS_WARNING("-Wswitch-default")
DOCTEST_GCC_SUPPRESS_WARNING("-Wunsafe-loop-optimizations")
DOCTEST_GCC_SUPPRESS_WARNING("-Wold-style-cast")
DOCTEST_GCC_SUPPRESS_WARNING("-Wunused-local-typedefs")
DOCTEST_GCC_SUPPRESS_WARNING("-Wuseless-cast")
DOCTEST_GCC_SUPPRESS_WARNING("-Wunused-function")
DOCTEST_GCC_SUPPRESS_WARNING("-Wmultiple-inheritance")
DOCTEST_GCC_SUPPRESS_WARNING("-Wnoexcept")
DOCTEST_GCC_SUPPRESS_WARNING("-Wsuggest-attribute")

DOCTEST_MSVC_SUPPRESS_WARNING_PUSH
DOCTEST_MSVC_SUPPRESS_WARNING(4616) // invalid compiler warning
DOCTEST_MSVC_SUPPRESS_WARNING(4619) // invalid compiler warning
DOCTEST_MSVC_SUPPRESS_WARNING(4996) // The compiler encountered a deprecated declaration
DOCTEST_MSVC_SUPPRESS_WARNING(4267) // 'var' : conversion from 'x' to 'y', possible loss of data
DOCTEST_MSVC_SUPPRESS_WARNING(4706) // assignment within conditional expression
DOCTEST_MSVC_SUPPRESS_WARNING(4512) // 'class' : assignment operator could not be generated
DOCTEST_MSVC_SUPPRESS_WARNING(4127) // conditional expression is constant
DOCTEST_MSVC_SUPPRESS_WARNING(4530) // C++ exception handler used, but unwind semantics not enabled
DOCTEST_MSVC_SUPPRESS_WARNING(4577) // 'noexcept' used with no exception handling mode specified
DOCTEST_MSVC_SUPPRESS_WARNING(4774) // format string expected in argument is not a string literal
DOCTEST_MSVC_SUPPRESS_WARNING(4365) // conversion from 'int' to 'unsigned', signed/unsigned mismatch
DOCTEST_MSVC_SUPPRESS_WARNING(4820) // padding in structs
DOCTEST_MSVC_SUPPRESS_WARNING(4640) // construction of local static object is not thread-safe
DOCTEST_MSVC_SUPPRESS_WARNING(5039) // pointer to potentially throwing function passed to extern C
DOCTEST_MSVC_SUPPRESS_WARNING(5045) // Spectre mitigation stuff
DOCTEST_MSVC_SUPPRESS_WARNING(4626) // assignment operator was implicitly defined as deleted
DOCTEST_MSVC_SUPPRESS_WARNING(5027) // move assignment operator was implicitly defined as deleted
DOCTEST_MSVC_SUPPRESS_WARNING(5026) // move constructor was implicitly defined as deleted
DOCTEST_MSVC_SUPPRESS_WARNING(4625) // copy constructor was implicitly defined as deleted
DOCTEST_MSVC_SUPPRESS_WARNING(4800) // forcing value to bool 'true' or 'false' (performance warning)
// static analysis
DOCTEST_MSVC_SUPPRESS_WARNING(26439) // This kind of function may not throw. Declare it 'noexcept'
DOCTEST_MSVC_SUPPRESS_WARNING(26495) // Always initialize a member variable
DOCTEST_MSVC_SUPPRESS_WARNING(26451) // Arithmetic overflow ...
DOCTEST_MSVC_SUPPRESS_WARNING(26444) // Avoid unnamed objects with custom construction and dtor...
DOCTEST_MSVC_SUPPRESS_WARNING(26812) // Prefer 'enum class' over 'enum'

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN

// required includes - will go only in one translation unit!
#include <ctime>
#include <cmath>
#include <climits>
// borland (Embarcadero) compiler requires math.h and not cmath - https://github.com/onqtam/doctest/pull/37
#ifdef __BORLANDC__
#include <math.h>
#endif // __BORLANDC__
#include <new>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <utility>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <atomic>
#include <mutex>
#include <set>
#include <map>
#include <exception>
#include <stdexcept>
#ifdef DOCTEST_CONFIG_POSIX_SIGNALS
#include <csignal>
#endif // DOCTEST_CONFIG_POSIX_SIGNALS
#include <cfloat>
#include <cctype>
#include <cstdint>

#ifdef DOCTEST_PLATFORM_MAC
#include <sys/types.h>
#include <unistd.h>
#include <sys/sysctl.h>
#endif // DOCTEST_PLATFORM_MAC

#ifdef DOCTEST_PLATFORM_WINDOWS

// defines for a leaner windows.h
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

// not sure what AfxWin.h is for - here I do what Catch does
#ifdef __AFXDLL
#include <AfxWin.h>
#else
#include <Windows.h>
#endif
#include <io.h>

#else // DOCTEST_PLATFORM_WINDOWS

#include <sys/time.h>
#include <unistd.h>

#endif // DOCTEST_PLATFORM_WINDOWS

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

// counts the number of elements in a C array
#define DOCTEST_COUNTOF(x) (sizeof(x) / sizeof(x[0]))

#ifdef DOCTEST_CONFIG_DISABLE
#define DOCTEST_BRANCH_ON_DISABLED(if_disabled, if_not_disabled) if_disabled
#else // DOCTEST_CONFIG_DISABLE
#define DOCTEST_BRANCH_ON_DISABLED(if_disabled, if_not_disabled) if_not_disabled
#endif // DOCTEST_CONFIG_DISABLE

#ifndef DOCTEST_CONFIG_OPTIONS_PREFIX
#define DOCTEST_CONFIG_OPTIONS_PREFIX "dt-"
#endif

#ifndef DOCTEST_THREAD_LOCAL
#define DOCTEST_THREAD_LOCAL thread_local
#endif

#ifdef DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
#define DOCTEST_OPTIONS_PREFIX_DISPLAY DOCTEST_CONFIG_OPTIONS_PREFIX
#else
#define DOCTEST_OPTIONS_PREFIX_DISPLAY ""
#endif

namespace doctest {

bool is_running_in_test = false;

namespace {
    using namespace detail;
    // case insensitive strcmp
    int stricmp(const char* a, const char* b) {
        for(;; a++, b++) {
            const int d = tolower(*a) - tolower(*b);
            if(d != 0 || !*a)
                return d;
        }
    }

    template <typename T>
    String fpToString(T value, int precision) {
        std::ostringstream oss;
        oss << std::setprecision(precision) << std::fixed << value;
        std::string d = oss.str();
        size_t      i = d.find_last_not_of('0');
        if(i != std::string::npos && i != d.size() - 1) {
            if(d[i] == '.')
                i++;
            d = d.substr(0, i + 1);
        }
        return d.c_str();
    }

    struct Endianness
    {
        enum Arch
        {
            Big,
            Little
        };

        static Arch which() {
            int x = 1;
            // casting any data pointer to char* is allowed
            auto ptr = reinterpret_cast<char*>(&x);
            if(*ptr)
                return Little;
            return Big;
        }
    };
} // namespace

namespace detail {
    void my_memcpy(void* dest, const void* src, unsigned num) { memcpy(dest, src, num); }

    String rawMemoryToString(const void* object, unsigned size) {
        // Reverse order for little endian architectures
        int i = 0, end = static_cast<int>(size), inc = 1;
        if(Endianness::which() == Endianness::Little) {
            i   = end - 1;
            end = inc = -1;
        }

        unsigned const char* bytes = static_cast<unsigned const char*>(object);
        std::ostringstream   oss;
        oss << "0x" << std::setfill('0') << std::hex;
        for(; i != end; i += inc)
            oss << std::setw(2) << static_cast<unsigned>(bytes[i]);
        return oss.str().c_str();
    }

    DOCTEST_THREAD_LOCAL std::ostringstream g_oss; // NOLINT(cert-err58-cpp)

    std::ostream* getTlsOss() {
        g_oss.clear(); // there shouldn't be anything worth clearing in the flags
        g_oss.str(""); // the slow way of resetting a string stream
        //g_oss.seekp(0); // optimal reset - as seen here: https://stackoverflow.com/a/624291/3162383
        return &g_oss;
    }

    String getTlsOssResult() {
        //g_oss << std::ends; // needed - as shown here: https://stackoverflow.com/a/624291/3162383
        return g_oss.str().c_str();
    }

#ifndef DOCTEST_CONFIG_DISABLE

namespace timer_large_integer
{
    
#if defined(DOCTEST_PLATFORM_WINDOWS)
    typedef ULONGLONG type;
#else // DOCTEST_PLATFORM_WINDOWS
    using namespace std;
    typedef uint64_t type;
#endif // DOCTEST_PLATFORM_WINDOWS
}

typedef timer_large_integer::type ticks_t;

#ifdef DOCTEST_CONFIG_GETCURRENTTICKS
    ticks_t getCurrentTicks() { return DOCTEST_CONFIG_GETCURRENTTICKS(); }
#elif defined(DOCTEST_PLATFORM_WINDOWS)
    ticks_t getCurrentTicks() {
        static LARGE_INTEGER hz = {0}, hzo = {0};
        if(!hz.QuadPart) {
            QueryPerformanceFrequency(&hz);
            QueryPerformanceCounter(&hzo);
        }
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);
        return ((t.QuadPart - hzo.QuadPart) * LONGLONG(1000000)) / hz.QuadPart;
    }
#else  // DOCTEST_PLATFORM_WINDOWS
    ticks_t getCurrentTicks() {
        timeval t;
        gettimeofday(&t, nullptr);
        return static_cast<ticks_t>(t.tv_sec) * 1000000 + static_cast<ticks_t>(t.tv_usec);
    }
#endif // DOCTEST_PLATFORM_WINDOWS

    struct Timer
    {
        void         start() { m_ticks = getCurrentTicks(); }
        unsigned int getElapsedMicroseconds() const {
            return static_cast<unsigned int>(getCurrentTicks() - m_ticks);
        }
        //unsigned int getElapsedMilliseconds() const {
        //    return static_cast<unsigned int>(getElapsedMicroseconds() / 1000);
        //}
        double getElapsedSeconds() const { return (getCurrentTicks() - m_ticks) / 1000000.0; }

    private:
        ticks_t m_ticks = 0;
    };

    // this holds both parameters from the command line and runtime data for tests
    struct ContextState : ContextOptions, TestRunStats, CurrentTestCaseStats
    {
        std::atomic<int> numAssertsCurrentTest_atomic;
        std::atomic<int> numAssertsFailedCurrentTest_atomic;

        std::vector<std::vector<String>> filters = decltype(filters)(9); // 9 different filters

        std::vector<IReporter*> reporters_currently_used;

        const TestCase* currentTest = nullptr;

        assert_handler ah = nullptr;

        Timer timer;

        std::vector<String> stringifiedContexts; // logging from INFO() due to an exception

        // stuff for subcases
        std::vector<SubcaseSignature>     subcasesStack;
        std::set<decltype(subcasesStack)> subcasesPassed;
        int                               subcasesCurrentMaxLevel;
        bool                              should_reenter;
        std::atomic<bool>                 shouldLogCurrentException;

        void resetRunData() {
            numTestCases                = 0;
            numTestCasesPassingFilters  = 0;
            numTestSuitesPassingFilters = 0;
            numTestCasesFailed          = 0;
            numAsserts                  = 0;
            numAssertsFailed            = 0;
            numAssertsCurrentTest       = 0;
            numAssertsFailedCurrentTest = 0;
        }

        void finalizeTestCaseData() {
            seconds = timer.getElapsedSeconds();

            // update the non-atomic counters
            numAsserts += numAssertsCurrentTest_atomic;
            numAssertsFailed += numAssertsFailedCurrentTest_atomic;
            numAssertsCurrentTest       = numAssertsCurrentTest_atomic;
            numAssertsFailedCurrentTest = numAssertsFailedCurrentTest_atomic;

            if(numAssertsFailedCurrentTest)
                failure_flags |= TestCaseFailureReason::AssertFailure;

            if(Approx(currentTest->m_timeout).epsilon(DBL_EPSILON) != 0 &&
               Approx(seconds).epsilon(DBL_EPSILON) > currentTest->m_timeout)
                failure_flags |= TestCaseFailureReason::Timeout;

            if(currentTest->m_should_fail) {
                if(failure_flags) {
                    failure_flags |= TestCaseFailureReason::ShouldHaveFailedAndDid;
                } else {
                    failure_flags |= TestCaseFailureReason::ShouldHaveFailedButDidnt;
                }
            } else if(failure_flags && currentTest->m_may_fail) {
                failure_flags |= TestCaseFailureReason::CouldHaveFailedAndDid;
            } else if(currentTest->m_expected_failures > 0) {
                if(numAssertsFailedCurrentTest == currentTest->m_expected_failures) {
                    failure_flags |= TestCaseFailureReason::FailedExactlyNumTimes;
                } else {
                    failure_flags |= TestCaseFailureReason::DidntFailExactlyNumTimes;
                }
            }

            bool ok_to_fail = (TestCaseFailureReason::ShouldHaveFailedAndDid & failure_flags) ||
                              (TestCaseFailureReason::CouldHaveFailedAndDid & failure_flags) ||
                              (TestCaseFailureReason::FailedExactlyNumTimes & failure_flags);

            // if any subcase has failed - the whole test case has failed
            if(failure_flags && !ok_to_fail)
                numTestCasesFailed++;
        }
    };

    ContextState* g_cs = nullptr;

    // used to avoid locks for the debug output
    // TODO: figure out if this is indeed necessary/correct - seems like either there still
    // could be a race or that there wouldn't be a race even if using the context directly
    DOCTEST_THREAD_LOCAL bool g_no_colors;

#endif // DOCTEST_CONFIG_DISABLE
} // namespace detail

void String::setOnHeap() { *reinterpret_cast<unsigned char*>(&buf[last]) = 128; }
void String::setLast(unsigned in) { buf[last] = char(in); }

void String::copy(const String& other) {
    using namespace std;
    if(other.isOnStack()) {
        memcpy(buf, other.buf, len);
    } else {
        setOnHeap();
        data.size     = other.data.size;
        data.capacity = data.size + 1;
        data.ptr      = new char[data.capacity];
        memcpy(data.ptr, other.data.ptr, data.size + 1);
    }
}

String::String() {
    buf[0] = '\0';
    setLast();
}

String::~String() {
    if(!isOnStack())
        delete[] data.ptr;
}

String::String(const char* in)
        : String(in, strlen(in)) {}

String::String(const char* in, unsigned in_size) {
    using namespace std;
    if(in_size <= last) {
        memcpy(buf, in, in_size + 1);
        setLast(last - in_size);
    } else {
        setOnHeap();
        data.size     = in_size;
        data.capacity = data.size + 1;
        data.ptr      = new char[data.capacity];
        memcpy(data.ptr, in, in_size + 1);
    }
}

String::String(const String& other) { copy(other); }

String& String::operator=(const String& other) {
    if(this != &other) {
        if(!isOnStack())
            delete[] data.ptr;

        copy(other);
    }

    return *this;
}

String& String::operator+=(const String& other) {
    const unsigned my_old_size = size();
    const unsigned other_size  = other.size();
    const unsigned total_size  = my_old_size + other_size;
    using namespace std;
    if(isOnStack()) {
        if(total_size < len) {
            // append to the current stack space
            memcpy(buf + my_old_size, other.c_str(), other_size + 1);
            setLast(last - total_size);
        } else {
            // alloc new chunk
            char* temp = new char[total_size + 1];
            // copy current data to new location before writing in the union
            memcpy(temp, buf, my_old_size); // skip the +1 ('\0') for speed
            // update data in union
            setOnHeap();
            data.size     = total_size;
            data.capacity = data.size + 1;
            data.ptr      = temp;
            // transfer the rest of the data
            memcpy(data.ptr + my_old_size, other.c_str(), other_size + 1);
        }
    } else {
        if(data.capacity > total_size) {
            // append to the current heap block
            data.size = total_size;
            memcpy(data.ptr + my_old_size, other.c_str(), other_size + 1);
        } else {
            // resize
            data.capacity *= 2;
            if(data.capacity <= total_size)
                data.capacity = total_size + 1;
            // alloc new chunk
            char* temp = new char[data.capacity];
            // copy current data to new location before releasing it
            memcpy(temp, data.ptr, my_old_size); // skip the +1 ('\0') for speed
            // release old chunk
            delete[] data.ptr;
            // update the rest of the union members
            data.size = total_size;
            data.ptr  = temp;
            // transfer the rest of the data
            memcpy(data.ptr + my_old_size, other.c_str(), other_size + 1);
        }
    }

    return *this;
}

String String::operator+(const String& other) const { return String(*this) += other; }

String::String(String&& other) {
    using namespace std;
    memcpy(buf, other.buf, len);
    other.buf[0] = '\0';
    other.setLast();
}

String& String::operator=(String&& other) {
    using namespace std;
    if(this != &other) {
        if(!isOnStack())
            delete[] data.ptr;
        memcpy(buf, other.buf, len);
        other.buf[0] = '\0';
        other.setLast();
    }
    return *this;
}

char String::operator[](unsigned i) const {
    return const_cast<String*>(this)->operator[](i); // NOLINT
}

char& String::operator[](unsigned i) {
    if(isOnStack())
        return reinterpret_cast<char*>(buf)[i];
    return data.ptr[i];
}

DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wmaybe-uninitialized")
unsigned String::size() const {
    if(isOnStack())
        return last - (unsigned(buf[last]) & 31); // using "last" would work only if "len" is 32
    return data.size;
}
DOCTEST_GCC_SUPPRESS_WARNING_POP

unsigned String::capacity() const {
    if(isOnStack())
        return len;
    return data.capacity;
}

int String::compare(const char* other, bool no_case) const {
    if(no_case)
        return doctest::stricmp(c_str(), other);
    return std::strcmp(c_str(), other);
}

int String::compare(const String& other, bool no_case) const {
    return compare(other.c_str(), no_case);
}

// clang-format off
bool operator==(const String& lhs, const String& rhs) { return lhs.compare(rhs) == 0; }
bool operator!=(const String& lhs, const String& rhs) { return lhs.compare(rhs) != 0; }
bool operator< (const String& lhs, const String& rhs) { return lhs.compare(rhs) < 0; }
bool operator> (const String& lhs, const String& rhs) { return lhs.compare(rhs) > 0; }
bool operator<=(const String& lhs, const String& rhs) { return (lhs != rhs) ? lhs.compare(rhs) < 0 : true; }
bool operator>=(const String& lhs, const String& rhs) { return (lhs != rhs) ? lhs.compare(rhs) > 0 : true; }
// clang-format on

std::ostream& operator<<(std::ostream& s, const String& in) { return s << in.c_str(); }

namespace {
    void color_to_stream(std::ostream&, Color::Enum) DOCTEST_BRANCH_ON_DISABLED({}, ;)
} // namespace

namespace Color {
    std::ostream& operator<<(std::ostream& s, Color::Enum code) {
        color_to_stream(s, code);
        return s;
    }
} // namespace Color

// clang-format off
const char* assertString(assertType::Enum at) {
    DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4062) // enum 'x' in switch of enum 'y' is not handled
    switch(at) {  //!OCLINT missing default in switch statements
        case assertType::DT_WARN                    : return "WARN";
        case assertType::DT_CHECK                   : return "CHECK";
        case assertType::DT_REQUIRE                 : return "REQUIRE";

        case assertType::DT_WARN_FALSE              : return "WARN_FALSE";
        case assertType::DT_CHECK_FALSE             : return "CHECK_FALSE";
        case assertType::DT_REQUIRE_FALSE           : return "REQUIRE_FALSE";

        case assertType::DT_WARN_THROWS             : return "WARN_THROWS";
        case assertType::DT_CHECK_THROWS            : return "CHECK_THROWS";
        case assertType::DT_REQUIRE_THROWS          : return "REQUIRE_THROWS";

        case assertType::DT_WARN_THROWS_AS          : return "WARN_THROWS_AS";
        case assertType::DT_CHECK_THROWS_AS         : return "CHECK_THROWS_AS";
        case assertType::DT_REQUIRE_THROWS_AS       : return "REQUIRE_THROWS_AS";

        case assertType::DT_WARN_THROWS_WITH        : return "WARN_THROWS_WITH";
        case assertType::DT_CHECK_THROWS_WITH       : return "CHECK_THROWS_WITH";
        case assertType::DT_REQUIRE_THROWS_WITH     : return "REQUIRE_THROWS_WITH";

        case assertType::DT_WARN_THROWS_WITH_AS     : return "WARN_THROWS_WITH_AS";
        case assertType::DT_CHECK_THROWS_WITH_AS    : return "CHECK_THROWS_WITH_AS";
        case assertType::DT_REQUIRE_THROWS_WITH_AS  : return "REQUIRE_THROWS_WITH_AS";

        case assertType::DT_WARN_NOTHROW            : return "WARN_NOTHROW";
        case assertType::DT_CHECK_NOTHROW           : return "CHECK_NOTHROW";
        case assertType::DT_REQUIRE_NOTHROW         : return "REQUIRE_NOTHROW";

        case assertType::DT_WARN_EQ                 : return "WARN_EQ";
        case assertType::DT_CHECK_EQ                : return "CHECK_EQ";
        case assertType::DT_REQUIRE_EQ              : return "REQUIRE_EQ";
        case assertType::DT_WARN_NE                 : return "WARN_NE";
        case assertType::DT_CHECK_NE                : return "CHECK_NE";
        case assertType::DT_REQUIRE_NE              : return "REQUIRE_NE";
        case assertType::DT_WARN_GT                 : return "WARN_GT";
        case assertType::DT_CHECK_GT                : return "CHECK_GT";
        case assertType::DT_REQUIRE_GT              : return "REQUIRE_GT";
        case assertType::DT_WARN_LT                 : return "WARN_LT";
        case assertType::DT_CHECK_LT                : return "CHECK_LT";
        case assertType::DT_REQUIRE_LT              : return "REQUIRE_LT";
        case assertType::DT_WARN_GE                 : return "WARN_GE";
        case assertType::DT_CHECK_GE                : return "CHECK_GE";
        case assertType::DT_REQUIRE_GE              : return "REQUIRE_GE";
        case assertType::DT_WARN_LE                 : return "WARN_LE";
        case assertType::DT_CHECK_LE                : return "CHECK_LE";
        case assertType::DT_REQUIRE_LE              : return "REQUIRE_LE";

        case assertType::DT_WARN_UNARY              : return "WARN_UNARY";
        case assertType::DT_CHECK_UNARY             : return "CHECK_UNARY";
        case assertType::DT_REQUIRE_UNARY           : return "REQUIRE_UNARY";
        case assertType::DT_WARN_UNARY_FALSE        : return "WARN_UNARY_FALSE";
        case assertType::DT_CHECK_UNARY_FALSE       : return "CHECK_UNARY_FALSE";
        case assertType::DT_REQUIRE_UNARY_FALSE     : return "REQUIRE_UNARY_FALSE";
    }
    DOCTEST_MSVC_SUPPRESS_WARNING_POP
    return "";
}
// clang-format on

const char* failureString(assertType::Enum at) {
    if(at & assertType::is_warn) //!OCLINT bitwise operator in conditional
        return "WARNING";
    if(at & assertType::is_check) //!OCLINT bitwise operator in conditional
        return "ERROR";
    if(at & assertType::is_require) //!OCLINT bitwise operator in conditional
        return "FATAL ERROR";
    return "";
}

DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wnull-dereference")
DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wnull-dereference")
// depending on the current options this will remove the path of filenames
const char* skipPathFromFilename(const char* file) {
    if(getContextOptions()->no_path_in_filenames) {
        auto back    = std::strrchr(file, '\\');
        auto forward = std::strrchr(file, '/');
        if(back || forward) {
            if(back > forward)
                forward = back;
            return forward + 1;
        }
    }
    return file;
}
DOCTEST_CLANG_SUPPRESS_WARNING_POP
DOCTEST_GCC_SUPPRESS_WARNING_POP

bool SubcaseSignature::operator<(const SubcaseSignature& other) const {
    if(m_line != other.m_line)
        return m_line < other.m_line;
    if(std::strcmp(m_file, other.m_file) != 0)
        return std::strcmp(m_file, other.m_file) < 0;
    return m_name.compare(other.m_name) < 0;
}

IContextScope::IContextScope()  = default;
IContextScope::~IContextScope() = default;

#ifdef DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
String toString(char* in) { return toString(static_cast<const char*>(in)); }
String toString(const char* in) { return String("\"") + (in ? in : "{null string}") + "\""; }
#endif // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
String toString(bool in) { return in ? "true" : "false"; }
String toString(float in) { return fpToString(in, 5) + "f"; }
String toString(double in) { return fpToString(in, 10); }
String toString(double long in) { return fpToString(in, 15); }

#define DOCTEST_TO_STRING_OVERLOAD(type, fmt)                                                      \
    String toString(type in) {                                                                     \
        char buf[64];                                                                              \
        std::sprintf(buf, fmt, in);                                                                \
        return buf;                                                                                \
    }

DOCTEST_TO_STRING_OVERLOAD(char, "%d")
DOCTEST_TO_STRING_OVERLOAD(char signed, "%d")
DOCTEST_TO_STRING_OVERLOAD(char unsigned, "%u")
DOCTEST_TO_STRING_OVERLOAD(int short, "%d")
DOCTEST_TO_STRING_OVERLOAD(int short unsigned, "%u")
DOCTEST_TO_STRING_OVERLOAD(int, "%d")
DOCTEST_TO_STRING_OVERLOAD(unsigned, "%u")
DOCTEST_TO_STRING_OVERLOAD(int long, "%ld")
DOCTEST_TO_STRING_OVERLOAD(int long unsigned, "%lu")
DOCTEST_TO_STRING_OVERLOAD(int long long, "%lld")
DOCTEST_TO_STRING_OVERLOAD(int long long unsigned, "%llu")

String toString(std::nullptr_t) { return "NULL"; }

#if DOCTEST_MSVC >= DOCTEST_COMPILER(19, 20, 0)
// see this issue on why this is needed: https://github.com/onqtam/doctest/issues/183
String toString(const std::string& in) { return in.c_str(); }
#endif // VS 2019

Approx::Approx(double value)
        : m_epsilon(static_cast<double>(std::numeric_limits<float>::epsilon()) * 100)
        , m_scale(1.0)
        , m_value(value) {}

Approx Approx::operator()(double value) const {
    Approx approx(value);
    approx.epsilon(m_epsilon);
    approx.scale(m_scale);
    return approx;
}

Approx& Approx::epsilon(double newEpsilon) {
    m_epsilon = newEpsilon;
    return *this;
}
Approx& Approx::scale(double newScale) {
    m_scale = newScale;
    return *this;
}

bool operator==(double lhs, const Approx& rhs) {
    // Thanks to Richard Harris for his help refining this formula
    return std::fabs(lhs - rhs.m_value) <
           rhs.m_epsilon * (rhs.m_scale + std::max<double>(std::fabs(lhs), std::fabs(rhs.m_value)));
}
bool operator==(const Approx& lhs, double rhs) { return operator==(rhs, lhs); }
bool operator!=(double lhs, const Approx& rhs) { return !operator==(lhs, rhs); }
bool operator!=(const Approx& lhs, double rhs) { return !operator==(rhs, lhs); }
bool operator<=(double lhs, const Approx& rhs) { return lhs < rhs.m_value || lhs == rhs; }
bool operator<=(const Approx& lhs, double rhs) { return lhs.m_value < rhs || lhs == rhs; }
bool operator>=(double lhs, const Approx& rhs) { return lhs > rhs.m_value || lhs == rhs; }
bool operator>=(const Approx& lhs, double rhs) { return lhs.m_value > rhs || lhs == rhs; }
bool operator<(double lhs, const Approx& rhs) { return lhs < rhs.m_value && lhs != rhs; }
bool operator<(const Approx& lhs, double rhs) { return lhs.m_value < rhs && lhs != rhs; }
bool operator>(double lhs, const Approx& rhs) { return lhs > rhs.m_value && lhs != rhs; }
bool operator>(const Approx& lhs, double rhs) { return lhs.m_value > rhs && lhs != rhs; }

String toString(const Approx& in) {
    return String("Approx( ") + doctest::toString(in.m_value) + " )";
}
const ContextOptions* getContextOptions() { return DOCTEST_BRANCH_ON_DISABLED(nullptr, g_cs); }

} // namespace doctest

#ifdef DOCTEST_CONFIG_DISABLE
namespace doctest {
Context::Context(int, const char* const*) {}
Context::~Context() = default;
void Context::applyCommandLine(int, const char* const*) {}
void Context::addFilter(const char*, const char*) {}
void Context::clearFilters() {}
void Context::setOption(const char*, int) {}
void Context::setOption(const char*, const char*) {}
bool Context::shouldExit() { return false; }
void Context::setAsDefaultForAssertsOutOfTestCases() {}
void Context::setAssertHandler(detail::assert_handler) {}
int  Context::run() { return 0; }

IReporter::~IReporter() = default;

int                         IReporter::get_num_active_contexts() { return 0; }
const IContextScope* const* IReporter::get_active_contexts() { return nullptr; }
int                         IReporter::get_num_stringified_contexts() { return 0; }
const String*               IReporter::get_stringified_contexts() { return nullptr; }

int registerReporter(const char*, int, IReporter*) { return 0; }

} // namespace doctest
#else // DOCTEST_CONFIG_DISABLE

#if !defined(DOCTEST_CONFIG_COLORS_NONE)
#if !defined(DOCTEST_CONFIG_COLORS_WINDOWS) && !defined(DOCTEST_CONFIG_COLORS_ANSI)
#ifdef DOCTEST_PLATFORM_WINDOWS
#define DOCTEST_CONFIG_COLORS_WINDOWS
#else // linux
#define DOCTEST_CONFIG_COLORS_ANSI
#endif // platform
#endif // DOCTEST_CONFIG_COLORS_WINDOWS && DOCTEST_CONFIG_COLORS_ANSI
#endif // DOCTEST_CONFIG_COLORS_NONE

namespace doctest_detail_test_suite_ns {
// holds the current test suite
doctest::detail::TestSuite& getCurrentTestSuite() {
    static doctest::detail::TestSuite data;
    return data;
}
} // namespace doctest_detail_test_suite_ns

namespace doctest {
namespace {
    // the int (priority) is part of the key for automatic sorting - sadly one can register a
    // reporter with a duplicate name and a different priority but hopefully that won't happen often :|
    typedef std::map<std::pair<int, String>, reporterCreatorFunc> reporterMap;

    reporterMap& getReporters() {
        static reporterMap data;
        return data;
    }
    reporterMap& getListeners() {
        static reporterMap data;
        return data;
    }
} // namespace
namespace detail {
#define DOCTEST_ITERATE_THROUGH_REPORTERS(function, ...)                                           \
    for(auto& curr_rep : g_cs->reporters_currently_used)                                           \
    curr_rep->function(__VA_ARGS__)

    bool checkIfShouldThrow(assertType::Enum at) {
        if(at & assertType::is_require) //!OCLINT bitwise operator in conditional
            return true;

        if((at & assertType::is_check) //!OCLINT bitwise operator in conditional
           && getContextOptions()->abort_after > 0 &&
           (g_cs->numAssertsFailed + g_cs->numAssertsFailedCurrentTest_atomic) >=
                   getContextOptions()->abort_after)
            return true;

        return false;
    }

#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
    [[noreturn]] void throwException() {
        g_cs->shouldLogCurrentException = false;
        throw TestFailureException();
    } // NOLINT(cert-err60-cpp)
#else // DOCTEST_CONFIG_NO_EXCEPTIONS
    void throwException() {}
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS
} // namespace detail

namespace {
    using namespace detail;
    // matching of a string against a wildcard mask (case sensitivity configurable) taken from
    // https://www.codeproject.com/Articles/1088/Wildcard-string-compare-globbing
    int wildcmp(const char* str, const char* wild, bool caseSensitive) {
        const char* cp = nullptr;
        const char* mp = nullptr;

        while((*str) && (*wild != '*')) {
            if((caseSensitive ? (*wild != *str) : (tolower(*wild) != tolower(*str))) &&
               (*wild != '?')) {
                return 0;
            }
            wild++;
            str++;
        }

        while(*str) {
            if(*wild == '*') {
                if(!*++wild) {
                    return 1;
                }
                mp = wild;
                cp = str + 1;
            } else if((caseSensitive ? (*wild == *str) : (tolower(*wild) == tolower(*str))) ||
                      (*wild == '?')) {
                wild++;
                str++;
            } else {
                wild = mp;   //!OCLINT parameter reassignment
                str  = cp++; //!OCLINT parameter reassignment
            }
        }

        while(*wild == '*') {
            wild++;
        }
        return !*wild;
    }

    //// C string hash function (djb2) - taken from http://www.cse.yorku.ca/~oz/hash.html
    //unsigned hashStr(unsigned const char* str) {
    //    unsigned long hash = 5381;
    //    char          c;
    //    while((c = *str++))
    //        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    //    return hash;
    //}

    // checks if the name matches any of the filters (and can be configured what to do when empty)
    bool matchesAny(const char* name, const std::vector<String>& filters, bool matchEmpty,
                    bool caseSensitive) {
        if(filters.empty() && matchEmpty)
            return true;
        for(auto& curr : filters)
            if(wildcmp(name, curr.c_str(), caseSensitive))
                return true;
        return false;
    }
} // namespace
namespace detail {

    Subcase::Subcase(const String& name, const char* file, int line)
            : m_signature({name, file, line}) {
        ContextState* s = g_cs;

        // check subcase filters
        if(s->subcasesStack.size() < size_t(s->subcase_filter_levels)) {
            if(!matchesAny(m_signature.m_name.c_str(), s->filters[6], true, s->case_sensitive))
                return;
            if(matchesAny(m_signature.m_name.c_str(), s->filters[7], false, s->case_sensitive))
                return;
        }
        
        // if a Subcase on the same level has already been entered
        if(s->subcasesStack.size() < size_t(s->subcasesCurrentMaxLevel)) {
            s->should_reenter = true;
            return;
        }

        // push the current signature to the stack so we can check if the
        // current stack + the current new subcase have been traversed
        s->subcasesStack.push_back(m_signature);
        if(s->subcasesPassed.count(s->subcasesStack) != 0) {
            // pop - revert to previous stack since we've already passed this
            s->subcasesStack.pop_back();
            return;
        }

        s->subcasesCurrentMaxLevel = s->subcasesStack.size();
        m_entered = true;

        DOCTEST_ITERATE_THROUGH_REPORTERS(subcase_start, m_signature);
    }

    DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4996) // std::uncaught_exception is deprecated in C++17	
    DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wdeprecated-declarations")	
    DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wdeprecated-declarations")

    Subcase::~Subcase() {
        if(m_entered) {
            // only mark the subcase stack as passed if no subcases have been skipped
            if(g_cs->should_reenter == false)
                g_cs->subcasesPassed.insert(g_cs->subcasesStack);
            g_cs->subcasesStack.pop_back();

#if __cplusplus >= 201703L && defined(__cpp_lib_uncaught_exceptions) && __cpp_lib_uncaught_exceptions >= 201411
            if(std::uncaught_exceptions() > 0
#else
            if(std::uncaught_exception()
#endif
            && g_cs->shouldLogCurrentException) {
                DOCTEST_ITERATE_THROUGH_REPORTERS(
                        test_case_exception, {"exception thrown in subcase - will translate later "
                                              "when the whole test case has been exited (cannot "
                                              "translate while there is an active exception)",
                                              false});
                g_cs->shouldLogCurrentException = false;
            }
            DOCTEST_ITERATE_THROUGH_REPORTERS(subcase_end, DOCTEST_EMPTY);
        }
    }

    DOCTEST_CLANG_SUPPRESS_WARNING_POP	
    DOCTEST_GCC_SUPPRESS_WARNING_POP	
    DOCTEST_MSVC_SUPPRESS_WARNING_POP

    Subcase::operator bool() const { return m_entered; }

    Result::Result(bool passed, const String& decomposition)
            : m_passed(passed)
            , m_decomp(decomposition) {}

    ExpressionDecomposer::ExpressionDecomposer(assertType::Enum at)
            : m_at(at) {}

    TestSuite& TestSuite::operator*(const char* in) {
        m_test_suite = in;
        // clear state
        m_description       = nullptr;
        m_skip              = false;
        m_may_fail          = false;
        m_should_fail       = false;
        m_expected_failures = 0;
        m_timeout           = 0;
        return *this;
    }

    TestCase::TestCase(funcType test, const char* file, unsigned line, const TestSuite& test_suite,
                       const char* type, int template_id) {
        m_file              = file;
        m_line              = line;
        m_name              = nullptr; // will be later overridden in operator*
        m_test_suite        = test_suite.m_test_suite;
        m_description       = test_suite.m_description;
        m_skip              = test_suite.m_skip;
        m_may_fail          = test_suite.m_may_fail;
        m_should_fail       = test_suite.m_should_fail;
        m_expected_failures = test_suite.m_expected_failures;
        m_timeout           = test_suite.m_timeout;

        m_test        = test;
        m_type        = type;
        m_template_id = template_id;
    }

    TestCase::TestCase(const TestCase& other)
            : TestCaseData() {
        *this = other;
    }

    DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(26434) // hides a non-virtual function
    DOCTEST_MSVC_SUPPRESS_WARNING(26437)           // Do not slice
    TestCase& TestCase::operator=(const TestCase& other) {
        static_cast<TestCaseData&>(*this) = static_cast<const TestCaseData&>(other);

        m_test        = other.m_test;
        m_type        = other.m_type;
        m_template_id = other.m_template_id;
        m_full_name   = other.m_full_name;

        if(m_template_id != -1)
            m_name = m_full_name.c_str();
        return *this;
    }
    DOCTEST_MSVC_SUPPRESS_WARNING_POP

    TestCase& TestCase::operator*(const char* in) {
        m_name = in;
        // make a new name with an appended type for templated test case
        if(m_template_id != -1) {
            m_full_name = String(m_name) + m_type;
            // redirect the name to point to the newly constructed full name
            m_name = m_full_name.c_str();
        }
        return *this;
    }

    bool TestCase::operator<(const TestCase& other) const {
        if(m_line != other.m_line)
            return m_line < other.m_line;
        const int file_cmp = std::strcmp(m_file, other.m_file);
        if(file_cmp != 0)
            return file_cmp < 0;
        return m_template_id < other.m_template_id;
    }
} // namespace detail
namespace {
    using namespace detail;
    // for sorting tests by file/line
    bool fileOrderComparator(const TestCase* lhs, const TestCase* rhs) {
#if DOCTEST_MSVC
        // this is needed because MSVC gives different case for drive letters
        // for __FILE__ when evaluated in a header and a source file
        const int res = doctest::stricmp(lhs->m_file, rhs->m_file);
#else  // MSVC
        const int res = std::strcmp(lhs->m_file, rhs->m_file);
#endif // MSVC
        if(res != 0)
            return res < 0;
        if(lhs->m_line != rhs->m_line)
            return lhs->m_line < rhs->m_line;
        return lhs->m_template_id < rhs->m_template_id;
    }

    // for sorting tests by suite/file/line
    bool suiteOrderComparator(const TestCase* lhs, const TestCase* rhs) {
        const int res = std::strcmp(lhs->m_test_suite, rhs->m_test_suite);
        if(res != 0)
            return res < 0;
        return fileOrderComparator(lhs, rhs);
    }

    // for sorting tests by name/suite/file/line
    bool nameOrderComparator(const TestCase* lhs, const TestCase* rhs) {
        const int res = std::strcmp(lhs->m_name, rhs->m_name);
        if(res != 0)
            return res < 0;
        return suiteOrderComparator(lhs, rhs);
    }

    // all the registered tests
    std::set<TestCase>& getRegisteredTests() {
        static std::set<TestCase> data;
        return data;
    }

#ifdef DOCTEST_CONFIG_COLORS_WINDOWS
    HANDLE g_stdoutHandle;
    WORD   g_origFgAttrs;
    WORD   g_origBgAttrs;
    bool   g_attrsInitted = false;

    int colors_init() {
        if(!g_attrsInitted) {
            g_stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
            g_attrsInitted = true;
            CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
            GetConsoleScreenBufferInfo(g_stdoutHandle, &csbiInfo);
            g_origFgAttrs = csbiInfo.wAttributes & ~(BACKGROUND_GREEN | BACKGROUND_RED |
                                                     BACKGROUND_BLUE | BACKGROUND_INTENSITY);
            g_origBgAttrs = csbiInfo.wAttributes & ~(FOREGROUND_GREEN | FOREGROUND_RED |
                                                     FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        }
        return 0;
    }

    int dumy_init_console_colors = colors_init();
#endif // DOCTEST_CONFIG_COLORS_WINDOWS

    DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wdeprecated-declarations")
    void color_to_stream(std::ostream& s, Color::Enum code) {
        ((void)s);    // for DOCTEST_CONFIG_COLORS_NONE or DOCTEST_CONFIG_COLORS_WINDOWS
        ((void)code); // for DOCTEST_CONFIG_COLORS_NONE
#ifdef DOCTEST_CONFIG_COLORS_ANSI
        if(g_no_colors ||
           (isatty(STDOUT_FILENO) == false && getContextOptions()->force_colors == false))
            return;

        auto col = "";
        // clang-format off
            switch(code) { //!OCLINT missing break in switch statement / unnecessary default statement in covered switch statement
                case Color::Red:         col = "[0;31m"; break;
                case Color::Green:       col = "[0;32m"; break;
                case Color::Blue:        col = "[0;34m"; break;
                case Color::Cyan:        col = "[0;36m"; break;
                case Color::Yellow:      col = "[0;33m"; break;
                case Color::Grey:        col = "[1;30m"; break;
                case Color::LightGrey:   col = "[0;37m"; break;
                case Color::BrightRed:   col = "[1;31m"; break;
                case Color::BrightGreen: col = "[1;32m"; break;
                case Color::BrightWhite: col = "[1;37m"; break;
                case Color::Bright: // invalid
                case Color::None:
                case Color::White:
                default:                 col = "[0m";
            }
        // clang-format on
        s << "\033" << col;
#endif // DOCTEST_CONFIG_COLORS_ANSI

#ifdef DOCTEST_CONFIG_COLORS_WINDOWS
        if(g_no_colors ||
           (isatty(fileno(stdout)) == false && getContextOptions()->force_colors == false))
            return;

#define DOCTEST_SET_ATTR(x) SetConsoleTextAttribute(g_stdoutHandle, x | g_origBgAttrs)

        // clang-format off
        switch (code) {
            case Color::White:       DOCTEST_SET_ATTR(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE); break;
            case Color::Red:         DOCTEST_SET_ATTR(FOREGROUND_RED);                                      break;
            case Color::Green:       DOCTEST_SET_ATTR(FOREGROUND_GREEN);                                    break;
            case Color::Blue:        DOCTEST_SET_ATTR(FOREGROUND_BLUE);                                     break;
            case Color::Cyan:        DOCTEST_SET_ATTR(FOREGROUND_BLUE | FOREGROUND_GREEN);                  break;
            case Color::Yellow:      DOCTEST_SET_ATTR(FOREGROUND_RED | FOREGROUND_GREEN);                   break;
            case Color::Grey:        DOCTEST_SET_ATTR(0);                                                   break;
            case Color::LightGrey:   DOCTEST_SET_ATTR(FOREGROUND_INTENSITY);                                break;
            case Color::BrightRed:   DOCTEST_SET_ATTR(FOREGROUND_INTENSITY | FOREGROUND_RED);               break;
            case Color::BrightGreen: DOCTEST_SET_ATTR(FOREGROUND_INTENSITY | FOREGROUND_GREEN);             break;
            case Color::BrightWhite: DOCTEST_SET_ATTR(FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE); break;
            case Color::None:
            case Color::Bright: // invalid
            default:                 DOCTEST_SET_ATTR(g_origFgAttrs);
        }
            // clang-format on
#endif // DOCTEST_CONFIG_COLORS_WINDOWS
    }
    DOCTEST_CLANG_SUPPRESS_WARNING_POP

    std::vector<const IExceptionTranslator*>& getExceptionTranslators() {
        static std::vector<const IExceptionTranslator*> data;
        return data;
    }

    String translateActiveException() {
#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
        String res;
        auto&  translators = getExceptionTranslators();
        for(auto& curr : translators)
            if(curr->translate(res))
                return res;
        // clang-format off
        DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wcatch-value")
        try {
            throw;
        } catch(std::exception& ex) {
            return ex.what();
        } catch(std::string& msg) {
            return msg.c_str();
        } catch(const char* msg) {
            return msg;
        } catch(...) {
            return "unknown exception";
        }
        DOCTEST_GCC_SUPPRESS_WARNING_POP
// clang-format on
#else  // DOCTEST_CONFIG_NO_EXCEPTIONS
        return "";
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS
    }
} // namespace

namespace detail {
    // used by the macros for registering tests
    int regTest(const TestCase& tc) {
        getRegisteredTests().insert(tc);
        return 0;
    }

    // sets the current test suite
    int setTestSuite(const TestSuite& ts) {
        doctest_detail_test_suite_ns::getCurrentTestSuite() = ts;
        return 0;
    }

#ifdef DOCTEST_IS_DEBUGGER_ACTIVE
    bool isDebuggerActive() { return DOCTEST_IS_DEBUGGER_ACTIVE(); }
#else // DOCTEST_IS_DEBUGGER_ACTIVE
#ifdef DOCTEST_PLATFORM_MAC
    // The following function is taken directly from the following technical note:
    // https://developer.apple.com/library/archive/qa/qa1361/_index.html
    // Returns true if the current process is being debugged (either
    // running under the debugger or has a debugger attached post facto).
    bool isDebuggerActive() {
        int        mib[4];
        kinfo_proc info;
        size_t     size;
        // Initialize the flags so that, if sysctl fails for some bizarre
        // reason, we get a predictable result.
        info.kp_proc.p_flag = 0;
        // Initialize mib, which tells sysctl the info we want, in this case
        // we're looking for information about a specific process ID.
        mib[0] = CTL_KERN;
        mib[1] = KERN_PROC;
        mib[2] = KERN_PROC_PID;
        mib[3] = getpid();
        // Call sysctl.
        size = sizeof(info);
        if(sysctl(mib, DOCTEST_COUNTOF(mib), &info, &size, 0, 0) != 0) {
            std::cerr << "\nCall to sysctl failed - unable to determine if debugger is active **\n";
            return false;
        }
        // We're being debugged if the P_TRACED flag is set.
        return ((info.kp_proc.p_flag & P_TRACED) != 0);
    }
#elif DOCTEST_MSVC || defined(__MINGW32__)
    bool isDebuggerActive() { return ::IsDebuggerPresent() != 0; }
#else
    bool isDebuggerActive() { return false; }
#endif // Platform
#endif // DOCTEST_IS_DEBUGGER_ACTIVE

    void registerExceptionTranslatorImpl(const IExceptionTranslator* et) {
        if(std::find(getExceptionTranslators().begin(), getExceptionTranslators().end(), et) ==
           getExceptionTranslators().end())
            getExceptionTranslators().push_back(et);
    }

#ifdef DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
    void toStream(std::ostream* s, char* in) { *s << in; }
    void toStream(std::ostream* s, const char* in) { *s << in; }
#endif // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
    void toStream(std::ostream* s, bool in) { *s << std::boolalpha << in << std::noboolalpha; }
    void toStream(std::ostream* s, float in) { *s << in; }
    void toStream(std::ostream* s, double in) { *s << in; }
    void toStream(std::ostream* s, double long in) { *s << in; }

    void toStream(std::ostream* s, char in) { *s << in; }
    void toStream(std::ostream* s, char signed in) { *s << in; }
    void toStream(std::ostream* s, char unsigned in) { *s << in; }
    void toStream(std::ostream* s, int short in) { *s << in; }
    void toStream(std::ostream* s, int short unsigned in) { *s << in; }
    void toStream(std::ostream* s, int in) { *s << in; }
    void toStream(std::ostream* s, int unsigned in) { *s << in; }
    void toStream(std::ostream* s, int long in) { *s << in; }
    void toStream(std::ostream* s, int long unsigned in) { *s << in; }
    void toStream(std::ostream* s, int long long in) { *s << in; }
    void toStream(std::ostream* s, int long long unsigned in) { *s << in; }

    DOCTEST_THREAD_LOCAL std::vector<IContextScope*> g_infoContexts; // for logging with INFO()

    ContextScopeBase::ContextScopeBase() {
        g_infoContexts.push_back(this);
    }

    DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4996) // std::uncaught_exception is deprecated in C++17	
    DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wdeprecated-declarations")	
    DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wdeprecated-declarations")

    // destroy cannot be inlined into the destructor because that would mean calling stringify after
    // ContextScope has been destroyed (base class destructors run after derived class destructors).
    // Instead, ContextScope calls this method directly from its destructor.
    void ContextScopeBase::destroy() {
#if __cplusplus >= 201703L && defined(__cpp_lib_uncaught_exceptions) && __cpp_lib_uncaught_exceptions >= 201411
        if(std::uncaught_exceptions() > 0) {
#else
        if(std::uncaught_exception()) {
#endif
            std::ostringstream s;
            this->stringify(&s);
            g_cs->stringifiedContexts.push_back(s.str().c_str());
        }
        g_infoContexts.pop_back();
    }

    DOCTEST_CLANG_SUPPRESS_WARNING_POP	
    DOCTEST_GCC_SUPPRESS_WARNING_POP	
    DOCTEST_MSVC_SUPPRESS_WARNING_POP
} // namespace detail
namespace {
    using namespace detail;

    std::ostream& file_line_to_stream(std::ostream& s, const char* file, int line,
                                      const char* tail = "") {
        const auto opt = getContextOptions();
        s << Color::LightGrey << skipPathFromFilename(file) << (opt->gnu_file_line ? ":" : "(")
          << (opt->no_line_numbers ? 0 : line) // 0 or the real num depending on the option
          << (opt->gnu_file_line ? ":" : "):") << tail;
        return s;
    }

#if !defined(DOCTEST_CONFIG_POSIX_SIGNALS) && !defined(DOCTEST_CONFIG_WINDOWS_SEH)
    struct FatalConditionHandler
    {
        void reset() {}
    };
#else // DOCTEST_CONFIG_POSIX_SIGNALS || DOCTEST_CONFIG_WINDOWS_SEH

    void reportFatal(const std::string&);

#ifdef DOCTEST_PLATFORM_WINDOWS

    struct SignalDefs
    {
        DWORD id;
        const char* name;
    };
    // There is no 1-1 mapping between signals and windows exceptions.
    // Windows can easily distinguish between SO and SigSegV,
    // but SigInt, SigTerm, etc are handled differently.
    SignalDefs signalDefs[] = {
            {EXCEPTION_ILLEGAL_INSTRUCTION, "SIGILL - Illegal instruction signal"},
            {EXCEPTION_STACK_OVERFLOW, "SIGSEGV - Stack overflow"},
            {EXCEPTION_ACCESS_VIOLATION, "SIGSEGV - Segmentation violation signal"},
            {EXCEPTION_INT_DIVIDE_BY_ZERO, "Divide by zero error"},
    };

    struct FatalConditionHandler
    {
        static LONG CALLBACK handleException(PEXCEPTION_POINTERS ExceptionInfo) {
            for(size_t i = 0; i < DOCTEST_COUNTOF(signalDefs); ++i) {
                if(ExceptionInfo->ExceptionRecord->ExceptionCode == signalDefs[i].id) {
                    reportFatal(signalDefs[i].name);
                    break;
                }
            }
            // If its not an exception we care about, pass it along.
            // This stops us from eating debugger breaks etc.
            return EXCEPTION_CONTINUE_SEARCH;
        }

        FatalConditionHandler() {
            isSet = true;
            // 32k seems enough for doctest to handle stack overflow,
            // but the value was found experimentally, so there is no strong guarantee
            guaranteeSize = 32 * 1024;
            // Register an unhandled exception filter
            previousTop = SetUnhandledExceptionFilter(handleException);
            // Pass in guarantee size to be filled
            SetThreadStackGuarantee(&guaranteeSize);
        }

        static void reset() {
            if(isSet) {
                // Unregister handler and restore the old guarantee
                SetUnhandledExceptionFilter(previousTop);
                SetThreadStackGuarantee(&guaranteeSize);
                previousTop = nullptr;
                isSet = false;
            }
        }

        ~FatalConditionHandler() { reset(); }

    private:
        static bool isSet;
        static ULONG guaranteeSize;
        static LPTOP_LEVEL_EXCEPTION_FILTER previousTop;
    };

    bool FatalConditionHandler::isSet = false;
    ULONG FatalConditionHandler::guaranteeSize = 0;
    LPTOP_LEVEL_EXCEPTION_FILTER FatalConditionHandler::previousTop = nullptr;

#else // DOCTEST_PLATFORM_WINDOWS

    struct SignalDefs
    {
        int         id;
        const char* name;
    };
    SignalDefs signalDefs[] = {{SIGINT, "SIGINT - Terminal interrupt signal"},
                               {SIGILL, "SIGILL - Illegal instruction signal"},
                               {SIGFPE, "SIGFPE - Floating point error signal"},
                               {SIGSEGV, "SIGSEGV - Segmentation violation signal"},
                               {SIGTERM, "SIGTERM - Termination request signal"},
                               {SIGABRT, "SIGABRT - Abort (abnormal termination) signal"}};

    struct FatalConditionHandler
    {
        static bool             isSet;
        static struct sigaction oldSigActions[DOCTEST_COUNTOF(signalDefs)];
        static stack_t          oldSigStack;
        static char             altStackMem[4 * SIGSTKSZ];

        static void handleSignal(int sig) {
            const char* name = "<unknown signal>";
            for(std::size_t i = 0; i < DOCTEST_COUNTOF(signalDefs); ++i) {
                SignalDefs& def = signalDefs[i];
                if(sig == def.id) {
                    name = def.name;
                    break;
                }
            }
            reset();
            reportFatal(name);
            raise(sig);
        }

        FatalConditionHandler() {
            isSet = true;
            stack_t sigStack;
            sigStack.ss_sp    = altStackMem;
            sigStack.ss_size  = sizeof(altStackMem);
            sigStack.ss_flags = 0;
            sigaltstack(&sigStack, &oldSigStack);
            struct sigaction sa = {};
            sa.sa_handler       = handleSignal; // NOLINT
            sa.sa_flags         = SA_ONSTACK;
            for(std::size_t i = 0; i < DOCTEST_COUNTOF(signalDefs); ++i) {
                sigaction(signalDefs[i].id, &sa, &oldSigActions[i]);
            }
        }

        ~FatalConditionHandler() { reset(); }
        static void reset() {
            if(isSet) {
                // Set signals back to previous values -- hopefully nobody overwrote them in the meantime
                for(std::size_t i = 0; i < DOCTEST_COUNTOF(signalDefs); ++i) {
                    sigaction(signalDefs[i].id, &oldSigActions[i], nullptr);
                }
                // Return the old stack
                sigaltstack(&oldSigStack, nullptr);
                isSet = false;
            }
        }
    };

    bool             FatalConditionHandler::isSet                                      = false;
    struct sigaction FatalConditionHandler::oldSigActions[DOCTEST_COUNTOF(signalDefs)] = {};
    stack_t          FatalConditionHandler::oldSigStack                                = {};
    char             FatalConditionHandler::altStackMem[]                              = {};

#endif // DOCTEST_PLATFORM_WINDOWS
#endif // DOCTEST_CONFIG_POSIX_SIGNALS || DOCTEST_CONFIG_WINDOWS_SEH

} // namespace

namespace {
    using namespace detail;

#ifdef DOCTEST_PLATFORM_WINDOWS
#define DOCTEST_OUTPUT_DEBUG_STRING(text) ::OutputDebugStringA(text)
#else
    // TODO: integration with XCode and other IDEs
#define DOCTEST_OUTPUT_DEBUG_STRING(text) // NOLINT(clang-diagnostic-unused-macros)
#endif // Platform

    void addAssert(assertType::Enum at) {
        if((at & assertType::is_warn) == 0) //!OCLINT bitwise operator in conditional
            g_cs->numAssertsCurrentTest_atomic++;
    }

    void addFailedAssert(assertType::Enum at) {
        if((at & assertType::is_warn) == 0) //!OCLINT bitwise operator in conditional
            g_cs->numAssertsFailedCurrentTest_atomic++;
    }

#if defined(DOCTEST_CONFIG_POSIX_SIGNALS) || defined(DOCTEST_CONFIG_WINDOWS_SEH)
    void reportFatal(const std::string& message) {
        g_cs->failure_flags |= TestCaseFailureReason::Crash;

        DOCTEST_ITERATE_THROUGH_REPORTERS(test_case_exception, {message.c_str(), true});

        while(g_cs->subcasesStack.size()) {
            g_cs->subcasesStack.pop_back();
            DOCTEST_ITERATE_THROUGH_REPORTERS(subcase_end, DOCTEST_EMPTY);
        }

        g_cs->finalizeTestCaseData();

        DOCTEST_ITERATE_THROUGH_REPORTERS(test_case_end, *g_cs);

        DOCTEST_ITERATE_THROUGH_REPORTERS(test_run_end, *g_cs);
    }
#endif // DOCTEST_CONFIG_POSIX_SIGNALS || DOCTEST_CONFIG_WINDOWS_SEH
} // namespace
namespace detail {

    ResultBuilder::ResultBuilder(assertType::Enum at, const char* file, int line, const char* expr,
                                 const char* exception_type, const char* exception_string) {
        m_test_case        = g_cs->currentTest;
        m_at               = at;
        m_file             = file;
        m_line             = line;
        m_expr             = expr;
        m_failed           = true;
        m_threw            = false;
        m_threw_as         = false;
        m_exception_type   = exception_type;
        m_exception_string = exception_string;
#if DOCTEST_MSVC
        if(m_expr[0] == ' ') // this happens when variadic macros are disabled under MSVC
            ++m_expr;
#endif // MSVC
    }

    void ResultBuilder::setResult(const Result& res) {
        m_decomp = res.m_decomp;
        m_failed = !res.m_passed;
    }

    void ResultBuilder::translateException() {
        m_threw     = true;
        m_exception = translateActiveException();
    }

    bool ResultBuilder::log() {
        if(m_at & assertType::is_throws) { //!OCLINT bitwise operator in conditional
            m_failed = !m_threw;
        } else if((m_at & assertType::is_throws_as) && (m_at & assertType::is_throws_with)) { //!OCLINT
            m_failed = !m_threw_as || (m_exception != m_exception_string);
        } else if(m_at & assertType::is_throws_as) { //!OCLINT bitwise operator in conditional
            m_failed = !m_threw_as;
        } else if(m_at & assertType::is_throws_with) { //!OCLINT bitwise operator in conditional
            m_failed = m_exception != m_exception_string;
        } else if(m_at & assertType::is_nothrow) { //!OCLINT bitwise operator in conditional
            m_failed = m_threw;
        }

        if(m_exception.size())
            m_exception = String("\"") + m_exception + "\"";

        if(is_running_in_test) {
            addAssert(m_at);
            DOCTEST_ITERATE_THROUGH_REPORTERS(log_assert, *this);

            if(m_failed)
                addFailedAssert(m_at);
        } else if(m_failed) {
            failed_out_of_a_testing_context(*this);
        }

        return m_failed && isDebuggerActive() &&
               !getContextOptions()->no_breaks; // break into debugger
    }

    void ResultBuilder::react() const {
        if(m_failed && checkIfShouldThrow(m_at))
            throwException();
    }

    void failed_out_of_a_testing_context(const AssertData& ad) {
        if(g_cs->ah)
            g_cs->ah(ad);
        else
            std::abort();
    }

    void decomp_assert(assertType::Enum at, const char* file, int line, const char* expr,
                       Result result) {
        bool failed = !result.m_passed;

        // ###################################################################################
        // IF THE DEBUGGER BREAKS HERE - GO 1 LEVEL UP IN THE CALLSTACK FOR THE FAILING ASSERT
        // THIS IS THE EFFECT OF HAVING 'DOCTEST_CONFIG_SUPER_FAST_ASSERTS' DEFINED
        // ###################################################################################
        DOCTEST_ASSERT_OUT_OF_TESTS(result.m_decomp);
        DOCTEST_ASSERT_IN_TESTS(result.m_decomp);
    }

    MessageBuilder::MessageBuilder(const char* file, int line, assertType::Enum severity) {
        m_stream   = getTlsOss();
        m_file     = file;
        m_line     = line;
        m_severity = severity;
    }

    IExceptionTranslator::IExceptionTranslator()  = default;
    IExceptionTranslator::~IExceptionTranslator() = default;

    bool MessageBuilder::log() {
        m_string = getTlsOssResult();
        DOCTEST_ITERATE_THROUGH_REPORTERS(log_message, *this);

        const bool isWarn = m_severity & assertType::is_warn;

        // warn is just a message in this context so we don't treat it as an assert
        if(!isWarn) {
            addAssert(m_severity);
            addFailedAssert(m_severity);
        }

        return isDebuggerActive() && !getContextOptions()->no_breaks && !isWarn; // break
    }

    void MessageBuilder::react() {
        if(m_severity & assertType::is_require) //!OCLINT bitwise operator in conditional
            throwException();
    }

    MessageBuilder::~MessageBuilder() = default;
} // namespace detail
namespace {
    using namespace detail;

    template <typename Ex>
    [[noreturn]] void throw_exception(Ex const& e) {
#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
        throw e;
#else  // DOCTEST_CONFIG_NO_EXCEPTIONS
        std::cerr << "doctest will terminate because it needed to throw an exception.\n"
                  << "The message was: " << e.what() << '\n';
        std::terminate();
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS
    }

#define DOCTEST_INTERNAL_ERROR(msg)                                                                \
    throw_exception(std::logic_error(                                                              \
            __FILE__ ":" DOCTEST_TOSTR(__LINE__) ": Internal doctest error: " msg))

    // clang-format off

// =================================================================================================
// The following code has been taken verbatim from Catch2/include/internal/catch_xmlwriter.h/cpp
// This is done so cherry-picking bug fixes is trivial - even the style/formatting is untouched.
// =================================================================================================

    class XmlEncode {
    public:
        enum ForWhat { ForTextNodes, ForAttributes };

        XmlEncode( std::string const& str, ForWhat forWhat = ForTextNodes );

        void encodeTo( std::ostream& os ) const;

        friend std::ostream& operator << ( std::ostream& os, XmlEncode const& xmlEncode );

    private:
        std::string m_str;
        ForWhat m_forWhat;
    };

    class XmlWriter {
    public:

        class ScopedElement {
        public:
            ScopedElement( XmlWriter* writer );

            ScopedElement( ScopedElement&& other ) noexcept;
            ScopedElement& operator=( ScopedElement&& other ) noexcept;

            ~ScopedElement();

            ScopedElement& writeText( std::string const& text, bool indent = true );

            template<typename T>
            ScopedElement& writeAttribute( std::string const& name, T const& attribute ) {
                m_writer->writeAttribute( name, attribute );
                return *this;
            }

        private:
            mutable XmlWriter* m_writer = nullptr;
        };

        XmlWriter( std::ostream& os = std::cout );
        ~XmlWriter();

        XmlWriter( XmlWriter const& ) = delete;
        XmlWriter& operator=( XmlWriter const& ) = delete;

        XmlWriter& startElement( std::string const& name );

        ScopedElement scopedElement( std::string const& name );

        XmlWriter& endElement();

        XmlWriter& writeAttribute( std::string const& name, std::string const& attribute );

        XmlWriter& writeAttribute( std::string const& name, const char* attribute );

        XmlWriter& writeAttribute( std::string const& name, bool attribute );

        template<typename T>
        XmlWriter& writeAttribute( std::string const& name, T const& attribute ) {
        std::stringstream rss;
            rss << attribute;
            return writeAttribute( name, rss.str() );
        }

        XmlWriter& writeText( std::string const& text, bool indent = true );

        //XmlWriter& writeComment( std::string const& text );

        //void writeStylesheetRef( std::string const& url );

        //XmlWriter& writeBlankLine();

        void ensureTagClosed();

    private:

        void writeDeclaration();

        void newlineIfNecessary();

        bool m_tagIsOpen = false;
        bool m_needsNewline = false;
        std::vector<std::string> m_tags;
        std::string m_indent;
        std::ostream& m_os;
    };

// =================================================================================================
// The following code has been taken verbatim from Catch2/include/internal/catch_xmlwriter.h/cpp
// This is done so cherry-picking bug fixes is trivial - even the style/formatting is untouched.
// =================================================================================================

using uchar = unsigned char;

namespace {

    size_t trailingBytes(unsigned char c) {
        if ((c & 0xE0) == 0xC0) {
            return 2;
        }
        if ((c & 0xF0) == 0xE0) {
            return 3;
        }
        if ((c & 0xF8) == 0xF0) {
            return 4;
        }
        DOCTEST_INTERNAL_ERROR("Invalid multibyte utf-8 start byte encountered");
    }

    uint32_t headerValue(unsigned char c) {
        if ((c & 0xE0) == 0xC0) {
            return c & 0x1F;
        }
        if ((c & 0xF0) == 0xE0) {
            return c & 0x0F;
        }
        if ((c & 0xF8) == 0xF0) {
            return c & 0x07;
        }
        DOCTEST_INTERNAL_ERROR("Invalid multibyte utf-8 start byte encountered");
    }

    void hexEscapeChar(std::ostream& os, unsigned char c) {
        std::ios_base::fmtflags f(os.flags());
        os << "\\x"
            << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(c);
        os.flags(f);
    }

} // anonymous namespace

    XmlEncode::XmlEncode( std::string const& str, ForWhat forWhat )
    :   m_str( str ),
        m_forWhat( forWhat )
    {}

    void XmlEncode::encodeTo( std::ostream& os ) const {
        // Apostrophe escaping not necessary if we always use " to write attributes
        // (see: https://www.w3.org/TR/xml/#syntax)

        for( std::size_t idx = 0; idx < m_str.size(); ++ idx ) {
            uchar c = m_str[idx];
            switch (c) {
            case '<':   os << "&lt;"; break;
            case '&':   os << "&amp;"; break;

            case '>':
                // See: https://www.w3.org/TR/xml/#syntax
                if (idx > 2 && m_str[idx - 1] == ']' && m_str[idx - 2] == ']')
                    os << "&gt;";
                else
                    os << c;
                break;

            case '\"':
                if (m_forWhat == ForAttributes)
                    os << "&quot;";
                else
                    os << c;
                break;

            default:
                // Check for control characters and invalid utf-8

                // Escape control characters in standard ascii
                // see https://stackoverflow.com/questions/404107/why-are-control-characters-illegal-in-xml-1-0
                if (c < 0x09 || (c > 0x0D && c < 0x20) || c == 0x7F) {
                    hexEscapeChar(os, c);
                    break;
                }

                // Plain ASCII: Write it to stream
                if (c < 0x7F) {
                    os << c;
                    break;
                }

                // UTF-8 territory
                // Check if the encoding is valid and if it is not, hex escape bytes.
                // Important: We do not check the exact decoded values for validity, only the encoding format
                // First check that this bytes is a valid lead byte:
                // This means that it is not encoded as 1111 1XXX
                // Or as 10XX XXXX
                if (c <  0xC0 ||
                    c >= 0xF8) {
                    hexEscapeChar(os, c);
                    break;
                }

                auto encBytes = trailingBytes(c);
                // Are there enough bytes left to avoid accessing out-of-bounds memory?
                if (idx + encBytes - 1 >= m_str.size()) {
                    hexEscapeChar(os, c);
                    break;
                }
                // The header is valid, check data
                // The next encBytes bytes must together be a valid utf-8
                // This means: bitpattern 10XX XXXX and the extracted value is sane (ish)
                bool valid = true;
                uint32_t value = headerValue(c);
                for (std::size_t n = 1; n < encBytes; ++n) {
                    uchar nc = m_str[idx + n];
                    valid &= ((nc & 0xC0) == 0x80);
                    value = (value << 6) | (nc & 0x3F);
                }

                if (
                    // Wrong bit pattern of following bytes
                    (!valid) ||
                    // Overlong encodings
                    (value < 0x80) ||
                    (                 value < 0x800   && encBytes > 2) || // removed "0x80 <= value &&" because redundant
                    (0x800 < value && value < 0x10000 && encBytes > 3) ||
                    // Encoded value out of range
                    (value >= 0x110000)
                    ) {
                    hexEscapeChar(os, c);
                    break;
                }

                // If we got here, this is in fact a valid(ish) utf-8 sequence
                for (std::size_t n = 0; n < encBytes; ++n) {
                    os << m_str[idx + n];
                }
                idx += encBytes - 1;
                break;
            }
        }
    }

    std::ostream& operator << ( std::ostream& os, XmlEncode const& xmlEncode ) {
        xmlEncode.encodeTo( os );
        return os;
    }

    XmlWriter::ScopedElement::ScopedElement( XmlWriter* writer )
    :   m_writer( writer )
    {}

    XmlWriter::ScopedElement::ScopedElement( ScopedElement&& other ) noexcept
    :   m_writer( other.m_writer ){
        other.m_writer = nullptr;
    }
    XmlWriter::ScopedElement& XmlWriter::ScopedElement::operator=( ScopedElement&& other ) noexcept {
        if ( m_writer ) {
            m_writer->endElement();
        }
        m_writer = other.m_writer;
        other.m_writer = nullptr;
        return *this;
    }


    XmlWriter::ScopedElement::~ScopedElement() {
        if( m_writer )
            m_writer->endElement();
    }

    XmlWriter::ScopedElement& XmlWriter::ScopedElement::writeText( std::string const& text, bool indent ) {
        m_writer->writeText( text, indent );
        return *this;
    }

    XmlWriter::XmlWriter( std::ostream& os ) : m_os( os )
    {
        writeDeclaration();
    }

    XmlWriter::~XmlWriter() {
        while( !m_tags.empty() )
            endElement();
    }

    XmlWriter& XmlWriter::startElement( std::string const& name ) {
        ensureTagClosed();
        newlineIfNecessary();
        m_os << m_indent << '<' << name;
        m_tags.push_back( name );
        m_indent += "  ";
        m_tagIsOpen = true;
        return *this;
    }

    XmlWriter::ScopedElement XmlWriter::scopedElement( std::string const& name ) {
        ScopedElement scoped( this );
        startElement( name );
        return scoped;
    }

    XmlWriter& XmlWriter::endElement() {
        newlineIfNecessary();
        m_indent = m_indent.substr( 0, m_indent.size()-2 );
        if( m_tagIsOpen ) {
            m_os << "/>";
            m_tagIsOpen = false;
        }
        else {
            m_os << m_indent << "</" << m_tags.back() << ">";
        }
        m_os << std::endl;
        m_tags.pop_back();
        return *this;
    }

    XmlWriter& XmlWriter::writeAttribute( std::string const& name, std::string const& attribute ) {
        if( !name.empty() && !attribute.empty() )
            m_os << ' ' << name << "=\"" << XmlEncode( attribute, XmlEncode::ForAttributes ) << '"';
        return *this;
    }

    XmlWriter& XmlWriter::writeAttribute( std::string const& name, const char* attribute ) {
        if( !name.empty() && attribute && attribute[0] != '\0' )
            m_os << ' ' << name << "=\"" << XmlEncode( attribute, XmlEncode::ForAttributes ) << '"';
        return *this;
    }

    XmlWriter& XmlWriter::writeAttribute( std::string const& name, bool attribute ) {
        m_os << ' ' << name << "=\"" << ( attribute ? "true" : "false" ) << '"';
        return *this;
    }

    XmlWriter& XmlWriter::writeText( std::string const& text, bool indent ) {
        if( !text.empty() ){
            bool tagWasOpen = m_tagIsOpen;
            ensureTagClosed();
            if( tagWasOpen && indent )
                m_os << m_indent;
            m_os << XmlEncode( text );
            m_needsNewline = true;
        }
        return *this;
    }

    //XmlWriter& XmlWriter::writeComment( std::string const& text ) {
    //    ensureTagClosed();
    //    m_os << m_indent << "<!--" << text << "-->";
    //    m_needsNewline = true;
    //    return *this;
    //}

    //void XmlWriter::writeStylesheetRef( std::string const& url ) {
    //    m_os << "<?xml-stylesheet type=\"text/xsl\" href=\"" << url << "\"?>\n";
    //}

    //XmlWriter& XmlWriter::writeBlankLine() {
    //    ensureTagClosed();
    //    m_os << '\n';
    //    return *this;
    //}

    void XmlWriter::ensureTagClosed() {
        if( m_tagIsOpen ) {
            m_os << ">" << std::endl;
            m_tagIsOpen = false;
        }
    }

    void XmlWriter::writeDeclaration() {
        m_os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    }

    void XmlWriter::newlineIfNecessary() {
        if( m_needsNewline ) {
            m_os << std::endl;
            m_needsNewline = false;
        }
    }

// =================================================================================================
// End of copy-pasted code from Catch
// =================================================================================================

    // clang-format on

    struct XmlReporter : public IReporter
    {
        XmlWriter  xml;
        std::mutex mutex;

        // caching pointers/references to objects of these types - safe to do
        const ContextOptions& opt;
        const TestCaseData*   tc = nullptr;

        XmlReporter(const ContextOptions& co)
                : xml(*co.cout)
                , opt(co) {}

        void log_contexts() {
            int num_contexts = get_num_active_contexts();
            if(num_contexts) {
                auto              contexts = get_active_contexts();
                std::stringstream ss;
                for(int i = 0; i < num_contexts; ++i) {
                    contexts[i]->stringify(&ss);
                    xml.scopedElement("Info").writeText(ss.str());
                    ss.str("");
                }
            }
        }

        unsigned line(unsigned l) const { return opt.no_line_numbers ? 0 : l; }

        void test_case_start_impl(const TestCaseData& in) {
            bool open_ts_tag = false;
            if(tc != nullptr) { // we have already opened a test suite
                if(std::strcmp(tc->m_test_suite, in.m_test_suite) != 0) {
                    xml.endElement();
                    open_ts_tag = true;
                }
            }
            else {
                open_ts_tag = true; // first test case ==> first test suite
            }

            if(open_ts_tag) {
                xml.startElement("TestSuite");
                xml.writeAttribute("name", in.m_test_suite);
            }

            tc = &in;
            xml.startElement("TestCase")
                    .writeAttribute("name", in.m_name)
                    .writeAttribute("filename", skipPathFromFilename(in.m_file))
                    .writeAttribute("line", line(in.m_line))
                    .writeAttribute("description", in.m_description);

            if(Approx(in.m_timeout) != 0)
                xml.writeAttribute("timeout", in.m_timeout);
            if(in.m_may_fail)
                xml.writeAttribute("may_fail", true);
            if(in.m_should_fail)
                xml.writeAttribute("should_fail", true);
        }

        // =========================================================================================
        // WHAT FOLLOWS ARE OVERRIDES OF THE VIRTUAL METHODS OF THE REPORTER INTERFACE
        // =========================================================================================

        void report_query(const QueryData& in) override {
            test_run_start();
            if(opt.list_reporters) {
                for(auto& curr : getListeners())
                    xml.scopedElement("Listener")
                            .writeAttribute("priority", curr.first.first)
                            .writeAttribute("name", curr.first.second);
                for(auto& curr : getReporters())
                    xml.scopedElement("Reporter")
                            .writeAttribute("priority", curr.first.first)
                            .writeAttribute("name", curr.first.second);
            } else if(opt.count || opt.list_test_cases) {
                for(unsigned i = 0; i < in.num_data; ++i) {
                    xml.scopedElement("TestCase").writeAttribute("name", in.data[i]->m_name)
                        .writeAttribute("testsuite", in.data[i]->m_test_suite)
                        .writeAttribute("filename", skipPathFromFilename(in.data[i]->m_file))
                        .writeAttribute("line", line(in.data[i]->m_line));
                }
                xml.scopedElement("OverallResultsTestCases")
                        .writeAttribute("unskipped", in.run_stats->numTestCasesPassingFilters);
            } else if(opt.list_test_suites) {
                for(unsigned i = 0; i < in.num_data; ++i)
                    xml.scopedElement("TestSuite").writeAttribute("name", in.data[i]->m_test_suite);
                xml.scopedElement("OverallResultsTestCases")
                        .writeAttribute("unskipped", in.run_stats->numTestCasesPassingFilters);
                xml.scopedElement("OverallResultsTestSuites")
                        .writeAttribute("unskipped", in.run_stats->numTestSuitesPassingFilters);
            }
            xml.endElement();
        }

        void test_run_start() override {
            // remove .exe extension - mainly to have the same output on UNIX and Windows
            std::string binary_name = skipPathFromFilename(opt.binary_name.c_str());
#ifdef DOCTEST_PLATFORM_WINDOWS
            if(binary_name.rfind(".exe") != std::string::npos)
                binary_name = binary_name.substr(0, binary_name.length() - 4);
#endif // DOCTEST_PLATFORM_WINDOWS

            xml.startElement("doctest").writeAttribute("binary", binary_name);
            if(opt.no_version == false)
                xml.writeAttribute("version", DOCTEST_VERSION_STR);

            // only the consequential ones (TODO: filters)
            xml.scopedElement("Options")
                    .writeAttribute("order_by", opt.order_by.c_str())
                    .writeAttribute("rand_seed", opt.rand_seed)
                    .writeAttribute("first", opt.first)
                    .writeAttribute("last", opt.last)
                    .writeAttribute("abort_after", opt.abort_after)
                    .writeAttribute("subcase_filter_levels", opt.subcase_filter_levels)
                    .writeAttribute("case_sensitive", opt.case_sensitive)
                    .writeAttribute("no_throw", opt.no_throw)
                    .writeAttribute("no_skip", opt.no_skip);
        }

        void test_run_end(const TestRunStats& p) override {
            if(tc) // the TestSuite tag - only if there has been at least 1 test case
                xml.endElement();

            xml.scopedElement("OverallResultsAsserts")
                    .writeAttribute("successes", p.numAsserts - p.numAssertsFailed)
                    .writeAttribute("failures", p.numAssertsFailed);

            xml.startElement("OverallResultsTestCases")
                    .writeAttribute("successes",
                                    p.numTestCasesPassingFilters - p.numTestCasesFailed)
                    .writeAttribute("failures", p.numTestCasesFailed);
            if(opt.no_skipped_summary == false)
                xml.writeAttribute("skipped", p.numTestCases - p.numTestCasesPassingFilters);
            xml.endElement();

            xml.endElement();
        }

        void test_case_start(const TestCaseData& in) override {
            test_case_start_impl(in);
            xml.ensureTagClosed();
        }
        
        void test_case_reenter(const TestCaseData&) override {}

        void test_case_end(const CurrentTestCaseStats& st) override {
            xml.startElement("OverallResultsAsserts")
                    .writeAttribute("successes",
                                    st.numAssertsCurrentTest - st.numAssertsFailedCurrentTest)
                    .writeAttribute("failures", st.numAssertsFailedCurrentTest);
            if(opt.duration)
                xml.writeAttribute("duration", st.seconds);
            if(tc->m_expected_failures)
                xml.writeAttribute("expected_failures", tc->m_expected_failures);
            xml.endElement();

            xml.endElement();
        }

        void test_case_exception(const TestCaseException& e) override {
            std::lock_guard<std::mutex> lock(mutex);

            xml.scopedElement("Exception")
                    .writeAttribute("crash", e.is_crash)
                    .writeText(e.error_string.c_str());
        }

        void subcase_start(const SubcaseSignature& in) override {
            std::lock_guard<std::mutex> lock(mutex);

            xml.startElement("SubCase")
                    .writeAttribute("name", in.m_name)
                    .writeAttribute("filename", skipPathFromFilename(in.m_file))
                    .writeAttribute("line", line(in.m_line));
            xml.ensureTagClosed();
        }

        void subcase_end() override { xml.endElement(); }

        void log_assert(const AssertData& rb) override {
            if(!rb.m_failed && !opt.success)
                return;

            std::lock_guard<std::mutex> lock(mutex);

            xml.startElement("Expression")
                    .writeAttribute("success", !rb.m_failed)
                    .writeAttribute("type", assertString(rb.m_at))
                    .writeAttribute("filename", skipPathFromFilename(rb.m_file))
                    .writeAttribute("line", line(rb.m_line));

            xml.scopedElement("Original").writeText(rb.m_expr);

            if(rb.m_threw)
                xml.scopedElement("Exception").writeText(rb.m_exception.c_str());

            if(rb.m_at & assertType::is_throws_as)
                xml.scopedElement("ExpectedException").writeText(rb.m_exception_type);
            if(rb.m_at & assertType::is_throws_with)
                xml.scopedElement("ExpectedExceptionString").writeText(rb.m_exception_string);
            if((rb.m_at & assertType::is_normal) && !rb.m_threw)
                xml.scopedElement("Expanded").writeText(rb.m_decomp.c_str());

            log_contexts();

            xml.endElement();
        }

        void log_message(const MessageData& mb) override {
            std::lock_guard<std::mutex> lock(mutex);

            xml.startElement("Message")
                    .writeAttribute("type", failureString(mb.m_severity))
                    .writeAttribute("filename", skipPathFromFilename(mb.m_file))
                    .writeAttribute("line", line(mb.m_line));

            xml.scopedElement("Text").writeText(mb.m_string.c_str());

            log_contexts();

            xml.endElement();
        }

        void test_case_skipped(const TestCaseData& in) override {
            if(opt.no_skipped_summary == false) {
                test_case_start_impl(in);
                xml.writeAttribute("skipped", "true");
                xml.endElement();
            }
        }
    };

    DOCTEST_REGISTER_REPORTER("xml", 0, XmlReporter);

    struct Whitespace
    {
        int nrSpaces;
        explicit Whitespace(int nr)
                : nrSpaces(nr) {}
    };

    std::ostream& operator<<(std::ostream& out, const Whitespace& ws) {
        if(ws.nrSpaces != 0)
            out << std::setw(ws.nrSpaces) << ' ';
        return out;
    }

    struct ConsoleReporter : public IReporter
    {
        std::ostream&                 s;
        bool                          hasLoggedCurrentTestStart;
        std::vector<SubcaseSignature> subcasesStack;
        std::mutex                    mutex;

        // caching pointers/references to objects of these types - safe to do
        const ContextOptions& opt;
        const TestCaseData*   tc;

        ConsoleReporter(const ContextOptions& co)
                : s(*co.cout)
                , opt(co) {}

        ConsoleReporter(const ContextOptions& co, std::ostream& ostr)
                : s(ostr)
                , opt(co) {}

        // =========================================================================================
        // WHAT FOLLOWS ARE HELPERS USED BY THE OVERRIDES OF THE VIRTUAL METHODS OF THE INTERFACE
        // =========================================================================================

        void separator_to_stream() {
            s << Color::Yellow
              << "==============================================================================="
                 "\n";
        }

        const char* getSuccessOrFailString(bool success, assertType::Enum at,
                                           const char* success_str) {
            if(success)
                return success_str;
            return failureString(at);
        }

        Color::Enum getSuccessOrFailColor(bool success, assertType::Enum at) {
            return success ? Color::BrightGreen :
                             (at & assertType::is_warn) ? Color::Yellow : Color::Red;
        }

        void successOrFailColoredStringToStream(bool success, assertType::Enum at,
                                                const char* success_str = "SUCCESS") {
            s << getSuccessOrFailColor(success, at)
              << getSuccessOrFailString(success, at, success_str) << ": ";
        }

        void log_contexts() {
            int num_contexts = get_num_active_contexts();
            if(num_contexts) {
                auto contexts = get_active_contexts();

                s << Color::None << "  logged: ";
                for(int i = 0; i < num_contexts; ++i) {
                    s << (i == 0 ? "" : "          ");
                    contexts[i]->stringify(&s);
                    s << "\n";
                }
            }

            s << "\n";
        }

        void logTestStart() {
            if(hasLoggedCurrentTestStart)
                return;

            separator_to_stream();
            file_line_to_stream(s, tc->m_file, tc->m_line, "\n");
            if(tc->m_description)
                s << Color::Yellow << "DESCRIPTION: " << Color::None << tc->m_description << "\n";
            if(tc->m_test_suite && tc->m_test_suite[0] != '\0')
                s << Color::Yellow << "TEST SUITE: " << Color::None << tc->m_test_suite << "\n";
            if(strncmp(tc->m_name, "  Scenario:", 11) != 0)
                s << Color::None << "TEST CASE:  ";
            s << Color::None << tc->m_name << "\n";

            for(auto& curr : subcasesStack)
                if(curr.m_name[0] != '\0')
                    s << "  " << curr.m_name << "\n";

            s << "\n";

            hasLoggedCurrentTestStart = true;
        }

        void printVersion() {
            if(opt.no_version == false)
                s << Color::Cyan << "[doctest] " << Color::None << "doctest version is \""
                  << DOCTEST_VERSION_STR << "\"\n";
        }

        void printIntro() {
            if(opt.no_intro == false) {
				printVersion();
				s << Color::Cyan << "[doctest] " << Color::None
				  << "run with \"--" DOCTEST_OPTIONS_PREFIX_DISPLAY "help\" for options\n";
            }
        }

        void printHelp() {
            int sizePrefixDisplay = static_cast<int>(strlen(DOCTEST_OPTIONS_PREFIX_DISPLAY));
            printVersion();
            // clang-format off
            s << Color::Cyan << "[doctest]\n" << Color::None;
            s << Color::Cyan << "[doctest] " << Color::None;
            s << "boolean values: \"1/on/yes/true\" or \"0/off/no/false\"\n";
            s << Color::Cyan << "[doctest] " << Color::None;
            s << "filter  values: \"str1,str2,str3\" (comma separated strings)\n";
            s << Color::Cyan << "[doctest]\n" << Color::None;
            s << Color::Cyan << "[doctest] " << Color::None;
            s << "filters use wildcards for matching strings\n";
            s << Color::Cyan << "[doctest] " << Color::None;
            s << "something passes a filter if any of the strings in a filter matches\n";
#ifndef DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
            s << Color::Cyan << "[doctest]\n" << Color::None;
            s << Color::Cyan << "[doctest] " << Color::None;
            s << "ALL FLAGS, OPTIONS AND FILTERS ALSO AVAILABLE WITH A \"" DOCTEST_CONFIG_OPTIONS_PREFIX "\" PREFIX!!!\n";
#endif
            s << Color::Cyan << "[doctest]\n" << Color::None;
            s << Color::Cyan << "[doctest] " << Color::None;
            s << "Query flags - the program quits after them. Available:\n\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "?,   --" DOCTEST_OPTIONS_PREFIX_DISPLAY "help, -" DOCTEST_OPTIONS_PREFIX_DISPLAY "h                      "
              << Whitespace(sizePrefixDisplay*0) <<  "prints this message\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "v,   --" DOCTEST_OPTIONS_PREFIX_DISPLAY "version                       "
              << Whitespace(sizePrefixDisplay*1) << "prints the version\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "c,   --" DOCTEST_OPTIONS_PREFIX_DISPLAY "count                         "
              << Whitespace(sizePrefixDisplay*1) << "prints the number of matching tests\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "ltc, --" DOCTEST_OPTIONS_PREFIX_DISPLAY "list-test-cases               "
              << Whitespace(sizePrefixDisplay*1) << "lists all matching tests by name\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "lts, --" DOCTEST_OPTIONS_PREFIX_DISPLAY "list-test-suites              "
              << Whitespace(sizePrefixDisplay*1) << "lists all matching test suites\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "lr,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "list-reporters                "
              << Whitespace(sizePrefixDisplay*1) << "lists all registered reporters\n\n";
            // ================================================================================== << 79
            s << Color::Cyan << "[doctest] " << Color::None;
            s << "The available <int>/<string> options/filters are:\n\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "tc,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "test-case=<filters>           "
              << Whitespace(sizePrefixDisplay*1) << "filters     tests by their name\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "tce, --" DOCTEST_OPTIONS_PREFIX_DISPLAY "test-case-exclude=<filters>   "
              << Whitespace(sizePrefixDisplay*1) << "filters OUT tests by their name\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "sf,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "source-file=<filters>         "
              << Whitespace(sizePrefixDisplay*1) << "filters     tests by their file\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "sfe, --" DOCTEST_OPTIONS_PREFIX_DISPLAY "source-file-exclude=<filters> "
              << Whitespace(sizePrefixDisplay*1) << "filters OUT tests by their file\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "ts,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "test-suite=<filters>          "
              << Whitespace(sizePrefixDisplay*1) << "filters     tests by their test suite\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "tse, --" DOCTEST_OPTIONS_PREFIX_DISPLAY "test-suite-exclude=<filters>  "
              << Whitespace(sizePrefixDisplay*1) << "filters OUT tests by their test suite\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "sc,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "subcase=<filters>             "
              << Whitespace(sizePrefixDisplay*1) << "filters     subcases by their name\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "sce, --" DOCTEST_OPTIONS_PREFIX_DISPLAY "subcase-exclude=<filters>     "
              << Whitespace(sizePrefixDisplay*1) << "filters OUT subcases by their name\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "r,   --" DOCTEST_OPTIONS_PREFIX_DISPLAY "reporters=<filters>           "
              << Whitespace(sizePrefixDisplay*1) << "reporters to use (console is default)\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "o,   --" DOCTEST_OPTIONS_PREFIX_DISPLAY "out=<string>                  "
              << Whitespace(sizePrefixDisplay*1) << "output filename\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "ob,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "order-by=<string>             "
              << Whitespace(sizePrefixDisplay*1) << "how the tests should be ordered\n";
            s << Whitespace(sizePrefixDisplay*3) << "                                       <string> - by [file/suite/name/rand]\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "rs,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "rand-seed=<int>               "
              << Whitespace(sizePrefixDisplay*1) << "seed for random ordering\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "f,   --" DOCTEST_OPTIONS_PREFIX_DISPLAY "first=<int>                   "
              << Whitespace(sizePrefixDisplay*1) << "the first test passing the filters to\n";
            s << Whitespace(sizePrefixDisplay*3) << "                                       execute - for range-based execution\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "l,   --" DOCTEST_OPTIONS_PREFIX_DISPLAY "last=<int>                    "
              << Whitespace(sizePrefixDisplay*1) << "the last test passing the filters to\n";
            s << Whitespace(sizePrefixDisplay*3) << "                                       execute - for range-based execution\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "aa,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "abort-after=<int>             "
              << Whitespace(sizePrefixDisplay*1) << "stop after <int> failed assertions\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "scfl,--" DOCTEST_OPTIONS_PREFIX_DISPLAY "subcase-filter-levels=<int>   "
              << Whitespace(sizePrefixDisplay*1) << "apply filters for the first <int> levels\n";
            s << Color::Cyan << "\n[doctest] " << Color::None;
            s << "Bool options - can be used like flags and true is assumed. Available:\n\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "s,   --" DOCTEST_OPTIONS_PREFIX_DISPLAY "success=<bool>                "
              << Whitespace(sizePrefixDisplay*1) << "include successful assertions in output\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "cs,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "case-sensitive=<bool>         "
              << Whitespace(sizePrefixDisplay*1) << "filters being treated as case sensitive\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "e,   --" DOCTEST_OPTIONS_PREFIX_DISPLAY "exit=<bool>                   "
              << Whitespace(sizePrefixDisplay*1) << "exits after the tests finish\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "d,   --" DOCTEST_OPTIONS_PREFIX_DISPLAY "duration=<bool>               "
              << Whitespace(sizePrefixDisplay*1) << "prints the time duration of each test\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "nt,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "no-throw=<bool>               "
              << Whitespace(sizePrefixDisplay*1) << "skips exceptions-related assert checks\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "ne,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "no-exitcode=<bool>            "
              << Whitespace(sizePrefixDisplay*1) << "returns (or exits) always with success\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "nr,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "no-run=<bool>                 "
              << Whitespace(sizePrefixDisplay*1) << "skips all runtime doctest operations\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "nv,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "no-version=<bool>             "
              << Whitespace(sizePrefixDisplay*1) << "omit the framework version in the output\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "nv,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "no-intro=<bool>               "
              << Whitespace(sizePrefixDisplay*1) << "don't print any intro when running the test binary\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "nc,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "no-colors=<bool>              "
              << Whitespace(sizePrefixDisplay*1) << "disables colors in output\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "fc,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "force-colors=<bool>           "
              << Whitespace(sizePrefixDisplay*1) << "use colors even when not in a tty\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "nb,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "no-breaks=<bool>              "
              << Whitespace(sizePrefixDisplay*1) << "disables breakpoints in debuggers\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "ns,  --" DOCTEST_OPTIONS_PREFIX_DISPLAY "no-skip=<bool>                "
              << Whitespace(sizePrefixDisplay*1) << "don't skip test cases marked as skip\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "gfl, --" DOCTEST_OPTIONS_PREFIX_DISPLAY "gnu-file-line=<bool>          "
              << Whitespace(sizePrefixDisplay*1) << ":n: vs (n): for line numbers in output\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "npf, --" DOCTEST_OPTIONS_PREFIX_DISPLAY "no-path-filenames=<bool>      "
              << Whitespace(sizePrefixDisplay*1) << "only filenames and no paths in output\n";
            s << " -" DOCTEST_OPTIONS_PREFIX_DISPLAY "nln, --" DOCTEST_OPTIONS_PREFIX_DISPLAY "no-line-numbers=<bool>        "
              << Whitespace(sizePrefixDisplay*1) << "0 instead of real line numbers in output\n";
            // ================================================================================== << 79
            // clang-format on

            s << Color::Cyan << "\n[doctest] " << Color::None;
            s << "for more information visit the project documentation\n\n";
        }

        void printRegisteredReporters() {
            printVersion();
            auto printReporters = [this] (const reporterMap& reporters, const char* type) {
                if(reporters.size()) {
                    s << Color::Cyan << "[doctest] " << Color::None << "listing all registered " << type << "\n";
                    for(auto& curr : reporters)
                        s << "priority: " << std::setw(5) << curr.first.first
                          << " name: " << curr.first.second << "\n";
                }
            };
            printReporters(getListeners(), "listeners");
            printReporters(getReporters(), "reporters");
        }

        void list_query_results() {
            separator_to_stream();
            if(opt.count || opt.list_test_cases) {
                s << Color::Cyan << "[doctest] " << Color::None
                  << "unskipped test cases passing the current filters: "
                  << g_cs->numTestCasesPassingFilters << "\n";
            } else if(opt.list_test_suites) {
                s << Color::Cyan << "[doctest] " << Color::None
                  << "unskipped test cases passing the current filters: "
                  << g_cs->numTestCasesPassingFilters << "\n";
                s << Color::Cyan << "[doctest] " << Color::None
                  << "test suites with unskipped test cases passing the current filters: "
                  << g_cs->numTestSuitesPassingFilters << "\n";
            }
        }

        // =========================================================================================
        // WHAT FOLLOWS ARE OVERRIDES OF THE VIRTUAL METHODS OF THE REPORTER INTERFACE
        // =========================================================================================

        void report_query(const QueryData& in) override {
            if(opt.version) {
                printVersion();
            } else if(opt.help) {
                printHelp();
            } else if(opt.list_reporters) {
                printRegisteredReporters();
            } else if(opt.count || opt.list_test_cases) {
                if(opt.list_test_cases) {
                    s << Color::Cyan << "[doctest] " << Color::None
                      << "listing all test case names\n";
                    separator_to_stream();
                }

                for(unsigned i = 0; i < in.num_data; ++i)
                    s << Color::None << in.data[i]->m_name << "\n";

                separator_to_stream();

                s << Color::Cyan << "[doctest] " << Color::None
                  << "unskipped test cases passing the current filters: "
                  << g_cs->numTestCasesPassingFilters << "\n";

            } else if(opt.list_test_suites) {
                s << Color::Cyan << "[doctest] " << Color::None << "listing all test suites\n";
                separator_to_stream();

                for(unsigned i = 0; i < in.num_data; ++i)
                    s << Color::None << in.data[i]->m_test_suite << "\n";

                separator_to_stream();

                s << Color::Cyan << "[doctest] " << Color::None
                  << "unskipped test cases passing the current filters: "
                  << g_cs->numTestCasesPassingFilters << "\n";
                s << Color::Cyan << "[doctest] " << Color::None
                  << "test suites with unskipped test cases passing the current filters: "
                  << g_cs->numTestSuitesPassingFilters << "\n";
            }
        }

        void test_run_start() override { printIntro(); }

        void test_run_end(const TestRunStats& p) override {
            separator_to_stream();
            s << std::dec;

            const bool anythingFailed = p.numTestCasesFailed > 0 || p.numAssertsFailed > 0;
            s << Color::Cyan << "[doctest] " << Color::None << "test cases: " << std::setw(6)
              << p.numTestCasesPassingFilters << " | "
              << ((p.numTestCasesPassingFilters == 0 || anythingFailed) ? Color::None :
                                                                          Color::Green)
              << std::setw(6) << p.numTestCasesPassingFilters - p.numTestCasesFailed << " passed"
              << Color::None << " | " << (p.numTestCasesFailed > 0 ? Color::Red : Color::None)
              << std::setw(6) << p.numTestCasesFailed << " failed" << Color::None << " | ";
            if(opt.no_skipped_summary == false) {
                const int numSkipped = p.numTestCases - p.numTestCasesPassingFilters;
                s << (numSkipped == 0 ? Color::None : Color::Yellow) << std::setw(6) << numSkipped
                  << " skipped" << Color::None;
            }
            s << "\n";
            s << Color::Cyan << "[doctest] " << Color::None << "assertions: " << std::setw(6)
              << p.numAsserts << " | "
              << ((p.numAsserts == 0 || anythingFailed) ? Color::None : Color::Green)
              << std::setw(6) << (p.numAsserts - p.numAssertsFailed) << " passed" << Color::None
              << " | " << (p.numAssertsFailed > 0 ? Color::Red : Color::None) << std::setw(6)
              << p.numAssertsFailed << " failed" << Color::None << " |\n";
            s << Color::Cyan << "[doctest] " << Color::None
              << "Status: " << (p.numTestCasesFailed > 0 ? Color::Red : Color::Green)
              << ((p.numTestCasesFailed > 0) ? "FAILURE!" : "SUCCESS!") << Color::None << std::endl;
        }

        void test_case_start(const TestCaseData& in) override {
            hasLoggedCurrentTestStart = false;
            tc                        = &in;
        }
        
        void test_case_reenter(const TestCaseData&) override {}

        void test_case_end(const CurrentTestCaseStats& st) override {
            // log the preamble of the test case only if there is something
            // else to print - something other than that an assert has failed
            if(opt.duration ||
               (st.failure_flags && st.failure_flags != TestCaseFailureReason::AssertFailure))
                logTestStart();

            if(opt.duration)
                s << Color::None << std::setprecision(6) << std::fixed << st.seconds
                  << " s: " << tc->m_name << "\n";

            if(st.failure_flags & TestCaseFailureReason::Timeout)
                s << Color::Red << "Test case exceeded time limit of " << std::setprecision(6)
                  << std::fixed << tc->m_timeout << "!\n";

            if(st.failure_flags & TestCaseFailureReason::ShouldHaveFailedButDidnt) {
                s << Color::Red << "Should have failed but didn't! Marking it as failed!\n";
            } else if(st.failure_flags & TestCaseFailureReason::ShouldHaveFailedAndDid) {
                s << Color::Yellow << "Failed as expected so marking it as not failed\n";
            } else if(st.failure_flags & TestCaseFailureReason::CouldHaveFailedAndDid) {
                s << Color::Yellow << "Allowed to fail so marking it as not failed\n";
            } else if(st.failure_flags & TestCaseFailureReason::DidntFailExactlyNumTimes) {
                s << Color::Red << "Didn't fail exactly " << tc->m_expected_failures
                  << " times so marking it as failed!\n";
            } else if(st.failure_flags & TestCaseFailureReason::FailedExactlyNumTimes) {
                s << Color::Yellow << "Failed exactly " << tc->m_expected_failures
                  << " times as expected so marking it as not failed!\n";
            }
            if(st.failure_flags & TestCaseFailureReason::TooManyFailedAsserts) {
                s << Color::Red << "Aborting - too many failed asserts!\n";
            }
            s << Color::None; // lgtm [cpp/useless-expression]
        }

        void test_case_exception(const TestCaseException& e) override {
            logTestStart();

            file_line_to_stream(s, tc->m_file, tc->m_line, " ");
            successOrFailColoredStringToStream(false, e.is_crash ? assertType::is_require :
                                                                   assertType::is_check);
            s << Color::Red << (e.is_crash ? "test case CRASHED: " : "test case THREW exception: ")
              << Color::Cyan << e.error_string << "\n";

            int num_stringified_contexts = get_num_stringified_contexts();
            if(num_stringified_contexts) {
                auto stringified_contexts = get_stringified_contexts();
                s << Color::None << "  logged: ";
                for(int i = num_stringified_contexts; i > 0; --i) {
                    s << (i == num_stringified_contexts ? "" : "          ")
                      << stringified_contexts[i - 1] << "\n";
                }
            }
            s << "\n" << Color::None;
        }

        void subcase_start(const SubcaseSignature& subc) override {
            std::lock_guard<std::mutex> lock(mutex);
            subcasesStack.push_back(subc);
            hasLoggedCurrentTestStart = false;
        }

        void subcase_end() override {
            std::lock_guard<std::mutex> lock(mutex);
            subcasesStack.pop_back();
            hasLoggedCurrentTestStart = false;
        }

        void log_assert(const AssertData& rb) override {
            if(!rb.m_failed && !opt.success)
                return;

            std::lock_guard<std::mutex> lock(mutex);

            logTestStart();

            file_line_to_stream(s, rb.m_file, rb.m_line, " ");
            successOrFailColoredStringToStream(!rb.m_failed, rb.m_at);
            if((rb.m_at & (assertType::is_throws_as | assertType::is_throws_with)) ==
               0) //!OCLINT bitwise operator in conditional
                s << Color::Cyan << assertString(rb.m_at) << "( " << rb.m_expr << " ) "
                  << Color::None;

            if(rb.m_at & assertType::is_throws) { //!OCLINT bitwise operator in conditional
                s << (rb.m_threw ? "threw as expected!" : "did NOT throw at all!") << "\n";
            } else if((rb.m_at & assertType::is_throws_as) &&
                      (rb.m_at & assertType::is_throws_with)) { //!OCLINT
                s << Color::Cyan << assertString(rb.m_at) << "( " << rb.m_expr << ", \""
                  << rb.m_exception_string << "\", " << rb.m_exception_type << " ) " << Color::None;
                if(rb.m_threw) {
                    if(!rb.m_failed) {
                        s << "threw as expected!\n";
                    } else {
                        s << "threw a DIFFERENT exception! (contents: " << rb.m_exception << ")\n";
                    }
                } else {
                    s << "did NOT throw at all!\n";
                }
            } else if(rb.m_at &
                      assertType::is_throws_as) { //!OCLINT bitwise operator in conditional
                s << Color::Cyan << assertString(rb.m_at) << "( " << rb.m_expr << ", "
                  << rb.m_exception_type << " ) " << Color::None
                  << (rb.m_threw ? (rb.m_threw_as ? "threw as expected!" :
                                                    "threw a DIFFERENT exception: ") :
                                   "did NOT throw at all!")
                  << Color::Cyan << rb.m_exception << "\n";
            } else if(rb.m_at &
                      assertType::is_throws_with) { //!OCLINT bitwise operator in conditional
                s << Color::Cyan << assertString(rb.m_at) << "( " << rb.m_expr << ", \""
                  << rb.m_exception_string << "\" ) " << Color::None
                  << (rb.m_threw ? (!rb.m_failed ? "threw as expected!" :
                                                   "threw a DIFFERENT exception: ") :
                                   "did NOT throw at all!")
                  << Color::Cyan << rb.m_exception << "\n";
            } else if(rb.m_at & assertType::is_nothrow) { //!OCLINT bitwise operator in conditional
                s << (rb.m_threw ? "THREW exception: " : "didn't throw!") << Color::Cyan
                  << rb.m_exception << "\n";
            } else {
                s << (rb.m_threw ? "THREW exception: " :
                                   (!rb.m_failed ? "is correct!\n" : "is NOT correct!\n"));
                if(rb.m_threw)
                    s << rb.m_exception << "\n";
                else
                    s << "  values: " << assertString(rb.m_at) << "( " << rb.m_decomp << " )\n";
            }

            log_contexts();
        }

        void log_message(const MessageData& mb) override {
            std::lock_guard<std::mutex> lock(mutex);

            logTestStart();

            file_line_to_stream(s, mb.m_file, mb.m_line, " ");
            s << getSuccessOrFailColor(false, mb.m_severity)
              << getSuccessOrFailString(mb.m_severity & assertType::is_warn, mb.m_severity,
                                        "MESSAGE") << ": ";
            s << Color::None << mb.m_string << "\n";
            log_contexts();
        }

        void test_case_skipped(const TestCaseData&) override {}
    };

    DOCTEST_REGISTER_REPORTER("console", 0, ConsoleReporter);

#ifdef DOCTEST_PLATFORM_WINDOWS
    struct DebugOutputWindowReporter : public ConsoleReporter
    {
        DOCTEST_THREAD_LOCAL static std::ostringstream oss;

        DebugOutputWindowReporter(const ContextOptions& co)
                : ConsoleReporter(co, oss) {}

#define DOCTEST_DEBUG_OUTPUT_REPORTER_OVERRIDE(func, type, arg)                                    \
    void func(type arg) override {                                                                 \
        bool with_col = g_no_colors;                                                               \
        g_no_colors   = false;                                                                     \
        ConsoleReporter::func(arg);                                                                \
        DOCTEST_OUTPUT_DEBUG_STRING(oss.str().c_str());                                            \
        oss.str("");                                                                               \
        g_no_colors = with_col;                                                                    \
    }

        DOCTEST_DEBUG_OUTPUT_REPORTER_OVERRIDE(test_run_start, DOCTEST_EMPTY, DOCTEST_EMPTY)
        DOCTEST_DEBUG_OUTPUT_REPORTER_OVERRIDE(test_run_end, const TestRunStats&, in)
        DOCTEST_DEBUG_OUTPUT_REPORTER_OVERRIDE(test_case_start, const TestCaseData&, in)
        DOCTEST_DEBUG_OUTPUT_REPORTER_OVERRIDE(test_case_reenter, const TestCaseData&, in)
        DOCTEST_DEBUG_OUTPUT_REPORTER_OVERRIDE(test_case_end, const CurrentTestCaseStats&, in)
        DOCTEST_DEBUG_OUTPUT_REPORTER_OVERRIDE(test_case_exception, const TestCaseException&, in)
        DOCTEST_DEBUG_OUTPUT_REPORTER_OVERRIDE(subcase_start, const SubcaseSignature&, in)
        DOCTEST_DEBUG_OUTPUT_REPORTER_OVERRIDE(subcase_end, DOCTEST_EMPTY, DOCTEST_EMPTY)
        DOCTEST_DEBUG_OUTPUT_REPORTER_OVERRIDE(log_assert, const AssertData&, in)
        DOCTEST_DEBUG_OUTPUT_REPORTER_OVERRIDE(log_message, const MessageData&, in)
        DOCTEST_DEBUG_OUTPUT_REPORTER_OVERRIDE(test_case_skipped, const TestCaseData&, in)
    };

    DOCTEST_THREAD_LOCAL std::ostringstream DebugOutputWindowReporter::oss;
#endif // DOCTEST_PLATFORM_WINDOWS

    // the implementation of parseOption()
    bool parseOptionImpl(int argc, const char* const* argv, const char* pattern, String* value) {
        // going from the end to the beginning and stopping on the first occurrence from the end
        for(int i = argc; i > 0; --i) {
            auto index = i - 1;
            auto temp = std::strstr(argv[index], pattern);
            if(temp && (value || strlen(temp) == strlen(pattern))) { //!OCLINT prefer early exits and continue
                // eliminate matches in which the chars before the option are not '-'
                bool noBadCharsFound = true;
                auto curr            = argv[index];
                while(curr != temp) {
                    if(*curr++ != '-') {
                        noBadCharsFound = false;
                        break;
                    }
                }
                if(noBadCharsFound && argv[index][0] == '-') {
                    if(value) {
                        // parsing the value of an option
                        temp += strlen(pattern);
                        const unsigned len = strlen(temp);
                        if(len) {
                            *value = temp;
                            return true;
                        }
                    } else {
                        // just a flag - no value
                        return true;
                    }
                }
            }
        }
        return false;
    }

    // parses an option and returns the string after the '=' character
    bool parseOption(int argc, const char* const* argv, const char* pattern, String* value = nullptr,
                     const String& defaultVal = String()) {
        if(value)
            *value = defaultVal;
#ifndef DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
        // offset (normally 3 for "dt-") to skip prefix
        if(parseOptionImpl(argc, argv, pattern + strlen(DOCTEST_CONFIG_OPTIONS_PREFIX), value))
            return true;
#endif // DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
        return parseOptionImpl(argc, argv, pattern, value);
    }

    // locates a flag on the command line
    bool parseFlag(int argc, const char* const* argv, const char* pattern) {
        return parseOption(argc, argv, pattern);
    }

    // parses a comma separated list of words after a pattern in one of the arguments in argv
    bool parseCommaSepArgs(int argc, const char* const* argv, const char* pattern,
                           std::vector<String>& res) {
        String filtersString;
        if(parseOption(argc, argv, pattern, &filtersString)) {
            // tokenize with "," as a separator
            // cppcheck-suppress strtokCalled
            DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wdeprecated-declarations")
            auto pch = std::strtok(filtersString.c_str(), ","); // modifies the string
            while(pch != nullptr) {
                if(strlen(pch))
                    res.push_back(pch);
                // uses the strtok() internal state to go to the next token
                // cppcheck-suppress strtokCalled
                pch = std::strtok(nullptr, ",");
            }
            DOCTEST_CLANG_SUPPRESS_WARNING_POP
            return true;
        }
        return false;
    }

    enum optionType
    {
        option_bool,
        option_int
    };

    // parses an int/bool option from the command line
    bool parseIntOption(int argc, const char* const* argv, const char* pattern, optionType type,
                        int& res) {
        String parsedValue;
        if(!parseOption(argc, argv, pattern, &parsedValue))
            return false;

        if(type == 0) {
            // boolean
            const char positive[][5] = {"1", "true", "on", "yes"};  // 5 - strlen("true") + 1
            const char negative[][6] = {"0", "false", "off", "no"}; // 6 - strlen("false") + 1

            // if the value matches any of the positive/negative possibilities
            for(unsigned i = 0; i < 4; i++) {
                if(parsedValue.compare(positive[i], true) == 0) {
                    res = 1; //!OCLINT parameter reassignment
                    return true;
                }
                if(parsedValue.compare(negative[i], true) == 0) {
                    res = 0; //!OCLINT parameter reassignment
                    return true;
                }
            }
        } else {
            // integer
            // TODO: change this to use std::stoi or something else! currently it uses undefined behavior - assumes '0' on failed parse...
            int theInt = std::atoi(parsedValue.c_str()); // NOLINT
            if(theInt != 0) {
                res = theInt; //!OCLINT parameter reassignment
                return true;
            }
        }
        return false;
    }
} // namespace

Context::Context(int argc, const char* const* argv)
        : p(new detail::ContextState) {
    parseArgs(argc, argv, true);
    if(argc)
        p->binary_name = argv[0];
}

Context::~Context() {
    if(g_cs == p)
        g_cs = nullptr;
    delete p;
}

void Context::applyCommandLine(int argc, const char* const* argv) {
    parseArgs(argc, argv);
    if(argc)
        p->binary_name = argv[0];
}

// parses args
void Context::parseArgs(int argc, const char* const* argv, bool withDefaults) {
    using namespace detail;

    // clang-format off
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "source-file=",        p->filters[0]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "sf=",                 p->filters[0]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "source-file-exclude=",p->filters[1]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "sfe=",                p->filters[1]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "test-suite=",         p->filters[2]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "ts=",                 p->filters[2]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "test-suite-exclude=", p->filters[3]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "tse=",                p->filters[3]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "test-case=",          p->filters[4]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "tc=",                 p->filters[4]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "test-case-exclude=",  p->filters[5]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "tce=",                p->filters[5]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "subcase=",            p->filters[6]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "sc=",                 p->filters[6]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "subcase-exclude=",    p->filters[7]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "sce=",                p->filters[7]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "reporters=",          p->filters[8]);
    parseCommaSepArgs(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "r=",                  p->filters[8]);
    // clang-format on

    int    intRes = 0;
    String strRes;

#define DOCTEST_PARSE_AS_BOOL_OR_FLAG(name, sname, var, default)                                   \
    if(parseIntOption(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX name "=", option_bool, intRes) ||  \
       parseIntOption(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX sname "=", option_bool, intRes))   \
        p->var = !!intRes;                                                                         \
    else if(parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX name) ||                           \
            parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX sname))                            \
        p->var = true;                                                                             \
    else if(withDefaults)                                                                          \
    p->var = default

#define DOCTEST_PARSE_INT_OPTION(name, sname, var, default)                                        \
    if(parseIntOption(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX name "=", option_int, intRes) ||   \
       parseIntOption(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX sname "=", option_int, intRes))    \
        p->var = intRes;                                                                           \
    else if(withDefaults)                                                                          \
    p->var = default

#define DOCTEST_PARSE_STR_OPTION(name, sname, var, default)                                        \
    if(parseOption(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX name "=", &strRes, default) ||        \
       parseOption(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX sname "=", &strRes, default) ||       \
       withDefaults)                                                                               \
    p->var = strRes

    // clang-format off
    DOCTEST_PARSE_STR_OPTION("out", "o", out, "");
    DOCTEST_PARSE_STR_OPTION("order-by", "ob", order_by, "file");
    DOCTEST_PARSE_INT_OPTION("rand-seed", "rs", rand_seed, 0);

    DOCTEST_PARSE_INT_OPTION("first", "f", first, 0);
    DOCTEST_PARSE_INT_OPTION("last", "l", last, UINT_MAX);

    DOCTEST_PARSE_INT_OPTION("abort-after", "aa", abort_after, 0);
    DOCTEST_PARSE_INT_OPTION("subcase-filter-levels", "scfl", subcase_filter_levels, INT_MAX);

    DOCTEST_PARSE_AS_BOOL_OR_FLAG("success", "s", success, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("case-sensitive", "cs", case_sensitive, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("exit", "e", exit, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("duration", "d", duration, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("no-throw", "nt", no_throw, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("no-exitcode", "ne", no_exitcode, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("no-run", "nr", no_run, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("no-version", "nv", no_version, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("no-intro", "nv", no_intro, true);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("no-colors", "nc", no_colors, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("force-colors", "fc", force_colors, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("no-breaks", "nb", no_breaks, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("no-skip", "ns", no_skip, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("gnu-file-line", "gfl", gnu_file_line, !bool(DOCTEST_MSVC));
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("no-path-filenames", "npf", no_path_in_filenames, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("no-line-numbers", "nln", no_line_numbers, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("no-skipped-summary", "nss", no_skipped_summary, false);
    // clang-format on

    if(withDefaults) {
        p->help             = false;
        p->version          = false;
        p->count            = false;
        p->list_test_cases  = false;
        p->list_test_suites = false;
        p->list_reporters   = false;
    }
    if(parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "help") ||
       parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "h") ||
       parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "?")) {
        p->help = true;
        p->exit = true;
    }
    if(parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "version") ||
       parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "v")) {
        p->version = true;
        p->exit    = true;
    }
    if(parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "count") ||
       parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "c")) {
        p->count = true;
        p->exit  = true;
    }
    if(parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "list-test-cases") ||
       parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "ltc")) {
        p->list_test_cases = true;
        p->exit            = true;
    }
    if(parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "list-test-suites") ||
       parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "lts")) {
        p->list_test_suites = true;
        p->exit             = true;
    }
    if(parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "list-reporters") ||
       parseFlag(argc, argv, DOCTEST_CONFIG_OPTIONS_PREFIX "lr")) {
        p->list_reporters = true;
        p->exit           = true;
    }
}

// allows the user to add procedurally to the filters from the command line
void Context::addFilter(const char* filter, const char* value) { setOption(filter, value); }

// allows the user to clear all filters from the command line
void Context::clearFilters() {
    for(auto& curr : p->filters)
        curr.clear();
}

// allows the user to override procedurally the int/bool options from the command line
void Context::setOption(const char* option, int value) {
    setOption(option, toString(value).c_str());
}

// allows the user to override procedurally the string options from the command line
void Context::setOption(const char* option, const char* value) {
    auto argv   = String("-") + option + "=" + value;
    auto lvalue = argv.c_str();
    parseArgs(1, &lvalue);
}

// users should query this in their main() and exit the program if true
bool Context::shouldExit() { return p->exit; }

void Context::setAsDefaultForAssertsOutOfTestCases() { g_cs = p; }

void Context::setAssertHandler(detail::assert_handler ah) { p->ah = ah; }

// the main function that does all the filtering and test running
int Context::run() {
    using namespace detail;

    // save the old context state in case such was setup - for using asserts out of a testing context
    auto old_cs = g_cs;
    // this is the current contest
    g_cs               = p;
    is_running_in_test = true;

    g_no_colors = p->no_colors;
    p->resetRunData();

    // stdout by default
    p->cout = &std::cout;
    p->cerr = &std::cerr;

    // or to a file if specified
    std::fstream fstr;
    if(p->out.size()) {
        fstr.open(p->out.c_str(), std::fstream::out);
        p->cout = &fstr;
    }

    auto cleanup_and_return = [&]() {
        if(fstr.is_open())
            fstr.close();

        // restore context
        g_cs               = old_cs;
        is_running_in_test = false;

        // we have to free the reporters which were allocated when the run started
        for(auto& curr : p->reporters_currently_used)
            delete curr;
        p->reporters_currently_used.clear();

        if(p->numTestCasesFailed && !p->no_exitcode)
            return EXIT_FAILURE;
        return EXIT_SUCCESS;
    };

    // setup default reporter if none is given through the command line
    if(p->filters[8].empty())
        p->filters[8].push_back("console");

    // check to see if any of the registered reporters has been selected
    for(auto& curr : getReporters()) {
        if(matchesAny(curr.first.second.c_str(), p->filters[8], false, p->case_sensitive))
            p->reporters_currently_used.push_back(curr.second(*g_cs));
    }

    // TODO: check if there is nothing in reporters_currently_used

    // prepend all listeners
    for(auto& curr : getListeners())
        p->reporters_currently_used.insert(p->reporters_currently_used.begin(), curr.second(*g_cs));

#ifdef DOCTEST_PLATFORM_WINDOWS
    if(isDebuggerActive())
        p->reporters_currently_used.push_back(new DebugOutputWindowReporter(*g_cs));
#endif // DOCTEST_PLATFORM_WINDOWS

    // handle version, help and no_run
    if(p->no_run || p->version || p->help || p->list_reporters) {
        DOCTEST_ITERATE_THROUGH_REPORTERS(report_query, QueryData());

        return cleanup_and_return();
    }

    std::vector<const TestCase*> testArray;
    for(auto& curr : getRegisteredTests())
        testArray.push_back(&curr);
    p->numTestCases = testArray.size();

    // sort the collected records
    if(!testArray.empty()) {
        if(p->order_by.compare("file", true) == 0) {
            std::sort(testArray.begin(), testArray.end(), fileOrderComparator);
        } else if(p->order_by.compare("suite", true) == 0) {
            std::sort(testArray.begin(), testArray.end(), suiteOrderComparator);
        } else if(p->order_by.compare("name", true) == 0) {
            std::sort(testArray.begin(), testArray.end(), nameOrderComparator);
        } else if(p->order_by.compare("rand", true) == 0) {
            std::srand(p->rand_seed);

            // random_shuffle implementation
            const auto first = &testArray[0];
            for(size_t i = testArray.size() - 1; i > 0; --i) {
                int idxToSwap = std::rand() % (i + 1); // NOLINT

                const auto temp = first[i];

                first[i]         = first[idxToSwap];
                first[idxToSwap] = temp;
            }
        }
    }

    std::set<String> testSuitesPassingFilt;

    bool                             query_mode = p->count || p->list_test_cases || p->list_test_suites;
    std::vector<const TestCaseData*> queryResults;

    if(!query_mode)
        DOCTEST_ITERATE_THROUGH_REPORTERS(test_run_start, DOCTEST_EMPTY);

    // invoke the registered functions if they match the filter criteria (or just count them)
    for(auto& curr : testArray) {
        const auto& tc = *curr;

        bool skip_me = false;
        if(tc.m_skip && !p->no_skip)
            skip_me = true;

        if(!matchesAny(tc.m_file, p->filters[0], true, p->case_sensitive))
            skip_me = true;
        if(matchesAny(tc.m_file, p->filters[1], false, p->case_sensitive))
            skip_me = true;
        if(!matchesAny(tc.m_test_suite, p->filters[2], true, p->case_sensitive))
            skip_me = true;
        if(matchesAny(tc.m_test_suite, p->filters[3], false, p->case_sensitive))
            skip_me = true;
        if(!matchesAny(tc.m_name, p->filters[4], true, p->case_sensitive))
            skip_me = true;
        if(matchesAny(tc.m_name, p->filters[5], false, p->case_sensitive))
            skip_me = true;

        if(!skip_me)
            p->numTestCasesPassingFilters++;

        // skip the test if it is not in the execution range
        if((p->last < p->numTestCasesPassingFilters && p->first <= p->last) ||
           (p->first > p->numTestCasesPassingFilters))
            skip_me = true;

        if(skip_me) {
            if(!query_mode)
                DOCTEST_ITERATE_THROUGH_REPORTERS(test_case_skipped, tc);
            continue;
        }

        // do not execute the test if we are to only count the number of filter passing tests
        if(p->count)
            continue;

        // print the name of the test and don't execute it
        if(p->list_test_cases) {
            queryResults.push_back(&tc);
            continue;
        }

        // print the name of the test suite if not done already and don't execute it
        if(p->list_test_suites) {
            if((testSuitesPassingFilt.count(tc.m_test_suite) == 0) && tc.m_test_suite[0] != '\0') {
                queryResults.push_back(&tc);
                testSuitesPassingFilt.insert(tc.m_test_suite);
                p->numTestSuitesPassingFilters++;
            }
            continue;
        }

        // execute the test if it passes all the filtering
        {
            p->currentTest = &tc;

            p->failure_flags = TestCaseFailureReason::None;
            p->seconds       = 0;

            // reset atomic counters
            p->numAssertsFailedCurrentTest_atomic = 0;
            p->numAssertsCurrentTest_atomic       = 0;

            p->subcasesPassed.clear();

            DOCTEST_ITERATE_THROUGH_REPORTERS(test_case_start, tc);

            p->timer.start();
            
            bool run_test = true;

            do {
                // reset some of the fields for subcases (except for the set of fully passed ones)
                p->should_reenter          = false;
                p->subcasesCurrentMaxLevel = 0;
                p->subcasesStack.clear();

                p->shouldLogCurrentException = true;

                // reset stuff for logging with INFO()
                p->stringifiedContexts.clear();

#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
                try {
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS
                    FatalConditionHandler fatalConditionHandler; // Handle signals
                    // execute the test
                    tc.m_test();
                    fatalConditionHandler.reset();
#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
                } catch(const TestFailureException&) {
                    p->failure_flags |= TestCaseFailureReason::AssertFailure;
                } catch(...) {
                    DOCTEST_ITERATE_THROUGH_REPORTERS(test_case_exception,
                                                      {translateActiveException(), false});
                    p->failure_flags |= TestCaseFailureReason::Exception;
                }
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS

                // exit this loop if enough assertions have failed - even if there are more subcases
                if(p->abort_after > 0 &&
                   p->numAssertsFailed + p->numAssertsFailedCurrentTest_atomic >= p->abort_after) {
                    run_test = false;
                    p->failure_flags |= TestCaseFailureReason::TooManyFailedAsserts;
                }
                
                if(p->should_reenter && run_test)
                    DOCTEST_ITERATE_THROUGH_REPORTERS(test_case_reenter, tc);
                if(!p->should_reenter)
                    run_test = false;
            } while(run_test);

            p->finalizeTestCaseData();

            DOCTEST_ITERATE_THROUGH_REPORTERS(test_case_end, *g_cs);

            p->currentTest = nullptr;

            // stop executing tests if enough assertions have failed
            if(p->abort_after > 0 && p->numAssertsFailed >= p->abort_after)
                break;
        }
    }

    if(!query_mode) {
        DOCTEST_ITERATE_THROUGH_REPORTERS(test_run_end, *g_cs);
    } else {
        QueryData qdata;
        qdata.run_stats = g_cs;
        qdata.data      = queryResults.data();
        qdata.num_data  = unsigned(queryResults.size());
        DOCTEST_ITERATE_THROUGH_REPORTERS(report_query, qdata);
    }

    // see these issues on the reasoning for this:
    // - https://github.com/onqtam/doctest/issues/143#issuecomment-414418903
    // - https://github.com/onqtam/doctest/issues/126
    auto DOCTEST_FIX_FOR_MACOS_LIBCPP_IOSFWD_STRING_LINK_ERRORS = []() DOCTEST_NOINLINE
        { std::cout << std::string(); };
    DOCTEST_FIX_FOR_MACOS_LIBCPP_IOSFWD_STRING_LINK_ERRORS();

    return cleanup_and_return();
}

IReporter::~IReporter() = default;

int IReporter::get_num_active_contexts() { return detail::g_infoContexts.size(); }
const IContextScope* const* IReporter::get_active_contexts() {
    return get_num_active_contexts() ? &detail::g_infoContexts[0] : nullptr;
}

int IReporter::get_num_stringified_contexts() { return detail::g_cs->stringifiedContexts.size(); }
const String* IReporter::get_stringified_contexts() {
    return get_num_stringified_contexts() ? &detail::g_cs->stringifiedContexts[0] : nullptr;
}

namespace detail {
    void registerReporterImpl(const char* name, int priority, reporterCreatorFunc c, bool isReporter) {
        if(isReporter)
            getReporters().insert(reporterMap::value_type(reporterMap::key_type(priority, name), c));
        else
            getListeners().insert(reporterMap::value_type(reporterMap::key_type(priority, name), c));
    }
} // namespace detail

} // namespace doctest

#endif // DOCTEST_CONFIG_DISABLE

#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4007) // 'function' : must be 'attribute' - see issue #182
int main(int argc, char** argv) { return doctest::Context(argc, argv).run(); }
DOCTEST_MSVC_SUPPRESS_WARNING_POP
#endif // DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

DOCTEST_CLANG_SUPPRESS_WARNING_POP
DOCTEST_MSVC_SUPPRESS_WARNING_POP
DOCTEST_GCC_SUPPRESS_WARNING_POP

#endif // DOCTEST_LIBRARY_IMPLEMENTATION
#endif // DOCTEST_CONFIG_IMPLEMENT