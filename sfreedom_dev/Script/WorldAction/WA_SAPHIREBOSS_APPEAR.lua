Useractor = nil
iSaphireBossAppearTime = -1
bLeftHandTouch = false
bRightHandTouch = false
iLeftHandTouchTime = 1500
iRightHandTouchTime = 4350
iOpeningEndTime = 12767
bOpening = false
fRotZ = 0.0
i06CameraState = 1	-- 06번 공격 카메라 상태
i06CameraInter = {}
i06CameraInter[1] = 123
i06CameraInter[2] = 2743
i06CameraInter[3] = 403
i06CameraInter[4] = 2033
i06CameraInter[5] = 1196
i06CameraInter[6] = 147
i06CameraAngleInter = {}
i06CameraAngleInter[1] = 0
i06CameraAngleInter[2] = 30
i06CameraAngleInter[3] = 40
i06CameraAngleInter[4] = 0
i06CameraAngleInter[5] = -70
i06CameraAngleInter[6] = 0
i06CameraTrnInter = {}
i06CameraTrnInter[1] = Point3(0,0,0)
i06CameraTrnInter[2] = Point3(0,0,0)
i06CameraTrnInter[3] = Point3(0,60,0)
i06CameraTrnInter[4] = Point3(0,0,0)
i06CameraTrnInter[5] = Point3(0,-60,0)
i06CameraTrnInter[6] = Point3(0,0,0)
i06CameraFOVInter = {}
i06CameraFOVInter[1] = 0
i06CameraFOVInter[2] = 0.06
i06CameraFOVInter[3] = 0.15
i06CameraFOVInter[4] = 0
i06CameraFOVInter[5] = -0.21
i06CameraFOVInter[6] = 0

bSB_Boss_is_Die = false

function WA_SAPHIREBOSS_APPEAR_OnReceivePacket(wa_obj,Packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if g_world:GetBossGUID():IsNil() == true then
		ODS("g_world:GetBossGUID() OnReceivePacket\n")
			return false
	end

	BossActor = g_world:FindActor(g_world:GetBossGUID())

	if BossActor:IsNil() == true then
		ODS("BossActor is Nil OnReceivePacket\n")
		return false
	end
	CallUI("FRM_MOVIE_IN")
	CallUI("FRM_Gremgris")
	BossActor:SetBoss()
	bLeftHandTouch = false
	bRightHandTouch = false
	bOpening = false
	bSB_Boss_is_Die = false
	LockPlayerInputMove(8) -- for saphire lock
	LockPlayerInput(8)
	
	return true
end

function WA_SAPHIREBOSS_APPEAR_OnEnter(wa_obj,ElapsedTimeAtStart) 
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end	
	if g_world:GetBossGUID():IsNil() == true then
		ODS("g_world:GetBossGUID() OnEnter\n")
		return false
	end
	BossActor = g_world:FindActor(g_world:GetBossGUID())
	if BossActor:IsNil() == true then
		ODS("BossActor is Nil onEnter\n")
      CallUI("FRM_MOVIE_OUT")
		return false
	end
 	ODS("WA_SAPHIREBOSS_APPEAR_OnEnter\n")
	iSaphireBossAppearTime = ElapsedTimeAtStart	-- 등장 시작시간 저장

--	BossActor:SetTargetAnimation("openning")
	BossActor:ReserveTransitAction("a_sp_appear")
	BossActor:SetHide(false)
	BossActor:AttachParticle(12351, "char_root", "e_b_dragonsapphire_opening_01_efc")
	BossActor:AttachParticle(12346, "Plane05", "e_b_dragonsapphire_eye_L")
	BossActor:AttachParticle(12350, "Plane04", "e_b_dragonsapphire_eye_R")
	BossActor:AttachParticle(12348, "Bip01 L Hand", "e_b_dragonsapphire_hand_L")
	BossActor:AttachParticle(12349, "Bip01 R Hand", "e_b_dragonsapphire_hand_R")

	g_world:SetCameraModeMovie(2, 0, "return_fllow_SB()")	--처음 시작이면 이벤트 카메라 발동
	QuakeCamera(iOpeningEndTime/1000.0, 1.0)

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

function WA_SAPHIREBOSS_APPEAR_OnUpdate(wa_obj,ElapsedTime)	--return false 이면 액션 삭제 및 leave 호출
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if g_world:GetBossGUID():IsNil() == true then
		ODS("g_world:GetBossGUID() OnUpdate\n")
			return false
	end
	BossActor = g_world:FindActor(g_world:GetBossGUID())
	if BossActor:IsNil() == true then
		ODS("BossActor is Nil OnUpdate\n")
		return false
	end

	if wa_obj:GetParamInt(9) == 0 then
		wa_obj:SetParamInt(9, 1)			-- 본지 안본지 셋팅
		local LookPoint = Point3(wa_obj:GetParamFloat(10), wa_obj:GetParamFloat(11), 0)
		BossActor:LookAt(LookPoint)			-- 쳐다보게 만들기
	end

	if ElapsedTime - iSaphireBossAppearTime >= iLeftHandTouchTime and bLeftHandTouch == false then
		bLeftHandTouch = true
	elseif ElapsedTime - iSaphireBossAppearTime >= iRightHandTouchTime and bRightHandTouch == false then
		bRightHandTouch = true
	elseif bOpening == false and ElapsedTime - iSaphireBossAppearTime >= iOpeningEndTime then	-- 오프닝 시간
		ODS("CallUI[FRM_MOVIE_OUT]")
		CallUI("FRM_MOVIE_OUT")
		bOpening = true
		UnLockPlayerInputMove(8) -- for saphire lock
		UnLockPlayerInput(8)
	elseif ElapsedTime - iSaphireBossAppearTime >= iOpeningEndTime + 3000 then	-- 오프닝 시간
		SetBossGUIDForHPBar(g_world:GetBossGUID())
		return false
	end
	return true
end

function WA_SAPHIREBOSS_APPEAR_OnLeave(wa_obj)
	return true
end

function Actor_Sapphire_Dragon_Hit(actor, keyName)
	if keyName == "hit01" or keyName == "hit02" or keyName == "hit_07" then
		QuakeCamera(0.8, 2.5)
	elseif keyName == "hit" then
		QuakeCamera(1.0, 1.5)		-- 손으로 때렸을 때 화면 진동 (시간, 진동)
	elseif keyName == "hit_opening01" then
		QuakeCamera(2.007, 1.5)
	elseif keyName == "hit_opening02" then
		QuakeCamera(8.482, 1.0)
	elseif keyName == "hit_explo" then
		QuakeCamera(1.0, 2.5)
--	elseif keyName == "Attk06_Start" or keyName == "Attk06_Up" or keyName == "Attk06_Stay" or keyName == "Attk06_Down" or keyName == "Attk06_End" then
--		i06CameraState = i06CameraState + 1
--		ODS("카메라 상태 증가 : " .. i06CameraState .. "\n")
	end
  
	return true
end

function return_fllow_SB()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	pilot = g_pilotMan:GetPlayerPilot()
	g_world:SetCameraMode(3, pilot:GetActor())
	SetCameraSetting(Point3(0, 380, 125), Point3(24, 0, 112))
	SetCameraSpeed(13.0, 2.0, 10.0, 4.5)
end
