class Values:

    def __init__(self) -> None:
        
        self.variables: dict[str, int] = dict()
        self.values: dict[tuple[str, int, int], int] = dict()
        self.current_value = 0

    def allocate_value(self) -> int:

        v = self.current_value

        self.current_value += 1

        return v

    def var2value(self, var: str) -> int:

        if var not in self.variables:
            self.variables[var] = self.allocate_value()

        return self.variables[var]

    def get_expression_value_no(self, expr: tuple[str, str, str], commutative=False):
        
        v1, op, v2 = expr

        val1, val2 = self.var2value(v1), self.var2value(v2)

        if (op, val1, val2) not in self.values:

            if commutative and (op, val2, val1) in self.values:
                self.values[(op, val1, val2)] = self.values[(op, val2, val1)]
            else:
                self.values[(op, val1, val2)] = self.allocate_value()
        
        return self.values[(op, val1, val2)]

    def update(self, expr: tuple[str, str, str, str], commutative=False) -> int:

        dest, v1, op, v2 = expr

        self.variables[dest] = self.get_expression_value_no((v1, op, v2), commutative)

        return self.variables[dest]

    