class Function:

    def __init__(self, args: list['Argument']) -> None:
        self.args = args
        self.blocks = []

class Value:
    VAL_BOOL = 0
    VAL_INT = 1

    def __init__(self, type: int) -> None:
        self.type = type
        pass

class Argument(Value):

    def __init__(self, type: int) -> None:
        super().__init__(type)
        pass
    

class Function:

    def __init__(self, arguments: list[Argument]) -> None:
        self.arguments = arguments

class Instruction(Value):

    ADD = 0
    SUB = 1
    MUL = 2
    DIV = 3
    PHI = 4

    def __init__(self, type: int, op_code: int) -> None:
        super().__init__(type)

        self.op_code = op_code
        self.block = None


class UnaryInstruction(Instruction):

    def __init__(self, type: int, op_code: int, op: Instruction) -> None:
        super().__init__(type, op_code)
        self.op = op


class BinaryInstruction(Instruction):

    def __init__(
        self,
        type: int,
        op_code: int,
        op_a: Instruction,
        op_b: Instruction,
    ) -> None:

        super().__init__(type, op_code)
        self.op_a = op_a
        self.op_b = op_b


class PhiInstruction(Instruction):

    def __init__(self, type: int) -> None:
        super().__init__(type, Instruction.PHI)
        self.operands = []

    def add_operand(self, op: Instruction, block: "Block"):
        self.operands.append((op, block))


class Block:

    def __init__(self) -> None:
        self.instruction = []
        self.jump_instruction = None
        self.left = None
        self.right = None

        self.preds = []


    def add_instruction(self, inst: Instruction):
        inst.block = self
        self.instruction.append(inst)


        
