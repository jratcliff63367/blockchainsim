#ifndef NV_WINDOWS_NVWINDOWSINTRINSICS_H
#define NV_WINDOWS_NVWINDOWSINTRINSICS_H

#include "NvAssert.h"

#if !NV_WINDOWS_FAMILY
#error "This file should only be included by Windows or WIN8ARM builds!!"
#endif

#include <math.h>
#include <float.h>

#if !NV_DOXYGEN
namespace blockchainsim
{
namespace intrinsics
{
#endif

//! \brief platform-specific absolute value
NV_CUDA_CALLABLE NV_FORCE_INLINE float abs(float a)
{
    return float(::fabs(a));
}

//! \brief platform-specific select float
NV_CUDA_CALLABLE NV_FORCE_INLINE float fsel(float a, float b, float c)
{
    return (a >= 0.0f) ? b : c;
}

//! \brief platform-specific sign
NV_CUDA_CALLABLE NV_FORCE_INLINE float sign(float a)
{
    return (a >= 0.0f) ? 1.0f : -1.0f;
}

//! \brief platform-specific reciprocal
NV_CUDA_CALLABLE NV_FORCE_INLINE float recip(float a)
{
    return 1.0f / a;
}

//! \brief platform-specific reciprocal estimate
NV_CUDA_CALLABLE NV_FORCE_INLINE float recipFast(float a)
{
    return 1.0f / a;
}

//! \brief platform-specific square root
NV_CUDA_CALLABLE NV_FORCE_INLINE float sqrt(float a)
{
    return ::sqrtf(a);
}

//! \brief platform-specific reciprocal square root
NV_CUDA_CALLABLE NV_FORCE_INLINE float recipSqrt(float a)
{
    return 1.0f / ::sqrtf(a);
}

//! \brief platform-specific reciprocal square root estimate
NV_CUDA_CALLABLE NV_FORCE_INLINE float recipSqrtFast(float a)
{
    return 1.0f / ::sqrtf(a);
}

//! \brief platform-specific sine
NV_CUDA_CALLABLE NV_FORCE_INLINE float sin(float a)
{
    return ::sinf(a);
}

//! \brief platform-specific cosine
NV_CUDA_CALLABLE NV_FORCE_INLINE float cos(float a)
{
    return ::cosf(a);
}

//! \brief platform-specific minimum
NV_CUDA_CALLABLE NV_FORCE_INLINE float selectMin(float a, float b)
{
    return a < b ? a : b;
}

//! \brief platform-specific maximum
NV_CUDA_CALLABLE NV_FORCE_INLINE float selectMax(float a, float b)
{
    return a > b ? a : b;
}

//! \brief platform-specific finiteness check (not INF or NAN)
NV_CUDA_CALLABLE NV_FORCE_INLINE bool isFinite(float a)
{
#ifdef __CUDACC__
    return !!isfinite(a);
#else
    return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(a)));
#endif
}

//! \brief platform-specific finiteness check (not INF or NAN)
NV_CUDA_CALLABLE NV_FORCE_INLINE bool isFinite(double a)
{
#ifdef __CUDACC__
    return !!isfinite(a);
#else
    return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(a)));
#endif
}

/*!
Sets \c count bytes starting at \c dst to zero.
*/
NV_FORCE_INLINE void* memZero(void* NV_RESTRICT dest, uint32_t count)
{
    return memset(dest, 0, count);
}

/*!
Sets \c count bytes starting at \c dst to \c c.
*/
NV_FORCE_INLINE void* memSet(void* NV_RESTRICT dest, int32_t c, uint32_t count)
{
    return memset(dest, c, count);
}

/*!
Copies \c count bytes from \c src to \c dst. User memMove if regions overlap.
*/
NV_FORCE_INLINE void* memCopy(void* NV_RESTRICT dest, const void* NV_RESTRICT src, uint32_t count)
{
    return memcpy(dest, src, count);
}

/*!
Copies \c count bytes from \c src to \c dst. Supports overlapping regions.
*/
NV_FORCE_INLINE void* memMove(void* NV_RESTRICT dest, const void* NV_RESTRICT src, uint32_t count)
{
    return memmove(dest, src, count);
}

/*!
Set 128B to zero starting at \c dst+offset. Must be aligned.
*/
NV_FORCE_INLINE void memZero128(void* NV_RESTRICT dest, uint32_t offset = 0)
{
    NV_ASSERT(((size_t(dest) + offset) & 0x7f) == 0,"Invalid destination address");
    memSet((char * NV_RESTRICT)dest + offset, 0, 128);
}

#if !NV_DOXYGEN
} // namespace intrinsics
} // namespace blockchainsim
#endif

#endif // #ifndef NV_WINDOWS_NVWINDOWSINTRINSICS_H
