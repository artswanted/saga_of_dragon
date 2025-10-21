function PROJECTILE_SafetyZoneShot_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_SafetyZoneShot_OnArrivedAtTarget(kProjectile)

	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
	if nil == kParentPilot or kParentPilot:IsNil() then
		return true
	end
	local	kActor = kParentPilot:GetActor()
	if kActor:IsNil() == false then
		local	kTargetPos = kProjectile:GetWorldPos();
		--kTargetPos:SetZ(kTargetPos:GetZ()+50)
		kActor:AttachParticleToPointS(12,kTargetPos,"ef_bang_02_p_ef_heart",4.0)
		--kActor:AttachSound(2784,"Mutisha_Skill02");
	end
	return	true;	-- returning false means Delete This projectile
end
function PROJECTILE_SafetyZoneShot_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end
