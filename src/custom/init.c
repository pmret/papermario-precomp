#include <ultra64.h>

extern u8 _customSegmentNoloadStart[];
extern u8 _customSegmentNoloadEnd[];

void papercut_patch_engine_callbacks(void);
void load_mods();

void custom_init() {
    bzero(_customSegmentNoloadStart, _customSegmentNoloadEnd - _customSegmentNoloadStart);
    
    papercut_patch_engine_callbacks();
    
    load_mods();
}
