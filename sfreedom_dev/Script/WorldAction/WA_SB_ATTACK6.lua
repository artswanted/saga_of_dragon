-- 전체 마법 쓰기

iGlobalMagicTime = 3378	-- 광역마법 발사 시간
i06AniStartTime = 0	-- 6번 애니 시작 시간
bGlobalMagic = false	-- 한번만 떨기
i06AniTotalTime = 6667		-- 애니 총 시간
i06CameraTime = 0
i06FrameTime = 0
fOriginZ = 0
OriginTrn = Point3(0,0,0)

function WA_SB_ATTACK6_OnReceivePacket(wa_obj,Packet)
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	wa_obj:SetParamGUID(0, g_world:GetBossGUID());	-- 보스 GUID
	BossActor = g_world:FindActor(wa_obj:GetParamGUID(0))
	bGlobalMagic = false
	i06AniStartTime = 0
	i06CameraState = 1
	i06FrameTime = 0
	return true
end

function WA_SB_ATTACK6_OnEnter(wa_obj,ElapsedTimeAtStart)
	if BossActor:IsNil() then
		ODS("BossActor:IsNil()\n")
		return false
	end
	BossActor:ReserveTransitAction("a_sp_attk_06")		-- 애니 시작
	BossActor:AttachParticle(12345, "char_root", "e_b_dragonsapphire_attk_06_efc")
	i06AniStartTime = ElapsedTimeAtStart
	i06CameraTime = ElapsedTimeAtStart
	fOriginZ = GetCameraRot():GetZ()
	OriginTrn = GetCameraTrn()
	ODS("WA_SB_ATTACK6_OnEnter\n")
	
	OriginLeft = GetFrustumLeft()
	OriginRight = GetFrustumRight()
	OriginTop = GetFrustumTop()
	OriginBottom = GetFrustumBottom()
	ODS("OriginLeft : " .. OriginLeft .. " OriginRight : " .. OriginRight .. " OriginTop : " .. OriginTop .. "\n")

	local now = BossActorGetMovingDir()
	wa_obj:SetParamInt(8, 0)
	wa_obj:SetParamFloat(9, now:GetX())
	wa_obj:SetParamFloat(10, now:GetY())

	return true
end

function WA_SB_ATTACK6_OnUpdate(wa_obj,ElapsedTime)
	if BossActor:IsNil() then
		return false
	end
	if 	wa_obj:GetParamInt(8) == 0 then
		BossActor:LookAt(Point3( wa_obj:GetParamFloat(9), wa_obj:GetParamFloat(10), 0 ) )
	end

	if ElapsedTime - i06CameraTime < i06CameraInter[i06CameraState] then
		local fMul = (ElapsedTime - i06FrameTime)/i06CameraInter[i06CameraState]

		local fAdd = fMul*i06CameraAngleInter[i06CameraState]

		local fTrnX = fMul*i06CameraTrnInter[i06CameraState]:GetX()
		local fTrnY = fMul*i06CameraTrnInter[i06CameraState]:GetY()
		local fTrnZ = fMul*i06CameraTrnInter[i06CameraState]:GetZ()

--		ODS("fTrnX : " .. fTrnX .. " fTrnY : " .. fTrnY .. " fTrnZ : " .. fTrnZ .. "\n")

		local kRot = GetCameraRot()
		kRot:SetZ(kRot:GetZ() + fAdd)
		local kTrn = GetCameraTrn()
		kTrn:SetX(kTrn:GetX() + fTrnX)
		kTrn:SetY(kTrn:GetY() + fTrnY)
		kTrn:SetZ(kTrn:GetZ() + fTrnZ)
		SetCameraSetting(kTrn, kRot)

		local fLeft = fMul*i06CameraFOVInter[i06CameraState]*OriginLeft + GetFrustumLeft()
		local fRight = fMul*i06CameraFOVInter[i06CameraState]*OriginRight + GetFrustumRight()
		local fTop = fMul*i06CameraFOVInter[i06CameraState]*OriginTop + GetFrustumTop()
		local fBottom = fMul*i06CameraFOVInter[i06CameraState]*OriginBottom + GetFrustumBottom()
		SetFrustum(fLeft, fRight, fTop, fBottom) 

		i06FrameTime = ElapsedTime
	else
		local iOverTime = ElapsedTime - i06CameraTime - i06CameraInter[i06CameraState]
		local iLeftTime = ElapsedTime - i06FrameTime - iOverTime
		local fMul = (iLeftTime)/i06CameraInter[i06CameraState]
		local fAdd = fMul*i06CameraAngleInter[i06CameraState]
		
		local fTrnX = fMul*i06CameraTrnInter[i06CameraState]:GetX()
		local fTrnY = fMul*i06CameraTrnInter[i06CameraState]:GetY()
		local fTrnZ = fMul*i06CameraTrnInter[i06CameraState]:GetZ()

		local kRot = GetCameraRot()
		kRot:SetZ(kRot:GetZ() + fAdd)
		local kTrn = GetCameraTrn()
		kTrn:SetX(kTrn:GetX() + fTrnX)
		kTrn:SetY(kTrn:GetY() + fTrnY)
		kTrn:SetZ(kTrn:GetZ() + fTrnZ)
		SetCameraSetting(kTrn, kRot)

		local fLeft = fMul*i06CameraFOVInter[i06CameraState]*OriginLeft + GetFrustumLeft()
		local fRight = fMul*i06CameraFOVInter[i06CameraState]*OriginRight + GetFrustumRight()
		local fTop = fMul*i06CameraFOVInter[i06CameraState]*OriginTop + GetFrustumTop()
		local fBottom = fMul*i06CameraFOVInter[i06CameraState]*OriginBottom + GetFrustumBottom()
		SetFrustum(fLeft, fRight, fTop, fBottom) 
		
		i06CameraTime = ElapsedTime - iLeftTime
		i06FrameTime = ElapsedTime - iLeftTime
		i06CameraState = i06CameraState + 1
		ODS("시간 다시 측정 i06CameraState : " .. i06CameraState .. "\n")
	end

	if ElapsedTime - i06AniStartTime >= iGlobalMagicTime - 1430 and bGlobalMagic == false then -- 광역 쓸 시간이 됬으면
		bGlobalMagic = true
		for i = 0, iTotalBlock-1 do
	  		if i % 6 ~= 0 then
	  			puppets[i]:TransitAction("explosion")	-- 빛 발하기
		  		puppets[i]:AttachParticle(12345+i, "block_s", "e_ef_trap_explosion")
		  	end
		end
	elseif i06CameraState > 6 or ElapsedTime - i06AniStartTime >= 6667 then
		ODS("전체공격 종료\n")
		local kRot = GetCameraRot()
		kRot:SetZ(fOriginZ)
		SetCameraSetting(OriginTrn, kRot)
		ODS("마지막 OriginLeft : " .. OriginLeft .. " OriginRight : " .. OriginRight .. " OriginTop : " .. OriginTop .. "\n")
--		MessageBox("걸려랏")
		SetFrustum(OriginLeft, OriginRight, OriginTop, OriginBottom) 
		
		return false
	end		

	return true
end

function WA_SB_ATTACK6_OnLeave(wa_obj)
	return true
end
