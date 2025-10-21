function PROJECTILE_ARROW_SHOWER_OnLoadingStart(kProjectile)
	kProjectile:SetParamValue("ARRIVED","FALSE");
	return	true;
end
function PROJECTILE_ARROW_SHOWER_OnLoading(kProjectile)

	return	true;
end
function PROJECTILE_ARROW_SHOWER_OnFlyingStart(kProjectile)
	kProjectile:SetParamValue("z", tostring(kProjectile:GetWorldPos():GetZ()))
   ODS("PROJECTILE_ARROW_SHOWER_OnFlyingStart \n")
	return	true;
end

function PROJECTILE_ARROW_SHOWER_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_ARROW_SHOWER_OnFlying(kProjectile)
	local zpos = tonumber( kProjectile:GetParamValue("z") )
   ODS("zpos : " .. zpos .. " >  kProjectile:GetWorldPos():GetZ() " ..  kProjectile:GetWorldPos():GetZ() .. "\n")
	if zpos-1 > kProjectile:GetWorldPos():GetZ() then
		kProjectile:SetHide(true)
      ODS("프로젝틸 숨기기\n")
	end

	kProjectile:SetParamValue("z", tostring(kProjectile:GetWorldPos():GetZ()))

	return	true;
end
function PROJECTILE_ARROW_SHOWER_OnArrivedAtTarget(kProjectile)
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
	if kParentPilot:IsNil() then
		return false;
	end
	
--	PROJECTILE_ARROW_SHOWER_HitOneTime(kProjectile,kParentPilot:GetActor());

	kProjectile:SetParamValue("ARRIVED","TRUE");
	kProjectile:SetParamValue("ARRIVED_TIME",""..timeGetTime() + 250);	

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local ptcl = g_world:ThrowRay(kProjectile:GetWorldPos(), Point3(0,0,-1), 100)
	if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
		local kDir = kParentPilot:GetActor():GetLookingDir();
		local kLoc = kParentPilot:GetActor():GetNodeWorldPos("char_root")
		kDir:Add(kProjectile:GetWorldPos());
		kDir:SetZ(kLoc:GetZ());
		ptcl = kDir
	end
	ptcl:SetZ(ptcl:GetZ()+5)
	kParentPilot:GetActor():AttachParticleToPoint(1509, ptcl, "ef_arrow_shower");
	kParentPilot:GetActor():AttachSound(125,"ArrowShower");


	return true;
end
function PROJECTILE_ARROW_SHOWER_OnAnimationEvent(kProjectile)

	return	true
end

function PROJECTILE_ARROW_SHOWER_OnUpdate(kProjectile)
	
	if kProjectile:GetParamValue("ARRIVED") ~= "TRUE" then
		return true;
	end
	
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
	--if kParentPilot:IsNil() then
	--	return false;
	--end

	local	iCurrentTime = timeGetTime();
	local	iPrevTime = tonumber(kProjectile:GetParamValue("ARRIVED_TIME"));

	if (iCurrentTime - iPrevTime) > 90 then
		kProjectile:SetParamValue("ARRIVED_TIME",""..timeGetTime());
		local iRet = PROJECTILE_ARROW_SHOWER_HitOneTime(kProjectile,kParentPilot:GetActor());

		return iRet;
	end
	

	return	true;
end

function PROJECTILE_ARROW_SHOWER_DoDamage(kParentActor,kTargetActor,kActionTargetInfo,kProjectile, actionResult)

	local	kTargetGUID = kActionTargetInfo:GetTargetGUID();
	
	local	kSkillDef	= GetSkillDef(kProjectile:GetParentActionNo());
	local actionName = kSkillDef:GetActionName():GetStr();
	if actionResult:GetCritical() then
		actionName = "Critical_dmg"
	end
    kTargetActor:PlayWeaponSound(WST_HIT, kParentActor, actionName, 0, kActionTargetInfo);
	
	local	iSphereIndex = kActionTargetInfo:GetABVIndex();
	local pt = kTargetActor:GetABVShapeWorldPos(iSphereIndex);		
	pt:SetX(pt:GetX() + math.random(-10,10))
	pt:SetY(pt:GetY() + math.random(-10,10))
	pt:SetZ(pt:GetZ() + math.random(-5,5))

	local	iHitCount = tonumber(kProjectile:GetParamValue("HIT_COUNT"));
	
	if iHitCount == nil then
		iHitCount = 0
	end

	--if math.random() > 0.4 then
		kTargetActor:AttachParticleToPoint(12 + iHitCount, pt, "e_dmg")
	--else
	--	kTargetActor:AttachParticleToPoint(12 + iHitCount, pt, "e_dmg_cri")
	--end	

	--kTargetActor:SetShakeInPeriod(5,200);
end


function PROJECTILE_ARROW_SHOWER_HitOneTime(kProjectile,actor)	

	local	iHitCount = tonumber(kProjectile:GetParamValue("HIT_COUNT"));
	
	if iHitCount == nil then
		iHitCount = 0
	end

	local	iTotalHit = 8;
	
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
						
						PROJECTILE_ARROW_SHOWER_DoDamage(actor,actorTarget,kActionTargetInfo,kProjectile, actionResult);
					end
					
				end
				
				i=i+1;
			
			end
		end
		
		kTargetList:ApplyActionEffects();
			
	else
			
		kTargetList:ApplyOnlyDamage(iTotalHit, false, 15);
				
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
						
						PROJECTILE_ARROW_SHOWER_DoDamage(actor,actorTarget,kActionTargetInfo,kProjectile, actionResult);
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

function PROJECTILE_ARROW_SHOWER_OnTargetListModified(kProjectile)

	ODS("PROJECTILE_ARROW_SHOWER_OnTargetListModified\n");

	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
	
	return PROJECTILE_ARROW_SHOWER_HitOneTime(kProjectile,kParentPilot:GetActor());

end