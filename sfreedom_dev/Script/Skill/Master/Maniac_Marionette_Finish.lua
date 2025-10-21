------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간

function Skill_Maniac_Marionette_Finish_OnEnter(actor, action)
	
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
	if actor:IsMyActor() == true then
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0.7, 0,true,true);
	end
	
	action:SetParamInt(10,0);
	action:SetParamFloat(11,g_world:GetAccumTime());
	
	local kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
	
	local   kEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
    local   kEffectTargetNode = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	if kEffectID ~= "" and kEffectTargetNode ~= "" then
		actor:AttachParticle(201, kEffectTargetNode, kEffectID);
	end
	
	local iSummonCount = actor:GetSummonUnitCount();
	local guid = actor:GetSummonUnit(0);
	local elemental = g_world:FindActor(guid);
	if elemental:IsNil() == false then
		if actor:IsMyActor() == false then
			local kLookAt = elemental:GetLookingDir()
			actor:LookAt(actor:GetTranslate():_Add(kLookAt), true);
		end
	end
				
	return true
end

function Skill_Maniac_Marionette_Finish_OnCastingCompleted(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:CutSkillCasting();
end

function Skill_Maniac_Marionette_Finish_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local iState = action:GetParamInt(10);

	if iState == 0 then
		if animDone == true then
			action:SetParamInt(10,1);
		end
	else
		return false
	end

	return true
end

function Skill_Maniac_Marionette_Finish_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(201);
	actor:DetachFrom(202);
	
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	if actor:IsMyActor() then
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end	
	
	local guid = actor:GetSummonUnit(0);
	local elemental = g_world:FindActor(guid);
	if elemental:IsNil() == false then
		actor:DeleteSummonUnit(guid);
	end
	
	return true;
end

function Skill_Maniac_Marionette_Finish_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction()	
	local actionID = action:GetID()
	
	if(nil == string.find(actionID, "a_run")) then
		if curAction:GetNextActionName() ~= actionID then
			ODS("다른게 들어옴:"..actionID.."\n",false, 912)
			return false;
		end
	end
	
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

function Skill_Maniac_Marionette_Finish_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
		Act_Melee_Default_HitOneTime(actor,action);
    end
end

function Skill_Maniac_Marionette_Finish_OnEvent(actor,textKey,seqID)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()
			
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if textKey == "hit" then
		Skill_Maniac_Marionette_Finish_Fire(actor,action);
	end
	return true;
end

function Skill_Maniac_Marionette_Finish_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsMyActor() == true then
		action:CreateActionTargetList(actor);
		action:BroadCastTargetListModify(actor:GetPilot());
		action:ClearTargetList();
	end
	
	if IsSingleMode() then
    	Skill_Maniac_Marionette_Finish_OnTargetListModified(actor,action,false);
	end	
	
	local   kEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
	local   kEffectTargetNode = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
	if kEffectID ~= "" and kEffectTargetNode ~= "" then
		actor:AttachParticle(202, kEffectTargetNode, kEffectID);
	end
				
end

