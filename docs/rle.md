# Run Length Encoding

Encoded data is made up of two types of packet (Data and RLE) whcih always
alternate. The first packet type is determined by the first bit in the data.

## Data Packet

Data packets are the simpler packet type. Pairs of bit are coppied to the
output buffer until a `0x00` packet is encountered. (The `0x00` packet is not
copied over.)

```
encoded = 01101100
decoded = 011011
```

## RLE Packet

An RLE packet encodes the number of `0x00` pairs that need to be written to the
output buffer. The number of packets is encoded in two parts to minimise the
number of bits required.

A number of bits are read until a zero is encountered (`L`), the same number of
bits is read again (`V`). These two numbers are added together, with an extra
+1, to give the number of packets.

```
encoded = 11100101

      L =  1110 (14)
      V =  0101 ( 5)
      1 =     1 ( 1)
          ----------
decoded = 10100 (20)
