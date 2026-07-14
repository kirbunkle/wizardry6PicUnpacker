# wizardry6PicUnpacker

Utility for decompressing and recompressing the .PIC image files from *Wizardry 6: Bane of the Cosmic Forge* (1990).

## About

Wizardry 6 stores its monster portraits and other images in a proprietary compressed .PIC format. This program decompresses .PIC files into editable formats and compresses edited images back into .PIC files the game can load, which is how the pre-edited files in this repository were made.

## Usage

```
wiz6PicUnpacker -dr <input.PIC> <output>  decompress .PIC to raw data (as it looks in game memory)
wiz6PicUnpacker -db <input.PIC> <output>  decompress .PIC to a pseudo-BMP (pixel layout of a BMP;
                                          not a valid .bmp header yet)
wiz6PicUnpacker -cr <input> <output.PIC>  compress raw data back to .PIC
wiz6PicUnpacker -cb <input> <output.PIC>  compress a pseudo-BMP back to .PIC
```

The program is a single C++ file (`Project1/wiz6PicUnpacker.cpp`) with no dependencies; build it with any C++ compiler, e.g. `g++ -o wiz6PicUnpacker Project1/wiz6PicUnpacker.cpp`.

## The .PIC format

The format is not documented anywhere I could find, so I reverse engineered it: running the game under a DOSBox debugger with breakpoints on the file-reading code, lifting routines with a decompiler, and editing bytes in the files to observe what changed on screen until the scheme made sense.

It turned out to be run-length encoding with signed control bytes. The game reads the file in 0x1000-byte blocks; within a block, a positive control byte *n* means "the next *n* bytes are literal data," a negative control byte means "repeat the next byte *-n* times," and 0x00 terminates the stream. Decompressed pixel data packs two 16-color pixels per byte.

## Pre-edited .PIC files

My original goal was to censor some of the nudity in the game so I could stream it. Three edited portraits are included:

- MON30 — Amazulu
- MON38 — Siren
- MON45 — Rebecca

Previews are in `EditedPics/SamplesOfEditedPics`. To install, copy the .PIC files into your Wizardry 6 installation directory (for the GOG version, e.g. `C:\Program Files (x86)\GOG Galaxy\Games\Wizardry 6\`). They replace the originals, so back those up first if you want them.

Enjoy.
