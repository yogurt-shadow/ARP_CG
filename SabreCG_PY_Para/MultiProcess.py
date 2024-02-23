import threading
from queue import Queue

def WorkerThread(_model, queue, _aircraftPart):
    for _aircraft in _aircraftPart:
        tempLof = _model.findNewMultiColumns(_aircraft)
        queue.put(tempLof)

# Split elements into num's parts
def Split(elements, num):
    length = len(elements)
    res = []
    if length < num:
        res = [[elements[i]] for i in range(length)]
        for i in range(length, num):
            res.append([])
    else:
        res = [[elements[i]] for i in range(num)]
        for i in range(num, length):
            res[i % num].append(elements[i])
    print("Split into " + str(num) + " parts")
    return res

def MultiProcess(num, _model, _aircraftList):
    # Create a queue to communicate with the worker threads
    print("multi number: ", num)
    queue = Queue()
    threads = []
    _aircraftParts = Split(_aircraftList, num)
    # Create `num` worker threads
    for x in range(num):
        if _aircraftParts[x] == []:
            continue
        worker = threading.Thread(target=WorkerThread, args=(_model, queue, _aircraftParts[x]))
        # Setting daemon to True will let the main thread exit even though the workers are blocking
        worker.daemon = True
        worker.start()
        threads.append(worker)
    # Causes the main thread to wait for the threads to finish processing all the tasks
    for thread in threads:
        thread.join()
    result = []
    while not queue.empty():
        curr = queue.get()
        for i in curr:
            result.append(i)
    return result