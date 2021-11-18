#ifndef __JSON2DAISY_PATCH_INIT_H__
#define __JSON2DAISY_PATCH_INIT_H__

#include "daisy_patch_sm.h"

namespace json2daisy {

struct DaisyPatch_init {

  /** Initializes the board according to the JSON board description
   *  \param boost boosts the clock speed from 400 to 480 MHz
   */
  void Init(bool boost=true) 
  {
    som.Init();

 
    // Switches
    sw1.Init(daisy::patch_sm::DaisyPatchSM::B7, som.AudioCallbackRate(), daisy::Switch::TYPE_MOMENTARY, daisy::Switch::POLARITY_INVERTED, daisy::Switch::PULL_UP);
    sw2.Init(daisy::patch_sm::DaisyPatchSM::B8, som.AudioCallbackRate(), daisy::Switch::TYPE_MOMENTARY, daisy::Switch::POLARITY_INVERTED, daisy::Switch::PULL_UP); 
    // Display
    // no display 


  }

  /** Handles all the controls processing that needs to occur at the block rate
   * 
   */
  void ProcessAllControls() 
  {
 
    sw1.Debounce();
    sw2.Debounce(); 
    som.ProcessAllControls();
  }

  /** Sets the audio sample rate
   *  \param sample_rate the new sample rate in Hz
   */
  void SetAudioSampleRate(size_t sample_rate) 
  {
    som.SetAudioSampleRate(sample_rate);
    sw1.SetUpdateRate(som.AudioCallbackRate());
    sw2.SetUpdateRate(som.AudioCallbackRate());
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
  daisy::patch_sm::DaisyPatchSM som;

  // I/O Components
  daisy::Switch sw1;
  daisy::Switch sw2;
  // no display

};

} // namspace json2daisy

#endif // __JSON2DAISY_PATCH_INIT_H__