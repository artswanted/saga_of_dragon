-- skill

function Act_Mon_Melee_Delay_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C)
end

function Act_Mon_Melee_Delay_SetState(actor,action,kState)
	return Act_Mon_Melee_SetState(actor,action,kState)
end

function Act_Mon_Melee_Delay_OnEnter(actor, action)
	return Act_Mon_Melee_OnEnter(actor, action)
end
function Act_Mon_Melee_Delay_OnCastingCompleted(actor,action)	
	Act_Mon_Melee_OnCastingCompleted(actor,action)
end

function Act_Mon_Melee_Delay_Fire(actor,action)
	Act_Mon_Melee_Fire(actor,action)
end

function Act_Mon_Melee_Delay_OnUpdate(actor, accumTime, frameTime)
	return Act_Mon_Melee_OnUpdate(actor, accumTime, frameTime)
end
function Act_Mon_Melee_Delay_OnCleanUp(actor, action)
end

function Act_Mon_Melee_Delay_OnLeave(actor, action)
	return Act_Mon_Melee_OnLeave(actor, action)
end

function Act_Mon_Melee_Delay_OnEvent(actor,textKey)
	return Act_Mon_Melee_OnEvent(actor,textKey)
end

function Act_Mon_Melee_Delay_OnTargetListModified(actor,action,bIsBefore)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	if bIsBefore == false then
		Act_Mon_Melee_DoDamage(actor,action)
	end
end
