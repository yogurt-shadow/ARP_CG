class OperLeg:
    def __init__(self, leg, aircraft = None):
        self._leg = leg
        self._depTime = leg.getDepTime()
        self._arrTime = leg.getArrTime()
        self._operAircraft = aircraft

    def print(self):
        pass

    def getLeg(self):
        return self._leg