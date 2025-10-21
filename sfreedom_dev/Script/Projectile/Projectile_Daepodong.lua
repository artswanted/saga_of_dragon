function PROJECTILE_DAEPODONG_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_DAEPODONG_OnArrivedAtTarget(kProjectile)
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	if kParentPilot:IsNil() then
		return false;
	end
	
	local	kParentActor = kParentPilot:GetActor()

	local	iSkillLevel = tonumber(kProjectile:GetParamValue("SkillLevel"));
	
	--스킬 레벨에 따라 크기가 변한다. 최대치 1
	kParentActor:AttachParticleToPointS(12,kProjectile:GetWorldPos(),"ef_deapodong_blasting", 0.2 + ( iSkillLevel * 0.16));	
	
	kParentActor:AttachSound(7285,"Deapodong");
	
	local i = 0;
	local	kTargetList = kProjectile:GetActionTargetList();
	
	kTargetList:ApplyActionEffects();
	
	-- 화면 깨짐 옵션이 켜져 있다면
	local FlyToCameraOptionValue = Config_GetValue(HEADKEY_GAME, SUBKEY_FLY_TO_CAMERA)
	if 1 == FlyToCameraOptionValue then 
		if kParentActor:IsMyActor() then
			local fRandomX = math.random(30,60);
			local fMinRandomY = 45 + iSkillLevel * 5
			if(fMinRandomY > 80) then  fMinRandomY = 80 end
			local fRandomY = math.random(fMinRandomY,80);
			fRandomX = fRandomX / 100;
			fRandomY = fRandomY / 100;

			QuakeCamera(0.5, 0.6 * iSkillLevel, 1, 1, 1);
			AddNewBreak(fRandomX, fRandomY);

			if 2 <= iSkillLevel then
				fRandomX = math.random(20,40);
				fRandomY = math.random(20,40);
				fRandomX = fRandomX / 100;
				fRandomY = fRandomY / 100;
				AddNewBreak(fRandomX, fRandomY);
			elseif 3 <= iSkillLevel then
				fRandomX = math.random(60,80);
				fRandomY = math.random(20,40);
				fRandomX = fRandomX / 100;
				fRandomY = fRandomY / 100;
				AddNewBreak(fRandomX, fRandomY);
			elseif 4 <= iSkillLevel then
				fRandomX = math.random(20,40);
				fRandomY = math.random(60,80);
				fRandomX = fRandomX / 100;
				fRandomY = fRandomY / 100;
				AddNewBreak(fRandomX, fRandomY);
			elseif 5 <= iSkillLevel then
				fRandomX = math.random(60,80);
				fRandomY = math.random(60,80);
				fRandomX = fRandomX / 100;
				fRandomY = fRandomY / 100;
				AddNewBreak(fRandomX, fRandomY);
			end

		end
	end
	
	return	false;	-- returning false means Delete This projectile
end
