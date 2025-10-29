#include <memory>
#include <new>

#include "gtest_wrapper.h"

#include "suppress_warning.h"

namespace {

// @@@ sample begin 0:0

class X {
public:
    explicit X(int v) : str_{std::to_string(v)} {}
    std::string const& get_str() const noexcept { return str_; }

    ~X() {}

private:
    std::string str_;
};
// @@@ sample end

TEST(core_lang_spec, placement_new)
{
    // @@@ sample begin 0:1

    alignas(X) uint8_t memory_[sizeof(X)];
    // alignas(X)はXのアライメント要件(通常4や8バイト境界)に合わせる
    // これがないと、memory_が不適切な境界(例:奇数アドレス)に配置され、
    // 一部のCPUアーキテクチャでクラッシュやパフォーマンス低下を引き起こす

    X* x = new (memory_) X{42};  // X型オブジェクトをmemory_上に生成

    ASSERT_EQ("42", x->get_str());  // 正しく生成されたかどうかの確認

    // ...
    // ... ここでエクセプションが発生して、
    // ... 以下のコードが実行できないとデストラクタが呼ばれないためメモリリークする可能性がある

    x->~X();  // プレースメントしたオブジェクトの解放にはdeleteを使ってはならないため、
              // このように直接デストラクタを呼び出す必要がある
    // @@@ sample end
}

TEST(core_lang_spec, placement_new2)
{
    // @@@ sample begin 0:2

    alignas(X) uint8_t memory_[sizeof(X)];

    auto deleter = [](X* p) {  // カスタムデリータの定義
        if (p) {
            p->~X();  // デストラクタのみ呼び出し
        }
    };

    std::unique_ptr<X, decltype(deleter)> x{new (memory_) X{42}, deleter};

    ASSERT_EQ("42", x->get_str());
    // xがスコープアウトするタイミングでカスタムデリータdeleterが呼ばれるため、~X()の呼び出し漏れが回避できる
    //  @@@ sample end
}
}  // namespace
