-- 발사체 두방, 베지어곡선 이용, 땅에 붙여서
function Act_Mon_Shot_S_Curve_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	return		true;
	
end

function Act_Mon_Shot_S_Curve_SetState(actor,action,kState)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action==nil);
	CheckNil(action:IsNil());
	
	local	iNewState = -1
	if kState == "BATTLEIDLE_START" then
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			return	false
		end
		action:SetSlot(0)
		iNewState = 0
	elseif kState == "BATTLEIDLE_LOOP" then
		action:SetSlot(1)
		iNewState = 1
	elseif kState == "FIRE" then
		action:SetSlot(2)
		iNewState = 2
	elseif kState == "RETURN" then
		action:SetSlot(3)
		iNewState = 3
	end

	actor:PlayCurrentSlot()
	action:SetParamInt(0,iNewState)
	return	true
end

function Act_Mon_Shot_S_Curve_OnCastingCompleted(actor, action)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action==nil);
	CheckNil(action:IsNil());
	
	ODS("Act_Mon_Shot_S_Curve_OnCastingCompleted\n")
	
	Act_Mon_Shot_S_Curve_SetState(actor,action,"FIRE")
	Act_Mon_Shot_S_Curve_Fire(actor,action)

end

function	Act_Mon_Shot_S_Curve_LoadToWeapon(actor,action,iProjectileIndex)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action==nil);
	CheckNil(action:IsNil());

	local	kAttachID=""
	
	if iProjectileIndex>0 then
		kAttachID = ""..iProjectileIndex;
	end
	
	--ODS("Act_Mon_Shot_S_Curve_LoadToWeapon CurrentSlot:"..action:GetCurrentSlot().."\n")
	local	kProjectileMan = GetProjectileMan()
	local	kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID"..kAttachID)
	if kProjectileID == nil then
		kProjectileID = "Projectile_S_Curve"
	end

	local	fProjetileScale = tonumber(actor:GetAnimationInfo("PROJECTILE_SCALE"..kAttachID))
	if fProjetileScale == nil then
		fProjetileScale = 1.0
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid())
	if kNewArrow:IsNil() then
		return nil
	end
	
	local	kTargetNodeID = actor:GetAnimationInfo("FIRE_START_NODE"..kAttachID)
	if kTargetNodeID == nil then
		kTargetNodeID = "p_ef_heart"
	end
	
	local	fProjectileSpeed = actor:GetAnimationInfo("PROJECTILE_SPEED"..kAttachID)
	if fProjectileSpeed ~= nil then
		fProjectileSpeed = tonumber(fProjectileSpeed)
		kNewArrow:SetSpeed(fProjectileSpeed)
	end
		
	kNewArrow:SetScale(fProjetileScale*kNewArrow:GetScale())
	
	kNewArrow:LoadToHelper(actor,kTargetNodeID)	--	장전
	return kNewArrow
end

function Act_Mon_Shot_S_Curve_Fire(actor,action)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action==nil);
	CheckNil(action:IsNil());
	
	--	is this boomerang type?
	local	bIsBoomerang = (actor:GetAnimationInfo("PROJECTILE_TYPE") == "BOOMERANG")
	if bIsBoomerang then
		action:SetParamInt(3,1)	--	this is boomerang type
	end
end

function Act_Mon_Shot_S_Curve_OnEnter(actor, action)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action==nil);
	CheckNil(action:IsNil());
	
	ODS("Act_Mon_Shot_S_Curve_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n" )	
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_Shot_S_Curve_OnCastingCompleted(actor,action)
	else
	
	    ODS("Act_Mon_Shot_S_Curve_OnEnter Start Casting\n")

		if( Act_Mon_Shot_S_Curve_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Shot_S_Curve_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
				
		-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
		local	kTargetList = action:GetTargetList()
		local	iTargetCount = kTargetList:size()
	
		ODS("Act_Mon_Shot_S_Curve_OnEnter Casting Start iTargetCount :"..iTargetCount.."\n")
		
		if iTargetCount>0 then
			local	kTargetInfo = kTargetList:GetTargetInfo(0)
			if kTargetInfo:IsNil() == false then
			
				local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID())
				if kTargetPilot:IsNil() == false then
				
					local	kTargetActor = kTargetPilot:GetActor()
					if kTargetActor:IsNil() == false then
					
						--actor:LookAtBidirection(kTargetActor:GetPos())
						actor:LookAt(kTargetActor:GetPos(),true,true)
					
					end
				end
			end
		end
	end

	return true
end

