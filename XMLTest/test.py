import xml.etree.ElementTree as et
import os
import sys

if __name__ == "__main__":
    filename = "Test.xml"
    if not os.path.isfile(filename):
        print("Error while reading aircrafts!")
        sys.exit(0)
    tree = et.ElementTree(file=filename)
    root = tree.getroot()
    for ele in root:
        print(ele.tag, ele.attrib, ele.text)
    