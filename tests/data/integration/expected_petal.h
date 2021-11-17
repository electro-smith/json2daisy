#ifndef __JSON2DAISY_PETAL_H__
#define __JSON2DAISY_PETAL_H__

#include "daisy_seed.h"

namespace json2daisy {

struct DaisyPetal {

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
    sw1.Init(som.GetPin(8), som.AudioCallbackRate(), daisy::Switch::TYPE_MOMENTARY, daisy::Switch::POLARITY_INVERTED, daisy::Switch::PULL_UP);
    sw2.Init(som.GetPin(9), som.AudioCallbackRate(), daisy::Switch::TYPE_MOMENTARY, daisy::Switch::POLARITY_INVERTED, daisy::Switch::PULL_UP);
    sw3.Init(som.GetPin(10), som.AudioCallbackRate(), daisy::Switch::TYPE_MOMENTARY, daisy::Switch::POLARITY_INVERTED, daisy::Switch::PULL_UP);
    sw4.Init(som.GetPin(13), som.AudioCallbackRate(), daisy::Switch::TYPE_MOMENTARY, daisy::Switch::POLARITY_INVERTED, daisy::Switch::PULL_UP);
    sw5.Init(som.GetPin(25), som.AudioCallbackRate(), daisy::Switch::TYPE_MOMENTARY, daisy::Switch::POLARITY_INVERTED, daisy::Switch::PULL_UP);
    sw6.Init(som.GetPin(26), som.AudioCallbackRate(), daisy::Switch::TYPE_MOMENTARY, daisy::Switch::POLARITY_INVERTED, daisy::Switch::PULL_UP);
    sw7.Init(som.GetPin(7), som.AudioCallbackRate(), daisy::Switch::TYPE_MOMENTARY, daisy::Switch::POLARITY_INVERTED, daisy::Switch::PULL_UP); 
 
    // Rotary encoders
    encoder.Init(som.GetPin(28), som.GetPin(27), som.GetPin(14), som.AudioCallbackRate()); 
 
    // Single channel ADC initialization
    cfg[0].InitSingle(som.GetPin(16));
    cfg[1].InitSingle(som.GetPin(19));
    cfg[2].InitSingle(som.GetPin(17));
    cfg[3].InitSingle(som.GetPin(20));
    cfg[4].InitSingle(som.GetPin(18));
    cfg[5].InitSingle(som.GetPin(21));
    cfg[6].InitSingle(som.GetPin(15)); 
    som.adc.Init(cfg, ANALOG_COUNT);
 
    // AnalogControl objects
    knob1.Init(som.adc.GetPtr(0), som.AudioCallbackRate(), false, false);
    knob2.Init(som.adc.GetPtr(1), som.AudioCallbackRate(), false, false);
    knob3.Init(som.adc.GetPtr(2), som.AudioCallbackRate(), false, false);
    knob4.Init(som.adc.GetPtr(3), som.AudioCallbackRate(), false, false);
    knob5.Init(som.adc.GetPtr(4), som.AudioCallbackRate(), false, false);
    knob6.Init(som.adc.GetPtr(5), som.AudioCallbackRate(), false, false);
    expression.Init(som.adc.GetPtr(6), som.AudioCallbackRate(), false, false); 
    // Display
    // no display 


    som.adc.Start();
  }

  /** Handles all the controls processing that needs to occur at the block rate
   * 
   */
  void ProcessAllControls() 
  {
 
    knob1.Process();
    knob2.Process();
    knob3.Process();
    knob4.Process();
    knob5.Process();
    knob6.Process();
    expression.Process();
    encoder.Debounce();
    sw1.Debounce();
    sw2.Debounce();
    sw3.Debounce();
    sw4.Debounce();
    sw5.Debounce();
    sw6.Debounce();
    sw7.Debounce(); 
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
  daisy::AnalogControl knob1;
  daisy::AnalogControl knob2;
  daisy::AnalogControl knob3;
  daisy::AnalogControl knob4;
  daisy::AnalogControl knob5;
  daisy::AnalogControl knob6;
  daisy::AnalogControl expression;
  daisy::Encoder encoder;
  LedDriverPca9685<2, true> led_driver;
  daisy::Switch sw1;
  daisy::Switch sw2;
  daisy::Switch sw3;
  daisy::Switch sw4;
  daisy::Switch sw5;
  daisy::Switch sw6;
  daisy::Switch sw7;
  I2CHandle i2c;
  // no display

};

} // namspace json2daisy

#endif // __JSON2DAISY_PETAL_H__