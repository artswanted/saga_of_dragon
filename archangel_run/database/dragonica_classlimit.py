import abc
import os
import re

UCLASS_FIGHTER = 1  # 전사0000 0000 0000 0002
UCLASS_MAGICIAN = 2  # 매지션0000 0000 0000 0004
UCLASS_ARCHER = 3  # 궁수0000 0000 0000 0008
UCLASS_THIEF = 4  # 도둑0000 0000 0000 0010
UCLASS_KNIGHT = 5  # 기사0000 0000 0000 0020
UCLASS_WARRIOR = 6  # 투사0000 0000 0000 0040
UCLASS_MAGE = 7  # 메이지0000 0000 0000 0080
UCLASS_BATTLEMAGE = 8  # 전투마법사0000 0000 0000 0100
UCLASS_HUNTER = 9  # 사냥꾼0000 0000 0000 0200
UCLASS_RANGER = 10  # 레인저0000 0000 0000 0400
UCLASS_CLOWN = 11  # 광대0000 0000 0000 0800
UCLASS_ASSASSIN = 12  # 암살자0000 0000 0000 1000
UCLASS_PALADIN = 13  # 성기사
UCLASS_GLADIATOR = 14  # 검투사
UCLASS_WIZARD = 15  # 위자드
UCLASS_WARMAGE = 16  # 워메이지
UCLASS_TRAPPER = 17  # 트랩퍼
UCLASS_SNIPER = 18  # 저격수
UCLASS_DANCER = 19  # 댄서
UCLASS_NINJA = 20  # 닌자
UCLASS_DRAOON = 21  # 용기사
UCLASS_DESTROYER = 22  # 파괴자
UCLASS_ARCHMAGE = 23  # 아크메이지
UCLASS_WARLORD = 24  # 워로드
UCLASS_SENTINEL = 25  # 수호자
UCLASS_LAUNCHER = 26  # 런처
UCLASS_MANIAC = 27  # 매니악
UCLASS_SHADOW = 28  # 새도우

# 신종족
UCLASS_SHAMAN = 51  # 샤먼
UCLASS_DOUBLE_FIGHTER = 52  # 더블 파이터
UCLASS_SUMMONER = 53  # 소환사
UCLASS_TWINS = 54  # 쌍둥이
UCLASS_BEAST_MASTER = 55  # 비스트 마스터
UCLASS_MIRAGE = 56  # 미라지
UCLASS_GAIA_MASTER = 57  # 가이아 마스터
UCLASS_DRAGON_FIGHTER = 58  # 드래곤파이터
UCLASS_MAX = 59  # 순서대로 정의하고 반드시 이걸 맨마지막에 정의할것

INT64_1 = 1

allNpcPath = os.path.join(os.path.dirname(__file__), "../../sfreedom_dev/XML/All_Npc.xml")

