function PROJECTILE_BOOMERANG_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_BOOMERANG_OnArrivedAtTarget(kProjectile)

	if  kProjectile:GetParamValue("STATE")=="RETURN" then	--	돌아가고 있는 것이다.

		local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
		if kParentPilot:IsNil() then
			return false;
		end
			
		Act_Mon_RangeAttack_SwordReturn(kParentPilot:GetActor(),kProjectile:GetParentActionInstanceID());
	
		return	false;	--	다 돌아온 것이다.
	else
	
		--	던진 놈한테로 돌아간다.
		kProjectile:SetTargetLoc(kProjectile:GetFireStartPos());
		kProjectile:Fire();

		kProjectile:SetParam(kProjectile:GetOriginalSpeed()*-1,1000,0);
		kProjectile:SetParamValue("STATE","RETURN");		
		
	end

	return	true;	-- returning false means Delete This projectile
end

function PROJECTILE_BOOMERANG_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end
