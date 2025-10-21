-- Spell

function Act_Spell_OnEnter(actor, action)
	actor:Stop()
	return true
end

function Act_Spell_OnUpdate(actor, accumTime, frameTime)
	return true
end
function Act_Spell_OnCleanUp(actor, action)
end

function Act_Spell_OnLeave(actor, action)
	return true
end

