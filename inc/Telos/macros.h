#ifndef MACROS_H
#define MACROS_H

#ifdef __cplusplus
// clang-format off
extern "C"
{
    namespace TELOS
    {
#endif

/********************** 导入导出宏 **********************/
#ifdef _WIN32
    #define TELOS_CDECL __cdecl
    #define TELOS_STDCALL __stdcall

    #if !defined(TELOS_HIDE_SYMBOLS) && !defined(TELOS_IMPORT_SYMBOLS) && !defined(TELOS_EXPORT_SYMBOLS)
        #define TELOS_EXPORT_SYMBOLS
    #endif

    #if defined(TELOS_HIDE_SYMBOLS)
        #define TELOS_PUBLIC TELOS_STDCALL
    #elif defined(TELOS_EXPORT_SYMBOLS)
        #define TELOS_PUBLIC __declspec(dllexport) TELOS_STDCALL
    #elif defined(TELOS_IMPORT_SYMBOLS)
        #define TELOS_PUBLIC __declspec(dllimport) TELOS_STDCALL
    #endif
#else
    #define TELOS_CDECL
    #define TELOS_STDCALL

    #if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__SUNPRO_C)) && defined(TELOS_API_VISIBILITY)
        #define TELOS_PUBLIC __attribute__((visibility("default")))
    #else
        #define TELOS_PUBLIC
    #endif
#endif

/********************** 弃用宏 **********************/

#if defined(__GNUC__) || defined(__clang__)
#define DEPRECATED(func, msg) func __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#define DEPRECATED(func, msg) __declspec(deprecated(msg)) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(func, msg) func
#endif

#ifdef __cplusplus
    }  // namespace TELOS
}
// clang-format on
#endif

/********************** 基本类型 **********************/

// #ifndef uintptr_t
// #define uintptr_t unsigned long long
// #endif

#endif  // MACROS_H
