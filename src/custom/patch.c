#include <ultra64.h>

void papercut_patch_hook(void* srcFunc, u32 offset, void* destFunc) {
    u8* srcBytes = (u8*)((u32)srcFunc + offset);
    u32 destAddr = (u32)destFunc;

    // jal destFunc[offset]
    srcBytes[0] = 0x08;
    srcBytes[1] = (destAddr >> 18) & 0xFF;
    srcBytes[2] = (destAddr >> 10) & 0xFF;
    srcBytes[3] = (destAddr >> 2)  & 0xFF;

    // nop
    srcBytes[4] = 0x00;
    srcBytes[5] = 0x00;
    srcBytes[6] = 0x00;
    srcBytes[7] = 0x00;
}

void papercut_patch_replace(void* srcFunc, void* destFunc) {
    u8* srcBytes = (u8*)srcFunc;
    u32 destAddr = (u32)destFunc;

    // j destFunc
    srcBytes[0] = 0x08;
    srcBytes[1] = (destAddr >> 18) & 0xFF;
    srcBytes[2] = (destAddr >> 10) & 0xFF;
    srcBytes[3] = (destAddr >> 2)  & 0xFF;

    // nop
    srcBytes[4] = 0x00;
    srcBytes[5] = 0x00;
    srcBytes[6] = 0x00;
    srcBytes[7] = 0x00;
}

void papercut_patch_nop(void* srcFunc, u32 offset) {
    u8* srcBytes = (u8*)((u32)srcFunc + offset);

    // nop
    srcBytes[0] = 0x00;
    srcBytes[1] = 0x00;
    srcBytes[2] = 0x00;
    srcBytes[3] = 0x00;
}
