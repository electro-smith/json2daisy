#ifndef __JSON2DAISY_PATCH_H__
#define __JSON2DAISY_PATCH_H__

#include "daisy_seed.h"

namespace json2daisy {

struct DaisyPatch {

  /** Initializes the board according to the JSON board description
   *  \param boost boosts the clock speed from 400 to 480 MHz
   */
  void Init(bool boost=true) 
  {
    som.Configure();
    som.Init(boost);

 
    // Gate ins
    dsy_gpio_pin gatein1_pin = som.GetPin(20);
    gatein1.Init(&gatein1_pin);
    dsy_gpio_pin gatein2_pin = som.GetPin(19);
    gatein2.Init(&gatein2_pin); 
 
    // Rotary encoders
    encoder.Init(som.GetPin(12), som.GetPin(11), som.GetPin(0), som.AudioCallbackRate()); 
 
    // Single channel ADC initialization
    cfg[0].InitSingle(som.GetPin(15));
    cfg[1].InitSingle(som.GetPin(16));
    cfg[2].InitSingle(som.GetPin(21));
    cfg[3].InitSingle(som.GetPin(18)); 
    som.adc.Init(cfg, ANALOG_COUNT);
 
    // AnalogControl objects
    knob1.Init(som.adc.GetPtr(0), som.AudioCallbackRate(), false, false);
    knob2.Init(som.adc.GetPtr(1), som.AudioCallbackRate(), false, false);
    knob3.Init(som.adc.GetPtr(2), som.AudioCallbackRate(), false, false);
    knob4.Init(som.adc.GetPtr(3), som.AudioCallbackRate(), false, false); 
 
    // Gate outs
    gateout.pin  = som.GetPin(17);
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
 

    // External Codec Initialization
    SaiHandle::Config sai_config[2];

    // Internal Codec
    sai_config[0].periph          = SaiHandle::Config::Peripheral::SAI_1;
    sai_config[0].sr              = SaiHandle::Config::SampleRate::SAI_48KHZ;
    sai_config[0].bit_depth       = SaiHandle::Config::BitDepth::SAI_24BIT;
    sai_config[0].a_sync          = SaiHandle::Config::Sync::MASTER;
    sai_config[0].b_sync          = SaiHandle::Config::Sync::SLAVE;
    sai_config[0].a_dir           = SaiHandle::Config::Direction::TRANSMIT;
    sai_config[0].b_dir           = SaiHandle::Config::Direction::RECEIVE;
    sai_config[0].pin_config.fs   = {DSY_GPIOE, 4};
    sai_config[0].pin_config.mclk = {DSY_GPIOE, 2};
    sai_config[0].pin_config.sck  = {DSY_GPIOE, 5};
    sai_config[0].pin_config.sa   = {DSY_GPIOE, 6};
    sai_config[0].pin_config.sb   = {DSY_GPIOE, 3};

    sai_config[1].periph          = SaiHandle::Config::Peripheral::SAI_2;
    sai_config[1].sr              = SaiHandle::Config::SampleRate::SAI_48KHZ;
    sai_config[1].bit_depth       = SaiHandle::Config::BitDepth::SAI_24BIT;
    sai_config[1].a_sync          = SaiHandle::Config::Sync::SLAVE;
    sai_config[1].b_sync          = SaiHandle::Config::Sync::MASTER;
    sai_config[1].a_dir           = SaiHandle::Config::Direction::TRANSMIT;
    sai_config[1].b_dir           = SaiHandle::Config::Direction::RECEIVE;
    sai_config[1].pin_config.fs   = som.GetPin(27);
    sai_config[1].pin_config.mclk = som.GetPin(24);
    sai_config[1].pin_config.sck  = som.GetPin(28);
    sai_config[1].pin_config.sa   = som.GetPin(26);
    sai_config[1].pin_config.sb   = som.GetPin(25);

    SaiHandle sai_handle[2];
    sai_handle[0].Init(sai_config[0]);
    sai_handle[1].Init(sai_config[1]);

    dsy_gpio_pin codec_reset_pin = som.GetPin(29);
    Ak4556::Init(codec_reset_pin);

    AudioHandle::Config cfg;
    cfg.blocksize  = 48;
    cfg.samplerate = SaiHandle::Config::SampleRate::SAI_48KHZ;
    cfg.postgain   = 0.5f;
    som.audio_handle.Init(
      cfg, 
      sai_handle[0]
      ,sai_handle[1]
    );

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
    encoder.Debounce(); 
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
  daisy::DacHandle::Config cvout1;
  daisy::DacHandle::Config cvout2;
  daisy::Encoder encoder;
  daisy::GateIn gatein1;
  daisy::GateIn gatein2;
  dsy_gpio gateout;
  daisy::OledDisplay<daisy::SSD130x4WireSpi128x64Driver> display;

};

} // namspace json2daisy

#endif // __JSON2DAISY_PATCH_H__