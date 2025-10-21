-- 손바닥 긁기

SortBlockNum = {}
iStartBlockNum = -1
iTotalBlock = 36	-- 총갯수36 임시
fBlockTime = 400	-- 블록 하나가 뒤집히는 시간 -- 몸통 돌리는것 보다 약간 빠르게 수정.
BossActor = nil 
iBlockFromCentor = 460
BossAction = nil
LookPoint = nil		-- 보스가 공격할 블럭의 Point3
LookPointBlock = nil	-- 보스가 공격할 블럭 인덱스
g_iNowIndex1 = nil		-- 시간이 지난 후의 시작 블럭. 난입이 아닐경우 그냥 시작 블럭 인덱스와 같다
iExplosionTime = 0
i1stAniTime = 1467	-- 손 뻗는 애니 시간
g_bExplosion1 = false
iLeftOrRight = -1
iExplosionBlockID = -1

iExplotionNum = -1
bRecoverBlock = false
iRecoverTime = -1
iWaitForRecoverBlockTime = 5000
iRotateIndex = 0
RotateBlockNum = {}

PuppetPosX = { 460, 453.01156638562, 432.25860556152, 398.37168574084, 352.38044383473, 295.68230045581, 230, 
157.32926592981, 79.878161726788, 0, -79.878161726788, -157.32926592981, -230, -295.68230045581, -352.38044383473, 
-398.37168574084, -432.25860556152, -453.01156638562, -460, -453.01156638562, -432.25860556152, -398.37168574084, 
-352.38044383473, -295.68230045581, -230, -157.32926592981, -79.878161726788, 0, 79.878161726788, 157.32926592981, 
230, 295.68230045581, 352.38044383473, 398.37168574084, 432.25860556152, 453.01156638562}

PuppetPosY = { 0, 79.878161726788, 157.32926592981, 230, 295.68230045581, 352.38044383473, 398.37168574084, 
432.25860556152, 453.01156638562, 460, 453.01156638562, 432.25860556152, 398.37168574084, 352.38044383473, 
295.68230045581, 230, 157.32926592981, 79.878161726788, 0, -79.878161726788, -157.32926592981, -230, -295.68230045581, 
-352.38044383473, -398.37168574084, -432.25860556152, -453.01156638562, -460, -453.01156638562, -432.25860556152, 
-398.37168574084, -352.38044383473, -295.68230045581, -230, -157.32926592981, -79.878161726788 }



function WA_SB_ATTACK1_OnReceivePacket(wa_obj,Packet)
-- 이 두개는 자동으로 c++소스에서 받아줌
--	wa_obj:SetParamInt(0,Packet:PopInt())	-- 월드액션 ID
--	wa_obj:SetParamInt(1,Packet:PopInt())	-- 흐른 시간	

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

	local i = iStartBlockNum
	if wa_obj:GetParamInt(0) == 0 then		-- 오른쪽 회전
		for idx = 0, iExplotionNum-1 do		-- 루프를 편하기 돌리기 위해 정리된 인덱스를 따로 저장하자
			SortBlockNum[idx] = i
			if( i >= iTotalBlock  )	then	-- 블록 갯수보다 크면
				i = i - iTotalBlock 
			end
		end
	elseif wa_obj:GetParamInt(0) == 1 then		-- 왼쪽 회전
		for idx = iExplotionNum-1, 0, -1 do	-- 루프를 편하기 돌리기 위해 정리된 인덱스를 따로 저장하자
			SortBlockNum[idx] = i
			if( i < 0  ) then		-- 블록 갯수보다 크면
				i = i + iTotalBlock 
			end
		end
	end
	g_bExplosion1 = false
	bRecoverBlock = false
	iRecoverTime = -1
	iRotateIndex = 0

	return true
end