function Act_Mon_Shot_S_Curve_OnUpdate(actor, accumTime, frameTime)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(action==nil);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local	iState = action:GetParamInt(0)
		
	
	if action:GetActionParam() == AP_FIRE then
		if animDone == true then
			if iState == 2 then
				if actor:GetAnimationLength(action:GetSlotAnimName(3)) == 0 then
					return false
				else
					Act_Mon_Shot_S_Curve_SetState(actor,action,"RETURN")
				end
			end
			
			if iState == 3 then
				return false
			end
		end
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_Shot_S_Curve_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
			
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Shot_S_Curve_OnCastingCompleted(actor,action)
			
			end
		end
		
	
	end
	return true
end

function Act_Mon_Shot_S_Curve_OnCleanUp(actor)
	return true
end

function Act_Mon_Shot_S_Curve_OnLeave(actor, action)
	ODS("Act_Mon_Shot_S_Curve_OnLeave\n")
	return true
end

function Act_Mon_Shot_S_Curve_SetProjectileDirection(kProjectile, fDelayFire)
	if nil==kProjectile then
		return false
	end

	local iSkillRange = tonumber(kProjectile:GetParamValue("iSkillRange"))
	local iSectionRange = tonumber(kProjectile:GetParamValue("iSectionRange"))
	local iCurve = tonumber(kProjectile:GetParamValue("iCurve"))
	local kLookingDir = Point3(tonumber(kProjectile:GetParamValue("kLookingDir.x")), tonumber(kProjectile:GetParamValue("kLookingDir.y")), tonumber(kProjectile:GetParamValue("kLookingDir.z")))
	--ODS("kLookingDir x:"..kLookingDir:GetX()..", y:"..kLookingDir:GetY()..", z:"..kLookingDir:GetZ().."\n")
	
	if iSkillRange < 0 then
		return false
	end

	local arrow_worldpos1 = kProjectile:GetWorldPos()
	--ODS("arrow_worldpos1 x:"..arrow_worldpos1:GetX()..", y:"..arrow_worldpos1:GetY()..", z:"..arrow_worldpos1:GetZ().."\n")
	local kTargetDir = Point3(kLookingDir:GetX(),kLookingDir:GetY(),kLookingDir:GetZ())
	local kCrossDir = Point3(kLookingDir:GetX(),kLookingDir:GetY(),kLookingDir:GetZ())
	kCrossDir:Cross(Point3(0,0,1))
			
	local kOneThird = iSectionRange/3
	local kLength1 = kOneThird + math.random(-kOneThird/2,kOneThird/2)
	local kLength2 = kOneThird + math.random(-kOneThird/2,kOneThird/2)
	local kPos1 = Point3(kCrossDir:GetX(),kCrossDir:GetY(),kCrossDir:GetZ())
	kPos1:Multiply(iCurve+math.random(-iCurve/2,iCurve/2))
	local kPos2 = Point3(kCrossDir:GetX(),kCrossDir:GetY(),kCrossDir:GetZ())
	kPos2:Multiply(-(iCurve+math.random(-iCurve/2,iCurve/2)))

	--ODS("1 X:"..kPos1:GetX().." Y : "..kPos1:GetY().." Z : "..kPos1:GetZ().."\n",false,987)
	--ODS("2 X:"..kPos2:GetX().." Y : "..kPos2:GetY().." Z : "..kPos2:GetZ().."\n",false,987)

	local kArrow1Pos1 = Point3(kTargetDir:GetX(),kTargetDir:GetY(),kTargetDir:GetZ())
	kArrow1Pos1:Multiply(kLength1)
	kArrow1Pos1:Add(arrow_worldpos1)
	kArrow1Pos1:Add(kPos1)
	local kArrow1Pos2 = Point3(kTargetDir:GetX(),kTargetDir:GetY(),kTargetDir:GetZ())
	kArrow1Pos2:Multiply(kLength1+kLength2)
	kArrow1Pos2:Add(arrow_worldpos1)
	kArrow1Pos2:Add(kPos2)
	
	--ODS("1 X:"..kArrow1Pos1:GetX().." Y : "..kArrow1Pos1:GetY().." Z : "..kArrow1Pos1:GetZ().."\n",false,987)
	--ODS("2 X:"..kArrow1Pos2:GetX().." Y : "..kArrow1Pos2:GetY().." Z : "..kArrow1Pos2:GetZ().."\n",false,987)
	
	local kFireTarget = kLookingDir
	kFireTarget:Unitize()
	kFireTarget:Multiply(iSectionRange)
	kFireTarget:Add(arrow_worldpos1)
	
	kProjectile:SetMovingType(2)
	kProjectile:SetParam_Bezier4SplineType(arrow_worldpos1, kArrow1Pos1, kArrow1Pos2, kFireTarget)
	kProjectile:SetTargetLoc(kFireTarget)
	if fDelayFire then
		kProjectile:DelayFire(fDelayFire)
	else
		kProjectile:Fire()
	end

	--
	kProjectile:SetParamValue("iSkillRange", iSkillRange-iSectionRange)
	kProjectile:SetParamValue("iSectionRange", iSectionRange)
	kProjectile:SetParamValue("iCurve", iCurve)
	return true
