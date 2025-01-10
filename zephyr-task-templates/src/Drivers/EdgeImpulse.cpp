#include <Drivers/EdgeImpulse.hpp>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(DSP, LOG_LEVEL_INF);

// Zephyr 3.1.x and newer uses different include scheme
#include <version.h>
#if (KERNEL_VERSION_MAJOR > 3) || ((KERNEL_VERSION_MAJOR == 3) && (KERNEL_VERSION_MINOR >= 1))
#include <zephyr/kernel.h>
#else
#include <zephyr.h>
#endif
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/dsp/numpy.hpp"
#ifdef EI_NORDIC
#include <nrfx_clock.h>
#endif

#include <Utils/zpp.hpp>

static const float features[] = {
    // copy raw features here (for example from the 'Live classification' page)
    // see https://docs.edgeimpulse.com/docs/running-your-impulse-locally-zephyr
    32.4032, -3.8433, 134.1506, 32.9463, -3.7231, 135.8095, 32.9851, -3.7262, 135.9390, 33.1984, -3.6954, 136.5583, 33.5901, -3.6578, 137.6860, 34.0751, -3.5882, 138.8312, 34.2524, -3.5687, 139.2410, 34.3683, -3.5518, 139.5550, 34.7967, -3.5457, 140.7792, 34.9917, -3.5999, 141.4902, 35.2316, -3.7090, 142.6156, 35.2756, -3.7504, 142.9503, 35.3250, -3.7763, 143.2179, 35.4744, -3.8355, 144.5165, 35.4941, -3.8245, 144.8908, 35.5215, -3.8331, 145.2129, 35.5238, -3.8471, 146.7930, 35.4789, -3.8439, 147.4177, 35.4535, -3.8655, 147.7101, 35.1120, -3.8496, 150.1511, 34.9784, -3.8332, 150.8242, 34.8835, -3.7943, 151.5884, 34.5426, -3.6609, 153.8665, 34.4270, -3.4734, 155.9445, 34.3759, -3.3817, 157.0251, 34.4146, -3.3557, 157.2649, 34.5895, -3.1801, 159.3526, 34.8667, -3.0513, 160.2847, 35.1665, -2.9508, 161.9920, 35.3181, -2.9395, 162.4005, 35.3153, -2.9382, 162.9849, 35.4213, -2.9381, 163.7998, 35.3426, -2.9422, 164.1228, 35.2518, -2.9490, 165.3674, 35.2183, -2.9547, 165.2820, 34.6859, -2.9925, 167.6151, 34.4530, -3.0114, 168.2651, 34.3274, -3.0084, 168.8638, 33.9769, -3.0104, 170.1485, 33.9879, -3.0090, 170.1751, 33.5467, -3.0107, 172.1639, 33.4473, -3.0090, 173.3139, 33.3879, -3.0116, 172.3348, 33.2047, -3.0032, 176.0434, 33.1780, -2.9613, 172.5614, 33.1703, -2.9701, 179.3334, 33.2192, -2.8056, 170.3155, 33.2664, -2.6931, 186.8779, 33.2794, -2.6460, 162.5055, 33.3552, -2.3502, 205.6013, 33.4775, -2.0501, 108.3029, 33.5982, -1.8227, -218.8184, 33.6063, -1.8169, -160.4054, 33.8668, -1.5111, -187.8914, 34.0028, -1.4267, -169.8480, 34.0178, -1.4024, -180.5170, 34.1263, -1.3943, -172.6805, 34.1322, -1.4602, -176.2180, 34.1288, -1.4747, -173.8279, 34.0786, -1.5899, -174.5644, 34.0828, -1.5843, -174.0622, 33.9681, -1.7278, -173.7518, 33.8614, -1.8132, -173.7216, 33.8581, -1.8142, -173.6975, 33.6338, -1.9886, -173.8136, 33.4649, -2.1398, -173.9767, 33.4554, -2.1611, -174.0054, 33.2668, -2.4206, -174.2880, 33.1478, -2.7398, -174.6707, 33.0782, -3.0253, -175.0285, 33.0763, -3.0423, -175.0559, 33.0080, -3.3342, -175.4491, 33.0101, -3.3302, -175.4461, 32.9380, -3.5867, -175.7793, 32.9444, -3.5708, -175.7612, 32.8536, -3.7698, -176.0106, 32.7650, -3.8490, -176.1278, 32.7623, -3.8563, -176.1409, 32.6741, -3.9124, -176.2087, 32.6769, -3.9132, -176.2160, 32.6050, -3.9427, -176.2781, 32.6006, -3.9638, -176.4096, 32.6600, -4.0126, -176.6259, 32.6604, -4.0132, -176.6333, 32.6914, -4.0930, -176.9456, 32.6919, -4.0928, -176.9489, 32.6613, -4.1523, -177.2347, 32.6611, -4.1531, -177.2316, 32.5718, -4.1423, -177.5025, 32.4713, -4.1050, -177.5117, 32.3991, -4.0229, -178.1743, 32.3974, -4.0234, -177.2999, 32.3776, -3.9546, -179.2393, 32.3755, -3.9505, -176.6589, 32.3752, -3.9526, -181.9514, 32.3828, -3.9690, -172.5341, 32.4289, -3.9914, -195.7828, 32.4563, -4.0425, 158.7571, 32.3860, -4.1488, 188.1336, 32.3821, -4.1498, 173.1871, 32.2876, -4.2323, 181.6843, 32.1791, -4.2490, 173.9167, 32.1856, -4.2363, 178.2775, 32.1562, -4.1363, 174.3334, 32.3084, -3.7998, 174.7746, 32.3103, -3.8163, 173.7246, 32.5000, -3.4094, 173.1630, 32.6964, -2.8642, 171.7975, 32.8323, -2.1399, 170.6841, 32.8482, -2.1331, 170.6396, 32.8746, -1.5852, 169.8353, 32.8906, -1.4848, 169.6098, 32.8435, -1.1876, 169.0472, 32.7667, -0.7852, 167.4895, 32.7425, -0.8123, 167.6862, 32.7693, -0.6716, 166.1284, 32.7452, -0.6624, 166.1054, 32.8574, -0.6112, 164.4932, 32.8624, -0.5968, 164.2045, 32.9988, -0.5807, 162.8658, 33.1748, -0.5330, 160.9461, 33.2714, -0.4683, 159.4552, 33.2956, -0.4613, 159.2556, 33.2749, -0.4276, 158.5059, 33.2806, -0.4204, 158.1575, 33.2446, -0.3954, 157.8445, 33.1687, -0.3956, 156.9057, 33.1514, -0.3952, 156.9693, 33.1381, -0.4432, 156.4319, 33.1288, -0.5443, 155.7369, 33.2885, -0.7148, 154.6326, 33.3220, -0.7481, 154.4071, 33.4664, -0.8823, 153.7153, 33.9115, -1.3046, 151.7047, 33.9973, -1.4002, 151.3757, 34.1962, -1.6085, 150.4373, 34.7847, -2.3679, 147.7713, 34.7738, -2.3878, 147.7244, 35.1724, -2.8623, 146.1566, 35.3536, -3.0901, 145.4119, 35.5699, -3.3338, 144.5907, 36.3035, -4.1196, 141.7222, 36.4007, -4.1873, 141.3159, 36.6457, -4.4446, 140.2776, 37.3071, -4.9813, 137.1779, 37.3489, -5.0054, 136.9686, 37.6418, -5.2438, 135.4378, 37.8263, -5.4095, 134.3958, 37.9520, -5.5055, 133.8466, 38.4840, -6.1004, 130.9794, 38.5063, -6.1301, 130.9182, 38.6830, -6.4324, 129.6668, 38.7944, -6.6340, 128.8858
};

