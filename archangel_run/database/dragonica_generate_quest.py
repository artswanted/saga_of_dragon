import dragonica_classlimit as cl
import csv
import os

csvPath = os.path.join(os.path.dirname(__file__), "../MMC/Patch/Contents/Table/DR2_Def") # use when new row will auto-addeding to table (soon)
MAP_XML_PATH = os.path.join(os.path.dirname(__file__), "../MMC/Patch/Map/XML/")

# example how to create a quest
"""
7557: {
        "Memo": "Drakan quest",
        "QuestType": cl.EQuestType.QT_Normal_Day,
        "Reward": [
            cl.RewardExp(1940000),
            cl.RewardGold(30000),
            cl.RewardSelect(0, [
                cl.RewardItem(1, 940000730)
            ])
        ],
        "LevelLimit": cl.LevelLimit(40, 99),
        "ClassLimit": cl.HumansLimit.all_humans,
        "DraClassLimit": cl.DrakansLimit.all_drakans,
        "TitleText": cl.TitleText(600564747),
        "PartyLimit": cl.PartyLimit(1, 4),
        "QuestLimit": [
            cl.PreQuestAnd
        ],
        "NPC": cl.NPC('1028740', 'Ocuku_normal')
    }
"""

quests = {
    7570    : {
        "Memo": "Drakan quest",
        "QuestType": cl.EQuestType.QT_Normal_Day,
        "Reward": cl.Reward([
            cl.RewardExp(1940000),
            cl.RewardGold(30000),
            cl.RewardSelect(0, [
                cl.RewardItem(1, 940000730)
            ])
        ]),
        "Events": cl.Events([
            cl.EventMonster_IncParam(1, 1, 6063100, 123)
        ]),
        "LevelLimit": cl.LevelLimit(40, 99),
        "ClassLimit": cl.ClassLimit.UCLIMIT_PALADIN | cl.ClassLimit.UCLIMIT_NINJA,
        "DraClassLimit": cl.ClassLimit.UCLIMIT_SHAMAN | cl.ClassLimit.UCLIMIT_DOUBLE_FIGHTER,
        "TitleText": cl.TitleText(600564747),
        "PartyLimit": cl.PartyLimit(1, 4),
        "QuestLimit": [
            
        ],
        "NPC": cl.NPC('1028740', 'Ocuku_normal')
    }
}

for questId, quest in quests.items():
    questXmlPath = f'QUEST/QUEST{questId:08}.XML'
    questXml = f"""<?xml version="1.0" encoding="utf-8"?>
<QUEST>
	<CLIENTS>
		<CLIENT TYPE="NPC" EVENTNO="10000">2FF6590B-6648-4D73-9A61-7B4FA9065498</CLIENT>
	</CLIENTS>
	<AGENTS>
		<AGENT TYPE="NPC" EVENTNO="10002">2FF6590B-6648-4D73-9A61-7B4FA9065498</AGENT>
	</AGENTS>
	<PAYERS>
		<PAYER TYPE="NPC" EVENTNO="10003">2FF6590B-6648-4D73-9A61-7B4FA9065498</PAYER>
	</PAYERS>

    {quest['Events'].gen()}

    {quest['Reward'].gen(cl.RewardBase.XML)}

	<DIALOGS>
		<DIALOG ID="101" TYPE="PROLOG">
			<BODY TEXT="506611" FACE="HunterG_normal"/>
			<SELECT ID="10000" TYPE="ACCEPT" TEXT="500004"></SELECT>
			<SELECT ID="20000" TYPE="REJECT" TEXT="500005"></SELECT>
		</DIALOG>
		<DIALOG ID="401">
			<BODY TEXT="506613" FACE="HunterG_angly1"/>
			<SELECT ID="0" TEXT="500003"></SELECT>
		</DIALOG>
		<DIALOG ID="901" TYPE="INFO">
			<BODY TEXT="519427"/>
			<SELECT ID="0" TEXT="19933"></SELECT>
		</DIALOG>
		<DIALOG ID="902">
			<BODY TEXT="500124"/>
			<SELECT ID="0" TEXT="500003"></SELECT>
		</DIALOG>
		<DIALOG ID="903">
			<BODY TEXT="501835"/>
			<SELECT ID="0" TEXT="500003"></SELECT>
		</DIALOG>
		<DIALOG ID="905">
			<BODY TEXT="506849"/>
			<SELECT ID="0" TEXT="500003"></SELECT>
		</DIALOG>
	</DIALOGS>
</QUEST>
"""
    with open(os.path.join(MAP_XML_PATH, questXmlPath), 'w') as f:
        f.write(questXml)
