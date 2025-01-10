#pragma once

#include <hal/Poll.hpp>
#include <hal/AtomicBitmask.hpp>
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/services/nus.h>
#include <Drivers/ble_state_machine.h>

#include <zephyr/logging/log.h>

class BleStateMachine {
public:
    enum class State {
        Advertising,
        Connected,
    };

    enum class Event {
        Connected,
        Disconnected,
    };

    BleStateMachine()
        : mCurrentState(State::Advertising), mPoll() {}

    void RaiseEvent(Event event) {
        mEventBitmask.SetBit(static_cast<int>(event));
        mPoll.RaiseSignal();
    }

    int Run() {
        int err;

        LOG_DBG("Starting BLE State Machine...");

        // err = bt_enable(nullptr);
        // if (err) {
        //     LOG_ERR("Bluetooth initialization failed (%d)", err);
        //     return err;
        // }

        err = SetupAdvertising();
        if (err) {
            return err;
        }

        // Dont Block, make it OneShot while (true) 
        {
            mPoll.Wait(K_FOREVER);
            mPoll.ResetSignal();

            if (HandleEvent(Event::Connected, State::Advertising, &BleStateMachine::OnConnected) ||
                HandleEvent(Event::Disconnected, State::Connected, &BleStateMachine::OnDisconnected)) {
                // Changes State inside the Callback OnConnected or OnDisconnected;
            }
        }

        return 0;
    }

private:
    int SetupAdvertising() {
        return 0; // NYI
    }

    template <typename Callback>
    bool HandleEvent(Event event, State expectedState, Callback callback) {
        if (mEventBitmask.TestAndClearBit(static_cast<int>(event)) &&
            mCurrentState == expectedState) {
            (this->*callback)();
            return true;
        }
        return false;
    }

    void OnConnected() {
        LOG_DBG("State changed: Connected");
        mCurrentState = State::Connected;
    }

    void OnDisconnected() {
        LOG_DBG("State changed: Disconnected. Restarting advertising...");
        mCurrentState = State::Advertising;

        if (SetupAdvertising()) {
            LOG_ERR("Failed to restart advertising");
        }
    }

private:
    static constexpr struct bt_data mAd = {.type = adXX->type,.data_len = adXX->data_len,.data = adXX->data};
    State mCurrentState;
    zpp::AtomicBitmask mEventBitmask;
    zpp::Poll mPoll;

};