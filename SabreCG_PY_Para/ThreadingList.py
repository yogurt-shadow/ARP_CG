import threading

"""
Multi-threaded Safe List Class
"""
class ThreadingList:
    def __init__(self):
        self.lock = threading.Lock()
        self.content = list()

    def append(self, item) -> None:
        with self.lock:
            self.content.append(item)

    def remove(self, item) -> None:
        with self.lock:
            self.content.remove(item)

    def reset(self):
        with self.lock:
            self.content = list()
    
    def __len__(self) -> int:
        return len(self.content)
    
    def insertSubNode(self, subNode) -> bool:
        with self.lock:
            if self.content == []:
                self.content.append(subNode)
                return True
            else:
                deleted = []
                for i in range(len(self.content)):
                    _subNode = self.content[i]
                    if _subNode.LessKey(subNode):
                        self.content = [self.content[i] for i in range(len(self.content)) if i not in deleted]
                        return False
                    if subNode.LessKey(_subNode):
                        deleted.append(i)
                self.content = [self.content[i] for i in range(len(self.content)) if i not in deleted]
                self.content.append(subNode)
                return True
            
        """
        nodelist1
        nodelist2
        nodelist3


        nodelist1
        a
        a b
        a b c


        a
        a b
        a c

        [a e]
        [b f]
        [c]
        [d]

        a0 a1   thread1
        a2 a3   thread2
        """