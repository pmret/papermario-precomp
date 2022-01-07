OUTPUT_ARCH (mips)

#define BEGIN_SEG(name, addr) \
    _##name##SegmentStart = ADDR(.name); \
    _##name##SegmentRomStart = __romPos; \
    .name addr : AT(__romPos)

#define END_SEG(name) \
    _##name##SegmentEnd = ADDR(.name) + SIZEOF(.name); \
    _##name##SegmentRomEnd = __romPos + SIZEOF(.name); \
    _##name##SegmentSize = _##name##SegmentEnd - _##name##SegmentStart; \
    __romPos += SIZEOF(.name);

#define BEGIN_NOLOAD(name) \
    _##name##SegmentNoloadStart = ADDR(.name.noload); \
    .name.noload (NOLOAD) :

#define END_NOLOAD(name) \
    _##name##SegmentNoloadEnd = ADDR(.name.noload) + SIZEOF(.name.noload);

#define PATCH(segment, symbol, offset) \
    .segment##symbol##offset##_patch symbol + offset : AT(_##segment##SegmentRomStart + symbol - _##segment##SegmentStart + offset)
    
#define CODE_PATCH(segment, symbol, offset) \
    .segment##symbol##offset##_patch symbol + offset : AT(segment##_ROM_START + symbol - segment##_VRAM + offset)

#define JAL_HOOK(segment, symbol, offset, helper, name) \
    .segment##symbol##offset##_patch symbol + offset : AT(segment##_ROM_START + symbol - segment##_VRAM + offset) \
    { \
        name = .; \
        BYTE(0x0C); \
        BYTE((helper >> 18) & 0xFF); \
        BYTE((helper >> 10) & 0xFF); \
        BYTE((helper >> 2)  & 0xFF); \
    }
#define J_HOOK_NOP(segment, symbol, offset, helper, name) \
    .segment##symbol##offset##_patch symbol + offset : AT(segment##_ROM_START + symbol - segment##_VRAM + offset) \
    { \
        name = .; \
        BYTE(0x08); \
        BYTE((helper >> 18) & 0xFF); \
        BYTE((helper >> 10) & 0xFF); \
        BYTE((helper >> 2)  & 0xFF); \
        BYTE(0x00); \
        BYTE(0x00); \
        BYTE(0x00); \
        BYTE(0x00); \
    }
    
#define ROM_PATCH(address) \
    .rom##address##_patch : AT(address)

SECTIONS
{
    __romPos = 0;

    BEGIN_SEG(baserom, 0)
    {
        BASEROM;
    }
    END_SEG(baserom)
    baseromEnd = __romPos;
    extRamStart = 0xFFFFFFFF80400000;
    . = extRamStart;
    BEGIN_SEG(custom, .)
    {
        BUILD_DIR/src/custom/init.o(.text);
        BUILD_DIR/src/custom/drawing.o(.text);
        BUILD_DIR/src/custom/usb.o(.text);
        BUILD_DIR/src/custom/debug.o(.text);
        BUILD_DIR/src/custom/init.o(.*data*);
        BUILD_DIR/src/custom/drawing.o(.*data*);
        BUILD_DIR/src/custom/usb.o(.*data*);
        BUILD_DIR/src/custom/debug.o(.*data*);
    }
    END_SEG(custom)
    BEGIN_NOLOAD(custom)
    {
        BUILD_DIR/src/custom/init.o(.bss);
        BUILD_DIR/src/custom/drawing.o(.bss);
        BUILD_DIR/src/custom/usb.o(.bss);
        BUILD_DIR/src/custom/debug.o(.bss);
    }
    END_NOLOAD(custom)

    /* Overwrite the IS64 initialization with a dma of custom code into ext ram */
    CODE_PATCH(_1000, is_debug_init, 0x0)
    {
        BUILD_DIR/src/patches/dma_code.o(.text);
    }

    /* Overwrite the IS64 print functions with jumps to debug_printf */
    J_HOOK_NOP(_1000, printf, 0x0, debug_printf, func_80025CC0_print_hook)

    /* Overwrite the IS64 print functions with jumps to debug_printf */
    J_HOOK_NOP(_1000, osSyncPrintf, 0x0, debug_printf, func_80025CFC_print_hook)

    /* Overwrite the IS64 print functions with jumps to debug_printf */
    J_HOOK_NOP(_1000, rmonPrintf, 0x0, debug_printf, func_80025D38_print_hook)

    ROM_PATCH(0x000000)
    {
        BUILD_DIR/src/patches/header.o(.data);
    }

    /* Discard everything not specifically mentioned above. */
    /DISCARD/ :
    {
        *(*);
    }
}
