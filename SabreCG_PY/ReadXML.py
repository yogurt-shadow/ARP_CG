import sys
import os
import time
from typing import List
from lxml import etree
import xml.etree.ElementTree as ET
from Type import aircraftType, airportClosureType, flightType, mtcType, paraSet

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

def importAircrafts(fullFileName: str) -> (bool, List[aircraftType]):
    if not os.path.isfile(fullFileName):
        print("Error while reading aircrafts!")
        return False, []
    input_aircrafts = []
    nmap = get_xml_namespaces(fullFileName)
    reverse_map = {v: k for k, v in nmap.items()}
    tree = ET.ElementTree(file=fullFileName)
    root = tree.getroot()
    eleChildren = firstChildElements(root, "ns1:aircraft", reverse_map)
    for eleChild1 in eleChildren:
        acObj = aircraftType()
        acObj.tailNumber = firstChildElement(eleChild1, "ns1:tailNumber", reverse_map).text
        st = firstChildElement(eleChild1, "ns1:startAvailableTime", reverse_map)
        et = firstChildElement(eleChild1, "ns1:endAvailableTime", reverse_map)
        acObj.startAvailableTime = int(st.text)
        acObj.endAvailableTime = int(et.text)
        acObj.startAvailableAirport = firstChildElement(eleChild1, "ns1:startAvailableAirport", reverse_map).text
        acObj.endAvailableAirport = firstChildElement(eleChild1, "ns1:endAvailableAirport", reverse_map).text
        input_aircrafts.append(acObj)
    return True, input_aircrafts

def importAirportClosures(fullFileName: str) -> (bool, List[airportClosureType]):
    if not os.path.isfile(fullFileName):
        print("Error while reading airport closures!")
        return False, []
    input_airportClosures = []
    nmap = get_xml_namespaces(fullFileName)
    reverse_map = {v: k for k, v in nmap.items()}
    tree = ET.ElementTree(file=fullFileName)
    root = tree.getroot()
    eleChildren = firstChildElements(root, "ns2:flow", reverse_map)
    for eleChild1 in eleChildren:
        apObj = airportClosureType()
        apObj.code = firstChildElement(eleChild1, "ns2:code", reverse_map).text
        st = firstChildElement(eleChild1, "ns2:startTime", reverse_map)
        et = firstChildElement(eleChild1, "ns2:endTime", reverse_map)
        apObj.startTime = int(st.text)
        apObj.endTime = int(et.text)
        input_airportClosures.append(apObj)
    return True, input_airportClosures

def importSchedules(fullFileName: str) -> (bool, List[flightType], List[mtcType]):
    if not os.path.isfile(fullFileName):
        print("Error while reading schedules!")
        return False, [], []
    input_flights, input_mtc = [], []
    nmap = get_xml_namespaces(fullFileName)
    reverse_map = {v: k for k, v in nmap.items()}
    tree = ET.ElementTree(file=fullFileName)
    root = tree.getroot()
    xml_fltList = firstChildElement(root, "ns3:flightInfoList", reverse_map)
    xml_fltFound = firstChildElements(xml_fltList, "ns3:flightInfo", reverse_map)
    for xml_flt in xml_fltFound:
        flightObj = flightType()
        flightObj.id = firstChildElement(xml_flt, "ns3:id", reverse_map).text
        st = firstChildElement(xml_flt, "ns3:departureTime", reverse_map)
        et = firstChildElement(xml_flt, "ns3:arrivalTime", reverse_map)
        flightObj.departureTime = int(st.text)
        flightObj.arrivalTime = int(et.text)
        flightObj.departureAirport = firstChildElement(xml_flt, "ns3:departureAirport", reverse_map).text
        flightObj.arrivalAirport = firstChildElement(xml_flt, "ns3:arrivalAirport", reverse_map).text
        flightObj.tailNumber = firstChildElement(xml_flt, "ns3:tailNumber", reverse_map).text 
        input_flights.append(flightObj)
    xml_mtcList = firstChildElement(root, "ns3:mtcInfoList", reverse_map)
    xml_mtcFound = firstChildElements(xml_mtcList, "ns3:mtcInfo", reverse_map)
    if len(xml_mtcFound) > 0:
        for xml_mtc in xml_mtcFound:
            mtcObj = mtcType()
            mtcObj.id = firstChildElement(xml_mtc, "ns3:id", reverse_map).text
            st = firstChildElement(xml_mtc, "ns3:startTime", reverse_map)
            et = firstChildElement(xml_mtc, "ns3:endTime", reverse_map)
            mtcObj.startTime = int(st.text)
            mtcObj.endTime = int(et.text)
            mtcObj.airport = firstChildElement(xml_mtc, "ns3:airport", reverse_map).text
            mtcObj.tailNumber = firstChildElement(xml_mtc, "ns3:tailNumber", reverse_map).text
            input_mtc.append(mtcObj)
    return True, input_flights, input_mtc

def importParameters(fullFileName: str) -> (bool, paraSet):
    if not os.path.isfile(fullFileName):
        print("Error while reading parameters!")
        return False, None
    input_para = paraSet()
    nmap = get_xml_namespaces(fullFileName)
    reverse_map = {v: k for k, v in nmap.items()}
    tree = ET.ElementTree(file=fullFileName)
    root = tree.getroot()
    turnTime = firstChildElement(root, "turnTime", reverse_map)
    maxDelayTime = firstChildElement(root, "maxDelayTime", reverse_map)
    w_cancelMtc = firstChildElement(root, "weightCancelMaintenance", reverse_map)
    w_cancelFlt = firstChildElement(root, "weightCancelFlight", reverse_map)
    w_violatedBalance = firstChildElement(root, "weightViolateBalance", reverse_map)
    w_violatedPosition = firstChildElement(root, "weightViolatePositioning", reverse_map)
    w_fltDelay = firstChildElement(root, "weightFlightDelay", reverse_map)
    w_fltSwap = firstChildElement(root, "weightFlightSwap", reverse_map)
    maxRunTime = firstChildElement(root, "maxRunTime", reverse_map)

    input_para.turnTime = int(turnTime.text)
    input_para.maxDelayTime = int(maxDelayTime.text)
    input_para.w_cancelMtc = int(w_cancelMtc.text)
    input_para.w_cancelFlt = int(w_cancelFlt.text)
    input_para.w_violatedBalance = int(w_violatedBalance.text)
    input_para.w_violatedPosition = int(w_violatedPosition.text)
    input_para.w_fltDelay = int(w_fltDelay.text)
    input_para.w_fltSwap = int(w_fltSwap.text)
    input_para.maxRunTime = int(maxRunTime.text)
    return True, input_para

