
--[[
-------------------------- 이동형 카메라 --------------------------
-- 기본형 카메라
function underCamera_01_OnEnter(trigger, actor)
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_01_OnUpdate(trigger, actor)
	return true
end

function underCamera_01_OnLeave(trigger, actor)
	return true
end

-- 가로형 카메라
function underCamera_02_OnEnter(trigger, actor)
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_02_OnUpdate(trigger, actor)
	return true
end

function underCamera_02_OnLeave(trigger, actor)
	return true
end

-- 세로형 카메라
function underCamera_03_OnEnter(trigger, actor)
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_03_OnUpdate(trigger, actor)
	return true
end

function underCamera_03_OnLeave(trigger, actor)
	return true
end



-------------------------- 고정형 카메라 세트 --------------------------

-- 1. 고정형 카메라
function setCamera_wc01_01_OnEnter(trigger, actor)
	g_world:SetCameraPose(Point3(-807, -1811, 223), Quaternion4(0.244734, 0.663404, 0.571289, 0.416687))
	g_world:SetCameraMode(0, actor)
	return true
end

function setCamera_wc01_01_OnUpdate(trigger, actor)
	return true
end

function setCamera_wc01_01_OnLeave(trigger, actor)
	return true
end

-- 2. 고정형 카메라
function setCamera_wc01_02_OnEnter(trigger, actor)
	g_world:SetCameraPose(Point3(-808, -1811, 659), Quaternion4(0.190871, 0.680859, 0.536127, 0.461050))
	g_world:SetCameraMode(0, actor)
	return true
end

function setCamera_wc01_02_OnUpdate(trigger, actor)
	return true
end

function setCamera_wc01_02_OnLeave(trigger, actor)
	return true
end

-- 3. 고정형 카메라
function setCamera_wc01_03_OnEnter(trigger, actor)
	g_world:SetCameraPose(Point3(766, -1699, 317), Quaternion4(0.572829, 0.414568, 0.085447, 0.701925))
	g_world:SetCameraMode(0, actor)
	return true
end

function setCamera_wc01_03_OnUpdate(trigger, actor)
	return true
end

function setCamera_wc01_03_OnLeave(trigger, actor)
	return true
end

-- 4. 고정형 카메라
function setCamera_wc01_04_OnEnter(trigger, actor)
	g_world:SetCameraPose(Point3(769, -1703, 687), Quaternion4(0.577346, 0.408255, 0.104910, 0.699281))
	g_world:SetCameraMode(0, actor)
	return true
end

function setCamera_wc01_04_OnUpdate(trigger, actor)
	return true
end

function setCamera_wc01_04_OnLeave(trigger, actor)
	return true
end

-- 5. 고정형 카메라
function setCamera_wc01_05_OnEnter(trigger, actor)
	g_world:SetCameraPose(Point3(769, -1703, 1107), Quaternion4(0.577346, 0.408255, 0.104910, 0.699281))
	g_world:SetCameraMode(0, actor)
	return true
end

function setCamera_wc01_05_OnUpdate(trigger, actor)
	return true
end

function setCamera_wc01_05_OnLeave(trigger, actor)
	return true
end

-- 6. 고정형 카메라
function setCamera_wc01_06_OnEnter(trigger, actor)
	g_world:SetCameraPose(Point3(-637, -891, 265), Quaternion4(-0.023170, 0.706727, 0.618140, 0.343369))
	g_world:SetCameraMode(0, actor)
	return true
end

function setCamera_wc01_06_OnUpdate(trigger, actor)
	return true
end

function setCamera_wc01_06_OnLeave(trigger, actor)
	return true
end


-- 7. 고정형 카메라
function setCamera_wc01_07_OnEnter(trigger, actor)
	g_world:SetCameraPose(Point3(628, -809, 251), Quaternion4(0.656573, 0.262510, -0.134291, 0.694238))
	g_world:SetCameraMode(0, actor)
	return true
end

function setCamera_wc01_07_OnUpdate(trigger, actor)
	return true
end

function setCamera_wc01_07_OnLeave(trigger, actor)
	return true
end
]]

