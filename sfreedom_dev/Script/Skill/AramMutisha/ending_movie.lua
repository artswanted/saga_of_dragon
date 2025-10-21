function Act_Ending_Movie_OnEnter(actor, action)
   	--g_world:SetCameraModeMovie2(actor, 2, 1000021, "nop()")
	ODS("Act_Ending_Movie_OnEnter\n",false,987)

	-- local kTime = g_world:GetAccumTime()
	-- actor:SetAnimationStartTime(kTime)
	
	action:SetParamInt(20,1)
	
	action:SetSlot(0)
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(1,0)
	

	
	ODS("Action Name:"..action:GetID().."\n",false,987)
	return true
end

function Act_Ending_Movie_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end

	action = actor:GetAction()		-- 지금 엑션 객체를 반환
	curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번

	if curSlot == 0 and actor:IsAnimationDone() then	-- 처음 에니이고 끝났다면
		local kPlay = action:GetParamInt(20)
		if kPlay == 1 then
			action:SetParamInt(20,0)
			Act_Ending_Movie_RemoveActor()
			ODS("Act_Ending_Movie_OnUpdate2\n",false,987)
		end
		return false
	end

	return true
end

function Act_Ending_Movie_RemoveEndingActor()
	ODS("Act_Ending_Movie_RemoveEndingActor\n",false,987)
	local kActor = g_pilotMan:FindActorByClassNo(6000932);
	
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:RemoveActorOnNextUpdate(kActor:GetPilotGuid())
	g_world:RemoveCameraAM()
	
	return true
end

function Act_Ending_Movie_RemoveActor()
	ODS("Act_Ending_Movie_RemoveActor\n",false,987)
	OnRecoveryMiniQuest(false)
	-- 사운드
	PlayBgSound(0)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 1.0, 0.0, 3.0, false )

	Act_Ending_Movie_RemoveEndingActor()

	g_world:SetCameraMode(1, GetMyActor())
	g_world:SetCameraFixed(true) -- 카메라 고정

	CloseUI("FRM_MOVIE_IN")
	UI_DefaultActivate( true )
	UnLockPlayerInputMove(8) -- for aram lock
	UnLockPlayerInput(8)
	CallMapMove(false)
	g_world:SetCameraZoom(1.0) -- 줌최대뒤로
end

function Act_Ending_Movie_OnCleanUp(actor, action)
	ODS("Act_Ending_Movie_OnCleanUp\n",false,987)
end

function Act_Ending_Movie_OnLeave(actor, action)
   ODS("Act_Ending_Movie_OnLeave\n",false,987)
	return true
end
