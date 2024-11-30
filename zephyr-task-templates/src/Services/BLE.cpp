/*
 * Copyright (c) 2024 fcarraustewart, SAS.
 *
 */
#include "Services/BLE.hpp"

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/services/nus.h>

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

static void received(struct bt_conn *conn, const void *data, uint16_t len, void *ctx)
{
	char message[CONFIG_BT_L2CAP_TX_MTU + 1] = "";

	ARG_UNUSED(conn);
	ARG_UNUSED(ctx);

	memcpy(message, data, MIN(sizeof(message) - 1, len));
	LOG_INF("%s() - Len: %d, Message: %s\n", __func__, len, message);

	switch(message[0]) {
		case 0x33:
			Service::BLE::send(message,1);
			break;
		case 0xAA:
			Service::BLE::send(message,1);
			break;

		default:
		break;
	}
}

struct bt_nus_cb nus_listener = {
	.notif_enabled = notif_enabled,
	.received = received,
};

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
	adv_param.options |= BT_LE_ADV_OPT_ONE_TIME;

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
	if (err) {
		LOG_ERR("Connection failed (err %u)", err);
	} else {
		LOG_INF("Connected");
		if (!ble_conn) {
			ble_conn = bt_conn_ref(connected);
		}
	}
}

static void disconnected(struct bt_conn *disconn, uint8_t reason)
{
	if (ble_conn) {
		bt_conn_unref(ble_conn);
		ble_conn = NULL;
	}

	LOG_INF("Disconnected, reason %u %s. Try re-advertising.", reason, bt_hci_err_to_str(reason));

	int err;
	/* ReStart advertising */
	//bt_le_adv_stop();
	adv_param = *BT_LE_ADV_CONN_FAST_1;
	adv_param.options |= BT_LE_ADV_OPT_ONE_TIME;

	err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
	if (err) {
		LOG_ERR("Advertising failed to start (err %d) %s ", err, bt_att_err_to_str(err));
		return;
	}
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

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

int Service::BLE::init(void)
{
	int err;

	LOG_INF("Sample - Bluetooth Peripheral NUS\n");

	err = bt_nus_cb_register(&nus_listener, NULL);
	if (err) {
		LOG_INF("Failed to register NUS callback: %d\n", err);
		return err;
	}

	err = bt_enable(bt_ready);

	LOG_INF("Initialization complete\n");

	return 0;
}
