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
python3 -m pip install dist/json2daisy-0.2.0.tar.gz
~~~
If pip complains about a missing file, make sure you've entered the correct version number in the archive path.

## Usage

This package can be used as a standalone program on the command line or as an imported module.

On the command line:
~~~
python -m json2daisy path/to/board_description.json
~~~

As a module:
~~~python
import json2daisy

# If you want to generate from an Electrosmith board:
header_string, board_name, component_info, component_aliases = json2daisy.generate_header_from_board('field')
# If you're supplying custom JSON
header_string, board_name, component_info, component_aliases = json2daisy.generate_header_from_file('path/to/board_description.json')
~~~

As you can see, the two functions above will return a number of supplementary objects in addition to the header string. These provide detailed information for how to interact with the generated header. The `component_info` dictionary provides getters and setters for I/O, and the `component_aliases` dictionary provides valid aliases for everything in `component_info`. 

For examples of how to structure the board description, take a look at the JSON files in [src/json2daisy/resources](https://github.com/electro-smith/json2daisy/tree/main/src/json2daisy/resources).
