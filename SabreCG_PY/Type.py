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
