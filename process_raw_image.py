from PIL import Image
from io import BytesIO
import argparse
import sys
import os

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--in_file', required=True, help="dat file containing raw image data")
    parser.add_argument('-f', '--format', required=False, default='JPEG', choices=['JPEG', 'PNG'])
    parser.add_argument('-r', '--resolution', required=False, default="640x480", choices=["800x600", "640x480"])


    args = parser.parse_args()
    
    in_file = args.in_file
    file_format = args.format
    resolution = args.resolution

    if not os.path.isfile(in_file):
        print(f"File {in_file} does not exist, exiting.")
        sys.exit(0)

    if not in_file.endswith(".dat"):
        print("Incorrect input file format specified, exiting.")
        sys.exit(0)

    if '640x480' == resolution:
        resolution = (640, 480)
    else:
        resolution = (800, 600)

    out_file = in_file[:-4]
    if file_format == 'JPEG':
        out_file += '.jpg'
    else:
        out_file += '.png'

    f = open(in_file, "rb")
    data = f.read()
    f.close()

    processed_img = Image.frombytes(mode="RGB", size=resolution, data=data)
    processed_img.save(out_file)

if __name__ == "__main__":
    main()