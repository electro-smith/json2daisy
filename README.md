# json2daisy

Utility for converting JSON board definitions into valid, [libDaisy](https://github.com/electro-smith/libDaisy) compatible C++ board support files for the [Daisy platform](https://www.electro-smith.com/daisy).

## Install

You can install this package with pip:
~~~
python -m pip install json2daisy
~~~

If you'd like to build the package and install locally, you'll need the `build` package:
~~~
python -m pip install build
~~~
Once that's set up, navigate to the root directory of json2daisy and run:
~~~
python3 -m build
python3 -m pip install dist/json2daisy-0.1.0.tar.gz
~~~
If pip complains about a missing file, make sure you've entered the correct version number in the archive path.

## Usage

This package is currently integrated tightly with Wasted Audio's [fork of hvcc](https://github.com/Wasted-Audio/hvcc), and is not intended to be used in a standalone configuration at this time. It handles Pure Data's I/O configuration and its runtime upkeep, allowing users to select among Electrosmith's Daisy boards (or custom Daisy-based boards) for embedded PD applications with ease.

To choose the Daisy target and select among Electrosmith's pre-existing boards, you'll need to supply the board name using hvcc's `meta` (`-m`) option. 
~~~bash
-m path/to/meta.json
~~~
Inside `meta.json`, you'll need a simple daisy field specifying the board:
~~~json
{
  "daisy": {
    "board": "patch"
  }
}
~~~

To supply a custom board file, omit the "board" field and include the following in your json:

~~~json
{
  "daisy": {
    "description": "path/to/description.json"
  }
}
~~~

For examples of how to structure the board description, take a look at the JSON files in [src/json2daisy/resources](https://github.com/electro-smith/json2daisy/tree/main/src/json2daisy/resources).

## pd2dsy
This above details and more is neatly handled by the [pd2dsy](https://github.com/electro-smith/pd2dsy) tool, which takes you from a bog-standard .pd file all the way to compilation and MCU flashing with a single command. This is the recommended approach for those working with Pure Data on a Daisy board.