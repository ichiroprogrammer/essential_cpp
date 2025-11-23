#include <stdexcept>

#include "gtest_wrapper.h"

#include "suppress_warning.h"

namespace ExceptionExample {

struct A {
    A(int) {}
};

void func(int num)
{
    // @@@ sample begin 0:0

    try {  // tryブロック

        auto a = A(num);  // エクセプションが発生する可能性のあるコード
        // 何らかの処理
        // ...
        IGNORE_UNUSED_VAR(a);  // @@@ delete
    }
    catch (std::logic_error const& e) {
        // エクセプション処理
        // ...
        IGNORE_UNUSED_VAR(e);  // @@@ delete
    }
    catch (std::runtime_error const& e) {
        /*
        throw e;  ここでリカバリ処理ができない場合、再throwすることで外部にエラーを伝えることができるが、
                  以下の記法の方が好ましい */
        throw;  // ここではエクセプション処理が完了できない場合、再throw
    }
    // @@@ sample end
}
SUPPRESS_WARN_BEGIN;
SUPPRESS_WARN_UNUSED_FUNC;

// @@@ sample begin 1:0

void function_try_block(int num)
try {  // 関数tryブロック

    auto a = A(num);  // エクセプションが発生する可能性のあるコード
    // 何らかの処理
    // ...
    IGNORE_UNUSED_VAR(a);  // @@@ delete
}
catch (std::length_error const& e) {  // 関数tryブロックのエクセプションハンドラ
    // エクセプション処理
    // ...
    IGNORE_UNUSED_VAR(e);  // @@@ delete
}
catch (std::logic_error const&) {  // 関数tryブロックのエクセプションハンドラ
    throw;                         // ここではエクセプション処理が完了できない場合、再throw
}
// @@@ sample end
SUPPRESS_WARN_END;

// @@@ sample begin 2:0

int divide(int a, int b)
{
    if (b == 0) {
        throw std::invalid_argument("b must be not ZERO");  // 0除算によるエクセプション
    }
    return a / b;
}
// @@@ sample end

TEST(CoreLangSpec, exception_sample)
{
    int a = 1;
    int b = 0;

    // @@@ sample begin 2:1

    try {  // tryブロック

        auto solution = divide(a, b);
        // 何らかの処理
        // ...
        IGNORE_UNUSED_VAR(solution);  // @@@ delete
    }
    catch (std::exception const& e) {
        // エクセプション処理
        // ...
        IGNORE_UNUSED_VAR(e);  // @@@ delete
    }
    // @@@ sample end
    // @@@ sample begin 2:2

    try {  // tryブロック

        auto solution = divide(a, b);
        // 何らかの処理
        // ...
        IGNORE_UNUSED_VAR(solution);  // @@@ delete
    }
    catch (std::exception const& e) {
        // エクセプション処理
        // ...
        IGNORE_UNUSED_VAR(e);  // @@@ delete
    }
    catch (...) {  // すべてのエクセプションをcatch（catch-all）
        // エクセプション処理
        // ...
    }
    // @@@ sample end
}

// @@@ sample begin 3:0

std::string f_noexcept() noexcept  // エクセプションを発生させない
{
    return "No exceptions here!";
}

std::string f_except() noexcept(false)  // エクセプションを発生させる
{
    throw std::runtime_error{"always throw"};

    return "No exceptions here!";
}

// noexcept or noexcept(false)と宣言しない限りnoexceptでない
std::string f_except2()  // エクセプションを発生させる
{
    throw std::runtime_error{"always throw"};

    return "No exceptions here!";
}
// @@@ sample end

TEST(TermExp, use_noexcept)
{
    // clang-format off
    // @@@ sample begin 3:1

    static_assert(noexcept(f_noexcept()));  // エクセプションを発生させる可能性の確認
    static_assert(!noexcept(f_except()));   // エクセプションを発生させない可能性の確認
    static_assert(!noexcept(f_except2()));  // エクセプションを発生させない可能性の確認

    ASSERT_EQ(f_noexcept(), "No exceptions here!");  // 動作確認
    ASSERT_THROW(f_except(), std::runtime_error);    // エクセプションの発生確認
    ASSERT_THROW(f_except2(), std::runtime_error);   // エクセプションの発生確認
    // @@@ sample end
    // clang-format on
}

// @@@ sample begin 3:2

class PossiblyThrow {  // オブジェクト生成でエクセプションの発生可能性あり
public:
    PossiblyThrow() {}
};

// テンプレート型Tがnoexceptで生成可能なら、関数もnoexceptにする
template <typename T>
void t_f(T const&) noexcept(std::is_nothrow_constructible_v<T>)
{
    // Tを生成して、何らかの処理を行う
}
// @@@ sample end

TEST(TemplateFunctionTest, use_noexcept2)
{
    // @@@ sample begin 3:3

    auto i = int{};
    auto p = PossiblyThrow{};

    static_assert(!std::is_nothrow_constructible_v<PossiblyThrow>);
    static_assert(std::is_nothrow_constructible_v<decltype(i)>);
    static_assert(noexcept(t_f(i)));
    static_assert(!noexcept(t_f(p)));
    // @@@ sample end
}
}  // namespace ExceptionExample
