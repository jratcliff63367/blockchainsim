#ifndef NS_USER_ALLOCATED_H
#define NS_USER_ALLOCATED_H

#include <malloc.h>

#define NV_ALLOC(x,y) ::malloc(x)
#define NV_FREE(x) ::free(x)

namespace blockchainsim
{

    class UserAllocated
    {
    public:
    };

}

#define NV_NEW(x) new x

#define NV_SAFE_RELEASE(x) if ( x ) { x->release(); x = nullptr; }

#endif
