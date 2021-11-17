#ifndef __JSON2DAISY_PATCH_SM_H__
#define __JSON2DAISY_PATCH_SM_H__

#include "daisy_patch_sm.h"

namespace json2daisy {

struct DaisyPatch_sm {

  /** Initializes the board according to the JSON board description
   *  \param boost boosts the clock speed from 400 to 480 MHz
   */
  void Init(bool boost=true) 
  {
    som.Init();

    // Display
    // no display 


  }

  /** Handles all the controls processing that needs to occur at the block rate
   * 
   */
  void ProcessAllControls() 
  {
    som.ProcessAllControls();
  }

  /** Sets the audio sample rate
   *  \param sample_rate the new sample rate in Hz
   */
  void SetAudioSampleRate(size_t sample_rate) 
  {
    som.SetAudioSampleRate(sample_rate);
    
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
  
  // no display

};

} // namspace json2daisy

#endif // __JSON2DAISY_PATCH_SM_H__