function WA_SB_ATTACK1_OnEnter(wa_obj, ElapsedTimeAtStart) 	-- ElapsedTimeAtStart 0이상이면 중간에 난입한것
	if BossActor:IsNil() then
		return
	end

	iExplosionTime = ElapsedTimeAtStart	-- 회전 시작시간 저장	

	LookPointBlock = iStartBlockNum			-- 보스가 쳐다 봐야 할 블록	-- 팔과 머리 각도 계산 필요

	local iAngle = LookPointBlock*360/iTotalBlock			-- 보스가 쳐다봐야 할 각도
	local iRad = math.rad(iAngle)
	local LookPoint = Point3(math.cos(iRad),math.sin(iRad),0)
	LookPoint:Unitize()					-- 단위벡터로 만들기
	LookPoint:SetX(LookPoint:GetX() * iBlockFromCentor)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
	LookPoint:SetY(LookPoint:GetY() * iBlockFromCentor)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
	LookPoint:SetZ(0)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌

	if iLeftOrRight == 0 then		-- 오른쪽 회전	
		BossActor:ReserveTransitAction("a_sp_attk_04")	-- 오른쪽 회전 시작 애니 반시계 방향
		iExplosionBlockID = iStartBlockNum
	else 					-- 왼쪽 회전
		BossActor:ReserveTransitAction("a_sp_attk_05")	-- 왼쪽 회전 시작 애니 시계 방향
		iExplosionBlockID = iStartBlockNum
	end
	g_iNowIndex1 = 0;
	
	wa_obj:SetParamInt(9, 0)			-- 본지 안본지 셋팅
	wa_obj:SetParamFloat(10, LookPoint:GetX())
	wa_obj:SetParamFloat(11, LookPoint:GetY())

	return true
end

function WA_SB_ATTACK1_OnUpdate(wa_obj,ElapsedTime)
	if BossActor:IsNil() then
		return
	end
	
	if wa_obj:GetParamInt(9) == 0 then
		wa_obj:SetParamInt(9, 1)			-- 본지 안본지 셋팅
		local LookPoint = Point3(wa_obj:GetParamFloat(10), wa_obj:GetParamFloat(11), 0)
		BossActor:LookAt(LookPoint)			-- 쳐다보게 만들기
	end

	if ElapsedTime - iExplosionTime <= i1stAniTime and g_bExplosion1 == false then	-- 손 뻗는 애니 시간동안 뒤집히면 안됨
		return true
	else
		g_bExplosion1 = true		-- 시간 지나면 폭팔 시작
	end
	
	if ElapsedTime - iExplosionTime >= fBlockTime and bRecoverBlock == false then	-- 블럭이 뒤집히는 시간간격보다 크면
--		ODS("뒤집기 " .. iExplosionBlockID)
		iExplosionTime = ElapsedTime			-- 시간 다시 셋팅
		puppets[iExplosionBlockID]:TransitAction("rotate_s")  -- 손 뒷쪽에선 블럭 뒤집기
		QuakeCamera(0.5, 1.0)
		g_iNowIndex1 = g_iNowIndex1 + 1
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
			BossActor:AttachParticleToPoint(22345+g_iNowIndex1, Point3(PuppetPosX[iExplosionBlockID1], PuppetPosY[iExplosionBlockID1], 5), "e_ef_trap_broken" )
		else	
			BossActor:AttachParticleToPoint(22345+g_iNowIndex1, Point3(PuppetPosX[iExplosionBlockID], PuppetPosY[iExplosionBlockID], 5), "e_ef_trap_broken" )
			
			iExplosionBlockID = iExplosionBlockID - 1
			if iExplosionBlockID < 0 then		-- 왼손
				iExplosionBlockID = iExplosionBlockID + iTotalBlock
			end
		end
--		ODS(" 폭발 : " .. iExplosionBlockID .. "\n" )

--		if iExplosionBlockID%6 ~= 0 then
--			puppets[iExplosionBlockID]:AttachParticle(12345+g_iNowIndex1, "block_s", "e_ef_trap_broken")
--		else
--			puppets[iExplosionBlockID]:AttachParticle(12345+g_iNowIndex1, "block_b", "e_ef_trap_broken")
--		end
	end

	if g_iNowIndex1 >= iExplotionNum then	-- 갯수만큼 터트렸으면
		if bRecoverBlock == false then
			bRecoverBlock = true
			iRecoverTime = ElapsedTime
		elseif ElapsedTime - iRecoverTime >= iWaitForRecoverBlockTime then
			for i = 0, iRotateIndex-1 do
				puppets[RotateBlockNum[i]]:TransitAction("rotate_e")  -- 블럭 다시 뒤집어 제자리로
 			end
			return false
		end
	end
	return true
end

function WA_SB_ATTACK1_OnLeave(wa_obj)
	iStartBlockNum = -1			-- 값 초기화
	return true
end
