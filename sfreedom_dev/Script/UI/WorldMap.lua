YX = -1
g_MapArrowX = 0-- 월드맵에서 내 위치 표시용 화살표 좌표
g_MapArrowY = 0-- 월드맵에서 내 위치 표시용 화살표 Y좌표. 안좋다 ㅜㅜ

MAP_PIECE_PATH = "../Data/6_ui/map/"                                                                                                                              		
ICON_BOSS_IMG_NAME 		= "mapiconBs.tga"
ICON_VILLAGE_IMG_NAME 	= "mapiconVlg.tga"
ICON_DEFAULT_IMG_NAME 	= "mapiconFd.tga"
ICON_DUNGEON_IMG_NAME 	= "mapiconDg.tga"
ICON_CHAOS_IMG_NAME		= "iconChaos.tga"
ICON_HIDDEN_IMG_NAME 	= "iconHd.tga"
ICON_NONE_IMG_NAME 		= "mapiconX.tga"
ICON_BIG_D_ARROW_IMG_NAME	= "mapIconAwB01.tga"
ICON_BIG_U_ARROW_IMG_NAME	= "mapIconAwB02.tga"
ICON_BIG_L_ARROW_IMG_NAME	= "mapIconAwB03.tga"
ICON_BIG_R_ARROW_IMG_NAME	= "mapIconAwB04.tga"
ICON_BIG_RD_ARROW_IMG_NAME	= "mapIconAwB05.tga"
ICON_BIG_LD_ARROW_IMG_NAME	= "mapIconAwB06.tga"
ICON_BIG_RU_ARROW_IMG_NAME	= "mapIconAwB07.tga"
ICON_BIG_LU_ARROW_IMG_NAME	= "mapIconAwB08.tga"
ICON_SMALL_D_ARROW_IMG_NAME	= "mapIconAwS01.tga"
ICON_SMALL_U_ARROW_IMG_NAME	= "mapIconAwS02.tga"
ICON_SMALL_R_ARROW_IMG_NAME	= "mapIconAwS03.tga"
ICON_SMALL_L_ARROW_IMG_NAME	= "mapIconAwS04.tga"
ICON_SMALL_RD_ARROW_IMG_NAME= "mapIconAwS05.tga"
ICON_SMALL_LD_ARROW_IMG_NAME= "mapIconAwS06.tga"
ICON_SMALL_RU_ARROW_IMG_NAME= "mapIconAwS07.tga"
ICON_SMALL_LU_ARROW_IMG_NAME= "mapIconAwS08.tga"

g_MapIconPath = {
					  ["VILLAGE"] 	= MAP_PIECE_PATH..ICON_VILLAGE_IMG_NAME
					, ["DEFAULT"] 	= MAP_PIECE_PATH..ICON_DEFAULT_IMG_NAME
					, ["BOSS"] 		= MAP_PIECE_PATH..ICON_BOSS_IMG_NAME
					, ["DUNGEON"] 	= MAP_PIECE_PATH..ICON_DUNGEON_IMG_NAME
					, ["CHAOS"] 	= MAP_PIECE_PATH..ICON_CHAOS_IMG_NAME
					, ["HIDDEN"] 	= MAP_PIECE_PATH..ICON_HIDDEN_IMG_NAME					
					, ["NONE"] 		= MAP_PIECE_PATH..ICON_NONE_IMG_NAME
}

g_Arrow={}
g_Arrow["PATH"] = {}
g_Arrow["PATH"] = {
					  ["BIG_D_ARROW"] 	= MAP_PIECE_PATH..ICON_BIG_D_ARROW_IMG_NAME
					, ["BIG_U_ARROW"] 	= MAP_PIECE_PATH..ICON_BIG_U_ARROW_IMG_NAME
					, ["BIG_L_ARROW"] 	= MAP_PIECE_PATH..ICON_BIG_L_ARROW_IMG_NAME
					, ["BIG_R_ARROW"] 	= MAP_PIECE_PATH..ICON_BIG_R_ARROW_IMG_NAME
					, ["BIG_LU_ARROW"] 	= MAP_PIECE_PATH..ICON_BIG_LU_ARROW_IMG_NAME
					, ["BIG_RU_ARROW"] 	= MAP_PIECE_PATH..ICON_BIG_RU_ARROW_IMG_NAME
					, ["BIG_LD_ARROW"] 	= MAP_PIECE_PATH..ICON_BIG_LD_ARROW_IMG_NAME
					, ["BIG_RD_ARROW"] 	= MAP_PIECE_PATH..ICON_BIG_RD_ARROW_IMG_NAME					
					, ["SMALL_D_ARROW"] 	= MAP_PIECE_PATH..ICON_SMALL_D_ARROW_IMG_NAME	
					, ["SMALL_U_ARROW"] 	= MAP_PIECE_PATH..ICON_SMALL_U_ARROW_IMG_NAME	
					, ["SMALL_L_ARROW"] 	= MAP_PIECE_PATH..ICON_SMALL_L_ARROW_IMG_NAME	
					, ["SMALL_R_ARROW"] 	= MAP_PIECE_PATH..ICON_SMALL_R_ARROW_IMG_NAME	
					, ["SMALL_LU_ARROW"] 	= MAP_PIECE_PATH..ICON_SMALL_LU_ARROW_IMG_NAME
					, ["SMALL_RU_ARROW"] 	= MAP_PIECE_PATH..ICON_SMALL_RU_ARROW_IMG_NAME
					, ["SMALL_LD_ARROW"] 	= MAP_PIECE_PATH..ICON_SMALL_LD_ARROW_IMG_NAME
					, ["SMALL_RD_ARROW"] 	= MAP_PIECE_PATH..ICON_SMALL_RD_ARROW_IMG_NAME
}
g_Arrow["SIZE"] = {}
g_Arrow["SIZE"] = {
					  ["BIG_D_ARROW"] 	= {80, 80}
					, ["BIG_U_ARROW"] 	= {80, 80}
					, ["BIG_L_ARROW"] 	= {80, 80}
					, ["BIG_R_ARROW"] 	= {80, 80}
					, ["BIG_LU_ARROW"] 	= {80, 80}
					, ["BIG_RU_ARROW"] 	= {80, 80}
					, ["BIG_LD_ARROW"] 	= {80, 80}
					, ["BIG_RD_ARROW"] 	= {80, 80}
					, ["SMALL_D_ARROW"] = {60, 60}
					, ["SMALL_U_ARROW"] = {60, 60}
					, ["SMALL_L_ARROW"] = {60, 60}
					, ["SMALL_R_ARROW"] = {60, 60}
					, ["SMALL_LU_ARROW"]= {60, 60}
					, ["SMALL_RU_ARROW"]= {60, 60}
					, ["SMALL_LD_ARROW"]= {60, 60}
					, ["SMALL_RD_ARROW"]= {60, 60}
}

--g_Continent["FRM_SILENTISLAND"] = { ["TITLE_TTW"] = 703, ["TITLE_X"] = 40, ["TITLE_Y"] = 35, ["UI_ID"]="FRM_SILENTISLAND", ["BG0"]="mapLn03_01.tga", ["BG1"]="mapLn03_02.tga", ["IMG_Y"]="15", ["MAP_IMG0"]="mapImg05.tga", ["MAP_IMG1"]="mapImg06.tga", ["NAMEID"] = 703 }

--대륙맵에 내 위치 표시를 위한 비율조정용 테이블
g_ContientMy = {}
g_ContientMy["WIND"] = {["MY_RATE"]=1.3, ["MY_X"]=200, ["MY_Y"]=55}
g_ContientMy["BEACH"] = {["MY_RATE"]=2.5, ["MY_X"]=67, ["MY_Y"]=60}
g_ContientMy["HERO"] = {["MY_RATE"]=2.2, ["MY_X"]=88, ["MY_Y"]=55}
g_ContientMy["LAVA"] = {["MY_RATE"]=2.1, ["MY_X"]=125, ["MY_Y"]=70}

g_ContientMy["KAZRA"] = {["MY_RATE"]=2.1, ["MY_X"]=100, ["MY_Y"]=80}
g_ContientMy["TIME"] = {["MY_RATE"]=3, ["MY_X"]=-10, ["MY_Y"]=20}
g_ContientMy["SPIRIT"] = {["MY_RATE"]=1.9, ["MY_X"]=95, ["MY_Y"]=95}
g_ContientMy["END"] = {["MY_RATE"]=1.7, ["MY_X"]=170, ["MY_Y"]=93}

--월드맵에 내 위치 표시를 위한 비율조정용 테이블
g_WorldMy = {}
g_WorldMy["WIND"] = {["MY_RATE"]=1.5, ["MY_X"]=-120, ["MY_Y"]=120}
g_WorldMy["BEACH"] = {["MY_RATE"]=2.7, ["MY_X"]=182, ["MY_Y"]=172}
g_WorldMy["HERO"] = {["MY_RATE"]=2.37, ["MY_X"]=88, ["MY_Y"]=65}
g_WorldMy["LAVA"] = {["MY_RATE"]=2.7, ["MY_X"]=200, ["MY_Y"]=35}

g_WorldMy["KAZRA"] = {["MY_RATE"]=2.3, ["MY_X"]=475, ["MY_Y"]=30}
g_WorldMy["TIME"] = {["MY_RATE"]=2.75, ["MY_X"]=630, ["MY_Y"]=180}
g_WorldMy["SPIRIT"] = {["MY_RATE"]=2.25, ["MY_X"]=545, ["MY_Y"]=285}
g_WorldMy["END"] = {["MY_RATE"]=1.9, ["MY_X"]=458, ["MY_Y"]=370}

--월드맵용 테이블
g_WorldMap = {}
g_WorldMap["DRA"] = {["TITLE_TTW"]=803000, ["BG"]="wmapNall0"}
g_WorldMap["DRA"][1] = {["X"]=72, ["Y"]=90, ["W"]=441, ["H"]=505, ["CONT"]="WIND", ["OVER1"]="wmapNallOv01.tga", ["OVER1_W"]=441, 
			["OVER1_H"]=505, ["NAME"]=803001, ["T_X"]=0, ["T_Y"]=-50}
g_WorldMap["DRA"][2] = {["X"]=539, ["Y"]=101, ["W"]=512, ["H"]=640, ["CONT"]="CHAOS", ["OVER1"]="wmapNallOv02_01.tga", ["OVER1_W"]=430,
			["OVER1_H"]=512, ["OVER2"]="wmapNallOv02_02.tga", ["OVER2_W"]=430, ["OVER2_H"]=94, ["OVER2_X"]=0, 
			["OVER2_Y"]=512, ["NAME"]=803002, ["T_X"]=0, ["T_Y"]=-10}

--대륙맵용 테이블
g_Continent = {}
g_Continent["WIND"] = {["TITLE_TTW"]=803001, ["BG"]="wmapNbaA0", ["OVER"]="wmapNbaOv0", ["WORLD"]="DRA", ["BTN"]="BTN_CONT1", ["T_X"]=0, ["T_Y"]=0}
g_Continent["WIND"][1] = {["Memo"]="바람평원",	["X"]=223,	["Y"]=192,	["W"]=325,	["H"]=512,	["GROUND"]="WIND",	["NAME"]=803100, ["T_X"]=-30, ["T_Y"]=50}
g_Continent["WIND"][2] = {["Memo"]="해안지대",	["X"]=466,	["Y"]=265,	["W"]=261,	["H"]=192,	["GROUND"]="BEACH",	["NAME"]=803101, ["T_X"]=-20, ["T_Y"]=0}
g_Continent["WIND"][3] = {["Memo"]="영웅신전",	["X"]=363,	["Y"]=125,	["W"]=277,	["H"]=235,	["GROUND"]="HERO",	["NAME"]=803102, ["T_X"]=0, ["T_Y"]=0}
g_Continent["WIND"][4] = {["Memo"]="용암지대",	["X"]=501,	["Y"]=77,	["W"]=265,	["H"]=225,	["GROUND"]="LAVA",	["NAME"]=803103, ["T_X"]=-30, ["T_Y"]=-30}

g_Continent["CHAOS"] = {["TITLE_TTW"]=803002, ["BG"]="wmapNhonA0", ["OVER"]="wmapNhonOv0", ["WORLD"]="DRA", ["1ST"]=9050120, ["BTN"]="BTN_CONT2", ["T_X"]=0, ["T_Y"]=0}
g_Continent["CHAOS"][1] = {["Memo"]="카즈라",	["X"]=401,	["Y"]=65,	["W"]=295,	["H"]=251,	["GROUND"]="KAZRA",	["NAME"]=803104, ["T_X"]=-40, ["T_Y"]=-40}
g_Continent["CHAOS"][2] = {["Memo"]="고비구릉",	["X"]=501,	["Y"]=181,	["W"]=305,	["H"]=268,	["GROUND"]="TIME",	["NAME"]=803105, ["T_X"]=0, ["T_Y"]=0}
g_Continent["CHAOS"][3] = {["Memo"]="원령산맥",	["X"]=445,	["Y"]=353,	["W"]=363,	["H"]=295,	["GROUND"]="SPIRIT",	["NAME"]=803106, ["T_X"]=20, ["T_Y"]=-30}
g_Continent["CHAOS"][4] = {["Memo"]="세상의끝",	["X"]=416,	["Y"]=469,	["W"]=287,	["H"]=284,	["GROUND"]="END",	["NAME"]=803107, ["T_X"]=20, ["T_Y"]=20}

--대륙타이틀
g_Ground = {}
g_Ground["WIND"] = {["TITLE_TTW"]=803100, ["CONT"]="WIND", ["BG"]="wmapNba01_0", ["LINE_IMG"]="wmapNba01_Ln.tga", ["IMG_X"]=400,
			["IMG_Y"]=175, ["IMG_W"]=339, ["IMG_H"]=326}
g_Ground["BEACH"] = {["TITLE_TTW"]=803101, ["CONT"]="WIND", ["BG"]="wmapNba02_0", ["LINE_IMG"]="wmapNba02_Ln.tga", ["IMG_X"]=360,
			["IMG_Y"]=130, ["IMG_W"]=504, ["IMG_H"]=494, ["OVER"]="wmapNbaIcon03_bg.tga", ["OVER_X"]=393, ["OVER_Y"]=336,
			["OVER_W"]=153, ["OVER_H"]=290}
g_Ground["HERO"] = {["TITLE_TTW"]=803102, ["CONT"]="WIND", ["BG"]="wmapNba03_0", ["LINE_IMG"]="wmapNba03_Ln.tga", ["IMG_X"]=320,
			["IMG_Y"]=150, ["IMG_W"]=356, ["IMG_H"]=467}
g_Ground["LAVA"] = {["TITLE_TTW"]=803103, ["CONT"]="WIND", ["BG"]="wmapNba04_0", ["LINE_IMG"]="wmapNba04_Ln.tga", ["IMG_X"]=270,
			["IMG_Y"]=160, ["IMG_W"]=419, ["IMG_H"]=400}

g_Ground["KAZRA"] = {["TITLE_TTW"]=803104, ["CONT"]="CHAOS", ["BG"]="wmapNhon01_0", ["LINE_IMG"]="wmapNhon01_Ln.tga", ["IMG_X"]=375,
			["IMG_Y"]=160, ["IMG_W"]=401, ["IMG_H"]=500}--카즈라
g_Ground["TIME"] = {["TITLE_TTW"]=803105, ["CONT"]="CHAOS", ["BG"]="wmapNhon02_0", ["LINE_IMG"]="wmapNhon02_Ln.tga", ["IMG_X"]=312,
			["IMG_Y"]=80, ["IMG_W"]=408, ["IMG_H"]=479}--고비구릉
g_Ground["SPIRIT"] = {["TITLE_TTW"]=803106, ["CONT"]="CHAOS", ["BG"]="wmapNhon03_0", ["LINE_IMG"]="wmapNhon03_Ln.tga", ["IMG_X"]=365,
			["IMG_Y"]=120, ["IMG_W"]=412, ["IMG_H"]=511}--원령산맥
