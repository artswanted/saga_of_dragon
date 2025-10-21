-- 긁다가 내려치기

bScratched = false
--iScratchToPunchInter = 2600
iScratchToPunchInter = 3800	-- 4배로 했을 때
bScratchToPunch = false
iPunchBlockNum = -1
iDownPuppetID13 = -1
fInterByMulti = 4.0

function WA_SB_ATTACK5_OnReceivePacket(wa_obj,Packet)	
	ODS("WA_SB_ATTACK5_OnReceivePacket\n")
	wa_obj:SetParamInt(0,Packet:PopChar())	-- 보스 회전 방향(0: RIGHT 1:LEFT) 0 반시계 방향
	wa_obj:SetParamInt(1,Packet:PopInt())	-- 회전 시작할 블록 인덱스
	wa_obj:SetParamInt(2,Packet:PopInt())	-- 회전 블록 갯수

	iExplotionNum = wa_obj:GetParamInt(2)
	iLeftOrRight = wa_obj:GetParamInt(0)
	
	-- 반시계방향으로 블럭 셋팅

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	BossActor = g_world:FindActor(g_world:GetBossGUID())	--보스 액터 갖고오기

	if BossActor:IsNil() then
		return
	end

	iStartBlockNum = wa_obj:GetParamInt(1)		-- 회전시작하는 블록의 ID를 갖고오자
	
	bExplosion = false
	bRecoverBlock = false
	iRecoverTime = -1
	iRotateIndex = 0
	bScratchToPunch = false

	iPunchBlockNum = -1

	bUp = false
	iDownPuppetID13 = -1

	return true
end

function WA_SB_ATTACK5_OnEnter(wa_obj,ElapsedTimeAtStart)
	if BossActor:IsNil() then
		return false
	end

	iExplosionTime = ElapsedTimeAtStart	-- 회전 시작시간 저장	

	local LookPointBlock = iStartBlockNum			-- 보스가 쳐다 봐야 할 블록	-- 팔과 머리 각도 계산 필요

	local iAngle = LookPointBlock*360/iTotalBlock			-- 보스가 쳐다봐야 할 각도
	local iRad = math.rad(iAngle)
	local LookPoint = Point3(math.cos(iRad),math.sin(iRad),0)
	LookPoint:Unitize()					-- 단위벡터로 만들기
	LookPoint:SetX(LookPoint:GetX() * iBlockFromCentor)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
	LookPoint:SetY(LookPoint:GetY() * iBlockFromCentor)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
	LookPoint:SetZ(0)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌

	if iLeftOrRight == 0 then		-- 오른쪽 회전	
		iPunchBlockNum = wa_obj:GetParamInt(1) + wa_obj:GetParamInt(2)
		if iPunchBlockNum >= iTotalBlock then
			iPunchBlockNum = iPunchBlockNum - iTotalBlock
		end
		BossActor:ReserveTransitAction("a_sp_attk_11")	-- 오른쪽 회전 시작 애니 반시계 방향
		iExplosionBlockID = iStartBlockNum
	else 					-- 왼쪽 회전
		iPunchBlockNum = wa_obj:GetParamInt(1) - wa_obj:GetParamInt(2)
		if iPunchBlockNum < 0 then
			iPunchBlockNum = iPunchBlockNum + iTotalBlock
		end
		BossActor:ReserveTransitAction("a_sp_attk_12")	-- 왼쪽 회전 시작 애니 시계 방향
		iExplosionBlockID = iStartBlockNum
	end
	g_iNowIndex5 = 0;

	iExlploList[0] = iPunchBlockNum- 1
	iExlploList[1] = iPunchBlockNum
	iExlploList[2] = iPunchBlockNum + 1

	for i = 0, iDownBlockNum-1 do
		if iExlploList[i] < 0 then
			iExlploList[i] = iExlploList[i] + iTotalBlock
		elseif iExlploList[i] >= iTotalBlock then
			iExlploList[i] = iExlploList[i] - iTotalBlock
		end

	end
	
	wa_obj:SetParamInt(9, 0)			-- 본지 안본지 셋팅
	wa_obj:SetParamFloat(10, LookPoint:GetX())
	wa_obj:SetParamFloat(11, LookPoint:GetY())

	return true
end

