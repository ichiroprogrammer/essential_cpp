# C++慣用語句
この章では、C++慣用言句ついて解説を行う。

___

__この章の構成__

<!-- index 1-4 -->

[インデックス](---)に戻る。  

___

## イディオム

### ガード節(Early Return)
ガード節とは、
「可能な場合、処理を早期に打ち切るために関数やループの先頭に配置される短い条件文(通常はif文)」
であり、以下のような利点がある。

* 処理の打ち切り条件が明確になる。
* 関数やループのネストが少なくなる。

まずは、ガード節を使っていない例を上げる。

```cpp
    // @@@ example/cpp_idioms/guard_ut.cpp #0:0 begin
```

上記の例を読んで一目で何が行われているか、理解できる人は稀である。
一方で、上記と同じロジックである下記関数を一目で理解できない人も稀である。

```cpp
    // @@@ example/cpp_idioms/guard_ut.cpp #1:0 begin
```

ここまで効果的な例はあまりない。

もう一例、(ガード節導入の効果が前例ほど明確でない)ガード節を使っていないコードを示す。

```cpp
    // @@@ example/cpp_idioms/guard_ut.cpp #0:1 begin
```

上記にガード節を適用した例を下記する。

```cpp
    // @@@ example/cpp_idioms/guard_ut.cpp #1:1 begin
```

ガード節を使っていない例に比べて、

* ネストが減って読みやすくなった
* max_numが1, 2, 65535, 65536である場合がロジックの境界値であることが一目でわかるようになった

といった改善はされたものの、最初の例ほどのレベル差はない。
しかし、ソースコードの改善やリファクタリングのほとんどは、このようなものであり、
この少しのレベルアップが数か月後、数年後に大きな差を生み出すことを忘れてはならない。

[演習-ガード節](~~~)  

### RAII(scoped guard)
RAIIとは、「Resource Acquisition Is Initialization」の略語であり、
リソースの確保と解放をオブジェクトの初期化と破棄処理に結びつけるパターンもしくはイデオムである。
特にダイナミックにオブジェクトを生成する場合、
RAIIに従わないとメモリリークを防ぐことは困難である。

下記は、関数終了付近でdeleteする素朴なコードである。

```cpp
    // @@@ example/cpp_idioms/raii_ut.cpp #0:0 begin
```

このコードは下記の単体テストが示す通り、第1パラメータが5以上の場合、
エクセプションが発生しメモリリークしてしまう。

```cpp
    // @@@ example/cpp_idioms/raii_ut.cpp #0:1 begin -1
```

以下は、std::unique_ptrによってRAIIを導入し、この問題に対処した例である。

```cpp
    // @@@ example/cpp_idioms/raii_ut.cpp #1:0 begin
```

下記単体テストで確認できるように、
エクセプション発生時にもstd::unique_ptrによる自動解放によりメモリリークは発生しない。

```cpp
    // @@@ example/cpp_idioms/raii_ut.cpp #1:1 begin -1
```

RAIIのテクニックはメモリ管理のみでなく、ファイルディスクリプタ(open-close、socket-close)
等のリソース管理においても有効であるという例を示す。

下記は、生成したソケットを関数終了付近でcloseする素朴なコードである。

```cpp
    // @@@ example/cpp_idioms/raii_ut.cpp #2:0 begin
```

エクセプションを扱うために関数の2か所でソケットをcloseしている。
この程度であれば大きな問題にはならないだろうが、実際には様々な条件が重なるため、
リソースの解放コードは醜悪にならざるを得ない。

このような場合には、下記するようなリソース解放用クラス

```cpp
    // @@@ h/scoped_guard.h #0:0 begin
    // @@@ h/scoped_guard.h #0:1 begin
    // @@@ h/scoped_guard.h #0:2 begin
```

を使用し、下記のようにすることで安全なコードをすっきりと書くことができる。

```cpp
    // @@@ example/cpp_idioms/raii_ut.cpp #2:1 begin
```

クリティカルセクションの保護をlock/unlockで行うstd::mutex等を使う場合にも、
std::lock_guard<>によってunlockを行うことで、同様の効果が得られる。

[演習-RAIIの効果](~~~)  
[演習-RAII](~~~)  

### Copy-And-Swap
メンバ変数にポインタやスマートポインタを持つクラスに

* copyコンストラクタ
* copy代入演算子
* moveコンストラクタ
* move代入演算子

が必要になった場合、コンパイラが生成するデフォルトの
[オブジェクト生成と初期化|特殊メンバ関数](---)では機能が不十分であることが多い。

下記に示すコードは、そのような場合の上記4関数の実装例である。

```cpp
    // @@@ example/cpp_idioms/no_copy_and_swap_ut.cpp #0:0 begin
```

コード内のコメントで示したように、このコードには以下のような問題がある。

* copy代入演算子には、[エクセプション安全性の保証](---)がない。
* 上記4関数は似ているにも関わらず、微妙な違いがあるためコードクローンとなっている。

ここで紹介するCopy-And-Swapはこのような問題を解決するためのイデオムである。

実装例を以下に示す。

```cpp
    // @@@ example/cpp_idioms/copy_and_swap_ut.cpp #0:0 begin
```

上記CopyAndSwapのcopyコンストラクタ、moveコンストラクタに変更はない。
また、CopyAndSwap::Swapに関してもstd::vector等が持つswapと同様のものである。
このイデオムの特徴は、copy代入演算子、
move代入演算子が各コンストラクタとSwap関数により実装されている所にある。
これにより[エクセプション安全性の保証](---)を持つ4関数をコードクローンすることなく実装できる。

[演習-Copy-And-Swap](~~~)  

### CRTP(curiously recurring template pattern)
CRTPとは、

```cpp
    // @@@ example/cpp_idioms/crtp_ut.cpp #0:0 begin
```

のようなテンプレートによる再帰構造を用いて、静的ポリモーフィズムを実現するためのパターンである。
以下にこのパターンを使用した[ミックスイン](---)の例を示す。

```cpp
    // @@@ example/cpp_idioms/crtp_ut.cpp #1:0 begin
```
```cpp
    // @@@ example/cpp_idioms/crtp_ut.cpp #1:1 begin -1
```

なお、このパターンは、[std::enable_shared_from_this](---)の使用において前提知識となっている。

### Accessor
publicメンバ変数とそれにアクセスするソースコードは典型的なアンチパターンであるため、
このようなコードを禁じるのが一般的なプラクティスである。

```cpp
    // @@@ example/cpp_idioms/accessor_ut.cpp #0:0 begin
```

とはいえ、ソフトウェアのプラクティスには必ずといってほど例外があり、
製品開発の現場において、オブジェクトのメンバ変数にアクセスせざるを得ないような場面は、
稀にではあるが発生する。
このような場合に適用するがのこのイデオムである。

```cpp
    // @@@ example/cpp_idioms/accessor_ut.cpp #1:0 begin
```

メンバ変数への直接のアクセスに比べ、以下のようなメリットがある。

* アクセスのログを入れることができる。
* メンバ変数へのアクセスをデバッガで捕捉しやすくなる。
* setterに都合の悪い値が渡された場合、何らかの手段を取ることができる(assertや、エラー処理)。
* リファクタリングや機能修正により対象のメンバ変数がなくなった場合においても、
  クラスのインターフェースの変更を回避できる(修正箇所を局所化できる)。

一方で、クラスに対するこのような細かい制御は、カプセル化に対して問題を起こしやすい。
下記はその典型的なアンチパターンである。

```cpp
    // @@@ example/cpp_idioms/accessor_ut.cpp #2:0 begin
```

上記ソースコードは、オブジェクトaのA::a\_が変更された場合、
その後、それをもとに何らかの動作を行うこと(a.DoSomething)を表しているが、
本来オブジェクトaの状態が変わったかどうかはオブジェクトa自体が判断すべきであり、
a.DoSomething()の実行においても、それが必要かどうかはオブジェクトaが判断すべきである。
この考えに基づいた修正ソースコードを下記に示す。

```cpp
    // @@@ example/cpp_idioms/accessor_ut.cpp #3:0 begin
```

setterを使用する場合、上記のように処理の隠蔽化には特に気を付ける必要がある。

[演習-Accessorの副作用](~~~)  
[演習-Accessor](~~~)  

### Immutable
クラスに対するimmutable、immutabilityの定義を以下のように定める。

* immutable(不変な)なクラスとは、初期化後、状態の変更ができないクラスを指す。
* immutability(不変性)が高いクラスとは、
  状態を変更するメンバ関数(非constなメンバ関数)が少ないクラスを指す。

immutabilityが高いほど、そのクラスの使用方法は制限される。
これにより、そのクラスやそのクラスを使用しているソースコードの可読性やデバッグ容易性が向上する。
また、クラスがimmutableでなくても、そのクラスのオブジェクトをconstハンドル経由でアクセスすることで、
immutableとして扱うことができる。

一方で、「[Accessor](---)」で紹介したsetterは、クラスのimmutabilityを下げる。
いつでも状態が変更できるため、ソースコードの可読性やデバッグ容易性が低下する。
また、マルチスレッド環境においてはこのことが競合問題や、
それを回避するためのロックがパフォーマンス問題やデッドロックを引き起こしてしまう。

従って、クラスを宣言、定義する場合、immutabilityを出来るだけ高くするべきであり、
そのクラスのオブジェクトを使う側は、
可能な限りimmutableオブジェクト(constオブジェクト)として扱うべきである。

[演習-Immutable](~~~)  

### NVI(non virtual interface)
NVIとは、「virtualなメンバ関数をpublicにしない」という実装上の制約である。

下記のようにクラスBaseが定義されているとする。

```cpp
    // @@@ example/cpp_idioms/nvi_ut.cpp #0:0 begin
```

これを使うクラスはBase::DoSomething()に依存する。
また、このクラスから派生した下記のクラスDerivedもBase::DoSomething()に依存する。

```cpp
    // @@@ example/cpp_idioms/nvi_ut.cpp #0:1 begin
```

この条件下ではBase::DoSomething()へ依存が集中し、この関数の修正や機能追加の作業コストが高くなる。
このイデオムは、この問題を軽減する。

これを用いた上記2クラスのリファクタリング例を以下に示す。

```cpp
    // @@@ example/cpp_idioms/nvi_ut.cpp #1:0 begin
```

オーバーライド元の関数とそのオーバーライドのデフォルト引数の値は一致させる必要がある。

それに従わない下記のようなクラスとその派生クラス

```cpp
    // @@@ example/cpp_idioms/nvi_ut.cpp #2:0 begin
```

には下記の単体テストで示したような、
メンバ関数の振る舞いがその表層型に依存してしまう問題を持つことになる。

```cpp
    // @@@ example/cpp_idioms/nvi_ut.cpp #2:1 begin -1
```

