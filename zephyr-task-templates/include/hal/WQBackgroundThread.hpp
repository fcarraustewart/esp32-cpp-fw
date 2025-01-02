#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zpp/result.hpp> // Assuming the zpp::result header is included from the zpp library

namespace zpp {

/**
 * Wrapper class for Zephyr's work queue functionality.
 * Provides RAII management and simplified methods for initialization and utilization.
 */
class WQBackgroundThread {
public:
    WQBackgroundThread() = default;

    ~WQBackgroundThread() = default;

    /**
     * Initializes the work queue.
     * @param stack Pointer to the stack data for the work queue.
     * @param stackSize Size of the stack.
     * @param priority Priority of the work queue thread.
     * @return Result indicating success or failure.
     */
    zpp::result<int,zpp::error_code> Initialize(void *stack, size_t stackSize, int priority) {
        k_work_queue_init(&mWorkQueue);

        k_work_queue_start(&mWorkQueue, static_cast<k_thread_stack_t *>(stack),
                           stackSize, priority, nullptr);
        return zpp::error_result(zpp::error_code::k_io);
    }

    /**
     * Schedules a work item with a delay using the provided work function.
     * @param workFn Function pointer to the work function.
     * @param delay Timeout duration for scheduling the work.
     * @return Result indicating success or failure.
     */
    zpp::error_result<zpp::error_code> ScheduleWork(void (*workFn)(struct k_work *), k_timeout_t delay) {
        if (!workFn) {
            LOG_ERR("Work function is null.");
            return zpp::error_result(zpp::error_code::k_inval);
        }

        k_work_init_delayable(&mWork, workFn);

        int res = k_work_reschedule_for_queue(&mWorkQueue, &mWork, delay);
        if (res < 0) {
            LOG_ERR("Failed to reschedule work: %d", res);
            return zpp::error_result(zpp::error_code::k_again);
        }

        LOG_INF("Work scheduled successfully with delay %d", delay.ticks);
        return zpp::error_result(zpp::error_code::k_io);
    }

private:
    struct k_work_q mWorkQueue;
    struct k_work_delayable mWork;
};

} // namespace zpp