#ifndef __JSON2DAISY_{{name|upper}}_H__
#define __JSON2DAISY_{{name|upper}}_H__

{% if som == 'seed' %}
#include "daisy_seed.h"
{% elif som == 'patch_sm' %}
#include "daisy_patch_sm.h"
{% endif %}

struct Daisy{{ name|capitalize }} {

  void Init(bool boost=true) {
    {% if som == 'seed' %}
    som.Configure();
    som.Init(boost);
    {% else %}
    som.Init();
    {% endif %}

    {{init}}
		{{i2c}}
		{{pca9685}}
		{{switch}}
		{{switch3}}
		{{cd4021}}
		{{gatein}}
		{{encoder}}
		{{init_single}}
		{% if som == 'seed' %}
		som.adc.Init(cfg, ANALOG_COUNT);
		{% endif %}
		{{ctrl_init}}
		{{ctrl_mux_init}}
		{{led}}
		{{rgbled}}
		{{gateout}}
		{{dachandle}}
		{{display}}

    {% if som == 'seed' and external_codecs|length > 0 %}

		SaiHandle::Config sai_config[{{ 1 + external_codecs|length }}];

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

		{% for codec in external_codecs %}
		sai_config[{{loop.index}}].periph          = SaiHandle::Config::Peripheral::{{codec.periph}};
    sai_config[{{loop.index}}].sr              = SaiHandle::Config::SampleRate::SAI_48KHZ;
    sai_config[{{loop.index}}].bit_depth       = SaiHandle::Config::BitDepth::SAI_24BIT;
    sai_config[{{loop.index}}].a_sync          = SaiHandle::Config::Sync::{{codec.a_sync}};
    sai_config[{{loop.index}}].b_sync          = SaiHandle::Config::Sync::{{codec.b_sync}};
    sai_config[{{loop.index}}].a_dir           = SaiHandle::Config::Direction::{{codec.a_dir}};
    sai_config[{{loop.index}}].b_dir           = SaiHandle::Config::Direction::{{codec.b_dir}};
    sai_config[{{loop.index}}].pin_config.fs   = som.GetPin({{codec.pin.fs}});
    sai_config[{{loop.index}}].pin_config.mclk = som.GetPin({{codec.pin.mclk}});
    sai_config[{{loop.index}}].pin_config.sck  = som.GetPin({{codec.pin.sck}});
    sai_config[{{loop.index}}].pin_config.sa   = som.GetPin({{codec.pin.sa}});
    sai_config[{{loop.index}}].pin_config.sb   = som.GetPin({{codec.pin.sb}});
		{% endfor %}

		SaiHandle sai_handle[{{ 1 + external_codecs|length }}];
		sai_handle[0].Init(sai_config[0]);
		{% for codec in external_codecs %}
		sai_handle[{{loop.index}}].Init(sai_config[{{loop.index}}]);
		{% endfor %}

		dsy_gpio_pin codec_reset_pin = som.GetPin(29);
    Ak4556::Init(codec_reset_pin);

		AudioHandle::Config cfg;
    cfg.blocksize  = 48;
    cfg.samplerate = SaiHandle::Config::SampleRate::SAI_48KHZ;
    cfg.postgain   = 0.5f;
    som.audio_handle.Init(
			cfg, 
			sai_handle[0]
			{% for codec in external_codecs %}
			,sai_handle[{{loop.index}}]
			{% endfor %}
		);

		{% endif %}

		{% if som == 'seed' %}
		som.adc.Start();
		{% endif %}
  }

  void ProcessAllControls() {
    {{process}}
		{% if som == 'patch_sm' %}
		driver.ProcessAllControls();
		{% endif %}
  }

  void SetAudioSampleRate() {

  }

  void SetAudioBlockSize() {

  }

  /** This is the board's "System On Module"
   */
  {{som_class}} som;

};

#endif // __JSON2DAISY_{{name|upper}}_H__