class ClassLimit:
    # 1차 Class
    UCLIMIT_FIGHTER = INT64_1 << UCLASS_FIGHTER
    UCLIMIT_MAGICIAN = INT64_1 << UCLASS_MAGICIAN
    UCLIMIT_ARCHER = INT64_1 << UCLASS_ARCHER
    UCLIMIT_THIEF = INT64_1 << UCLASS_THIEF

    # 2차 Class
    UCLIMIT_KNIGHT = INT64_1 << UCLASS_KNIGHT
    UCLIMIT_WARRIOR = INT64_1 << UCLASS_WARRIOR
    UCLIMIT_MAGE = INT64_1 << UCLASS_MAGE  # 메이지0000 0000 0000 0080
    UCLIMIT_BATTELMAGE = INT64_1 << UCLASS_BATTLEMAGE  # 전투마법사0000 0000 0000 0100
    UCLIMIT_HUNTER = INT64_1 << UCLASS_HUNTER  # 사냥꾼0000 0000 0000 0200
    UCLIMIT_RANGER = INT64_1 << UCLASS_RANGER  # 레인저0000 0000 0000 0400
    UCLIMIT_CLOWN = INT64_1 << UCLASS_CLOWN  # 광대0000 0000 0000 0800
    UCLIMIT_ASSASSIN = INT64_1 << UCLASS_ASSASSIN  # 암살자0000 0000 0000 1000

    # 3차 class(1)
    UCLIMIT_PALADIN = INT64_1 << UCLASS_PALADIN  # 성기사
    UCLIMIT_GLADIATOR = INT64_1 << UCLASS_GLADIATOR  # 검투사
    UCLIMIT_WIZARD = INT64_1 << UCLASS_WIZARD  # 위자드
    UCLIMIT_WARMAGE = INT64_1 << UCLASS_WARMAGE  # 워메이지
    UCLIMIT_TRAPPER = INT64_1 << UCLASS_TRAPPER  # 트랩퍼
    UCLIMIT_SNIPER = INT64_1 << UCLASS_SNIPER  # 저격수
    UCLIMIT_DANCER = INT64_1 << UCLASS_DANCER  # 댄서
    UCLIMIT_NINJA = INT64_1 << UCLASS_NINJA  # 닌자
    # 3차 class(2)
    UCLIMIT_DRAOON = INT64_1 << UCLASS_DRAOON  # 용기사
    UCLIMIT_DESTROYER = INT64_1 << UCLASS_DESTROYER  # 파괴자
    UCLIMIT_ARCHMAGE = INT64_1 << UCLASS_ARCHMAGE  # 아크메이지
    UCLIMIT_WARLORD = INT64_1 << UCLASS_WARLORD  # 워로드
    UCLIMIT_SENTINEL = INT64_1 << UCLASS_SENTINEL  # 수호자
    UCLIMIT_LAUNCHER = INT64_1 << UCLASS_LAUNCHER  # 런처
    UCLIMIT_MANIAC = INT64_1 << UCLASS_MANIAC  # 매니악
    UCLIMIT_SHADOW = INT64_1 << UCLASS_SHADOW  # 새도우

    # 신종족
    UCLIMIT_SHAMAN = INT64_1 << UCLASS_SHAMAN
    UCLIMIT_DOUBLE_FIGHTER = INT64_1 << UCLASS_DOUBLE_FIGHTER
    UCLIMIT_SUMMONER = INT64_1 << UCLASS_SUMMONER
    UCLIMIT_TWINS = INT64_1 << UCLASS_TWINS
    UCLIMIT_BEAST_MASTER = INT64_1 << UCLASS_BEAST_MASTER
    UCLIMIT_MIRAGE = INT64_1 << UCLASS_MIRAGE
    UCLIMIT_GAIA_MASTER = INT64_1 << UCLASS_GAIA_MASTER
    UCLIMIT_DRAGON_FIGHTER = INT64_1 << UCLASS_DRAGON_FIGHTER

class EQuestType:
    QT_None				= 0
    QT_Normal			= 1  # 보통 (NPC 얼굴이 나오는 기본 퀘스트, 현상수배 퀘스트도 이 타입)
    QT_Scenario			= 2  # 시나리오 퀘
    QT_ClassChange		= 3  # 전직 퀘 (Reward에 전직이 존재하면 자동으로 이 퀘스트가 된다)
    QT_Loop				= 4  # 반복 퀘 (퍼렁색 마크, 별도의 리스트 UI가 존재)
    QT_Day				= 5  # 1일 퀘스트 (1일 마다 리셋이 되는 퀘스트)
    QT_MissionQuest		= 6  # 미션 퀘스트 (사용하지 않음)
    QT_GuildTactics		= 7  # 길드 용병 퀘스트 (1일 마다 리셋이 되는 길드 경험치 퀘스트, 녹색 마크, 별도의 리스트 UI가 존재)
    QT_Couple			= 8  # 커플 들이 하는 퀘스트
    QT_SweetHeart		= 9  # 부부 들이 하는 퀘스트
    QT_Random			= 10  # 랜덤 퀘스트
    QT_RandomTactics	= 11  # 랜덤 용병 퀘스트
    QT_Soul				= 12  # Soul Quest
    QT_Wanted			= 13  # 현상수배 퀘스트( 하루마다 초기화 )	
    QT_Scroll			= 14
    QT_Week				= 15  # 주간퀘스트

    QT_Event_Normal		= 21  #기존 Type 1과 동일한 역할을 한다. QT_Normal
    QT_Event_Loop		= 24  #기존 Type 4와 동일한 역할을 한다. QT_Loop
    QT_Event_Normal_Day	= 25  #기존 Type 101과 동일한 역할을 한다. QT_Normal_Day

    QT_Normal_Day		= 101  # 일반 퀘스트(보통 퀘스트 UI에, 1일 마다 반복)

    QT_Dummy			= 99  # 더미 퀘스트들(퀘스트 분기 지점을 만들기 위한 데이타)
    QT_Deleted			= -1  # 삭제 예정 퀘스트

