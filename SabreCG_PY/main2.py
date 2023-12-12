# from Station import Station
# from Aircraft import Aircraft
# from Leg import Leg
# from Lof import Lof
# from OperLeg import OperLeg
# from Schedule import Schedule

from Structures import Station, Aircraft, Leg, Lof, OperLeg, Schedule
import Util as ut
from Model import Model
from ReadXML import importAircrafts, importAirportClosures, importSchedules, importParameters
from typing import List
from Type import flightType, aircraftType, mtcType, airportClosureType, paraSet
import lxml.etree as et
import sys
import os
import time

def processArguments(argc, argv):
    isCheck = False
    input_dir, output_dir = "", ""
    for i in range(1, argc):
        if i == 1:
            input_dir += (argv[i] + "\\")
        elif i == 2:
            output_dir += (argv[i] + "\\")
            isCheck = True
    print("input_dir is %s" % input_dir)
    print("output_dir is %s" % output_dir)
    return isCheck, input_dir, output_dir

def readConfigurationList() -> (str, str):
    configFile = "./Config.txt"
    input_dir, output_dir = "", ""
    if not os.path.exists(configFile):
        return "", ""
    with open(configFile, 'r') as f:
        line = f.readlines()
    for i in range(len(line)):
        _line = line[i].strip()
        ele = _line.split()
        if len(ele) > 0 and ele[0] == "END":
            break
        elif len(ele) > 0 and ele[0] == "INPUT_DIRECTORY":
            input_dir += ele[1] + "\\"
            continue
        elif len(ele) > 0 and ele[0] == "OUTPUT_DIRECTORY":
            output_dir += ele[1] + "\\"
            continue
        if i + 1 == 2:
            break
    return input_dir, output_dir

def updaInfo(_LofListSoln: list[Lof], _InitLegList: list[Leg]) -> list[Leg]:
    print("test for upda")
    legList = []
    # dealing with assigned flights
    for _lof in _LofListSoln:
        _OperLegList = _lof.getLegList()
        for _operLeg in _OperLegList:
            _operLeg.print()
            depStation = _operLeg.getLeg().getDepStation()
            arrStation = _operLeg.getLeg().getArrStation()
            depTime = _operLeg.getPrintDepTime()
            arrTime = _operLeg.getPrintArrTime()
            flightNum = _operLeg.getLeg().getFlightNum()
            aircraft = _lof.getAircraft()
            leg = Leg(flightNum, depStation, arrStation, depTime, arrTime, aircraft)
            leg.setAssigned(True)
            leg.setIsMaint(_operLeg.getLeg().isMaint())
            legList.append(leg)
    # dealing with cancelled flights
    for _initleg in _InitLegList:
        found  = False
        for _leg in legList:
            if _leg.getFlightNum() == _initleg.getFlightNum():
                found = True
                break
        if not found:
            _initleg.setAssigned(False)
            legList.append(_initleg)
    return legList

