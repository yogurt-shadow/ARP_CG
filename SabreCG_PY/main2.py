from Station import Station
from Aircraft import Aircraft
from Leg import Leg
from Schedule import Schedule
import util as ut
from typing import List
import xml.dom.minidom

import sys
import os
import time

class flightType:
    def __init__(self):
        self.id, self.tailNumber = "", ""
        self.arrivalAirport, self.departureAirport = "", ""
        self.arrivalTime, self.departureTime = 0, 0

class aircraftType:
    def __init__(self):
        self.tailNumber, self.startAvailableAirport, self.endAvailableAirport = "", "", ""
        self.startAvailableTime, self.endAvailableTime = 0, 0

class mtcType:
    def __init__(self):
        self.id, self.airport, self.tailNumber = "", "", ""
        self.startTime, self.endTime = 0, 0

class airportClosureType:
    def __init__(self):
        self.code = ""
        self.startTime, self.endTime = 0, 0

class paraSet:
    def __init__(self):
        self.turnTime = 0
        self.maxDelayTime = 0
        self.w_cancelMtc, self.cancelFlt = 0, 0
        self.w_violatedBalance, self.w_violatedPosition = 0, 0
        self.w_fltDelay, self.w_fltSwap = 0, 0
        self.maxRunTime = 0

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
    return isCheck

def readConfigurationFile(input_dir, output_dir):
    # configFile = "./Config.txt"
    # if not os.path.isfile(configFile):
    #     return
    # i = 0
    # print("\nReading the configuration file")
    # with open(configFile, "r") as myfile:
    pass

def importAircrafts(fullFileName: str) -> (bool, List[aircraftType]):
    if not os.path.isfile(fullFileName):
        print("Error while reading aircrafts!")
        return False, []
    doc = xml.dom.minidom.parse(fullFileName)
    pass
    # TODO


if __name__ == "__main__":
    print("program start...")
    succeed, input_path, output_path = processArguments(len(sys.argv), sys.argv)
    if not succeed:
        readConfigurationFile(input_path, output_path)
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
        