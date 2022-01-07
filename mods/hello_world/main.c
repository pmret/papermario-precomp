#include <papercut.h>
#include <common.h>

static void post_render(PapercutCallbackCtx ctx) {
    draw_msg(0x1D00AD, 60, 100, 0xFF, 0xA, 0); // "You made the first strike!!"
}

void hello_world_main() {
    papercut_register_callback(PAPERCUT_EVENT_POST_RENDER, post_render);
}
