import csv
import os

SOURCE_TABLE = os.path.join(os.path.dirname(__file__), "./bank/item_bag.csv")
DT_DEF_PATH = os.path.join(os.path.dirname(__file__), "../MMC/Patch/Contents/Table/DR2_Def/")

DT_ITEM_BAG_PATH = os.path.join(DT_DEF_PATH, "TB_DefItemBag.csv")
DT_SUCCESS_RATE_CONTROL_PATH = os.path.join(DT_DEF_PATH, "TB_DefSuccessRateControl.csv")
DT_ITEM_BAG_ELEMENTS_PATH = os.path.join(DT_DEF_PATH, "TB_DefItemBagElements.csv")
MONEY_CONTROL_PATH = os.path.join(DT_DEF_PATH, "TB_DefDropMoneyControl.csv")
COUNT_CONTROL_PATH = os.path.join(DT_DEF_PATH, "TB_DefCountControl.csv")

ITEM_BAG_FIELDS = ['f_NationCodeStr', 'BagNo', 'Level', 'Memo', 'ElementsNo', 'SuccessRateControlNo', 'CountControlNo', 'DropMoneyControlNo']
SUCCESS_RATE_CONTROL_FIELDS = ['f_NationCodeStr', 'No', 'Memo', 'Rate01', 'Rate02', 'Rate03', 'Rate04', 'Rate05', 'Rate06', 'Rate07', 'Rate08', 'Rate09', 'Rate10', 'Total_Rate']
ITEM_BAG_ELEMENTS_FIELDS = ['f_NationCodeStr', 'BagNo', 'Memo', 'ItemNo01', 'ItemNo02', 'ItemNo03', 'ItemNo04', 'ItemNo05', 'ItemNo06', 'ItemNo07', 'ItemNo08', 'ItemNo09', 'ItemNo10', 'ItemNoTypeFlag']
MONEY_CONTROL_FIELDS = ['No', 'Memo', 'Rate', 'Min', 'Max']
COUNT_CONTROL_FIELDS = ['f_NationCodeStr', 'No', 'Memo', 'Count01', 'Count02', 'Count03', 'Count04', 'Count05', 'Count06', 'Count07', 'Count08', 'Count09', 'Count10', 'Total_Count']

def save_cast(func, val, default):
    try:
        return func(val)
    except ValueError:
        return default

def flush_csv(path, fields, data):
    with open(path, 'w', newline='', encoding='utf-8') as ff:
        w = csv.DictWriter(ff, fields)
        w.writeheader()
        w.writerows(data)

def make_item_bag(data):
    print("Make Drop start...")

    # set for fast O(1) amortized lookup
    success_rate_seen, item_bag_elements_seen, item_bag_out_seen, money_control_seen, count_control_seen = set(), set(), set(), set(), set()
    success_rate_out, item_bag_elements_out, item_bag_out, money_control_out, count_control_out = [], [], [], [], []

    for i in range(0, len(data), 3):
        data_row, success_row, count_row = data[i], data[i + 1], data[i + 2]

        nation_code_key = (data_row['f_NationCode'], data_row['BagNo'], data_row['Level'])
        if nation_code_key not in item_bag_out_seen:
            item_bag_out.append({
                'f_NationCodeStr': data_row['f_NationCode'],
                'BagNo': int(data_row['BagNo']),
                'Memo': data_row['Memo'],
                'Level': data_row['Level'],
                'SuccessRateControlNo': success_row['BagElementsNo'],
                'CountControlNo': count_row['BagElementsNo'],
                'ElementsNo': data_row['BagElementsNo'],
                'DropMoneyControlNo': save_cast(int, data_row['MoneyControllNo'], 0),
            })
            item_bag_out_seen.add(nation_code_key)

        nation_code_key = (data_row['f_NationCode'], data_row['BagElementsNo'])
        if nation_code_key not in item_bag_elements_seen:
            item_bag_elements_out.append({
                'f_NationCodeStr': data_row['f_NationCode'],
                'BagNo': int(data_row['BagElementsNo']),
                'Memo': data_row['BagElementsMemo'],
                'ItemNoTypeFlag': 0,
            } | {f'ItemNo{index:02}': save_cast(int, data_row[f"Value{index:02}"], 0) for index in range(1, 11)})
            item_bag_elements_seen.add(nation_code_key)

        nation_code_key = (success_row['f_NationCode'], success_row['BagElementsNo'])
        if nation_code_key not in success_rate_seen:
            success_rate_out.append({
                'f_NationCodeStr': success_row['f_NationCode'],
                'No': int(success_row['BagElementsNo']),
                'Memo': success_row['BagElementsMemo'],
                'Total_Rate': sum([save_cast(int, success_row[f"Value{index:02}"], 0) for index in range(1, 11)]),
            } | {f"Rate{index:02}": save_cast(int, success_row[f"Value{index:02}"], 0) for index in range(1, 11)})
            success_rate_seen.add(nation_code_key)

        if data_row['MoneyControllNo'] not in money_control_seen and data_row['MoneyControllNo'] != '':
            money_control_out.append({
                'No': int(data_row['MoneyControllNo']),
                'Memo': data_row['MoneyControllMemo'],
                'Rate': data_row['MoneyRate'],
                'Min': data_row['MoneyMin'],
                'Max': data_row['MoneyMax'],
            })
            money_control_seen.add(data_row['MoneyControllNo'])

        nation_code_key = (count_row['f_NationCode'], count_row['BagElementsNo'])
        if nation_code_key not in count_control_seen:
            count_control_out.append({
                'f_NationCodeStr': count_row['f_NationCode'],
                'No': int(count_row['BagElementsNo']),
                'Memo': count_row['BagElementsMemo'],
                'Total_Count': 0,
            } | {f"Count{index:02}": save_cast(int, count_row[f"Value{index:02}"], 0) for index in range(1, 11)})
            count_control_seen.add(nation_code_key)

    flush_csv( DT_ITEM_BAG_PATH, ITEM_BAG_FIELDS,
        sorted(item_bag_out, key=lambda x: (x['BagNo'], x['f_NationCodeStr']))
    )

    flush_csv( DT_SUCCESS_RATE_CONTROL_PATH, SUCCESS_RATE_CONTROL_FIELDS,
        sorted(success_rate_out, key=lambda x: (x['No'], x['f_NationCodeStr']))
    )

    flush_csv( DT_ITEM_BAG_ELEMENTS_PATH, ITEM_BAG_ELEMENTS_FIELDS,
        sorted(item_bag_elements_out, key=lambda x: (x['BagNo'], x['f_NationCodeStr']))
    )

    flush_csv( MONEY_CONTROL_PATH, MONEY_CONTROL_FIELDS,
        sorted(money_control_out, key=lambda x: x['No'])
    )

    flush_csv( COUNT_CONTROL_PATH, COUNT_CONTROL_FIELDS,
        sorted(count_control_out, key=lambda x: (x['No'], x['f_NationCodeStr']))
    )

    print("Make Drop end...")


if __name__ == "__main__":
    with open(SOURCE_TABLE, newline='', encoding='utf-8') as ff:
        make_item_bag(list(csv.DictReader(ff)))
