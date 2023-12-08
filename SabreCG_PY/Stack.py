class Stack(object):
    def __init__(self):
        self.stack = []

    def push(self, data):
        self.stack.append(data)
        
    def pop(self):
        if self.stack:
            return self.stack.pop()
        else:
            raise IndexError("Empty Stack")

    def peek(self):
        if self.stack:
            return self.stack[-1]
        
    def is_empty(self) -> bool:
        return not bool(self.stack)
 
    def size(self) -> int:
        return len(self.stack)
    
    def __len__(self) -> int:
        return len(self.stack)