g_Ground["END"] = {["TITLE_TTW"]=803107, ["CONT"]="CHAOS", ["BG"]="wmapNhon04_0", ["LINE_IMG"]="wmapNhon04_Ln.tga", ["IMG_X"]=335,
			["IMG_Y"]=290, ["IMG_W"]=394, ["IMG_H"]=278}--세상의끝

--각 맵 정보들
g_GroundPos = {}
g_GroundPos[9018200] = { ["CONT"]="WIND",	["KEY"]=1,	["TTW"]=801000}	--종소리
g_GroundPos[9010450] = { ["CONT"]="WIND",	["KEY"]=2,	["TTW"]=802040}	--칼바람 늑대소굴
g_GroundPos[9010460] = { ["CONT"]="WIND",	["KEY"]=3,	["TTW"]=802041}	--통곡고개 늑대소굴
g_GroundPos[9010410] = { ["CONT"]="WIND",	["KEY"]=4,	["TTW"]=800007}	--칼바람
g_GroundPos[9010420] = { ["CONT"]="WIND",	["KEY"]=5,	["TTW"]=800008}	--통곡고개
g_GroundPos[9010400] = { ["CONT"]="WIND",	["KEY"]=6,	["TTW"]=800005}	--배신자고개
g_GroundPos[9010430] = { ["CONT"]="WIND",	["KEY"]=7,	["TTW"]=800006}	--망각협곡
g_GroundPos[9010300] = { ["CONT"]="WIND",	["KEY"]=8,	["TTW"]=800001}	--용자의길
g_GroundPos[9010330] = { ["CONT"]="WIND",	["KEY"]=9,	["TTW"]=800002}	--숙련된용자의길

g_GroundPos[9018100] = { ["CONT"]="BEACH",	["KEY"]=1,	["TTW"]=801001}	--바람항구
g_GroundPos[9010502] = { ["CONT"]="BEACH",	["KEY"]=2,	["TTW"]=802000}	--지하수로
g_GroundPos[9010701] = { ["CONT"]="BEACH",	["KEY"]=3,	["TTW"]=802034}	--물의신전
g_GroundPos[9010100] = { ["CONT"]="BEACH",	["KEY"]=4,	["TTW"]=800011}	--수염고래
g_GroundPos[9010110] = { ["CONT"]="BEACH",	["KEY"]=5,	["TTW"]=800012}	--톱날꽃게
g_GroundPos[9010130] = { ["CONT"]="BEACH",	["KEY"]=6,	["TTW"]=800014}	--스티븐
g_GroundPos[9010120] = { ["CONT"]="BEACH",	["KEY"]=7,	["TTW"]=800013}	--갑판상어
g_GroundPos[9010150] = { ["CONT"]="BEACH",	["KEY"]=8,	["TTW"]=800016}	--칼리곤
g_GroundPos[9010140] = { ["CONT"]="BEACH",	["KEY"]=9,	["TTW"]=800015}	--앵무조개
g_GroundPos[9010503] = { ["PMAP_NO"]=9010502}	--파렐 대저택
g_GroundPos[9010709] = { ["PMAP_NO"]=9010701}	--엔다이론

g_GroundPos[9018310] = { ["CONT"]="HERO",	["KEY"]=1,	["TTW"]=801010}	--달빛해안
g_GroundPos[9010910] = { ["CONT"]="HERO",	["KEY"]=2,	["TTW"]=802043}	--지하묘지
g_GroundPos[9010801] = { ["CONT"]="HERO",	["KEY"]=3,	["TTW"]=802037}	--망자의탑
g_GroundPos[9010230] = { ["CONT"]="HERO",	["KEY"]=4,	["TTW"]=800021}	--팔미르
g_GroundPos[9010220] = { ["CONT"]="HERO",	["KEY"]=5,	["TTW"]=800020}	--벨칸
g_GroundPos[9010200] = { ["CONT"]="HERO",	["KEY"]=6,	["TTW"]=800018}	--미리내
g_GroundPos[9010210] = { ["CONT"]="HERO",	["KEY"]=7,	["TTW"]=800019}	--스카이피
g_GroundPos[9010610] = { ["CONT"]="HERO",	["KEY"]=8,	["TTW"]=800027}	--지하수로
g_GroundPos[9010600] = { ["CONT"]="HERO",	["KEY"]=9,	["TTW"]=800026}	--위험동굴
g_GroundPos[9010620] = { ["CONT"]="HERO",	["KEY"]=10,	["TTW"]=800028}	--채굴지대
g_GroundPos[9010920] = { ["PMAP_NO"]=9010910}	--지하묘지
g_GroundPos[9010809] = { ["PMAP_NO"]=9010801}	--영웅제단

g_GroundPos[9018320] = { ["CONT"]="LAVA",	["KEY"]=1,	["TTW"]=801002}	--상인마을
g_GroundPos[9020201] = { ["CONT"]="LAVA",	["KEY"]=2,	["TTW"]=802002}	--마그마
g_GroundPos[9020130] = { ["CONT"]="LAVA",	["KEY"]=3,	["TTW"]=800033}	--화암산호
g_GroundPos[9020100] = { ["CONT"]="LAVA",	["KEY"]=4,	["TTW"]=800034}	--악마숨결
g_GroundPos[9020110] = { ["CONT"]="LAVA",	["KEY"]=5,	["TTW"]=800031}	--심연열기
g_GroundPos[9020300] = { ["CONT"]="LAVA",	["KEY"]=6,	["TTW"]=800040}	--이글늪
g_GroundPos[9020320] = { ["CONT"]="LAVA",	["KEY"]=7,	["TTW"]=800042}	--갈대슾지
g_GroundPos[9020310] = { ["CONT"]="LAVA",	["KEY"]=8,	["TTW"]=800041}	--마른나무
g_GroundPos[9020200] = { ["PMAP_NO"]=9020201}	--라발론

g_GroundPos[9058100] = { ["CONT"]="KAZRA",	["KEY"]=1,	["TTW"]=801012}	--카즈라
g_GroundPos[9020601] = { ["CONT"]="KAZRA",	["KEY"]=2,	["TTW"]=802004, ["BOSS"]=9020600} --쿤다라
g_GroundPos[9020701] = { ["CONT"]="KAZRA",	["KEY"]=3,	["TTW"]=802045}	--신록의숲
g_GroundPos[9050120] = { ["CONT"]="KAZRA",	["KEY"]=4,	["TTW"]=800074}	--에르다
g_GroundPos[9050100] = { ["CONT"]="KAZRA",	["KEY"]=5,	["TTW"]=800075}	--샬바론
g_GroundPos[9050301] = { ["CONT"]="KAZRA",	["KEY"]=6,	["TTW"]=802047}	--시험동굴
g_GroundPos[9050110] = { ["CONT"]="KAZRA",	["KEY"]=7,	["TTW"]=800076}	--라미에타
g_GroundPos[9020420] = { ["CONT"]="KAZRA",	["KEY"]=8,	["TTW"]=800049}	--안개내린
g_GroundPos[9020430] = { ["CONT"]="KAZRA",	["KEY"]=9,	["TTW"]=800047}	--솔방울
g_GroundPos[9020510] = { ["CONT"]="KAZRA",	["KEY"]=10,	["TTW"]=800051}	--무너진
g_GroundPos[9020500] = { ["CONT"]="KAZRA",	["KEY"]=11,	["TTW"]=800050}	--신들의
g_GroundPos[9020530] = { ["CONT"]="KAZRA",	["KEY"]=12,	["TTW"]=800053}	--케이론
g_GroundPos[9050300] = { ["PMAP_NO"]=9050301}	--아카나이
g_GroundPos[9020700] = { ["PMAP_NO"]=9020701}	--신록의제단
g_GroundPos[9020600] = { ["PMAP_NO"]=9020601}	--아람

g_GroundPos[9028100] = { ["CONT"]="TIME",	["KEY"]=1,	["TTW"]=801003}	--버섯마을 

g_GroundPos[9030110] = { ["CONT"]="TIME",	["KEY"]=2,	["TTW"]=800056}	--투투꽃
g_GroundPos[9030140] = { ["CONT"]="TIME",	["KEY"]=3,	["TTW"]=800058}	--부서진 버섯
g_GroundPos[9030100] = { ["CONT"]="TIME",	["KEY"]=4,	["TTW"]=800055}	--잊혀진 마을
g_GroundPos[9030130] = { ["CONT"]="TIME",	["KEY"]=5,	["TTW"]=800059}	--잠자는 마을

g_GroundPos[9030210] = { ["CONT"]="TIME",	["KEY"]=6,	["TTW"]=800061}	--눈보라고개
g_GroundPos[9030200] = { ["CONT"]="TIME",	["KEY"]=7,	["TTW"]=800060}	--유혹설원 
g_GroundPos[9030230] = { ["CONT"]="TIME",	["KEY"]=8,	["TTW"]=800063}	--얼음협곡
g_GroundPos[9030240] = { ["CONT"]="TIME",	["KEY"]=9,	["TTW"]=800064}	--폭풍설원
g_GroundPos[9030220] = { ["CONT"]="TIME",	["KEY"]=10,	["TTW"]=800062}	--얼음호수
g_GroundPos[9050610] = { ["CONT"]="TIME",	["KEY"]=11,	["TTW"]=800080}	--바티카 구릉
g_GroundPos[9050620] = { ["CONT"]="TIME",	["KEY"]=12,	["TTW"]=800081}	--벤시협곡
g_GroundPos[9050601] = { ["CONT"]="TIME",	["KEY"]=13,	["TTW"]=802053}	--붉은여우
g_GroundPos[9050600] = { ["CONT"]="TIME",	["KEY"]=14, ["TTW"]=802054}	--하이드론코어

--g_GroundPos[9050501] = { ["CONT"]="TIME",	["KEY"]=2,	["TTW"]=802049}	--모래사원
--g_GroundPos[9030601] = { ["CONT"]="TIME",	["KEY"]=4,	["TTW"]=802051}	--얼음사원
--g_GroundPos[9050520] = { ["CONT"]="TIME",	["KEY"]=5,	["TTW"]=800078}	--먼지폭풍
--g_GroundPos[9050510] = { ["CONT"]="TIME",	["KEY"]=6,	["TTW"]=800077}	--신기루 언덕
--g_GroundPos[9050530] = { ["CONT"]="TIME",	["KEY"]=7,	["TTW"]=800079}	--고비 황무지
--g_GroundPos[9050500] = { ["PMAP_NO"]=9050501}	--생명의제단
--g_GroundPos[9030600] = { ["PMAP_NO"]=9030601}	--달의제단

g_GroundPos[9038300] = { ["CONT"]="SPIRIT",	["KEY"]=1,	["TTW"]=801009}	--잿빛안개
g_GroundPos[9030599] = { ["CONT"]="SPIRIT",	["KEY"]=2,	["TTW"]=802006, ["BOSS"]=9030500} --반클리프
g_GroundPos[9030830] = { ["CONT"]="SPIRIT",	["KEY"]=3,	["TTW"]=800073}	--얼음바위 구릉
g_GroundPos[9030810] = { ["CONT"]="SPIRIT",	["KEY"]=4,	["TTW"]=800071}	--살얼음 고개
g_GroundPos[9030800] = { ["CONT"]="SPIRIT",	["KEY"]=5,	["TTW"]=800070}	--눈구름 골짜기
g_GroundPos[9030820] = { ["CONT"]="SPIRIT",	["KEY"]=6,	["TTW"]=800072}	--곰발톱 광산
g_GroundPos[9030730] = { ["CONT"]="SPIRIT",	["KEY"]=7,	["TTW"]=800069}	--설원의 묘역
g_GroundPos[9030710] = { ["CONT"]="SPIRIT",	["KEY"]=8,	["TTW"]=800067}	--하얀추적자 언덕
g_GroundPos[9030700] = { ["CONT"]="SPIRIT",	["KEY"]=9,	["TTW"]=800066}	--원령나무 숲
g_GroundPos[9030720] = { ["CONT"]="SPIRIT",	["KEY"]=10,	["TTW"]=800068}	--영혼의 안식처

g_GroundPos[9030501] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030502] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030503] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030504] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030505] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030506] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030507] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030508] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030509] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030510] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030511] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030512] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030513] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030514] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030515] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030516] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030517] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030518] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030519] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030520] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030521] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030522] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030523] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030524] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030525] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030526] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030527] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030528] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030529] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030530] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030531] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030532] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030533] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030534] = { ["PMAP_NO"]=9030599}	--반클리프
g_GroundPos[9030535] = { ["PMAP_NO"]=9030599}	g_GroundPos[9030500] = { ["PMAP_NO"]=9030599}	--반클리프

g_GroundPos[9038200] = { ["CONT"]="END",	["KEY"]=1,	["TTW"]=801004}	--엘로라
g_GroundPos[9030301] = { ["CONT"]="END",	["KEY"]=2,	["TTW"]=802039}	--대미궁
g_GroundPos[9050401] = { ["CONT"]="END",	["KEY"]=3,	["TTW"]=802055}	--검은발톱둥지
g_GroundPos[9050220] = { ["CONT"]="END",	["KEY"]=4,	["TTW"]=800083}	--날개협곡
g_GroundPos[9050200] = { ["CONT"]="END",	["KEY"]=5,	["TTW"]=800082}	--드래곤계곡
g_GroundPos[9050210] = { ["CONT"]="END",	["KEY"]=6,	["TTW"]=800084}	--손톱날고개
g_GroundPos[9050230] = { ["CONT"]="END",	["KEY"]=7,	["TTW"]=800085}	--세상의끝
g_GroundPos[9050400] = { ["PMAP_NO"]=9050401}	--엘가
g_GroundPos[9050402] = { ["PMAP_NO"]=9050401}	--엘가

g_GroundPos[9030300] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030302] = { ["PMAP_NO"]=9030301}	--대미궁
g_GroundPos[9030303] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030304] = { ["PMAP_NO"]=9030301}
g_GroundPos[9030305] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030306] = { ["PMAP_NO"]=9030301}
g_GroundPos[9030307] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030308] = { ["PMAP_NO"]=9030301}
g_GroundPos[9030309] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030310] = { ["PMAP_NO"]=9030301}
g_GroundPos[9030311] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030312] = { ["PMAP_NO"]=9030301}
g_GroundPos[9030313] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030314] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030315] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030316] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030317] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030318] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030319] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030320] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030321] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030322] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030323] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030324] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030325] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030326] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030327] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030328] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030329] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030330] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030331] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030332] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030333] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030334] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030335] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030336] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030337] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030338] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030339] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030340] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030341] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030342] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030343] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030344] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030345] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030346] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030347] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030348] = { ["PMAP_NO"]=9030301}	
g_GroundPos[9030349] = { ["PMAP_NO"]=9030301}	g_GroundPos[9030350] = { ["PMAP_NO"]=9030301}	

g_GroundList = {}
g_GroundList["WIND"] = {}
g_GroundList["WIND"][1] = {["Memo"]="종소리",	["X"]=465,	["Y"]=385,	["MAP_NO"]=9018200,	["TYPE"]="VILLAGE", ["ICON"]="wmapNbaIcon01.tga", ["ICON_W"]=160, ["ICON_H"]=120, ["T_Y"]=55}
g_GroundList["WIND"][8] = {["Memo"]="용자의길",	["X"]=428,	["Y"]=473,	["MAP_NO"]=9010300,	["TYPE"]="DEFAULT"} 
g_GroundList["WIND"][9] = {["Memo"]="숙련된",	["X"]=367,	["Y"]=427,	["MAP_NO"]=9010330,	["TYPE"]="DEFAULT"}
g_GroundList["WIND"][6] = {["Memo"]="배신자",	["X"]=420,	["Y"]=325,	["MAP_NO"]=9010400,	["TYPE"]="DEFAULT"} 
g_GroundList["WIND"][7] = {["Memo"]="망각협곡",	["X"]=539,	["Y"]=375,	["MAP_NO"]=9010430,	["TYPE"]="DEFAULT"}
g_GroundList["WIND"][4] = {["Memo"]="칼바람",	["X"]=432,	["Y"]=250,	["MAP_NO"]=9010410,	["TYPE"]="DEFAULT"} 
g_GroundList["WIND"][5] = {["Memo"]="통곡고개",	["X"]=540,	["Y"]=275,	["MAP_NO"]=9010420,	["TYPE"]="DEFAULT"}
g_GroundList["WIND"][2] = {["Memo"]="칼바람늑",	["X"]=418,	["Y"]=145,	["MAP_NO"]=9010450,	["TYPE"]="DUNGEON", ["ICON"]="wmapNbaIcon02.tga", ["ICON_W"]=140, ["ICON_H"]=100, ["T_Y"]=45}
g_GroundList["WIND"][3] = {["Memo"]="통곡늑대",	["X"]=560,	["Y"]=150,	["MAP_NO"]=9010460,	["TYPE"]="DUNGEON", ["ICON"]="wmapNbaIcon02.tga", ["ICON_W"]=140, ["ICON_H"]=100, ["T_Y"]=45}
g_Arrow["WIND"] = {}
g_Arrow["WIND"][1] = { ["Memo"]="바람평원->해안지대", ["X"]=676, ["Y"]=225, ["TYPE"]="BIG_RU_ARROW", ["TARGET_LOCAL_MAP"] = "BEACH", ["TTW"] =803201, ["T_X"]=18, ["T_Y"]=35}


