class Aircraft:
    _count = 0
    """
    _tail: Tail Number
    _id
    _count
    _startT
    _endT
    _depStation
    _arrStation
    _dual
    _planLegList
    """
    def __init__(self, name, startT, endT, depS, arrS):
        self._tail = name
        self._startT, self._endT = startT, endT
        self._depStation, self._arrStation = depS, arrS
        self._id = _count
        _count += 1
        self._dual = 0
    
    def print(self):
        startT, endT = self._startT, self._endT
        print("Tail %s" % self._tail)
        print("%s %s" % (self._depStation.getName(), startT))
        print("%s %s" % (self._arrStation.getName(), endT))

    def sortScheLegByDepTime(self):
        pass

    def isPlanLegFeasible(self):
        pass