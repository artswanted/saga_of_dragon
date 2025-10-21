function PROJECTILE_SHIELD_BOOMERANG_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_SHIELD_BOOMERANG_OnArrivedAtTarget(kProjectile)

	if kProjectile:GetParamValue("RETURN")=="TRUE" then	--	돌아가고 있는 것이다.

		local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
		if kParentPilot:IsNil() then
			return false;
		end
			
		SwordDance_SwordReturn(kParentPilot:GetActor(),kProjectile:GetParentActionInstanceID());

		return	false;	--	다 돌아온 것이다.
	
	else
	
		kProjectile:SetParamValue("RETURN","TRUE");
	
		local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

		if kParentPilot:IsNil() then
			return false;
		end

		local i = 0;
		local	kTargetList = kProjectile:GetActionTargetList();
		while i<kTargetList:size() do
			
			local	kActionTargetInfo = kProjectile:GetActionTargetInfo(i);
			local	kActionResult = kActionTargetInfo:GetActionResult();
			
			if kActionResult:IsMissed() == false then
				
				local	kTargetPilot =  g_pilotMan:FindPilot(kActionTargetInfo:GetTargetGUID());
				if kTargetPilot:IsNil() == false then

  					local	kTargetActor = kTargetPilot:GetActor()
  					local	kParentActor = kParentPilot:GetActor()

					local	kTargetGUID = kActionTargetInfo:GetTargetGUID();
					
					local	kSkillDef	= GetSkillDef(kProjectile:GetParentActionNo());
					local actionName = kSkillDef:GetActionName():GetStr()
					if kActionResult:GetCritical() then
						actionName = "Critical_dmg"
					end
					kTargetActor:PlayWeaponSound(WST_HIT, kParentActor, actionName, 0, kActionTargetInfo);
					
					local	iSphereIndex = kActionTargetInfo:GetABVIndex();
					local pt = kTargetActor:GetABVShapeWorldPos(iSphereIndex);		

					if kActionResult:GetCritical() then
						kTargetActor:AttachParticleToPoint(12, pt, "e_dmg_cri")
					else
						kTargetActor:AttachParticleToPoint(12, pt, "e_dmg")
					end	

					--kTargetActor:SetShakeInPeriod(5,200);
						
				end
				
			end
			
			i=i+1;
			
		end
		
		kTargetList:ApplyActionEffects();
		
		
		--	쏜 놈한테 돌려보내자.
		kProjectile:SetTargetLoc(kParentPilot:GetActor():GetPos());
		kProjectile:Fire();
		kProjectile:SetParam(kProjectile:GetCurrentSpeed()*-1,1000,0);
				
		
	end

	return	true;	-- returning false means Delete This projectile
end
