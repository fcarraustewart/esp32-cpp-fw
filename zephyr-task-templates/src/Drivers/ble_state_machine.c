#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/services/nus.h>
#include <Drivers/ble_state_machine.h>

#define LOG_LEVEL 3
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(Drivers_BLE_state_machine);

static struct bt_conn *default_conn;

enum bt_sample_adv_evt {
	BT_SAMPLE_EVT_CONNECTED,
	BT_SAMPLE_EVT_DISCONNECTED,
	BT_SAMPLE_EVT_MAX,
};

enum bt_sample_adv_st {
	BT_SAMPLE_ST_ADV,
	BT_SAMPLE_ST_CONNECTED,
};

static ATOMIC_DEFINE(evt_bitmask, BT_SAMPLE_EVT_MAX);

static volatile enum bt_sample_adv_st app_st = BT_SAMPLE_ST_ADV;

static struct k_poll_signal poll_sig = K_POLL_SIGNAL_INITIALIZER(poll_sig);
static struct k_poll_event poll_evt = K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_SIGNAL,
						K_POLL_MODE_NOTIFY_ONLY, &poll_sig);

static void raise_evt(enum bt_sample_adv_evt evt)
{
	(void)atomic_set_bit(evt_bitmask, evt);
	k_poll_signal_raise(poll_evt.signal, 1);
}

static void connected_cb(struct bt_conn *conn, uint8_t err)
{
	LOG_INF("Connected (err 0x%02X)\n", err);

	if (err) {
		return;
	}

	__ASSERT(!default_conn, "Attempting to override existing connection object!");
	default_conn = bt_conn_ref(conn);

	raise_evt(BT_SAMPLE_EVT_CONNECTED);
}

static void disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("Disconnected, reason 0x%02X %s\n", reason, bt_hci_err_to_str(reason));

	__ASSERT(conn == default_conn, "Unexpected disconnected callback");

	bt_conn_unref(default_conn);
	default_conn = NULL;
}

static void recycled_cb(void)
{
	LOG_INF("Connection object available from previous conn. Disconnect is complete!\n");
	raise_evt(BT_SAMPLE_EVT_DISCONNECTED);
}

// BT_CONN_CB_DEFINE(conn_cb) = {
// 	.connected = connected_cb,
// 	.disconnected = disconnected_cb,
// 	.recycled = recycled_cb,
// };

static int start_advertising(struct bt_le_ext_adv *adv)
{
	int err;

	LOG_INF("Starting Extended Advertising\n");
	err = bt_le_ext_adv_start(adv, BT_LE_EXT_ADV_START_DEFAULT);
	if (err) {
		LOG_INF("Failed to start extended advertising (err %d)\n", err);
	}

	return err;
}

int ble_state_machine_run(void)
{
	int err;
	struct bt_le_ext_adv *adv;

	LOG_INF("Starting Extended Advertising Demo\n");

	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(NULL);
	if (err) {
		LOG_INF("Bluetooth init failed (err %d)\n", err);
		return err;
	}

	/* Create a connectable advertising set */
	err = bt_le_ext_adv_create(BT_LE_EXT_ADV_CONN, NULL, &adv);
	if (err) {
		LOG_INF("Failed to create advertising set (err %d)\n", err);
		return err;
	}

	/* Set advertising data to have complete local name set */
	err = bt_le_ext_adv_set_data(adv, adXX, ARRAY_SIZE(adXX), NULL, 0);
	if (err) {
		LOG_INF("Failed to set advertising data (err %d)\n", err);
		return 0;
	}

	err = start_advertising(adv);
	if (err) {
		return err;
	}

	while (true) {
		k_poll(&poll_evt, 1, K_FOREVER);

		k_poll_signal_reset(poll_evt.signal);
		poll_evt.state = K_POLL_STATE_NOT_READY;

		/* Identify event and act upon if applicable */
		if (atomic_test_and_clear_bit(evt_bitmask, BT_SAMPLE_EVT_CONNECTED) &&
		    app_st == BT_SAMPLE_ST_ADV) {

			LOG_INF("Connected state!\n");
			app_st = BT_SAMPLE_ST_CONNECTED;

			LOG_INF("Initiating disconnect within 5 seconds...\n");
			if (k_poll(&poll_evt, 1, K_SECONDS(5)) == 0) {
				LOG_INF("Remote disconnected early...\n");
				/* Don't clear event here as we want the loop to run immediately */
			} else {
				/* Connection still alive after 5 seconds, terminate it */
				bt_conn_disconnect(default_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
			}
		} else if (atomic_test_and_clear_bit(evt_bitmask, BT_SAMPLE_EVT_DISCONNECTED) &&
			   app_st == BT_SAMPLE_ST_CONNECTED) {

			LOG_INF("Disconnected state! Restarting advertising\n");
			app_st = BT_SAMPLE_ST_ADV;
			err = start_advertising(adv);
			if (err) {
				return err;
			}
		}
	}

	return err;
}
