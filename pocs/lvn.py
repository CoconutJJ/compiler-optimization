class Op:
    ADD = 1
    SUB = 2
    MUL = 3
    DIV = 4


class ValueTable:
    def __init__(self) -> None:
        self.var_to_value_table = dict()
        self.expr_to_value_table = dict()
        self.value_to_var_table = dict()
        self.value_no = 0

    def var_to_value(self, var: str):
        if var not in self.var_to_value_table:
            self.var_to_value_table[var] = self.value_no
            self.value_no += 1

        return self.value_to_var_table[var]

    def expr_to_value(self, expr: tuple[str, int, str]):
        fst, op, snd = expr

        expr_key = (self.var_to_value(fst), op, self.var_to_value(snd))

        if expr_key in self.expr_to_value_table:
            return self.expr_to_value_table[expr_key]

        if op not in (Op.ADD, Op.MUL):
            self.expr_to_value_table[expr_key] = self.value_no
            self.value_no += 1
            return self.expr_to_value_table[expr_key]

        a, b, c = expr_key
        expr_key = (c, b, a)

        if expr_key not in self.expr_to_value_table:
            self.expr_to_value_table[expr_key] = self.value_no
            self.value_no += 1

        return self.expr_to_value_table[expr_key]

    def set_var_value(self, var: str, value: int):
        self.var_to_value_table[var] = value
        self.value_to_var_table[value] = var

    def value_to_var(self, value: str):
        return self.value_to_var_table[value]
