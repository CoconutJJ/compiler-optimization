from value_numbering import Values


class Node:

    def __init__(self) -> None:
        self.parents = 0
        self.labels = []

    def add_parent(self):
        self.parents += 1

    def add_label(self, label):
        self.labels.append(label)


class VarNode(Node):

    def __init__(self, name: str) -> None:
        super().__init__()
        self.name = name
        self.add_label(name)


class OpNode(Node):

    def __init__(self, op: str, left: 'Node', right: 'Node') -> None:
        super().__init__()
        self.op = op
        self.left = left
        self.right = right
        
        left.add_parent()
        right.add_parent()


class DAG:

    def __init__(self) -> None:
        self.values = Values()
        self.variables: dict[int, Node] = dict()
        self.defs: dict[str, Node] = dict()
        self.nodes: set[Node] = set()

    def allocate_def(self, var: str) -> Node:

        value_no = self.values.var2value(var)

        if value_no in self.variables:
            self.defs[var] = self.variables[value_no]
            self.defs[var].add_label(var)

        else:
            self.defs[var] = VarNode(var)
            self.defs[var].add_label(var)
            self.variables[value_no] = self.defs[var]
            self.nodes.add(self.defs[var])

        return self.defs[var]

    def get_value_expression(self, stmt: tuple[str, str, str, str]):
        value_no = self.values.update(stmt, True)

        if value_no not in self.variables:

            _, arg1, op, arg2 = stmt

            arg1_node = self.allocate_def(arg1)
            arg2_node = self.allocate_def(arg2)

            self.variables[value_no] = OpNode(op, arg1_node, arg2_node)

            self.nodes.add(self.variables[value_no])

        return self.variables[value_no]
            

    def add_statement(self, stmt: tuple[str, str, str, str]):

        dest, _, _, _ = stmt

        expression = self.get_value_expression(stmt)

        self.defs[dest] = expression

        expression.add_label(dest)

    def optimize(self):
        statements = []
        generated = set()

        def generate(node: 'Node'):

            if isinstance(node, VarNode):
                return node.name
            elif isinstance(node, OpNode):
                for c in node.labels:
                    if c in generated:
                        return c

                l = generate(node.left)
                r = generate(node.right)
                statements.append("%s = %s %s %s" %
                                  (node.labels[0], l, node.op, r))
                generated.add(node.labels[0])
                return node.labels[0]

        for node in self.nodes:

            if node.parents == 0:
                generate(node)

        return statements


graph = DAG()

graph.add_statement(("a", "b", "+", "c"))
graph.add_statement(("b", "a", "-", "d"))
graph.add_statement(("c", "b", "+", "c"))
graph.add_statement(("d", "a", "-", "d"))

print(graph.optimize())
