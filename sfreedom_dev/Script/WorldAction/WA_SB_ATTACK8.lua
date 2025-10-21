-- 주먹으로 때리기 레이지 모드

iAttack1stAniTime8 = 933	-- 첫번째 손으로 때리는 애니 시간
iExplotionNum = -1
iStartBlockNum = -1
fBlockExploInter = 1000		-- 한 블럭을 회전하는데 걸리는 시간.
iExlploList = {}		-- 폭발 인덱스 리스트
g_bExplotion8 = false
iExplosionTime8 = -1
iDownBlockNum = 3		-- 쳤을 때 가라앉는 블럭 갯수
iLeftOrRight = -1		-- 왼손인지 오른손인지	
iGrapTime1000 = 1500
g_bUp8 = false
iDownPuppetID8 = -1

function WA_SB_ATTACK8_OnReceivePacket(wa_obj,Packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	iDownPuppetID8 = -1
	wa_obj:SetParamGUID(0,g_world:GetBossGUID())		-- 보스 GUID
	wa_obj:SetParamInt(2,Packet:PopChar())			-- 0:오른주먹 1:왼주먹
	wa_obj:SetParamInt(1,Packet:PopInt())			-- 타겟 블럭 인덱스

	iStartBlockNum = wa_obj:GetParamInt(1)	

	iExlploList[0] = iStartBlockNum - 1
	iExlploList[1] = iStartBlockNum 
	iExlploList[2] = iStartBlockNum + 1
	for i = 0, iDownBlockNum-1 do
		if iExlploList[i] < 0 then
			iExlploList[i] = iExlploList[i] + iTotalBlock
		elseif iExlploList[i] >= iTotalBlock then
			iExlploList[i] = iExlploList[i] - iTotalBlock
		end

	end
	iLeftOrRight = wa_obj:GetParamInt(2)
	g_bUp8 = false
	iDownPuppetID8 = iExlploList[1]

	return true
end

function WA_SB_ATTACK8_OnEnter(wa_obj,ElapsedTimeAtStart)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	BossActor = g_world:FindActor(g_world:GetBossGUID())
	if BossActor:IsNil() then
		BossActor:SetAnimSpeed(1)
		return false
	end
	iExplosionTime8 = ElapsedTimeAtStart		-- 폭발 시작시간 저장
	g_bExplotion8 = false
	
	if iLeftOrRight == 1 then			-- 왼 주먹
		BossActor:ReserveTransitAction("a_sp_attk_07")
	elseif iLeftOrRight == 0 then
		BossActor:ReserveTransitAction("a_sp_attk_08")	-- 오른 주먹
	end

	BossActor:SetAnimSpeed(4)

	local LookPointBlock = iStartBlockNum			-- 보스가 바라봐야 할 블럭 인덱스.
	local iAngle = LookPointBlock*360/iTotalBlock			-- 보스가 쳐다봐야 할 각도
	local iRad = math.rad(iAngle)
	local LookPoint = Point3(math.cos(iRad), math.sin(iRad),0)
	LookPoint:Unitize()					-- 단위벡터로 만들기
	LookPoint:SetX(LookPoint:GetX() * iBlockFromCentor)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
	LookPoint:SetY(LookPoint:GetY() * iBlockFromCentor)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
	LookPoint:SetZ(0)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌

	wa_obj:SetParamInt(9, 0)			-- 본지 안본지 셋팅
	wa_obj:SetParamFloat(10, LookPoint:GetX())
	wa_obj:SetParamFloat(11, LookPoint:GetY())

	return true
end

function WA_SB_ATTACK8_OnUpdate(wa_obj,ElapsedTime)
	if BossActor:IsNil() then
		BossActor:SetAnimSpeed(1)
		return false
	end
	if wa_obj:GetParamInt(9) == 0 then
		wa_obj:SetParamInt(9, 1)			-- 본지 안본지 셋팅
		local LookPoint = Point3(wa_obj:GetParamFloat(10), wa_obj:GetParamFloat(11), 0)
		BossActor:LookAt(LookPoint)			-- 쳐다보게 만들기
	end
	BossActor:SetAnimSpeed(4)

	if ElapsedTime - iExplosionTime8 < iAttack1stAniTime8 and g_bExplotion8 == false then	-- 손 치는 애니 시간동안 대기
		return true
	elseif ElapsedTime - iExplosionTime8 >= iAttack1stAniTime8 and g_bExplotion8 == false then
		g_bExplotion8 = true

		iExplosionTime8 = ElapsedTime
		BossActor:SetAnimSpeed(1)
		return false
	end

	if ElapsedTime - iExplosionTime8 >= iAttack1stAniTime8 + 100 and g_bExplotion8 == true and g_bUp8 == false then	-- 손을 띠면
		g_bUp8 = true
		BossActor:SetAnimSpeed(1)
		return false
	end
	
	return true
end

function WA_SB_ATTACK8_OnLeave(wa_obj)
	BossActor:SetAnimSpeed(1)
	iExplotionNum = -1

	return true
end
