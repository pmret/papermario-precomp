# papermario-precomp

This is a precomp of Paper Mario (US). It allows custom code to be injected into the ROM. Thanks to [Mr-Wiseguy](https://github.com/Mr-Wiseguy/).

## Setup

This repo should be cloned alongside the [papermario](https://github.com/pmret/papermario) repo, e.g.

```
papermario/
papermario-precomp/
```

The precomp works by patching code in the `.elf` that is created as part of papermario's build process. You must first [build papermario](https://github.com/pmret/papermario/blob/master/INSTALL.md) before you can use this precomp.

Compile with `make` to produce `build/papermario.z64`.

## Modding

Mods can be placed in the `mods/` directory. Mods can use the [papercut.h](include/papercut.h) to patch and extend the game at runtime.

An example mod, [hello_world](mods/hello_world/), is included in this repo. To build the game with it, use `make MODS="hello_world"`.

To load multiple mods, separate them with spaces: `make MODS="mod_one mod_two"`.