function WA_SB_ATTACK5_OnUpdate(wa_obj,ElapsedTime)
	if BossActor:IsNil() then
		return
	end
	if wa_obj:GetParamInt(9) == 0 then
		wa_obj:SetParamInt(9, 1)			-- 본지 안본지 셋팅
		local LookPoint = Point3(wa_obj:GetParamFloat(10), wa_obj:GetParamFloat(11), 0)
		BossActor:LookAt(LookPoint)			-- 쳐다보게 만들기
		BossActor:SetAnimSpeed(1)
	end
	if ElapsedTime - iExplosionTime <= i1stAniTime and bExplosion == false then	-- 손 뻗는 애니 시간동안 뒤집히면 안됨
		return true
	else
		bExplosion = true		-- 시간 지나면 폭팔 시작
	end

	if ElapsedTime - iExplosionTime >= fBlockTime and bRecoverBlock == false then	-- 블럭이 뒤집히는 시간간격보다 크면
--		ODS("뒤집기 " .. iExplosionBlockID)
		iExplosionTime = ElapsedTime			-- 시간 다시 셋팅
		puppets[iExplosionBlockID]:TransitAction("rotate_s")  -- 손 뒷쪽에선 블럭 뒤집기
		g_iNowIndex5 = g_iNowIndex5 + 1
		RotateBlockNum[iRotateIndex] = iExplosionBlockID
		iRotateIndex = iRotateIndex + 1
		if iLeftOrRight == 0 then			-- 오른손
			iExplosionBlockID = iExplosionBlockID + 1
			if iExplosionBlockID >= iTotalBlock then
				iExplosionBlockID = iExplosionBlockID - iTotalBlock
			end

			iExplosionBlockID1 = iExplosionBlockID + 1
			if iExplosionBlockID1 >= iTotalBlock then
				iExplosionBlockID1 = iExplosionBlockID1 - iTotalBlock
			end
			BossActor:AttachParticleToPoint(22345+g_iNowIndex5, Point3(PuppetPosX[iExplosionBlockID1], PuppetPosY[iExplosionBlockID1], 5), "e_ef_trap_broken" )
		else	
			BossActor:AttachParticleToPoint(22345+g_iNowIndex5, Point3(PuppetPosX[iExplosionBlockID], PuppetPosY[iExplosionBlockID], 5), "e_ef_trap_broken" )
			
			iExplosionBlockID = iExplosionBlockID - 1
			if iExplosionBlockID < 0 then		-- 왼손
				iExplosionBlockID = iExplosionBlockID + iTotalBlock
			end
		end
--		ODS(" 폭발 : " .. iExplosionBlockID .. "\n" )
	end

	if g_iNowIndex5 >= iExplotionNum then	-- 갯수만큼 터트렸으면
		if bRecoverBlock == false then
			bRecoverBlock = true
			iRecoverTime = ElapsedTime
		elseif bScratchToPunch == false and ElapsedTime - iRecoverTime >= iScratchToPunchInter/fInterByMulti then
			for i = 0, iTotalBlock-1 do
				if i%6 ~=0 then
					puppets[i]:TransitAction("rotate_s") 
				end
			end
			for i = 0, iRotateIndex-1 do
				puppets[RotateBlockNum[i]]:TransitAction("rotate_e")  -- 블럭 다시 뒤집어 제자리로				
 			end
	
			puppets[iExlploList[0]]:TransitAction("down")			
			puppets[iExlploList[1]]:TransitAction("down")			
			puppets[iExlploList[2]]:TransitAction("down")		

			bScratchToPunch = true
			iRecoverTime = ElapsedTime
		elseif bScratchToPunch == true and ElapsedTime - iRecoverTime >= iScratchToPunchInter + iGrapTime1000 then
			for i = 0, iTotalBlock-1 do
				if puppets[i]:GetCurAnimation() == 2 then
					puppets[i]:TransitAction("rotate_e") 
				end
			end
			puppets[iExlploList[0]]:TransitAction("up")			
			puppets[iExlploList[1]]:TransitAction("up")			
			puppets[iExlploList[2]]:TransitAction("up")
			return false
		end
	end
	return true
end

function WA_SB_ATTACK5_OnLeave(wa_obj)
	iExplotionNum = -1
	iPunchBlockNum = -1

	return true
end
