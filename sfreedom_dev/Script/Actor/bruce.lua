function Skill_Bruce(actor, keyName)
	action = actor:GetAction()
	actionID = action:GetID()
	
	if actionID == "a_transform_7kick" and keyName == "hit" then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
	
		OriginalActor = actor:GetOriginalActor();
		
		actorTarget = g_world:FindTarget(actor, 2, 60.0)
		if actorTarget:IsNil() == false then
			--actor:AttachParticle(12, "Dummy01", "e_dmg")
			actorTarget:AttachParticle(12, "p_ef_heart", "e_dmg")
			actorTarget:ReserveTransitAction("a_dmg")
			
			if OriginalActor:IsNil() == false and OriginalActor:IsMyActor() == true then
				-- Net_ReqDamage(actor, actorTarget, 1023)
			end
		end		
	
	end
	
	return true
end