end

function Act_Mon_Shot_S_Curve_OnEvent(actor,textKey)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());

	local kAction = actor:GetAction()
		
	CheckNil(kAction==nil);
	CheckNil(kAction:IsNil());
	
	--ODS("Act_Mon_Shot_S_Curve_OnEvent textKey:"..textKey..", CurrentSlot:"..kAction:GetCurrentSlot().. "\n")

	if kAction:GetActionParam() == AP_CASTING then
		return true
	end
	
	if textKey == "hit" or textKey == "fire" then
		local	bIsBoomerang = (actor:GetAnimationInfo("PROJECTILE_TYPE") == "BOOMERANG")

		if bIsBoomerang then
			actor:HideParts(EQUIP_POS_WEAPON, true)	--	칼 숨기자
			kAction:SetParamInt(4,1)	--	칼 숨겼음		
		end
		
		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID")
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID)
		end

		local fireEffect = actor:GetAnimationInfo("FIRE_EFFECT")	
		if fireEffect ~= nil then
			local fireStartNode	= actor:GetAnimationInfo("FIRE_EFFECT_NODE")	
			
			if fireStartNode == nil then
				fireStartNode = "p_ef_heart"
			end

			local EffectScale = actor:GetAnimationInfo("FIRE_EFFECT_SCALE")
			if nil==EffectScale or ""==EffectScale then
				EffectScale = 1
			else
				EffectScale = tonumber(EffectScale)
			end
			
			actor:AttachParticleS(231, fireStartNode, fireEffect, EffectScale)
		end
		
		local	iSkillRange = kAction:GetSkillRange(0,actor)
		
		if iSkillRange == 0 then
			iSkillRange = 250 
		end
		--ODS("iSkillRange: "..iSkillRange.."\n")

		local	iSectionRange = actor:GetAnimationInfo("SECTION_RANGE")
		if nil == iSectionRange or "" == iSectionRange then
			iSectionRange = iSkillRange
		else
			iSectionRange = tonumber(iSectionRange)
		end
		
		local	iCurve = actor:GetAnimationInfo("CURVE_VALUE")
		if nil == iCurve or "" == iCurve then
			iCurve = 100
		else
			iCurve = tonumber(iCurve)
		end
		
		local fire_delay = actor:GetAnimationInfo("FIRE_DELAY")
		if nil == fire_delay or "" == fire_delay then
			fire_delay = 0 -- 기본 딜레이 없음
		else
			fire_delay = tonumber(fire_delay)
		end

		local Projectile_cnt = actor:GetAnimationInfo("PROJECTILE_COUNT")
		if nil == Projectile_cnt or "" == Projectile_cnt then
			Projectile_cnt = 1
		else
			Projectile_cnt = tonumber(Projectile_cnt)
		end

		local   iArrowCount = 0
		while   iArrowCount<Projectile_cnt do
		
			local kArrow1 = Act_Mon_Shot_S_Curve_LoadToWeapon(actor, kAction, 0)
			local kTargetDir = actor:GetLookingDir()
			kTargetDir:Unitize()
			--ODS("kTargetDir x:"..kTargetDir:GetX()..", y:"..kTargetDir:GetY()..", z:"..kTargetDir:GetZ().."\n")
			kArrow1:SetParamValue("kLookingDir.x", kTargetDir:GetX())
			kArrow1:SetParamValue("kLookingDir.y", kTargetDir:GetY())
			kArrow1:SetParamValue("kLookingDir.z", kTargetDir:GetZ())
			kArrow1:SetParamValue("iSkillRange", iSkillRange)
			kArrow1:SetParamValue("iSectionRange", iSectionRange)
			kArrow1:SetParamValue("iCurve", iCurve)
			Act_Mon_Shot_S_Curve_SetProjectileDirection(kArrow1, iArrowCount*fire_delay)

			iArrowCount=iArrowCount+1
		end
		

		kAction:ClearTargetList()

	end
	
	if textKey == "quake" then
		Act_Mon_QuakeCamera(actor)
	end

	return	true
end
