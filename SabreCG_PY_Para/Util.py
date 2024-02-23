MAXTURNTIME              =            5 * 60 * 60
DATECUTTIME              =            3 * 60 * 60
DATEDURATION             =            24 * 60 * 60
MINLEG                   =            1
MAXLEG                   =            7
TIMEDIFF                 =            0
INITLOFNUM               =            10000
SELECTNUM                =            100
SEED                     =            1
THREADSIZE               =            8

class util:
    turnTime             =            10
    maxDelayTime         =            10800
    w_cancelMtc          =            500
    w_cancelFlt          =            100
    w_violatedBalance    =            1
    w_violatedPosition   =            1
    w_fltDelay           =            1
    w_fltSwap            =            1
    maxRunTime           =            0
    newamount            =            2
    threadSize           =            2

    # @classmethod
    def print():
        print("print parameters")
        print("turnTime = ", util.turnTime)
        print("maxDelayTime = ", util.maxDelayTime)
        print("w_cancelMtc = ", util.w_cancelMtc)
        print("w_cancelFlt = ", util.w_cancelFlt)
        print("w_violatedBalance = ", util.w_violatedBalance)
        print("w_violatedPosition = ", util.w_violatedPosition)
        print("w_fltDelay = ", util.w_fltDelay)
        print("w_fltSwap = ", util.w_fltSwap)
        print("maxRunTime = ", util.maxRunTime)
        print("newamount = ", util.newamount)
        print("threadSize = ", util.threadSize)
