#ifndef __CLIMATICMONITORING_HPP__
#define __CLIMATICMONITORING_HPP__

/* --------- include libraries -------- */ 
#include <sensor_language/entities.hpp>
#include <sensor_language/lang.hpp>
#include <sensor_language/sensors/STMicroelectronics.hpp>

#include <math.h>
#include <sensor_language/sensors/STMicroelectronics/HTS221.hpp>

/* --------- defines -------- */ 

/* --------- typedef --------- */
typedef struct {
    float temp;
    float hum;
    hux::uint64_t timestamp;
} humiditytemp_timestamp_t;

/* --------- declare simulation data -------- */
HUX_DECLARE_SIMULATION_DATA(sim_dataset, hux::make_tuple(
    std::vector<float>{1, 2, 3, 4, 5, 6},
    std::vector<float>{7, 8, 9, 10, 11, 12},
    std::vector<hux::uint64_t>{1646435958000, 1646435958050, 1646435958100, 1646435958150, 1646435958200, 1646435958250}));

/* ------------ declare sensor configuration ------------ */
HUX_DECLARE_SENSOR_CONFIGURATION(humtemp_cfg, hux::sensors::STMicroelectronics::HTS221, 
    .odr = hux::sensors::STMicroelectronics::HTS221::configs::odr_1);

/* ------------ declare sensors ------------ */
HUX_DECLARE_SENSOR(my_humtemp, hux::sensors::STMicroelectronics::HTS221, sim_dataset, humtemp_cfg);


/* -------- define the computation --------- */
HUX_DECLARE_CHANNEL(temperature_buf, buffer<60>, my_humtemp.get_temperature());
HUX_DECLARE_CHANNEL(humidity_buf, buffer<60>, my_humtemp.get_humidity());

HUX_DECLARE_CHANNEL(humiditytemp_buf_time_ch, zip_latest, 
    temperature_buf,
    humidity_buf,
    my_humtemp.get_timestamp()
);

HUX_DECLARE_PROCESSING(humiditytemp_avg_node, humiditytemp_buf_time_ch, {
    humiditytemp_timestamp_t result;

    auto &temp_buffer       = hux::get<0>(hux_input);
    auto &pres_buffer       = hux::get<1>(hux_input);
    hux::uint64_t timestamp = hux::get<2>(hux_input);

    size_t buffer_size  = temp_buffer.size();
    float sense_sum     = 0.0f;

    for(int i = 0; i < buffer_size; i++) {
        sense_sum += temp_buffer[i];
    }

    result.temp = sense_sum / buffer_size;

    buffer_size = pres_buffer.size();
    sense_sum   = 0.0f;

    for(int i = 0; i < buffer_size; i++) {
        sense_sum += pres_buffer[i];
    }

    result.hum = sense_sum / buffer_size;

    result.timestamp = timestamp;
    
    return result;

});

HUX_DECLARE_CHANNEL(humiditytemp_avg_ch, merge, humiditytemp_avg_node);

HUX_DECLARE_PROCESSING(climaticmonitor_output_node, humiditytemp_avg_ch, {
    HUX_DECLARE_OUTPUT_VALUE(temperature_average, Float, "temperature", hux_input.temp);
    HUX_DECLARE_OUTPUT_VALUE(humidity_average, Float, "humidity", hux_input.hum);
    HUX_DECLARE_OUTPUT_VALUE(timestamp, Long, "timestamp", hux_input.timestamp);

    HUX_DECLARE_OUTPUT_VALUE(climaticmonitor_obj, Object, "temperaturehumidity", 
                                                        temperature_average,
                                                        humidity_average,
                                                        timestamp);

    return climaticmonitor_obj;
})

HUX_REGISTER_OUTPUT(climaticmonitor_output_node);

#endif/* __CLIMATICMONITORING_HPP__ */