<!-- ./md/essential_intro.md -->
# イントロダクション <a id="SS_1"></a>
このドキュメントは、C++を使用するチームが必要とする知識全般を提供するドキュメントである
[https://github.com/ichiroprogrammer/cpp_docs](https://github.com/ichiroprogrammer/cpp_docs)
の中から、C++の言語機能や慣用語句の解説を抜き出したものである。

## 改訂履歴 <a id="SS_1_1"></a>
* V20.12
    * CRTPの説明強化
    * 「標準ライブラリとプログラミングの概念」に「std::enable_shared_from_this」の説明追加
    * このドキュメントのSOLIDとデザインパターンの章をdeep_cppへ移動
    * C++慣用語句に「Modern CMake project layout」を追加

* V20.11
    * git@github:ichiroprogrammer/comprehensive_cpp.gitの用語説明からのスピンアウト
    * 標準ライブラリとプログラミングの概念に「ロック所有ラッパー」、「並列処理」とstd::condition_variable追加
    * C++慣用語句に「関数設計のガイドライン」、「クラス設計のガイドライン」追加

## ドキュメントの諸注意 <a id="SS_1_2"></a>
__技術用語の用法__

* 参照
    * 「～を参照する」というような文脈で使われる**「参照」**はそのまま使用する。
    * C++での参照型を表す**参照**は使わず、代わりに**「リファレンス」**を使用する。
* 例外
    * 「～の場合は例外である」というような文脈で使われる**「例外」**はそのまま使用する。
    * C++でthrowすると発生する事象を表す**例外**は使わず、代わりに**「エクセプション」**を使用する。
* classとクラス
    * **class**はC++のキーワードとして使用する。
    * **クラス**は上記以外で使用する。
* プログラミングとコーディング、ソースコードとコード、インスタンスとオブジェクト、に関しては同義語として使用する。

__コード例について__

次章以降では、ソースコードを使って説明を行う場合がある。このような場合の注意点を述べる。

* 「`// ...`」のような行は、ソースコードの省略を表す。
* 特定の規則、法則、慣習等を説明するためのソースコードは、シンプルさを優先するため、
  その他の規則、法則、慣習に従っていない場合があるが、
  [Trailing Underscore(末尾アンダースコア)](cpp_idioms.md#SS_4_9_4)には従っている。
  また、一般に標準ライブラリのコンテナクラスをnewする必要はないが、
  コードの動作を示すためにあえてそのようにする場合がある。
* ソースコード内に動作説明のような本来不要なコメントがあるのは、
  読者にその意味を知らせるためであるため、製品コードのコメントをこのようにするべきではない。
* 例示したコードの動作の確認、明示のために
  [google test(gtest)](http://opencv.jp/googletestdocs/primer.html)のアサーション(下表)を使用する。


__google test_のアサーションの簡単な説明__

|アサーションマクロ  | 意味                                     |
|:-------------------|:-----------------------------------------|
| ASSERT_TRUE(x)     | xがtrue                                  |
| ASSERT_FALSE(x)    | xがfalse                                 |
| ASSERT_EQ(x, y)    | (x == y)がtrue                           |
| ASSERT_NE(x, y)    | (x != y)がtrue                           |
| ASSERT_GE(x, y)    | (x >= y)がtrue                           |
| ASSERT_GT(x, y)    | (x >  y)がtrue                           |
| ASSERT_LE(x, y)    | (x <= y)がtrue                           |
| ASSERT_LT(x, y)    | (x <  y)がtrue                           |
| ASSERT_STREQ(x, y) | (std::string(x) == std::string(y))がtrue |
| ASSERT_DEATH(x, y) | xを実行するとアボートすればtrue          |
| ASSERT_THROW(x, y) | xを実行するとy例外が発生すればtrue       |


## インデックス <a id="SS_1_3"></a>
___

- [C++コア言語仕様](core_lang_spec.md#SS_2)
- [標準ライブラリとプログラミングの概念](stdlib_and_concepts.md#SS_3)
- [C++慣用語句](cpp_idioms.md#SS_4)
- [Sample Code](sample_code.md#SS_6)



