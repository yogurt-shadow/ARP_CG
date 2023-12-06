import sys
import os
import util as ut

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