def exportSolution(output_path: str, _LegList: list[Leg], nmap: dict[str, str]) -> bool:
    try:
        if not os.path.exists(output_path):
            os.makedirs(output_path)
        root = et.Element("exportAircrafts", nsmap=nmap)
        # Flight Info
        xml_fltList = et.SubElement(root, "{%s}flightInfoList" % nmap["ns3"])
        for _leg in _LegList:
            if not _leg.isMaint():
                xml_fltType = et.SubElement(xml_fltList, "{%s}flightInfo" % nmap["ns3"])
                xml_flt_id = et.SubElement(xml_fltType, "{%s}id" % nmap["ns3"])
                xml_value = _leg.getFlightNum()
                xml_flt_id.text = str(xml_value)

                xml_flt_depTime = et.SubElement(xml_fltType, "{%s}departureTime" % nmap["ns3"])
                xml_value = str(_leg.getDepTime())
                xml_flt_depTime.text = xml_value

                xml_flt_arrTime = et.SubElement(xml_fltType, "{%s}arrivalTime" % nmap["ns3"])
                xml_value = str(_leg.getArrTime())
                xml_flt_arrTime.text = xml_value

                xml_flt_depArp = et.SubElement(xml_fltType, "{%s}departureAirport" % nmap["ns3"])
                xml_value = _leg.getDepStation().getName()
                xml_flt_depArp.text = xml_value

                xml_flt_arrArp = et.SubElement(xml_fltType, "{%s}arrivalAirport" % nmap["ns3"])
                xml_value = _leg.getArrStation().getName()
                xml_flt_arrArp.text = xml_value

                xml_flt_tailNum = et.SubElement(xml_fltType, "{%s}tailNumber" % nmap["ns3"])
                xml_value = _leg.getAircraft().getTail()
                xml_flt_tailNum.text = xml_value

                xml_flt_status = et.SubElement(xml_fltType, "{%s}status" % nmap["ns3"])
                if _leg.getAssigned():
                    xml_value = "Assigned"
                else:
                    xml_value = "Cancelled"
                xml_flt_status.text = xml_value
        
        # Maintenance Info
        xml_mtcList = et.SubElement(root, "{%s}mtcInfoList" % nmap["ns3"])
        for _leg in _LegList:
            if _leg.isMaint():
                xml_mtcType = et.SubElement(xml_mtcList, "{%s}mtcInfo" % nmap["ns3"])

                xml_mtc_id = et.SubElement(xml_mtcType, "{%s}id" % nmap["ns3"])
                xml_value = _leg.getFlightNum()
                xml_mtc_id.text = str(xml_value)

                xml_mtc_startTime = et.SubElement(xml_mtcType, "{%s}startTime"  % nmap["ns3"])
                xml_value = str(_leg.getDepTime())
                xml_mtc_startTime.text = xml_value

                xml_mtc_endTime = et.SubElement(xml_mtcType, "{%s}endTime" % nmap["ns3"])
                xml_value = str(_leg.getArrTime())
                xml_mtc_endTime.text = xml_value

                xml_mtc_airport = et.SubElement(xml_mtcType, "{%s}airport" % nmap["ns3"])
                xml_value = _leg.getDepStation().getName()
                xml_mtc_airport.text = xml_value

                xml_mtc_tailNum = et.SubElement(xml_mtcType, "{%s}tailNumber" % nmap["ns3"])
                xml_value = _leg.getAircraft().getTail()
                xml_mtc_tailNum.text = xml_value

                xml_mtc_status = et.SubElement(xml_mtcType, "{%s}status" % nmap["ns3"])
                if _leg.getAssigned():
                    xml_value = "Assigned"
                else:
                    xml_value = "Cancelled"
                xml_mtc_status.text = xml_value
        # lxml cannot generate tags with ':'
        tree = et.ElementTree(root)
        if os.path.exists(output_path + "Output.xml"):
            os.remove(output_path + "Output.xml")
            print("remove previous output file")
        if not tree.write(output_path + "Output.xml", encoding='utf-8', pretty_print=True):
            return False
    except Exception as e:
        print(e)
        return False
    return True

