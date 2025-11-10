#include <atomic>
#include <thread>

#include "gtest_wrapper.h"

#include "suppress_warning.h"

namespace conflict {
// @@@ sample begin 0:0

struct Conflict {
    void     increment() { ++count_; }  // 非アトミック（データレースの原因）
    uint32_t count_ = 0;
};
// @@@ sample end

TEST(ExpTerm, thread)
{
    // @@@ sample begin 0:1

    Conflict c;

    constexpr uint32_t inc_per_thread = 5'000'000;
    constexpr uint32_t expected       = 2 * inc_per_thread;

    auto worker = [&c] {  // スレッドのボディとなるラムダの定義
        for (uint32_t i = 0; i < inc_per_thread; ++i) {
            c.increment();
        }
    };

    std::thread t1{worker};  // ラムダworker関数を使用したスレッドの起動
    std::thread t2{worker};

    t1.join();  // スレッドの終了待ち
    t2.join();  // スレッドの終了待ち
                // 注意: join()もdetach()も呼ばずにスレッドオブジェクトが
                // デストラクトされると、std::terminateが呼ばれる

    // ASSERT_EQ(c.count_, expected);  t1とt2が++count_が競合するためこのテストは成立しないため、
    //                                 一例では次のようになる  c.count_: 6825610 expected: 10000000
    ASSERT_NE(c.count_, expected);
    // @@@ sample end
}
}  // namespace conflict

namespace no_conflict {
// @@@ sample begin 1:0

struct Conflict {
    void increment()
    {
        mtx_.lock();  // クリティカルセクションの保護開始

        ++count_;

        mtx_.unlock();  // クリティカルセクションの保護終了
    }
    uint32_t   count_ = 0;
    std::mutex mtx_{};
};
// @@@ sample end

TEST(ExpTerm, thread)
{
    // @@@ sample begin 1:1

    Conflict c;

    constexpr uint32_t inc_per_thread = 5'000'000;
    constexpr uint32_t expected       = 2 * inc_per_thread;

    auto worker = [&c] {  // スレッドのボディとなるラムダの定義
        for (uint32_t i = 0; i < inc_per_thread; ++i) {
            c.increment();
        }
    };

    std::thread t1{worker};  // ラムダworker関数を使用したスレッドの起動
    std::thread t2{worker};

    t1.join();  // スレッドの終了待ち
    t2.join();  // スレッドの終了待ち
                // 注意: join()もdetach()も呼ばずにスレッドオブジェクトが
                // デストラクトされると、std::terminateが呼ばれる

    ASSERT_EQ(c.count_, expected);
    // @@@ sample end
}
}  // namespace no_conflict
namespace atomic {
// @@@ sample begin 2:0

struct Conflict {
    void increment()
    {
        ++count_;  // ++count_は「count_の値の呼び出し -> その値のインクリメント、その値のcount_への書き戻し」である
                   // この一連の操作は排他的(アトミック)に行われる

    }  // lockオブジェクトのデストラクタでmtx_.unlock()が呼ばれる
    std::atomic<uint32_t> count_ = 0;
};
// @@@ sample end

TEST(ExpTerm, thread)
{
    // @@@ sample begin 2:1

    Conflict c;

    constexpr uint32_t inc_per_thread = 5'000'000;
    constexpr uint32_t expected       = 2 * inc_per_thread;

    auto worker = [&c] {  // スレッドのボディとなるラムダの定義
        for (uint32_t i = 0; i < inc_per_thread; ++i) {
            c.increment();
        }
    };

    std::thread t1{worker};  // ラムダworker関数を使用したスレッドの起動
    std::thread t2{worker};

    t1.join();  // スレッドの終了待ち
    t2.join();  // スレッドの終了待ち
                // 注意: join()もdetach()も呼ばずにスレッドオブジェクトが
                // デストラクトされると、std::terminateが呼ばれる

    ASSERT_EQ(c.count_, expected);
    // @@@ sample end
}
}  // namespace atomic

namespace {

// @@@ sample begin 3:0

std::mutex              mutex;
std::condition_variable cond_var;
bool                    event_occured = false;

void notify()  // 通知を行うスレッドが呼び出す関数
{
    auto lock = std::lock_guard{mutex};

    event_occured = true;

    cond_var.notify_all();  // wait()で待ち状態のすべてのスレッドを起こす
}

void wait()
{
    auto lock = std::unique_lock{mutex};

    // notifyされるのを待つ。
    cond_var.wait(lock, []() noexcept { return event_occured; });  // Spurious Wakeup対策
}
// @@@ sample end

TEST(ExpTerm, event)
{
    // clang-format off
    // @@@ sample begin 3:1

    std::thread t1{[]() { wait(); /* 通知待ち */ }};
    std::thread t2{[]() { wait(); /* 通知待ち */ }};

    notify();  // 通知待ちのスレッドに通知

    t1.join();
    t2.join();
    // @@@ sample end
    // clang-format on
}
}  // namespace
