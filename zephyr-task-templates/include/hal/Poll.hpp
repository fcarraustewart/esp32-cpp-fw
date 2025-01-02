#pragma once

#include <zephyr/kernel.h>

namespace zpp {

/**
 * Wrapper for Zephyr's poll_signal and poll_event structs.
 * Provides RAII-based management and a simplified interface.
 */
class Poll {
public:
    // constexpr Poll()
    //     : mPollSignal(K_POLL_SIGNAL_INITIALIZER(mPollSignal)),
    //       mPollEvent(K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_SIGNAL, K_POLL_MODE_NOTIFY_ONLY, &mPollSignal) ) {}

    Poll() {
        k_poll_signal_init(&mPollSignal);
        k_poll_event_init(&mPollEvent, K_POLL_TYPE_SIGNAL, K_POLL_MODE_NOTIFY_ONLY, &mPollSignal);
    }

    ~Poll() = default;

    // Reset the signal
    inline void ResetSignal() {
        k_poll_signal_reset(&mPollSignal);
        mPollEvent.state = K_POLL_STATE_NOT_READY;
    }

    // Wait for an event to occur with a timeout
    inline void Wait(k_timeout_t timeout = K_FOREVER) {
        k_poll(&mPollEvent, 1, timeout);
    }

    // Raise a signal to notify waiting threads
    inline void RaiseSignal(int value = 1) {
        k_poll_signal_raise(&mPollSignal, value);
    }

    // Check the current state of the poll event
    inline bool IsReady() const {
        return mPollEvent.state == K_POLL_STATE_SIGNALED;
    }

private:
    struct k_poll_signal mPollSignal;
    mutable struct k_poll_event mPollEvent;
};

} // namespace zpp