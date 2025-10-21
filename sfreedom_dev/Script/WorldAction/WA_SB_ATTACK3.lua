-- 물어뜯기
iBiteTime = 1225		-- 물어 뜯는 타이밍
i03AniStartTime = -1
iBlockRotEndTime = -1
LeftUpsideDown = {}	
RightUpsideDown = {}
bBite = false
bBite1 = false
iBiteLongTime = 15000	-- 물고 있는 시간
iUpsideDownInterByBite = 400.0	-- 물었을 때 타일이 뒤집히는 시간 간격
bUpsideDownByBite = false
iNowUpsideDownIndex = 0
iUpsideDownBlockIDByBite = 5	-- 물었을 때 뒤집히는 타일 갯수
g_iRecoverTime3 = 0
bRecoverStart = false

function WA_SB_ATTACK3_OnReceivePacket(wa_obj,Packet)

	bBite = false
	bBite1 = false
	i03AniStartTime = 0	
	iNowUpsideDownIndex = 0
	bUpsideDownByBite = false

	wa_obj:SetParamInt(0,Packet:PopInt())		-- 타겟 블럭
	iStartBitBlockID = wa_obj:GetParamInt(0)

	for i = 0, iUpsideDownBlockIDByBite-1 do
		LeftUpsideDown[i] = iStartBitBlockID + i + 1
		if LeftUpsideDown[i] >= iTotalBlock then
			LeftUpsideDown[i] = LeftUpsideDown[i] - iTotalBlock
		end
		RightUpsideDown[i] = iStartBitBlockID - i - 1		  
		if RightUpsideDown[i] < 0 then
			RightUpsideDown[i] = RightUpsideDown[i] + iTotalBlock
		end
	end	

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	wa_obj:SetParamGUID(0, g_world:GetBossGUID());	-- 보스 GUID
	BossActor = g_world:FindActor(g_world:GetBossGUID())
	g_iRecoverTime3 = 0
	bRecoverStart = false
	iDownPuppetID4 = -1

	return true
end

function WA_SB_ATTACK3_OnEnter(wa_obj,ElapsedTimeAtStart)
	if BossActor:IsNil() then
		return false
	end

	i03AniStartTime = ElapsedTimeAtStart
	BossActor:ReserveTransitAction("a_sp_attk_03")

	local LookPointBlock = iStartBitBlockID		-- 보스가 쳐다 봐야 할 블록

	local iAngle = LookPointBlock/iTotalBlock*360
	local iRad = math.rad(iAngle)
	local LookPoint = Point3(math.cos(iRad),math.sin(iRad),0)
	LookPoint:Unitize()					-- 단위벡터로 만들기
	LookPoint:SetX(LookPoint:GetX() * iBlockFromCentor)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
	LookPoint:SetY(LookPoint:GetY() * iBlockFromCentor)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
	LookPoint:SetZ(0)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌

	wa_obj:SetParamInt(9, 0)			-- 본지 안본지 셋팅
	wa_obj:SetParamFloat(10, LookPoint:GetX())
	wa_obj:SetParamFloat(11, LookPoint:GetY())
	
	return true
end

function WA_SB_ATTACK3_OnUpdate(wa_obj,ElapsedTime)
	if BossActor:IsNil() then
		return
	end
	if wa_obj:GetParamInt(9) == 0 then
		wa_obj:SetParamInt(9, 1)			-- 본지 안본지 셋팅
		local LookPoint = Point3(wa_obj:GetParamFloat(10), wa_obj:GetParamFloat(11), 0)
		BossActor:LookAt(LookPoint)			-- 쳐다보게 만들기
		BossActor:SetAnimSpeed(1)
	end
	
	if ElapsedTime - i03AniStartTime >= iBiteTime and bBite == false and bBite1 == false then	-- 무는 타임
		bBite = true
		bBite1 = true
		i03AniStartTime = ElapsedTime
		QuakeCamera(0.8, 1.5)		-- 물면 진동
		iBlockRotEndTime = ElapsedTime			-- 물은 타이밍
		puppets[iStartBitBlockID]:TransitAction("down")	-- 물은곳 가라앉음
		iDownPuppetID4 = iStartBitBlockID
	end

	if ElapsedTime - i03AniStartTime >= iUpsideDownInterByBite and bBite == true then
		if bUpsideDownByBite == false then				-- 뒤집기
			puppets[LeftUpsideDown[iNowUpsideDownIndex]]:TransitAction("rotate_s")
			puppets[RightUpsideDown[iNowUpsideDownIndex]]:TransitAction("rotate_s")
			iNowUpsideDownIndex = iNowUpsideDownIndex + 1
			if iNowUpsideDownIndex >= iUpsideDownBlockIDByBite then
				bUpsideDownByBite = true
				bBite = false
				iNowUpsideDownIndex = iNowUpsideDownIndex - 1
			end		
		end
		i03AniStartTime = ElapsedTime
	end

	if ElapsedTime - iBlockRotEndTime >= iBiteLongTime - 11000 and bUpsideDownByBite == true then
		if bRecoverStart == false then -- 처음 시간 체크
			bRecoverStart = true
			g_iRecoverTime3 = ElapsedTime
		end
	
		if ElapsedTime - g_iRecoverTime3 >= iUpsideDownInterByBite and iNowUpsideDownIndex >= 0 then
			puppets[LeftUpsideDown[iNowUpsideDownIndex]]:TransitAction("rotate_e")
			puppets[RightUpsideDown[iNowUpsideDownIndex]]:TransitAction("rotate_e")
			iNowUpsideDownIndex = iNowUpsideDownIndex - 1
			g_iRecoverTime3 = ElapsedTime
		elseif ElapsedTime - iBlockRotEndTime >= iBiteLongTime - iBiteTime then
			return false
		end
	end

	return true
end

function WA_SB_ATTACK3_OnLeave(wa_obj)
	return true
end
