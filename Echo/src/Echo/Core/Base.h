#pragma once

#ifndef PLATFORM_DETECTION_H
#define PLATFORM_DETECTION_H

// Windows
#if defined(_WIN32) || defined(_WIN64)
#define ECHO_PLATFORM_WIN

// macOS
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_OS_MAC == 1
#define ECHO_PLATFORM_MACOS
#endif

// Linux
#elif defined(__linux__)
#define ECHO_PLATFORM_LINUX

// Android
#elif defined(__ANDROID__)
#if defined(__ANDROID__)
#if defined(__ANDROID_API__)
#include <android/api-level.h>
#endif
#if __ANDROID_API__ >= 9
#if defined(__ANDROID__)
#define ECHO_PLATFORM_ANDROID
#if defined(__ANDROID_PHONE__)
#define ECHO_PLATFORM_ANDROIDPHONE
#elif defined(__ANDROID_TABLET__)
#define ECHO_PLATFORM_ANDROIDTABLET
#endif
#endif
#endif
#endif

// iOS
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE == 1
#define ECHO_PLATFORM_IOS
#if TARGET_OS_IPHONE && TARGET_OS_SIMULATOR == 0
#define ECHO_PLATFORM_IOSPHONE
#else
#define ECHO_PLATFORM_IOSTABLET
#endif
#endif

// Xbox
#elif defined(_XBOX_ONE) || defined(_XBOX)
#define ECHO_PLATFORM_XBOX

// PlayStation
#elif defined(__ORBIS__) || defined(__PROSPERO__)
#define ECHO_PLATFORM_PLAYSTATION

// Nintendo Switch
#elif defined(__NX__)
#define ECHO_PLATFORM_SWITCH

#else
#error "Unknown platform!"
#endif

#endif // PLATFORM_DETECTION_H

#define BIT(x) (1 << x)

#ifdef ECHO_DEBUG
	#define EC_ENABLE_ASSERTS
#endif

#ifdef EC_ENABLE_ASSERTS
	#define EC_ASSERT(x, ...) {if(!(x)) { EC_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
	#define EC_CORE_ASSERT(x, ...) {if(!(x)) { EC_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
#else 
	#define EC_ASSERT(x, ...)
	#define EC_CORE_ASSERT(x, ...)
#endif