g_iStage = 0
g_fParelOpeningTime = 0.0

function Init_01503(world)
	g_iStage = 0
end

function Ready_01503(world)
	LockPlayerInputMove(7) -- for parel lock
	LockPlayerInput(7)
	CloseAllUI()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	CallUI("FRM_Parel")
	g_fParelNowTime = g_world:GetAccumTime()		-- 현재시간 저장
	g_fParelOpeningTime = 22.0
	g_iStage = 1
	
	-- ESC Script 등록
	RegistESCScript( "OnEsc_ParelOpening" )
	StopBgSound()	--배경음 끄기
	
end

function CreateOpeningActor_01503()
	local guid = GUID("123")
	guid:Generate();
	
	local pilot = g_pilotMan:NewPilot(guid, 6000921, 0,"NPC")
	
	if pilot == nil or pilot:IsNil() == true then
		return true
	end
	
	local kActor = pilot:GetActor()
	if kActor == nil or kActor:IsNil() == true then
		return true
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	if IsSingleMode() == false then
		g_pilotMan:InsertPilot(guid,pilot);
	end

	local	kSpawnPos = Point3(0, 0, 4000)
	
	kActor:FreeMove(true)
	kActor:SetUseLOD(false)
	kActor:SetUseSmoothShow(false)
	g_world:AddActor(guid, pilot:GetActor(), kSpawnPos, 6)
	
	pilot:GetActor():ClearReservedAction()
	local kAction = pilot:GetActor():ReserveTransitAction("a_opening_movie")
	kAction:SetSlot(0)
	kActor:ResetAnimation();

	pilot:GetActor():ClearActionState()
	pilot:SetAbil(AT_HP, 1000000);
	pilot:SetAbil(AT_UNIT_SIZE,2);
	pilot:SetAbil(AT_DAMAGEACTION_TYPE,0);
	pilot:SetAbil(AT_NOT_SEE_PLAYER, 0)
	pilot:SetAbil(AT_DAMAGEACTION_TYPE, 0)	
	
	g_world:SetCameraModeMovie2(kActor, 2, 1000000, "nop()")
end

function RemoveOpeningActor_01503()
	local kActor = g_pilotMan:FindActorByClassNo(6000921);
	
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:RemoveActorOnNextUpdate(kActor:GetPilotGuid())
	g_world:RemoveCameraAM()
	
	return true
end

function BossDie_01503()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:RestartAnimation("laser_trap_01")
	g_world:SetStopAnimationByName("laser_trap_01")
	g_world:HideNode("laser_trap_01", true)
	g_world:RestartAnimation("laser_trap_02")
	g_world:SetStopAnimationByName("laser_trap_02")
	g_world:HideNode("laser_trap_02", true)
	LockPlayerInput(30)
end

function RemoveParel_01503()
	UnLockPlayerInput(30)
end

function Update_01503()
	if 0 == g_iStage then
		return false
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local fTime = g_world:GetAccumTime()
	g_fParelOpeningTime = g_fParelOpeningTime - ( fTime - g_fParelNowTime )
	g_fParelNowTime = fTime

	--ODS("g_fParelOpeningTime : " .. g_fParelOpeningTime .. "\n", false, 1509)
	
	if 1 == g_iStage then
		-- SoundPlay
		--g_world:SetCameraModeByName("M_Parel_opening 01","Camera01")
		--g_world:SetStartAnimationByName("M_Parel_opening 01",0)
		--g_world:AttachSound("Parel_Opening", GetCameraTrn(true), 1,10000,10000)
		
		CreateOpeningActor_01503()
		
		CallMapMove(true)
		ODS("GetCameraTrn(true) X : " .. GetCameraTrn(true):GetX() .. " Y : " .. GetCameraTrn(true):GetY().. " Z : " .. GetCameraTrn(true):GetZ() .."\n", false, 1509)
		g_iStage = 1.1
		g_fParelNowTime = fTime		-- 현재시간 저장
	elseif 1.1 == g_iStage and g_fParelOpeningTime < 16.2 then	--펑 변신
		g_iStage = 1.5
		QuakeCamera(0.5,1,1)	
	elseif 1.5 == g_iStage and g_fParelOpeningTime < 8.9 then	--보석 받기
		g_iStage = 2
		QuakeCamera(0.2,1,1)	
		return true
	elseif 2 == g_iStage and g_fParelOpeningTime < 4.8 then	-- 고함지르기
		g_iStage = 3
		QuakeCamera(2.5,1,1)	
		return true
	elseif 3 == g_iStage and g_fParelOpeningTime < 3.5 then
		g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 0.0, 1.0, 1.0, true )
		g_iStage = 4
	elseif 4 == g_iStage and g_fParelOpeningTime < 0.0 then 
		OnEsc_ParelOpening()
	elseif 5 == g_iStage then
		g_iStage = 0
	end
	return true