g_GroundList["BEACH"] = {}
g_GroundList["BEACH"][1] = { ["Memo"]="바람항구",	["X"]=415,	["Y"]=330,	["MAP_NO"]=9018100, ["TYPE"]="VILLAGE", ["ICON"]="wmapNbaIcon03.tga", ["ICON_W"]=200, ["ICON_H"]=260, ["T_X"]=60, ["T_Y"]=130} 
g_GroundList["BEACH"][2] = { ["Memo"]="지하수로",	["X"]=340,	["Y"]=400,	["MAP_NO"]=9010502, ["TYPE"]="DUNGEON", ["ICON"]="wmapNbaIcon04.tga", ["ICON_W"]=140, ["ICON_H"]=100, ["T_X"]=30, ["T_Y"]="45"} 
g_GroundList["BEACH"][3] = { ["Memo"]="물의신전",	["X"]=457,	["Y"]=280,	["MAP_NO"]=9010701, ["TYPE"]="DUNGEON", ["ICON"]="wmapNbaIcon05.tga", ["ICON_W"]=120, ["ICON_H"]=140, ["T_X"]=30, ["T_Y"]=-23} 
g_GroundList["BEACH"][4] = { ["Memo"]="수염고래",	["X"]=552,	["Y"]=443,	["MAP_NO"]=9010100, ["TYPE"]="DEFAULT"} 
g_GroundList["BEACH"][5] = { ["Memo"]="톱날꽃게",	["X"]=630,	["Y"]=423,	["MAP_NO"]=9010110, ["TYPE"]="DEFAULT"} 
g_GroundList["BEACH"][6] = { ["Memo"]="스티븐",		["X"]=570,	["Y"]=325,	["MAP_NO"]=9010130, ["TYPE"]="DEFAULT"} 
g_GroundList["BEACH"][7] = { ["Memo"]="갑판상어",	["X"]=730,	["Y"]=342,	["MAP_NO"]=9010120, ["TYPE"]="DEFAULT"} 
g_GroundList["BEACH"][8] = { ["Memo"]="칼리곤",		["X"]=675,	["Y"]=255,	["MAP_NO"]=9010150, ["TYPE"]="DEFAULT"} 
g_GroundList["BEACH"][9] = { ["Memo"]="앵무조개",	["X"]=817,	["Y"]=245,	["MAP_NO"]=9010140, ["TYPE"]="DEFAULT"} 
g_Arrow["BEACH"] = {}
g_Arrow["BEACH"][1] = { ["Memo"]="해안지대->영웅신전", ["X"]=717, ["Y"]=135, ["TYPE"]="BIG_U_ARROW",  ["TARGET_LOCAL_MAP"] = "HERO", ["TTW"] =803203, ["T_X"]=18, ["T_Y"]=35}
g_Arrow["BEACH"][2] = { ["Memo"]="해안지대->바람평원", ["X"]=413, ["Y"]=600, ["TYPE"]="BIG_LD_ARROW", ["TARGET_LOCAL_MAP"] = "WIND", ["TTW"] =803202, ["T_X"]=18, ["T_Y"]=35}

g_GroundList["HERO"] = {}
g_GroundList["HERO"][1] = {["Memo"]="달빛해안",	["X"]=520,	["Y"]=480,	["MAP_NO"]=9018310,	["TYPE"]="VILLAGE", ["ICON"]="wmapNbaIcon06.tga", ["ICON_W"]=200, ["ICON_H"]=160, ["T_Y"]=70}
g_GroundList["HERO"][2] = {["Memo"]="지하묘지",	["X"]=500,	["Y"]=555,	["MAP_NO"]=9010910,	["TYPE"]="DUNGEON", ["ICON"]="wmapNbaIcon07.tga", ["ICON_W"]=140, ["ICON_H"]=140, ["T_Y"]=60}
g_GroundList["HERO"][3] = {["Memo"]="망자의탑",	["X"]=420,	["Y"]=195,	["MAP_NO"]=9010801,	["TYPE"]="DUNGEON", ["ICON"]="wmapNbaIcon08.tga", ["ICON_W"]=160, ["ICON_H"]=160, ["T_Y"]=75}
g_GroundList["HERO"][4] = {["Memo"]="팔미르",	["X"]=465,	["Y"]=290,	["MAP_NO"]=9010230,	["TYPE"]="DEFAULT"} 
g_GroundList["HERO"][5] = {["Memo"]="벨칸신전",	["X"]=295,	["Y"]=320,	["MAP_NO"]=9010220,	["TYPE"]="DEFAULT"} 
g_GroundList["HERO"][6] = {["Memo"]="미리내",	["X"]=337,	["Y"]=420,	["MAP_NO"]=9010200,	["TYPE"]="DEFAULT"} 
g_GroundList["HERO"][7] = {["Memo"]="스카이피",	["X"]=425,	["Y"]=360,	["MAP_NO"]=9010210,	["TYPE"]="DEFAULT"}
g_GroundList["HERO"][8] = {["Memo"]="지하수로",	["X"]=625,	["Y"]=462,	["MAP_NO"]=9010610,	["TYPE"]="DEFAULT"}
g_GroundList["HERO"][9] = {["Memo"]="위험동굴",	["X"]=540,	["Y"]=425,	["MAP_NO"]=9010600,	["TYPE"]="DEFAULT"}
g_GroundList["HERO"][10] = {["Memo"]="채굴",	["X"]=538,	["Y"]=333,	["MAP_NO"]=9010620,	["TYPE"]="DEFAULT"}
g_Arrow["HERO"] = {}
g_Arrow["HERO"][1] = { ["Memo"]="영웅신전->용암지대", ["X"]=534, ["Y"]=190, ["TYPE"]="BIG_U_ARROW",  ["TARGET_LOCAL_MAP"] = "LAVA", ["TTW"] =803205, ["T_X"]=18, ["T_Y"]=35}
g_Arrow["HERO"][2] = { ["Memo"]="영웅신전->해안지대", ["X"]=643, ["Y"]=565, ["TYPE"]="BIG_RD_ARROW", ["TARGET_LOCAL_MAP"] = "BEACH", ["TTW"] =803204, ["T_X"]=18, ["T_Y"]=35}

g_GroundList["LAVA"] = {}
g_GroundList["LAVA"][1] = {["Memo"]="상인마을",	["X"]=395,	["Y"]=375,	["MAP_NO"]=9018320,	["TYPE"]="DEFAULT", ["ICON"]="wmapNbaIcon09.tga", ["ICON_W"]=220, ["ICON_H"]=140, ["T_Y"]=62}
g_GroundList["LAVA"][2] = {["Memo"]="마그마던",	["X"]=301,	["Y"]=177,	["MAP_NO"]=9020201,	["TYPE"]="DUNGEON", ["ICON"]="wmapNbaIcon10.tga", ["ICON_W"]=140, ["ICON_H"]=160, ["T_X"]=45, ["T_Y"]=75}
g_GroundList["LAVA"][3] = {["Memo"]="화암산호",	["X"]=415,	["Y"]=210,	["MAP_NO"]=9020130,	["TYPE"]="DEFAULT"} 
g_GroundList["LAVA"][4] = {["Memo"]="악마숨결",	["X"]=365,	["Y"]=310,	["MAP_NO"]=9020100,	["TYPE"]="DEFAULT"} 
g_GroundList["LAVA"][5] = {["Memo"]="심연열기",	["X"]=240,	["Y"]=290,	["MAP_NO"]=9020110,	["TYPE"]="DEFAULT"} 
g_GroundList["LAVA"][6] = {["Memo"]="이글늪",	["X"]=475,	["Y"]=430,	["MAP_NO"]=9020300,	["TYPE"]="DEFAULT"}
g_GroundList["LAVA"][7] = {["Memo"]="갈대슾지",	["X"]=600,	["Y"]=435,	["MAP_NO"]=9020320,	["TYPE"]="DEFAULT"}
g_GroundList["LAVA"][8] = {["Memo"]="마른나무",	["X"]=455,	["Y"]=535,	["MAP_NO"]=9020310,	["TYPE"]="DEFAULT"}
g_Arrow["LAVA"] = {}
g_Arrow["LAVA"][1] = { ["Memo"]="용암지대->카즈라", ["X"]=593, ["Y"]=187, ["TYPE"]="BIG_RU_ARROW",  ["TARGET_LOCAL_MAP"] = "KAZRA", ["TTW"] =803207, ["T_X"]=18, ["T_Y"]=35}
g_Arrow["LAVA"][2] = { ["Memo"]="용암지대->영웅신전", ["X"]=320, ["Y"]=490, ["TYPE"]="BIG_LD_ARROW", ["TARGET_LOCAL_MAP"] = "HERO", ["TTW"] =803206, ["T_X"]=18, ["T_Y"]=35}

g_GroundList["KAZRA"] = {}
g_GroundList["KAZRA"][1] = {["Memo"]="카즈라",	["X"]=557,	["Y"]=258,	["MAP_NO"]=9058100,	["TYPE"]="VILLAGE", ["ICON"]="wmapNhonIcon01.tga", ["ICON_W"]=120, ["ICON_H"]=100, ["T_Y"]=45}
g_GroundList["KAZRA"][2] = {["Memo"]="쿤다라",	["X"]=695,	["Y"]=320,	["MAP_NO"]=9020601,	["TYPE"]="DUNGEON", ["ICON"]="wmapNhonIcon02.tga", ["ICON_W"]=140, ["ICON_H"]=140, ["T_Y"]=60}
g_GroundList["KAZRA"][3] = {["Memo"]="신록의",	["X"]=340,	["Y"]=375,	["MAP_NO"]=9020701,	["TYPE"]="DUNGEON", ["ICON"]="wmapNhonIcon03.tga", ["ICON_W"]=160, ["ICON_H"]=120, ["T_Y"]=55}
g_GroundList["KAZRA"][4] = {["Memo"]="에르다",	["X"]=495,	["Y"]=230,	["MAP_NO"]=9050120,	["TYPE"]="DEFAULT"} 
g_GroundList["KAZRA"][5] = {["Memo"]="샬바론",	["X"]=620,	["Y"]=253,	["MAP_NO"]=9050100,	["TYPE"]="DEFAULT"} 
g_GroundList["KAZRA"][6] = {["Memo"]="아키아",	["X"]=520,	["Y"]=305,	["MAP_NO"]=9050301,	["TYPE"]="DUNGEON", ["ICON"]="wmapNhonIcon13.tga", ["ICON_W"]=120, ["ICON_H"]=130, ["T_Y"]=57} 
g_GroundList["KAZRA"][7] = {["Memo"]="라미에",	["X"]=580,	["Y"]=370,	["MAP_NO"]=9050110,	["TYPE"]="DEFAULT"}
g_GroundList["KAZRA"][8] = {["Memo"]="안개내",	["X"]=416,	["Y"]=434,	["MAP_NO"]=9020420,	["TYPE"]="DEFAULT"}
g_GroundList["KAZRA"][9] = {["Memo"]="솔방울",	["X"]=503,	["Y"]=427,	["MAP_NO"]=9020430,	["TYPE"]="DEFAULT"}
g_GroundList["KAZRA"][10] = {["Memo"]="무너진",	["X"]=575,	["Y"]=500,	["MAP_NO"]=9020510,	["TYPE"]="DEFAULT"}
g_GroundList["KAZRA"][11] = {["Memo"]="신들의",	["X"]=680,	["Y"]=434,	["MAP_NO"]=9020500,	["TYPE"]="DEFAULT"}
g_GroundList["KAZRA"][12] = {["Memo"]="케이론",	["X"]=665,	["Y"]=510,	["MAP_NO"]=9020530,	["TYPE"]="DEFAULT"}
g_Arrow["KAZRA"] = {}
g_Arrow["KAZRA"][2] = { ["Memo"]="카즈라->용암지대", ["X"]=487, ["Y"]=195, ["TYPE"]="BIG_LU_ARROW", ["TARGET_LOCAL_MAP"] = "LAVA", ["TTW"] =803208, ["T_X"]=0, ["T_Y"]=-30}
g_Arrow["KAZRA"][1] = { ["Memo"]="카즈라->고비구릉", ["X"]=645, ["Y"]=600, ["TYPE"]="BIG_LD_ARROW", ["TARGET_LOCAL_MAP"] = "TIME", ["TTW"] =803209, ["T_X"]=18, ["T_Y"]=35}

g_GroundList["TIME"] = {}
g_GroundList["TIME"][1] = {["Memo"]="버섯마을",	["X"]=535,	["Y"]=150,	["MAP_NO"]=9028100,	["TYPE"]="VILLAGE", ["ICON"]="wmapNhonIcon04.tga", ["ICON_W"]=180, ["ICON_H"]=140, ["T_Y"]=60}
g_GroundList["TIME"][2] = {["Memo"]="투투꽃 나무숲",	["X"]=393,	["Y"]=291,	["MAP_NO"]=9030110,	["TYPE"]="DEFAULT"} 
g_GroundList["TIME"][3] = {["Memo"]="부서진버섯공장",	["X"]=475,	["Y"]=260,	["MAP_NO"]=9030140,	["TYPE"]="DEFAULT"}
g_GroundList["TIME"][4] = {["Memo"]="잊혀진 마을",	["X"]=327,	["Y"]=388,	["MAP_NO"]=9030100,	["TYPE"]="DEFAULT"} 
g_GroundList["TIME"][5] = {["Memo"]="잠자는 마을",	["X"]=433,	["Y"]=350,	["MAP_NO"]=9030130,	["TYPE"]="DEFAULT"}
g_GroundList["TIME"][6] = {["Memo"]="눈보라",	["X"]=605,	["Y"]=315,	["MAP_NO"]=9030210,	["TYPE"]="DEFAULT"}
g_GroundList["TIME"][7] = {["Memo"]="유혹설원",	["X"]=668,	["Y"]=375,	["MAP_NO"]=9030200,	["TYPE"]="DEFAULT"}
g_GroundList["TIME"][8] = {["Memo"]="얼음협곡", ["X"]=560,	["Y"]=410,	["MAP_NO"]=9030230,	["TYPE"]="DEFAULT"}
g_GroundList["TIME"][9] = {["Memo"]="폭풍설원", ["X"]=543,	["Y"]=516,	["MAP_NO"]=9030240,	["TYPE"]="DEFAULT"}
g_GroundList["TIME"][10] = {["Memo"]="얼음호수",["X"]=650,	["Y"]=450,	["MAP_NO"]=9030220,	["TYPE"]="DEFAULT"}
g_GroundList["TIME"][11] = {["Memo"]="바티카",	["X"]=513,	["Y"]=330,	["MAP_NO"]=9050610,	["TYPE"]="DEFAULT"}
g_GroundList["TIME"][12] = {["Memo"]="벤시",	["X"]=460,	["Y"]=430,	["MAP_NO"]=9050620,	["TYPE"]="DEFAULT"}
g_GroundList["TIME"][13] = {["Memo"]="붉은여우",["X"]=384,	["Y"]=405,	["MAP_NO"]=9050601,	["TYPE"]="DUNGEON", ["ICON"]="wmapNhonIcon06.tga", ["ICON_W"]=160, ["ICON_H"]=120, ["T_Y"]=55}
g_GroundList["TIME"][14] = {["Memo"]="하이드론",["X"]=284,	["Y"]=455,	["MAP_NO"]=9050600,	["TYPE"]="DUNGEON", ["ICON"]="wmapNhonIcon14.tga", ["ICON_W"]=140, ["ICON_H"]=134, ["T_Y"]=60}
g_Arrow["TIME"] = {}
g_Arrow["TIME"][2] = { ["Memo"]="고비구릉->카즈라", ["X"]=602, ["Y"]=95, ["TYPE"]="BIG_U_ARROW", ["TARGET_LOCAL_MAP"] = "KAZRA", ["TTW"] =803210, ["T_X"]=18, ["T_Y"]=35}
g_Arrow["TIME"][1] = { ["Memo"]="고비구릉->원령산맥", ["X"]=369, ["Y"]=520, ["TYPE"]="BIG_LD_ARROW", ["TARGET_LOCAL_MAP"] = "SPIRIT", ["TTW"] =803211, ["T_X"]=18, ["T_Y"]=35}

