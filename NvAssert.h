#ifndef NV_NVFOUNDATION_NVASSERT_H
#define NV_NVFOUNDATION_NVASSERT_H

/** \addtogroup foundation
@{ */

#include <assert.h>

#define NV_ASSERT(x,...) assert(x)
#define NV_ALWAYS_ASSERT(...) assert(0)
#define NV_DBG_ASSERT(x,...) assert(x)
#define NV_DBG_ALWAYS_ASSERT(...) assert(0)

#endif // #ifndef NV_NVFOUNDATION_NVASSERT_H
