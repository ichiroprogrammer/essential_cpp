#include <memory>
#include <vector>

#include "gtest_wrapper.h"

namespace SharedProblem0 {
// @@@ sample begin 0:0

class A {
public:
    void register_self(std::vector<std::shared_ptr<A>>& vec) { vec.push_back(std::shared_ptr<A>{this}); }
};
// @@@ sample end

TEST(StdLib, DISABLED_shared_problem)
{
    // @@@ sample begin 0:1

    auto sp1 = std::make_shared<A>();  // Aのポインタを管理するshared_ptr(sp1)が作られる
                                       // sp1が管理するポインタを便宜上、sp1_pointerと呼ぶことにする

    std::vector<std::shared_ptr<A>> vec;

    sp1->register_self(vec);  // vecに登録されるのはsp1_pointerを管理するshared_ptrであるが、
                              // vecに保持された「sp1_pointerを管理するshared_ptr」は、
                              // sp1と個別に生成されたため、sp1とuseカウンタを共有しない

    // ここまで来ると、
    // * sp1がスコープアウトするため、sp1がsp1_pointerを解放する。
    // * vecがスコープアウトするため、vecが保持するshared_ptrが、sp1_pointerを解放する。

    // 以上によりsp1_pointer二重解放されるため、未定義動作につながる
    // @@@ sample end
}
}  // namespace SharedProblem0

namespace SharedProblem1 {
// @@@ sample begin 1:0

class A : public std::enable_shared_from_this<A> {
public:
    void register_self(std::vector<std::shared_ptr<A>>& vec) { vec.push_back(shared_from_this()); }
};
// @@@ sample end

TEST(StdLib, DISABLED_shared_problem)
{
    // @@@ sample begin 1:1

    auto sp1 = std::make_shared<A>();  // Aのポインタを管理するstd::shread_ptr(sp1)が作られる
                                       // sp1が管理するポインタを便宜上、sp1_pointerと呼ぶことにする

    std::vector<std::shared_ptr<A>> vec;

    sp1->register_self(vec);  // shared_from_this()により、
                              // sp1と同じuseカウンタを共有する新しいshared_ptrが生成されvecに格納される。

    // スコープアウト時には参照カウントが正しく管理されているため、
    // 最後のshared_ptrが破棄されるまでオブジェクトは解放されない
    // @@@ sample end
}

}  // namespace SharedProblem1