int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

int EImpulse::Run() {
    // This is needed so that output of printf is output immediately without buffering
    setvbuf(stdout, NULL, _IONBF, 0);

#ifdef CONFIG_SOC_NRF5340_CPUAPP // this comes from Zephyr
    // Switch CPU core clock to 128 MHz
    nrfx_clock_divider_set(NRF_CLOCK_DOMAIN_HFCLK, NRF_CLOCK_HFCLK_DIV_1);
#endif

	zpp::this_thread::sleep_for(std::chrono::milliseconds(1000));
    LOG_DBG("Edge Impulse standalone inferencing (Zephyr)\n");

    if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        LOG_DBG("The size of your 'features' array is not correct. Expected %d items, but had %u\n",
            EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
        return 1;
    }

    ei_impulse_result_t result = { 0 };

    while (1) {
        // the features are stored into flash, and we don't want to load everything into RAM
        signal_t features_signal;
        features_signal.total_length = sizeof(features) / sizeof(features[0]);
        features_signal.get_data = &raw_feature_get_data;

        // invoke the impulse
        EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false);
        LOG_DBG("run_classifier returned: %d\n", res);

        if (res != 0) return 1;

        LOG_DBG("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                result.timing.dsp, result.timing.classification, result.timing.anomaly);
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
        bool bb_found = result.bounding_boxes[0].value > 0;
        for (size_t ix = 0; ix < result.bounding_boxes_count; ix++) {
            auto bb = result.bounding_boxes[ix];
            if (bb.value == 0) {
                continue;
            }
            LOG_DBG("    %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\n", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
        }
        if (!bb_found) {
            LOG_DBG("    No objects found\n");
        }
#else
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            LOG_DBG("    %s: %.5f\n", result.classification[ix].label,
                                    result.classification[ix].value);
            if(result.classification[ix].value > 0.90f)
                LOG_DBG("    \t\t %s result.classification[%d].value (%d) => 0.90 ",result.classification[ix].label, ix, 100*(int)result.classification[ix].value);
        }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        LOG_DBG("    anomaly score: %.3f\n", result.anomaly);
#endif
#endif
    	zpp::this_thread::sleep_for(std::chrono::milliseconds(1000));
		LOG_INF("\t\tEnd;");
    }
}
