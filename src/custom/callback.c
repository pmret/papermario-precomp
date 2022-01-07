#include <common.h>
#include <papercut.h>

struct PapercutCallbackCtx {
    s32 hasReturnValue;
    void* returnValue;
};

static PapercutCallbackFunc callbacks[PAPERCUT_NUM_EVENTS][PAPERCUT_MAX_CALLBACKS_PER_EVENT];

s32 papercut_register_callback(enum PapercutEvent event, PapercutCallbackFunc func) {
    PapercutCallbackFunc* callbacksForEvent = callbacks[event];
    PapercutCallbackFunc* slot = NULL;
    s32 i;

    for (i = 0; i < PAPERCUT_MAX_CALLBACKS_PER_EVENT; i++) {
        if (callbacksForEvent[i] == NULL) {
            slot = &callbacksForEvent[i];
            break;
        }
    }

    if (slot == NULL) {
        return 0;
    }

    *slot = func;

    return 1;
}

s32 papercut_unregister_callback(enum PapercutEvent event, PapercutCallbackFunc func) {
    PapercutCallbackFunc* callbacksForEvent = callbacks[event];
    s32 i;

    for (i = 0; i < PAPERCUT_MAX_CALLBACKS_PER_EVENT; i++) {
        if (callbacksForEvent[i] == func) {
            callbacksForEvent[i] = NULL;
            return 1;
        }
    }

    return 0;
}

#define EXECUTE_CALLBACKS(event) \
    do { \
        PapercutCallbackFunc* callbacksForEvent = &callbacks[event]; \
        struct PapercutCallbackCtx ctx = { \
            .hasReturnValue = FALSE, \
            .returnValue = NULL, \
        }; \
        \
        s32 i; \
        for (i = 0; i < PAPERCUT_MAX_CALLBACKS_PER_EVENT; i++) { \
            if (callbacksForEvent[i] != NULL) { \
                callbacksForEvent[i](&ctx); \
            } \
        } \
        \
        if (ctx.hasReturnValue) { \
            return ctx.returnValue; \
        } \
    } while (0)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ CALLBACK HOOKS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// gfx_draw_frame: PAPERCUT_EVENT_PRE_RENDER, PAPERCUT_EVENT_POST_RENDER
#include <nu/nusys.h>
extern s16 D_800741A0;
extern s16 D_800741A2;
extern s32 D_800741A8[];
void gfx_draw_frame(void);
static void patched_gfx_draw_frame(void) {
    gMatrixListPos = 0;
    gMasterGfxPos = &gDisplayContext->mainGfx[0];

    if (gOverrideFlags & 8) {
        gCurrentDisplayContextIndex = gCurrentDisplayContextIndex ^ 1;
        return;
    }

    gSPMatrix(gMasterGfxPos++, D_800741A8, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    spr_render_init();

    EXECUTE_CALLBACKS(PAPERCUT_EVENT_PRE_RENDER);

    if (!(gOverrideFlags & 2)) {
        render_frame(0);
    }

    player_render_interact_prompts();
    func_802C3EE4();
    render_screen_overlay_backUI();
    render_generic_entities_backUI();
    render_hud_elements_backUI();
    render_effects_UI();
    state_render_backUI();

    if (!(gOverrideFlags & 0x10000)) {
        render_window_root();
    }
    if (!(gOverrideFlags & 2) && gGameStatusPtr->disableScripts == 0) {
        render_frame(1);
    }

    if (!(gOverrideFlags & 0x100010)) {
        render_messages();
    }

    render_generic_entities_frontUI();
    render_hud_elements_frontUI();
    render_screen_overlay_frontUI();

    if ((gOverrideFlags & 0x100010) == 0x10) {
        render_messages();
    }

    render_curtains();

    if (gOverrideFlags & 0x100000) {
        render_messages();
    }
    if (gOverrideFlags & 0x10000) {
        render_window_root();
    }

    state_render_frontUI();

    if (gOverrideFlags & 0x20) {
        switch (D_800741A2) {
            case 0:
            case 1:
                _render_transition_stencil(7, D_800741A0, NULL);
                break;
        }
    }

    EXECUTE_CALLBACKS(PAPERCUT_EVENT_POST_RENDER);

    ASSERT((s32)(((u32)(gMasterGfxPos - gDisplayContext->mainGfx) << 3) >> 3) < ARRAY_COUNT(gDisplayContext->mainGfx));

    gDPFullSync(gMasterGfxPos++);
    gSPEndDisplayList(gMasterGfxPos++);

    nuGfxTaskStart(gDisplayContext->mainGfx, (u32)(gMasterGfxPos - gDisplayContext->mainGfx) * 8, NU_GFX_UCODE_F3DEX2,
                   NU_SC_TASK_LODABLE | NU_SC_SWAPBUFFER);
    gCurrentDisplayContextIndex = gCurrentDisplayContextIndex ^ 1;
    crash_screen_set_draw_info(nuGfxCfb_ptr, SCREEN_WIDTH, SCREEN_HEIGHT);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INIT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void papercut_patch_engine_callbacks(void) {
    papercut_patch_replace(gfx_draw_frame, patched_gfx_draw_frame);
}
