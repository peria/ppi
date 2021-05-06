# FMT

Routines to compute Fast Modulas Transformation (FMT).
FMT is an abstract of DFT and NTT, to apply FFT-like algorithms in convolutions.

In general FMT, n elements' arrays `a` and `A` are converted into each another
in the following rules.

```
          n-1
  A[k] = \sum a[j] * w^(j*k)
          j=0
                 n-1
  a[j] = n^(-1) \sum A[k] * w^(-j*k)
                 k=0
```

where `w` is an primary n-th root in the field.


## DFT

DFT is a FMT in complex field, where each element consists of a double-precision
complex number. `w` for DFT is exp(i*\pi/n).


## SSA

Schonhage-Strassen Algorithm (SSA) is a FMT in a modulus field, where each
element consists of a multiprecision number. `w` for SSA is 2 limbs (=2^128),
and the modulus field length is n-limbs (mod w^(n/2)+1).

```
           +-----+-------+-...-+-----+-----+
 Element = | [n] | [n-1] | ... | [1] | [0] |
           +-----+-------+-...-+-----+-----+
              ^-- This limb can be 1 iff the element is w^(n/2).
```
