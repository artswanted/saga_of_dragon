-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [FIRE_SOUND_ID] : 스킬이 FIRE 될때 플레이 할 사운드 ID
-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

-- Melee
function Skill_LightningMargnetSA_OnCheckCanEnter(actor,action)

	CheckNil(nil==action);
	CheckNil(action:IsNil());

	return		true;
	
end
function Skill_LightningMargnetSA_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_LightningMargnetSA_Fire(actor,action);
end
function Skill_LightningMargnetSA_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:CreateActionTargetList(actor);
	if action:GetTargetCount()>0 then
	    action:BroadCastTargetListModify(actor:GetPilot());
	end
	action:ClearTargetList()
	local kRange = action:GetSkillRange(action:GetActionNo(), actor)
	if kRange==0 then kRange = 160 end
	actor:AttachParticleToPointS(181, actor:GetPos(), "ef_skill_LightningMagnet_03", kRange/160)	--160 기준임

end

function Skill_LightningMargnetSA_OnEnter(actor, action)
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

	local kTime = actor:GetAbil(AT_LIFETIME)
	if 0==kTime then
		kTime = 17000	--기본 15초
	else
		kTime = kTime + 5000	--있으면 5초 더 주자
	end

	action:SetParamFloat(10, action:GetParamFloat(1)+kTime*0.001)
	return true
end

function Skill_LightningMargnetSA_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()

	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local kAccumTime = action:GetParamFloat(1)	
	local kCoolTime = action:GetParamFloat(2)	

	if kCoolTime <= accumTime - kAccumTime then
		Skill_LightningMargnetSA_Fire(actor, action)
		action:SetParamFloat(1, accumTime)
	end
	
	if accumTime > action:GetParamFloat(10) then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())	--강제로 지움
		return false
	end
	
	return true	--return false해줄 필요 없음. 엔티티가 죽으면 자동으로 끝남
end

function Skill_LightningMargnetSA_OnCleanUp(actor)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	return true;
end

function Skill_LightningMargnetSA_OnLeave(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	return true
end

function Skill_LightningMargnetSA_OnEvent(actor,textKey)

	return	true;
end

function Skill_LightningMargnetSA_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
    
    if bIsBefore == false then
        --  때린다.
        action:GetTargetList():ApplyActionEffects();    
    end
end
