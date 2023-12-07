import os
import sys
from lxml import etree

if __name__ == "__main__":
    filename = "Aircraft.xml"
    if not os.path.isfile(filename):
        print("Error while reading aircrafts!")
        sys.exit(0)
    
