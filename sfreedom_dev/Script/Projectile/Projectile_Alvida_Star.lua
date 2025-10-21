function PROJECTILE_ALVIDA_STAR_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_ALVIDA_STAR_OnArrivedAtTarget(kProjectile)

	local	kActor = GetMyActor()
	if kActor:IsNil() == false then
	
		local	kTargetPos = kProjectile:GetWorldPos();
		kTargetPos:SetZ(kTargetPos:GetZ()+50)
		
		kActor:AttachParticleToPoint(71002,kTargetPos,"ef_Shark_king_abdula_skill_03_02_char_root");
	
	end
	return	false;	-- returning false means Delete This projectile
end
function PROJECTILE_ALVIDA_STAR_OnTargetListModified(kProjectile)


	return	PROJECTILE_ARROW_OnTargetListModified(kProjectile);
end

function PROJECTILE_DICTIONARY_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_DICTIONARY_OnArrivedAtTarget(kProjectile)

	local	kActor = GetMyActor()
	if kActor:IsNil() == false then
	
		local	kTargetPos = kProjectile:GetWorldPos();
		kTargetPos:SetZ(kTargetPos:GetZ()+50)
		
		kActor:AttachParticleToPoint(71002,kTargetPos,"ef_dictionary_skill_03_04_char_root");
		kActor:AttachSound(2784+math.random(0,100),"Book_Skill04",60)	--사운드 볼륨 조절
	
	end
	return	false;	-- returning false means Delete This projectile
end
function PROJECTILE_DICTIONARY_OnTargetListModified(kProjectile)


	return	PROJECTILE_ARROW_OnTargetListModified(kProjectile);
end
