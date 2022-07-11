#ifndef __PERSONROOMDETECTION_HPP__
#define __PERSONROOMDETECTION_HPP__

/* --------- include libraries -------- */ 
#include <sensor_language/entities.hpp>
#include <sensor_language/lang.hpp>
#include <sensor_language/sensors/STMicroelectronics.hpp>

#include <math.h>

/* --------- defines -------- */ 
#define MICROPHONE_VAR_THR      (0.01f)
#define MICROPHONE_VAR_TIME_MS  (10*1000) //10seconds
#define BUF_SIZE                60

/* --------- typedef --------- */
typedef struct {
    float mean;
    float var;
    bool  noise;
    hux::uint64_t timestamp;
} microphone_timestamp_t;

struct noise_detect_t {
    bool detected = false;
    hux::uint64_t last_noise_time;
};

/* --------- declare simulation data -------- */
HUX_DECLARE_SIMULATION_DATA(sim_dataset_1, hux::make_tuple(
    std::vector<float>{0.10, 0.11, 0.20, 0.10, 0.22, 0.04},
    std::vector<float>{0.03, 0.10, -0.05, -0.10, 0.31, 0.15},
    std::vector<hux::uint64_t>{1646435958000, 1646435958050, 1646435958100, 1646435958150, 1646435958200, 1646435958250}));

HUX_DECLARE_SIMULATION_DATA(sim_dataset_2, hux::make_tuple(
    std::vector<float>{0.50, 0.51, 0.01, 0.80, -0.22, -0.12},
    std::vector<float>{-0.10, 0.89, -0.56, 0.20, 0.88, -0.35},
    std::vector<hux::uint64_t>{1646435958010, 1646435958060, 1646435958110, 1646435958160, 1646435958210, 1646435958260}));

/* ------------ declare sensors ------------ */
HUX_DECLARE_SENSOR(my_microphone_1, hux::sensors::STMicroelectronics::IMP34DT05, sim_dataset_1);
HUX_DECLARE_SENSOR(my_microphone_2, hux::sensors::STMicroelectronics::IMP34DT05, sim_dataset_2);

/* ------------ declare sensor configuration ------------ */

/* -------- define the computation --------- */
HUX_DECLARE_CHANNEL(microphone_1_ch, zip_latest,  
    my_microphone_1.get_mean(),
    my_microphone_1.get_variance()
);

HUX_DECLARE_CHANNEL(microphone_2_ch, zip_latest, 
    my_microphone_2.get_mean(),
    my_microphone_2.get_variance()
);

HUX_DECLARE_PROCESSING(mean_1_node, microphone_1_ch, {
    float mean = hux::get<0>(hux_input);
    return mean;
});

HUX_DECLARE_PROCESSING(variance_1_node, microphone_1_ch, {
    float var = hux::get<1>(hux_input);
    return var;
});

HUX_DECLARE_PROCESSING(mean_2_node, microphone_2_ch, {
    float mean = hux::get<0>(hux_input);
    return mean;
});

HUX_DECLARE_PROCESSING(variance_2_node, microphone_2_ch, {
    float var = hux::get<1>(hux_input);
    return var;
});

HUX_DECLARE_CHANNEL(mean_1_buf, buffer<BUF_SIZE>,mean_1_node);
HUX_DECLARE_CHANNEL(variance_1_buf, buffer<BUF_SIZE>,variance_1_node);

HUX_DECLARE_CHANNEL(mean_2_buf, buffer<BUF_SIZE>,mean_2_node);
HUX_DECLARE_CHANNEL(variance_2_buf, buffer<BUF_SIZE>,variance_2_node);

HUX_DECLARE_CHANNEL(microphone_1_buf_time_ch, 
                    zip_latest, 
                    mean_1_buf,
                    variance_1_buf,
                    my_microphone_1.get_timestamp());

HUX_DECLARE_CHANNEL(microphone_2_buf_time_ch, 
                    zip_latest, 
                    mean_2_buf,
                    variance_2_buf,
                    my_microphone_2.get_timestamp());

