g_iStage = 0
g_fBossOpeningTime = 0.0

function Init_030702(world)
	--ODS("Init_030702\n",false,987)
	g_iStage = 0
end

function Ready_030702(world)
	--ODS("Ready_030702\n",false,987)
	LockPlayerInputMove(8) -- for lock
	LockPlayerInput(8)
	CloseAllUI()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	CallUI("FRM_Paris")
	g_iStage = 1
	g_fBossOpeningTime = 25
	g_fParelNowTime = g_world:GetAccumTime()

	-- ESC Script 등록
	RegistESCScript( "PT_C_M_REQ_INDUN_START" )
	StopBgSound()	--배경음 끄기
end

function Update_030702()
	--ODS("Update_030702\n",false,987)
	if 0 == g_iStage then
		return false
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local fDeltaTime = g_world:GetAccumTime() - g_fParelNowTime

	if 1 == g_iStage then
		--BoneDragonCreateOpeningActor()
		CallMapMove(true)
		g_fParelNowTime = g_world:GetAccumTime()
		g_iStage = 3
	elseif 3 == g_iStage and (fDeltaTime >= g_fBossOpeningTime or g_MovieMgr:IsPlay() == false)  then
		PT_C_M_REQ_INDUN_START()
	
	elseif 5 == g_iStage then
		g_iStage = 0
		UI_DefaultActivate( true )
		UnLockPlayerInputMove(8) -- for  lock
		UnLockPlayerInput(8)
		CallMapMove(false)
		g_world:SetCameraZoom(1.0) -- 줌최대뒤로
	end

	return true
end

function Start_030702(world)
	ODS("Start_030702\n",false,987)
	OnRecoveryMiniQuest(false)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	-- 사운드
	PlayBgSound(0)
	--g_world:DetachSound("Mutisha_Opening")
	--g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 1.0, 0.0, 3.0, false )
	g_iStage = 5
	
	g_world:SetCameraMode(1, GetMyActor())
	g_world:SetCameraFixed(true) -- 카메라 고정
	--BoneDragonRemoveOpeningActor()
end
