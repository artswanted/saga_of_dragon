import datetime
from order_type import ORDER_NOTICES, EGMC_TOOL_REQ_ORDER
from gm import DragonicaGM
from gm_struct import OrderDB, pack_order_db

def GM_SEND_ORDER_NOTICES(_gm: DragonicaGM, message, repeat_count = 0, repeate_time = 0):
    data = OrderDB()
    data.Type = ORDER_NOTICES
    data.kCmdGuid = _gm.cmdGuid
    data.dtTime[0] = datetime.datetime.now()
    data.iValue[0] = repeat_count
    data.iValue[1] = repeate_time
    data.wsString2 = message
    data.usRealm = 1
    data.usChannel = 1
    _gm.send(EGMC_TOOL_REQ_ORDER, pack_order_db(data))

with DragonicaGM('127.0.0.1') as gm:
    GM_SEND_ORDER_NOTICES(gm, "kek")
