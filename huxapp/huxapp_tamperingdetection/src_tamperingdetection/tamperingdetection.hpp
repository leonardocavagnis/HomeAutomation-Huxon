#ifndef __TAMPERINGDETECTION_HPP__
#define __TAMPERINGDETECTION_HPP__

/* --------- include libraries -------- */ 
#include <sensor_language/entities.hpp>
#include <sensor_language/lang.hpp>
#include <sensor_language/sensors/STMicroelectronics.hpp>

#include <math.h>

/* --------- defines -------- */ 
#define ACC_AVG_THR         (2.0f)
#define BUF_SIZE            60

/* --------- typedef --------- */
typedef struct {
    float   xyz_abs;
    bool    tamper;
    hux::uint64_t timestamp;
} acc_timestamp_t;

/* --------- declare simulation data -------- */
HUX_DECLARE_SIMULATION_DATA(sim_dataset_1, hux::make_tuple(
    std::vector<float>{0.20, 0.12, 0.13, 0.14, 0.15, 0.16},
    std::vector<float>{0.73, 0.74, 0.75, 0.76, 0.77, 0.78},
    std::vector<float>{1.0, 0.9, 0.8, 0.7, 0.6, 0.5},
    std::vector<float>{0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
    std::vector<float>{0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
    std::vector<float>{0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
    std::vector<hux::uint64_t>{1646435958000, 1646435958050, 1646435958100, 1646435958150, 1646435958200, 1646435958250}));

HUX_DECLARE_SIMULATION_DATA(sim_dataset_2, hux::make_tuple(
    std::vector<float>{0.20, 0.22, 0.23, 0.24, 0.25, 0.26},
    std::vector<float>{0.93, 0.54, 0.55, 0.56, 0.57, 0.58},
    std::vector<float>{-1.0, -0.9, -0.8, -0.7, -0.6, -0.5},
    std::vector<float>{0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
    std::vector<float>{0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
    std::vector<float>{0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
    std::vector<hux::uint64_t>{1646435958010, 1646435958060, 1646435958110, 1646435958160, 1646435958210, 1646435958260}));


HUX_DECLARE_SIMULATION_DATA(sim_dataset_3, hux::make_tuple(
    std::vector<float>{0.01, 0.22, 0.23, 0.24, 0.25, 0.26},
    std::vector<float>{0.93, 0.54, 0.55, 0.56, 0.57, 0.58},
    std::vector<float>{-1.0, -0.9, -0.8, -0.7, -0.6, -0.5},
    std::vector<float>{0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
    std::vector<float>{0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
    std::vector<float>{0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
    std::vector<hux::uint64_t>{1646435958010, 1646435958060, 1646435958110, 1646435958160, 1646435958210, 1646435958260}));

/* ------------ declare sensors ------------ */
HUX_DECLARE_SENSOR(my_accelerometer_1, hux::sensors::STMicroelectronics::ISM330DHCX, sim_dataset_1);
HUX_DECLARE_SENSOR(my_accelerometer_2, hux::sensors::STMicroelectronics::ISM330DHCX, sim_dataset_2);
HUX_DECLARE_SENSOR(my_accelerometer_3, hux::sensors::STMicroelectronics::ISM330DHCX, sim_dataset_3);

/* ------------ declare sensor configuration ------------ */

/* -------- define the computation --------- */
HUX_DECLARE_CHANNEL(acc_1_ch, zip_latest, 
    my_accelerometer_1.get_accX(),
    my_accelerometer_1.get_accY(),
    my_accelerometer_1.get_accZ()
);

HUX_DECLARE_CHANNEL(acc_2_ch, zip_latest,
    my_accelerometer_2.get_accX(),
    my_accelerometer_2.get_accY(),
    my_accelerometer_2.get_accZ()
);

HUX_DECLARE_CHANNEL( acc_3_ch, zip_latest, 
    my_accelerometer_3.get_accX(),
    my_accelerometer_3.get_accY(),
    my_accelerometer_3.get_accZ()
);

HUX_DECLARE_PROCESSING(acc_1_node, acc_1_ch, {
    float x = hux::get<0>(hux_input);
    float y = hux::get<1>(hux_input);
    float z = hux::get<2>(hux_input);

    return (abs(x)+abs(y)+abs(z));
});

HUX_DECLARE_PROCESSING(acc_2_node, acc_2_ch, {
    float x = hux::get<0>(hux_input);
    float y = hux::get<1>(hux_input);
    float z = hux::get<2>(hux_input);

    return (abs(x)+abs(y)+abs(z));
});

HUX_DECLARE_PROCESSING(acc_3_node, acc_3_ch, {
    float x = hux::get<0>(hux_input);
    float y = hux::get<1>(hux_input);
    float z = hux::get<2>(hux_input);

    return (abs(x)+abs(y)+abs(z));
});
HUX_DECLARE_CHANNEL(acc_1_buf, buffer<BUF_SIZE>, acc_1_node);
HUX_DECLARE_CHANNEL(acc_2_buf, buffer<BUF_SIZE>, acc_2_node);
HUX_DECLARE_CHANNEL(acc_3_buf, buffer<BUF_SIZE>, acc_3_node);

HUX_DECLARE_CHANNEL(acc_1_buf_time_ch, 
                    zip_latest, 
                    acc_1_buf,
                    my_accelerometer_1.get_timestamp());

HUX_DECLARE_CHANNEL(acc_2_buf_time_ch,
                    zip_latest,  
                    acc_2_buf,
                    my_accelerometer_2.get_timestamp());

HUX_DECLARE_CHANNEL(acc_3_buf_time_ch,
                    zip_latest, 
                    acc_3_buf,
                    my_accelerometer_3.get_timestamp());

HUX_DECLARE_PROCESSING(acc_1_avg_node, acc_1_buf_time_ch, {
    acc_timestamp_t result;

    auto            acc_buf         = hux::get<0>(hux_input);
    hux::uint64_t   timestamp       = hux::get<1>(hux_input);
    size_t          buffer_size     = acc_buf.size();

    result.xyz_abs      = 0;
    result.tamper       = false;
    result.timestamp    = timestamp;

    for (int i = 0; i < buffer_size; i++) {
        if (acc_buf[i] >= ACC_AVG_THR) {
            result.xyz_abs      = acc_buf[i];
            result.tamper       = true;

            break;
        }
    }

    return result;
});

HUX_DECLARE_PROCESSING(acc_2_avg_node, acc_2_buf_time_ch, {
    acc_timestamp_t result;

    auto            acc_buf         = hux::get<0>(hux_input);
    hux::uint64_t   timestamp       = hux::get<1>(hux_input);
    size_t          buffer_size     = acc_buf.size();

    result.xyz_abs      = 0;
    result.tamper       = false;
    result.timestamp    = timestamp;

    for (int i = 0; i < buffer_size; i++) {
        if (acc_buf[i] >= ACC_AVG_THR) {
            result.xyz_abs      = acc_buf[i];
            result.tamper       = true;

            break;
        }
    }

    return result;
});

HUX_DECLARE_PROCESSING(acc_3_avg_node, acc_3_buf_time_ch, {
    acc_timestamp_t result;

    auto            acc_buf         = hux::get<0>(hux_input);
    hux::uint64_t   timestamp       = hux::get<1>(hux_input);
    size_t          buffer_size     = acc_buf.size();

    result.xyz_abs      = 0;
    result.tamper       = false;
    result.timestamp    = timestamp;

    for (int i = 0; i < buffer_size; i++) {
        if (acc_buf[i] >= ACC_AVG_THR) {
            result.xyz_abs      = acc_buf[i];
            result.tamper       = true;

            break;
        }
    }

    return result;
});

HUX_DECLARE_CHANNEL(acc_avg_ch,
                    combine_latest,  
                    acc_1_avg_node,
                    acc_2_avg_node,
                    acc_3_avg_node);

HUX_DECLARE_PROCESSING(acc_output_node, acc_avg_ch, {
    HUX_DECLARE_OUTPUT_VALUE(acc_abs_sum_1, Float, "acc_abs_sum_1", hux::get<0>(hux_input).xyz_abs);
    HUX_DECLARE_OUTPUT_VALUE(tamper_1, Boolean, "tamper_1", hux::get<0>(hux_input).tamper);
    HUX_DECLARE_OUTPUT_VALUE(timestamp_1, Long, "timestamp_1", hux::get<0>(hux_input).timestamp);

    HUX_DECLARE_OUTPUT_VALUE(acc_abs_sum_2, Float, "acc_abs_sum_2", hux::get<1>(hux_input).xyz_abs);
    HUX_DECLARE_OUTPUT_VALUE(tamper_2, Boolean, "tamper_2", hux::get<1>(hux_input).tamper);
    HUX_DECLARE_OUTPUT_VALUE(timestamp_2, Long, "timestamp_2", hux::get<1>(hux_input).timestamp);

    HUX_DECLARE_OUTPUT_VALUE(acc_abs_sum_3, Float, "acc_abs_sum_3", hux::get<2>(hux_input).xyz_abs);
    HUX_DECLARE_OUTPUT_VALUE(tamper_3, Boolean, "tamper_3", hux::get<2>(hux_input).tamper);
    HUX_DECLARE_OUTPUT_VALUE(timestamp_3, Long, "timestamp_3", hux::get<2>(hux_input).timestamp);

    HUX_DECLARE_OUTPUT_VALUE(acc_obj, Object, "accelerometer", 
                                                        acc_abs_sum_1,
                                                        tamper_1,
                                                        timestamp_1,                                           
                                                        acc_abs_sum_2,
                                                        tamper_2,
                                                        timestamp_2,
                                                        acc_abs_sum_3,
                                                        tamper_3,
                                                        timestamp_3);
    
    return acc_obj;
});

HUX_REGISTER_OUTPUT(acc_output_node);

#endif/* __TAMPERINGDETECTION_HPP__ */