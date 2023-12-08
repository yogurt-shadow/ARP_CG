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
import xml.etree.ElementTree as et
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
    print("input dir is %s", input_dir)
    print("output dir is %s", output_dir)
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

def updaInfo(_LofListSoln: List[Lof], _InitLegList: List[Leg]) -> List[Leg]:
    legList = []
    # dealing with assigned flights
    for _lof in _LofListSoln:
        _OperLegList = _lof.getLegList()
        for _operLeg in _OperLegList:
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

def exportSolution(output_path: str, _LegList: List[Leg]) -> bool:
    try:
        if not os.path.exists(output_path):
            os.makedirs(output_path)
        root = et.Element("exportAircrafts")
        root.text = ""
        tree = et.ElementTree(root)

        # Flight Info
        xml_fltList = et.Element("ns3:flightInfoList")
        xml_fltList.text = ""
        for _leg in _LegList:
            if _leg.isMaint():
                xml_fltType = et.Element("ns3:flightInfo")
                xml_flt_id = et.Element("ns3:id")
                xml_value = _leg.getFlightNum()
                xml_flt_id.text = str(xml_value)
                xml_fltType.append(xml_flt_id)

                xml_flt_depTime = et.Element("ns3:departureTime")
                xml_value = str(_leg.getDepTime())
                xml_flt_depTime.text = xml_value
                xml_fltType.append(xml_flt_depTime)

                xml_flt_arrTime = et.Element("ns3:arrivalTime")
                xml_value = str(_leg.getArrTime())
                xml_flt_arrTime.text = xml_value
                xml_fltType.append(xml_flt_arrTime)

                xml_flt_depArp = et.Element("ns3:departureAirport")
                xml_value = _leg.getDepStation().getName()
                xml_flt_depArp.text = xml_value
                xml_fltType.append(xml_flt_depArp)

                xml_flt_arrArp = et.Element("ns3:arrivalAirport")
                xml_value = _leg.getArrStation().getName()
                xml_flt_arrArp.text = xml_value
                xml_fltType.append(xml_flt_arrArp)

                xml_flt_tailNum = et.Element("ns3:tailNumber")
                xml_value = _leg.getAircraft().getTail()
                xml_flt_tailNum.text = xml_value
                xml_fltType.append(xml_flt_tailNum)

                xml_flt_status = et.Element("ns3:status")
                if _leg.getAssigned():
                    xml_value = "Assigned"
                else:
                    xml_value = "Cancelled"
                xml_flt_status.text = xml_value
                xml_fltType.append(xml_flt_status)
                xml_fltList.append(xml_fltType)
        
        # Maintenance Info
        xml_mtcList = et.Element("ns3:mtcInfoList")
        for _leg in _LegList:
            if _leg.isMaint():
                xml_mtcType = et.Element("ns3:mtcInfo")

                xml_mtc_id = et.Element("ns3:id")
                xml_value = _leg.getFlightNum()
                xml_mtc_id.text = str(xml_value)
                xml_mtcType.append(xml_mtc_id)

                xml_mtc_startTime = et.Element("ns3:startTime")
                xml_value = str(_leg.getDepTime())
                xml_mtc_startTime.text = xml_value
                xml_mtcType.append(xml_mtc_startTime)

                xml_mtc_endTime = et.Element("ns3:endTime")
                xml_value = str(_leg.getArrTime())
                xml_mtc_endTime.text = xml_value
                xml_mtcType.append(xml_mtc_endTime)

                xml_mtc_airport = et.Element("ns3:airport")
                xml_value = _leg.getDepStation().getName()
                xml_mtc_airport.text = xml_value
                xml_mtcType.append(xml_mtc_airport)

                xml_mtc_tailNum = et.Element("ns3:tailNumber")
                xml_value = _leg.getAircraft().getTail()
                xml_mtc_tailNum.text = xml_value
                xml_mtcType.append(xml_mtc_tailNum)

                xml_mtc_status = et.Element("ns3:status")
                if _leg.getAssigned():
                    xml_value = "Assigned"
                else:
                    xml_value = "Cancelled"
                xml_mtc_status.text = xml_value
                xml_mtcType.append(xml_mtc_status)
                xml_mtcList.append(xml_mtcType)
        root.append(xml_fltList)
        root.append(xml_mtcList)
        if not tree.write(output_path + "Output.xml"):
            return False
    except Exception as e:
        print(e)
        return False
    return True

