#ifndef NV_NVFOUNDATION_NVINTRINSICS_H
#define NV_NVFOUNDATION_NVINTRINSICS_H

#include "NvPreprocessor.h"

#if NV_WINDOWS_FAMILY
#include "windows/NvWindowsIntrinsics.h"
#elif NV_X360
#include "xbox360/NvXbox360Intrinsics.h"
#elif(NV_LINUX || NV_ANDROID || NV_APPLE_FAMILY || NV_PS4)
#include "unix/NvUnixIntrinsics.h"
#elif NV_PS3
#include "ps3/NvPS3Intrinsics.h"
#elif NV_PSP2
#include "psp2/NvPSP2Intrinsics.h"
#elif NV_WIIU
#include "wiiu/NvWiiUIntrinsics.h"
#elif NV_WII
#include "wii/NvWiiIntrinsics.h"
#elif NV_XBOXONE
#include "XboxOne/NvXboxOneIntrinsics.h"
#else
#error "Platform not supported!"
#endif

#endif // #ifndef NV_NVFOUNDATION_NVINTRINSICS_H
