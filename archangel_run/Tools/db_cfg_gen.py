import os
import argparse

archangel_path = os.path.abspath(os.path.pardir)
mmc_path = os.path.join(archangel_path, 'MMC')
config_path = os.path.join(mmc_path, 'Patch/Config')

parser = argparse.ArgumentParser()
parser.add_argument("--id", required=True)
parser.add_argument("--pw", required=True)
parser.add_argument("--addr", required=True)
args = parser.parse_args()

def gen_config(path, data):
    with open(path, 'w+') as ff:
        ff.write(data)

def DB_Config(id, pw, addr):
    return f"""[R0C0_COMMON]
MAX_DB = 1

[R0C0_DB_INFO_1]
TYPE	= 1
ADDR	= {addr}
NAME	= DR2_SiteConfig
ID	= {id}
PW	= {pw}

[ETC]
SERVER_TBNAME = TB_SiteConfig
MAPCFG_TBNAME_STATIC = TB_MapCfg_Static
MAPCFG_TBNAME_MISSION = TB_MapCfg_Mission
MAPCFG_TBNAME_PUBLIC = TB_MapCfg_Public"""

def Contents_DB_Config(id, pw, addr):
    return f"""[R1C0_COMMON]
MAX_DB = 4

[R1C0_DB_INFO_1]
TYPE	= 2
ADDR	= {addr}
NAME	= DR2_Def
ID	= {id}
PW	= {pw}
CONNECT = 1

[R1C0_DB_INFO_2]
TYPE	= 5
ADDR	= {addr}
NAME	= DR2_User
ID	= {id}
PW	= {pw}
CONNECT = 1
WORKER_COUNT = 6

[R1C0_DB_INFO_3]
TYPE	= 4
ADDR	= {addr}
NAME	= DR2_Member
ID	= {id}
PW	= {pw}
CONNECT = 1

[R1C0_DB_INFO_4]
TYPE	= 11
ADDR	= {addr}
NAME	= DR2_Local
ID	= {id}
PW	= {pw}
CONNECT = 1

[ETC]
LOGICAL_CHANNEL_COUNT = 1
LOGICAL_CHANNEL_MAX_USER = 100"""

def GM_DB_Config(id, pw, addr):
    return f"""[R0C0_COMMON]
MAX_DB = 1

[R0C0_DB_INFO_1]
TYPE	= 10
ADDR	= {addr}
NAME	= DR2_GM
ID	= {id}
PW	= {pw}"""

def Immigration_DB_Config(id, pw, addr):
    return f"""[R0C0_COMMON]
MAX_DB = 2

[R0C0_DB_INFO_1]
TYPE	= 1
ADDR	= {addr}
NAME	= DR2_SiteConfig
ID	= {id}
PW	= {pw}

[R0C0_DB_INFO_2]
TYPE	= 4
ADDR	= {addr}
NAME	= DR2_Member
ID	= {id}
PW	= {pw}

[ETC]
SERVER_TBNAME = TB_SiteConfig
RESTRICT_TBNAME = TB_Site_RestrictionsOnChannel
MAPCFG_TBNAME_STATIC = TB_MapCfg_Static
MAPCFG_TBNAME_MISSION = TB_MapCfg_Mission
MAPCFG_TBNAME_PUBLIC = TB_MapCfg_Public"""

def Log_DB_Config(id, pw, addr):
    return f"""[R0C0_COMMON]
MAX_DB = 1

[R0C0_DB_INFO_1]
TYPE	= 9
ADDR	= {addr}
NAME	= DR2_Log
ID	= {id}
PW	= {pw}
CONNECT = 1
WORKER_COUNT = 8

[R1C0_COMMON]
MAX_DB = 1

[R1C0_DB_INFO_1]
TYPE	= 9
ADDR	= {addr}
NAME	= DR2_Log
ID	= {id}
PW	= {pw}
CONNECT = 1
WORKER_COUNT = 8"""

gen_config(os.path.join(mmc_path, 'DB_Config.ini'), DB_Config(args.id, args.pw, args.addr))
gen_config(os.path.join(config_path, 'Contents_DB_Config.ini'), Contents_DB_Config(args.id, args.pw, args.addr))
gen_config(os.path.join(config_path, 'GM_DB_Config.ini'), GM_DB_Config(args.id, args.pw, args.addr))
gen_config(os.path.join(config_path, 'Immigration_DB_Config.ini'), Immigration_DB_Config(args.id, args.pw, args.addr))
gen_config(os.path.join(config_path, 'Log_DB_Config.ini'), Log_DB_Config(args.id, args.pw, args.addr))
