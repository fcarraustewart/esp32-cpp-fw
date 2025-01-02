/*
 * Copyright (c) 2024 fcarraustewart, SAS.
 *
 */
#include <Services/BLE.hpp>

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/services/nus.h>
#include <zephyr/sys/util.h>

#define LOG_LEVEL 3
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(BLE);

#define DEVICE_NAME		CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN		(sizeof(DEVICE_NAME) - 1)

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static const struct bt_data sd[] = {
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_NUS_SRV_VAL),
};

/* BLE connection */
struct bt_conn *ble_conn;
static struct bt_le_adv_param adv_param;

/* Notification state */
volatile bool notify_enable;

static void notif_enabled(bool enabled, void *ctx)
{
	ARG_UNUSED(ctx);
	notify_enable = (true);
	LOG_INF("%s() - %s\n", __func__, (enabled ? "Enabled" : "Disabled"));
}

#define BT_LE_MY_CONN_PARAM BT_LE_CONN_PARAM(6, 6, 15, 30)

static void request_data_len_update(void)
{
	int err;
	const bt_conn_le_data_len_param params[] = { { .tx_max_len = (0x00fb), .tx_max_time = (0x4290), } };
	err = bt_conn_le_data_len_update(ble_conn, params);
		if (err) {
			LOG_ERR("LE data length update request failed: %d",  err);
		}
}

static void request_phy_update(void)
{
	int err;
	const bt_conn_le_phy_param params[] = { { .options = BT_CONN_LE_PHY_OPT_NONE, .pref_tx_phy = (BT_GAP_LE_PHY_2M), .pref_rx_phy = (BT_GAP_LE_PHY_2M), } };
	err = bt_conn_le_phy_update(ble_conn, params);
		if (err) {
			LOG_ERR("Phy update request failed: %d",  err);
		}
}
static struct bt_le_conn_param *conn_param = BT_LE_MY_CONN_PARAM;
static int update_connection_parameters(void)
{	
	int err;
	err = bt_conn_le_param_update(ble_conn, conn_param);
		if (err) {
			LOG_ERR("Cannot update conneciton parameter (err: %d)", err);
			return err;
		}
	LOG_INF("Connection parameters update requested");
	return 0;
}

//callback
static void conn_params_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{
	LOG_INF("Conn params updated: interval %d unit, latency %d, timeout: %d0 ms",interval, latency, timeout);
}

int Service::BLE::send(const void *arg, uint16_t len)
{
	int err;

    err = bt_nus_send(NULL, arg, len);
    
	if(err == 0)
		LOG_DBG("Data send - Result: %d\n", err);

    if (err < 0 && (err != -EAGAIN) && (err != -ENOTCONN)) {
        return err;
    }

    return 0;
}

static void received(struct bt_conn *conn, const void *data, uint16_t len, void *ctx)
{
	uint8_t message[CONFIG_BT_L2CAP_TX_MTU + 1] = "";

	ARG_UNUSED(conn);
	ARG_UNUSED(ctx);

	memcpy(message, data, MIN(sizeof(message) - 1, len));
	LOG_INF("%s() - Len: %d, Message: %s\n", __func__, len, message);

	switch(message[0]) {
		case 0x33:
			Service::BLE::Send(message);
			break;
		case 0xAA:
			Service::BLE::Send(message);
			break;

		default:
		break;
	}
}

struct bt_nus_cb nus_listener = {
	.notif_enabled = notif_enabled,
	.received = received,
};

static uint16_t payload_length = 0;

static void MTU_exchange_cb(struct bt_conn *conn, uint8_t err, struct bt_gatt_exchange_params *params)
{
	if (!err) {
		LOG_INF("MTU exchange done. "); 
		payload_length=bt_gatt_get_mtu(ble_conn)-3; //3 bytes ATT header
	} else {
		LOG_WRN("MTU exchange failed (err %" PRIu8 ")", err);
	}
}

static void request_mtu_exchange(void)
{	
	int err;
	static struct bt_gatt_exchange_params exchange_params;
	exchange_params.func = MTU_exchange_cb;

	err = bt_gatt_exchange_mtu(ble_conn, &exchange_params);
	if (err) {
		LOG_WRN("MTU exchange failed (err %d)", err);
	} else {
		LOG_INF("MTU exchange pending");
	}
}

void mtu_updated(struct bt_conn *conn, uint16_t tx, uint16_t rx)
{
	printk("Updated MTU: TX: %d RX: %d bytes\n", tx, rx);
}

static struct bt_gatt_cb gatt_callbacks = {
	.att_mtu_updated = mtu_updated,
};

static void bt_ready(int err)
{
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)", err);
		return;
	}
	LOG_INF("Bluetooth initialized");
	/* Start advertising */
	adv_param = *BT_LE_ADV_CONN_FAST_1;

	err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
	if (err) {
		LOG_ERR("Advertising failed to start (err %d)", err);
		return;
	}

	bt_gatt_cb_register(&gatt_callbacks);

	LOG_INF("Configuration mode: waiting connections...");
}
static void connected(struct bt_conn *connected, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	if (err) {
		LOG_ERR("Connection failed (err %u)", err);
	} else {
		LOG_INF("Connected");
		if (!ble_conn) {
			ble_conn = bt_conn_ref(connected);
		}
	}

	bt_addr_le_to_str(bt_conn_get_dst(ble_conn), addr, sizeof(addr));
	LOG_INF("Connected %s", addr);
	//Delays added to avoid collision (in case the central also send request), should be better with a state machine. 
	update_connection_parameters();
	zpp::this_thread::sleep_for(std::chrono::milliseconds(500));
	request_mtu_exchange();
	zpp::this_thread::sleep_for(std::chrono::milliseconds(500));
	request_data_len_update();
	zpp::this_thread::sleep_for(std::chrono::milliseconds(500));
	request_phy_update();

}

