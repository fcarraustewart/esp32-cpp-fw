.. zephyr:code-sample:: cpp_synchronization
   :name: C++ synchronization

   Use Zephyr synchronization primitives from C++ code.

Overview
********
The sample project illustrates usage of pure virtual class, member
functions with different types of arguments, global objects constructor
invocation.

A simple application demonstrates basic sanity of the kernel.  The main thread
and a cooperative thread take turns printing a greeting message to the console,
and use timers and semaphores to control the rate at which messages are
generated. This demonstrates that kernel scheduling, communication, and
timing are operating correctly.

Building and Running
********************

This kernel project outputs to the console.  It can be built and executed
on QEMU as follows:

.. zephyr-app-commands::
   :zephyr-app: samples/cpp/cpp_synchronization
   :host-os: unix
   :board: qemu_x86
   :goals: run
   :compact:

Sample Output
=============

.. code-block:: console

     Create semaphore 0x001042b0
     Create semaphore 0x001042c4
     main: Hello World!
     coop_thread_entry: Hello World!
     main: Hello World!
     coop_thread_entry: Hello World!
     main: Hello World!
     coop_thread_entry: Hello World!
     main: Hello World!
     coop_thread_entry: Hello World!
     main: Hello World!
     coop_thread_entry: Hello World!
     main: Hello World!
     coop_thread_entry: Hello World!
     main: Hello World!
     coop_thread_entry: Hello World!
     main: Hello World!
     coop_thread_entry: Hello World!
     main: Hello World!
     coop_thread_entry: Hello World!
     main: Hello World!

     <repeats endlessly>

Exit QEMU by pressing :kbd:`CTRL+A` :kbd:`x`.





[00:00:50.097,000] <inf> thread_analyzer:  idle                : STACK: unused 268 usage 244 / 512 (47 %); CPU: 93 %
[00:00:50.097,000] <inf> thread_analyzer:                      : Total CPU cycles used: 745695601
[00:00:50.097,000] <inf> thread_analyzer:  main                : STACK: unused 2380 usage 1716 / 4096 (41 %); CPU: 1 %
[00:00:50.097,000] <inf> thread_analyzer:                      : Total CPU cycles used: 15491248
[00:00:50.097,000] <inf> thread_analyzer:  ISR0                : STACK: unused 0 usage 2048 / 2048 (100 %)
[00:00:50.425,000] <inf> LEDs: Run buzzer_thread
[00:00:50.425,000] <dbg> buzzer_output_drv: buzzer_thread: double_beep
Updated MTU: TX: 23 RX: 23 bytes
[00:00:50.666,000] <inf> BLE: Connected
[00:00:50.666,000] <inf> BLE: Connected 80:65:7C:CE:A5:31 (public)
[00:00:50.666,000] <inf> BLE: Connection parameters update requested
[00:00:50.744,000] <wrn> bt_conn: conn 0x3fcad410 failed to establish. RF noise?
[00:00:51.021,000] <inf> DSP: 		End;
[00:00:51.167,000] <wrn> BLE: MTU exchange failed (err -128)
[00:00:51.384,000] <inf> LEDs: Starting BLE State Machine...
[00:00:51.384,000] <inf> LEDs: State changed: Connected
[00:00:51.384,000] <inf> LEDs: Starting BLE State Machine...
[00:00:51.384,000] <inf> LEDs: State changed: Disconnected. Restarting advertising...
[00:00:51.669,000] <wrn> bt_hci_core: opcode 0x2022 status 0x02 
[00:00:51.669,000] <err> BLE: LE data length update request failed: -5
[00:00:52.042,000] <inf> DSP: 		End;
[00:00:52.171,000] <wrn> bt_hci_core: opcode 0x2032 status 0x02 
[00:00:52.171,000] <err> BLE: Phy update request failed: -5
[00:00:52.171,000] <inf> BLE: Disconnected, reason 62 . Try re-advertising.
Connection object available from previous conn. Disconnect is complete!
[00:00:53.063,000] <inf> DSP: 		End;
[00:00:54.000,000] <inf> LEDs: Work scheduled successfully with delay 500
[00:00:54.000,000] <inf> LEDs: CMD_WORKQUEUE_SONG: Result (res.message())
[00:00:54.084,000] <inf> DSP: 		End;
[00:00:54.501,000] <inf> LEDs: Run buzzer_thread
[00:00:54.501,000] <dbg> buzzer_output_drv: buzzer_thread: double_beep
Updated MTU: TX: 23 RX: 23 bytes
[00:00:54.523,000] <inf> BLE: Connected
[00:00:54.523,000] <inf> BLE: Connected 80:65:7C:CE:A5:31 (public)
[00:00:54.523,000] <inf> BLE: Connection parameters update requested
[00:00:54.600,000] <wrn> bt_conn: conn 0x3fcad410 failed to establish. RF noise?
[00:00:55.024,000] <wrn> BLE: MTU exchange failed (err -128)
[00:00:55.105,000] <inf> DSP: 		End;
[00:00:55.460,000] <inf> LEDs: Starting BLE State Machine...
[00:00:55.460,000] <inf> LEDs: State changed: Connected
[00:00:55.460,000] <inf> LEDs: Starting BLE State Machine...
[00:00:55.460,000] <inf> LEDs: State changed: Disconnected. Restarting advertising...
[00:00:55.526,000] <wrn> bt_hci_core: opcode 0x2022 status 0x02 
[00:00:55.526,000] <err> BLE: LE data length update request failed: -5
[00:00:56.027,000] <wrn> bt_hci_core: opcode 0x2032 status 0x02 
[00:00:56.027,000] <err> BLE: Phy update request failed: -5
[00:00:56.028,000] <inf> BLE: Disconnected, reason 62 . Try re-advertising.
Connection object available from previous conn. Disconnect is complete!
Updated MTU: TX: 23 RX: 23 bytes
[00:00:56.106,000] <inf> BLE: Connected
[00:00:56.106,000] <inf> BLE: Connected 80:65:7C:CE:A5:31 (public)
[00:00:56.106,000] <inf> BLE: Connection parameters update requested
[00:00:56.126,000] <inf> DSP: 		End;
[00:00:56.190,000] <wrn> bt_conn: conn 0x3fcad410 failed to establish. RF noise?
[00:00:56.607,000] <wrn> BLE: MTU exchange failed (err -128)
[00:00:57.108,000] <wrn> bt_hci_core: opcode 0x2022 status 0x02 
[00:00:57.108,000] <err> BLE: LE data length update request failed: -5
[00:00:57.147,000] <inf> DSP: 		End;
[00:00:57.610,000] <wrn> bt_hci_core: opcode 0x2032 status 0x02 
[00:00:57.610,000] <err> BLE: Phy update request failed: -5