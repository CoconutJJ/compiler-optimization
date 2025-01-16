from enum import Enum
from constructs import Argument, Value, Function

class Token(Enum):
    AT = 0
    LPAREN = 1
    RPAREN = 2
    LCURL = 3
    RCURL = 4
    COLON = 5
    COMMA = 6
    EQUAL = 7
    DOT = 8
    STR = 9
    ADD = 10
    SUB = 11
    MUL = 12
    DIV = 13
    NEG = 14
    NOT = 15
    INT = 16
    BOOL = 17
    BR = 18
    JMP = 19


class IRTokenizer:

    def __init__(self, source: str) -> None:
        self.IRSource = source
        self.LastStr = ""

    def match(self, s: str):

        if self.IRSource[: len(s)] == s:
            self.IRSource = self.IRSource[len(s) :]
            return True

        return False

    def peek(self):
        return self.IRSource[0]

    def advance(self):
        self.IRSource = self.IRSource[1:]

    def parse_str(self):

        ident = ""

        for i, v in enumerate(self.IRSource):

            if not v.isalnum():
                self.IRSource = self.IRSource[i:]
                break

            ident += v

        return ident

    def parse_keyword(self) -> Token:

        identifier = self.parse_str()

        match identifier:
            case "add":
                return Token.ADD
            case "sub":
                return Token.SUB
            case "mul":
                return Token.MUL
            case "div":
                return Token.DIV
            case "br":
                return Token.BR
            case "jmp":
                return Token.JMP
            case "int":
                return Token.INT
            case "bool":
                return Token.BOOL
            case _:
                self.LastStr = identifier
                return Token.STR

    def get_last_str(self):
        return self.LastStr

    def tokenize(self) -> Token:

        while len(self.IRSource) > 0:

            match self.peek():
                case "@":
                    self.advance()
                    return Token.AT

                case "(":
                    self.advance()
                    return Token.LPAREN

                case ")":
                    self.advance()
                    return Token.RPAREN

                case "{":
                    self.advance()
                    return Token.LCURL

                case "}":
                    self.advance()
                    return Token.RCURL

                case ":":
                    self.advance()
                    return Token.COLON

                case ",":
                    self.advance()
                    return Token.COMMA

                case "=":
                    self.advance()
                    return Token.EQUAL

                case ".":
                    self.advance()
                    return Token.DOT
                case "\t":
                    self.advance()
                case "\n":
                    self.advance()
                case " ":
                    self.advance()

                case _:
                    return self.parse_keyword()


class IRParser:

    def __init__(self, source: str) -> None:
        self.tokenizer = IRTokenizer(source)
        self.current_token = self.tokenizer.tokenize()
        self.symbols = dict()
    def advance(self):
        old_token = self.current_token
        self.current_token = self.tokenizer.advance()

        return old_token

    def peek(self):
        return self.current_token

    def match(self, token: Token) -> bool:
        if self.peek() == token:
            self.advance()
            return True

        return False

    def match_str(self, s: str) -> bool:

        if self.peek() != Token.STR:
            return False

        if self.tokenizer.get_last_str() == s:
            self.advance()
            return True

        return False

    def consume(self, token: Token, error: str = ""):

        assert self.match(token), error

    def consume_str(self, error: str = "") -> str:
        assert self.match(Token.STR), error

        return self.tokenizer.get_last_str()

    def parse_args(self):
        pass

    def parse_arglist(self):
        
        argument_list = []
        while not self.match(Token.RPAREN):
            arg_name = self.consume_str()
            self.consume(Token.COLON, "expected ':' after function argument")

            type_name = self.consume_str()

            if self.peek() != Token.RPAREN:
                self.consume(Token.COMMA, "expected ',' or ')' after function argument")
            
            match type_name:
                case "int":
                    self.symbols[arg_name] = Argument(Value.VAL_INT)
                case "bool":
                    self.symbols[arg_name] = Argument(Value.VAL_BOOL)

            argument_list.append(self.symbols[arg_name])

        return argument_list

    def parse_block(self):

        

        pass

    def parse_function(self):

        self.consume(Token.AT)
        fn_name = self.consume_str()

        self.consume(Token.LPAREN)

        if not self.match(Token.RPAREN):
            pass
        
        fn = Function(self.parse_arglist())

        self.consume(Token.LCURL)

        while not self.match(Token.RCURL):
            self.parse_block()