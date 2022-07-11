#ifndef __OPENWINDOWDETECTION_HPP__
#define __OPENWINDOWDETECTION_HPP__

/* --------- include libraries -------- */ 
#include <sensor_language/entities.hpp>
#include <sensor_language/lang.hpp>
#include <sensor_language/sensors/STMicroelectronics.hpp>

#include <math.h>
#include <sensor_language/simulation/data_loader.hpp>
#include <sensor_language/types.hpp>

/* --------- defines -------- */ 
#define BUF_SIZE            60
#define GYR_THR             (1.0f)

/* --------- typedef --------- */
typedef struct {
    float gyr_val;
    bool  window_opened;
    hux::uint64_t timestamp;
} gyro_window_timestamp_t;

// declare types for stateful processing
struct window_state_t{
    bool last_window_status = false;
};

/* --------- declare simulation data -------- */
HUX_DECLARE_SIMULATION_DATA(sim_dataset_1, hux::simulation::load_csv<float, float, float, float, float, float, hux::uint64_t>("./dataset1.csv", ";"));
HUX_DECLARE_SIMULATION_DATA(sim_dataset_2, hux::simulation::load_csv<float, float, float, float, float, float, hux::uint64_t>("./dataset2.csv", ";"));

/* ------------ declare sensors ------------ */
HUX_DECLARE_SENSOR(my_accelerometer_1, hux::sensors::STMicroelectronics::ISM330DHCX, sim_dataset_1);
HUX_DECLARE_SENSOR(my_accelerometer_2, hux::sensors::STMicroelectronics::ISM330DHCX, sim_dataset_2);

/* ------------ declare sensor configuration ------------ */

/* -------- define the computation --------- */
HUX_DECLARE_CHANNEL(gyr_1_ch, zip_latest, 
    my_accelerometer_1.get_gyrX(),
    my_accelerometer_1.get_gyrY(),
    my_accelerometer_1.get_gyrZ()   
);

HUX_DECLARE_CHANNEL(gyr_2_ch, zip_latest, 
    my_accelerometer_2.get_gyrX(),
    my_accelerometer_2.get_gyrY(),
    my_accelerometer_2.get_gyrZ()   
);

HUX_DECLARE_PROCESSING(gyr_1_node, gyr_1_ch, {
    float gyr_val = hux::get<1>(hux_input);
    return gyr_val;
});

HUX_DECLARE_PROCESSING(gyr_2_node, gyr_2_ch, {
    float gyr_val = hux::get<1>(hux_input);
    return gyr_val;
});

HUX_DECLARE_CHANNEL(gyr_1_buf, buffer<BUF_SIZE>, gyr_1_node);
HUX_DECLARE_CHANNEL(gyr_2_buf, buffer<BUF_SIZE>, gyr_2_node);

HUX_DECLARE_CHANNEL(gyr_1_buf_time_ch, zip_latest, 
    gyr_1_buf,
    my_accelerometer_1.get_timestamp()
);

HUX_DECLARE_CHANNEL(gyr_2_buf_time_ch, zip_latest, 
    gyr_2_buf,
    my_accelerometer_2.get_timestamp()
);

HUX_DECLARE_STATEFUL_PROCESSING(gyr_1_avg_node, window_state_t, gyr_1_buf_time_ch, {
    auto &buffer            = hux::get<0>(hux_input);
    hux::uint64_t timestamp = hux::get<1>(hux_input);
    gyro_window_timestamp_t result;

    size_t buffer_size = buffer.size();

    result.window_opened    = hux_state.last_window_status;
    result.gyr_val            = 0;
    result.timestamp        = timestamp;

    for(int i = 0; i < buffer_size; i++) {
        if (buffer[i] > GYR_THR) {
            //Open window event
            result.window_opened = true;
            result.gyr_val = buffer[i];
        }

        if (buffer[i] < -GYR_THR) {
            //Close window event
            result.window_opened = false;
            result.gyr_val = buffer[i];
        }
    }

    hux_state.last_window_status = result.window_opened;

    return result;
});

HUX_DECLARE_STATEFUL_PROCESSING(gyr_2_avg_node, window_state_t, gyr_2_buf_time_ch, {
    auto &buffer            = hux::get<0>(hux_input);
    hux::uint64_t timestamp = hux::get<1>(hux_input);
    gyro_window_timestamp_t result;

    size_t buffer_size = buffer.size();

    result.window_opened    = hux_state.last_window_status;
    result.gyr_val            = 0;
    result.timestamp        = timestamp;

    for(int i = 0; i < buffer_size; i++) {
        if (buffer[i] > GYR_THR) {
            //Open window event
            result.window_opened = true;
            result.gyr_val = buffer[i];
        }

        if (buffer[i] < -GYR_THR) {
            //Close window event
            result.window_opened = false;
            result.gyr_val = buffer[i];
        }
    }

    hux_state.last_window_status = result.window_opened;

    return result;
});

HUX_DECLARE_CHANNEL(gyr_avg_ch, 
    combine_latest,  
    gyr_1_avg_node,
    gyr_2_avg_node);

HUX_DECLARE_PROCESSING(gyr_output_node, gyr_avg_ch, {
    HUX_DECLARE_OUTPUT_VALUE(gyr_val_1, Float, "gyr_val_1", hux::get<0>(hux_input).gyr_val);
    HUX_DECLARE_OUTPUT_VALUE(window_opened_1, Boolean, "window_opened_1", hux::get<0>(hux_input).window_opened);
    HUX_DECLARE_OUTPUT_VALUE(timestamp_1, Long, "timestamp_1", hux::get<0>(hux_input).timestamp);

    HUX_DECLARE_OUTPUT_VALUE(gyr_val_2, Float, "gyr_val_2", hux::get<1>(hux_input).gyr_val);
    HUX_DECLARE_OUTPUT_VALUE(window_opened_2, Boolean, "window_opened_2", hux::get<1>(hux_input).window_opened);
    HUX_DECLARE_OUTPUT_VALUE(timestamp_2, Long, "timestamp_2", hux::get<1>(hux_input).timestamp);

    HUX_DECLARE_OUTPUT_VALUE(gyr_obj, Object, "gyroscope", 
                                                        gyr_val_1,
                                                        window_opened_1,
                                                        timestamp_1,
                                                        gyr_val_2,
                                                        window_opened_2,
                                                        timestamp_2);
    
    return gyr_obj;
});

HUX_REGISTER_OUTPUT(gyr_output_node);

#endif/* __OPENWINDOWDETECTION_HPP__ */