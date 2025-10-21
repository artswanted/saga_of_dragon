g_iStage = 0
g_fBossOpeningTime = 0.0

function READY_010801()
	-- ESC Script 등록
	RegistESCScript( "PT_C_M_REQ_INDUN_START2" )
end

function START_010801()
	ClearESCScript()
	
	OnRecoveryMiniQuest(false)
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	-- 사운드
	PlayBgSound(0)
	g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 1.0, 0.0, 3.0, false )
	g_iStage = 5
	--g_world:DetachObjectByName("M_Parel_opening 01")
	g_world:SetCamera("Camera 01")
	g_world:SetCameraMode(1, GetMyActor())
	g_world:SetCameraFixed(true) -- 카메라 고정
	
	RemoveOpeningActor_010801()
end

function RemoveOpeningActor_010801()
	local kActor = g_pilotMan:FindActorByClassNo(1030000);
	
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	GetEventScriptSystem():DeactivateAll()
	
	g_world:RemoveActorOnNextUpdate(kActor:GetPilotGuid())
	g_world:RemoveCameraAM()
	
	g_world:SetShowWorldFocusFilterColorAlpha(kColor,1,0,1,true,false);
	UI_CallMovieScene(false,1);
	g_world:SetCameraMode(1,kActor)

	UnLockPlayerInput(1)
	UnLockPlayerInputMove(1)
		
	return true
end
