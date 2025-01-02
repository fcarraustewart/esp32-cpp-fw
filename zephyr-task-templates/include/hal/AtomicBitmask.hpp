#pragma once

#include <zephyr/kernel.h>

namespace zpp {

/**
 * Wrapper for atomic bitmask operations.
 * Provides an easy-to-use interface for atomic bit manipulation.
 */
class AtomicBitmask {
public:
    constexpr AtomicBitmask() 
    : mBitmask{0/*ATOMIC_DEFINE(mBitmask, _EVT_MAX)*/} 
    {}

    inline void SetBit(int bit) {
        atomic_set_bit(mBitmask, bit);
    }

    inline bool TestAndClearBit(int bit) {
        return atomic_test_and_clear_bit(mBitmask, bit);
    }

private:
    atomic_t mBitmask[1];
};

} // namespace zpp