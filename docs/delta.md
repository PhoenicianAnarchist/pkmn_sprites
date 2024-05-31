# Delta Encoding

The RLE algorithm is more efficient with longer stretches of zeros. Delta
encoding will maximise the length of zero-strings.

## Encoding

Each bit is compared to the previous bit, if they are the same, a `0` is
written to the output buffer, otherwise a `1` will be written. The first bit is
compared to an implicit value of `0`.

## Deecoding

Each `1` in the input data will flip the current "output bit" (initial value is
`0`) _before_ it is written to the output stream.

## example

```
  encoded = 00100110 01010000 010
  decoded = 00111011 10011111 100
```
