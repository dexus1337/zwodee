#pragma once

/**
 * @file    api.hpp
 * @author  dexus1337
 * @brief   Defines macros for controlling symbol visibility in the Zwodee Engine and platform/architecture detection.
 * @version 1.0
 * @date    21.06.2026
 */

#if not defined(ZWODEE_PLATFORM_LINUX) && not defined(ZWODEE_PLATFORM_WINDOWS) && not defined(ZWODEE_PLATFORM_APPLE)
    #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
        #define ZWODEE_PLATFORM_WINDOWS
    #elif defined(__linux__)
        #define ZWODEE_PLATFORM_LINUX
    #elif defined(__APPLE__)
        #define ZWODEE_PLATFORM_APPLE
    #endif
#endif

#if defined(ZWODEE_PLATFORM_WINDOWS)
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif

    #ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    #endif
    
    #ifdef ZWODEE_EXPORTS
        #define ZWODEE_API __declspec(dllexport)
    #else 
        #define ZWODEE_API __declspec(dllimport)
    #endif
#else
    #if __GNUC__ >= 4
        #define ZWODEE_API __attribute__ ((visibility ("default")))
    #else
        #define ZWODEE_API
    #endif
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
    #define ZWODEE_CPU_ARM64
#elif defined(__x86_64__) || defined(_M_X64)
    #define ZWODEE_CPU_X64
#endif

#if defined(ZWODEE_BUILD_DEBUG)
    #define debug_statement(x) x
#else
    #define debug_statement(x)
#endif

#if defined(_MSC_VER) && !defined(__clang__)
    #define ZWODEE_CONSTEVAL  constexpr
    #define ZWODEE_CONSTEXPR constexpr
#else
    #define ZWODEE_CONSTEVAL  consteval
    #define ZWODEE_CONSTEXPR constexpr
#endif
