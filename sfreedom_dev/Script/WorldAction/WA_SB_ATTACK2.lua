-- 주먹으로 때리기 강화

iExplosionTime11 = -1
iDownPuppetID11 = nil

function WA_SB_ATTACK2_OnReceivePacket(wa_obj,Packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	wa_obj:SetParamGUID(0,g_world:GetBossGUID())		-- 보스 GUID
	wa_obj:SetParamInt(2,Packet:PopChar())			-- 0:오른주먹 1:왼주먹
	wa_obj:SetParamInt(1,Packet:PopInt())			-- 타겟 블럭 인덱스

	iStartBlockNum = wa_obj:GetParamInt(1)	
	iDownPuppetID11 = iStartBlockNum

	iExlploList[0] = iStartBlockNum

	iLeftOrRight = wa_obj:GetParamInt(2)
	bUp = false

	iDownPuppetID4 = -1	

	return true
end

function WA_SB_ATTACK2_OnEnter(wa_obj,ElapsedTimeAtStart)
	if BossActor:IsNil() then
		return
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	BossActor = g_world:FindActor(g_world:GetBossGUID())
	iExplosionTime11 = ElapsedTimeAtStart		-- 폭발 시작시간 저장

	g_bExplosion2 = false

	if iLeftOrRight == 1 then			-- 왼 주먹
		BossActor:ReserveTransitAction("a_sp_attk_09")
	elseif iLeftOrRight == 0 then

		BossActor:ReserveTransitAction("a_sp_attk_10")	-- 오른 주먹
	end
	BossActor:SetAnimSpeed(1)

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

function WA_SB_ATTACK2_OnUpdate(wa_obj,ElapsedTime)
	if BossActor:IsNil() then
		return
	end
	if wa_obj:GetParamInt(9) == 0 then
		wa_obj:SetParamInt(9, 1)			-- 본지 안본지 셋팅
		local LookPoint = Point3(wa_obj:GetParamFloat(10), wa_obj:GetParamFloat(11), 0)
		BossActor:LookAt(LookPoint)			-- 쳐다보게 만들기
		BossActor:SetAnimSpeed(1)
	end
	
	if ElapsedTime - iExplosionTime11 < iAttack1stAniTime and g_bExplosion2 == false then	-- 손 치는 애니 시간동안 대기
		return true
	elseif ElapsedTime - iExplosionTime11 >= iAttack1stAniTime and g_bExplosion2 == false then
		g_bExplosion2 = true
		puppets[0]:TransitAction("rotate_s")		-- for로 돌리면 36번을 돌려야 하니까 이게 낫다
		puppets[6]:TransitAction("rotate_s")
		puppets[12]:TransitAction("rotate_s")
		puppets[18]:TransitAction("rotate_s")
		puppets[24]:TransitAction("rotate_s")
		puppets[30]:TransitAction("rotate_s")

		puppets[iExlploList[0]]:TransitAction("down")		

		iExplosionTime11 = ElapsedTime
		return true
	end

	if ElapsedTime - iExplosionTime11 >= iAttack1stAniTime + iGrapTime1000 + 1000 and g_bExplosion2 == true and bUp == false then	-- 손을 띠면
		puppets[0]:TransitAction("rotate_e")		-- for로 돌리면 36번을 돌려야 하니까 이게 낫다
		puppets[6]:TransitAction("rotate_e")
		puppets[12]:TransitAction("rotate_e")
		puppets[18]:TransitAction("rotate_e")
		puppets[24]:TransitAction("rotate_e")
		puppets[30]:TransitAction("rotate_e")

		puppets[iExlploList[0]]:TransitAction("up")		
		return false
	end
	return true
end

function WA_SB_ATTACK2_OnLeave(wa_obj)
	iExplotionNum = -1

	return true
end
