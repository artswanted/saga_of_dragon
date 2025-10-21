------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간
-- 12[float] : 중간 시간
function Skill_Shadow_ShadowBladeSAFinish_OnCastingCompleted(kActor,kAction)
end

function Skill_Shadow_ShadowBladeSAFinish_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	action:SetParamInt(10,0);
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(11,g_world:GetAccumTime());
	action:SetDoNotBroadCast(true)
	
	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end		
	
	local   kEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
    local   kEffectTargetNode = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	if kEffectID ~= "" and kEffectTargetNode ~= "" then
		actor:AttachParticle(201, kEffectTargetNode, kEffectID);
	end
	
	return true
end

function Skill_Shadow_ShadowBladeSAFinish_OnUpdate(actor, accumTime, frameTime)

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

function Skill_Shadow_ShadowBladeSAFinish_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(201);
	actor:DetachFrom(202);
	
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	local kCallerGuid = actor:GetCallerGuid();
	if kCallerGuid:IsNil() == false then
		local callerActor = g_world:FindActor(kCallerGuid)
		if callerActor:IsNil() == false then
			local callerAction = callerActor:GetAction();
			if callerAction ~= nil and callerAction:IsNil() == false then
				callerAction:SetParamInt(10,1);
			end
			local effectpos = actor:GetPos()
			callerActor:AttachParticleToPoint(305, effectpos, "ef_smog_03_char_root");
		end
	end
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());
end

function Skill_Shadow_ShadowBladeSAFinish_OnLeave(actor, action)
	return true;
end

function Skill_Shadow_ShadowBladeSAFinish_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
end

function Skill_Shadow_ShadowBladeSAFinish_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if textKey == "hit" then
		if actor:IsUnderMyControl() then
			action:CreateActionTargetList(actor);
			action:BroadCastTargetListModify(actor:GetPilot());
			action:ClearTargetList();
		end
	elseif textKey == "alpha_end" then
		actor:SetTargetAlpha(actor:GetAlpha(),0, 0.25);
	end

	return	true;
end

function Skill_Shadow_ShadowBladeSAFinish_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if bIsBefore == false then
		Act_Melee_Default_HitOneTime(actor,action);
    end
end
