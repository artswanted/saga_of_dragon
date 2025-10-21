import csv
import os

VERSION = (0, 0, 1)
DT_DEF = 1
DT_LOCAL = 2

DT_DEF_PATH = os.path.abspath('../dragonica_exe/archangel_run/MMC/Patch/Contents/Table/DR2_Def')
DT_LOCAL_PATH = os.path.abspath('../dragonica_exe/archangel_run/MMC/Patch/Contents/Table/DR2_Local')

TABLES = [
    {
        'dt': DT_DEF,
        'name': 'TB_DefQuestReward.csv',
        'collumns': [
            'NationCodeStr',
            'QuestID',
            'XmlPath',
            'GroupNo',
            'QuestType',
            'RewardExp',
            'RewardGold',
            'iRewardPerLevel_Exp',
            'iRewardPerLevel_Money',
            'LevelMin',
            'LevelMax',
            'LimitLevelMax',
            'TacticsLevel',
            'TacticsExp',
            'GuildExp',
            'QuestDifficult',
            'f_OrderIndex',
            'f_IsCanRemoteComplete',
            'f_IsCanShare',
            'TitleText',
            'GroupName',
            'Class',
            'PreQuestAnd',
            'PreQuestOr',
            'NotQuest',
            'MinParty',
            'MaxParty',
            'DraClassLimit',
            'f_Wanted_ClearCount',
            'f_Wanted_ItemNo',
            'f_Wanted_ItemCount',
            'f_Wanted_CoolTime',
            'f_Dra_Wanted_ItemNo',
            'f_Dra_Wanted_ItemCount',
            'f_IsCanRemoteAccept'
        ]
    },
    {
        'dt': DT_DEF,
        'name': 'TB_DefItemBag.csv',
        'collumns': [
            'f_NationCodeStr',
            'BagNo',
            'Level',
            'ElementsNo',
            'SuccessRateControlNo',
            'CountControlNo',
            'DropMoneyControlNo'
        ]
    },
    {
        'dt': DT_DEF,
        'name': 'TB_DefItemBagElements.csv',
        'collumns': [
            'f_NationCodeStr',
            'BagNo',
            'ItemNo01',
            'ItemNo02',
            'ItemNo03',
            'ItemNo04',
            'ItemNo05',
            'ItemNo06',
            'ItemNo07',
            'ItemNo08',
            'ItemNo09',
            'ItemNo10',
            'ItemNoTypeFlag'
        ]
    },
    {
        'dt': DT_DEF,
        'name': 'TB_DefItemBagGroup.csv',
        'collumns': [
            'f_NationCodeStr',
            'BagGroupNo',
            'SuccessRateNo',
            'BagNo01',
            'BagNo02',
            'BagNo03',
            'BagNo04',
            'BagNo05',
		    'BagNo06',
            'BagNo07',
            'BagNo08',
            'BagNo09',
            'BagNo10'
        ]
    },
    {
        'dt': DT_DEF,
        'name': 'TB_DefMonsterAbil.csv',
        'collumns': [
            'f_NationCodeStr',
            'MonsterAbilNo',
            'Type01',
            'Value01',
        ]
    },
    {
        'dt': DT_DEF,
        'name': 'TB_DefMonster.csv',
        'collumns': [
            'f_NationCodeStr',
            'MonsterNo',
            'Name',
            'Abil01',# Next abil by iterator
        ]
    },
    {
        'dt': DT_DEF,
        'name': 'TB_DefSuccessRateControl.csv',
        'collumns': [
            'f_NationCodeStr',
            'No',
            'Rate01', # Next by iterator
        ]
    },
    {
        'dt': DT_DEF,
        'name': 'TB_DefEffect_Player.csv',
        'collumns': [
            'NationCodeStr',
            'EffectID',
            'Name',
            'ActionName',
            'Type',
            'Interval',
            'DurationTime',
            'Toggle',
		    'Abil01' # Next by iterator
        ]
    },
    {
        'dt': DT_DEF,
        'name': 'TB_DefEffectAbil_Player.csv',
        'collumns': [
            'NationCodeStr',
            'EffectAbilNo',
            'Type01',
            'Value01',
        ]
    },
    {
        'dt': DT_LOCAL,
        'name': 'TB_GemStore.csv',
        'collumns': [
            'NpcGuid',
            'ItemNo',
            'Gem1',
            'GemCount1',
            'CP',
        ]
    },
    {
        'dt': DT_LOCAL,
        'name': 'TB_DefPremiumArticle.csv',
        'collumns': [
            'ArticleNo',
            'Type',
            'Lv',
            'Abil01',
            'Value01'
        ]
    },
    {
        'dt': DT_LOCAL,
        'name': 'TB_DefPremiumService.csv',
        'collumns': [
            'ServiceNo',
            'Title',
            'ServiceType',
            'Grade',
            'UseDate',
            'Article01'
        ]
    },
    {
        'dt': DT_DEF,
        'name': 'TB_DefCountControl.csv',
        'collumns': [
            'f_NationCodeStr',
            'No',
            'Count01',
        ]
    },
    {
        'dt': DT_DEF,
        'name': 'TB_DefDropMoneyControl.csv',
        'collumns': [
            'No',
            'Rate',
            'Min',
            'Max',
        ]
    },
    {
        'dt': DT_DEF,
        'name': 'TB_DefGemStore.csv',
        'collumns': [
            'NationCodeStr',
            'NpcGuid',
            'Menu',
            'OrderIndex',
            'ItemNo',
            'ItemCount',
            'Gem1',
            'GemCount1',
        ]
    },
]

header = f'/*Code generated by codegen version: {".".join(list(map(str, VERSION)))}*/'
global_gen = ''
for table in TABLES:
    name = table['name']
    print(f'Export collumns index for: {name}')
    dt = table['dt']
    dtpath = DT_DEF_PATH   if dt == DT_DEF else \
             DT_LOCAL_PATH if dt == DT_LOCAL else ""
    print(dtpath)
    if dtpath != "":
        with open(os.path.join(dtpath, name), encoding='utf-8') as ff:
            reader = csv.DictReader(ff, delimiter=',')
            code_gen = f'''
    {header}
    BM::Stream::STREAM_DATA buff;
    CsvParser csv;
    csvdb::load(buff, csv, BM::vstring(pkTBPath) << "{name}");
    const size_t
'''
            for i, collumn in enumerate(table['collumns']):
                idx = reader.fieldnames.index(collumn)
                if idx < 0:
                    print(f'Field: {collumn} not exist')
                    exit(-1)
                if not collumn.startswith('f_'):
                    collumn = 'f_' + collumn
                code_gen += f'\t{collumn} = {idx}'
                code_gen += ';\n' if i == len(table['collumns']) - 1 else ',\n'
            print(code_gen)
        global_gen += f'//{name}\n{code_gen}\n    while( csv.next_row() /*skip header*/ )\n'
    else:
        print(f'Undefined data table: {table["dt"]}')

print(global_gen)