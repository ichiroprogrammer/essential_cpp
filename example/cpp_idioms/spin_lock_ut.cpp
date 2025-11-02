#include <atomic>
#include <thread>

#include "gtest_wrapper.h"

#include "suppress_warning.h"
#include "spin_lock.h"

namespace {

// @@@ sample begin 0:0

struct Conflict {
    void     increment()
    { 
        std::lock_guard<SpinLock> lock{spin_lock_}; // スピンロックのロックガードオブジェクト生成
        ++count_;
    }

    SpinLock spin_lock_{};
    uint32_t count_ = 0;
};
// @@@ sample end

TEST(Idiom, spin_lock)
{
    // @@@ sample begin 0:1

    Conflict c{};

    constexpr uint32_t inc_per_thread = 5'000'000;
    constexpr uint32_t expected       = 2 * inc_per_thread;
    auto thread_body = [&c] {  
        for (uint32_t i = 0; i < inc_per_thread; ++i) {
            c.increment();
        }
    };

    std::thread t1{thread_body};
    std::thread t2{thread_body};

    t1.join();  // スレッドの終了待ち
    t2.join();  // スレッドの終了待ち
                // 注意: join()もdetach()も呼ばずにスレッドオブジェクトが
                // デストラクトされると、std::terminateが呼ばれる

    ASSERT_EQ(c.count_, expected);
    // @@@ sample end
}
}

