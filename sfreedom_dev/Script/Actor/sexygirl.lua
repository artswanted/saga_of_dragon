function Skill_SexyGirl(actor, keyName)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action = actor:GetAction()
	actionID = action:GetID()

	if actionID == "a_transform" then
		if keyName == "hit" then
			OriginalActor = actor:GetOriginalActor();

			actor:AttachParticle(100, "p_ef_heart", "e_heart")
			actorTarget = g_world:FindTarget(actor, 2, 60.0)
			if actorTarget:IsNil() == false then
				actor:AttachParticle(12, "Dummy01", "e_dmg")
				actorTarget:AttachParticle(12, "p_ef_heart", "e_dmg")
				actorTarget:ReserveTransitAction("a_dmg")
				
				if OriginalActor:IsNil() == false and OriginalActor:IsMyActor() == true then
					Net_ReqDamage(actor, actorTarget, 1)
				end				
				
			end
		end
	end

	return true
end

