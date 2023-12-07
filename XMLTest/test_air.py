import os
import sys
from lxml import etree
import xml.etree.ElementTree as ET

def get_xml_namespaces(inputPath: str) -> dict[str, str]:
    """Get the namespaces of an XML file.

    Args:
        inputPath (str): Path to the XML file.

    Returns:
        dict[str, str]: Dictionary of namespaces.
    """
    xml_str = ""
    with open(inputPath, 'r') as f:
        xml_str = f.read().encode('utf-8')
    xml = etree.fromstring(xml_str)
    return xml.nsmap

def namespaces2original(tag: str, reverse_map: dict[str, str]) -> str:
    """Convert a tag with namespaces to the original tag.

    Args:
        tag (str): Tag with namespaces.
        reverse_map (dict[str, str]): Dictionary of namespaces.

    Returns:
        str: Original tag.
    """
    for v, k in reverse_map.items():
        tag = tag.replace("{" + v + "}", k + ":")
    return tag

def firstChildElement(root: ET.Element, tag: str, reverse_map: dict[str, str]) -> ET.Element:
    """Get the first child element of a root element with original tag.

    Args:
        root (ET.Element): Root element.

    Returns:
        ET.Element: first child element.
    """
    for child in root:
        if namespaces2original(child.tag, reverse_map) == tag:
            return child
    return None

def firstChildElements(root: ET.Element, tag: str, reverse_map: dict[str, str]) -> list[ET.Element]:
    """Get the first child elements of a root element with original tag.

    Args:
        root (ET.Element): Root element.

    Returns:
        list[ET.Element]: first child elements.
    """
    children = []
    for child in root:
        if namespaces2original(child.tag, reverse_map) == tag:
            children.append(child)
    return children

if __name__ == "__main__":
    filename = "Aircraft.xml"
    if not os.path.isfile(filename):
        print("Error while reading aircrafts!")
        sys.exit(0)
    nmap = get_xml_namespaces(filename)
    reverse_map = {v: k for k, v in nmap.items()}
    tree = ET.ElementTree(file=filename)
    root = tree.getroot()
    eleChildren = firstChildElements(root, "ns1:aircraft", reverse_map)
    for eleChild1 in eleChildren:
        tailNumber = firstChildElement(eleChild1, "ns1:tailNumber", reverse_map).text
        print(tailNumber)
        st = firstChildElement(eleChild1, "ns1:startAvailableTime", reverse_map)
        et = firstChildElement(eleChild1, "ns1:endAvailableTime", reverse_map)
        startAvailableTime = int(st.text)
        endAvailableTime = int(et.text)
        print(startAvailableTime)
        print(endAvailableTime)

        startAvailableAirport = firstChildElement(eleChild1, "ns1:startAvailableAirport", reverse_map).text
        endAvailableAirport = firstChildElement(eleChild1, "ns1:endAvailableAirport", reverse_map).text
        print(startAvailableAirport)
        print(endAvailableAirport)

