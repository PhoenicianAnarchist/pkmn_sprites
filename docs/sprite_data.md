# Sprite Data Format

Sprite data is compressed before storage to save space.

## Data format

- 4 bits   : sprite width
- 4 bits   : sprite height
- 1 bit    : primary buffer
- variable : bitplane 0 data
- 1/2 bits : encoding mode
- variable : bitplane 1 data

### Bitplane

The first bit in the bitplane data denotes the first packet type. A 1 indicates
a data packet; 0 indicates an rle packet. The rest of the bitplane data is
encoded usine Run Length Encoding, but there is no terminator so data is
decoded until enough data has been decoded (determined by the width and height
of the sprite).