end

function Start_01503(world)
	ClearESCScript()
	
	local kWnd = GetUIWnd("FRM_Parel")
	if false == kWnd:IsNil() then
		CloseUI("FRM_Parel")		
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	OnRecoveryMiniQuest(false)
	
	-- 사운드
	PlayBgSound(0)
	g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 1.0, 0.0, 3.0, false )
	g_iStage = 5
	g_world:DetachObjectByName("M_Parel_opening 01")
	g_world:SetCamera("Camera 01")
	g_world:SetCameraMode(1, GetMyActor())
	g_world:SetCameraFixed(true) -- 카메라 고정
	CallUI("FRM_GOGO")
	
	RemoveOpeningActor_01503()

	local kWnd = GetUIWnd("FRM_GOGO")
	if kWnd:IsNil() then
		g_iStage = 5
		CloseUI("FRM_MOVIE_IN")
		UnLockPlayerInputMove(7) -- for parel lock
		UnLockPlayerInput(7)
		g_world:SetCameraZoom(1.0) -- 줌최대뒤로
			
	end
	UI_DefaultActivate( true )
	CallMapMove(false)
end

function OnEsc_ParelOpening()
	g_iStage = 4
	local kPacket = NewPacket(13201) -- PT_C_M_REQ_INDUN_START
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

--[[
function Actor_Parel_Hit(actor, keyName)

	if keyName == "hit" then
		QuakeCamera(1.0, 1.0)		-- 손으로 때렸을 때 화면 진동 (시간, 진동)
	elseif keyName == "down" then
		QuakeCamera(2.0, 1.0)
	elseif keyName == "grap" then
		QuakeCamera(0.5, 1.0)
	elseif keyName == "cam01" then	-- 손으로 벽치기
		actor:DetachFrom(101)
		local hand = actor:GetNodeTranslate("p_ef_l_hand")
		actor:AttachParticleToPoint(114, hand, "e_boss_die_small_explode")

		QuakeCamera(0.8, 1.0)
	elseif keyName == "scream" then
		QuakeCamera(2.5, 1.0)
	elseif keyName == "skill_02" then
		QuakeCamera(0.5, 1.0)
	elseif keyName == "hit_skill_01" then
		ODS("keyName == hit_skill_01 \n")
		QuakeCamera(1.0, 1.0)
		local NowPos = actor:GetTranslate()
		local Dir = actor:GetLookingDir()
		Dir:SetZ(0)
		Dir:Unitize()
		Dir:Multiply(73.622048)	-- 187을 인치로
		NowPos:Add(Dir)
		NowPos:SetZ(NowPos:GetZ()-20)	-- 임시
		actor:AttachParticleToPoint(104, NowPos, "e_ef_parel_skill_01")
	elseif keyName == "skill_04" then	-- 공중 공격
		QuakeCamera(1.0, 1.0)
		local NowPos = actor:GetTranslate()
		NowPos:SetZ(5.274)	-- 임시
		actor:AttachParticleToPoint(114, NowPos, "e_ef_parel_skill_04")
	end
  
	return true
end  ]]