if __name__ == "__main__":
    print("program start...")
    succeed, input_path, output_path = processArguments(len(sys.argv), sys.argv)
    if not succeed:
        print("read config")
        input_path, output_path = readConfigurationList()
    else:
        print("skip config")
    if len(input_path) == 0 and len(output_path) == 0:
        print("Either input directory or output directory is not specified.")
        sys.exit()
    aircraft = list[aircraftType]
    succeed, aircrafts, nsmap1 = importAircrafts(input_path + "Aircraft.xml")
    if not succeed:
        sys.exit()
    airportClosures = list[airportClosureType]
    succeed, airportClosures, nsmap2 = importAirportClosures(input_path + "AirportClosure.xml")
    if not succeed:
        sys.exit()
    flights = list[flightType]
    succeed, flights, mtcs, nsmap3 = importSchedules(input_path + "Schedule.xml")
    if not succeed:
        sys.exit()
    parameters = paraSet()
    succeed, parameters, nsmap4 = importParameters(input_path + "Parameters.xml")
    if not succeed:
        sys.exit()
    # Generate namespace dictionary according to input XML files
    nmap = nsmap1.copy()
    nmap.update(nsmap2)
    nmap.update(nsmap3)
    nmap.update(nsmap4)
    # Initialize the parameters
    ut.util.maxDelayTime = parameters.maxDelayTime
    ut.util.maxRunTime = parameters.maxRunTime
    ut.util.turnTime = parameters.turnTime
    ut.util.w_cancelFlt = parameters.w_cancelFlt
    ut.util.w_cancelMtc = parameters.w_cancelMtc
    ut.util.w_fltDelay = parameters.w_fltDelay
    ut.util.w_fltSwap = parameters.w_fltSwap
    ut.util.w_violatedBalance = parameters.w_violatedBalance
    ut.util.w_violatedPosition = parameters.w_violatedPosition
    ut.util.print()
    stationList, aircraftList = [], []
    # Initialize the aircraft information and station Information
    for _aircraft in aircrafts:
        tail = _aircraft.tailNumber
        startTime, endTime = _aircraft.startAvailableTime, _aircraft.endAvailableTime
        depName, arrName = _aircraft.startAvailableAirport, _aircraft.endAvailableAirport
        depStation, arrStation = None, None
        for _station in stationList:
            if _station.getName() == depName:
                depStation = _station
                break
        if depStation == None:
            depStation = Station(depName)
            stationList.append(depStation)
        for _station in stationList:
            if _station.getName() == arrName:
                arrStation = _station
                break
        if arrStation == None:
            arrStation = Station(arrName)
            stationList.append(arrStation)
        aircraftList.append(Aircraft(tail, startTime, endTime, depStation, arrStation))

    # Initialize the legList, flights+maintenances
    legList = []
    for _flight in flights:
        depName, arrName = _flight.departureAirport, _flight.arrivalAirport
        depStation, arrStation = None, None
        depTime, arrTime = _flight.departureTime, _flight.arrivalTime
        flightNum = _flight.id
        tail = _flight.tailNumber
        aircraft = None
        for _station in stationList:
            if _station.getName() == depName:
                depStation = _station
                break
        if depStation == None:
            depStation = Station(depName)
            stationList.append(depStation)
        for _station in stationList:
            if _station.getName() == arrName:
                arrStation = _station
                break
        if arrStation == None:
            arrStation = Station(arrName)
            stationList.append(arrStation)
        for _aircraft in aircraftList:
            if _aircraft.getTail() == tail:
                aircraft = _aircraft
                break
        if aircraft == None:
            print("Cannot Find Aircraft " + tail)
            sys.exit(0)
        leg = Leg(flightNum, depStation, arrStation, depTime, arrTime, aircraft)
        legList.append(leg)
    
    for _aircraft in aircraftList:
        depStation = _aircraft.getDepStation()
        depStation.pushDepAircraft(_aircraft)
    print("********************* Number of Aircraft Departured *********************")
    for _station in stationList:
        print("number of aircraft departured from station " + _station.getName() \
              + " is " + str(_station.getDepAircraftNumber()))
    print("********************* END PRINTING *********************")
    for _station in stationList:
        _station.setLegNum(len(legList))
    
    # Initialize maintList,only includes maintenance
    maintList = []
    for _mtcs in mtcs:
        stationName = _mtcs.airport
        startTime, endTime = _mtcs.startTime, _mtcs.endTime
        maintId, tail = _mtcs.id, _mtcs.tailNumber
        station = None
        for _station in stationList:
            if _station.getName() == stationName:
                station = _station
                break
        if station == None:
            station = Station(stationName)
            stationList.append(station)
        aircraft = None
        for _aircraft in aircraftList:
            if _aircraft.getTail() == tail:
                aircraft = _aircraft
                # should we break ?
        if aircraft == None:
            print("Cannot Find Aircraft " + tail)
            sys.exit(0)
        maint = Leg(maintId, station, station, startTime, endTime, aircraft)
        maintList.append(maint)
        legList.append(maint)

    # set the closure time of station
    for _airportClosure in airportClosures:
        stationName = _airportClosure.code
        startT, endT = _airportClosure.startTime, _airportClosure.endTime
        station = None
        for _station in stationList:
            if _station.getName() == stationName:
                station = _station
                break
        if station == None:
            print("Error, cannot Find Station " + stationName)
            sys.exit(0)
        closeTime = (startT, endT)
        station.pushCloseTime(closeTime)
    
    print("Ready to go!")

    startTime = time.time()
    schedule = Schedule(stationList, aircraftList, legList)
    schedule.computeTopOrder()


    # for i in aircraftList[5].getArrStation().getArrLegList():
    #     print(len(i.getSubNodeList()))

    model = Model(stationList, aircraftList, legList, schedule.getTopOrderList())
    lofListSoln = model.solveColGen()
    # finaLegList = updaInfo(lofListSoln, legList)
    # print("Total number of connection of leg network: " + str(schedule.getConnectionSize()))
    # endTime = time.time()
    # print("total run time is " + str(endTime - startTime) + " seconds")

    # if exportSolution(output_path, finaLegList, nmap):
    #     print("Solution is printed.")
    # print()
    # print("Canceled flights are")
    # for _fina in finaLegList:
    #     if not _fina.getAssigned():
    #         _fina.print()
    #         print("dual is " + str(_fina.getDual()))
    # print("program finish")
        