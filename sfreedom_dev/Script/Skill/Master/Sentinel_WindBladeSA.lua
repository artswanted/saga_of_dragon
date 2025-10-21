------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간
function Skill_Sentinel_WindBladeSA_OnCastingCompleted(actor, action)
end

function Skill_Sentinel_WindBladeSA_OnEnter(actor, action)

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

	local   kEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
    local   kEffectTargetNode = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	if kEffectID ~= "" and kEffectTargetNode ~= "" then
		actor:AttachParticle(201, kEffectTargetNode, kEffectID);
	end
	
	actor:AttachParticle(202, "bone16", "ef_skill_wind_blade_02_bone16");
	actor:AttachParticle(203, "char_root", "ef_skill_wind_blade_03_char_root");
	
	return true
end

function Skill_Sentinel_WindBladeSA_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local iState = action:GetParamInt(10);
	
	if iState == 0 then
		if animDone == true then
			action:SetParamInt(10,1);
		end
	else
		return false;
	end

	return true;
end

function Skill_Sentinel_WindBladeSA_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(201);
	actor:DetachFrom(202);
	actor:DetachFrom(203);
	
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());
end

function Skill_Sentinel_WindBladeSA_OnLeave(actor, action)
	return true;
end

function Skill_Sentinel_WindBladeSA_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
end

function Skill_Sentinel_WindBladeSA_OnEvent(actor,textKey)
	
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

function Skill_Sentinel_WindBladeSA_OnTargetListModified(actor,action,bIsBefore)
end
