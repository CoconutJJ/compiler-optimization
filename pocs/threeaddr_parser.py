class MultiOp:

    def __init__(self, **args) -> None:
        self.ops = []


class Variable:

    def __init__(self, name: str) -> None:
        self.name = name
        self._ssa_index = 0

    def is_name_equal(self, __value: "Variable") -> bool:
        return self.name == __value.name

    def __eq__(self, __value: "Variable") -> bool:

        return self.name == __value.name and self._ssa_index == __value._ssa_index

    def __hash__(self) -> int:
        return hash(f"{self.name}_{self._ssa_index}")


class TwoOp:
    def __init__(self, dest, op1) -> None:
        self.dest = dest
        self.op1 = op1


class ThreeOp:
    def __init__(self, dest, op1, op2) -> None:
        self.dest = dest
        self.op1 = op1
        self.op2 = op2


class Add(ThreeOp):
    def __init__(self, dest, op1, op2) -> None:
        super().__init__(dest, op1, op2)


class Sub(ThreeOp):
    def __init__(self, dest, op1, op2) -> None:
        super().__init__(dest, op1, op2)


class Mul(ThreeOp):
    def __init__(self, dest, op1, op2) -> None:
        super().__init__(dest, op1, op2)


class Div(ThreeOp):
    def __init__(self, dest, op1, op2) -> None:
        super().__init__(dest, op1, op2)


class Set(TwoOp):
    def __init__(self, dest, op1) -> None:
        super().__init__(dest, op1)


class Phi:

    def __init__(self, dest, **args) -> None:
        self.dest = None
        self.ops = list(args)


class Block:

    block_id = 0

    def __init__(self) -> None:
        self.statements = []

        self.id = Block.block_id
        Block.block_id += 1

        self.parents = []
        self.left: Block = None
        self.right: Block = None

    def __hash__(self) -> int:
        return hash(self.id)

class SSARenamingRecord:

    def __init__(self) -> None:
        self.counters: dict[Variable, int] = dict()


def postorder_traversal(root: Block):

    st = [root]
    sequence = []
    visited = set()

    while len(st) != 0:

        blk = st[-1]

        if blk.right is not None and blk.right not in visited:
            visited.add(blk.right)
            st.append(blk.right)
            continue

        if blk.left is not None and blk.left not in visited:
            visited.add(blk.left)
            st.append(blk.left)
            continue

        sequence.append(blk)
        st.pop()

    return sequence


def compute_multi_block_live_global_names(blocks: list[Block]):
    """
    Find all global names that are live in more than one block
    """
    global_names = set()
    live_blocks = dict()

    for b in blocks:

        killed = set()

        for st in b.statements:

            if st.op1 not in killed:
                global_names.add(st.op1)

            if st.op2 not in killed:
                global_names.add(st.op2)

            killed.add(st.dest)
            live_blocks[st.dest] = live_blocks.get(st.dest, set()).union(b)

    return global_names, live_blocks


def insert_phi_functions(
    global_names: set[Variable],
    live_block_map: dict[Variable, set[Block]],
    dom_frontier_map: dict[Block, set[Block]],
):
    for v in global_names:

        # all blocks that contain upwards exposed uses of v
        live_blocks = list(live_block_map[v])

        # insert phi fns using the iterated dominance frontier algorithm
        while len(live_blocks) != 0:
            b = live_blocks.pop()
            for frontier_node in dom_frontier_map[b]:

                for st in frontier_node.statements:
                    if not isinstance(st, Phi):
                        continue

                    if st.dest == v:
                        break

                else:  # executed if no phi function for v is found

                    frontier_node.statements = [Phi(dest=v)] + frontier_node.statements

                    # since the phi function kills the previous definition of v, this may induce addition of more
                    # phi functions, we need to add this frontier node to our list
                    live_blocks.append(frontier_node)


def compute_dominators(blocks: list[Block]):

    dom_nodes: dict[Block, set] = dict()

    for b in blocks:
        dom_nodes[b] = set()

    has_changes = True

    while has_changes:
        has_changes = False
        for b in blocks:

            dominators = set(blocks)
            for parent in b.parents:
                dominators = dominators.intersection(dom_nodes[parent])

            if len(dom_nodes[b]) != len(dominators):
                has_changes = True

            dom_nodes[b] = dominators.union(set([b]))

    # invert the graph

    dom_nodes_inverse: dict[Block, set] = dict()

    for b in dom_nodes:
        for dominator in dom_nodes[b]:
            dom_nodes_inverse[dominator] = dom_nodes_inverse.get(
                dominator, set()
            ).union(b)

    return dom_nodes_inverse


def compute_immediate_dominator(node: Block, dom_nodes: dict[Block, set]):
    dominators = []
    for b in dom_nodes:

        if node in dom_nodes[b]:
            dominators.append(b)

    for candidate in dominators:
        dom_list = [
            i
            for i, v in enumerate(dominators)
            if candidate != v and candidate not in dom_nodes[v]
        ]

        if len(dom_list) > 1:
            continue

        return dominators[dom_list[0]]


def compute_dominance_frontier(dom_nodes: dict[Block, set]):

    dom_frontier: dict[Block, set] = dict()

    # first invert the dom_nodes graph, this gives for each node b, which nodes
    # b dominates

    for b in dom_nodes:
        for dom_node in dom_nodes[b]:
            frontier_nodes = []
            if dom_node.left not in dom_nodes[b] and dom_node.left != b:
                frontier_nodes.append(dom_node.left)

            if dom_node.right not in dom_nodes[b] and dom_node.right != b:
                frontier_nodes.append(dom_node.right)

            dom_frontier[b] = dom_frontier.get(b, set()).union(frontier_nodes)

    return dom_frontier


class Parser:
    def __init__(self, source: str) -> None:
        self.code: list[ThreeOp | TwoOp] = []
        self.labels: dict[str, int] = dict()
        lines = source.splitlines()

        for l in lines:

            args = l.split(" ")

            match args[0]:

                case "alloc":
                    self.parse_alloc(args[1:])
                case "set":
                    self.parse_set(args[1:])
                case "add":
                    self.parse_add(args[1:])
                case "sub":
                    self.parse_sub(args[1:])
                case "mul":
                    self.parse_mul(args[1:])
                case "div":
                    self.parse_div(args[1:])
                case "mov":
                    self.parse_mov(args[1:])
                case "phi":
                    self.parse_phi(args[1:])
                case "jmpif":
                    self.parse_jmpif(args[1:])
                case "jmp":
                    self.parse_jmp(args[1:])
                case s:
                    pass

    def parse_alloc(s: list[str]):
        pass

    def parse_set(s: list[str]):
        pass

    def parse_add(s: list[str]):
        pass

    def parse_sub(s: list[str]):
        pass

    def parse_mul(s: list[str]):
        pass

    def parse_div(s: list[str]):
        pass

    def parse_mov(s: list[str]):
        pass

    def parse_phi(s: list[str]):
        pass

    def parse_jmpif(s: list[str]):
        pass

    def parse_jmp(s: list[str]):
        pass