--g_GroundList["TIME"][14] = {["Memo"]="모래사원",["X"]=617,	["Y"]=227,	["MAP_NO"]=9050501,	["TYPE"]="DUNGEON", ["ICON"]="wmapNhonIcon05.tga", ["ICON_W"]=140, ["ICON_H"]=100, ["T_Y"]=55}
--g_GroundList["TIME"][3] = {["Memo"]="얼음사원",	["X"]=586,	["Y"]=464,	["MAP_NO"]=9030601,	["TYPE"]="DUNGEON", ["ICON"]="wmapNhonIcon07.tga", ["ICON_W"]=120, ["ICON_H"]=120, ["T_Y"]=53}
--g_GroundList["TIME"][4] = {["Memo"]="먼지폭풍",	["X"]=571,	["Y"]=177,	["MAP_NO"]=9050520,	["TYPE"]="DEFAULT"} 
--g_GroundList["TIME"][5] = {["Memo"]="신기루",	["X"]=524,	["Y"]=229,	["MAP_NO"]=9050510,	["TYPE"]="DEFAULT"} 
--g_GroundList["TIME"][6] = {["Memo"]="고비",	["X"]=575,	["Y"]=283,	["MAP_NO"]=9050530,	["TYPE"]="DEFAULT"}

g_GroundList["SPIRIT"] = {}
g_GroundList["SPIRIT"][1] = {["Memo"]="잿빛안개",	["X"]=375,	["Y"]=220,	["MAP_NO"]=9038300,	["TYPE"]="VILLAGE", ["ICON"]="wmapNhonIcon08.tga", ["ICON_W"]=200, ["ICON_H"]=120, ["T_Y"]=55}
g_GroundList["SPIRIT"][2] = {["Memo"]="반클리프",	["X"]=460,	["Y"]=140,	["MAP_NO"]=9030599,	["TYPE"]="DUNGEON", ["ICON"]="wmapNhonIcon09.tga", ["ICON_W"]=160, ["ICON_H"]=180, ["T_Y"]=85}
g_GroundList["SPIRIT"][3] = {["Memo"]="얼음바위",	["X"]=658,	["Y"]=390,	["MAP_NO"]=9030830,	["TYPE"]="DEFAULT"}
g_GroundList["SPIRIT"][4] = {["Memo"]="살얼음고개",		["X"]=680,	["Y"]=575,	["MAP_NO"]=9030810,	["TYPE"]="DEFAULT"} 
g_GroundList["SPIRIT"][5] = {["Memo"]="눈구름",		["X"]=601,	["Y"]=480,	["MAP_NO"]=9030800,	["TYPE"]="DEFAULT"} 
g_GroundList["SPIRIT"][6] = {["Memo"]="곰발톱",		["X"]=490,	["Y"]=500,	["MAP_NO"]=9030820,	["TYPE"]="DEFAULT"} 
g_GroundList["SPIRIT"][7] = {["Memo"]="설원의",		["X"]=560,	["Y"]=290,	["MAP_NO"]=9030730,	["TYPE"]="DEFAULT"} 
g_GroundList["SPIRIT"][8] = {["Memo"]="추적자",		["X"]=725,	["Y"]=340,	["MAP_NO"]=9030710,	["TYPE"]="DEFAULT"} 
g_GroundList["SPIRIT"][9] = {["Memo"]="원령나무",	["X"]=368,	["Y"]=353,	["MAP_NO"]=9030700,	["TYPE"]="DEFAULT"} 
g_GroundList["SPIRIT"][10] = {["Memo"]="영혼의안식처",	["X"]=500,	["Y"]=390,	["MAP_NO"]=9030720,	["TYPE"]="DEFAULT"} 
g_Arrow["SPIRIT"] = {}
g_Arrow["SPIRIT"][2] = { ["Memo"]="원령산맥->고비구릉", ["X"]=358, ["Y"]=140, ["TYPE"]="BIG_U_ARROW", ["TARGET_LOCAL_MAP"] = "TIME", ["TTW"] =803212, ["T_X"]=18, ["T_Y"]=35}
g_Arrow["SPIRIT"][1] = { ["Memo"]="원령산맥->세상의끝", ["X"]=569, ["Y"]=600, ["TYPE"]="BIG_LD_ARROW", ["TARGET_LOCAL_MAP"] = "END", ["TTW"] =803213, ["T_X"]=18, ["T_Y"]=35}

g_GroundList["END"] = {}
g_GroundList["END"][1] = {["Memo"]="엘로라",	["X"]=510,	["Y"]=350,	["MAP_NO"]=9038200,	["TYPE"]="VILLAGE", ["ICON"]="wmapNhonIcon10.tga", ["ICON_W"]=180, ["ICON_H"]=120, ["T_Y"]=55}
g_GroundList["END"][2] = {["Memo"]="대미궁",	["X"]=510,	["Y"]=250,	["MAP_NO"]=9030300,	["TYPE"]="DUNGEON", ["ICON"]="wmapNhonIcon11.tga", ["ICON_W"]=200, ["ICON_H"]=140, ["T_Y"]=61}
g_GroundList["END"][3] = {["Memo"]="검은발톱",	["X"]=315,	["Y"]=400,	["MAP_NO"]=9050401,	["TYPE"]="DUNGEON", ["ICON"]="wmapNhonIcon12.tga", ["ICON_W"]=140, ["ICON_H"]=100, ["T_Y"]=45}
g_GroundList["END"][4] = {["Memo"]="날개협곡",	["X"]=550,	["Y"]=545,	["MAP_NO"]=9050220,	["TYPE"]="DEFAULT"} 
g_GroundList["END"][5] = {["Memo"]="드래곤",	["X"]=570,	["Y"]=450,	["MAP_NO"]=9050200,	["TYPE"]="DEFAULT"} 
g_GroundList["END"][6] = {["Memo"]="손톱날",	["X"]=470,	["Y"]=467,	["MAP_NO"]=9050210,	["TYPE"]="DEFAULT"} 
g_GroundList["END"][7] = {["Memo"]="세상의끝",	["X"]=380,	["Y"]=498,	["MAP_NO"]=9050230,	["TYPE"]="DEFAULT"} 
g_Arrow["END"] = {}
g_Arrow["END"][1] = { ["Memo"]="세상의끝->원령산맥", ["X"]=690, ["Y"]=350, ["TYPE"]="BIG_RU_ARROW", ["TARGET_LOCAL_MAP"] = "SPIRIT", ["TTW"] =803214, ["T_X"]=18, ["T_Y"]=35}

-- 네비게이션 UI용 월드맵 전체 위치 수정--
g_Navi_Ground = {}
g_Navi_Ground["WIND"] = {["MAP_X"]=-300, ["MAP_Y"]=-128, ["ICON_X"]=-280, ["ICON_Y"]=-128, ["SCALE"]=0.8, ["ICON_SCALE"]=0.9}
g_Navi_Ground["BEACH"] = {["MAP_X"]=-190, ["MAP_Y"]=-60, ["ICON_X"]=-180, ["ICON_Y"]=-60, ["SCALE"]=0.55, ["ICON_SCALE"]=0.9}
g_Navi_Ground["HERO"] = {["MAP_X"]=-175, ["MAP_Y"]=-105, ["ICON_X"]=-165, ["ICON_Y"]=-105, ["SCALE"]=0.65, ["ICON_SCALE"]=0.85}
g_Navi_Ground["LAVA"] = {["MAP_X"]=-175, ["MAP_Y"]=-125, ["ICON_X"]=-155, ["ICON_Y"]=-128, ["SCALE"]=0.7, ["ICON_SCALE"]=0.9}

g_Navi_Ground["KAZRA"] = {["MAP_X"]=-203, ["MAP_Y"]=-109, ["ICON_X"]=-188, ["ICON_Y"]=-109, ["SCALE"]=0.63, ["ICON_SCALE"]=0.85}
g_Navi_Ground["TIME"] = {["MAP_X"]=-162, ["MAP_Y"]=-52, ["ICON_X"]=-148, ["ICON_Y"]=-52, ["SCALE"]=0.6, ["ICON_SCALE"]=0.8}
g_Navi_Ground["SPIRIT"] = {["MAP_X"]=-200, ["MAP_Y"]=-75, ["ICON_X"]=-190, ["ICON_Y"]=-75, ["SCALE"]=0.6, ["ICON_SCALE"]=0.8}
g_Navi_Ground["END"] = { ["MAP_X"]=-220, ["MAP_Y"]=-160, ["ICON_X"]=-210, ["ICON_Y"]=-168, ["SCALE"]=0.72, ["ICON_SCALE"]=0.9}

-- 네비게이션 UI용 월드맵 세부 위치 수정 --
g_Navi_GroundPos = {}
g_Navi_GroundPos[9018200] = {["Memo"]="종소리",		["TYPE"]="VILLAGE", ["ICON_X"]=0, ["ICON_Y"]=15, ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=0}
g_Navi_GroundPos[9010450] = {["Memo"]="칼바람늑", 	["TYPE"]="DUNGEON", ["ICON_Y"]=15, ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=802040}
g_Navi_GroundPos[9010460] = {["Memo"]="통곡늑대", 	["TYPE"]="DUNGEON", ["ICON_X"]=-3, ["ICON_Y"]=15, ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=802041}
g_Navi_GroundPos[9010300] = {["Memo"]="용자의길",	["TYPE"]="DEFAULT", ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=800001} 
g_Navi_GroundPos[9010330] = {["Memo"]="숙련된",		["TYPE"]="DEFAULT", ["T_X"]=25, ["T_Y"]=21, ["NAVI_TTW"]=800002} 
g_Navi_GroundPos[9010400] = {["Memo"]="배신자",		["TYPE"]="DEFAULT", ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=800005} 
g_Navi_GroundPos[9010430] = {["Memo"]="망각협곡",	["TYPE"]="DEFAULT", ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=800006}
g_Navi_GroundPos[9010410] = {["Memo"]="칼바람",		["TYPE"]="DEFAULT", ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=800007} 
g_Navi_GroundPos[9010420] = {["Memo"]="통곡고개", 	["TYPE"]="DEFAULT", ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=800008}


g_Navi_GroundPos[9018100] = { ["Memo"]="바람항구", ["TYPE"]="VILLAGE", ["ICON_X"]=20, ["ICON_Y"]=30, ["T_X"]=12, ["T_Y"]=21} 
g_Navi_GroundPos[9010502] = { ["Memo"]="시청지하수로", ["TYPE"]="DUNGEON", ["ICON_X"]=2, ["ICON_Y"]=-0, ["T_X"]=22, ["T_Y"]=21, ["NAVI_TTW"]=802000} 
g_Navi_GroundPos[9010701] = { ["Memo"]="물의신전", ["TYPE"]="DUNGEON", ["ICON_X"]=-5, ["ICON_Y"]=10, ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=802034} 
g_Navi_GroundPos[9010100] = { ["Memo"]="수염고래", ["TYPE"]="DEFAULT", ["ICON_X"]=-16, ["ICON_Y"]=-8, ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=800011} 
g_Navi_GroundPos[9010110] = { ["Memo"]="톱날꽃게", ["TYPE"]="DEFAULT", ["ICON_X"]=4, ["ICON_Y"]=-3, ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=800012} 
g_Navi_GroundPos[9010120] = { ["Memo"]="갑판상어", ["TYPE"]="DEFAULT", ["ICON_X"]=-2, ["ICON_Y"]=-4, ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=800013} 
g_Navi_GroundPos[9010130] = { ["Memo"]="스티븐",   ["TYPE"]="DEFAULT", ["ICON_X"]=1, ["ICON_Y"]=-4, ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=800014} 
g_Navi_GroundPos[9010140] = { ["Memo"]="앵무조개", ["TYPE"]="DEFAULT", ["ICON_X"]=-2, ["ICON_Y"]=0, ["T_X"]=-6, ["T_Y"]=21, ["NAVI_TTW"]=800015}
g_Navi_GroundPos[9010150] = { ["Memo"]="칼리곤",   ["TYPE"]="DEFAULT", ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=800016}
 

