function MagicCannon_Shot_OnCheckCanEnter(kActor, kAction)
	return true
end

function MagicCannon_Shot_OnCastingCompleted(kActor, kAction)
	return true
end

function MagicCannon_Shot_SetState(kActor,kAction,kState)
	--ODS( GetAccumTime() .. " MagicCannon_Shot_SetState ......" .. kState .. "\n", false, 998 )
	local	iNewState = -1
	if kState == "FIRE" then
		if kActor:GetAnimationLength(kAction:GetSlotAnimName(0)) == 0 then
			return	false
		end
		kAction:SetSlot(0)
		iNewState = 0
	elseif kState == "RETURN" then
		kAction:SetSlot(1)
		iNewState = 1
	end
	
	kActor:ResetAnimation()
	kActor:PlayCurrentSlot()
	kAction:SetParamInt(0,iNewState)
	
	return true
end

function	MagicCannon_Shot_LoadToWeapon(kActor, kAction)
	--ODS( GetAccumTime() .. " MagicCannon_Shot_LoadToWeapon..... \n", false, 998 )
	
	local	kProjectileMan = GetProjectileMan()
	local	kProjectileID = kActor:GetAnimationInfo("PROJECTILE_ID")
	local	fProjetileScale = tonumber(kActor:GetAnimationInfo("PROJECTILE_SCALE"))
	if nil  == fProjetileScale then
		fProjetileScale = 1.0
	end
	if nil == kProjectileID then
		return nil
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID, kAction, kActor:GetPilotGuid())
	if kNewArrow:IsNil() then
		return nil
	end
	
	local	kTargetNodeID = kActor:GetAnimationInfo("FIRE_START_NODE")
	if nil == kTargetNodeID then
		kTargetNodeID = "p_ef_heart"
	end
	
	local	fProjectileSpeed = kActor:GetAnimationInfo("PROJECTILE_SPEED")
	if nil ~= fProjectileSpeed then
		fProjectileSpeed = tonumber(fProjectileSpeed)
		kNewArrow:SetSpeed(fProjectileSpeed)
	end
	
	kNewArrow:SetScale(fProjetileScale*kNewArrow:GetScale())
	
	kNewArrow:LoadToHelper(kActor,kTargetNodeID)
	return kNewArrow
end

function MagicCannon_Shot_OnEnter(kActor, kAction)
	--ODS( GetAccumTime() .. " MagicCannon_Shot_OnEnter ....." .. kAction:GetActionParam() .. "\n", false, 998 )
	
	local	prevAction = kActor:GetAction()
	if prevAction:GetID() ~= "a_jump" then
		kActor:Stop()
	end
	
	--[[
		Casting -> Fire
		Fire -> BIdle
	]]
	
	if kAction:GetActionParam() == AP_CASTING then
		if not MagicCannon_Shot_SetState(kActor,kAction,"FIRE") then
			return false
		end
	else --if kAction:GetActionParam() == AP_FIRE then
		MagicCannon_Shot_SetState(kActor,kAction,"RETURN")

		--[[local	kTargetList = kAction:GetTargetList()
		if nil ~= kTargetList and not kTargetList:IsNil() and 0 < kTargetList:size() then
			local	kTargetInfo = kTargetList:GetTargetInfo(0)
			if not kTargetInfo:IsNil() then
				local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID())
				if not kTargetPilot:IsNil() then
					local	kTargetActor = kTargetPilot:GetActor()
					if not kTargetActor:IsNil() then
						kActor:LookAt(kTargetActor:GetPos(),true,false)
					end
				end
			end
		end]]--
	end
	return true
end

function MagicCannon_Shot_OnUpdate(kActor, accumTime, frameTime)
	local kAction = kActor:GetAction()
	local bAnimDone = kActor:IsAnimationDone()
	local iState = kAction:GetParamInt(0)
	
	if kAction:GetActionParam() == AP_FIRE then
		if bAnimDone == true then
			return false
		end
	elseif kAction:GetActionParam() == AP_CASTING then
		if iState == 0 then
			if kActor:IsAnimationDone() then
				MagicCannon_Shot_SetState(kActor,kAction,"RETURN")
			end
		end
	end
	return true
end

function MagicCannon_Shot_OnCleanUp(kActor)
	--ODS( GetAccumTime() .. " MagicCannon_Shot_OnCleanUp .....\n", false, 998 )
	kActor:AttachSound(12, "MagicCannon_Idle01")
end

function MagicCannon_Shot_OnLeave(kActor, action)
	--ODS( GetAccumTime() .. " MagicCannon_Shot_OnLeave .....\n", false, 998 )
	return true
end

function MagicCannon_Shot_OnEvent(kActor, kTextKey)
	local kAction = kActor:GetAction()
	--[[if kAction:GetActionParam() == AP_CASTING then
		return true
	end]]
	
	if kTextKey == "fire" then
		--ODS( GetAccumTime() .. " MagicCannon_Shot_OnEvent ..... " .. kTextKey .. " \n", false, 998 )
		--[[ local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID")
		if kSoundID~="" then
			kActor:AttachSound(2784,kSoundID)
		end ]]
		
		
		local	kProjectileMan = GetProjectileMan()
		local	kArrow = MagicCannon_Shot_LoadToWeapon(kActor,kAction)
		if not kArrow:IsNil() then
			local	iTargetCount = kAction:GetTargetCount()
			local	iTargetABVShapeIndex = 0
			local	kActionTargetInfo = nil
			if iTargetCount>0 then
				--[[local kTargetGUID = kAction:GetTargetGUID(0)
				iTargetABVShapeIndex = kAction:GetTargetABVShapeIndex(0)
				kActionTargetInfo = kAction:GetTargetInfo(0)
				local kTargetobject = g_pilotMan:FindPilot(kTargetGUID)
				if kTargetobject:IsNil() == false then
					local kTargetActor = kTargetobject:GetActor()
					if not kTargetActor:IsNil() then
						local kTargetLoc = kTargetActor:GetABVShapeWorldPos(iTargetABVShapeIndex)]]
						--kActor:LookAt(kTargetLoc,true,false)
						--kTargetLoc:Add( Point3(0,-100,150) )
						--kArrow:SetTargetLoc(kTargetLoc)
						kArrow:SetTargetLoc( Point3(0,-290,170) )
						--kArrow:SetTargetLoc( kTargetActor:GetNodeTranslate("p_ef_head") )
						--kArrow:SetTargetObject( kActionTargetInfo )
						
						--[[kActor:ReleaseAllParticles()
						kArrow:SetParamValue("exValue", tostring(-150));
					end
				end]]
			end
			
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp())
			kArrow:Fire()
		end
		kAction:ClearTargetList()
	end
	return	true
end