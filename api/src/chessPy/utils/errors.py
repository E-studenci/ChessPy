

class DynamicCodeError(Exception):
    def __init__(self, name, description, code, *args: object) -> None:
        self.name = name
        self.description = description
        self.code = code
        super().__init__(*args)


class _GenericError(Exception):
    def __init__(self, name, description, *args: object) -> None:
        self.name = name
        self.description = description
        super().__init__(*args)


class DatabaseError(_GenericError):
    pass

class UnauthorizedError(_GenericError):
    pass

class BadRequestError(_GenericError):
    pass