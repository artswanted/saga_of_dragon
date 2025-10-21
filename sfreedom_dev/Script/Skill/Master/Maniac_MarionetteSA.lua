------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간
-- 12[float] : 중간 시간
-- 19[int] : 이동력

function Skill_Maniac_MarionetteSA_OnCastingCompleted(actor,action)
end

function Skill_Maniac_MarionetteSA_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	action:SetParamInt(10,0);
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(11,g_world:GetAccumTime());
	
	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end		

	return true
end

function Skill_Maniac_MarionetteSA_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local iSlotNum = action:GetCurrentSlot();
	local iState = action:GetParamInt(10);
	
	if iState == 0 then
		if animDone == true then
			if iSlotNum == 0 then
				action:SetSlot(iSlotNum+1);
				actor:PlayCurrentSlot(true);
			elseif iSlotNum == 1 then
				action:SetSlot(iSlotNum+1);
				actor:PlayCurrentSlot(true);
			elseif iSlotNum == 2 then
				action:SetSlot(iSlotNum+1);
				actor:PlayCurrentSlot(true);
			elseif iSlotNum == 3 then
				action:SetSlot(iSlotNum+1);
				actor:PlayCurrentSlot(true);
			elseif iSlotNum == 4 then
				action:SetParamInt(10,1);
			end
		end
		
		if iSlotNum == 1 then
			local velocity = action:GetParamInt(19);
			local dir = actor:GetLookingDir();
			dir:Multiply(velocity);
			actor:SetMovingDelta(dir);
		end
		
		local guid = action:GetGUIDInContainer(0);
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local parent = g_world:FindActor(guid);
		if parent:IsNil() == false then
			local paction = parent:GetAction();
			if paction:IsNil() == false then
				local ActionID = paction:GetID()
				if ActionID ~= "a_Marionette" and ActionID ~= "a_Marionette_Finish" then
					action:SetParamInt(10,1);
				end
			end
		end
	else
		return false;
	end

	return true;
end

function Skill_Maniac_MarionetteSA_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());
end

function Skill_Maniac_MarionetteSA_OnLeave(actor, action)
	return true;
end

function Skill_Maniac_MarionetteSA_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
end

function Skill_Maniac_MarionetteSA_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if textKey == "alpha_end" then
		actor:SetTargetAlpha(actor:GetAlpha(),0, 0.25);
	end

	return	true;
end

function Skill_Maniac_MarionetteSA_OnTargetListModified(actor,action,bIsBefore)
end
