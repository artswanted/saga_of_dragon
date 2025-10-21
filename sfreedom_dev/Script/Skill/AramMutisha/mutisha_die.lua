
function Act_Mutisha_Die_CreateEngingActor()
	local guid = GUID("123")
	guid:Generate()
	
	local pilot = g_pilotMan:NewPilot(guid, 6000932, 0,"NPC")
	
	if pilot == nil or pilot:IsNil() == true then
		return true
	end
	
	local kActor = pilot:GetActor()
	if kActor == nil or kActor:IsNil() == true then
		return true
	end
	
	if IsSingleMode() == false then
		g_pilotMan:InsertPilot(guid,pilot);
	end

	local	kSpawnPos = Point3(0, 0, 1000)
	
	kActor:FreeMove(true)
	kActor:SetUseLOD(false)
	kActor:SetUseSmoothShow(false)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:AddActor(guid, pilot:GetActor(), kSpawnPos, 6)
	
	-- local kTime = g_world:GetAccumTime()
	-- kActor:SetAnimationStartTime(kTime)
	
	--pilot:SetName(kName);
	pilot:GetActor():ClearReservedAction()
	local kAction = pilot:GetActor():ReserveTransitAction("a_ending_movie")
	kAction:SetSlot(0)
	kActor:ResetAnimation();
	--kActor:PlayCurrentSlot();

	pilot:GetActor():ClearActionState()
	pilot:SetAbil(AT_HP, 1000000);
	pilot:SetAbil(AT_UNIT_SIZE,2);
	pilot:SetAbil(AT_DAMAGEACTION_TYPE,0);
	pilot:SetAbil(AT_NOT_SEE_PLAYER, 0)
	pilot:SetAbil(AT_DAMAGEACTION_TYPE, 0)	
	
	g_world:SetCameraModeMovie2(kActor, 2, 1000021, "nop()")
end

function Act_Mutisha_Die_OnEnter(actor, action)
	--AramCreateOpeningActor()
	action:SetParamInt(20,1)
	return true
end

function Act_Mutisha_Die_Ready()
	LockPlayerInputMove(8) -- for aram lock
	LockPlayerInput(8)
	CloseAllUI()
	
	CallUI("FRM_MOVIE_IN")
	StopBgSound()	--배경음 끄기
end

function Act_Mutisha_Die_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end

	action = actor:GetAction()
	curSlot = action:GetCurrentSlot()
	
	if curSlot == 0 and actor:IsAnimationDone() then	-- 처음 에니이고 끝났다면
		local kPlay = action:GetParamInt(20)
		if kPlay == 1 then
			action:SetParamInt(20,0)
			Act_Mutisha_Die_Ready()
			Act_Mutisha_Die_CreateEngingActor()
			actor:SetHide(true);
			actor:SetHideShadow(true);
			--AramCreateOpeningActor()
			ODS("Act_Mutisha_Die_OnUpdate\n",false,987)
		end
		
		return false
	end

	return true
end



function Act_Mutisha_Die_OnCleanUp(actor, action)
	ODS("Act_Mutisha_Die_OnCleanUp\n",false,987)
end

function Act_Mutisha_Die_OnEvent(actor,textKey)
	ODS("Act_Mutisha_Die_OnEvent\n",false,987)
	return true
end

function Act_Mutisha_Die_OnLeave(actor, action)
   ODS("Act_Mutisha_Die_OnLeave\n",false,987)
	return true
end