static void disconnected(struct bt_conn *disconn, uint8_t reason)
{
	LOG_INF("Disconnected, reason %u %s. Try re-advertising.", reason, bt_hci_err_to_str(reason));

	__ASSERT(ble_conn == disconn, "Unexpected disconnected callback");

	if (ble_conn) {
		bt_conn_unref(ble_conn);
		ble_conn = NULL;
	}
}

static void recycled_cb(void)
{
	printk("Connection object available from previous conn. Disconnect is complete!\n");

	int err;
	/* ReStart advertising */
	//bt_le_adv_stop();
	adv_param = *BT_LE_ADV_CONN_FAST_1;

	err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
	if (err) {
		LOG_ERR("Advertising failed to start (err %d) %s ", err, bt_att_err_to_str(err));
		return;
	}
}

static const char *phy2str(uint8_t phy)
{
	switch (phy) {
	case 0: return "No packets";
	case BT_GAP_LE_PHY_1M: return "LE 1M";
	case BT_GAP_LE_PHY_2M: return "LE 2M";
	case BT_GAP_LE_PHY_CODED: return "LE Coded";
	default: return "Unknown";
	}
}

static void le_phy_updated(struct bt_conn *conn,
			   struct bt_conn_le_phy_info *param)
{
	LOG_INF("LE PHY updated: TX PHY %s, RX PHY %s\n",
	       phy2str(param->tx_phy), phy2str(param->rx_phy));
}

static void le_data_length_updated(struct bt_conn *conn,
				   struct bt_conn_le_data_len_info *info)
{
	LOG_INF("LE data len updated: TX (len: %d time: %d)"
	       " RX (len: %d time: %d)\n", info->tx_max_len,
	       info->tx_max_time, info->rx_max_len, info->rx_max_time);
	

}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
	.recycled = recycled_cb,
	.le_param_updated= conn_params_updated
};

int Service::BLE::InitializeDriver(void)
{
	int err;
	
	err = bt_nus_cb_register(&nus_listener, NULL);
	if (err) {
		LOG_INF("Failed to register NUS callback: %d\n", err);
		return err;
	}

	err = bt_enable(bt_ready);

	return 0;
}

void Service::BLE::Initialize() {
    // #define EVENTS_INTERESTED RTOS::MsgBroker::Event::BLE_Connected , ...
    // System::mMsgBroker::Subscribe<EVENTS_INTERESTED>(); 
    LOG_INF("[Service::%s]::%s().", mName, __FUNCTION__);
	if(!InitializeDriver()) 	
    	LOG_INF("\t\t\t%s: BLE Module Initialized correctly.", __FUNCTION__);
}

void Service::BLE::Handle(const uint8_t arg[]) {
    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
        default:
        {
			Service::BLE::send(arg,18);
            LOG_DBG("[Service::%s]::%s():\t%x.\tNYI.", mName, __func__, arg[0]);   
            LOG_HEXDUMP_DBG(arg, 5, "\t\t\t BLE msg Buffer values.");
            break;
        }
    };
}

/**
 * Build the static members on the RTOS::ActiveObject
 */
namespace Service
{
    using                       _BLE = RTOS::ActiveObject<Service::BLE>;

    template <>
    const uint8_t               _BLE::mName[] =  "BLEss";
    template <>
    uint8_t                     _BLE::mCountLoops = 0;
    template <>
    const uint8_t               _BLE::mInputQueueItemLength = 16;
    template <>
    const uint8_t               _BLE::mInputQueueItemSize = sizeof(uint16_t);
    template <>
    const size_t                _BLE::mInputQueueSizeBytes = 
                                        RTOS::ActiveObject<Service::BLE>::mInputQueueItemLength 
                                        * RTOS::ActiveObject<Service::BLE>::mInputQueueItemSize;
    template <>
    char                        _BLE::mInputQueueAllocation[
                                        RTOS::ActiveObject<Service::BLE>::mInputQueueSizeBytes
                                    ] = { 0 };
    template <>
    RTOS::QueueHandle_t         _BLE::mInputQueue = RTOS::Hal::QueueCreate(
                                        RTOS::ActiveObject<Service::BLE>::mInputQueueItemLength,
                                        RTOS::ActiveObject<Service::BLE>::mInputQueueItemSize,
                                        RTOS::ActiveObject<Service::BLE>::mInputQueueAllocation
                                    );
    template <>
    uint8_t                     _BLE::mReceivedMsg[
                                        RTOS::ActiveObject<Service::BLE>::mInputQueueItemLength
                                    ] = { 0 };

	namespace {
    ZPP_KERNEL_STACK_DEFINE(cBLEThreadStack, 2048);
    template <>
    zpp::thread_data            _BLE::mTaskControlBlock = zpp::thread_data();
    template <>
    zpp::thread                 _BLE::mHandle = zpp::thread(
                                        mTaskControlBlock, 
                                        Service::cBLEThreadStack(), 
                                        RTOS::cThreadAttributes, 
                                        Service::_BLE::Run
                                    );
    } //https://www.reddit.com/r/cpp/comments/4ukhh5/what_is_the_purpose_of_anonymous_namespaces/#:~:text=The%20purpose%20of%20an%20anonymous,will%20not%20have%20internal%20linkage.
                                  
}