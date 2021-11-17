#ifndef __JSON2DAISY_FIELD_H__
#define __JSON2DAISY_FIELD_H__

#include "daisy_seed.h"

namespace json2daisy {

struct DaisyField {

  /** Initializes the board according to the JSON board description
   *  \param boost boosts the clock speed from 400 to 480 MHz
   */
  void Init(bool boost=true) 
  {
    som.Configure();
    som.Init(boost);

    // i2c
    i2c.Init({daisy::I2CHandle::Config::Peripheral::I2C_1, {som.GetPin(11), som.GetPin(12)}, daisy::I2CHandle::Config::Speed::I2C_1MHZ}); 
 
    // LED Drivers
    led_driver.Init(i2c, {0x00, 0x02}, led_driver_dma_buffer_a, led_driver_dma_buffer_b); 
 
    // Switches
    sw1.Init(som.GetPin(30), som.AudioCallbackRate(), daisy::Switch::TYPE_MOMENTARY, daisy::Switch::POLARITY_INVERTED, daisy::Switch::PULL_UP);
    sw2.Init(som.GetPin(29), som.AudioCallbackRate(), daisy::Switch::TYPE_MOMENTARY, daisy::Switch::POLARITY_INVERTED, daisy::Switch::PULL_UP); 
 
    // Muxes
    pad_shift.Init({ som.GetPin(28), som.GetPin(27), { som.GetPin(26) } }); 
 
    // Gate ins
    dsy_gpio_pin gatein_pin = som.GetPin(0);
    gatein.Init(&gatein_pin); 
 
    // Single channel ADC initialization
    cfg[0].InitSingle(som.GetPin(17));
    cfg[1].InitSingle(som.GetPin(18));
    cfg[2].InitSingle(som.GetPin(25));
    cfg[3].InitSingle(som.GetPin(24));
    size_t pot_mux_index = 4;
    cfg[pot_mux_index].InitMux(som.GetPin(16), 8, som.GetPin(21), som.GetPin(20), som.GetPin(19)); 
    som.adc.Init(cfg, ANALOG_COUNT);
 
    // AnalogControl objects
    cv1.InitBipolarCv(som.adc.GetPtr(0), som.AudioCallbackRate());
    cv2.InitBipolarCv(som.adc.GetPtr(1), som.AudioCallbackRate());
    cv3.InitBipolarCv(som.adc.GetPtr(2), som.AudioCallbackRate());
    cv4.InitBipolarCv(som.adc.GetPtr(3), som.AudioCallbackRate()); 
 
    // Multiplexed AnlogControl objects
    knob1.Init(som.adc.GetMuxPtr(pot_mux_index, 0), som.AudioCallbackRate(), false, false);
    knob2.Init(som.adc.GetMuxPtr(pot_mux_index, 3), som.AudioCallbackRate(), false, false);
    knob3.Init(som.adc.GetMuxPtr(pot_mux_index, 1), som.AudioCallbackRate(), false, false);
    knob4.Init(som.adc.GetMuxPtr(pot_mux_index, 4), som.AudioCallbackRate(), false, false);
    knob5.Init(som.adc.GetMuxPtr(pot_mux_index, 2), som.AudioCallbackRate(), false, false);
    knob6.Init(som.adc.GetMuxPtr(pot_mux_index, 5), som.AudioCallbackRate(), false, false);
    knob7.Init(som.adc.GetMuxPtr(pot_mux_index, 6), som.AudioCallbackRate(), false, false);
    knob8.Init(som.adc.GetMuxPtr(pot_mux_index, 7), som.AudioCallbackRate(), false, false); 
 
    // Gate outs
    gateout.pin  = som.GetPin(15);
    gateout.mode = DSY_GPIO_MODE_OUTPUT_PP;
    gateout.pull = DSY_GPIO_NOPULL;
    dsy_gpio_init(&gateout); 
    // DAC 
    cvout1.bitdepth = daisy::DacHandle::BitDepth::BITS_12;
    cvout1.buff_state = daisy::DacHandle::BufferState::ENABLED;
    cvout1.mode = daisy::DacHandle::Mode::POLLING;
    cvout1.chn = daisy::DacHandle::Channel::BOTH;
    som.dac.Init(cvout1);
    som.dac.WriteValue(daisy::DacHandle::Channel::BOTH, 0);
    cvout2.bitdepth = daisy::DacHandle::BitDepth::BITS_12;
    cvout2.buff_state = daisy::DacHandle::BufferState::ENABLED;
    cvout2.mode = daisy::DacHandle::Mode::POLLING;
    cvout2.chn = daisy::DacHandle::Channel::BOTH;
    som.dac.Init(cvout2);
    som.dac.WriteValue(daisy::DacHandle::Channel::BOTH, 0); 
    // Display
    daisy::OledDisplay<daisy::SSD130x4WireSpi128x64Driver>::Config display_config;
    display_config.driver_config.transport_config.Defaults();
    display.Init(display_config);
 


    som.adc.Start();
  }

  /** Handles all the controls processing that needs to occur at the block rate
   * 
   */
  void ProcessAllControls() 
  {
 
    cv1.Process();
    cv2.Process();
    cv3.Process();
    cv4.Process();
    knob1.Process();
    knob2.Process();
    knob3.Process();
    knob4.Process();
    knob5.Process();
    knob6.Process();
    knob7.Process();
    knob8.Process();
    sw1.Debounce();
    sw2.Debounce(); 
  }

  /** Sets the audio sample rate
   *  \param sample_rate the new sample rate in Hz
   */
  void SetAudioSampleRate(size_t sample_rate) 
  {
    SaiHandle::Config::SampleRate enum_rate;
    if (sample_rate >= 96000)
      enum_rate = SAI_96KHZ;
    else if (sample_rate >= 48000)
      enum_rate = SAI_48KHZ;
    else if (sample_rate >= 32000)
      enum_rate = SAI_32KHZ;
    else if (sample_rate >= 16000)
      enum_rate = SAI_16KHZ;
    else
      enum_rate = SAI_8KHZ;
    som.SetAudioSampleRate(enum_rate);
    
  }

  /** Sets the audio block size
   *  \param block_size the new block size in words
   */
  inline void SetAudioBlockSize(size_t block_size) 
  {
    som.SetAudioBlockSize(block_size);
  }

  /** Starts up the audio callback process with the given callback
   * 
   */
  inline void StartAudio(AudioHandle::AudioCallback cb)
  {
    som.StartAudio(cb);
  }

  /** This is the board's "System On Module"
   */
  daisy::DaisySeed som;

  // I/O Components
  daisy::AnalogControl cv1;
  daisy::AnalogControl cv2;
  daisy::AnalogControl cv3;
  daisy::AnalogControl cv4;
  ShiftRegister4021<2> pad_shift;
  daisy::AnalogControl knob1;
  daisy::AnalogControl knob2;
  daisy::AnalogControl knob3;
  daisy::AnalogControl knob4;
  daisy::AnalogControl knob5;
  daisy::AnalogControl knob6;
  daisy::AnalogControl knob7;
  daisy::AnalogControl knob8;
  daisy::DacHandle::Config cvout1;
  daisy::DacHandle::Config cvout2;
  daisy::GateIn gatein;
  dsy_gpio gateout;
  LedDriverPca9685<2, true> led_driver;
  daisy::Switch sw1;
  daisy::Switch sw2;
  I2CHandle i2c;
  daisy::OledDisplay<daisy::SSD130x4WireSpi128x64Driver> display;

};

} // namspace json2daisy

#endif // __JSON2DAISY_FIELD_H__