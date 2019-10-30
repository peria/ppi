# ppi

円周率の計算をするプログラムを作っていきます。
2018年アドベントカレンダーにする予定でしたが、仮にアドベントカレンダーとして採用するなら早くて2020年だと思います。

以下のような計算ができることを目標にしています。

- 2 段階 FMT の採用
- ファイルを利用する規模の桁数の計算
- SRT を利用した基底変換
- 演算内の並列計算
- レベル違いでのエンコードでのシームレス多倍長変換

諸事情でビルドやそもそもチェックアウトする方法が一般的な方法ではありません。

## 事前準備系

1. [depot_tools](https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up) を入れます
2. `gclient config --unmanaged https://github.com/peria/ppi` もしくは `gclient config --unmanaged git@github.com:peria/ppi` で `.gclient` ファイルができます。
3. `gclient sync` で `ppi/` というディレクトリができてその下に色々チェックアウトされます

## ビルド

1. `ppi/` の下で `gn gen out/Default` とすると `out/Default` ディレクトリができます
2. `ninja -C out/Default` でビルドされます
