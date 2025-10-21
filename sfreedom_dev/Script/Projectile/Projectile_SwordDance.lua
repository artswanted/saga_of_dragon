function PROJECTILE_SWORDDANCE_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_SWORDDANCE_OnArrivedAtTarget(kProjectile)

	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
		if kParentPilot:IsNil() then
			return false;
		end
	PROJECTILE_SWORDDANCE_HitOneTime(kProjectile,kParentPilot:GetActor());

	return false;
end

function PROJECTILE_SWORDDANCE_DoDamage(kParentActor,kTargetActor,kActionTargetInfo,kProjectile,actionResult)

	local	kTargetGUID = kActionTargetInfo:GetTargetGUID();
	
	local	kSkillDef	= GetSkillDef(kProjectile:GetParentActionNo());
	local actionName = kSkillDef:GetActionName():GetStr()
	if actionResult:GetCritical() then
		actionName = "Critical_dmg"
	end
	kTargetActor:PlayWeaponSound(WST_HIT, kParentActor, actionName, 0, kActionTargetInfo);
	
	local	iSphereIndex = kActionTargetInfo:GetABVIndex();
	local pt = kTargetActor:GetABVShapeWorldPos(iSphereIndex);		

	if actionResult:GetCritical() then
		kTargetActor:AttachParticleToPoint(12, pt, "e_dmg_cri")
	else
		kTargetActor:AttachParticleToPoint(12, pt, "e_dmg")
	end	

	--kTargetActor:SetShakeInPeriod(5,200);
end


function PROJECTILE_SWORDDANCE_HitOneTime(kProjectile,actor)

	local	iHitCount = tonumber(kProjectile:GetParamValue("HIT_COUNT"));
	
	if iHitCount == nil then
		iHitCount = 0
	end

	local	iTotalHit = 1;
	
	if iHitCount == iTotalHit then
		return false;
	end
	
	local	kTargetList = kProjectile:GetActionTargetList();
		
	if iHitCount == iTotalHit-1 then
	
		local iTargetCount = kTargetList:size();
		local i =0;
		if iTargetCount>0 then
			
			while i<iTargetCount do
			
				local	kActionTargetInfo = kTargetList:GetTargetInfo(i);
				local actionResult = kActionTargetInfo:GetActionResult();
				
				if actionResult:IsNil() == false and actionResult:IsMissed() == false then
				
					local	iOneDmg = atoi(""..actionResult:GetValue()/(iTotalHit));
					actionResult:SetValue(actionResult:GetValue() - iOneDmg*(iTotalHit-1));
						
					local kTargetGUID = kActionTargetInfo:GetTargetGUID();
					local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
					if kTargetPilot:IsNil() == false then
						
						local actorTarget = kTargetPilot:GetActor();
						
						PROJECTILE_SWORDDANCE_DoDamage(actor,actorTarget,kActionTargetInfo,kProjectile,actionResult);
					end
					
				end
				
				i=i+1;
			
			end
		end
				
		kTargetList:ApplyActionEffects();
	
	else
	
		kTargetList:ApplyOnlyDamage(iTotalHit);
		
		local iTargetCount = kTargetList:size();
		local i =0;
		if iTargetCount>0 then
			
			while i<iTargetCount do
			
				local	kActionTargetInfo = kTargetList:GetTargetInfo(i);
				local actionResult = kActionTargetInfo:GetActionResult();
				
				if actionResult:IsNil() == false and actionResult:IsMissed() == false then
				
					local kTargetGUID = kActionTargetInfo:GetTargetGUID();
					local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
					if kTargetPilot:IsNil() == false then
						
						local actorTarget = kTargetPilot:GetActor();
						
						PROJECTILE_SWORDDANCE_DoDamage(actor,actorTarget,kActionTargetInfo,kProjectile,actionResult);
					end
				end
				
				i=i+1;
			
			end
		end
		
	end
	
	
	iHitCount = iHitCount + 1;
	kProjectile:SetParamValue("HIT_COUNT",""..iHitCount)
	
	if iHitCount == iTotalHit then
		return	false	--	다 때렸다
	end
	
	return	true;
	

end


function PROJECTILE_SWORDDANCE_OnTargetListModified(kProjectile)
	
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
	if kParentPilot:IsNil() then
		return false;
	end
	
	PROJECTILE_SWORDDANCE_HitOneTime(kProjectile,kParentPilot:GetActor());
end