g_Navi_GroundPos[9018310] = {["Memo"]="달빛해안",	 ["TYPE"]="VILLAGE", ["ICON_X"]=9, ["ICON_Y"]=11, ["T_X"]=9, ["T_Y"]=19}
g_Navi_GroundPos[9010910] = {["Memo"]="지하묘지",	 ["TYPE"]="DUNGEON", ["ICON_X"]=0, ["ICON_Y"]=21, ["T_X"]=8, ["T_Y"]=-21, ["NAVI_TTW"]=803011}
g_Navi_GroundPos[9010801] = {["Memo"]="망자의탑",	 ["TYPE"]="DUNGEON", ["ICON_X"]=3, ["ICON_Y"]=23, ["T_X"]=10, ["T_Y"]=-20, ["NAVI_TTW"]=802037}
g_Navi_GroundPos[9010200] = {["Memo"]="미리내",		 ["TYPE"]="DEFAULT", ["ICON_X"]=5, ["ICON_Y"]=-1, ["T_X"]=10, ["T_Y"]=20, ["NAVI_TTW"]=800018} 
g_Navi_GroundPos[9010210] = {["Memo"]="스카이피",	 ["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=5, ["T_X"]=10, ["T_Y"]=20, ["NAVI_TTW"]=800019}
g_Navi_GroundPos[9010220] = {["Memo"]="벨칸신전",	 ["TYPE"]="DEFAULT", ["ICON_X"]=4, ["ICON_Y"]=5, ["T_X"]=12, ["T_Y"]=21, ["NAVI_TTW"]=800020} 
g_Navi_GroundPos[9010230] = {["Memo"]="팔미르",		 ["TYPE"]="DEFAULT", ["ICON_X"]=2, ["ICON_Y"]=-2, ["T_X"]=10, ["T_Y"]=20, ["NAVI_TTW"]=800021} 
g_Navi_GroundPos[9010600] = {["Memo"]="위험동굴",	 ["TYPE"]="DEFAULT", ["ICON_X"]=3, ["ICON_Y"]=0, ["T_X"]=42, ["T_Y"]=-7, ["NAVI_TTW"]=800026}
g_Navi_GroundPos[9010610] = {["Memo"]="지하수로",	 ["TYPE"]="DEFAULT", ["T_X"]=10, ["T_Y"]=20, ["NAVI_TTW"]=800027}
g_Navi_GroundPos[9010620] = {["Memo"]="채굴",		 ["TYPE"]="DEFAULT", ["ICON_X"]=2, ["ICON_Y"]=0, ["T_X"]=42, ["T_Y"]=-2, ["NAVI_TTW"]=800028}

g_Navi_GroundPos[9018320] = {["Memo"]="상인마을",["TYPE"]="VILLAGE", ["ICON_X"]=10, ["ICON_Y"]=10, ["T_X"]=10, ["T_Y"]=21}
g_Navi_GroundPos[9020201] = {["Memo"]="마그마",  ["TYPE"]="DUNGEON", ["ICON_X"]=-19, ["ICON_Y"]=15, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=802002}
g_Navi_GroundPos[9020110] = {["Memo"]="심연열기",["TYPE"]="DEFAULT", ["ICON_X"]=3, ["ICON_Y"]=0,["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800031}
g_Navi_GroundPos[9020130] = {["Memo"]="화암산호",["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=0,["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800033} 
g_Navi_GroundPos[9020100] = {["Memo"]="악마숨결",["TYPE"]="DEFAULT", ["ICON_X"]=-2, ["ICON_Y"]=0,["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800034} 
g_Navi_GroundPos[9020300] = {["Memo"]="이글늪",  ["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=0,["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800040}
g_Navi_GroundPos[9020310] = {["Memo"]="마른나무",["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=0,["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800041}
g_Navi_GroundPos[9020320] = {["Memo"]="갈대슾지",["TYPE"]="DEFAULT", ["ICON_X"]=-2, ["ICON_Y"]=0,["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800042}

g_Navi_GroundPos[9058100] = {["Memo"]="카즈라마을",["TYPE"]="VILLAGE", ["ICON_X"]=-5, ["ICON_Y"]=7, ["T_X"]=10, ["T_Y"]=21}
g_Navi_GroundPos[9020601] = {["Memo"]="쿤다라사원",["TYPE"]="DUNGEON", ["ICON_X"]=2, ["ICON_Y"]=10, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=802004}
g_Navi_GroundPos[9020701] = {["Memo"]="신록의숲",["TYPE"]="DUNGEON", ["ICON_X"]=0, ["ICON_Y"]=14, ["T_X"]=5, ["T_Y"]=21, ["NAVI_TTW"]=802045}
g_Navi_GroundPos[9050301] = {["Memo"]="아키아동굴",["TYPE"]="DUNGEON", ["ICON_X"]=-8, ["ICON_Y"]=14, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=802047}

g_Navi_GroundPos[9050120] = {["Memo"]="에르다",["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=-5, ["T_X"]=10, ["T_Y"]=18, ["NAVI_TTW"]=800074} 
g_Navi_GroundPos[9050100] = {["Memo"]="샬바론",["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800075} 
g_Navi_GroundPos[9050110] = {["Memo"]="라미에",["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800076}
g_Navi_GroundPos[9020420] = {["Memo"]="안개내",["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=-1, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800049}
g_Navi_GroundPos[9020430] = {["Memo"]="솔방울",["TYPE"]="DEFAULT", ["ICON_X"]=3, ["ICON_Y"]=-5, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800047}
g_Navi_GroundPos[9020510] = {["Memo"]="무너진",["TYPE"]="DEFAULT", ["ICON_X"]=-5, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800051}
g_Navi_GroundPos[9020500] = {["Memo"]="신들의",["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800050}
g_Navi_GroundPos[9020530] = {["Memo"]="케이론",["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800053}

g_Navi_GroundPos[9028100] = {["Memo"]="버섯마을",["TYPE"]="VILLAGE", ["ICON_X"]=0, ["ICON_Y"]=15, ["T_X"]=10, ["T_Y"]=21}
g_Navi_GroundPos[9050601] = {["Memo"]="붉은여우",["TYPE"]="DUNGEON", ["ICON_X"]=-5, ["ICON_Y"]=20, ["T_X"]=10, ["T_Y"]=17}
g_Navi_GroundPos[9050600] = {["Memo"]="하이드론",["TYPE"]="DUNGEON", ["ICON_X"]=0, ["ICON_Y"]=10, ["T_X"]=10, ["T_Y"]=21}

g_Navi_GroundPos[9030110] = {["Memo"]="투투꽃 나무숲",["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=-3, ["T_X"]=10, ["T_Y"]=19, ["NAVI_TTW"]=800056} 
g_Navi_GroundPos[9030140] = {["Memo"]="부서진버섯공장",["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=-5, ["T_X"]=10, ["T_Y"]=19, ["NAVI_TTW"]=800058} 
g_Navi_GroundPos[9030100] = {["Memo"]="잊혀진 마을",  ["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=-3, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800055} 
g_Navi_GroundPos[9030130] = {["Memo"]="잠자는 마을", ["TYPE"]="DEFAULT", ["ICON_X"]=-3, ["ICON_Y"]=4, ["T_X"]=10, ["T_Y"]=19, ["NAVI_TTW"]=800059}

g_Navi_GroundPos[9030200] = {["Memo"]="유혹설원",  ["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=19, ["NAVI_TTW"]=800060}
g_Navi_GroundPos[9030210] = {["Memo"]="눈보라고개",["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800061}
g_Navi_GroundPos[9030220] = {["Memo"]="얼음호수",  ["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800062}
g_Navi_GroundPos[9030230] = {["Memo"]="얼음협곡",  ["TYPE"]="DEFAULT", ["ICON_X"]=-2, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800063}
g_Navi_GroundPos[9030240] = {["Memo"]="폭풍설원",  ["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=-5, ["T_X"]=10, ["T_Y"]=20, ["NAVI_TTW"]=800064}
g_Navi_GroundPos[9050610] = {["Memo"]="바티카구릉",["TYPE"]="DEFAULT", ["ICON_X"]=0, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800080}
g_Navi_GroundPos[9050620] = {["Memo"]="벤시협곡",  ["TYPE"]="DEFAULT", ["ICON_X"]=-2, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800081}

--g_Navi_GroundPos[9050501] = {["Memo"]="모래사원",["TYPE"]="DUNGEON", ["ICON_Y"]=8, ["T_X"]=10, ["T_Y"]=18}
--g_Navi_GroundPos[9030601] = {["Memo"]="얼음사원",["TYPE"]="DUNGEON", ["ICON_X"]=-8, ["ICON_Y"]=10, ["T_X"]=10, ["T_Y"]=18}
--g_Navi_GroundPos[9050520] = {["Memo"]="먼지폭풍",["TYPE"]="DEFAULT", ["T_X"]=10, ["T_Y"]=18, ["NAVI_TTW"]=0} 
--g_Navi_GroundPos[9050510] = {["Memo"]="신기루언덕",  ["TYPE"]="DEFAULT", ["T_X"]=10, ["T_Y"]=18, ["NAVI_TTW"]=0} 
--g_Navi_GroundPos[9050530] = {["Memo"]="고비황무지",	 ["TYPE"]="DEFAULT", ["T_X"]=10, ["T_Y"]=18, ["NAVI_TTW"]=0}

g_Navi_GroundPos[9038300] = {["Memo"]="잿빛안개",	["TYPE"]="VILLAGE", ["ICON_X"]=12, ["ICON_Y"]=4, ["T_X"]=10, ["T_Y"]=21}
g_Navi_GroundPos[9030599] = {["Memo"]="반클리프",	["TYPE"]="DUNGEON", ["ICON_X"]=10, ["ICON_Y"]=30, ["T_X"]=10, ["T_Y"]=21}

g_Navi_GroundPos[9030700] = {["Memo"]="원령나무숲",	["TYPE"]="DEFAULT", ["ICON_X"]=3, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800066}
g_Navi_GroundPos[9030710] = {["Memo"]="추적자언덕",  	["TYPE"]="DEFAULT", ["ICON_X"]=-1, ["ICON_Y"]=-5, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800067} 
g_Navi_GroundPos[9030720] = {["Memo"]="영혼의안식처", 	["TYPE"]="DEFAULT", ["ICON_X"]=3, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800068} 
g_Navi_GroundPos[9030730] = {["Memo"]="설원의묘역",  	["TYPE"]="DEFAULT", ["ICON_X"]=3, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800069}
g_Navi_GroundPos[9030800] = {["Memo"]="눈구름골짜기",  	["TYPE"]="DEFAULT", ["ICON_X"]=3, ["ICON_Y"]=0, ["T_X"]=8, ["T_Y"]=21, ["NAVI_TTW"]=800070}
g_Navi_GroundPos[9030810] = {["Memo"]="살얼음고개",  	["TYPE"]="DEFAULT", ["ICON_X"]=3, ["ICON_Y"]=3, ["T_X"]=10, ["T_Y"]=-25, ["NAVI_TTW"]=800071} 
g_Navi_GroundPos[9030820] = {["Memo"]="곰발톱광산",  	["TYPE"]="DEFAULT", ["ICON_X"]=4, ["ICON_Y"]=-3, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800072} 
g_Navi_GroundPos[9030830] = {["Memo"]="얼음바위구릉",	["TYPE"]="DEFAULT", ["ICON_X"]=3, ["ICON_Y"]=0, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800073}

g_Navi_GroundPos[9038200] = {["Memo"]="엘로라",		["TYPE"]="VILLAGE", ["ICON_X"]=12, ["ICON_Y"]=5, ["T_X"]=10, ["T_Y"]=21}
g_Navi_GroundPos[9030300] = {["Memo"]="대미궁",		["TYPE"]="DUNGEON", ["ICON_X"]=14, ["ICON_Y"]=23, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=802028}

g_Navi_GroundPos[9050401] = {["Memo"]="검은발톱둥지",	["TYPE"]="DUNGEON", ["ICON_X"]=3, ["ICON_Y"]=10, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=802055}
g_Navi_GroundPos[9050200] = {["Memo"]="드래곤",		["TYPE"]="DEFAULT", ["ICON_X"]=3, ["ICON_Y"]=3, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800082}
g_Navi_GroundPos[9050220] = {["Memo"]="날개협곡",	["TYPE"]="DEFAULT", ["ICON_X"]=2, ["ICON_Y"]=5, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800083} 
g_Navi_GroundPos[9050210] = {["Memo"]="손톱날",		["TYPE"]="DEFAULT", ["ICON_X"]=7, ["ICON_Y"]=2, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800084} 
g_Navi_GroundPos[9050230] = {["Memo"]="세상의끝",	["TYPE"]="DEFAULT", ["ICON_X"]=-5, ["ICON_Y"]=3, ["T_X"]=10, ["T_Y"]=21, ["NAVI_TTW"]=800085} 

g_NowContintnt = 1
g_NowMapNo = 1

g_iDefBgW = 905

function AddTestActor(no)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local guid = GUID("123")
	guid:Generate()
	local pilot = g_pilotMan:NewPilot(guid, no, 0)
	if pilot:IsNil() == false then
		local kMyActor = g_pilotMan:GetPlayerActor()
		if false==kMyActor:IsNil() then
			local pos = kMyActor:GetPos()
			pos:SetZ(pos:GetZ()+25)
			if true== g_world:AddActor(guid, pilot:GetActor(), kMyActor:GetPos(), 9) then
				pilot:GetActor():ReserveTransitAction("a_opening")
			end
		end
	end
end

function CallWorldUI(wnd)
	if nil==wnd or wnd:IsNil() then return end
	local sz = wnd:GetCustomDataAsStr()
	if nil==sz or sz:IsNil() then return end

	local tbl = g_WorldMap[sz:GetStr()]
	if nil==tbl then return end

	local world = ActivateUI("FRM_WORLD_MAP")
	if world:IsNil() then return end

	local title = world:GetControl("FRM_TITLE")
	if title:IsNil() then
		return
	end
	title:SetStaticTextW(GetTextW(tbl["TITLE_TTW"]))
	UI_T_Y_ToCenter(title)

	for i=0,3 do	--4장이라는 가정
		local kBG = world:GetControl("FRM_BG"..i)
		if false==kBG:IsNil() then
			local bgName = tbl["BG"]
			local path = MAP_PIECE_PATH..bgName..(i+1)..".tga"
			kBG:ChangeImage(path)
		end
	end

	for i=1,10 do
		local over = world:GetControl("FRM_OVER"..i)
		if over:IsNil() then break end
		local name = world:GetControl("FRM_NAME"..i)
		if name:IsNil() then break end
		local tblCont = tbl[i]
		if nil~=tblCont then
			over:SetLocation(Point2(tblCont["X"],tblCont["Y"]))
			over:SetSize(Point2(tblCont["W"],tblCont["H"]))
			over:SetCustomDataAsStr(tblCont["CONT"])
			for j=1,2 do	--각 over그림은 최대 2개로 구성된다는 가정
				local overImg = over:GetControl("IMG_OVER"..j)
				if false==overImg:IsNil() then
					local path = tblCont["OVER"..j]
					if nil~=path then
						overImg:Visible(true)
						overImg:SetSize(Point2(tblCont["OVER"..j.."_W"],tblCont["OVER"..j.."_H"]))
						overImg:SetImgSize(Point2(tblCont["OVER"..j.."_W"],tblCont["OVER"..j.."_H"]))
						if j>1 then
							overImg:SetLocation(Point2(tblCont["OVER"..j.."_X"],tblCont["OVER"..j.."_Y"]))
						end
						overImg:ChangeImage(MAP_PIECE_PATH..path)
						overImg:SetCustomDataAsInt(1)

					else
						overImg:Visible(false)
						overImg:SetCustomDataAsInt(0)
					end
				end
			end
			name:Visible(true)
			name:SetStaticTextW(GetTextW(tblCont["NAME"]))
			name:SetLocation(over:GetLocation())
			name:SetSize(over:GetSize())
			local iTX = 0
			if nil~=tbl[i]["T_X"] then
				iTX = tbl[i]["T_X"]
			end
			local iTY = 0
			if nil~=tbl[i]["T_Y"] then
				iTY = tbl[i]["T_Y"]
			end
			name:SetTextPos(Point2(tblCont["W"]/2+iTX, tblCont["H"]/2+iTY))
		else
			over:Visible(false)
			name:Visible(false)
		end
	end
	
	--월드맵에 내 위치 화살표 표시
	local arrow =  world:GetControl("IMG_ARROW")
	if arrow:IsNil() then
		return
	end
	arrow:Visible(false)

	local myGround = ""
	local pmap = FindPMAP_NO(g_mapNo)
	if 0==pmap then
		pmap = g_mapNo
	end

	local x = 0
	local y = 0
	local posTbl = g_GroundPos[pmap]
	if nil~=posTbl then
		myGround = posTbl["CONT"]
		if nil~=myGround and 
		   nil~=g_GroundList[myGround][posTbl["KEY"]] then
			x = g_GroundList[myGround][posTbl["KEY"]]["X"]	--맵 아이콘 위치
			y = g_GroundList[myGround][posTbl["KEY"]]["Y"]
		end

		local mytbl = g_WorldMy[myGround]
		if nil==mytbl then
			return	
		end
		arrow:Visible(true)

		local ratepos = Point2(x/mytbl["MY_RATE"]+mytbl["MY_X"], y/mytbl["MY_RATE"]+mytbl["MY_Y"])
		g_MapArrowX = ratepos:GetX()
		g_MapArrowY = ratepos:GetY()
		arrow:SetLocation(ratepos)
	end

end

function TeleCardMapDef()
	for Key, Val in pairs(g_GroundList) do
		for SubKey, SubVal in pairs(Val) do
			local cont = g_GroundPos[SubVal["MAP_NO"]]["CONT"]
			if nil~=cont then
				AddTeleCarMapInfo(g_Ground[g_GroundPos[SubVal["MAP_NO"]]["CONT"]]["TITLE_TTW"], SubKey, SubVal["MAP_NO"])
			end
		end
	end
end

function OnMapPieceBuild(wnd)
	if true == wnd:IsNil() then return end

	local wndPnt = wnd:GetParent()
	if true == wndPnt:IsNil() then return end

	local PntName = wndPnt:GetID():GetStr()
	local iIdx = wnd:GetBuildIndex()
	local kTable = g_GroundList[PntName][iIdx]
	if string.len(kTable["BG"]) > 0 then
		wnd:ChangeImage(MAP_PIECE_PATH .. kTable["BG"])
		wnd:SetImgSize( Point2(kTable["W"], kTable["H"]))
	end

	wnd:SetLocation( Point2(kTable["X"], kTable["Y"]))
	wnd:SetSize( Point2(kTable["W"], kTable["H"]))
	
end

function OnMapPieceCall(wnd)
	if true == wnd:IsNil() then return end

	local PntName = wnd:GetParent():GetID():GetStr()
	local iIdx = wnd:GetBuildIndex()
	local kTable = g_GroundList[PntName][iIdx]

	local bOpen = IsOpenGroundByNo(kTable["MAP_NO"])
	wnd:Visible(bOpen)
	if bOpen then
	else
	end

end

function OnMapTitleBuild(wnd)
	if true == wnd:IsNil() then return end
	local wndPnt = wnd:GetParent()
	if true == wndPnt:IsNil() then return end
	local wndPP = wndPnt:GetParent()
	if true == wndPP:IsNil() then return end
	local iIdx = wnd:GetBuildIndex()

	local PntName = wndPP:GetID():GetStr()
	local kTable = g_GroundList[PntName][iIdx]

	local ptText = Point2(kTable["X"] + kTable["ICN_X"] + 15, kTable["Y"] + kTable["ICN_Y"] )

	if "NONE" ~= kTable["TYPE"] then
		wnd:SetStaticTextW(GetMapNameW(kTable["MAP_NO"]))
	else
		wnd:SetStaticText("")
	end
	wnd:SetLocation( Point2(kTable["X"] + kTable["ICN_X"], ptText:GetY()))
	local name_flag = XTF_OUTLINE+XTF_ALIGN_CENTER 
	local t_xy = Point2(wnd:GetSize():GetX()*0.5, 30)
	if nil~=kTable["NAME_FLAG"] then
		name_flag = kTable["NAME_FLAG"]
		local tx = -2
		if nil~=kTable["T_X"] then
			tx = kTable["T_X"]
		end
		t_xy:SetX(tx)
		t_xy:SetY(7)
	end
	wnd:SetTextPos(t_xy)
	wnd:SetFontFlag(name_flag)
	wnd:SetCustomDataAsInt(kTable["MAP_NO"])

	local szType = kTable["TYPE"]
	local wndIcon = wnd:GetControl("IMG_ICON")
	if true == wndIcon:IsNil() then return end
	wndIcon:ChangeImage(g_MapIconPath[szType])
--	wndIcon:SetLocation( Point2(kTable["ICN_X"], 0))
end

function OnMapTitleCall(wnd)
	if true == wnd:IsNil() then return end
	local wndPnt = wnd:GetParent()
	if true == wndPnt:IsNil() then return end
	local wndPP = wndPnt:GetParent()
	if true == wndPP:IsNil() then return end
	local iIdx = wnd:GetBuildIndex()

	local PntName = wndPP:GetID():GetStr()
	local kTable = g_GroundList[PntName][iIdx]
	local bOpen = IsOpenGroundByNo(kTable["MAP_NO"])

	local wndIcon = wnd:GetControl("IMG_ICON")
	if true == wndIcon:IsNil() then return end
	if nil ~=g_world then
		wndIcon:Visible(bOpen)
		local iNo = g_world:GetMapNo()

		local run = wndPnt:GetControl("IMG_RUN")
		if true == run:IsNil() then return end
		if iNo == kTable["MAP_NO"] then
			wndIcon:SetUVIndex(2)
			local kPosTbl = g_GroundPos[kTable["MAP_NO"]]
			if nil~=kPosTbl and nil~=kPosTbl["PMAP_NO"] then
				kPosTbl = g_GroundPos[kPosTbl["PMAP_NO"]]
				kTable = g_GroundList[PntName][kPosTbl["KEY"]]
			end
			run:SetLocation(Point2(kTable["X"] + kTable["ICN_X"]-18, kTable["Y"] + kTable["ICN_Y"]-75))
		else
			wndIcon:SetUVIndex(1)
		end
		run:Visible(g_GroundPos[iNo]["CONT"]==PntName)
		if "HIDDEN" == kTable["TYPE"] or "CHAOS" == kTable["TYPE"] then
			if 0<wnd:GetCustomDataAsInt() then
				wnd:Visible(bOpen)
			end
		end
	end
end

function OnTitleCall(wnd)
	if true == wnd:IsNil() then return end
	local Name = wnd:GetID():GetStr()
	local Title = GetTextW(g_Continent[Name]["TITLE_TTW"])
	local wndTitle = wnd:GetControl("FRM_TITLE")
	if true == wndTitle:IsNil() then return end
	wndTitle:SetStaticTextW(Title)
	wndTitle:SetLocation(Point2(g_Continent[Name]["TITLE_X"], g_Continent[Name]["TITLE_Y"]))
end

function OnContinentDisplay(szPath,wnd)
	if true == wnd:IsNil() then return end
	local Img = wnd:GetControl("IMG_OVER")
	if true == Img:IsNil() then return end
	local point = GetCursorPos()
	if false == wnd:PtInUI(point) then 
		Img:Visible(false)
		return 
	end

	local A = GetAlphaFromImgAtCursor(szPath, wnd)
	Img:Visible(A >= 254)
end

function OnContinentDisplayNew(szHead, wnd, iMax)
	if true == wnd:IsNil() then return end

	local bVisible = false
	local count = 0
	if nil==iMax or 0==iMax then
		iMax = 9
	end
	
	for i=1,iMax do
		local Img = wnd:GetControl(szHead..i)
		if false==Img:IsNil() then
			local custom = 1
			if i>1 then	--하나 이상일 때만 검사하자
				custom = Img:GetCustomDataAsInt()
			end

			local A = 0
			if 0<custom then
				local szPath = Img:GetDefaultImgName()
				A = GetAlphaFromImgAtCursor(szPath:GetStr(), Img)
			end
			if (A >= 254) then
				bVisible = true
				break
			end
		else
			break
		end
	end

	for i=1,iMax do
		local Img = wnd:GetControl(szHead..i)
		if false==Img:IsNil() then
			Img:Visible(bVisible)
		end
	end
end

function OnWorldMapCall(wnd)
	if true == wnd:IsNil() then return end

	if nil ~=g_world then
		local iNo = g_world:GetMapNo()
		local pt = Point2(g_GroundPos[iNo]["X"], g_GroundPos[iNo]["Y"])
		local wndHere = wnd:GetControl("FRM_HERE")
		if true == wndHere:IsNil() then return end
		pt:IncX(-10)
		pt:IncY(-wndHere:GetSize():GetY())
		wndHere:SetLocation(pt)		

		if "VILLAGE" == g_GroundPos[iNo]["TYPE"] then
			wndHere:SetUVIndex(1)
		else
			wndHere:SetUVIndex(2)
		end
	end
end

function CallNewWorldMap()
	if nil ==g_world then return end

	local mapno = FindPMAP_NO(g_world:GetMapNo())
	if 0==mapno then
		mapno = g_world:GetMapNo()
	end
	local tblGrnd = g_GroundPos[mapno]

	if true == g_world:IsHaveWorldAttr(GATTR_FLAG_NOWORLDMAP) or nil==tblGrnd then
		AddWarnDataTT(98)
		return
	end

	g_LastContName = tblGrnd["CONT"]
	local MapUI = GetUIWnd("FRM_LOCAL_MAP")
	if MapUI:IsNil() then
		local MapUI = GetUIWnd("FRM_CONT_MAP")
		if MapUI:IsNil() then
			local MapUI = GetUIWnd("FRM_WORLD_MAP")
			if MapUI:IsNil() then
				CallLocalMap(g_LastContName)
			end
		end
	else
		CloseAllContMapUI()
	end
end

function CloseAllContMapUI()
	CloseUI("FRM_WORLD_MAP")
	CloseUI("FRM_CONT_MAP")
	CloseUI("FRM_LOCAL_MAP")
end

function CallContinentUI(wnd)
	if true == wnd:IsNil() then return end
	local szCont = wnd:GetCustomDataAsStr()
	if szCont:IsNil() then return end

	local tblGrnd = g_Continent[szCont:GetStr()]
	if nil==tblGrnd then return end

	local cont = ActivateUI("FRM_CONT_MAP")
	if cont:IsNil() then return end
	
	for i=0,3 do	--4장이라는 가정
		local kBG = cont:GetControl("FRM_BG"..i)
		if false==kBG:IsNil() then
			local bgName = tblGrnd["BG"]
			local path = MAP_PIECE_PATH..bgName..(i+1)..".tga"
			kBG:ChangeImage(path)
		end
	end

	g_LastContName = szName

	local title = cont:GetControl("FRM_TITLE")
	if title:IsNil() then
		return
	end
	title:SetStaticTextW(GetTextW(tblGrnd["TITLE_TTW"]))
	UI_T_Y_ToCenter(title)

	local head = tblGrnd["OVER"]
	
	local bld = cont:GetControl("BLD_OVER")
	if bld:IsNil() then return end
	local buildCnt = bld:GetBuildCount()
	local Cnt = buildCnt:GetX()
	
	--내 위치 화살표 표시 준비
	local x = 0
	local y = 0
	local myGround = ""
	local pmap = FindPMAP_NO(g_mapNo)
	if 0==pmap then
		pmap = g_mapNo
	end
	local posTbl = g_GroundPos[pmap]
	if nil~=posTbl then
		myGround = posTbl["CONT"]
		if nil~=myGround and 
		   nil~=g_GroundList[myGround][posTbl["KEY"]] then
			x = g_GroundList[myGround][posTbl["KEY"]]["X"]
			y = g_GroundList[myGround][posTbl["KEY"]]["Y"]
		end
	end

	local arrow = cont:GetControl("IMG_ARROW")
	if arrow:IsNil() then return end
	arrow:Visible(false)

	for i=1,10 do
		if nil~=tblGrnd[i] then
			local over = cont:GetControl("FRM_OVER"..i)
			if over:IsNil() then return end
			local overImg = over:GetControl("IMG_OVER1")	--over그림이 하나밖에 없다고 가정
			if overImg:IsNil() then return end
			over:SetLocation(Point2( tblGrnd[i]["X"], tblGrnd[i]["Y"] ))
			over:SetImgSize(Point2(tblGrnd[i]["W"], tblGrnd[i]["H"]))
			over:SetSize(Point2(tblGrnd[i]["W"], tblGrnd[i]["H"]))

			overImg:SetImgSize(Point2(tblGrnd[i]["W"], tblGrnd[i]["H"]))
			overImg:SetSize(Point2(tblGrnd[i]["W"], tblGrnd[i]["H"]))
			overImg:ChangeImage(MAP_PIECE_PATH..head..i..".tga", false)
			overImg:SetCustomDataAsStr(tblGrnd[i]["GROUND"])
			if nil~=tblGrnd[i]["NAME"] then
				over:SetStaticTextW(GetTextW(tblGrnd[i]["NAME"]))
				local iTX = 0
				if nil~=tblGrnd[i]["T_X"] then
					iTX = tblGrnd[i]["T_X"]
				end
				local iTY = 0
				if nil~=tblGrnd[i]["T_Y"] then
					iTY = tblGrnd[i]["T_Y"]
				end
				over:SetTextPos(Point2(tblGrnd[i]["W"]/2+iTX, tblGrnd[i]["H"]/2+iTY))
				overImg:SetStaticTextW(GetTextW(tblGrnd[i]["NAME"]))
				overImg:SetTextPos(Point2(tblGrnd[i]["W"]/2+iTX, tblGrnd[i]["H"]/2+iTY))
			end

			if tblGrnd[i]["GROUND"]==myGround then				
				arrow:Visible(false)
				local mytbl = g_ContientMy[myGround]
				if nil~=mytbl then
					arrow:Visible(true)
				
					local startPos = Point2(tblGrnd[i]["X"],tblGrnd[i]["Y"])
					local ratepos = Point2(startPos:GetX()+x/mytbl["MY_RATE"]-mytbl["MY_X"],
					startPos:GetY()+y/mytbl["MY_RATE"]-mytbl["MY_Y"])
					
					g_MapArrowX = ratepos:GetX()
					g_MapArrowY = ratepos:GetY()
					arrow:SetLocation(ratepos)
				end
			end
		end
	end

	local btn = cont:GetControl("BTN_WORLD_MAP")
	if btn:IsNil() then
		return
	end

	btn:SetCustomDataAsStr(tblGrnd["WORLD"])
end

function OnWorldMapIconClick(wnd)
	if true == wnd:IsNil() then return end
	local wndPnt = wnd:GetParent()
	if true == wndPnt:IsNil() then return end
	local iIdx = wndPnt:GetBuildIndex()
	local wntTop = wnd:FindTopParent()
	if true == wntTop:IsNil() then return end	--최상위
	local kTable = g_GroundList[wntTop:GetID():GetStr()]

	local iMapNo = kTable[iIdx]["MAP_NO"]

	CallMapInfo(iMapNo)
end

function ShowNpcIcon(wnd, bNext)
	if true == wnd:IsNil() then return end
	local wndPnt = wnd:GetParent()
	if true == wndPnt:IsNil() then return end

	for i=0,19 do
		local icon = wndPnt:GetControl("ICON_NPC_"..i)
		if icon:IsNil() == false then
			icon:Visible(not bNext)
		end
	end

	for i=20,39 do
		local icon = wndPnt:GetControl("ICON_NPC_"..i)
		if icon:IsNil() == false then
			icon:Visible(bNext)
		end
	end
end

function CallLocalMap(szLocal)	--대륙맵 호출

	if nil==szLocal or ""==szLocal then
		return
	end

	local tblCont = g_Ground[szLocal]	--대륙정보
	if nil==tblCont then return end
	
	local tbl = g_GroundList[szLocal]		--필드정보
	if nil==tbl then return end

	local kLocal = ActivateUI("FRM_LOCAL_MAP")
	if kLocal:IsNil() then return end

	local minimap = kLocal:GetControl("SFRM_BIG_MAP")
	if false==minimap:IsNil() then
		minimap:Visible(false)
	end

	local kRoute = kLocal:GetControl("FRM_ROUTE_MAP")
	if kRoute:IsNil() then return end

	g_LastContName = szLocal

	for i=0,3 do	--4장이라는 가정
		local kBG = kLocal:GetControl("FRM_BG"..i)
		if false==kBG:IsNil() then
			local bgName = tblCont["BG"]
			local path = MAP_PIECE_PATH..bgName..(i+1)..".tga"
			kBG:ChangeImage(path)
		end
	end

	kRoute:SetLocation(Point2(tblCont["IMG_X"], tblCont["IMG_Y"]))
	kRoute:SetImgSize(Point2(tblCont["IMG_W"], tblCont["IMG_H"]))
	kRoute:SetSize(Point2(tblCont["IMG_W"], tblCont["IMG_H"]))
	kRoute:ChangeImage(MAP_PIECE_PATH..tblCont["LINE_IMG"])

	local title = kLocal:GetControl("FRM_TITLE")
	if title:IsNil() then
		return
	end
	title:SetStaticTextW(GetTextW(tblCont["TITLE_TTW"]))
	UI_T_Y_ToCenter(title)

	--오버이미지
	local over = kLocal:GetControl("FRM_OVER_BG")
	if false==over:IsNil() then
		if nil~=tblCont["OVER"] then
			over:Visible(true)
			over:ChangeImage(MAP_PIECE_PATH..tblCont["OVER"])
			over:SetLocation(Point2(tblCont["OVER_X"], tblCont["OVER_Y"]))
			over:SetImgSize(Point2(tblCont["OVER_W"], tblCont["OVER_H"]))
			over:SetSize(Point2(tblCont["OVER_W"], tblCont["OVER_H"]))
		else
			over:Visible(false)
		end
	end
	
	--아이콘 배치
	local kIconBuild = kLocal:GetControl("BLD_ICON")
	if kIconBuild:IsNil() then
		return
	end

	local iMapCnt = table.getn(tbl)
	local ptIconCnt = kIconBuild:GetBuildCount()
	local kIconCnt = ptIconCnt:GetX()*ptIconCnt:GetY()

	local iIndexForBuild = 1
	local iOpenMap = 0
	local iNow = FindPMAP_NO(g_mapNo)
	if 0==iNow then
		iNow = g_mapNo
	end

	for i=1,kIconCnt do
		local wndMap = kLocal:GetControl("FRM_MAP"..i)	
		if false==wndMap:IsNil() then
			wndMap:SetStaticText("")
			if i<=iMapCnt then	--맵 갯수만큼 ELEMENT를 표시하고
				local iTempMapNo = tbl[i]["MAP_NO"]
				local bVisible = true
				if YX==tbl[i]["X"] or YX==tbl[i]["Y"] then
					bVisible = false
				end
				wndMap:SetLocation(Point2(tbl[i]["X"], tbl[i]["Y"]))	--좌표 설정
				local t_w = 80
				if nil~=tbl[i]["T_W"] then		--로컬 때문에 가로로 넓게 퍼지지 않게 가로값을 조절
					t_w = tbl[i]["T_W"]
				end
				wndMap:SetSize(Point2(t_w, wndMap:GetSize():GetY()))

				local wndIcon = wndMap:GetControl("IMG_ICON")
				if false == wndIcon:IsNil() then
					local iUV = 1
					local bOpenMap = IsOpenGroundByNo(iTempMapNo)

					if iNow == iTempMapNo then				--지금 내가 있는 맵이면
						local x = tbl[i]["X"]
						local y = tbl[i]["Y"]
						if nil~=g_GroundPos[iTempMapNo]["PMAP_NO"] then
							x = tbl[g_GroundPos[g_GroundPos[iTempMapNo]["PMAP_NO"]]["KEY"]]["X"]
							y = tbl[g_GroundPos[g_GroundPos[iTempMapNo]["PMAP_NO"]]["KEY"]]["Y"]
						end

						if nil~=tbl[i]["ICN_Y"] then
							y=y+tbl[i]["ICN_Y"]
						end
						iUV = 2
					elseif false == bOpenMap then	--아직 안가본 맵이면
						iUV = 3				--흑백으로
					end

					if bOpenMap then		--가본 맵이면  맵 갯수 증가
						iOpenMap = iOpenMap + 1
					end
					wndIcon:SetUVIndex(iUV)
					wndIcon:SetCustomDataAsInt(iTempMapNo)
					local iTX = 40
					local iAddW = 0
					if nil~=tbl[i]["ADD_TEXT_W"] then
						iAddW = tbl[i]["ADD_TEXT_W"]
					end
					if nil==tbl[i]["ICON"] then
						wndIcon:SetLocation(Point2(25,0))
						wndIcon:SetSize(Point2(30,30))
						wndIcon:SetImgSize(Point2(48,48))
						wndIcon:ChangeImage(g_MapIconPath[tbl[i]["TYPE"]])
						wndMap:SetSize(Point2(80+iAddW,70))
					else
						local sizeW = tonumber(tbl[i]["ICON_W"])
						local sizeH = tonumber(tbl[i]["ICON_H"])
						wndIcon:SetLocation(Point2(0,0))
						wndIcon:SetSize(Point2(sizeW/2,sizeH/2))
						wndIcon:SetImgSize(Point2(sizeW,sizeH))
						wndIcon:ChangeImage(MAP_PIECE_PATH..tbl[i]["ICON"])
						wndMap:SetSize(Point2(sizeW/2+iAddW,sizeH/2+30))
						iTX = sizeW/4
					end

					
					local iIconPosY = 0	--아이콘의 Y값 조절이 필요한 것들이 있다
					local iIconPosYDef = tbl[i]["ICN_Y"]
					if nil~=iIconPosYDef and 0 < iIconPosYDef then
						iIconPosY = iIconPosYDef
					end
					local ptIconPos = wndIcon:GetLocation()
					ptIconPos:SetY(iIconPosY)
					wndIcon:SetLocation(ptIconPos)

					local iTY = 26	--T_Y값 조절이 필요한 것들이 있다
					if nil~=tbl[i]["T_Y"] then
						iTY = tbl[i]["T_Y"]
					end
					if nil~=tbl[i]["T_X"] then
						iTX = tbl[i]["T_X"]
					end
					if nil~=tbl[i]["T_CENTER"] then
						wndMap:SetFontFlag(tbl[i]["T_CENTER"])
					else
						wndMap:SetFontFlag(24)	--외각선+가운데 정렬
					end
					local ptTPos = wndMap:GetTextPos()
					ptTPos:SetX(iTX)
					ptTPos:SetY(iTY)					
					wndMap:SetTextPos(ptTPos)

					if nil~=g_Navi_GroundPos[iTempMapNo]["NAVI_TTW"] and
						0~=g_Navi_GroundPos[iTempMapNo]["NAVI_TTW"]then
						wndMap:SetStaticTextW(GetTT(g_Navi_GroundPos[iTempMapNo]["NAVI_TTW"]))			--맵 이름 설정
					else
						wndMap:SetStaticTextW(GetMapNameW(iTempMapNo))			--맵 이름 설정
					end
				end

				wndMap:Visible(bVisible)
				if bVisible then
					iIndexForBuild = iIndexForBuild + 1
				end
			else--그 이하는 안보이게
				wndMap:Visible(false)
			end
		end
	end
	
	for i=iIndexForBuild,kIconCnt do
		local wndMap = kRoute:GetControl("FRM_MAP"..i)	
		if false==wndMap:IsNil() then
			wndMap:Visible(false)
		end
	end

	SetCoupleIcon(kLocal, GetMyCoupleMapNo())
	SetPartyIcon(kLocal)

	local up = kLocal:GetControl("BTN_CONT_MAP")
	if false==up:IsNil() and nil~=tblCont["CONT"] then
		up:SetCustomDataAsStr(tblCont["CONT"])
	end
	
	-- 화살표 설정
	CallWorldMapArrow(szLocal, kLocal)
end

function OnWorldMapIconClickNew(wnd)	--클릭한곳 화살표 표시
	if wnd:IsNil() then return end

	local iMapNo = wnd:GetCustomDataAsInt()
	if false == IsOpenGroundByNo(iMapNo) then	--아직 안가본 맵이면
		AddWarnDataTT(528)
		return
	end

	local wndParent = wnd:GetParent()
	if wndParent:IsNil() then return end
	local wndPParent = wndParent:GetParent()
	if wndPParent:IsNil() then return end

	CallBigMap(iMapNo)
end

function SetMapNameToUI(wnd, iNo, szName)
	if wnd:IsNil() then
		return
	end

	if nil==szName or ""==szName then
		szName = "FRM_MAP_NAME"
	end

	local wndName = wnd:GetControl(szName)
	if false==wndName:IsNil() then
		if nil==iNo or 0==iNo then
			wndName:SetStaticText("")
		else
			wndName:SetStaticTextW(GetMapNameW(iNo))
		end
	end
end

function SetConNameToUI(wnd, iNo, szName)
	if wnd:IsNil() then	return end

	if nil==szName or ""==szName then
		szName = "FRM_CONT_NAME"
	end

	local wndName = wnd:GetControl(szName)
	if false==wndName:IsNil() then
		if nil==iNo or 0==iNo then
			wndName:SetStaticText("")
		else
			wndName:SetStaticTextW(GetTextW(iNo))
		end
	end
end

g_NaviMoveDelta = 77

function MapPartyIconOnOff()
	local PartyTbl = {}
	local iCount = 1
	for i=0,3 do
		local kGuid = GetMemberGuidAt(i)		
		local kPlayerGuid = g_pilotMan:GetPlayerPilotGuid()
		local bVisible = (not kGuid:IsNil())
		local bVisible2 = (not kPlayerGuid:IsEqual(kGuid))
		if bVisible and bVisible2 then
			local iMapNo = GetPartyMemberLocation(kGuid)
			local szMemberName = GetMemberNameAt(i):GetStr()
			if nil==PartyTbl[iMapNo] then	--first 
				local kLocal = GetUIWnd("FRM_LOCAL_MAP")
				local wndIcon = SetCoupleIcon(kLocal, iMapNo, "IMG_PARTY".. iCount-1)
				if nil~=wndIcon then
					local tPos = wndIcon:GetTotalLocation()
					wndIcon:Visible(0<=tPos:GetX())
				end
			end
			iCount = iCount + 1
		end
	end

	local wndCouple = SetCoupleIcon(GetMyCoupleMapNo())
	if nil~=wndCouple then
		local tPos = wndCouple:GetTotalLocation()
		wndCouple:Visible(0<=tPos:GetX())
	end
end

function OnMapLeftScrBtnClick(wnd)
	if wnd:IsNil() then	return end
	local wndRoute = wnd:GetParent():GetControl("FRM_ROUTE_MAP")
	if wndRoute:IsNil() then return end
	local iMin = g_NaviMoveDelta
	local ptSize = wndRoute:GetSize()
	if ptSize:GetX()<=g_iDefBgW then return end
	local ptLoc = wndRoute:GetLocation()
	local iDeltaLeft = math.min(g_NaviMoveDelta,g_NaviMoveDelta-ptLoc:GetX()) --왼쪽으로 삐져 나온 길이
	ptLoc:IncX(iDeltaLeft)	
	wndRoute:SetLocation(ptLoc)

	MapPartyIconOnOff()
end

function OnMapRightScrBtnClick(wnd)
	if wnd:IsNil() then	return end
	local wndRoute = wnd:GetParent():GetControl("FRM_ROUTE_MAP")
	if wndRoute:IsNil() then return end
	local iMin = g_NaviMoveDelta
	local ptSize = wndRoute:GetSize()
	if ptSize:GetX()<=g_iDefBgW then return end
	local ptLoc = wndRoute:GetLocation()
	local iDeltaLeft = g_NaviMoveDelta-ptLoc:GetX() --왼쪽으로 삐져 나온 길이
	iDeltaLeft = math.max(0, iDeltaLeft)
	local iDeltaRight = ptSize:GetX() - iDeltaLeft - g_iDefBgW
	iDeltaRight = math.max(0, iDeltaRight)
	iDeltaRight = math.min(80, iDeltaRight)	

	ptLoc:IncX(-iDeltaRight)	
	wndRoute:SetLocation(ptLoc)
	
	MapPartyIconOnOff()
end

function SetCoupleIcon(kLocal, iNo, szFormName)
	if nil==kLocal or kLocal:IsNil() then 
		kLocal = GetUIWnd("FRM_LOCAL_MAP")
	end
	if nil==kLocal or kLocal:IsNil() then 
		kLocal = GetUIWnd("SFRM_NAVI_MAP")
	end
	if nil==kLocal and kLocal:IsNil() then return end
	
	local bLocalMap = false
	if "FRM_LOCAL_MAP" == kLocal:GetID():GetStr() then
		bLocalMap = true
	elseif "SFRM_NAVI_MAP" == kLocal:GetID():GetStr() then
		bLocalMap = false
	end
	
	if nil==szFormName or ""==szFormName then
		szFormName = "IMG_COUPLE"
	end
	local wndCouple = kLocal:GetControl(szFormName)
	if wndCouple:IsNil() then return end

	wndCouple:Visible(false)

	local kGrndTbl = g_GroundPos[iNo]
	if nil==kGrndTbl then
		return
	end

	if nil~=kGrndTbl["PMAP_NO"] then
		kGrndTbl = g_GroundPos[kGrndTbl["PMAP_NO"]]
	end

	local wndMap0 = kLocal:GetControl("FRM_MAP1")	--첫번째 맵을 찾아서 같은 대륙인지 검사하자
	if wndMap0:IsNil() then return end

	local kMap0Tbl = g_GroundPos[wndMap0:GetControl("IMG_ICON"):GetCustomDataAsInt()]
	if nil==kMap0Tbl then
		return
	end

	if kGrndTbl["CONT"] ~= kMap0Tbl["CONT"] then	--대륙이 다르면 리턴
		return
	end

	local kListTble = g_GroundList[kGrndTbl["CONT"]][kGrndTbl["KEY"]]
	if nil==kListTble then return end

	local wndMapNow = kLocal:GetControl("FRM_MAP"..kGrndTbl["KEY"])	--키가 1부터 시작
	if false == wndMapNow:IsNil() then
		local icon_y = 0
		local icon_x = 0
		if nil~=kListTble["ICON_W"] then
			icon_x = kListTble["ICON_W"]/4-29
		end
		if nil~=kListTble["ICON_H"] then
			icon_y = kListTble["ICON_H"]/4-4
		end
		wndCouple:Visible(true)
		if true==bLocalMap then
			wndCouple:SetLocation(Point2(wndMapNow:GetLocation():GetX()+icon_x+29, wndMapNow:GetLocation():GetY()+icon_y+4))
		else
			wndCouple:SetLocation(Point2(wndMapNow:GetLocation():GetX()+2, wndMapNow:GetLocation():GetY()+2))
		end
	end

	return wndCouple
end

function SetPartyIcon(kLocal)
	if nil==kLocal or kLocal:IsNil() then 
		kLocal = GetUIWnd("FRM_LOCAL_MAP")
	end
	if nil==kLocal or kLocal:IsNil() then 
		kLocal = GetUIWnd("SFRM_NAVI_MAP")
	end
	if nil==kLocal and kLocal:IsNil() then return end
	
	local wndCouple = {}
	for i=1,3 do
		wndCouple[i] = kLocal:GetControl("IMG_PARTY"..i-1)
		if wndCouple[i]:IsNil() then
			return
		else
			wndCouple[i]:Visible(false)
		end
	end

	if false==IsInParty() then
		return
	end

	local PartyTbl = {}

	local iCount = 1
	for i=0,3 do
		local kGuid = GetMemberGuidAt(i)		
		local kPlayerGuid = g_pilotMan:GetPlayerPilotGuid()
		local bVisible = (not kGuid:IsNil())
		local bVisible2 = (not kPlayerGuid:IsEqual(kGuid))
		if bVisible and bVisible2 then
			local iMapNo = GetPartyMemberLocation(kGuid)
			local szMemberName = GetMemberNameAt(i):GetStr()
			if nil==PartyTbl[iMapNo] then	--first 
				local wndIcon = SetCoupleIcon(kLocal, iMapNo, "IMG_PARTY".. iCount-1)
				if nil~=wndIcon and false==wndIcon:IsNil() then
					wndIcon:SetCustomDataAsStr(szMemberName)
				end

				PartyTbl[iMapNo] = wndIcon
			else
				local szName = PartyTbl[iMapNo]:GetCustomDataAsStr()
				szName = szName:GetStr() .. "\n" .. szMemberName
				PartyTbl[iMapNo]:SetCustomDataAsStr(szName)
			end
			iCount = iCount + 1
		end
	end
	CloseToolTip()
end

function OnContEleClick(wnd)
	local wndForm = wnd:GetParent():GetParent()
	if wndForm:IsNil() then return end

	wndForm:Visible(false)
	CallLocalMap(wnd:GetCustomDataAsStr():GetStr())
end

function OnContDropBtnClick(wnd)
	local wndList = wnd:GetParent():GetControl("SFRM_LIST_BOX")
	if wndList:IsNil() then return end

	wndList:Visible(not wndList:IsVisible())
end

function OnFindNpcClick(wnd)
	local Edt = wnd:GetParent():GetControl("EDT_NPC_NAME")
	local iMapNo = GetMapNoFromNpcName(Edt:GetEditText())
	local tbl = g_GroundPos[iMapNo]
	if nil==tbl then
		AddWarnDataTT(527)
		return
	end

	CallLocalMap(tbl["CONT"])

	local GrndTble = g_GroundList[tbl["CONT"]][tbl["KEY"]]
	if nil==GrndTble then return end

	local wndArr = wnd:GetParent():GetControl("FRM_ROUTE_MAP"):GetControl("IMG_ARROW")
	if wndArr:IsNil() then return end
	wndArr:Visible(true)
	wndArr:SetLocation(Point2(GrndTble["X"]+25, GrndTble["Y"]-12))
	Edt:SetEditText("")

	--화살표까지는 안가본 맵이라도 설정해 주자
	if false == IsOpenGroundByNo(iMapNo) then	--아직 안가본 맵이면
		AddWarnDataTT(528)
		return
	end

	CallBigMap(iMapNo)
end

function CallBigMap(iMapNo)
	if 0>=iMapNo then return end
	local kLocal = GetUIWnd("FRM_LOCAL_MAP")
	if kLocal:IsNil() then return end

	local kMap = kLocal:GetControl("SFRM_BIG_MAP")
	if kMap:IsNil() then return end
	local wndRealMap = kMap:GetControl("FRM_MAP")
	if wndRealMap:IsNil() then return end

	local szPath = GetMiniMapPath(iMapNo)
	if nil==szPath or ""==szPath:GetStr() then
		AddWarnDataTT(665)
		return
	end

	kMap:Visible(true)
	wndRealMap:ChangeImage(szPath:GetStr())
	local ptSize = GetImgSizeFromPath(szPath:GetStr())
	wndRealMap:SetSize(ptSize)
	wndRealMap:SetImgSize(ptSize)
	ptSize:IncX(6)
	ptSize:IncY(6)
	kMap:SetSize(ptSize)
	wndRealMap:SetInvalidate()
	kMap:SetInvalidate()
end

function LockMyInputForWorldMap(bLock)
	local kMyPilot = g_pilotMan:GetPlayerPilot()
	if nil ~= kMyPilot and false == kMyPilot:IsNil() then
		local MyUnit = kMyPilot:GetUnit()
		if nil ~= MyUnit and false == MyUnit:IsNil() then
			if MyUnit:IsEffect(402001) == false then	--안전거품 사용안했을 때
				if bLock then
					LockPlayerInputMove(9)
					LockPlayerInput(9)
				else
					UnLockPlayerInputMove(9)
					UnLockPlayerInput(9)
				end
				BlockGlobalHotKey(bLock)
			end
		end
	end
end

function IsInGroundPosTable(iNo)
	return nil~=g_GroundPos[iNo]
end

function CallImageMap()
	CallUI(g_LastContName)
end

function CallMapTooltip(wnd)
	if wnd:IsNil() then return end

	local iMapNo = wnd:GetCustomDataAsInt()
	if nil == g_GroundPos[iMapNo] then
		return
	end
	local iTTW = g_GroundPos[iMapNo]["TTW"]
	if nil==iTTW or 0==iTTW then
		return
	end
	local bParent = false
	local lwStr = WideString("")--GetTextW(iTTW)
	local nametbl = {}
	local maptbl = {}
	local iMapNo3 = iMapNo
	local iCount = 0
	local NowMapNo = g_world:GetMapNo()
	local iMyTTW = g_GroundPos[NowMapNo]["TTW"]
	local iMyPmap = g_GroundPos[NowMapNo]["PMAP_NO"]
	local bChange = false	--다 같은 맵인가.
	if nil~=iMyPmap and iMapNo == iMyPmap then	--이 맵이 내가 있는 맵의 부모맵이면
		local kMyPilot = g_pilotMan:GetPlayerPilot()
		if nil~=kMyPilot and false==kMyPilot:IsNil() then
			nametbl[iCount] = kMyPilot:GetName():GetStr()
			maptbl[iCount] = GetMapNameW(NowMapNo):GetStr()
--			iMapNo3 = NowMapNo
			iCount = iCount + 1	--하나 증가
		end
	end


	if IsInParty() then
		local kPlayerGuid = g_pilotMan:GetPlayerPilotGuid()
		for i=0,3 do
			local kGuid = GetMemberGuidAt(i)		
			local bVisible = (not kGuid:IsNil())
			local bVisible2 = (not kPlayerGuid:IsEqual(kGuid))
			if bVisible and bVisible2 then
				local iMapNo2 = GetPartyMemberLocation(kGuid)
				if iMapNo==iMapNo2 then
					nametbl[iCount] = GetMemberNameAt(i):GetStr()
					maptbl[iCount] = GetMapNameW(iMapNo2):GetStr()
					iCount = iCount + 1
				elseif nil~=g_GroundPos[iMapNo2] then
					local iPmap = g_GroundPos[iMapNo2]["PMAP_NO"]
					if nil~=iPmap and iMapNo == iPmap then
						if iMapNo3 ~= iMapNo2 then
							bChange = true	--한명이라도 같은맵에 있지 않으면
						end

						nametbl[iCount] = GetMemberNameAt(i):GetStr()
						maptbl[iCount] = GetMapNameW(iMapNo2):GetStr()
						iCount = iCount + 1
					end
				end
			end
		end
	end

	if false==bChange then	--다 같은 맵이면
		iTTW = g_GroundPos[iMapNo3]["TTW"]
		--lwStr = GetTextW(iTTW)
	end

	for i=0,iCount-1 do
		lwStr:Add("\n"..nametbl[i])
		if bChange then
			lwStr:Add(" "..maptbl[i])
		end
	end
	
	if 0 < lwStr:Length() then
		CallToolTipText(0, lwStr, GetCursorPos())
	end
end

function CallNaviMap_WorldMap()

	if nil ==g_world then 
		return 
	end
	
	local WorldAttr = g_world:GetAttr()
	
	if GATTR_PVP == WorldAttr 
	and GATTR_MISSION == WorldAttr 
	and GATTR_BOSS == WorldAttr 
	and GATTR_EMPORIABATTLE == WorldAttr then
		return
	end
	
	local iShowNavi = Config_GetValue(HEADKEY_ETC,SUBKEY_NAVIGATION)
	if 0 == iShowNavi then
		return
	end
	
	local mapno = FindPMAP_NO(g_world:GetMapNo())
	if 0==mapno then
		mapno = g_world:GetMapNo()
	end
	local tblGrnd = g_GroundPos[mapno]

	if true == g_world:IsHaveWorldAttr(GATTR_FLAG_NOWORLDMAP) 
	or true == g_world:IsHaveWorldAttr(GATTR_FLAG_SUPER) 
	or true == g_world:IsHaveWorldAttr(GATTR_HARDCORE_DUNGEON) 
	or nil==tblGrnd then
		return
	end

	local szLocal = tblGrnd["CONT"]
	
	if nil==szLocal or ""==szLocal then
		return
	end

	local tblCont = g_Ground[szLocal]	--대륙정보
	if nil==tblCont then return end
	local tbl = g_GroundList[szLocal]		--필드정보
	if nil==tbl then return end
	local tNavi = g_Navi_Ground[szLocal]		--필드정보
	if nil==tNavi then return end
	

	local kMap = ActivateUI("SFRM_NAVI_MAP")
	if kMap:IsNil() then return end
	
	local kRoute = kMap:GetControl("FRM_ROUTE_MAP")
	if kRoute:IsNil() then return end
	--길 이미지
	local RouteX = (tblCont["IMG_X"]*tNavi["SCALE"]) + tNavi["MAP_X"]
	local RouteY = (tblCont["IMG_Y"]*tNavi["SCALE"]) + tNavi["MAP_Y"]
	kRoute:SetLocation(Point2( RouteX, RouteY ) )
	kRoute:SetImgSize(Point2(tblCont["IMG_W"], tblCont["IMG_H"]))
	kRoute:SetSize(Point2(tblCont["IMG_W"], tblCont["IMG_H"]))
	kRoute:ChangeImage(MAP_PIECE_PATH..tblCont["LINE_IMG"])
	kRoute:SetScale(tNavi["SCALE"])
	
	
	--아이콘 배치
	local kIconBuild = kMap:GetControl("BLD_ICON")
	if kIconBuild:IsNil() then
		return
	end
	
	local iMapCnt = table.getn(tbl)
	local ptIconCnt = kIconBuild:GetBuildCount()
	local kIconCnt = ptIconCnt:GetX()*ptIconCnt:GetY()

	local iIndexForBuild = 1
	local iOpenMap = 0
	local iNow = FindPMAP_NO(g_mapNo)
	if 0==iNow then
		iNow = g_mapNo
	end
	
	for i=1,kIconCnt do
		local wndMap = kMap:GetControl("FRM_MAP"..i)	
		if false==wndMap:IsNil() then
			wndMap:SetStaticText("")
			if i<=iMapCnt then	--맵 갯수만큼 ELEMENT를 표시하고
				local iTempMapNo = tbl[i]["MAP_NO"]
				local tblCustomIcon = g_Navi_GroundPos[iTempMapNo]
				local bVisible = true
				if YX==tbl[i]["X"] or YX==tbl[i]["Y"] then
					bVisible = false
				end
				
				local IconX = (tbl[i]["X"]*tNavi["SCALE"]) + tNavi["ICON_X"]
				local IconY = (tbl[i]["Y"]*tNavi["SCALE"]) + tNavi["ICON_Y"]
				if nil~=tblCustomIcon["ICON_X"] then
					IconX = IconX + tblCustomIcon["ICON_X"]
				end
				if nil~=tblCustomIcon["ICON_Y"] then
					IconY = IconY + tblCustomIcon["ICON_Y"]
				end
				wndMap:SetLocation(Point2(IconX, IconY))	--좌표 설정
				local t_w = 80
				if nil~=tbl[i]["T_W"] then		--로컬 때문에 가로로 넓게 퍼지지 않게 가로값을 조절
					t_w = tbl[i]["T_W"]
				end
				wndMap:SetSize(Point2(t_w, wndMap:GetSize():GetY()))

				local wndIcon = wndMap:GetControl("IMG_ICON")
				if false == wndIcon:IsNil() then
					local iUV = 1
					local bOpenMap = IsOpenGroundByNo(iTempMapNo)

					if iNow == iTempMapNo then				--지금 내가 있는 맵이면
						local x = tbl[i]["X"]
						local y = tbl[i]["Y"]
						if nil~=g_GroundPos[iTempMapNo]["PMAP_NO"] then
							x = tbl[g_GroundPos[g_GroundPos[iTempMapNo]["PMAP_NO"]]["KEY"]]["X"]
							y = tbl[g_GroundPos[g_GroundPos[iTempMapNo]["PMAP_NO"]]["KEY"]]["Y"]
						end

						if nil~=tbl[i]["ICN_Y"] then
							y=y+tbl[i]["ICN_Y"]
						end
						iUV = 2
					elseif false == bOpenMap then	--아직 안가본 맵이면
						iUV = 3				--흑백으로
					end

					if bOpenMap then		--가본 맵이면  맵 갯수 증가
						iOpenMap = iOpenMap + 1
					end
					wndIcon:SetUVIndex(iUV)
					wndIcon:SetCustomDataAsInt(iTempMapNo)
					
					wndIcon:SetLocation(Point2(0,0))
					wndIcon:SetSize(Point2(30,30))
					wndIcon:SetImgSize(Point2(48,48))
					if nil ~= tblCustomIcon["TYPE"] then
						wndIcon:ChangeImage( g_MapIconPath[tblCustomIcon["TYPE"]] )
					else
						wndIcon:ChangeImage(g_MapIconPath[tbl[i]["TYPE"]])
					end
					wndIcon:SetScale(tNavi["ICON_SCALE"])

					local ptTPos = wndIcon:GetTextPos()
					if nil~=tblCustomIcon["T_X"] then
						ptTPos:SetX(ptTPos:GetX() + tblCustomIcon["T_X"] )
					end
					if nil~=tblCustomIcon["T_Y"] then
						ptTPos:SetY(ptTPos:GetY() + tblCustomIcon["T_Y"] )
					end
					wndMap:SetTextPos(ptTPos)
					
					local iAddW = 0
					if nil~=tbl[i]["ADD_TEXT_W"] then
						iAddW = tbl[i]["ADD_TEXT_W"]
					end
					wndMap:SetSize(Point2(80+iAddW,70))

					if nil~=g_Navi_GroundPos[iTempMapNo]["NAVI_TTW"] and
						0~=g_Navi_GroundPos[iTempMapNo]["NAVI_TTW"]then
						wndMap:SetStaticTextW(GetTT(g_Navi_GroundPos[iTempMapNo]["NAVI_TTW"]))			--맵 이름 설정
					else
						wndMap:SetStaticTextW(GetMapNameW(iTempMapNo))			--맵 이름 설정
					end
				end

				wndMap:Visible(bVisible)
				if bVisible then
					iIndexForBuild = iIndexForBuild + 1
				end
			else--그 이하는 안보이게
				wndMap:Visible(false)
			end
		end
	end
	
	for i=iIndexForBuild,kIconCnt do
		local wndMap = kRoute:GetControl("FRM_MAP"..i)	
		if false==wndMap:IsNil() then
			wndMap:Visible(false)
		end
	end

	SetCoupleIcon(kMap, GetMyCoupleMapNo())
	SetPartyIcon(kMap)

end

function CallWorldMapArrow(szLocal, kLocal)
	if(nil == szLocal) then return end
	if(nil == kLocal) then return end
	if( true == kLocal:IsNil() ) then return end	
	local kTblSize = g_Arrow["SIZE"]
	if(nil == kTblSize ) then return end	
	local kTblPath = g_Arrow["PATH"]
	if(nil == kTblPath ) then return end
	
	-- 가용한 화살표의 개수를 알아와서 
	local kArrowBuild = kLocal:GetControl("BLD_ARROW")	
	if kArrowBuild:IsNil() then	return end	
	local ptArrowCnt = kArrowBuild:GetBuildCount()	
	local iArrowCnt = ptArrowCnt:GetX()*ptArrowCnt:GetY()
	
	local i = 0
		
	for i=1,iArrowCnt  do  -- 개수 만큼 
		local wndArrow = kLocal:GetControl("BTN_ARROW"..i)
		if(true == wndArrow:IsNil()) then return end
		--local IconImg = wndArrow:GetControl("IMG_ICON")
		--if(true == IconImg:IsNil()) then return end
		local kTblArrow = g_Arrow[szLocal]
		if(nil == kTblArrow) then 
			wndArrow:Visible(false)
		else
			local iCntArrowTbl = table.getn( kTblArrow )
			
			if(i <= iCntArrowTbl) then						
				if(nil == kTblArrow[i]) then break end
				
				local kType = kTblArrow[i]["TYPE"] -- KEY TABLE
				if( nil == kType ) then return end
					
				local kPath = kTblPath[kType] -- PATH TABLE IN KEY
				if( nil == kPath ) then return end
				
				wndArrow:Visible(true)
				--wndArrow:ChangeImage( kPath )
								
				local kTblSizePoint = kTblSize[kType]
				if(nil == kTblSizePoint) then return end		
				
				local sizeW = kTblSizePoint[1] -- SIZE TABLE IN KEY
				local sizeH = kTblSizePoint[2] -- SIZE TABLE IN KEY
				wndArrow:ButtonChangeImage( kPath, sizeW, sizeH )
				--wndArrow:SetImgSize(Point2(sizeW,sizeH))
				--wndArrow:SetSize( Point2(sizeW/2,sizeH/2) )
				wndArrow:SetLocation(Point2(kTblArrow[i]["X"], kTblArrow[i]["Y"]))	--좌표 설정
				wndArrow:SetCustomDataAsStr( kTblArrow[i]["TARGET_LOCAL_MAP"] )
				wndArrow:SetUVIndex(2)
				wndArrow:SetMaxUVIndex(2,2)
				
				-- 설명
				local iTTW = kTblArrow[i]["TTW"]
				if(nil ~= iTTW) then
					wndArrow:SetStaticTextW( GetTextW(kTblArrow[i]["TTW"]) )
					local ptTPos = wndArrow:GetTextPos()
					ptTPos:SetX(kTblArrow[i]["T_X"])
					ptTPos:SetY(kTblArrow[i]["T_Y"])
					wndArrow:SetTextPos(ptTPos)
				else
					wndArrow:SetStaticTextW("")
				end
			else
				wndArrow:Visible(false)
				wndArrow:SetCustomDataAsStr( "" )
			end
		end
	end	
end

function OnClickWorldMapArrow(kWnd)
	if(nil == kWnd) then return end
	if( true == kWnd:IsNil() ) then return end
	local kStr = kWnd:GetCustomDataAsStr()
	if(nil == kStr) then return end
	if( true == kStr:IsNil() ) then return end
	CallLocalMap( kStr:GetStr() )
end

iCurUVIndex = 1
function UpdateProgressMap(kSelf)
	if kSelf == nil or kSelf:IsNil() == true then
		return
	end
	
	iCurUVIndex = kSelf:GetUVIndex()
	if iCurUVIndex < 9 then
		iCurUVIndex = iCurUVIndex + 1
		kSelf:SetUVIndex(iCurUVIndex)
	else
		iCurUVIndex = 1
		kSelf:Visible(false)
	end
end
