class Node:
    def __init__(self, name, depth=0):
        self.name = name
        self.total = 0.0
        self.num_calls = 0
        self.min = float('inf')
        self.max = 0
        self.children = {}
        self.depth = depth

    def add_child(self, child_name):
        if child_name not in self.children:
            self.children[child_name] = Node(child_name, self.depth + 1)
        return self.children[child_name]