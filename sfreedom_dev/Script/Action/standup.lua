--AT_DAMAGEACTION_TYPE		= 3039,
--AT_POSTURE_STATE			= 3072,	//누워있거나 공중에 떠 있거나
--AT_ELITEPATTEN_STATE		= 6330,	// 정예 패턴 상태


function IsElitePattenMonsterDamageAction(actor)
	return actor:IsUnitType(UT_MONSTER) and actor:GetAbil(6330) and actor:GetAbil(3039)
end

function Act_StandUp_OnEnter(actor, action)
	
	ODS("Act_StandUp_OnEnter\n");
	if IsElitePattenMonsterDamageAction(actor) then
		actor:SetAbil(3072,1)
	end
	return true
end

function Act_StandUp_OnUpdate(actor, accumTime, frameTime)

	local	bAnimDone = actor:IsAnimationDone()
	
	if bAnimDone then
	
		actor:GetAction():SetParam(0,"end");
		if( actor:IsUnitType( UT_SUB_PLAYER ) ) then	-- 보조캐릭터가 넘어졌다면 다시 원래 따라다니는 액션으로 바꿔줘야 한다
			TryMustChangeActorAction(actor, "a_twin_sub_trace_ground");
		end
		return	false;
	
	end

	return true
end

function Act_StandUp_OnCleanUp(actor, action)
	return true
end

function Act_StandUp_OnLeave(actor, action)
	if IsElitePattenMonsterDamageAction(actor) then
		actor:SetAbil(3072,0)
	end

	if actor:GetAction():GetParam(0) == "end" then
		return	true
	end
	
	return false;
end
