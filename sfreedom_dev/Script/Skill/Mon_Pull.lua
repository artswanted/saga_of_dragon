function Skill_Mon_Pull_OnCheckCanEnter(actor,action)

	CheckNil(nil==action);
	CheckNil(action:IsNil());

	return		true;
	
end

function Skill_Mon_Pull_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Mon_Pull_Fire(actor,action);
end

function Skill_Mon_Pull_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:CreateActionTargetList(actor);
	if action:GetTargetCount()>0 then
	    action:BroadCastTargetListModify(actor:GetPilot());
	end
	action:ClearTargetList()

	local kFireEffectID = "ef_skill_LightningMagnet_03"
	local kFireAttachNode = "p_ef_heart"
	local kScale = 1

	actor:AttachParticleS(182, kFireAttachNode, kFireEffectID, kScale)

end

function Skill_Mon_Pull_OnEnter(actor, action)
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(1, g_world:GetAccumTime())
	local kCoolTime = action:GetAbil(ATS_COOL_TIME)
	if nil==kCoolTime or 0==kCoolTime then
		kCoolTime = 500
	end

	action:SetParamFloat(2, kCoolTime*0.001)

	local kTime = action:GetAbil(AT_MAINTENANCE_TIME)/1000
	ODS("Skill_Mon_Pull_OnEnter kTime: "..kTime.."\n",false,6482)
	if 0==kTime then
		kTime = 5	--기본 5초
	else
		kTime = kTime + 5	--있으면 5초 더 주자
	end

	action:SetParamFloat(10, action:GetParamFloat(1)+kTime)


	local kLoopEffectID = ""
	local kLoopAttachNode = "p_ef_heart"
	local kScale = 1
	
	if nil~=kLoopEffectID and ""~=kLoopEffectID then
		actor:AttachParticleS(181, kLoopAttachNode, kLoopEffectID, kScale)
	end
	return true
end

function Skill_Mon_Pull_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()

	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local kAccumTime = action:GetParamFloat(1)
	local kCoolTime = action:GetParamFloat(2)

	if kCoolTime <= accumTime - kAccumTime then
		Skill_Mon_Pull_Fire(actor, action)
		action:SetParamFloat(1, accumTime)
	end
	
	if accumTime > action:GetParamFloat(10) then
		return false
	end
	
	return true
end

function Skill_Mon_Pull_OnCleanUp(actor)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	return true;
end

function Skill_Mon_Pull_OnLeave(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:DetachFrom(181)
	actor:DetachFrom(182)
	return true
end

function Skill_Mon_Pull_OnEvent(actor,textKey)

	return	true;
end

function Skill_Mon_Pull_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
    
    if bIsBefore == false then
        --  때린다.
        action:GetTargetList():ApplyActionEffects();
    end
end
