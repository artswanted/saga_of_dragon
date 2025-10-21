-- 주먹으로 때리기 

iAttack1stAniTime = 1867	-- 첫번째 손으로 때리는 애니 시간
iExplotionNum = -1
g_iStartBlockNum4 = -1
fBlockExploInter = 1000		-- 한 블럭을 회전하는데 걸리는 시간.
g_iExploList4 = {}		-- 폭발 인덱스 리스트
g_bExplosion4 = false
iExplosionTime4 = -1
iDownBlockNum = 3		-- 쳤을 때 가라앉는 블럭 갯수
iLeftOrRight = -1		-- 왼손인지 오른손인지	
iGrapTime1000 = 1500
g_bUp4 = false
iDownPuppetID4 = -1

function WA_SB_ATTACK4_OnReceivePacket(wa_obj,Packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	wa_obj:SetParamGUID(0,g_world:GetBossGUID())		-- 보스 GUID
	wa_obj:SetParamInt(2,Packet:PopChar())			-- 0:오른주먹 1:왼주먹
	wa_obj:SetParamInt(1,Packet:PopInt())			-- 타겟 블럭 인덱스

	g_iStartBlockNum4 = wa_obj:GetParamInt(1)	

	g_iExploList4[0] = g_iStartBlockNum4 - 1
	g_iExploList4[1] = g_iStartBlockNum4 
	g_iExploList4[2] = g_iStartBlockNum4 + 1
	for i = 0, iDownBlockNum-1 do
		if g_iExploList4[i] < 0 then
			g_iExploList4[i] = g_iExploList4[i] + iTotalBlock
		elseif g_iExploList4[i] >= iTotalBlock then
			g_iExploList4[i] = g_iExploList4[i] - iTotalBlock
		end

	end
	iLeftOrRight = wa_obj:GetParamInt(2)
	g_bUp4 = false

	iDownPuppetID4 = -1

	return true
end

function WA_SB_ATTACK4_OnEnter(wa_obj,ElapsedTimeAtStart)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	BossActor = g_world:FindActor(g_world:GetBossGUID())
	if BossActor:IsNil() then
		return false
	end
	iExplosionTime4 = ElapsedTimeAtStart		-- 폭발 시작시간 저장
	g_bExplosion4 = false

	if iLeftOrRight == 1 then			-- 왼 주먹
		BossActor:ReserveTransitAction("a_sp_attk_01")
	elseif iLeftOrRight == 0 then

		BossActor:ReserveTransitAction("a_sp_attk_02")	-- 오른 주먹
	end
	BossActor:SetAnimSpeed(1)

	local LookPointBlock = g_iStartBlockNum4			-- 보스가 바라봐야 할 블럭 인덱스.
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

function WA_SB_ATTACK4_OnUpdate(wa_obj,ElapsedTime)
	if BossActor:IsNil() then
		return false
	end
	if wa_obj:GetParamInt(9) == 0 then
		wa_obj:SetParamInt(9, 1)			-- 본지 안본지 셋팅
		local LookPoint = Point3(wa_obj:GetParamFloat(10), wa_obj:GetParamFloat(11), 0)
		BossActor:LookAt(LookPoint)			-- 쳐다보게 만들기
		BossActor:SetAnimSpeed(1)
	end
	if ElapsedTime - iExplosionTime4 < iAttack1stAniTime and g_bExplosion4 == false then	-- 손 치는 애니 시간동안 대기
		return true
	elseif ElapsedTime - iExplosionTime4 >= iAttack1stAniTime and g_bExplosion4 == false then
		
		g_bExplosion4 = true
		for i = 0, iTotalBlock-1 do		-- 일단 다 뒤집은 후에
			if i%6 ~= 0 then
				puppets[i]:TransitAction("rotate_s")
			end
		end
		for i = 0, iDownBlockNum-1 do
			puppets[g_iExploList4[i]]:TransitAction("down")			
		end
		iExplosionTime4 = ElapsedTime
		ODS("다 뒤집기\n")
		return true
	end

	if ElapsedTime - iExplosionTime4 >= iAttack1stAniTime + iGrapTime1000 and g_bExplosion4 == true and g_bUp4 == false then	-- 손을 띠면
		for i = 0, iTotalBlock-1 do		-- 일단 다 뒤집은 후에
			if i%6 ~= 0 and i ~= g_iExploList4[1] then
				puppets[i]:TransitAction("rotate_e")
			end
		end

		puppets[g_iExploList4[0]]:TransitAction("up")			
		puppets[g_iExploList4[2]]:TransitAction("up")		
		iDownPuppetID4 = g_iExploList4[1]	
		g_bUp4 = true
--		return false
	elseif g_bUp4 == true and ElapsedTime - iExplosionTime4 >= iAttack1stAniTime + iGrapTime1000 + iGrapTime*1000 then
		if iDownPuppetID4 ~= -1 then
			puppets[iDownPuppetID4]:TransitAction("up")		
			iDownPuppetID4 = -1
		end
		return false
	end

	return true
end

function WA_SB_ATTACK4_OnLeave(wa_obj)
	ODS("WA_SB_ATTACK4_OnLeave\n")
	iExplotionNum = -1
	return true
end
