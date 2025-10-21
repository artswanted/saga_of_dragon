
-- 대각선아래방향으로 세발의 화살을 동시에 발사!
-- [PROJECTILE_ID] : 발사할 프로젝틸 ID. 빈문자열일 경우 기본 프로젝틸이 발사된다.
-- [ATTACH_NODE] : 프로젝틸을 붙일 노드. 빈문자열일 경우 무기에 붙이게 된다.
function Skill_Ice_ShowerSA_OnCheckCanEnter(actor,action)
	return true
end
function Skill_Ice_ShowerSA_OnEnter(actor, action)
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	action:SetDoNotBroadCast(true)
	return true
end

function Skill_Ice_ShowerSA_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:PlayCurrentSlot();

end

function Skill_Ice_ShowerSA_Shot_Fire(actor,action)

end


function Skill_Ice_ShowerSA_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil())

	return not actor:IsAnimationDone()
end
function Skill_Ice_ShowerSA_OnCleanUp(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())

	CheckNil(nil==action);
	CheckNil(action:IsNil());
	action:SetDoNotBroadCast(true)

	return true;
end
function Skill_Ice_ShowerSA_OnLeave(actor, action)
	return true
end

function Skill_Ice_ShowerSA_OnEvent(actor, textKey)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
		
	if textKey == "fire" or textKey=="hit" then
		local kCallerGuid = actor:GetCallerGuid()
		if true == kCallerGuid:IsNil() then return end

		local kCallerPilot = g_pilotMan:FindPilot(kCallerGuid)
		if true == kCallerPilot:IsNil() then return end

		local kCallerActor = kCallerPilot:GetActor()
		if true == kCallerActor:IsNil() then return end

		local kCallerAction = kCallerActor:GetAction()
		if true == kCallerAction:IsNil() then return end

		kCallerAction:SetParamInt(1, 1)
	elseif textKey == "alpha_end" then
		actor:SetTargetAlpha(actor:GetAlpha(),0.0, 0.25); -- 일단 처음엔 투명하게
	end
end
