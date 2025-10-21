------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간
-- 12[float] : 타이머 시작 시간
-- 13[int] : 시작 여부

function Skill_MagmaPiston_attack_OnCheckCanEnter(actor,action)
	return true;
end

function Skill_MagmaPiston_attack_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--	시작 시간 기록
	action:SetParamInt(10,0);
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(11,g_world:GetAccumTime());
	local iTime = actor:GetAbil(AT_TIME) / AT_CALCUATEABIL_MAX;
	
	action:SetParamFloat(12,iTime + g_world:GetAccumTime());
	action:SetParamInt(13,0);
	action:SetDoNotBroadCast(true)

	return true
end

function Skill_MagmaPiston_attack_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local iState = action:GetParamInt(10);
	if iState == 0 then
		local startTime = action:GetParamFloat(12);
		local startOK = action:GetParamInt(13);
		if startOK == 0 and accumTime > startTime then
			action:StartTimer(4.0,1.0,0);
			action:SetParamInt(13,1);
		end
		
		local	fElapsedTime = accumTime - action:GetParamFloat(12);
		if fElapsedTime > 5 then
			action:SetParamInt(10,1);
		end
	else
		return false;
	end
	
	return true;
end

function Skill_MagmaPiston_attack_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(201);
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());
end

function Skill_MagmaPiston_attack_OnLeave(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local curAction = actor:GetAction();
	
	if action:GetEnable() == false then
		return	false;
	end
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	if curAction:GetParamInt(10) == 1 then
		return true;
	end
	
	return false;
end

function Skill_MagmaPiston_attack_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	return	true;
end

function Skill_MagmaPiston_attack_OnTargetListModified(actor,action,bIsBefore)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if bIsBefore == false then
		Act_Melee_Default_HitOneTime(actor,action);
    end
end

function Skill_MagmaPiston_attack_OnTimer(actor,accumtime,action,iTimerID)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	if actor:IsUnderMyControl() then
		action:CreateActionTargetList(actor);
		action:BroadCastTargetListModify(actor:GetPilot());
		action:ClearTargetList();
	end
	
	local effectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
	local effectNode = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
	if effectID ~= "" and effectNode ~= "" then
		actor:DetachFrom(201, true);
		actor:AttachParticleS(201, effectNode, effectID, 0.12)
	end
	
	if IsSingleMode() then
    	Skill_WarLord_SoulDrain_OnTargetListModified(actor,action,false);
    	return true;
	end	
	
	return true;
end

function MagmaPistonPlayInit(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	-- actor:SetHide(true);
	-- actor:SetHideShadow(true);	
end

function Skill_MagmaPiston_attack_OnOverridePacket(actor,action,packet)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
end

function Skill_MagmaPiston_attack_OnCastingCompleted(actor,action)
end
