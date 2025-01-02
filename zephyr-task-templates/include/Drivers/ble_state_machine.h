#ifndef __BLE_STATE_MACHINE__H__
#define __BLE_STATE_MACHINE__H__

#ifdef __cplusplus
extern "C" {
#endif

static const struct bt_data adXX[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};


int ble_state_machine_run(void);


#ifdef __cplusplus
}
#endif

#endif