HUX_DECLARE_STATEFUL_PROCESSING(microphone_1_avg_node, noise_detect_t, microphone_1_buf_time_ch, {
    microphone_timestamp_t result;

    auto &mean_buffer       = hux::get<0>(hux_input);
    auto &var_buffer        = hux::get<1>(hux_input);
    hux::uint64_t timestamp = hux::get<2>(hux_input);

    size_t buffer_size  = mean_buffer.size();
    float sense_sum     = 0.0f;

    for(int i = 0; i < buffer_size; i++) {
        sense_sum += mean_buffer[i];
    }

    result.mean = sense_sum / buffer_size;

    buffer_size = var_buffer.size();
    sense_sum   = 0.0f;

    for(int i = 0; i < buffer_size; i++) {
        sense_sum += var_buffer[i];
    }

    result.var = sense_sum / buffer_size;

    result.timestamp = timestamp;

    if (result.var >= MICROPHONE_VAR_THR) {
        hux_state.detected = true;
        hux_state.last_noise_time = timestamp;
    }

    if ( (hux_state.detected == true) && (timestamp - hux_state.last_noise_time) > MICROPHONE_VAR_TIME_MS ) {
        hux_state.detected  = false;
    }

    result.noise = hux_state.detected;
    
    return result;
});

HUX_DECLARE_STATEFUL_PROCESSING(microphone_2_avg_node, noise_detect_t, microphone_2_buf_time_ch, {
    microphone_timestamp_t result;

    auto &mean_buffer       = hux::get<0>(hux_input);
    auto &var_buffer        = hux::get<1>(hux_input);
    hux::uint64_t timestamp = hux::get<2>(hux_input);

    size_t buffer_size  = mean_buffer.size();
    float sense_sum     = 0.0f;

    for(int i = 0; i < buffer_size; i++) {
        sense_sum += mean_buffer[i];
    }

    result.mean = sense_sum / buffer_size;

    buffer_size = var_buffer.size();
    sense_sum   = 0.0f;

    for(int i = 0; i < buffer_size; i++) {
        sense_sum += var_buffer[i];
    }

    result.var = sense_sum / buffer_size;

    result.timestamp = timestamp;

    if (result.var >= MICROPHONE_VAR_THR) {
        hux_state.detected = true;
        hux_state.last_noise_time = timestamp;
    }

    if ( (hux_state.detected == true) && (timestamp - hux_state.last_noise_time) > MICROPHONE_VAR_TIME_MS ) {
        hux_state.detected  = false;
    }

    result.noise = hux_state.detected;
    
    return result;
});

HUX_DECLARE_CHANNEL(microphone_avg_ch, 
                    combine_latest, 
                    microphone_1_avg_node,
                    microphone_2_avg_node);

HUX_DECLARE_PROCESSING(microphone_output_node, microphone_avg_ch, {
    HUX_DECLARE_OUTPUT_VALUE(mean_1_average, Float, "mean_1", hux::get<0>(hux_input).mean);
    HUX_DECLARE_OUTPUT_VALUE(var_1_average, Float, "variance_1", hux::get<0>(hux_input).var);
    HUX_DECLARE_OUTPUT_VALUE(noise_1, Boolean, "noise_1", hux::get<0>(hux_input).noise);
    HUX_DECLARE_OUTPUT_VALUE(timestamp_1, Long, "timestamp_1", hux::get<0>(hux_input).timestamp);

    HUX_DECLARE_OUTPUT_VALUE(mean_2_average, Float, "mean_2", hux::get<1>(hux_input).mean);
    HUX_DECLARE_OUTPUT_VALUE(var_2_average, Float, "variance_2", hux::get<1>(hux_input).var);
    HUX_DECLARE_OUTPUT_VALUE(noise_2, Boolean, "noise_2", hux::get<1>(hux_input).noise);
    HUX_DECLARE_OUTPUT_VALUE(timestamp_2, Long, "timestamp_2", hux::get<1>(hux_input).timestamp);

    HUX_DECLARE_OUTPUT_VALUE(microphone_obj, Object, "microphone", 
                                                        mean_1_average,
                                                        var_1_average,
                                                        noise_1,
                                                        timestamp_1,
                                                        mean_2_average,
                                                        var_2_average,
                                                        noise_2,
                                                        timestamp_2);

    return microphone_obj;
});

HUX_REGISTER_OUTPUT(microphone_output_node);

#endif/* __PERSONROOMDETECTION_HPP__ */