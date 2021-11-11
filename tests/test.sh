#!/bin/bash

VERSION=0.1.0

python3 -m venv json2daisy_env
source json2daisy_env/bin/activate
python3 -m pip install build
cd ..; python3 -m build; python3 -m pip install dist/json2daisy-$VERSION.tar.gz
cd tests
python3 -m unittest discover -p "*_test.py"
deactivate
rm -rf json2daisy_env