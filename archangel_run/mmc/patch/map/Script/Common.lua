
-- 테스트를 쉽게 하기 위해 만든 함수
function Eric_GetTestInt()
	-- 정예 몬스터 스킬 번호 --
	--return 6004900;	-- 베가본드 왼손치기
	--return 6004901;	-- 베가본드 물기
	--return 6004902;	-- 베가본드 할퀴기 날리기
	--return 6004903;	-- 베가본드 소리지르기
	--return 6004904;	-- 베가본드 돌진하기
	--return 6004905;	-- 베가본드 오른손치기
	--return 6006900;		-- 마할카 도끼내려치기
	--return 6006901;		-- 마할카 도끼 던졌다가 받기
	--return 6006902;		-- 마할카 휠윈드
	--return 6006903;		-- 마할카 휠윈드 전진하기
	--return 6006904;		-- 마할카 돌진하기
	--return 6009600;		-- 데스마스터 크게 베기
	--return 6009601;		-- 데스마스터 방패 던지기
	--return 6009602;		-- 데스마스터 기모았다 발산	
	--return 6009603;		-- 데스마스터 블럭킹
	--return 6009604;		-- 데스마스터 대지의 분노
	--return 6013900;	-- 압둘라 휘두르기
	--return 6013901;	-- 압둘라 총쏘기
	--return 6013902;	-- 압둘라 보다 강하게 총쏘기
	--return 6013903;		-- 압둘라 넷바인더
	--return 6013904;	-- 압둘라 스스로 힐하기
	--return 6013905;		-- 압둘라 단체 힐하기
	--return 6015900;	-- 삼바바 늑대 휘두르기
	--return 6015901;	-- 삼바바 늑대 내려치기
	--return 6015902;	-- 삼바바 입 벌려 독 발사하기
	--return 6015903;	-- 삼바바 점프해서 바닥 찍기
	return 6013909 -- 알비다 불가사리 비 소환
	
	-- 기타 스킬 번호 --
	--return 101000401	-- 와신상담
	--return 106300505	-- 와일드 레이지
	--return 106500204	-- 휠윈드
	
end



GOBAL_LUA_MONSTER_KILL_SYSTEM		= 0;
GOBAL_LUA_MONSTER_KILL_MAX			= 1;
GOBAL_LUA_MONSTER_KILL_GIVE_ITEM_1	= 2;
GOBAL_LUA_MONSTER_KILL_GIVE_ITEM_2	= 3;
GOBAL_LUA_MONSTER_KILL_GIVE_ITEM_3	= 4;
GOBAL_LUA_MONSTER_KILL_HALF_MESSAGE_COUNT = 5; --중간 쯤에 메시지를 띄워줄 마리 수

-- 하드코딩이 필요한 값들을 루아에 선언
function GetGlobalAbil(iAbil)

	if iAbil == GOBAL_LUA_MONSTER_KILL_SYSTEM then -- 몬스터 킬 시스템을 사용할 것인가?
		return true;
	elseif iAbil == GOBAL_LUA_MONSTER_KILL_MAX then -- 코인이 지급되야하는 몬스터 킬 수
		return 100;
	elseif iAbil == GOBAL_LUA_MONSTER_KILL_HALF_MESSAGE_COUNT then
		return 50;
	elseif iAbil == GOBAL_LUA_MONSTER_KILL_GIVE_ITEM_1 then -- 몬스터 킬이 만족 될 경우 지급 되는 아이템1
		return 20200188; -- 메달
	elseif iAbil == GOBAL_LUA_MONSTER_KILL_GIVE_ITEM_1 then -- 몬스터 킬이 만족 될 경우 지급 되는 아이템2
		return 0; -- 0 은 지급하지 않는 것
	elseif iAbil == GOBAL_LUA_MONSTER_KILL_GIVE_ITEM_1 then -- 몬스터 킬이 만족 될 경우 지급 되는 아이템3
		return 0;	
	end
end
