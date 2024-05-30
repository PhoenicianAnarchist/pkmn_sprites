# Pokémon sprite extractor and renderer

Takes a binary file of decompressed tile data and outputs a .pgm file.

## todo

- [x] extract raw sprite data (and other data?) from ROM
- [x] decompress raw sprite data
- [ ] output to a more common file format (e.g. .png)
- [ ] bugfix: check the sprite bank index list, some are junk

## extra stuff

- [x] add colour palette options
- [x] rewrite to be more accurate with respect to memory layout
- [ ] stitch pokemon sprites together into a single sprite map
- [ ] reduce the amount of manual indexing that is required (i.e. find the pointer table in ROM)
- [ ] handle glitch pokemon data
- [ ] extract back sprites
- [ ] extract non-pokemon sprites too
- [ ] have a visual animation of the decompression process

## BUGS:

There are a few decoding errors in certain sprites, need to double check all
sprites.

- lines, example:
  - WIGGLYTUFF,  ID (101) DexNo (40)
  - GOLDUCK,  ID (128) DexNo (55)

- wrong colours, example:
  - GEODUDE,  ID (169) DexNo (74)
  - OMANYTE,  ID (98) DexNo (138)
