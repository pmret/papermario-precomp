#include <ultra64.h>

#ifndef PAPERCUT
#define PAPERCUT

/// Hooks a function, overwriting its first two instructions with:
///     j   destFunc
///     nop
void papercut_patch_hook(void* srcFunc, u32 offset, void* destFunc);

/// Replaces a function entirely, so all calls to it route to destFunc.
void papercut_patch_replace(void* srcFunc, void* destFunc);

/// Writes a nop instruction at the given offset.
void papercut_patch_nop(void* srcFunc, u32 offset);

/// The maximum number of callbacks that can be registered to a single event.
#define PAPERCUT_MAX_CALLBACKS_PER_EVENT 0x10

/// A callback event.
enum PapercutEvent {
    PAPERCUT_EVENT_PRE_RENDER,
    PAPERCUT_EVENT_POST_RENDER,

    PAPERCUT_NUM_EVENTS,
};

/// Opaque pointer for a callback context. Can be used with `papercut_ctx_*` functions.
typedef struct PapercutCallbackCtx * PapercutCallbackCtx;

/// A callback function.
typedef void (*PapercutCallbackFunc)(PapercutCallbackCtx ctx);

/// Registers a callback function to be called when the specified event occurs.
/// Returns 1 on success.
s32 papercut_register_callback(enum PapercutEvent event, PapercutCallbackFunc func);

/// Unregisters a previous-registered callback from an event.
/// Returns 1 on success.
s32 papercut_unregister_callback(enum PapercutEvent event, PapercutCallbackFunc func);

/// Causes the event handler to return the given value from the parent function,
/// skipping anything after the event. Other callbacks still run.
void papercut_ctx_return_early(struct PapercutCallbackCtx* ctx, void* returnValue);

#endif
