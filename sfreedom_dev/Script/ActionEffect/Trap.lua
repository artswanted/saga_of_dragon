
function Act_Trap_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local	actorID = actor:GetID()

	ODS("Act_Trap_OnEnter actor:"..actor:GetID().." action : "..action:GetID().."\n");
	if actor:IsRidingPet() then
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
		end
	end
	action:SetParamFloat(1,200);	-- Initial Velocity
	
	action:SetParamInt(2,0);	-- Step
	action:SetParamFloat(10,g_world:GetAccumTime());
	action:SetParamFloat(11,0);
	action:SetParamFloat(13,g_world:GetAccumTime());
	actor:FreeMove(true);	
	actor:StopJump();
	actor:StartGodTime(3);
	actor:SetMovingDelta(Point3(0,0,0));
	actor:SetNoWalkingTarget(false);
			
	action:SetParamInt(2,1);
	action:SetSlot(1);
	
	actor:SetCanHit(false);
	
	return true
end

function Act_Trap_OnUpdate(actor, accumTime, frameTime,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	currentSlot = action:GetCurrentSlot()
	local	param = action:GetParam(0)
	
	local	iStep = action:GetParamInt(2);

	local	fTotalElapsedTime = action:GetParamFloat(11);
	action:SetParamFloat(11,fTotalElapsedTime+frameTime);
	
	local	fTotalElapsedTime2 = accumTime - action:GetParamFloat(13);
	local	kEffectDef = GetEffectDef(action:GetActionNo());
	local	fTotalPlayTime = 3;
	if kEffectDef:IsNil() == false then
		--fTotalPlayTime = kEffectDef:GetDurationTime()/1000.0;
		fTotalPlayTime = 3;
	end
	
--	if fTotalElapsedTime2>fTotalPlayTime then
--		return	 false;
--	end
	
	local	fHorizSpeed = action:GetScriptParamAsInt("VELOCITY");
	local	fBlowUp_Gravity = -600;
	if iStep == 0 or iStep == 1 then
		
		local	kMovingDir = actor:GetLookingDir();
		local	fZ_Velocity_Init = action:GetParamFloat(1);
		local	fZ_Velocity = 2*fBlowUp_Gravity*fTotalElapsedTime+fZ_Velocity_Init;
		kMovingDir:SetZ(fZ_Velocity);
		kMovingDir:SetX(kMovingDir:GetX()*-1*fHorizSpeed);
		kMovingDir:SetY(kMovingDir:GetY()*-1*fHorizSpeed);
		actor:SetMovingDelta(kMovingDir);
		
	elseif iStep == 2 then
		
		local	kMovingDir = actor:GetLookingDir();
		local	fZ_Velocity_Init = action:GetParamFloat(1);
		local	fZ_Velocity = 2*fBlowUp_Gravity*fTotalElapsedTime+fZ_Velocity_Init;
		kMovingDir:SetZ(fZ_Velocity);
		kMovingDir:SetX(kMovingDir:GetX()*-1*fHorizSpeed*0.5);
		kMovingDir:SetY(kMovingDir:GetY()*-1*fHorizSpeed*0.5);
		actor:SetMovingDelta(kMovingDir);
	
	end
	if iStep == 5 and actor:IsAnimationDone() == true then
		action:SetNextActionName("a_idle")
		action:SetParam(4966,"end");
		actor:SetDownState(false);
		return	false;
	elseif iStep == 5 and actor:CheckStatusEffectExist("se_barricade") then
		action:SetNextActionName("a_idle")
		action:SetParam(4966,"end");
		actor:SetDownState(false);
		return	false;
	end 	
	if iStep == 4 then
		local	fElapsedDownTime = accumTime - action:GetParamFloat(3);
		if fElapsedDownTime>=fTotalPlayTime-(1.5+(action:GetParamFloat(3)-action:GetParamFloat(13))) then
			action:SetParamInt(2,5);
			actor:PlayNext();
			if actor:IsMyActor() then
				actor:SetSendBlowStatus(false);
			end
		end
	
	end
	if iStep == 3 and actor:IsAnimationDone() == true then
		action:SetParamInt(2,4);
		action:SetParamFloat(3,accumTime);	--	다운 시작 시간 기록
	elseif iStep == 3 and actor:CheckStatusEffectExist("se_barricade") then
		action:SetParamInt(2,4);
		action:SetParamFloat(3,accumTime);	--	다운 시작 시간 기록
	end
	
	if actor:IsMeetFloor() == true then
	
		if iStep == 1 then
			ODS("To Step 2\n");

			action:SetParamFloat(1,200);	-- Initial Velocity
			action:SetParamFloat(11,0);
			action:SetParamInt(2,2);
			action:SetParamInt(4,0);
			local pt = actor:GetTranslate()
			pt:SetZ(pt:GetZ() - 30)
			actor:AttachParticleToPoint(200, pt, "e_jump")
			actor:PlayNext();
			
		elseif iStep == 2 then
			if action:GetParamInt(4) == 0 then
				action:SetParamInt(4,1);
			else
		
				local pt = actor:GetTranslate()
				pt:SetZ(pt:GetZ() - 30)
				actor:AttachParticleToPoint(201, pt, "e_jump")
				action:SetParamInt(2,3);
				actor:ResetAnimation();
				action:SetSlot(2);
				actor:SetTargetAnimation(action:GetCurrentSlotAnimName());
					
				actor:SetDownState(true);
				actor:SetCanHit(true);			
				if actor:IsMyActor() then
					actor:SetSendBlowStatus(false, true);
				end
			end
		end
	end

	return true
end

function Act_Trap_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	actor:SetDownState(false)
	actor:SetCanHit(true)		
	
	actor:SetSendBlowStatus(false)

	actor:FreeMove(false)
	actor:SetMeetFloor(true)
	
	
	actor:FreeMove(false);
	actor:UseSkipUpdateWhenNotVisible(true);
end

function Act_Trap_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local kCurAction = actor:GetAction()
	if( CheckNil(kCurAction == nil) ) then return true end
	if( CheckNil(kCurAction:IsNil()) ) then return true end
	
	local newActionID = action:GetID()
	local iCurrnetSlot = kCurAction:GetCurrentSlot()

	if kCurAction:GetParam(4966) == "end" and
		(iCurrnetSlot == 3)then
		if newActionID == "a_run" or
			newActionID == "a_idle" then
			ODS("Act_Trap_OnLeave actor:FreeMove(false)\n");
			actor:FreeMove(false);
			return true
		end
	end
	return false

	--[[
	if action:GetID() ~= "a_idle" then
		return false
	end

	if action:GetID() ~= "a_die" then
		local	kEffectDef = GetEffectDef(actor:GetAction():GetActionNo())
		local	fGodTime = 1
		if kEffectDef:IsNil() == false then
			fGodTime = kEffectDef:GetAbil(AT_GOD_TIME)/1000.0
		end
		--actor:StartGodTime(fGodTime)
		--actor:StartGodTime(1)
	end

	actor:SetDownState(false)
	actor:SetCanHit(true)		
	
	--actor:SetSendBlowStatus(false)

	actor:FreeMove(false)
	actor:SetMeetFloor(true)

	--if 	action:GetID() == "a_run" then
	--	return false;
	--end
	return true;
	]]--
end