if __name__ == "__main__":
    print("program start...")
    succeed, input_path, output_path = processArguments(len(sys.argv), sys.argv)
    if not succeed:
        input_path, output_path = readConfigurationList()
    if len(input_path) == 0 and len(output_path) == 0:
        print("Either input directory or output directory is not specified.")
        sys.exit()
    succeed, aircrafts = importAircrafts(input_path + "Aircraft.xml")
    if not succeed:
        sys.exit()
    succeed, airportClosures = importAirportClosures(input_path + "AirportClosure.xml")
    if not succeed:
        sys.exit()
    succeed, flights, mtcs = importSchedules(input_path + "Schedule.xml")
    if not succeed:
        sys.exit()
    succeed, parameters = importParameters(input_path + "Parameters.xml")
    if not succeed:
        sys.exit()
    ut.util.maxDelayTime = parameters.maxDelayTime
    ut.util.maxRunTime = parameters.maxRunTime
    ut.util.maxDelayTime = parameters.maxDelayTime
    ut.util.maxRunTime = parameters.maxRunTime
    ut.util.turnTime = parameters.turnTime
    ut.util.w_cancelFlt = parameters.w_cancelFlt
    ut.util.w_cancelMtc = parameters.w_cancelMtc
    ut.util.w_fltDelay = parameters.w_fltDelay
    ut.util.w_fltSwap = parameters.w_fltSwap
    ut.util.w_violatedBalance = parameters.w_violatedBalance
    ut.util.w_violatedPosition = parameters.w_violatedPosition
    
    stationList, aircraftList = [], []
    for _aircraft in aircrafts:
        tail = _aircraft.tailNumber
        startTime, endTime = _aircraft.startAvailableTime, _aircraft.endAvailableTime
        depName, arrName = _aircraft.startAvailableAirport, _aircraft.endAvailableAirport
        depStationList = [_station for _station in stationList if _station.getName() == depName]
        arrStationList = [_station for _station in stationList if _station.getName() == arrName]
        depStation, arrStation = None, None
        if len(depStationList) == 0:
            depStation = Station(depName)
            stationList.append(depStation)
        else:
            depStation = depStationList[0]
        if len(arrStationList) == 0:
            arrStation = Station(arrName)
            stationList.append(arrStation)
        else:
            arrStation = arrStationList[0]
        aircraftList.append(Aircraft(tail, startTime, endTime, depStation, arrStation))
    
    legList = []
    for _flight in flights:
        depName, arrName = _flight.departureAirport, _flight.arrivalAirport
        depStation, arrStation = None, None
        depTime, arrTime = _flight.departureTime, _flight.arrivalTime
        flightNum = _flight.id
        tail = _flight.tailNumber
        depStationList = [_station for _station in stationList if _station.getName() == depName]
        arrStationList = [_station for _station in stationList if _station.getName() == arrName]
        aircraftFoundList = [_aircraft for _aircraft in aircraftList if _aircraft.getTail() == tail]
        depStation, arrStation, aircraft = None, None, None
        if len(depStationList) == 0:
            depStation = Station(depName)
            stationList.append(depStation)
        else:
            depStation = depStationList[0]
        if len(arrStationList) == 0:
            arrStation = Station(arrName)
            stationList.append(arrStation)
        else:
            arrStation = arrStationList[0]
        if len(aircraftFoundList) == 0:
            print("Cannot Find Aircraft " + tail)
            sys.exit(0)
        else:
            aircraft = aircraftFoundList[0]
        legList.append(Leg(flightNum, depStation, arrStation, depTime, arrTime, aircraft))
    
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

    maintList = []
    for _mtcs in mtcs:
        stationName = _mtcs.airport
        startTime, endTime = _mtcs.startTime, _mtcs.endTime
        maintId, tail = _mtcs.id, _mtcs.tailNumber
        station, aircraft = None, None
        stationFoundList = [_station for _station in stationList if _station.getName() == stationName]
        if len(stationFoundList) == 0:
            station = Station(stationName)
            stationList.append(station)
        else:
            station = stationFoundList[0]
        aircraftFoundList = [_aircraft for _aircraft in aircraftList if _aircraft.getTail() == tail]
        if len(aircraftFoundList) == 0:
            print("Cannot Find Aircraft " + tail)
            sys.exit(0)
        else:
            aircraft = aircraftFoundList[0]
        maint = Leg(maintId, station, station, startTime, endTime, aircraft)
        maintList.append(maint)
        legList.append(maint)

    for _airportClosure in airportClosures:
        stationName = _airportClosure.code
        startT, endT = _airportClosure.startTime, _airportClosure.endTime
        station = None
        stationFoundList = [_station for _station in stationList if _station.getName() == stationName]
        if len(stationFoundList) == 0:
            print("Error, Cannot Find Station " + stationName)
            sys.exit(0)
        else:
            station = stationFoundList[0]
        closeTime = (startT, endT)
        station.pushCloseTime(closeTime)
    
    print("Ready to go!")
    startTime = time.time()
    schedule = Schedule(stationList, aircraftList, legList)
    schedule.computeTopOrder()
    model = Model(stationList, aircraftList, legList, schedule.getTopOrderList())
    lofListSoln = model.solveColGen()
    finaLegList = updaInfo(lofListSoln, legList)
    print("Total number of connection of leg network: " + schedule.getConnectionSize())
    endTime = time.time()
    print("total run time is " + str(endTime - startTime) + " seconds")

    if exportSolution(output_path, finaLegList):
        print("Solution is printed.")
    print()
    print("Canceled flights are")
    for _fina in finaLegList:
        if not _fina.getAssigned():
            _fina.print()
            print("dual is " + str(_fina.getDual()))
    print("program finish")
        