この例のように継承階層が浅く、デフォルト引数の数も少ない場合、
この値を一致させることは難しくないが、
これよりも遥かに複雑な実際のコードではこの一致の維持は困難になる。

下記のようにNVIに従わせることでこのような問題に対処できる。

```cpp
    // @@@ example/cpp_idioms/nvi_ut.cpp #3:0 begin
```

下記の単体テストにより、この問題の解消が確認できる。

```cpp
    // @@@ example/cpp_idioms/nvi_ut.cpp #3:1 begin -1
```

なお、メンバ関数のデフォルト引数は、
そのクラス外部からのメンバ関数呼び出しを簡潔に記述するための記法であるため、
privateなメンバ関数はデフォルト引数を持つべきではない。

[演習-NVI](~~~)  

## 実装パターン
### Pimpl
このパターンは、「クラスA(a.cpp、a.hで宣言、定義)を使用するクラスにAの実装の詳細を伝搬させたくない」
ような場合に使用する。
そのため[オープン・クローズドの原則(OCP)](---)の実装方法としても有用である。

一般的に、STLライブラリのパースは多くのCPUタイムを消費する。
クラスAがSTLクラスをメンバに使用し、a.hにそのSTLヘッダファイルがインクルードされた場合、
a.hをインクルードするファイルをコンパイルする度にそのSTLヘッダファイルはパースされる。
これはさらに多くのCPUタイムの消費につながり、ソースコード全体のビルドは遅くなる。
こういった問題をあらかじめ避けるためにも有効な手段ではあるが、
そのトレードオフとして実行速度は若干遅くなる。

下記は、Pimplイデオム未使用の、std::stringに依存したクラスStringHolderOldの例である。

```cpp
    // @@@ example/cpp_idioms/string_holder_old.h #0:0 begin
```

```cpp
    // @@@ example/cpp_idioms/string_holder_old.cpp #0:0 begin
```


下記は、上記クラスStringHolderOldにPimplイデオムを適用したクラスStringHolderNewの例である。

```cpp
    // @@@ example/cpp_idioms/string_holder_new.h #0:0 begin
```

```cpp
    // @@@ example/cpp_idioms/string_holder_new.cpp #0:0 begin
```

下記図は、上記ファイルやそれらを使用するファイルの依存関係である。
string_holder_old.hは、std::stringに依存しているが、string_holder_new.hは、
std::stringに依存していないこと、
それによってStringHolderNewを使用するファイルから、std::stringへの依存を排除できていることがわかる。

![ファイルの依存関係](plant_uml/pimpl_pattern.png)

このパターンを使用して問題のある依存関係をリファクタリングする例を示す。

まずは、リファクタリング前のコードを下記する。

```cpp
    // in lib/h/widget.h

    // @@@ example/cpp_idioms/widget_ng.h #0:0 begin 0 -1
    // @@@ example/cpp_idioms/widget_ng.h #0:1 begin 0 -1
    // @@@ example/cpp_idioms/widget_ng.h #0:2 begin 0 -1
```
```cpp
    // in lib/src/widget.cpp

    #include "widget.h"
    // @@@ example/cpp_idioms/widget_ng.cpp #0:0 begin 0 -1
```
```cpp
    // in lib/ut/widget_ut.cpp

    #include "widget.h"
    // @@@ example/cpp_idioms/widget_ng_ut.cpp #0:0 begin 0 -1
```

何らかの事情により、単体テストでprivateなメンバにアクセスする必要があったため、
単体テストクラスをテスト対象クラスのfriendすることで、それを実現している。

単体テストクラスをテスト対象クラスのfriendにするためには、
上記コードの抜粋である下記を記述する必要がある。

```cpp
    // @@@ example/cpp_idioms/widget_ng.h #0:1 begin 0 -1
```

このマクロは、gtest.h内で定義されているため、widget.hからgtest.hをインクルードしている。

このため、ファイルの依存関係は下記のようになる。

![ファイルの依存関係](plant_uml/widget_ng.png)

この依存関係は、Widgetのクライアントに不要な依存関係を強要してしまう問題のある構造を作り出す。

この問題をPimplによるリファクタリングで解決したコードを以下に示す
(コンパイラのインクルードパスにはlib/hのみが入っていることを前提とする)。

```cpp
    // in lib/h/widget.h

    // @@@ example/cpp_idioms/widget_ok.h #0:0 begin 0 -1
```
```cpp
    // in lib/src/widget.cpp

    #include "widget_internal.h"
    // @@@ example/cpp_idioms/widget_ok.cpp #0:0 begin 0 -1
```
```cpp
    // in lib/src/widget_internal.h

    #include "widget.h"
    // @@@ example/cpp_idioms/widget_ok_internal.h #0:0 begin 0 -1
```
```cpp
    // in lib/ut/widget_ut.cpp

    #include "../src/widget_internal.h"  // 単体テストのみに、このようなインクルードを認める
    // @@@ example/cpp_idioms/widget_ok_ut.cpp #0:0 begin 0 -1
```

このリファクタリングにより、ファイルの依存は下記のようになり、
問題のある構造は解消された。

![ファイルの依存関係](plant_uml/widget_ok.png)
[演習-Pimpl](~~~)  

### lightweight Pimpl
[Pimpl](---)の解説で示したように依存関係をシンプルに保つには極めて有効なパターンではあるが、
このパターンで実装されたクラスのインスタンス化のたびに一回以上のヒープからのアロケーションが必要になるため、
このオーバーヘッドが気になるような場合に備えて、アロケーションを少なくするテクニックを以下に示す
(なお、lightweight Pimplとは筆者の造語であり、ここで紹介するパターンはPimplの一種である)。

```cpp
    // @@@ example/cpp_idioms/light_pimpl.h #0:0 begin
```

このクラスの実装を以下に示す。

```cpp
    // @@@ example/cpp_idioms/light_pimpl_ut.cpp #0:0 begin
    // @@@ example/cpp_idioms/light_pimpl_ut.cpp #0:1 begin
    // @@@ example/cpp_idioms/light_pimpl_ut.cpp #0:2 begin
    // @@@ example/cpp_idioms/light_pimpl_ut.cpp #0:3 begin
```

ヒープ以外のメモリからnewするための[プレースメントnew](---)を使用しているため、
上記の抜粋である以下のコードはやや見慣れないかもしれない。

```cpp
    // @@@ example/cpp_idioms/light_pimpl_ut.cpp #0:1 begin
```

プレースメントnewで構築したオブジェクトの解放にはdeleteは使えない。
オブジェクトがその上で構築されているメモリはヒープのものではないため、deleteすると未定義動作につながる。

deleteを使わずにプレースメントnewで構築したオブジェクトの各メンバのデストラクタを呼び出さなければ、
リソースリークにつながる。この問題を解決するためのコードは、上記の抜粋である以下のようなものになる。

```cpp
    // @@@ example/cpp_idioms/light_pimpl_ut.cpp #0:2 begin
```

上記のクラスの動作を以下の単体テストにより示す。

```cpp
    // @@@ example/cpp_idioms/light_pimpl_ut.cpp #1:0 begin -1
```

**[ 通常のPimplとの比較 ]**

| 特徴                 | 通常のPimpl                   | Lightweight Pimpl           |
|----------------------|-------------------------------|-----------------------------|
| メモリ確保           | ヒープ                        | スタック(オブジェクト内)    |
| アロケーション回数   | インスタンス毎に1回以上       | 0回                         |
| パフォーマンス       | new/deleteのオーバーヘッド    | 通常のPimplより良い         |
| 実装の複雑さ         | シンプル                      | やや複雑(プレースメントnew) |
| メモリサイズの柔軟性 | 高い                          | 低い(コンパイル時に固定)    |

### BitmaskType
下記のようなビットマスク表現は誤用しやすいインターフェースである。
修正や拡張等に関しても脆弱であるため、避けるべきである。

```cpp
    // @@@ example/cpp_idioms/enum_operator.h #0:0 begin
```

```cpp
    // @@@ example/cpp_idioms/enum_operator_ut.cpp #0:0 begin -1
```

上記のような誤用を防ぐために、
enumによるビットマスク表現を使用して型チェックを強化した例を以下に示す。
このテクニックは、STLのインターフェースとしても使用されている強力なイデオムである。

```cpp
    // @@@ example/cpp_idioms/enum_operator.h #1:0 begin
```

```cpp
    // @@@ example/cpp_idioms/enum_operator_ut.cpp #1:0 begin -1
```

この改善により、Animalのコンストラクタに域値外の値を渡すことは困難になった
(少なくとも不注意で間違うことはないだろう)。
この修正の延長で、Animal::GetPhisicalAbility()の戻り値もenumになり、これも誤用が難しくなった。

[演習-BitmaskType](~~~)  

