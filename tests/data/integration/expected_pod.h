#ifndef __JSON2DAISY_POD_H__
#define __JSON2DAISY_POD_H__

#include "daisy_seed.h"

namespace json2daisy {

struct DaisyPod {

  /** Initializes the board according to the JSON board description
   *  \param boost boosts the clock speed from 400 to 480 MHz
   */
  void Init(bool boost=true) 
  {
    som.Configure();
    som.Init(boost);

 
    // Switches
    sw1.Init(som.GetPin(27), som.AudioCallbackRate(), daisy::Switch::TYPE_MOMENTARY, daisy::Switch::POLARITY_INVERTED, daisy::Switch::PULL_UP);
    sw2.Init(som.GetPin(28), som.AudioCallbackRate(), daisy::Switch::TYPE_MOMENTARY, daisy::Switch::POLARITY_INVERTED, daisy::Switch::PULL_UP); 
    // SPDT Switches
    sw3.Init(som.GetPin(1), som.GetPin(2)); 
 
    // Gate ins
    dsy_gpio_pin gatein_pin = som.GetPin(12);
    gatein.Init(&gatein_pin); 
 
    // Rotary encoders
    encoder.Init(som.GetPin(26), som.GetPin(25), som.GetPin(13), som.AudioCallbackRate()); 
 
    // Single channel ADC initialization
    cfg[0].InitSingle(som.GetPin(21));
    cfg[1].InitSingle(som.GetPin(15)); 
    som.adc.Init(cfg, ANALOG_COUNT);
 
    // AnalogControl objects
    knob1.Init(som.adc.GetPtr(0), som.AudioCallbackRate(), false, false);
    knob2.Init(som.adc.GetPtr(1), som.AudioCallbackRate(), false, false); 
 
    // LEDs
    led3.Init(som.GetPin(10), true);
    led3.Set(0.0f); 
    // RBG LEDs 
    led1.Init(som.GetPin(20), som.GetPin(19), som.GetPin(18), true);
    led1.Set(0.0f, 0.0f, 0.0f);
    led2.Init(som.GetPin(17), som.GetPin(24), som.GetPin(23), true);
    led2.Set(0.0f, 0.0f, 0.0f); 
    // DAC 
    cvout1.bitdepth = daisy::DacHandle::BitDepth::BITS_12;
    cvout1.buff_state = daisy::DacHandle::BufferState::ENABLED;
    cvout1.mode = daisy::DacHandle::Mode::POLLING;
    cvout1.chn = daisy::DacHandle::Channel::BOTH;
    som.dac.Init(cvout1);
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
 
    knob1.Process();
    knob2.Process();
    encoder.Debounce();
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
  daisy::AnalogControl knob1;
  daisy::AnalogControl knob2;
  daisy::DacHandle::Config cvout1;
  daisy::Encoder encoder;
  daisy::GateIn gatein;
  daisy::Led led3;
  daisy::RgbLed led1;
  daisy::RgbLed led2;
  daisy::Switch sw1;
  daisy::Switch sw2;
  daisy::Switch3 sw3;
  daisy::OledDisplay<daisy::SSD130x4WireSpi128x64Driver> display;

};

} // namspace json2daisy

#endif // __JSON2DAISY_POD_H__