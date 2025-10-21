------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간

function Skill_WarLord_SoulDrain_Finish_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
			return	false;
		end
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamInt(10,0);
	action:SetParamFloat(11,g_world:GetAccumTime());

	local kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	

	Skill_WarLord_SoulDrain_Finish_Fire(actor,action);	
	return true
end

function Skill_WarLord_SoulDrain_Finish_OnCastingCompleted(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:CutSkillCasting();
end

function Skill_WarLord_SoulDrain_Finish_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:LockBidirection(false);
	actor:SeeFront(true);
	
	local	iState = action:GetParamInt(10);
	
	if iState == 0 then
		if animDone == true then
			action:SetParamInt(10,1);
		end
	else
		return false
	end

	return true
end

function Skill_WarLord_SoulDrain_Finish_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	actor:DetachFrom(201);
	actor:DetachFrom(202);

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end

	actor:RestoreLockBidirection();
	
	return true;
end

function Skill_WarLord_SoulDrain_Finish_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetEnable() == false then
		return	false;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	
	local	curAction = actor:GetAction();
	if curAction:GetParamInt(10) == 1 then
		return true
	end

	return false
end

function Skill_WarLord_SoulDrain_Finish_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
		Act_Melee_Default_HitOneTime(actor,action);
    end
end

function Skill_WarLord_SoulDrain_Finish_OnEvent(actor,textKey,seqID)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()
			
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	return	true;
end

function Skill_WarLord_SoulDrain_Finish_Fire(actor,action)
		
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:PlayNext();
	
	if actor:IsMyActor() == true then
		action:CreateActionTargetList(actor);
		action:BroadCastTargetListModify(actor:GetPilot());
		action:ClearTargetList();
	end
end

