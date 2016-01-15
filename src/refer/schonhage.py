#!/usr/bin/python

import math
import random
import time

def main():
  unittest()
  benchmark()

def unittest():
  low = 1 << 10
  up = 1 << 10000

  random.seed(0)
  for i in xrange(1000):
    a = random.randint(low, up)
    b = random.randint(low, up)
    c = SchonhageStrassen(a, b)
    d = a * b
    assert c == d

  print "Passed 1000 random tests"

def benchmark():
  low = 1 << 100000
  up = low * 2

  random.seed(0)
  st = time.clock()
  for i in xrange(100):
    a = random.randint(low, up)
    b = random.randint(low, up)
    c = SchonhageStrassen(a, b)
  ed = time.clock()
  print "Schonhage: ", ed - st

  random.seed(0)
  st = time.clock()
  for i in xrange(100):
    a = random.randint(low, up)
    b = random.randint(low, up)
    c = a * b
  ed = time.clock()
  print "ToomCook:", ed - st

def SchonhageStrassen(a, b):
  n, k = SetUp(a, b)
  m = (1 << (n / 2)) + 1
  a = NTT(Split(a, n, k), m)
  b = NTT(Split(b, n, k), m)
  c = MultEach(a, b, m)
  return Merge(NTTInv(c, m), k)

def Split(a, n, k):
  mask = (1 << k) - 1
  elements = []
  for i in xrange(n):
    elements.append(a & mask)
    a >>= k
  return elements

def NTT(a, m, p=1):
  n = len(a)
  q = n / 2
  while q >= 1:
    for i in xrange(q):
      w = pow(2, p * i % n, m)
      for j in xrange(i, n, 2 * q):
        k = j + q
        a[j], a[k] = (a[j] + a[k]) % m, (a[j] - a[k] + m) * w % m
    p, q = p * 2, q / 2

  # bit-reverse shuffle
  i = 0
  for j in xrange(1, n):
    k = n / 2
    i ^= k
    while i < k:
      k /= 2
      i ^= k
    if j < i:
      a[j], a[i] = a[i], a[j]

  return a

def MultEach(a, b, m):
  for i in xrange(len(a)):
    a[i] = a[i] * b[i] % m
  return a

def NTTInv(a, m):
  n = len(a)
  a = NTT(a, m, n - 1)

  logn = int(math.log(n, 2))
  inv = pow(2, n - logn, m)  # inv * n % m = 1
  for i in xrange(n):
    a[i] = a[i] * inv % m
  return a

def Merge(a, k):
  val = 0
  a.reverse()
  for v in a:
    val = (val << k) + v
  return val

def SetUp(a, b):
  N = max(a.bit_length(), b.bit_length())
  for b in xrange(5, 30):
    n = 1 << b
    k = (n / 2 - b) / 2
    if n * k >= N * 2:
      return n, k
  # 2^30 for n is too large
  assert false

if __name__ == '__main__':
  main()
