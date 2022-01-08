#include <papercut.h>
#include <common.h>

#include "messages.msg.c"

static s32 x = -100;
static s32 y = -100;
static s32 dx = 1;
static s32 dy = 1;

static void post_render(PapercutCallbackCtx ctx) {
    // Get the width and height of the message
    s32 width;
    s32 height;
    get_msg_properties(helloWorld, &height, &width, NULL, NULL, NULL, NULL, 0);

    // Initialise x and y to a random position
    if (x == -100)
        x = rand_int(SCREEN_WIDTH - width);
    if (y == -100)
        y = rand_int(SCREEN_HEIGHT - height);
    
    // Apply velocity
    x += dx;
    y += dy;

    // Bounce from left and right
    if (x > SCREEN_WIDTH - width)
        dx = -1;
    else if (x < 0)
        dx = 1;

    // Bounce from top and bottom
    if (y > SCREEN_HEIGHT - height)
        dy = -1;
    else if (y < 0)
        dy = 1;

    // Draw the message
    draw_msg(helloWorld, x, y, 0xFF, 0xA, 0);
}

void hello_world_main() {
    papercut_register_callback(PAPERCUT_EVENT_POST_RENDER, post_render);
}
