/*
 * Copyright (c) 2018-2020 Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
// This file was taken from the Stratosphere library at https://github.com/Atmosphere-NX/Atmosphere-libs/tree/master/libstratosphere

#pragma once
#include <switch.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ServiceGuard {
    Mutex mutex;
    u32 refCount;
} ServiceGuard;

NX_INLINE bool serviceGuardBeginInit(ServiceGuard* g)
{
    mutexLock(&g->mutex);
    return (g->refCount++) == 0;
}

NX_INLINE Result serviceGuardEndInit(ServiceGuard* g, Result rc, void (*cleanupFunc)(void))
{
    if (R_FAILED(rc)) {
        cleanupFunc();
        --g->refCount;
    }
    mutexUnlock(&g->mutex);
    return rc;
}

NX_INLINE void serviceGuardExit(ServiceGuard* g, void (*cleanupFunc)(void))
{
    mutexLock(&g->mutex);
    if (g->refCount && (--g->refCount) == 0)
        cleanupFunc();
    mutexUnlock(&g->mutex);
}

#define NX_GENERATE_SERVICE_GUARD_PARAMS(name, _paramdecl, _parampass) \
\
static ServiceGuard g_##name##Guard; \
NX_INLINE Result _##name##Initialize _paramdecl; \
static void _##name##Cleanup(void); \
\
Result name##Initialize _paramdecl \
{ \
    Result rc = 0; \
    if (serviceGuardBeginInit(&g_##name##Guard)) \
        rc = _##name##Initialize _parampass; \
    return serviceGuardEndInit(&g_##name##Guard, rc, _##name##Cleanup); \
} \
\
void name##Exit(void) \
{ \
    serviceGuardExit(&g_##name##Guard, _##name##Cleanup); \
}

#define NX_GENERATE_SERVICE_GUARD(name) NX_GENERATE_SERVICE_GUARD_PARAMS(name, (void), ())

#ifdef __cplusplus
}
#endif
