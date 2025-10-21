-- Fireball

function Skill_Icebress_OnEnter(actor)
	return true
end

function Skill_Icebress_OnUpdate(actor, accumTime, frameTime)
	if actor:IsAnimationDone() == true then
		return false
	end

	return true
end

function Skill_Icebress_OnLeave(actor, action)
	return true
end

function Skill_Icebress_OnEvent(actor, textKey)
	if textKey == "hit" then
		particle = g_particleMan:NewParticle("e_icebress")

		loc = actor:GetNodeTranslate("Bip01 R Hand")
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddParticle(particle, loc, 9)
		particle:SetTrigger("Skill_Icebress_OnTrigger", 4, 1)
	
		-- throw
		vel = actor:GetLookingDir()
		vel:SetZ(0)
		vel:Unitize()
		vel:Multiply(300)
		particle:SetVelocity(vel)
	end

	return true
end

function Skill_Icebress_OnTrigger(particle, actor)
	actor:AttachParticle(20000, "p_ef_heart", "e_dmg_fire")
	actor:ReserveTransitAction("a_dmg")
end