class RewardBase:
    DB = 1
    XML = 2

    def storage_type(self):
        raise NotImplementedError("")

    def gen(self):
        raise NotImplementedError("")

class Reward:
    def __init__(self, childs) -> None:
        self.childs = childs

    def _gen_xml(self):
        out = "<REWARD>\n"
        for child in self.childs:
            if child.storage_type() == RewardBase.XML:
                out += child.gen()
        out += "\t</REWARD>\n"
        return out

    def _gen_db(self):
        pass

    def gen(self, type):
        if type == RewardBase.DB:
            return self._gen_db()
        if type == RewardBase.XML:
            return self._gen_xml()
        raise ValueError(f"Invalid gen type: {type}")

class RewardDB(RewardBase):
    def storage_type(self):
        return self.DB


class RewardXML(RewardBase):
    def storage_type(self):
        return self.XML


class RewardExp(RewardDB):
    def __init__(self, exp) -> None:
        self.exp = exp

    def gen(self):
        return self.exp


class RewardGold(RewardDB):
    def __init__(self, gold) -> None:
        self.gold = gold

    def gen(self):
        return self.gold


class RewardSelect(RewardXML):
    def __init__(self, count, childs) -> None:
        self.count = count
        self.childs = childs

    
    def gen(self):
        if len(self.childs) == 0:
            return ""
        out = f'\t\t<SELECT1 COUNT="{self.count}">\n'
        for child in self.childs:
            out += child.gen()
        out += "\t\t</SELECT1>\n"
        return out


class RewardItem(RewardXML):
    def __init__(self, count, itemNo) -> None:
        self.count = count
        self.itemNo = itemNo

    def gen(self):
        return f'\t\t\t<ITEM COUNT="{self.count}">{self.itemNo}</ITEM>\n'


class LevelLimit():
    def __init__(self, min, max) -> None:
        self.min = min
        self.max = max

    def genMin(self):
        return self.min
    
    def genMax(self):
        return self.max


class TitleText():
    def __init__(self, title) -> None:
        self.title = title

    def gen(self):
        return self.title


class PreQuest():
    QAnd = 1
    QOr = 2
    QNot = 3

    def __init__(self, count, childs) -> None:
        self.count = count
        self.childs = childs


class PreQuestAnd(PreQuest):
    def column(self):
        return self.QAnd


class PreQuestOr(PreQuest):
    def column(self):
        return self.QOr


class PreQuestNot(PreQuest):
    def column(self):
        return self.QNot


class PartyLimit():
    def __init__(self, min, max) -> None:
        self.min = min
        self.max = max

    def genMin(self):
        return self.min
    
    def genMax(self):
        return self.max


class NPC():
    def __init__(self, name, face) -> None:
        self.name = name
        self.face = face

    
    def genID(self):
        with open(allNpcPath) as f:
            for line in f:
                if self.name in line:
                    return line[line.find('GUID="') + len('GUID="'): line.find('" KID')]
    

    def genFace(self):
        return self.face


class EventBase:
    @abc.abstractmethod
    def gen(self):
        raise NotImplementedError("Not impl")

class Events(EventBase):
    def __init__(self, childs) -> None:
        self.childs = childs

    def gen(self):
        events = []
        objects = []
        for child in self.childs:
            _event, _object = child.gen()
            if _event:
                events.append(_event)
            if _object:
                objects.append(_object)
        return "<EVENTS></EVENTS>"

class EventNpc(EventBase):
    def __init__(self, no, type, value, body, count, text):
        self.no = no
        self.type = type
        self.value = value
        self.body = body
        self.count = count
        self.text = text

    def gen(self):
        return f'<NPC OBJECTNO="{self.no}" TYPE="{self.type}" VALUE="{self.value}">{self.body}</NPC>', None

class EventMonster(EventBase):
    def __init__(self, no, type, value, body, count, text):
        self.no = no
        self.type = type
        self.value = value
        self.body = body
        self.count = count
        self.text = text

    def gen(self):
        return f'<MONSTER OBJECTNO="{self.no}" TYPE="{self.type}" VALUE="{self.value}">{self.body}</NPC>', f'<OBJECT{self.no} COUNT="{self.count}" TEXT="{self.text}"/>'

class EventMonster_IncParam(EventMonster):
    def __init__(self, no, count, map_no, text) -> None:
        super().__init__(no, "INCPARAM", count, map_no, count, text)
