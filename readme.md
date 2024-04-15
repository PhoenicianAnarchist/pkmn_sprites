# Pokémon sprite extractor and renderer

Takes a binary file of decompressed tile data and outputs a .pgm file.

## todo

- extract raw sprite data (and other data?) from ROM
- decompress raw sprite data
- output to a more common file format (e.g. .png)

## extra stuff

- add colour palette options
- stitch pokemon sprites together into a single sprite map
- reduce the amount of manual indexing that is required (i.e. find the pointer table in ROM)
- handle glitch pokemon data
- extract non-pokemon sprites too
- have a visual animation of the decompression process
