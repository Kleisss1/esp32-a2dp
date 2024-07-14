#include "BluetoothA2DPSink.h"
//#include <Wire.h>

BluetoothA2DPSink a2dp_sink;

void setup() {

    i2s_pin_config_t my_pin_config = {
        .bck_io_num = 27, ////BCLK 
        .ws_io_num = 26,  ////LRC pin 
        .data_out_num = 25, ///Din pin
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    a2dp_sink.set_pin_config(my_pin_config);
    a2dp_sink.start("ESP32JVtomamango");

    static const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
        .sample_rate = 44100, // corrected by info from bluetooth
        .bits_per_sample = (i2s_bits_per_sample_t) 16, /* the DAC module will only take the 8bits from MSB */
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_STAND_MSB,
        .intr_alloc_flags = 0, // default interrupt priority
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false
    };

    //a2dp_sink.set_i2s_config(i2s_config);

}

void loop() {
}