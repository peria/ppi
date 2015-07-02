master: [![CircleCI](https://circleci.com/gh/peria/ppi.svg?style=svg)](https://circleci.com/gh/peria/ppi) [![Coverage Status](https://coveralls.io/repos/github/peria/ppi/badge.svg?branch=master)](https://coveralls.io/github/peria/ppi?branch=master)

# ppi
ppi は円周率の計算をするプログラムを書いていくお一人様プロジェクトです。十分なストレージがあれば現実的な時間で１兆桁とか計算できるようになることを目標にしています。なので円周率の計算で必要になる多倍長計算に [GMP](https://gmplib.org/)のようなライブラリは用いません。

## 計算原理など
ひとまず [Qiita](http://qiita.com/) に以下のような記事でざっくり書いてます。後でコードとの対応がつくように纏めますが、 Markdown にするか、数式が見やすくなるように HTML にするかは現在思案中です。
- [Chudnovsky の公式を用いた円周率の計算用メモ](http://qiita.com/peria/items/c02ef9fc18fb0362fb89)
- [FFT を用いた多倍長整数の乗算の効率化 (1)](http://qiita.com/peria/items/fbdd52768b4659823d88)
- [FFT を用いた多倍長整数の乗算の効率化 (2)](http://qiita.com/peria/items/cf4c4b72ebbeec7728af)
- [Schönhage-Strassen のアルゴリズムメモ](http://qiita.com/peria/items/19e37b192cb62bdbc3a1) (未実装)
