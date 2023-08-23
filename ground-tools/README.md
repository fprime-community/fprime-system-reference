## Setup
1. From the project root, navigate to the ground tools directory: `cd ground-tools`
2. In a python virtual environment, install the ground tool dependencies by doing: `pip install -r requirements.txt`


## Usage
`process_raw_image.py`: Script that takes in a .dat file consisting of raw images data and processes it as either JPEG or PNG. Run the script with the `--help` option for more details.

Example: `python process_raw_image.py -in capture_1.dat -f PNG -r 800x600`