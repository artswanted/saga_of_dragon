function Effect_HideUnit_Begin(actor, iEffectID, iEffectKey)
end

function Effect_HideUnit_End(actor, iEffectID, iEffectKey)
	actor:HideNode("Scene Root", false)
end

function Effect_HideUnit_OnTick(actor, iEffectID, fAccumTime)
	if actor:IsAlphaTransitDone() then
		actor:HideNode("Scene Root", true)
	end
	return
end
