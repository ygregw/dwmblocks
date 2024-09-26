# dwmblocks
Modular status bar for dwm written in c (forked from [torrinfail/dwmblocks](https://github.com/torrinfail/dwmblocks))

# usage
To use dwmblocks first run `make` and then install it with `sudo make install`.
After that you can put dwmblocks in your `xinitrc` or other startup script to
have it start with dwm.

# modifying blocks
The statusbar is made from text output from commandline programs. Blocks are
added and removed by editing the `blocks.h` header file. By default the
`blocks.h` header file is created the first time you run make which copies the
default config from `blocks.def.h`. This is so you can edit your status bar
commands and they will not get overwritten in a future update.

# extrabar support
The current version supports dwm's [extrabar](https://dwm.suckless.org/patches/extrabar/) patch.
See `blocks.def.h` file for an example block data setup.
The fourth data column, `Fixed-length`, is new. See explanation below.

# fixing block width
Running the executable `dwmblocks` as is gives default behavior where blocks
are separated by a user-chosen delimiter. The statusbar is "elastic" in this
mode, however, since blocks tend to have varying length over time. For example,
when cpu usage changes from 10.0% to 100.0%, the module becomes one character
longer.

If you find this behavior distracting (as I do), you can run the executable
with newly added `-f` flag. The `Fixed-length` data fixes the length of each
block. A negative value left-aligns the output within the block (consistent
with `printf`'s format specifier). Note that, in this mode, block content is
padded with spaces; delimiter is therefore removed due to varying amount of
paddings from block to block.

*A demo (with fixed-length blocks):*

![dwmblocks](https://gregw.xyz/bkp/files/fixed.png)
