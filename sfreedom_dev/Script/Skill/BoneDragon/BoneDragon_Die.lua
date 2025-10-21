
function Act_BoneDragon_Die_CreateEndingActor(iNo, kPos)
	local guid = GUID("123")
	guid:Generate()
	
	local pilot = g_pilotMan:NewPilot(guid, iNo, 0,"NPC")
	
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
	
	kActor:FreeMove(true)
	kActor:SetUseLOD(false)
	kActor:SetUseSmoothShow(false)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:AddActor(guid, pilot:GetActor(), kPos, 6)
	
	-- local kTime = g_world:GetAccumTime()
	-- kActor:SetAnimationStartTime(kTime)
	
	--pilot:SetName(kName);
	pilot:GetActor():ClearReservedAction()
	local kAction = pilot:GetActor():ReserveTransitAction("a_bonedragon_ending_movie")
	kAction:SetSlot(0)
	kActor:ResetAnimation();
	--kActor:PlayCurrentSlot();

	pilot:GetActor():ClearActionState()
	pilot:SetAbil(AT_HP, 1000000);
	pilot:SetAbil(AT_UNIT_SIZE,2);
	pilot:SetAbil(AT_DAMAGEACTION_TYPE,0);
	pilot:SetAbil(AT_NOT_SEE_PLAYER, 0)
	pilot:SetAbil(AT_DAMAGEACTION_TYPE, 0)	
	
	g_world:SetCameraModeMovie2(kActor, 2, 1000000, "nop()")
end

function Act_BoneDragon_Die_OnEnter(actor, action)
	local ifirst = action:GetParamInt(20)
	if ifirst == 1 then
		return true
	end
	
	local kEnding = actor:GetParam("ending")
	if kEnding == "start" then
		return true
	end
	
	action:SetParamInt(20,1)
	
	return true
end

function Act_BoneDragon_Die_Start(actor, action)
	
	actor:SetParam("ending", "start")

	Act_BoneDragon_Die_Ready()
	
	local iActor = actor:GetAnimationInfoFromAniName("DIE_ACTOR", "die_01")
	if nil==iActor or true==iActor:IsNil() then
		iActor = 6000952
	else
		iActor = tonumber(iActor:GetStr())
	end	

	local kPos = Point3(0, 0, -3500)
	local kX = actor:GetAnimationInfoFromAniName("ENDING_ACTOR_X", "die_01")
	local kY = actor:GetAnimationInfoFromAniName("ENDING_ACTOR_Y", "die_01")
	local kZ = actor:GetAnimationInfoFromAniName("ENDING_ACTOR_Z", "die_01")
	if nil~=kX and false==kX:IsNil() then
		kPos:SetX(tonumber(kX:GetStr()))
	end
	if nil~=kY and false==kY:IsNil() then
		kPos:SetY(tonumber(kY:GetStr()))
	end
	if nil~=kZ and false==kZ:IsNil() then
		kPos:SetZ(tonumber(kZ:GetStr()))
	end

	Act_BoneDragon_Die_CreateEndingActor(iActor, kPos)
	actor:SetHide(true);
	actor:SetHideShadow(true);

	return true
end

function Act_BoneDragon_Die_Ready()
	LockPlayerInputMove(8) -- for aram lock
	LockPlayerInput(8)
	CloseAllUI()
	
	CallUI("FRM_MOVIE_IN")
	StopBgSound()	--배경음 끄기
end

function Act_BoneDragon_Die_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end

	action = actor:GetAction()
	curSlot = action:GetCurrentSlot()
	
	if actor:IsAnimationDone() == true then
		local kEnding = actor:GetParam("ending")
		if kEnding == "start" then
			return true
		end
		Act_BoneDragon_Die_Start(actor, action)
		action:SetSlot(1)
		actor:PlayCurrentSlot()
	end
	
	return true
end



function Act_BoneDragon_Die_OnCleanUp(actor, action)
end

function Act_BoneDragon_Die_OnEvent(actor,textKey)
	return true
end

function Act_BoneDragon_Die_OnLeave(actor, action)
	if actor == nil then
		return true
	end
	if actor:IsAnimationDone() ~= true then
		return false
	end

	return true
end
