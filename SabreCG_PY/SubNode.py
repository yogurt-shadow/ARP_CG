class SubNode:
    def __init__(self, leg = None, parentSubNode = None, subNodeCost = 0, delay = 0):
        self._leg = leg
        self._parentSubNode, self._subNodeCost, self._delay = parentSubNode, subNodeCost, delay

    def getLeg(self):
        return self._leg

    def setLeg(self, leg):
        self._leg = leg

    def getParentSubNode(self):
        return self._parentSubNode
    
    def setParentSubNode(self, parentSubNode):
        self._parentSubNode = parentSubNode

    def getSubNodeCost(self):
        return self._subNodeCost

    def setSubNodeCost(self, subNodeCost):
        self._subNodeCost = subNodeCost

    def getDelay(self):
        return self._delay
    
    def setDelay(self, delay):
        self._delay = delay

    def print(self):
        pass

    def getOperDepTime(self):
        return self._leg.getDepTime() + self._delay
    
    def getOperArrTime(self):
        return self._leg.getArrTime() + self._delay
    
    @classmethod
    def LessKey(p1, p2):
        if p1.getSubNodeCost() == p2.getSubNodeCost() and p1.getDelay() == p2.getDelay():
            return False
        a, b = p1.getSubNodeCost() <= p2.getSubNodeCost(), p1.getDelay() <= p2.getDelay()
        return a and b

    def CostKey(self):
        return self.getSubNodeCost()