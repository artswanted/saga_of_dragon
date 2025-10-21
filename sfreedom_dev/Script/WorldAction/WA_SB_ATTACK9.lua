-- 양손 긁기

LeftBlockList = {}    -- 왼손
RightBlockList = {}   -- 오른손
iScratchNum = 9       -- 한 손이 긁을 칸 수
iScratchAniTime = 2348  -- 손 긁는 애니 시작까지 걸리는 시간. 현재 임시
fScratchBlockTime = 400

function WA_SB_ATTACK9_OnReceivePacket(wa_obj,Packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	wa_obj:SetParamInt(0,Packet:PopInt())	-- 쳐다봐야 할 인덱스

	BossActor = g_world:FindActor(g_world:GetBossGUID())	--보스 액터 갖고오기

	if BossActor:IsNil() then	
		return false
	end

	local i = wa_obj:GetParamInt(0)		-- 쳐다 볼 블록의 ID를 갖고오자
	
	local iFirstLeft = i + iScratchNum
	local iFirstRight = i - iScratchNum
	if iFirstLeft >= iTotalBlock then
		iFirstLeft = iFirstLeft - iTotalBlock
	end
	if iFirstRight < 0 then
		iFirstRight = iFirstRight + iTotalBlock
	end
	
--	ODS("왼손 : ")	
	for idx = 0, iScratchNum do		-- 루프를 편하기 돌리기 위해 정리된 인덱스를 따로 저장하자
		RightBlockList[idx] = idx + iFirstRight -- 오른손
		if RightBlockList[idx] >= iTotalBlock then	-- 블록 갯수보다 크면
			RightBlockList[idx] = RightBlockList[idx] - iTotalBlock 
		end
	end
--	ODS("\n오른손 : ")

	for idx = 0, iScratchNum do		-- 루프를 편하기 돌리기 위해 정리된 인덱스를 따로 저장하자
		LeftBlockList[idx] = iFirstLeft - idx  -- 왼손
		if LeftBlockList[idx] < 0  then		-- 블록 갯수보다 크면
			LeftBlockList[idx] = LeftBlockList[idx] + iTotalBlock 
		end
	end
	g_iNowIndex9 = 0
	g_bExplosion9 = false

	return true
end

function WA_SB_ATTACK9_OnEnter(wa_obj, ElapsedTimeAtStart) 	-- ElapsedTimeAtStart 0이상이면 중간에 난입한것
	if BossActor:IsNil() then
		return false
	end
	if wa_obj:GetParamInt(9) == 0 then
		wa_obj:SetParamInt(9, 1)			-- 본지 안본지 셋팅
		local LookPoint = Point3(wa_obj:GetParamFloat(10), wa_obj:GetParamFloat(11), 0)
		BossActor:LookAt(LookPoint)			-- 쳐다보게 만들기
	end

	iExplosionTime = ElapsedTimeAtStart	-- 회전 시작시간 저장	

	local LookPointBlock = wa_obj:GetParamInt(0)		-- 보스가 쳐다 봐야 할 블록	-- 팔과 머리 각도 계산 필요

	local iAngle = LookPointBlock*360/iTotalBlock			-- 보스가 쳐다봐야 할 각도
	local iRad = math.rad(iAngle)
	local LookPoint = Point3(math.cos(iRad),math.sin(iRad),0)
	LookPoint:Unitize()					-- 단위벡터로 만들기
	LookPoint:SetX(LookPoint:GetX() * iBlockFromCentor)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
	LookPoint:SetY(LookPoint:GetY() * iBlockFromCentor)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
	LookPoint:SetZ(0)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
	
	BossActor:ReserveTransitAction("a_sp_attk_13")
	
	wa_obj:SetParamInt(9, 0)			-- 본지 안본지 셋팅
	wa_obj:SetParamFloat(10, LookPoint:GetX())
	wa_obj:SetParamFloat(11, LookPoint:GetY())

	return true
end

function WA_SB_ATTACK9_OnUpdate(wa_obj,ElapsedTime)
	if BossActor:IsNil() then
		return false
	end
	
	if wa_obj:GetParamInt(9) == 0 then
		wa_obj:SetParamInt(9, 1)			-- 본지 안본지 셋팅
		local LookPoint = Point3(wa_obj:GetParamFloat(10), wa_obj:GetParamFloat(11), 0)
		BossActor:LookAt(LookPoint)			-- 쳐다보게 만들기
	end

	if ElapsedTime - iExplosionTime <= iScratchAniTime and g_bExplosion9 == false then	-- 손 뻗는 애니 시간동안 뒤집히면 안됨
		return true
	else
		g_bExplosion9 = true		-- 시간 지나면 폭팔 시작
	end
	
	if ElapsedTime - iExplosionTime >= fScratchBlockTime then	-- 블럭이 뒤집히는 시간간격보다 크면
		iExplosionTime = ElapsedTime			-- 시간 다시 셋팅
		
		if g_iNowIndex9 >= iScratchNum then	-- 갯수만큼 터트렸으면
			for i = 0, iScratchNum-1 do
				puppets[RightBlockList[i]]:TransitAction("rotate_e")  -- 블럭 다시 뒤집어 제자리로
				puppets[LeftBlockList[i]]:TransitAction("rotate_e")  -- 블럭 다시 뒤집어 제자리로
	 		end
			return false
		end

--		ODS("뒤집기 왼손 " .. LeftBlockList[g_iNowIndex9] .. " 오른손 " .. RightBlockList[g_iNowIndex9] .. "\n")
		puppets[RightBlockList[g_iNowIndex9]]:TransitAction("rotate_s")  -- 손 뒷쪽에선 블럭 뒤집기
		puppets[LeftBlockList[g_iNowIndex9]]:TransitAction("rotate_s")  -- 손 뒷쪽에선 블럭 뒤집기
		QuakeCamera(0.5, 1.0)
		g_iNowIndex9 = g_iNowIndex9 + 1
		if g_iNowIndex9 < iScratchNum then
			iExplo = g_iNowIndex9 + 1

			BossActor:AttachParticleToPoint(22345+g_iNowIndex9, Point3(PuppetPosX[RightBlockList[iExplo]], PuppetPosY[RightBlockList[iExplo]], 5), "e_ef_trap_broken" )
			BossActor:AttachParticleToPoint(32345+g_iNowIndex9, Point3(PuppetPosX[LeftBlockList[g_iNowIndex9-1]], PuppetPosY[LeftBlockList[g_iNowIndex9-1]], 5), "e_ef_trap_broken" )
--			ODS("폭팔 왼손 " .. LeftBlockList[g_iNowIndex9] .. " 오른손 " .. RightBlockList[g_iNowIndex9] .. "\n")
--			ODS("g_iNowIndex9 : " .. g_iNowIndex9 .. "\n")
		end
	end
	
	return true
end

function WA_SB_ATTACK9_OnLeave(wa_obj)
	iStartBlockNum = -1			-- 값 초기화
	return true
end
