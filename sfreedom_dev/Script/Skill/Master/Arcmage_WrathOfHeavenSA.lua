------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 11[float] : 시간
-- 12[int] : 손이펙트 떼어냈는지 여부
function Skill_Arcmage_WrathOfHeavenSA_OnCastingCompleted(actor,action)
end

function Skill_Arcmage_WrathOfHeavenSA_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(11,g_world:GetAccumTime());
	action:SetParamInt(12, 0);
	
	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end		
	
	local effectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
	local effectNode = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	if effectID ~= "" and effectNode ~= "" then
		actor:AttachParticle(201, effectNode, effectID);
	end
	
	local effectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
	local effectNode = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
	if effectID ~= "" and effectNode ~= "" then
		actor:AttachParticle(202, effectNode, effectID);
	end
	actor:FreeMove(true)
	return true
end

function Skill_Arcmage_WrathOfHeavenSA_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	
	local deltaTime = accumTime - action:GetParamFloat(11);
	local IsDetach = action:GetParamInt(12);
	if IsDetach == 0 then
		if deltaTime > 1 then
			action:SetParamInt(12,1);
			actor:DetachFrom(202);
		end
	end
	
	if animDone == true then
		return false;
	end
	
	return true;
end

function Skill_Arcmage_WrathOfHeavenSA_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(201, true);
	actor:DetachFrom(202);
	
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());
end

function Skill_Arcmage_WrathOfHeavenSA_OnLeave(actor, action)
	return true;
end

function Skill_Arcmage_WrathOfHeavenSA_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
end

function Skill_Arcmage_WrathOfHeavenSA_OnEvent(actor,textKey)
	
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

function Skill_Arcmage_WrathOfHeavenSA_OnTargetListModified(actor,action,bIsBefore)
end