### Future
[Future](https://ja.wikipedia.org/wiki/Future_%E3%83%91%E3%82%BF%E3%83%BC%E3%83%B3)とは、
並行処理のためのデザインパターンであり、別スレッドに何らかの処理をさせる際、
その結果の取得を、必要になるまで後回しにする手法である。

C++11では、std::future, std::promise, std::asyncによって実現できる。

まずは、C++03以前のスタイルから示す。

```cpp
    // @@@ example/cpp_idioms/future_ut.cpp #0:0 begin
```

上記は、

1. 時間がかかる処理を並行して行うために、スレッドを二つ作る。
2. それぞれの完了をthread::join()で待ち合わせる。
3. その結果を参照キャプチャによって受け取る。
4. その2つの結果を別の関数に渡す。

という処理を行っている。

この程度の単純なコードでは特に問題にはならないが、目的外の処理が多いことがわかるだろう。

次にFutureパターンによって上記をリファクタリングした例を示す。

```cpp
    // @@@ example/cpp_idioms/future_ut.cpp #0:1 begin
```

リファクタリングした例では、時間のかかる処理をstd::future型のオブジェクトにし、
その結果を必要とする関数に渡すことができるため、目的をよりダイレクトに表すことができる。

なお、

```cpp
    std::async(関数オブジェクト)
```

という形式を使った場合、関数オブジェクトは、

```cpp
    std::launch::async | std::launch::deferred
```

が指定されたとして実行される。この場合、

```cpp
    std::launch::deferred
```

の効果により、関数オブジェクトは、並行に実行されるとは限らない
(この仕様はランタイム系に依存しており、std::future::get()のコンテキストで実行されることもあり得る)。
従って、並行実行が必要な場合、上記例のように

```cpp
    std::launch::async
```

のみを明示的に指定するべきである。

[演習-Future](~~~)  


### Null Object
オブジェクトへのポインタを受け取った関数が
「そのポインタがnullptrでない場合、そのポインタが指すオブジェクトに何かをさせる」
というような典型的な条件文を削減するためのパターンである。

```cpp
    // @@@ example/cpp_idioms/null_object_ut.cpp #0:0 begin
```

上記例にNull Objectパターンを適用した例を下記する。

```cpp
    // @@@ example/cpp_idioms/null_object_ut.cpp #0:1 begin
```

この単純な例では、逆にソースコードが複雑化したように見えるが、

```cpp
    if(a != nullptr)
```

を頻繁に使うような関数、
クラスではソースコードの単純化やnullptrチェック漏れの防止に非常に有効である。

[演習-Null Object](~~~)  

### Cでのクラス表現
このドキュメントは、C++でのソフトウェア開発を前提としているため、
ここで示したコードもC++で書いているが、

* 何らかの事情でCを使わざるを得ないプログラマがデザインパターンを使用できるようにする
* クラスの理解が曖昧なC++プログラマの理解を助ける(「[ポリモーフィックなクラス](---)」参照)

ような目的のためにCでのクラスの実現方法を例示する。

下記のような基底クラスPointとその派生クラスPoint3Dがあった場合、

![ステートマシン図](plant_uml/class_c.png)

C++では、Pointのコードは下記のように表すことが一般的である。

```cpp
    // @@@ example/cpp_idioms/class_ut.cpp #0:0 begin
```

この単体テストは、下記のようになる。

```cpp
    // @@@ example/cpp_idioms/class_ut.cpp #0:1 begin -1
```

これをCで表した場合、下記のようになる。

```cpp
    // @@@ example/cpp_idioms/class_ut.cpp #2:0 begin
```

C++のメンバ関数はプログラマから見えない引数thisを持つ。
これを表したものが各関数の第1引数selfである。
また、ポリモーフィックな関数は関数ポインタで、
非ポリモーフィックな関数は通常の関数で表される。

この単体テストは、下記のようになる。

```cpp
    // @@@ example/cpp_idioms/class_ut.cpp #2:1 begin -1
```

Pointから派生したクラスPoint3DのC++での実装を以下に示す。

```cpp
    // @@@ example/cpp_idioms/class_ut.cpp #1:0 begin
```

この単体テストは、下記のようになる。

```cpp
    // @@@ example/cpp_idioms/class_ut.cpp #1:1 begin -1
```

これをCで実装したものが下記である。

```cpp
    // @@@ example/cpp_idioms/class_ut.cpp #3:0 begin
```

この単体テストは、下記のようになる。

```cpp
    // @@@ example/cpp_idioms/class_ut.cpp #3:1 begin -1
```

以上からわかる通り、Cでのクラス実装はC++のものに比べ、

* 記述が多い
* キャストを使わざるを得ない
* リファレンスが使えないため、NULLにならないハンドル変数をポインタにせざるを得ない

等といった問題があるため、「何らかの事情でC++が使えない」チームは、
なるべく早い時期にその障害を乗り越えることをお勧めする。

どうしてもその障害を超えられない場合は、
[モダンC言語プログラミング](https://www.amazon.co.jp/%E3%83%A2%E3%83%80%E3%83%B3C%E8%A8%80%E8%AA%9E%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0-%E3%82%A2%E3%82%B9%E3%82%AD%E3%83%BC%E6%9B%B8%E7%B1%8D-%E8%8A%B1%E4%BA%95-%E5%BF%97%E7%94%9F-ebook/dp/B00HWLJEKW)
が役に立つだろう。

## オブジェクト指向
### is-a
「is-a」の関係は、オブジェクト指向プログラミング（OOP）
においてクラス間の継承関係を説明する際に使われる概念である。
クラスDerivedとBaseが「is-a」の関係である場合、
DerivedがBaseの派生クラスであり、Baseの特性をDerivedが引き継いでいることを意味する。
C++でのOOPでは、DerivedはBaseのpublic継承として定義される。
通常DerivedやBaseは以下の条件を満たす必要がある。

* Baseはvirtualメンバ関数(Base::f)を持つ。
* DerivedはBase::fのオーバーライド関数を持つ。
* DerivedはBaseに対して
  [リスコフの置換原則](https://ja.wikipedia.org/wiki/%E3%83%AA%E3%82%B9%E3%82%B3%E3%83%95%E3%81%AE%E7%BD%AE%E6%8F%9B%E5%8E%9F%E5%89%87)
  を守る必要がある。
  この原則を簡単に説明すると、
  「派生クラスのオブジェクトは、
  いつでもその基底クラスのオブジェクトと置き換えても、
  プログラムの動作に悪影響を与えずに問題が発生してはならない」という設計の制約である。

 「is-a」の関係とは「一種の～」と言い換えることができることが多い.
ペンギンや九官鳥 は一種の鳥であるため、この関係を使用したコード例を次に示す。

```cpp
    // @@@ example/cpp_idioms/class_relation_ut.cpp #0:0 begin
```

bird::flyのオーバーライド関数(penguin::fly)について、[リスコフの置換原則(LSP)](---)に反した例を下記する。

```cpp
    // @@@ example/cpp_idioms/class_relation_ut.cpp #0:1 begin
    // @@@ example/cpp_idioms/class_relation_ut.cpp #0:2 begin -1
```

birdからpenguinへの派生がリスコフ置換の原則に反してしまった原因は以下のように考えることができる。

* bird::flyの事前条件penguin::flyが強めた
* bird::flyの事後条件をpenguin::flyが弱めた

penguinとbirdの関係はis-aの関係ではあるが、
上記コードの問題によって不適切なis-aの関係と言わざるを得ない。

上記の例では鳥全般と鳥の種類のis-a関係をpublic継承を使用して表した(一部不適切であるもの)。
さらにis-aの誤った適用例を示す。
自身が飼っている九官鳥に"キューちゃん"と名付けることはよくあることである。
キューちゃんという名前の九官鳥は一種の九官鳥であることは間違いのないことであるが、
このis-aの関係を表すためにpublic継承を使用するのは、is-aの関係の誤用になることが多い。
実際のコード例を以下に示す。この場合、型とインスタンスの概念の混乱が原因だと思われる。

```cpp
    // @@@ example/cpp_idioms/class_relation_ut.cpp #1:0 begin
```

この誤用を改めた例を以下に示す。

```cpp
    // @@@ example/cpp_idioms/class_relation_ut.cpp #2:0 begin
    // @@@ example/cpp_idioms/class_relation_ut.cpp #2:1 begin -1
```

修正されたKyukancho はstd::string インスタンスをメンバ変数として持ち、
kyukanchoとstd::stringの関係を[has-a](---)の関係と呼ぶ。


### has-a
「has-a」の関係は、
あるクラスのインスタンスが別のクラスのインスタンスを構成要素として含む関係を指す。
つまり、あるクラスのオブジェクトが別のクラスのオブジェクトを保持している関係である。

例えば、CarクラスとEngineクラスがあるとする。CarクラスはEngineクラスのインスタンスを含むので、
CarはEngineを「has-a」の関係にあると言える。
通常、has-aの関係はクラス内でメンバ変数またはメンバオブジェクトとして実装される。
Carクラスの例ではCarクラスにはEngine型のメンバ変数が存在する。

```cpp
    // @@@ example/cpp_idioms/class_relation_ut.cpp #3:0 begin
```

### is-implemented-in-terms-of
「is-implemented-in-terms-of」の関係は、
オブジェクト指向プログラミング（OOP）において、
あるクラスが別のクラスの機能を内部的に利用して実装されていることを示す概念である。
これは、あるクラスが他のクラスのインターフェースやメンバ関数を用いて、
自身の機能を提供する場合に使われる。
[has-a](---)の関係は、is-implemented-in-terms-of の関係の一種である。

is-implemented-in-terms-ofは下記の手段1-3に示した方法がある。

* 手段1.[public継承によるis-implemented-in-terms-of](---)  
* 手段2.[private継承によるis-implemented-in-terms-of](---)  
* 手段3.[コンポジションによる(has-a)is-implemented-in-terms-of](---)  

手段1-3にはそれぞれ、長所、短所があるため、必要に応じて手段を選択する必要がある。
以下の議論を単純にするため、下記のようにクラスS、C、CCを定める。

* S(サーバー): 実装を提供するクラス
* C(クライアント): Sの実装を利用するクラス
* CC(クライアントのクライアント): Cのメンバを使用するクラス

コード量の観点から考えた場合、手段1が最も優れていることが多い。
依存関係の複雑さから考えた場合、CはSに強く依存する。
場合によっては、この依存はCCからSへの依存間にも影響をあたえる。
従って、手段3が依存関係を単純にしやすい。
手段1は[is-a](---)に見え、以下に示すような問題も考慮する必要があるため、
可読性、保守性を劣化させる可能性がある。

```cpp
    // @@@ example/cpp_idioms/class_relation_ut.cpp #6:0 begin
    // @@@ example/cpp_idioms/class_relation_ut.cpp #6:1 begin -1
```

以上述べたように問題の多い手段1であるが、実践的には有用なパターンであり、
[CRTP(curiously recurring template pattern)](https://ja.wikibooks.org/wiki/More_C%2B%2B_Idioms/%E5%A5%87%E5%A6%99%E3%81%AB%E5%86%8D%E5%B8%B0%E3%81%97%E3%81%9F%E3%83%86%E3%83%B3%E3%83%97%E3%83%AC%E3%83%BC%E3%83%88%E3%83%91%E3%82%BF%E3%83%BC%E3%83%B3(Curiously_Recurring_Template_Pattern))
の実現手段でもあるため、一概にコーディング規約などで排除することもできない。


#### public継承によるis-implemented-in-terms-of
public継承によるis-implemented-in-terms-ofの実装例を以下に示す。

```cpp
    // @@@ example/cpp_idioms/class_relation_ut.cpp #7:0 begin
    // @@@ example/cpp_idioms/class_relation_ut.cpp #7:1 begin -1
```

すでに述べたようにこの方法は、
[private継承によるis-implemented-in-terms-of](---)や、
[コンポジションによる(has-a)is-implemented-in-terms-of](---)
と比べコードがシンプルになる。 

#### private継承によるis-implemented-in-terms-of
private継承によるis-implemented-in-terms-ofの実装例を以下に示す。

```cpp
    // @@@ example/cpp_idioms/class_relation_ut.cpp #4:0 begin
    // @@@ example/cpp_idioms/class_relation_ut.cpp #4:1 begin -1
```

この方法は、[public継承によるis-implemented-in-terms-of](---)が持つデストラクタ問題は発生せす、
[is-a](---)と誤解してしまう問題も発生しない。


#### コンポジションによる(has-a)is-implemented-in-terms-of
コンポジションによる(has-a)is-implemented-in-terms-ofの実装例を示す。

```cpp
    // @@@ example/cpp_idioms/class_relation_ut.cpp #5:0 begin
    // @@@ example/cpp_idioms/class_relation_ut.cpp #5:1 begin -1
```

この方は実装を利用するクラストの依存関係を他の2つに比べるとシンプルにできるが、
逆に実装例から昭なとおり、コード量が増えてしまう。

## オブジェクトの所有権
### オブジェクトの排他所有
オブジェクトの排他所有や、それを容易に実現するための
[std::unique_ptr](https://cpprefjp.github.io/reference/memory/unique_ptr.html)
の仕様を説明するために、下記のようにクラスA、Xを定義する。

```cpp
    // @@@ example/cpp_idioms/unique_ptr_ownership_ut.cpp #0:0 begin
```

下記に示した上記クラスの単体テストにより、
オブジェクトの所有権やその移動、
std::unique_ptr、std::move()、[expression|rvalue](---)の関係を解説する。

```cpp
    // @@@ example/cpp_idioms/unique_ptr_ownership_ut.cpp #0:1 begin -1
```

```cpp
    // @@@ example/cpp_idioms/unique_ptr_ownership_ut.cpp #0:2 begin -1
```

![所有権1](plant_uml/unique_ownership_1.png)

```cpp
    // @@@ example/cpp_idioms/unique_ptr_ownership_ut.cpp #0:3 begin -1
```

![所有権2](plant_uml/unique_ownership_2.png)

```cpp
    // @@@ example/cpp_idioms/unique_ptr_ownership_ut.cpp #0:4 begin -1
```
![所有権3](plant_uml/unique_ownership_3.png)

```cpp
    // @@@ example/cpp_idioms/unique_ptr_ownership_ut.cpp #0:5 begin -1
```

![所有権4](plant_uml/unique_ownership_4.png)

```cpp
    // @@@ example/cpp_idioms/unique_ptr_ownership_ut.cpp #0:6 begin -1
```

![所有権5](plant_uml/unique_ownership_5.png)

```cpp
    // @@@ example/cpp_idioms/unique_ptr_ownership_ut.cpp #0:7 begin -1
```

![所有権6](plant_uml/unique_ownership_6.png)


また、以下に見るようにstd::unique_ptrはcopy生成やcopy代入を許可しない。

```cpp
    // @@@ example/cpp_idioms/unique_ptr_ownership_ut.cpp #1:0 begin -1
```

以上で示したstd::unique_ptrの仕様の要点をまとめると、以下のようになる。

* std::unique_ptrはダイナミックに生成されたオブジェクトを保持する。
* ダイナミックに生成されたオブジェクトを保持するstd::unique_ptrがスコープアウトすると、
  保持中のオブジェクトは自動的にdeleteされる。
* 保持中のオブジェクトを他のstd::unique_ptrにmoveすることはできるが、
  copyすることはできない。このため、下記に示すような不正な方法以外で、
  複数のstd::unique_ptrが1つのオブジェクトを共有することはできない。

```cpp
    // @@@ example/cpp_idioms/unique_ptr_ownership_ut.cpp #2:0 begin -1
```

こういった機能によりstd::unique_ptrはオブジェクトの排他所有を実現している。

### オブジェクトの共有所有
オブジェクトの共有所有や、それを容易に実現するための
[std::shared_ptr](https://cpprefjp.github.io/reference/memory/shared_ptr.html)
の仕様を説明するために、下記のようにクラスA、Xを定義する。

```cpp
    // @@@ example/cpp_idioms/shared_ptr_ownership_ut.cpp #0:0 begin
```

下記に示した上記クラスの単体テストにより、
オブジェクトの所有権やその移動、共有、
std::shared_ptr、std::move()、[expression|rvalue](---)の関係を解説する。

```cpp
    // @@@ example/cpp_idioms/shared_ptr_ownership_ut.cpp #0:1 begin -1
```

```cpp
    // @@@ example/cpp_idioms/shared_ptr_ownership_ut.cpp #0:2 begin -1
```

![所有権1](plant_uml/shared_ownership_1.png)


```cpp
    // @@@ example/cpp_idioms/shared_ptr_ownership_ut.cpp #0:3 begin -1
```

![所有権2](plant_uml/shared_ownership_2.png)

```cpp
    // @@@ example/cpp_idioms/shared_ptr_ownership_ut.cpp #0:4 begin -1
```

![所有権3](plant_uml/shared_ownership_3.png)

```cpp
    // @@@ example/cpp_idioms/shared_ptr_ownership_ut.cpp #0:5 begin -1
```

![所有権4](plant_uml/shared_ownership_4.png)

```cpp
    // @@@ example/cpp_idioms/shared_ptr_ownership_ut.cpp #0:6 begin -1
```

![所有権5](plant_uml/shared_ownership_5.png)


```cpp
    // @@@ example/cpp_idioms/shared_ptr_ownership_ut.cpp #0:7 begin -1
```

![所有権6](plant_uml/shared_ownership_6.png)


```cpp
    // @@@ example/cpp_idioms/shared_ptr_ownership_ut.cpp #0:8 begin -1
```

![所有権7](plant_uml/shared_ownership_7.png)

以上で示したstd::shared_ptrの仕様の要点をまとめると、以下のようになる。

* std::shared_ptrはダイナミックに生成されたオブジェクトを保持する。
* ダイナミックに生成されたオブジェクトを保持するstd::shared_ptrがスコープアウトすると、
  共有所有カウントはデクリメントされ、その値が0ならば保持しているオブジェクトはdeleteされる。
* std::shared_ptrを他のstd::shared_ptrに、
    * moveすることことで、保持中のオブジェクトの所有権を移動できる。
    * copyすることことで、保持中のオブジェクトの所有権を共有できる。
* 下記のようなコードはstd::shared_ptrの仕様が想定する[セマンティクス](---)に沿っておらず、
  [未定義動作](---)に繋がる。

```cpp
    // @@@ example/cpp_idioms/shared_ptr_ownership_ut.cpp #1:0 begin -1
```

こういった機能によりstd::shared_ptrはオブジェクトの共有所有を実現している。


### オブジェクトの循環所有
[std::shared_ptr](https://cpprefjp.github.io/reference/memory/shared_ptr.html)の使い方を誤ると、
以下のコード例が示すようにメモリーリークが発生する。

なお、この節の題名である「オブジェクトの循環所有」という用語は、
この前後の節がダイナミックに確保されたオブジェクトの所有の概念についての解説しているため、
この用語を選択したが、文脈によっては、「オブジェクトの循環参照」といった方がふさわしい場合もある。

---

まずは、**メモリリークが発生しない**`std::shared_ptr`の正しい使用例を示す。

```cpp
    // @@@ example/cpp_idioms/shared_ptr_cycle_ut.cpp #0:0 begin
```

上記のクラスの使用例を示す。下記をステップ1とする。

```cpp
    // @@@ example/cpp_idioms/shared_ptr_cycle_ut.cpp #1:0 begin -1
```

![shread_ptrメモリーリーク](plant_uml/shared_each_1.png)


上記の続きを以下に示し、ステップ2とする。

```cpp
    // @@@ example/cpp_idioms/shared_ptr_cycle_ut.cpp #1:1 begin -1
```

![shread_ptrメモリーリーク](plant_uml/shared_each_2.png)


上記の続きを以下に示し、ステップ3とする。

```cpp
    // @@@ example/cpp_idioms/shared_ptr_cycle_ut.cpp #1:2 begin -1
```

![shread_ptrメモリーリーク](plant_uml/shared_each_3.png)


上記の続きを以下に示し、ステップ4とする。

```cpp
    // @@@ example/cpp_idioms/shared_ptr_cycle_ut.cpp #1:3 begin -1
```

![shread_ptrメモリーリーク](plant_uml/shared_each_4.png)


このような動作により、`std::make_shared<>`で生成されたX、Yオブジェクトは解放される。

---

次は**メモリリークが発生する**`std::shared_ptr`の誤用を示す。まずはクラスの定義から。

```cpp
    // @@@ example/cpp_idioms/shared_ptr_cycle_ut.cpp #2:0 begin
```

上記のクラスの動作を以下に示したコードで示す。

```cpp
    // @@@ example/cpp_idioms/shared_ptr_cycle_ut.cpp #2:1 begin -1
```

x0のライフタイムに差を作るために新しいスコープを導入し、そのスコープ内で、y0を生成し、
`X::Register`、`Y::Register`を用いて、循環を作ってしまう例(メモリーリークを起こすバグ)を示す。

```cpp
    // @@@ example/cpp_idioms/shared_ptr_cycle_ut.cpp #2:2 begin -1
```

![shread_ptrメモリーリーク](plant_uml/shared_cyclic.png)

下記のコードでは、y0がスコープアウトするが、そのタイミングでは、x0はまだ健在であるため、
Yオブジェクトの参照カウントは1になる(x0::y_が存在するため0にならない)。

```cpp
    // @@@ example/cpp_idioms/shared_ptr_cycle_ut.cpp #2:3 begin -1
```

![shread_ptrメモリーリーク](plant_uml/shared_cyclic_2.png)

ここでの状態をまとめると、

- y0がもともと持っていたXオブジェクトは健在(このオブジェクトはx0が持っているものでもあるため、use_countは2のまま)
- x0が宣言されたスコープが残っているため、当然ながらx0は健在
- x0はYオブジェクトを持ったままではあるが、y0がスコープアウトしたため、Yオブジェクトのuse_countは1に減った

  
次のコードでは、x0がスコープアウトし、y0がもともと持っていたXオブジェクトは健在であるため、
Xオブジェクトの参照カウントも1になる。このため、x0、y0がスコープアウトした状態でも、
X、Yオブジェクトの参照カウントは0にならず、従ってこれらのオブジェクトは解放されない
(shared_ptrは参照カウントが1->0に変化するタイミングで保持するオブジェクトを解放する)。

```cpp
    // @@@ example/cpp_idioms/shared_ptr_cycle_ut.cpp #2:4 begin -1
```

![shread_ptrメモリーリーク](plant_uml/shared_cyclic_3.png)

X、Yオブジェクトへの[ハンドル](---)を完全に失った状態であり、X、Yオブジェクトを解放する手段はない。

## copy/moveと等価性のセマンティクス
### 等価性のセマンティクス
純粋数学での実数の等号(=)は、任意の実数x、y、zに対して、

| 律   |意味                     |
|------|-------------------------|
|反射律|x = x                    |
|対称律|x = yならばy = x         |
|推移律|x = y且つy = zならばx = z|

を満たしている。x = yが成立する場合、「xはyと等しい」もしくは「xはyと同一」であると言う。

C++における組み込みの==も純粋数学の等号と同じ性質を満たしている。
下記のコードは、その性質を表している。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #0:0 begin -2
```

しかし、下記のコード内のa、bは同じ値を持つが、
アドレスが異なるため同一のオブジェクトではないにもかかわらず、組み込みの==の値はtrueとなる。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #0:1 begin -2
```

このような場合、aとbは等価であるという。同一ならば等価であるが、等価であっても同一とは限らない。

ポインタや配列をオペランドとする場合を除き、C++における組み込みの==は、
数学の等号とは違い、等価を表していると考えられるが、
上記した3つの律を守っている。従ってオーバーロードoperator==も同じ性質を守る必要がある。

組み込みの==やオーバーロード[==演算子](---)のこのような性質をここでは「等価性のセマンティクス」と呼ぶ。

クラスAを下記のように定義し、

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #1:0 begin
```

そのoperator==を下記のように定義した場合、

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #1:1 begin
```

単体テストは下記のように書けるだろう。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #1:2 begin -2
```

これは、一応パスするが(処理系定義の動作を前提とするため、必ず動作する保証はない)、
下記のようにすると、パスしなくなる。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #1:3 begin -2
```

一般にポインタの等価性は、その値の同一性ではなく、
そのポインタが指すオブジェクトの等価性で判断されるべきであるが、
先に示したoperator==はその考慮をしていないため、このような結果になった。

次に、これを修正した例を示す。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #1:4 begin
```

ポインタをメンバに持つクラスのoperator==については、上記したような処理が必要となる。

次に示す例は、基底クラスBaseとそのoperator==である。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #2:0 begin
```

次の単体テストが示す通り、これ自体には問題がないように見える。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #2:1 begin -1
```

しかし、Baseから派生したクラスDerivedを

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #3:0 begin
```

のように定義すると、下記の単体テストで示す通り、等価性のセマンティクスが破壊される。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #3:1 begin -1
```

Derived用のoperator==を

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #4:0 begin
```

と定義しても、下記に示す通り部分的な効果しかない。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #4:1 begin -1
```

この問題は、「[型とインスタンス|RTTI](---)」使った下記のようなコードで対処できる。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #5:0 begin
```

下記に示す通り、このコードは、
[オープン・クローズドの原則(OCP)](---)にも対応した柔軟な構造を実現している。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #5:1 begin
```

前例では「両辺の型が等しいこと」が「等価であること」の必要条件となるが、
この要件が、すべてのoperator==に求められるわけではない。

次に示すのは、一見すると両辺の型が違うにもかかわらず、
等価性のセマンティクスを満たしている例である。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #6:0 begin -1
```

これは、文字列リテラルを第1引数に取るstd::stringのコンストラクタが非explicitであることによって、
文字列リテラルからstd::stringへの[暗黙の型変換](---)が起こるために成立する。

以上で見てきたように、等価性のセマンティクスを守ったoperator==の実装には多くの観点が必要になる。

### copyセマンティクス
copyセマンティクスとは以下を満たすようなセマンティクスである。

* a = bが行われた後に、aとbが等価である。
* a = bが行われた前後でbの値が変わっていない。

従って、これらのオブジェクトに対して[等価性のセマンティクス](---)
を満たすoperator==が定義されている場合、
以下を満たすようなセマンティクスであると言い換えることができる。

* a = bが行われた後に、a == bがtrueになる。
* b == b_preがtrueの時に、a = bが行われた後でもb == b_preがtrueとなる。

下記に示す通り、std::stringはcopyセマンティクスを満たしている。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #7:0 begin -2
```

一方で、std::auto_ptrはcopyセマンティクスを満たしていない。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #7:1 begin -2
```

この仕様は極めて不自然であり、std::auto_ptrはC++11で非推奨となり、C++17で規格から排除された。

下記の単体テストから明らかな通り、
「[等価性のセマンティクス](---)」で示した最後の例も、copyセマンティクスを満たしていない。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #7:2 begin -2
```

原因は、copy代入で[オブジェクトのコピー|スライシング](---)が起こるためである。


### moveセマンティクス
moveセマンティクスとは以下を満たすようなセマンティクスである(operator==が定義されていると前提)。

* パフォーマンス要件  
    move代入の実行コスト <= copy代入の実行コスト(通常はmove代入の方が高速)

* 意味的要件  
    a == b が true の時に、c = std::move(a) を実行すると、  
    * b == c が true になる（値が保存される）
    * a == c は true にならなくても良い（aはmove後に不定状態になり得る）

* リソース管理   
    必須ではないが、aがポインタ等のリソースを保有している場合、
     move代入後にはそのリソースはcに移動していることが一般的である(「[expression|rvalue](---)」参照)

* エクセプション安全性  
    [no-fail保証](---)をする(noexceptと宣言し、エクセプションをthrowしない)

moveセマンティクスは、使用後に破棄されるオブジェクト(主にrvalue)からの代入処理の実行コストを削減するために導入された。

下記のクラスのmove代入の内部処理はcopy代入が行われており、
moveセマンティクスの目的である「パフォーマンスの向上」が達成されない。

そのため、このようなmove代入は避けるべきである。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #8:0 begin
```

下記のコードのようにメンバの代入もできる限りmove代入を使うことで、
パフォーマンスの良い代入ができる。

```cpp
    // @@@ example/cpp_idioms/semantics_ut.cpp #8:1 begin
```

### MoveAssignable要件
MoveAssignable要件は、C++において型がmove代入をサポートするために満たすべき条件を指す。
move代入はリソースを効率的に転送する操作であり、以下の条件を満たす必要がある。

1. リソースの移動  
   move代入では、リソース(動的メモリ等)が代入元から代入先へ効率的に転送される。

2. 有効だが未定義の状態  
   move代入後、代入元のオブジェクトは有効ではあるが未定義の状態となる。
   未定義の状態とは、破棄や再代入が可能である状態を指し、それ以外の操作は保証されない。

3. 自己代入の安全性  
   同一のオブジェクトをmove代入する場合でも、未定義動作やリソースリークを引き起こしてはならない。

4. 効率性  
   move代入は通常、copy代入よりも効率的であることが求められる。
   これは、リソースの複製を避けることで達成される(「[moveセマンティクス](---)」参照)。

5. デフォルト実装  
   move代入演算子が明示的に定義されていない場合でも、
   クラスが一定の条件(例: move不可能なメンバが存在しないこと)を満たしていれば、
   コンパイラがデフォルトの実装(「[オブジェクト生成と初期化|特殊メンバ関数](---)」参照)を生成する。

### CopyAssignable要件
CopyAssignable要件は、C++において型がcopy代入をサポートするために満たすべき条件を指す。

1. 動作が定義されていること  
   代入操作は未定義動作を引き起こしてはならない。自己代入（同じオブジェクトを代入する場合）においても正しく動作し、
   リソースリークを引き起こさないことが求められる。

2. 値の保持  
   代入後、代入先のオブジェクトの値は代入元のオブジェクトの値と一致していなければならない。

3. 正しいセマンティクス  
   copy代入によって代入元のオブジェクトが変更されてはならない(「[copyセマンティクス](---)」参照)。
   代入先のオブジェクトが保持していたリソース(例: メモリ)は適切に解放される必要がある。

4. デフォルト実装  
   copy代入演算子が明示的に定義されていない場合でも、
   クラスが一定の条件(例: copy不可能なメンバが存在しないこと)を満たしていれば、
   コンパイラがデフォルトの実装(「[オブジェクト生成と初期化|特殊メンバ関数](---)」参照)を生成する。

## 関数設計のガイドライン
### 関数の引数と戻り値の型
関数の引数型および戻り値型に関するガイドラインを以下の表で表す。

<table>
  <tr bgcolor="#cccccc">
    <th style="text-align: center;"> </th>
    <th style="text-align: center;">copy/moveが低コスト</th>
    <th style="text-align: center;">copyが高コスト/moveが低コスト</th>
    <th style="text-align: center;">moveが高コスト</th>
    <th style="text-align: center;">fがヌルを扱う</th>
  </tr>
  <tr>
    <td style="text-align: center;">in</td>
    <td style="text-align: center;"><code>f(X)</code></td>
    <td colspan="2" style="text-align: center;"><code>f(X const&)</code></td>
    <td style="text-align: center;"><code>f(X const\*)</code></td>
  </tr>
  <tr>
    <td style="text-align: center;">in/out</td>
    <td colspan="3" style="text-align: center;"><code>f(X&)</code></td>
    <td style="text-align: center;"><code>f(X\*)</code></td>
  </tr>
  <tr>
    <td style="text-align: center;">out</td>
    <td colspan="2" style="text-align: center;"><code>X f()</code></td>
    <td style="text-align: center;"><code>f(X&)</code></td>
    <td style="text-align: center;"><code>f(X\*)</code></td>
  </tr>
  <tr>
    <td style="text-align: center;">move</td>
    <td colspan="3" style="text-align: center;"><code>f(X&&)</code></td>
    <td style="text-align: center;">-</td>
  </tr>
  <tr>
    <td style="text-align: center;">forward</td>
    <td colspan="3" style="text-align: center;"><code>template&lt;typename T&gt; f(T&&)</code></td>
    <td style="text-align: center;">-</td>
  </tr>
</table>

[注] Xは任意の型  

[注] `templat<typename T> f(T&&)`の`T&&`は[forwardingリファレンス](---)である。  

[注] 以下のような引数型は避けるべきである。  

* `X const*`
* `X*`
* `X&`


### サイクロマティック複雑度のクライテリア
関数構造の適・不適については、[サイクロマティック複雑度](---)によって下記テーブルのように定義する。

| サイクロマティック複雑度(CC) | 複雑さの状態                                     |
| :--------------------------: | :----------------------------------------------- |
|            CC <= 10          | 非常に良い構造(適)                               |
|       11 < CC <  30          | やや複雑(概ね適)                                 |
|       31 < CC <  50          | 構造的なリスクあり(場合により不適)               |
|       51 < CC                | テスト不可能、デグレードリスクが非常に高い(不適) |

### 関数の行数のクライテリア
C++の創始者であるビャーネ・ストラウストラップ氏は、
  [プログラミング言語C++ 第4版](https://www.amazon.co.jp/%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0%E8%A8%80%E8%AA%9EC-%E7%AC%AC4%E7%89%88-%E3%83%93%E3%83%A3%E3%83%BC%E3%83%8D%E3%83%BB%E3%82%B9%E3%83%88%E3%83%A9%E3%82%A6%E3%82%B9%E3%83%88%E3%83%A9%E3%83%83%E3%83%97-ebook/dp/B01BGEO9MS)
  の中で、下記のように述べている。

```
    約 40 行を関数の上限にすればよい。 
    私自身は、もっと小さい平均 7 行程度を理想としている。 
```

## クラス設計のガイドライン
### ゼロの原則(Rule of Zero)
「ゼロの原則」は、リソース管理を直接クラスで行わず、
リソース管理を専門とするクラス
(例: 標準ライブラリの[RAII(scoped guard)](---)クラス)に任せる設計ガイドラインを指す。
この法則に従うと、自身で特殊メンバ関数を定義する必要がなくなる。

```cpp
    // @@@ example/cpp_idioms/rule_of_zero_ut.cpp #0:0 begin
```
```cpp
    // @@@ example/cpp_idioms/rule_of_zero_ut.cpp #0:1 begin -1
```

クラスがリソースを直接管理する場合、メモリリークや二重解放などのリスクを伴う。
上記のように信頼性の高いクラスに特殊メンバ関数の処理を任せることにより、
クラス自体にリソース管理の責任を持たせる必要がなくなる。

### 五の原則(Rule of Five)
「五の原則」は、
クラスがリソース(例: 動的メモリやファイルハンドルなど)を管理する場合、
デフォルトコンストラクタを除く[オブジェクト生成と初期化|特殊メンバ関数](---)、
つまり以下の5つの関数をすべて適切に定義する必要があるという設計ガイドラインを指す。

* デストラクタ
* コピーコンストラクタ
* コピー代入演算子
* ムーブコンストラクタ
* ムーブ代入演算子

特殊メンバ関数の挙動を正しく定義しないと、
リソースの不適切な管理(例: メモリリーク、リソースの二重解放)を招く可能性がある。
自動生成されるメンバ関数では、
複雑なリソース管理の要件を満たせないことがある(「[シャローコピー](---)」参照)。

なお、「五の原則」は、「六の原則」と呼ばれることもある。
その場合、この原則が対象とする関数は、
[オブジェクト生成と初期化|特殊メンバ関数](---)のすべてとなる。

このガイドラインに従って、コピーやムーブを実装する場合、

* [等価性のセマンティクス](---)
* [copyセマンティクス](---)
* [moveセマンティクス](---)

に従わなけならない。

### クラス凝集性のクライテリア
クラス構造の適・不適については、[LCOM](---)によって下記テーブルのように定義する。

| 凝集性の欠如(LCOM)  |  クラスの状態              |
|:-------------------:|:--------------------------:|
|       `LCOM <= 0.4` | 理想的な状態(適)           |
|`0.4 <  LCOM <  0.6` | 要注意状態(場合により不適) |
|`0.6 <= LCOM`        | 改善必須状態(不適)         |


* `LCOM <= 0.4`  
  クラスが非常に凝集しており、[単一責任の原則(SRP)](---)を強く遵守している状態であるため、
  通常、デザインの見直しは不要である。

* `0.4 < LCOM < 0.6`  
  クラスの凝集性がやや弱くなり始めている。
  デザイン見直しの必要な時期が迫りつつあると考えるべきだろう。
  このタイミングであればリファクタリングは低コストで完了できるだろう。

* `0.6 <= LCOM`  
  クラス内のメンバ関数間の関連性が低く、凝集性が不十分である。
  メンバ関数が異なる責務にまたがっている可能性が高いため、
  一刻も早くデザインの見直しを行うべきだろう。

## Modern CMake project layout
[Modern CMake project layout](https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html)
はパッケージ単位でディレクトリを分割し、各パッケージが独立したビルド単位となる構造である。
このような構造はビルドツールに[CMake](https://cliutils.gitlab.io/modern-cmake/)を使用する場合は特に有効であるが、
makeプロジェクトにおいても有効である。  

[注]
この節でのパッケージとはUMLでのパッケージとは同じソフトウェアの構造の単位であるが、
CMakeでの`find_package`でのパッケージとは意味が異なる。


__[主な特徴]__  

* 公開ヘッダと実装ファイルの明確な分離
* 各パッケージに独自のCMakeLists.txtを配置
* テストコードを各パッケージに併置
* target_include_directories()のPUBLIC/PRIVATEでAPI境界を制御

この構造により、パッケージ間の依存関係が明示化され、モジュール性とテスタビリティが向上する。

__[ディレクトリ構造例]__  

```
    project/
    ├── CMakeLists.txt
    ├── app/
    │   └── main.cpp
    ├── core/
    │   ├── CMakeLists.txt
    │   ├── include/                # 公開ヘッダ
    │   │   └── core/
    │   │     ├── engine.h
    │   │     └── logger.h 
    │   ├── src/                    # 実装ファイル
    │   │   ├── engine.cpp
    │   │   └── internal.h          # 内部ヘッダ
    │   └── tests/                  # 単体テスト
    │       └── engine_test.cpp
    └── logger/
        ├── CMakeLists.txt
        ├── include/
        │   └── logger/
        │       └── logger.h
        ├── src/
        │   └── logger.cpp
        └── tests/
            └── logger_test.cpp
```

ディレクトリ構造例のcore/include/coreの構造は一見冗長に見えるが、
コンパイラに指定するインクルードパスを各パッケージのincludeディレクトリを指定することにより、
パッケージ外部の実装ファイルのインクルードセクションは以下のように記述される。

```cpp
  // インクルードセクション
  #include "core/logger.h"          // パッケージcoreからのインポート
  #include "logger/logger.h"        // パッケージloggerからのインポート
```

このような記述には以下のようなメリットがある。

- ヘッダ名の衝突を避けることができる
- main.cppのインクルードセクションの可読性が向上する

  
__[core/CMakeLists.txt例]__  

```
    # coreライブラリの定義
    add_library(core STATIC
        src/engine.cpp
    )

    # インクルードディレクトリの設定
    # PUBLIC: このライブラリを使う側にも公開されるヘッダパス
    # PRIVATE: このライブラリ内部でのみ使用するヘッダパス
    target_include_directories(core
        PUBLIC 
            ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    # 単体テストの定義
    add_executable(core_test tests/engine_test.cpp)

    # テストがcoreライブラリに依存することを宣言
    target_link_libraries(core_test PRIVATE core)
```

__[トップレベルCMakeLists.txt例]__  

```
    cmake_minimum_required(VERSION 3.15)
    project(MyProject)

    # 各コンポーネントをサブディレクトリとして追加
    add_subdirectory(core)
    add_subdirectory(logger)

    # アプリケーション実行ファイルの定義
    add_executable(app app/main.cpp)

    # アプリケーションが依存するライブラリを指定
    # PRIVATE: appの実装内部でのみ使用（他のターゲットにはヘッダパスを公開しない）
    target_link_libraries(app PRIVATE core logger)
```

### Modern CMake project layoutのカスタマイズ
このドキュメントでは、以下の方針に基づいて[Modern CMake project layout](---)の構成をカスタマイズすることを推奨する。

- パス名が過度に長くなることを避ける。
- `tests`（または `test`）という語は統合テストを指す場合もあるため、
  曖昧さを避ける目的で、単体テストには `ut`（unit test）という語を使用する。

そのため、以下のような置き換えを推奨する。

| オリジナル       | カスタマイズ |
|------------------|--------------|
| include/         | h/           |
| tests/           | ut/          |
| xxx_test.cpp     | xxx_ut.cpp   |


__[置き換え後のディレクトリ構造例]__  

```
    project/
    ├── CMakeLists.txt
    ├── app/
    │   └── main.cpp
    ├── core/
    │   ├── CMakeLists.txt
    │   ├── h/                      # 公開ヘッダ
    │   │   └── core/
    │   │     ├── engine.h
    │   │     └── logger.h 
    │   ├── src/                    # 実装ファイル
    │   │   ├── engine.cpp
    │   │   └── internal.h          # 内部ヘッダ
    │   └── ut/                     # 単体テスト
    │       └── engine_ut.cpp
    └── logger/
        ├── CMakeLists.txt
        ├── h/
        │   └── logger/
        │       └── logger.h
        ├── src/
        │   └── logger.cpp
        └── ut/
            └── logger_ut.cpp
```

## コーディングスタイル
### AAAスタイル
このドキュメントでのAAAとは、単体テストのパターンarrange-act-assertではなく、
almost always autoを指し、
AAAスタイルとは、「可能な場合、型を左辺に明示して変数を宣言する代わりに、autoを使用する」
というコーディングスタイルである。
この用語は、Andrei Alexandrescuによって造られ、Herb Sutterによって広く推奨されている。

特定の型を明示して使用する必要がない場合、下記のように書く。

```cpp
    // @@@ example/cpp_idioms/aaa.cpp #0:0 begin -1
```

特定の型を明示して使用する必要がある場合、下記のように書く。

```cpp
    // @@@ example/cpp_idioms/aaa.cpp #1:0 begin -1
```

関数の戻り値を受け取る変数を宣言する場合、下記のように書く。

```cpp
    // @@@ example/cpp_idioms/aaa.cpp #2:0 begin -1
```

ただし、関数の戻り値型が容易に推測しがたい下記のような場合、
型を明示しないAAAスタイルは使うべきではない。

```cpp
    // @@@ example/cpp_idioms/aaa.cpp #2:1 begin -1
```

インライン関数や関数テンプレートの宣言は、下記のように書く。

```cpp
    // @@@ example/cpp_idioms/aaa.cpp #3:0 begin
```

ただし、インライン関数や関数テンプレートが複雑な下記のような場合、
AAAスタイルは出来る限り避けるべきである。

```cpp
    // @@@ example/cpp_idioms/aaa.cpp #3:1 begin
```

このスタイルには下記のような狙いがある。

* コードの安全性の向上  
  autoで宣言された変数は未初期化にすることができないため、未初期化変数によるバグを防げる。
  また、下記のように縮小型変換(下記では、unsignedからsignedの変換)を防ぐこともできる。

```cpp
    // @@@ example/cpp_idioms/aaa.cpp #4:0 begin -1
```

* コードの可読性の向上  
  冗長なコードを排除することで、可読性の向上が見込める。

* コードの保守性の向上  
  「変数宣言時での左辺と右辺を同一の型にする」非AAAスタイルは
  [DRYの原則](https://ja.wikipedia.org/wiki/Don%27t_repeat_yourself#:~:text=Don't%20repeat%20yourself%EF%BC%88DRY,%E3%81%A7%E3%81%AA%E3%81%84%E3%81%93%E3%81%A8%E3%82%92%E5%BC%B7%E8%AA%BF%E3%81%99%E3%82%8B%E3%80%82)
  に反するが、この観点において、AAAスタイルはDRYの原則に沿うため、
  コード修正時に型の変更があった場合でも、それに付随したコード修正を最小限に留められる。


AAAスタイルでは、以下のような場合に注意が必要である。

* 関数の戻り値をautoで宣言された変数で受ける場合  
  上記で述べた通り、AAAの過剰な仕様は、可読性を下げてしまう。

* autoで推論された型が直感に反する場合  
  下記のような型推論は、直感に反する場合があるため、autoの使い方に対する習熟が必要である。

```cpp
    // @@@ example/cpp_idioms/aaa.cpp #5:0 begin -1
```
 
### east-const
east-constとは、`const`修飾子を修飾する型要素の右側(east＝右)に置くコーディングスタイルのこと。
つまり「`const`はどの対象を修飾するか」を明確にするため、被修飾対象の直後に const を書くのが特徴である。

このスタイルは、C言語由来の「`const`を左に置く」スタイル([west-const](---))に比べ、
テンプレート展開や型推論の際に一貫性があり、C++コミュニティではしばしば論理的・直感的と評価されている。

```cpp
    // @@@ example/cpp_idioms/east_west_const.cpp #0:0 begin -1
```

このスタイルは 「east constスタイル」 または 「右側const」と呼ばれ、
typeid のデマングル結果や Itanium C++ ABI でもこの形式が採用されている。

なお、このドキュメントでは、このスタイルを採用している。

### west-const
west-constとは、`const`修飾子を型の左側(west＝左)に置くコーディングスタイルのこと。
C言語からの伝統的な表記法であり、多くの標準ライブラリや教科書でも依然としてこの書き方が用いられている。

可読性は慣れに依存するが、`const`の位置が一貫しないケース(`T* const`など)では理解しづらくなることもある。

```cpp
    // @@@ example/cpp_idioms/east_west_const.cpp #0:1 begin -1
```

このスタイルは「west constスタイル」または「左側const」と呼ばれ、
C言語文化圏での可読性・慣習を重視する場合に採用されることが多い。

### Trailing Underscore（末尾アンダースコア）
Trailing underscoreとは、C++においてメンバー変数名の末尾にアンダースコア
(\_)を付ける命名規約である。例えば、data_、count_、name_ のように記述する。

__採用の背景__  
この規約が広まった主な理由は以下の通りである：  

* 予約識別子との衝突回避 - 先頭のアンダースコアは標準で予約されている(\_+大文字、\_\_など)ため使用できない
* 可読性の向上 - プレフィックス方式(m_dataなど)と比べて、自然な語順を保てる
* コンストラクタでの利便性 - 初期化リストで `data_{data}` のようにパラメータ名と区別しやすい

__主要な採用例__

* Google C++ Style Guide
* Scott Meyers著「Effective C++」シリーズ
* 多くのオープンソースプロジェクト
* このドキュメント

この規約により、メンバー変数とローカル変数を明確に区別でき、コードの保守性が期待できる。


## オブジェクトのコピー
### シャローコピー
シャローコピー(浅いコピー)とは、暗黙的、
もしくは=defaultによってコンパイラが生成するようなcopyコンストラクタ、
copy代入演算子が行うコピーであり、[ディープコピー](---)と対比的に使われる概念である。

以下のクラスShallowOKには、コンパイラが生成するcopyコンストラクタ、
copy代入演算子と同等なものを定義したが、これは問題のないシャローコピーである
(が、正しく自動生成される関数を実装すると、メンバ変数が増えた際にバグを生み出すことがあるため、
実践的にはこのようなことはすべきではない)。

```cpp
    // @@@ example/cpp_idioms/deep_shallow_copy_ut.cpp #0:0 begin
```

コンストラクタでポインタのようなリソースを確保し、
デストラクタでそれらを解放するようなクラスの場合、シャローコピーは良く知られた問題を起こす。

下記のShallowNGはその例である。

```cpp
    // @@@ example/cpp_idioms/deep_shallow_copy_ut.cpp #1:0 begin
```

シャローコピーにより、メンバで保持していたポインタ(ポインタが指しているオブジェクトではない)
がコピーされてしまうため、下記のコード内のコメントで示した通り、
メモリリークや2重解放を起こしてしまう。

```cpp
    // @@@ example/cpp_idioms/deep_shallow_copy_ut.cpp #1:1 begin -1
```

### ディープコピー
ディープコピーとは、[シャローコピー](---)が発生させる問題を回避したコピーである。

以下に例を示す。

```cpp
    // @@@ example/cpp_idioms/deep_shallow_copy_ut.cpp #2:0 begin
```

上記クラスのDeepは、copyコンストラクタ、copy代入演算子でポインタをコピーするのではなく、
ポインタが指しているオブジェクトを複製することにより、シャローコピーの問題を防ぐ。


### スライシング
オブジェクトのスライシングとは、

* クラスBaseとその派生クラスDerived
* クラスDerivedのインスタンスd1、d2(解説のために下記例ではd0も定義)
* d2により初期化されたBase&型のd2_ref(クラスBase型のリファレンス)

が宣言されたとした場合、 

```cpp
    d2_ref = d1;    // オブジェクトの代入
```

を実行した時に発生するようなオブジェクトの部分コピーのことである
(この問題はリファレンスをポインタに代えた場合にも起こる)。

以下のクラスと単体テストはこの現象を表している。

```cpp
    // @@@ example/cpp_idioms/slice_ut.cpp #0:0 begin
```

```cpp
    // @@@ example/cpp_idioms/slice_ut.cpp #0:1 begin -1
```

copy代入演算子(=)によりコピーが行われた場合、=の両辺のオブジェクトは等価になるべきだが
(copy代入演算子をオーバーロードした場合も、そうなるように定義すべきである)、
スライシングが起こった場合、そうならないことが問題である(「[等価性のセマンティクス](---)」参照)。

下記にこの現象の発生メカニズムについて解説する。

1. 上記クラスBase、Derivedのメモリ上のレイアウトは下記のようになる。

![スライシングクラス](plant_uml/slicing_class.png)

2. 上記インスタンスd0、d1、d2、d2_refのメモリ上のレイアウトは下記のようになる。

![スライシング初期化](plant_uml/slicing_init.png)

3. d2 = d0をした場合の状態は下記のようになる。

![スライシング問題なし](plant_uml/slicing_normal.png)

4. 上記の状態でd2_ref = d1をした場合の状態は下記のようになる。

![スライシング問題](plant_uml/slicing_slicing.png)

d2.name1\_の値が元のままであるが(これがスライシングである)、その理由は下記の疑似コードが示す通り、
「d2_refの表層型がクラスBaseであるためd1もクラスBase(正確にはBase型へのリファレンス)へ変換された後、
d2_refが指しているオブジェクト(d2)へコピーされた」からである。

```cpp
    d2_ref.Base::operator=(d1);   // Base::operator=(Base const&)が呼び出される
                                  // 関数Base::operator=(Base const&)の中では、
                                  // d1の型はBase型のリファレンスとなる
```

次に示すのは、
「オブジェクトの配列をその基底クラスへのポインタに代入し、
そのポインタを配列のように使用した場合に発生する」スライシングと類似の現象である。

```cpp
    // @@@ example/cpp_idioms/slice_ut.cpp #1:0 begin -1
```

![スライシング配列](plant_uml/slicing_array.png)


## C++注意点
### オーバーライドとオーバーロードの違い
下記例では、Base::g()がオーバーロードで、Derived::f()がオーバーライドである
(Derived::g()はオーバーロードでもオーバーライドでもない(「[name-hiding](---)」参照))。


```cpp
    // @@@ example/cpp_idioms/override_overload_ut.cpp #0:0 begin
```

下記図の通り、

* BaseのインスタンスはBase用のvtblへのポインタを内部に持ち、
  そのvtblでBase::f()のアドレスを保持する。
* DerivedのインスタンスはDerived用のvtblへのポインタを内部に持ち、
  そのvtblでDerived::f()のアドレスを保持する。
* Base::g()、Base::g(int)、
  Derived::g()のアドレスはBaseやDerivedのインスタンスから辿ることはできない。

![vtbl](plant_uml/vtbl.png)

vtblとは仮想関数テーブルとも呼ばれる、仮想関数ポインタを保持するための上記のようなテーブルである
(「[ポリモーフィックなクラス](---)」参照)。

Base::f()、Derived::f()の呼び出し選択は、オブジェクトの表層の型ではなく、実際の型により決定される。
Base::g()、Derived::g()の呼び出し選択は、オブジェクトの表層の型により決定される。

```cpp
    // @@@ example/cpp_idioms/override_overload_ut.cpp #0:1 begin -1
```

上記のメンバ関数呼び出し

```cpp
    d_ref.f() 
```

がどのように解釈され、Derived::f()が選択されるかを以下に疑似コードで例示する。

```cpp
    vtbl = d_ref.vtbl             // d_refの実態はDerivedなのでvtblはDerivedのvtbl

    member_func = vtbl->f         // vtbl->fはDerived::f()のアドレス

    (d_ref.*member_func)(&d_ref)  // member_func()の呼び出し
```

このようなメカニズムにより仮想関数呼び出しが行われる。

### danglingリファレンス
Dangling リファレンスとは、破棄後のオブジェクトを指しているリファレンスを指す。
このようなリファレンスにアクセスすると、[未定義動作](---)に繋がるに繋がる。

```cpp
    // @@@ example/cpp_idioms/dangling_ut.cpp #0:0 begin

    // @@@ example/cpp_idioms/dangling_ut.cpp #0:1 begin -1
```

### danglingポインタ
danglingポインタとは、[danglingリファレンス](---)と同じような状態になったポインタを指す。


### Most Vexing Parse
Most Vexing Parse(最も困惑させる構文解析)とは、C++の文法に関連する問題で、
Scott Meyersが彼の著書"Effective STL"の中でこの現象に名前をつけたことに由来する。

この問題はC++の文法が関数の宣言と変数の定義とを曖昧に扱うことによって生じる。
特にオブジェクトの初期化の文脈で発生し、意図に反して、その行は関数宣言になってしまう。

```cpp
    // @@@ example/cpp_idioms/most_vexing_parse_ut.cpp #0:0 begin

    // @@@ example/cpp_idioms/most_vexing_parse_ut.cpp #0:1 begin -1
```

[特殊メンバ関数|初期化子リストコンストラクタ](---)の呼び出しでオブジェクトの初期化を行うことで、
このような問題を回避できる。

## ソフトウェア一般
### ヒープ
ヒープとは、プログラム実行時に動的メモリ割り当てを行うためのメモリ領域である。
malloc、calloc、reallocといった関数を使用して必要なサイズのメモリを確保し、freeで解放する。
スタックとは異なり、プログラマが明示的にメモリ管理を行う必要があり、解放漏れはメモリリークを引き起こす。
ヒープ領域はスタックよりも大きく、動的なサイズのデータ構造や長寿命のオブジェクトに適しているが、
アクセス速度はスタックより遅い。また、断片化（フラグメンテーション）が発生しやすく、
連続的な割り当てと解放により利用可能なメモリが分散する課題がある。適切なヒープ管理は、
C/C++プログラミングにおける重要なスキルの一つである。

### スレッドセーフ
スレッドセーフとは「複数のスレッドから同時にアクセスされても、
排他制御などの機構([std::mutex](---))により共有データの整合性が保たれ、正しく動作する性質」である。

### リエントラント
リエントラントとは「実行中に同じ関数が再度呼び出されても、グローバル変数や静的変数に依存せず、
ローカル変数のみで動作するため正しく動作する性質」である。

一般に、リエントラントな関数は[スレッドセーフ](---)であるが、逆は成り立たない。


### クリティカルセクション
複数のスレッドから同時にアクセスされると競合状態を引き起こす可能性があるコード領域をクリティカルセクションと呼ぶ。
典型的には、共有変数や共有データ構造を読み書きするコード部分がこれに該当する。
クリティカルセクションは、[std::mutex](---)等の排他制御機構によって保護し、
一度に一つのスレッドのみが実行できるようにする必要がある。

### スピンロック
スピンロックとは、
スレッドがロックを取得できるまでCPUを占有したままビジーループで待機する排他制御方式である。
スリープを伴わずカーネルを呼び出さないため、短時間の競合では高速に動作するが、
長時間の待機ではCPUを浪費しやすい。リアルタイム処理や割り込み制御に適する。

C++11では、スピンロックは[std::atomic](---)を使用して以下のように定義できる。

```cpp
    // @@@ h/spin_lock.h #0:0 begin
```

以下の単体テスト(「[std::mutex](---)」の単体テストを参照)
に示したように[std::scoped_lock](---)のテンプレートパラメータとして使用できる。

```cpp
    // @@@ example/cpp_idioms/spin_lock_ut.cpp #0:0 begin
```
```cpp
    // @@@ example/cpp_idioms/spin_lock_ut.cpp #0:1 begin -1
```

### ミックスイン
ミックスインとは、オブジェクト指向プログラミングにおいて、
複数のクラスに対して特定の機能やメソッドを提供するための設計パターンである。
「混ぜ込む（mix in）」という名称が示すとおり、既存のクラスに機能を追加する目的で使用される。

C++では[CRTP(curiously recurring template pattern)](---)や通常の継承によってミックスインを実現する。

### ハンドル
CやC++の文脈でのハンドルとは、ポインタかリファレンスを指す。

### フリースタンディング環境
[フリースタンディング環境](https://ja.wikipedia.org/wiki/%E3%83%95%E3%83%AA%E3%83%BC%E3%82%B9%E3%82%BF%E3%83%B3%E3%83%87%E3%82%A3%E3%83%B3%E3%82%B0%E7%92%B0%E5%A2%83)
とは、組み込みソフトウェアやOSのように、その実行にOSの補助を受けられないソフトウエアを指す。

### サイクロマティック複雑度
[サイクロマティック複雑度](https://ja.wikipedia.org/wiki/%E5%BE%AA%E7%92%B0%E7%9A%84%E8%A4%87%E9%9B%91%E5%BA%A6)
とは関数の複雑さを表すメトリクスである。


### 凝集性
[凝集性(凝集度)](https://ja.wikipedia.org/wiki/%E5%87%9D%E9%9B%86%E5%BA%A6)
とはクラス設計の妥当性を表す尺度の一種であり、
「[凝集性の欠如](---)(LCOM)」というメトリクスで計測される。

* [凝集性の欠如](---)メトリクスの値が1に近ければ凝集性は低く、この値が0に近ければ凝集性は高い。
* メンバ変数やメンバ関数が多くなれば、凝集性は低くなりやすい。
* 凝集性は、クラスのメンバがどれだけ一貫した責任を持つかを示す。
* 「[単一責任の原則(SRP)](---)」を守ると凝集性は高くなりやすい。
* 「[Accessor](---)」を多用すれば、振る舞いが分散しがちになるため、通常、凝集性は低くなる。
   従って、下記のようなクラスは凝集性が低い。言い換えれば、凝集性を下げることなく、
   より小さいクラスに分割できる。
   なお、以下のクラスでは、LCOMが位置にに近い値となっており、凝集性が欠如していることがわかる。

```cpp
    // @@@ example/cpp_idioms/lack_of_cohesion_ut.cpp #0:0 begin
```

良く設計されたクラスは、下記のようにメンバが結合しあっているため凝集性が高い
(ただし、「[Immutable](---)」の観点からは、QuadraticEquation::Set()がない方が良い)。
言い換えれば、凝集性を落とさずにクラスを分割することは難しい。
なお、上記の凝集性を欠くクラスを凝集性が高くなるように修正した例を以下に示す。

```cpp
    // @@@ example/cpp_idioms/lack_of_cohesion_ut.cpp #0:1 begin
```

#### 凝集性の欠如
[ソフトウェア一般|凝集性](---)の欠如(Lack of Cohesion in Methods/LCOM)とは、
クラス設計の妥当性を表す尺度の一種であり、`0 ～ 1`の値で表すメトリクスである。

LCOMの値が大きい(1か1に近い値)場合、「クラス内のメンバ関数が互いに関連性を持たず、
それぞれが独立した責務やデータに依存するため、クラス全体の統一性が欠けている」ことを表す。

クラスデザイン見直しの基準値としてLCOMを活用する場合、[クラス凝集性のクライテリア](---)に具体的な推奨値を示す。

#### LCOM
[凝集性の欠如](---)とはLack of Cohesion in Methodsの和訳であり、LCOMと呼ばれる。

### Spurious Wakeup
[Spurious Wakeup](https://en.wikipedia.org/wiki/Spurious_wakeup)とは、
条件変数に対する通知待ちの状態であるスレッドが、その通知がされていないにもかかわらず、
起き上がってしまう現象のことを指す。

下記のようなstd::condition_variableの使用で起こり得る。

```cpp
    // @@@ example/cpp_idioms/spurious_wakeup_ut.cpp #0:0 begin
```

std::condition_variable::wait()の第2引数を下記のようにすることでこの現象を回避できる。

```cpp
    // @@@ example/cpp_idioms/spurious_wakeup_ut.cpp #1:0 begin
```

### Static Initialization Order Fiasco(静的初期化順序問題)
静的初期化順序問題とは、
グローバルや名前空間スコープの静的オブジェクトの初期化順序が翻訳単位間で未定義であることに起因する不具合である。
あるオブジェクトAが初期化時に別のオブジェクトBに依存していても、Bがまだ初期化されていない場合、
Aの初期化は未定義の状態となり、不正アクセスやクラッシュを引き起こす可能性がある。

原因は、C++標準が同じ翻訳単位内の静的オブジェクトの初期化順序は保証するが、
異なる翻訳単位間の順序は保証しないことにある。さらに、動的初期化を必要とするオブジェクトでは、
初期化順序の依存関係が問題を起こす。

C++20からこの問題の対策として、[constinit](---)が導入された。

### 副作用
プログラミングにおいて、式の評価による作用には、
主たる作用とそれ以外の
[副作用](https://ja.wikipedia.org/wiki/%E5%89%AF%E4%BD%9C%E7%94%A8_(%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%A0))
(side effect)とがある。
式は、評価値を得ること(関数では「引数を受け取り値を返す」と表現する)が主たる作用とされ、
それ以外のコンピュータの論理的状態(ローカル環境以外の状態変数の値)を変化させる作用を副作用という。
副作用の例としては、グローバル変数や静的ローカル変数の変更、
ファイルの読み書き等のI/O実行、等がある。


### Itanium C++ ABI
ItaniumC++ABIとは、C++コンパイラ間でバイナリ互換性を確保するための規約である。
関数呼び出し規約、クラスレイアウト、仮想関数テーブル、例外処理、
名前修飾(マングリング)などC++のオブジェクト表現と呼び出し方法に関する標準ルールを定めている。

もともとはIntelItanium(IA-64)プロセッサ向けに策定されたが、
[g++](---)や[clang++](---)はx86/x86-64やARM64など多くのプラットフォームでもItaniumC++ABI準拠の規約を採用している。
そのため異なるコンパイラ間でもオブジェクトファイルやライブラリのリンクが可能である。
また、typeid(...).name()をデマングルした場合、
constがeast-const形式(T const)で表示されるのもこのABIの規約によるものである。

| ABI               | 主な対象プラットフォーム  | マングリング規則    | クラスレイアウト  | 例外処理    |
| ----------------- | ------------------------- | ------------------- | ----------------- | ----------- |
| **ItaniumC++ABI** | IA-64, x86, x86-64, ARM64 | east-const形式      | Itanium規則       | Itanium規則 |
| **MSVC C++ABI**   | Windows x86/x64           | 独自形式            | 独自規則          | 独自規則    |
| **ARM C++ABI**    | AArch32, AArch64          | 基本的にItanium準拠だが例外あり         | ARM規則     |


## C++コンパイラ
本ドキュメントで使用するg++/clang++のバージョンは以下のとおりである。

### g++
```
    g++ (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0
    Copyright (C) 2021 Free Software Foundation, Inc.
    This is free software; see the source for copying conditions.  There is NO
    warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

### clang++
```
    Ubuntu clang version 14.0.0-1ubuntu1
    Target: x86_64-pc-linux-gnu
    Thread model: posix
    InstalledDir: /usr/bin
```

## 非ソフトウェア用語
### セマンティクス
シンタックスとは構文論のことであり、セマンティクスとは意味論のことである。
セマンティクス、シンタックスの違いをはっきりと際立たせる以下の有名な例文により、
セマンティクスの意味を直感的に理解することができる。

```
    Colorless green ideas sleep furiously(直訳:無色の緑の考えが猛烈に眠る)
```

この文は文法的には正しい(シンタックス的に成立している)が、意味的には不自然で理解不能である
(セマンティクス的には破綻している)。ノーム・チョムスキーによって提示されたこの例文は、
構文が正しくても意味が成立しないことがあるという事実を示しており、構文と意味の違いを鮮やかに浮かび上がらせる。


### 割れ窓理論
[割れ窓理論](https://ja.wikipedia.org/wiki/%E5%89%B2%E3%82%8C%E7%AA%93%E7%90%86%E8%AB%96)とは、
軽微な犯罪も徹底的に取り締まることで、凶悪犯罪を含めた犯罪を抑止できるとする環境犯罪学上の理論。
アメリカの犯罪学者ジョージ・ケリングが考案した。
「建物の窓が壊れているのを放置すると、誰も注意を払っていないという象徴になり、
やがて他の窓もまもなく全て壊される」との考え方からこの名がある。

ソフトウェア開発での割れ窓とは、「朝会に数分遅刻する」、「プログラミング規約を守らない」
等の軽微なルール違反を指し、この理論の実践には、このような問題を放置しないことによって、

* チームのモラルハザードを防ぐ
* コードの品質を高く保つ

等の重要な狙いがある。


### 車輪の再発明
[車輪の再発明](https://ja.wikipedia.org/wiki/%E8%BB%8A%E8%BC%AA%E3%81%AE%E5%86%8D%E7%99%BA%E6%98%8E)
とは、広く受け入れられ確立されている技術や解決法を（知らずに、または意図的に無視して）
再び一から作ること」を指すための慣用句である。
ソフトウェア開発では、STLのような優れたライブラリを使わずに、
それと同様なライブラリを自分たちで実装するような非効率な様を指すことが多い。



