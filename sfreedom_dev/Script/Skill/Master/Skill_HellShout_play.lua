-- [FIRE_SOUND_ID] : 시전시에 플레이 할 사운드 ID

function Skill_HellShout_play_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end		
	
	local effectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
	local effectNode = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	if effectID ~= "" and effectNode ~= "" then
		actor:AttachParticle(201, effectNode, effectID);
	end
	actor:FreeMove(true)
	return true
end

function Skill_HellShout_play_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	
	if animDone == true then
		return false;
	end
	
	return true;
end

function Skill_HellShout_play_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(201);
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());
end

function Skill_HellShout_play_OnLeave(actor, action)
	return true;
end

function Skill_HellShout_play_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
end

function Skill_HellShout_play_OnEvent(actor,textKey)
	
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

function Skill_HellShout_play_OnTargetListModified(actor,action,bIsBefore)
end

function Skill_HellShout_play_OnCastingCompleted(actor,action,bIsBefore)
end