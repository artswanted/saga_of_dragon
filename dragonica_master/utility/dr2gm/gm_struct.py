import struct
import uuid
import datetime

GUID_NULL = uuid.UUID(bytes=b'\x00'*16, version=4)

class OrderDB:
    # Head
    kCmdGuid: uuid.UUID = GUID_NULL
    kReqGuid: uuid.UUID = GUID_NULL
    Type: int = 0
    usRealm: int = 0
    usChannel: int = 0
    # Body
    kGuid: list[uuid.UUID] = [GUID_NULL] * 5
    fValue: list[float] = [0.0] * 5
    iValue: list[int] = [0] * 5
    biValue: list[int] = [0] * 5

    wsString1: str = ""
    wsString2: str = ""
    dtTime: list[datetime.datetime] = [datetime.datetime.fromtimestamp(0)] * 2

def pack_uuid(_uuid: uuid.UUID):
    def __(d):
        if d == 0:
            return b'\0' * 6
        m = hex(d)[2::]
        return (bytes.fromhex('0' * (8 - len(m)) + m))
    data = struct.pack(
        '>LHHBB',
        _uuid.fields[0],
        _uuid.fields[1],
        _uuid.fields[2],
        _uuid.fields[3],
        _uuid.fields[4]
    ) + __(_uuid.fields[5])
    assert len(data) == 16, len(data)
    return data

def pack_wstring(_str: str):
    l = len(_str)
    if not l:
        struct.pack('I', 0)
    return struct.pack('I', l) + _str.encode('UTF-16LE')

def pack_datetime(_datetime: datetime.datetime):
    return struct.pack(
        "hhhhhhl",
        _datetime.year,
        _datetime.month,
        _datetime.day,
        _datetime.hour,
        _datetime.minute,
        _datetime.second,
        _datetime.microsecond,
    )

def pack_order_db(order: OrderDB):
    return  struct.pack('H', order.usRealm) + \
            struct.pack('H', order.usChannel) + \
            pack_uuid(order.kCmdGuid) + \
            struct.pack('h', order.Type) + \
            pack_uuid(order.kGuid[0]) + \
            pack_uuid(order.kGuid[1]) + \
            struct.pack('iiiii', *order.iValue) + \
            struct.pack('QQQQQ', *order.biValue) + \
            pack_wstring(order.wsString1) + \
            pack_wstring(order.wsString2) + \
            pack_datetime(order.dtTime[0]) + \
            pack_datetime(order.dtTime[1])