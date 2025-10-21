iSBDieStartTime = 0
iSBDieTotalTime = 12400
iEndingExlpoInter = 0
iEndingExlpoTime = 0
iParticleIndex = 9
iDieState = 0

function WA_SAPHIREBOSS_DIE_OnReceivePacket(wa_obj,Packet)
	CallUI("FRM_MOVIE_IN")
	iEndingExlpoInter = 0
	iEndingExlpoTime = 0	
	iParticleIndex = 19
	iDieState = 0
	bSB_Boss_is_Die = true
	CloseUI("FRM_BAR_BOSS")
	LockPlayerInputMove(8) -- for saphire lock
	LockPlayerInput(8)
	return true
end

function WA_SAPHIREBOSS_DIE_OnEnter(wa_obj,ElapsedTimeAtStart)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	BossActor = g_world:FindActor(g_world:GetBossGUID())
	if BossActor:IsNil() == true then
		ODS("죽을 때 보스 액터가 nil\n")
		return false
	end

	g_world:SetCameraModeMovie(2, 1, "return_fllow_SB()")	--처음 시작이면 이벤트 카메라 발동
	BossActor:ReserveTransitAction("a_sp_die")
	BossActor:AttachParticle(12345, "sapphire", "e_b_dragonsapphire_die_01_efc")
	iSBDieStartTime = ElapsedTimeAtStart
--	math.randomseed(os.data("%d%H%M%S"))
	if iDownPuppetID4 ~= -1 then      -- 내려가 있는 타일이 있으면 올림
		if puppets[iDownPuppetID4]:IsNil() == false then
			puppets[iDownPuppetID4]:TransitAction("up")
			iDownPuppetID4 = -1
		end
	end

	for i = 0, iTotalBlock-1 do
		if puppets[i]:GetCurAnimation() == 2 then
			puppets[i]:TransitAction("rotate_e") 
		elseif puppets[i]:GetCurAnimation() == 0 then
			puppets[i]:TransitAction("up") 
		end
	end

	QuakeCamera(4,1.0)

	iAngle = 27*360/iTotalBlock			-- 보스가 쳐다봐야 할 각도
	iRad = math.rad(iAngle)
	LookPoint = Point3(math.cos(iRad), math.sin(iRad),0)
	LookPoint:Unitize()					-- 단위벡터로 만들기
	LookPoint:SetX(LookPoint:GetX() * iBlockFromCentor)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
	LookPoint:SetY(LookPoint:GetY() * iBlockFromCentor)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
	LookPoint:SetZ(0)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
	
	wa_obj:SetParamInt(9, 0)			-- 본지 안본지 셋팅
	wa_obj:SetParamFloat(10, LookPoint:GetX())
	wa_obj:SetParamFloat(11, LookPoint:GetY())

	return true
end

function WA_SAPHIREBOSS_DIE_OnUpdate(wa_obj,ElapsedTime)
	if BossActor:IsNil() == true then
		ODS("죽을 때 보스 액터가 nil\n")
		return false
	end
	
--	if wa_obj:GetParamInt(9) == 0 then
--		wa_obj:SetParamInt(9, 1)			-- 본지 안본지 셋팅
		local LookPoint = Point3(wa_obj:GetParamFloat(10), wa_obj:GetParamFloat(11), 0)
		BossActor:LookAt(LookPoint)			-- 쳐다보게 만들기
--	end
	
	if ElapsedTime - iSBDieStartTime >= iSBDieTotalTime then
		ODS("WA_SAPHIREBOSS_DIE_OnUpdate return false\n")
		return false
	elseif iDieState == 0 and ElapsedTime - iSBDieStartTime >= 4216 then
		QuakeCamera(1.0, 2.0)
		iDieState = iDieState + 1
	elseif iDieState == 1 and ElapsedTime - iSBDieStartTime >= 4316 then
		QuakeCamera(8.0, 1.0)
		iDieState = iDieState + 1
	else
		if ElapsedTime - iEndingExlpoTime >= iEndingExlpoInter then
			iX = math.random(-400,400)
			iY = math.random(-400,400)
			iZ = math.random(4,400)
--			ODS("iParticleIndex : "..iParticleIndex.. " iX = " .. iX .. " iY = " .. iY .. " iZ = " .. iZ .. "\n")
			BossActor:AttachParticleToPoint(iParticleIndex, Point3(iX, iY, iZ), "e_boss_die_small_explode")

			iEndingExlpoTime = ElapsedTime
			iEndingExlpoInter = 250 + math.random(-50, 50)
			iParticleIndex = math.random(14000,100000) -- iParticleIndex + 1
			QuakeCamera(1.0, 2.0)
		end
		return true
	end
	return true
end

function WA_SAPHIREBOSS_DIE_OnLeave(wa_obj)
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	CallUI("FRM_MOVIE_OUT")
	UnLockPlayerInputMove(8) -- for saphire lock
	UnLockPlayerInput(8)
      g_world:RemoveActorOnNextUpdate(g_world:GetBossGUID())	
	return true
end
