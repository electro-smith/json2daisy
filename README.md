# json2daisy

Utility for converting JSON board definitions into valid, [libDaisy](https://github.com/electro-smith/libDaisy) compatible C++ board support files for the [Daisy platform](https://www.electro-smith.com/daisy).

## Install

Note that this module is not intended as a standalone application at this time. You should not need to install it manually for most use cases.

You can install this package with pip:
```
python -m pip install json2daisy
```

If you'd like to build the package and install locally, you'll need the `build` package:
```
python -m pip install build
```
Once that's set up, navigate to the root directory of json2daisy and run:
```
python3 -m build
python3 -m pip install dist/json2daisy-0.2.0.tar.gz
```
If pip complains about a missing file, make sure you've entered the correct version number in the archive path.

## Usage

This package can be used as a standalone program on the command line or as an imported module.

On the command line:
```
python -m json2daisy path/to/board_description.json
```

As a module:
```python
import json2daisy

# If you want to generate from an Electrosmith board:
header_string, board_name, component_info, component_aliases = json2daisy.generate_header_from_board('field')
# If you're supplying custom JSON
header_string, board_name, component_info, component_aliases = json2daisy.generate_header_from_file('path/to/board_description.json')
```

As you can see, the two functions above will return a number of supplementary objects in addition to the header string. These provide detailed information for how to interact with the generated header. The `component_info` dictionary provides getters and setters for I/O, and the `component_aliases` dictionary provides valid aliases for everything in `component_info`. 

For examples of how to structure the board description, take a look at the JSON files in [src/json2daisy/resources](https://github.com/electro-smith/json2daisy/tree/main/src/json2daisy/resources).

## Custom JSON

JSON is a human-readable file format for data storage, and it's what plugdata uses to help translate your Pd patch into something that will run on a Daisy-based board. If you haven't worked with something like it before, it might look a little strange, but don't worry! We'll cover enough here to get you on your way to custom hardware in no time.

### JSON Basics

For most hardware descriptions, there are only three JSON concepts you need to know: numbers, strings, and objects.

#### Numbers and strings

Numbers are just numbers! They can be integers (1, 2, 3) or any real number (3.141), and don't need any special formatting or indication.

Text data is stored as "strings", which are strings of characters enclosed in double quote marks ("like this :O").

#### Objects

An object is a set of _name_, _value_ pairs, enclosed in curly brackets. Each name is a unique string, and each value can be a number, a string, or another object. The name and value are separated by a colon. For a simple object, you might just have one name with a simple value:

```json
{ "pin": 15 }
```

When things get a little more complicated, such as objects with mutliple pairs, the pairs are usually broken up into their own lines for readability:

```json
{
    "component": "Encoder",
    "pin": {
        "a": 12,
        "b": 11,
        "click": 0
    }
}
```

An object with multiple pairs must have commas between each.

To make editing JSON by hand easy, it's usually best to use a program that provides syntax highlighting (like the colors you see on this page). [VS Code](https://code.visualstudio.com/) is a great cross-platform tool for this. Of course, you can just use a normal text editor, but errors may be more difficult to spot, and some may even mangle the text in undesireable ways.

### Board description format

To describe a board, your JSON will need an object with a few critical pairs:

- name
  - This describes your project name and determines the names of automatically generated files
- som
  - This is the SOM (system-on-module) that your project uses. This will usually be the Daisy Seed, but can also be the Patch SM.
  - The exact strings are:
    - `"seed"`
    - `"patch_sm"`
- audio
    - This is an object describing your audio layout. For most projects, this will simply have a single pair describing the number of audio channels:
```json
{ "channels": 2 }
```
- components
  - This is the meat of your description. Here, you'll describe all the input and output components of your design such as knobs, CVs, LEDs, and so on. The name determines the receive or send name in your Pd patch, and must have an object with a `"component"` name. For example, a gate input could be described as:

```json
"components": {
    "myGateIn": {
        "component": "GateIn",
        "pin": 20
    }
}
```
  - More complicated objects may have more required fields.

So, a very simple but viable board description might look like:

```json
{
    "name": "MyProject",
    "som": "seed",
    "audio": { "channels": 2 },
    "components": {
        "myGateIn": {
            "component": "GateIn",
            "pin": 20
        }
    }
}

```

### Component reference

| Type | _variant | Behavior |
| --- | --- | --- |
| **Inputs** | --- | --- |
| AnalogControl | --- | Returns a floating point representation of the voltage at its input. The typical range is 0-5 V, which is represented as 0-1. |
| BipolarAnalogControl | --- | Similar to a regular analog control, but can handle negative voltages. |
| Switch | --- | Returns a bang on the signal's rising edge (i.e. when the switch is actuated). |
| Switch | _press | Returns a float representing the current state (1 = pressed, 0 = not pressed) |
| Switch | _fall | Returns a bang on the signal's falling edge (i.e. when the switch is released). |
| Switch | _seconds | Returns a float representing the number of seconds the switch has been held down. |
| Switch3 | --- | Returns a float representing the current state, either 0 or 1. |
| Encoder | --- | Returns a 1 if turned one direction, -1 if turned in the other, and 0 otherwise. |
| Encoder | \_rise | Returns a bang when the encoder is pressed. The special alias _EncSwitch_ is always bound to this. |
| Encoder | _press | Same as switch _press. |
| Encoder | _fall | Same as switch _fall. |
| Encoder | _seconds | Same as switch _seconds. |
| GateIn | --- | Returns a float representing the current gate voltage, where a _high_ voltage is 1 and a _low_ voltage is 0. |
| GateIn | _trig | Returns a bang on the rising edge of the gate signal. |
| **Outputs** | --- | --- |
| CVOuts | --- | Expects a floating point value from 0-1, usually converted to 0-5V. |
| GateOut | --- | Expects a floating point value from 0-1. 0 sets the output low, and 1 sets it high. |
| Led | --- | Expects a floating point value from 0-1. The brightness is PWM modulated to match the input. |
| RgbLed | --- | Expects a floating point value from 0-1. The default behavior sets all three colors to the same brightness. |
| RgbLed | _white | Same as default. |
| RgbLed | _red | Expects a floating point value from 0-1. Sets the brightness of the red LED only. |
| RgbLed | _green | Expects a floating point value from 0-1. Sets the brightness of the green LED only. |
| RgbLed | _blue | Expects a floating point value from 0-1. Sets the brightness of the blue LED only. |

### Built-in descriptions

If some details are still unclear, the [JSON descriptions for the built-in boards](https://github.com/electro-smith/json2daisy/tree/main/src/json2daisy/resources) might help.
