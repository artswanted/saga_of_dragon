g_iStage = 0
g_fBossOpeningTime = 0.0

function Ready_030300(world)

	CallUI("FRM_Paris")

	-- ESC Script 등록
	RegistESCScript( "PT_C_M_REQ_INDUN_START" )
end

function Start_030300(world)
	ClearESCScript()
	
	OnRecoveryMiniQuest(false)
	
	-- 사운드
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	PlayBgSound(0)
	g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 1.0, 0.0, 3.0, false )
	g_iStage = 5
	--g_world:DetachObjectByName("M_Parel_opening 01")
	g_world:SetCamera("Camera 01")
	g_world:SetCameraMode(1, GetMyActor())
	g_world:SetCameraFixed(true) -- 카메라 고정
	
	RemoveOpeningActor_030300()
end

function RemoveOpeningActor_030300()
	local kActor = g_pilotMan:FindActorByClassNo(1030000);
	
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:RemoveActorOnNextUpdate(kActor:GetPilotGuid())
	g_world:RemoveCameraAM()
	
	return true
end
