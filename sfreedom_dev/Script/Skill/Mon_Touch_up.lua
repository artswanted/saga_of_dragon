-- 발사체 목표지점에 도착하면 다른 발사체가 새로 발동됨
function Act_Mon_Touch_up_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	return		true;
	
end

function Act_Mon_Touch_up_SetState(actor,action,kState)

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

function Act_Mon_Touch_up_OnCastingCompleted(actor, action)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action==nil);
	CheckNil(action:IsNil());
	
	--ODS("Act_Mon_Touch_up_OnCastingCompleted\n")
	
	Act_Mon_Touch_up_SetState(actor,action,"FIRE")
	Act_Mon_Touch_up_Fire(actor,action)

end

function	Act_Mon_Touch_up_LoadToWeapon(actor,action,iProjectileIndex)
	--ODS("Act_Mon_Touch_up_LoadToWeapon\n",false,6482)
	CheckNil(actor==nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action==nil);
	CheckNil(action:IsNil());

	local	kAttachID=""
	
	if iProjectileIndex>0 then
		kAttachID = ""..iProjectileIndex;
	end
	
	--ODS("Act_Mon_Touch_up_LoadToWeapon CurrentSlot:"..action:GetCurrentSlot().."\n")
	local	kProjectileMan = GetProjectileMan()
	local	kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID"..kAttachID)
	if kProjectileID == nil then
		kProjectileID = "Projectile_Touch_up"
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
		kTargetNodeID = "p_ef_mouth"
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

function Act_Mon_Touch_up_Fire(actor,action)

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

function Act_Mon_Touch_up_OnEnter(actor, action)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action==nil);
	CheckNil(action:IsNil());
	
	--ODS("Act_Mon_Touch_up_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n")
	
	local kPacket = action:GetParamAsPacket()
	if kPacket ~= nil and false == kPacket:IsNil() then
		local byPacketType = kPacket:PopByte()
		local iCount = kPacket:PopInt()
		
		action:SetParamAsPoint(0, kPacket:PopPoint3() )
	end
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_Touch_up_OnCastingCompleted(actor,action)
	else
	
	    --ODS("Act_Mon_Touch_up_OnEnter Start Casting\n")

		if( Act_Mon_Touch_up_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Touch_up_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
				
		-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
		local	kTargetList = action:GetTargetList()
		local	iTargetCount = kTargetList:size()
	
		--ODS("Act_Mon_Touch_up_OnEnter Casting Start iTargetCount :"..iTargetCount.."\n")
		
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

function Act_Mon_Touch_up_OnUpdate(actor, accumTime, frameTime)
	--ODS("Act_Mon_Touch_up_OnUpdate \n",false,6482)
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
					Act_Mon_Touch_up_SetState(actor,action,"RETURN")
				end
			end
			
			if iState == 3 then
				return false
			end
		end
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_Touch_up_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
			
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Touch_up_OnCastingCompleted(actor,action)
			
			end
		end
		
	
	end
	return true
end

function Act_Mon_Touch_up_OnCleanUp(actor)
	return true
end

function Act_Mon_Touch_up_OnLeave(actor, action)
	ODS("Act_Mon_Touch_up_OnLeave\n")
	return true
end

function  Act_Mon_Touch_up_NextProjectile(kProjectile, fDelayFire)	
	if nil==kProjectile then
		return false
	end

	--ODS("Act_Mon_Touch_up_NextProjectile\n",false,6482)
	--발사체 나갈때의 이펙트
	--새로운 발사체
	--액션은 새로 만들어서 넣자. 그래야 공격대미지를 제대로 줄수 있겠지..

	local upActionNo = tonumber(kProjectile:GetParamValue("UpActionNo"))
	local upProjectileSpeed = tonumber(kProjectile:GetParamValue("UpProjectileSpeed"))
	local upProjectileScale = tonumber(kProjectile:GetParamValue("UpProjectileScale"))
	local upProjectileAlpha = tonumber(kProjectile:GetParamValue("UpProjectileAlpha"))
	local upProjectileHeight = tonumber(kProjectile:GetParamValue("UpProjectileHeight"))
	local upFireEffect = kProjectile:GetParamValue("UpFireEffect")
	local upFireEffectScale = kProjectile:GetParamValue("UpFireEffectScale")
	local upFireSound = kProjectile:GetParamValue("UpFireSound")
		
	
	--ODS("NextProjectile UpActionNo:"..upActionNo..", FireEffect:"..upFireEffect..", ProjectileSpeed:"..upProjectileSpeed..", upProjectileScale:"..upProjectileScale.."\n",false,6482)
	
	if upActionNo == 0 then
		-- local iAttachParticleNo = tonumber(kProjectile:GetParamValue("UpFireAttachParticleNo"))
		-- g_world:DetachParticle(iAttachParticleNo)		
		return false
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local actor = g_world:FindActor(kProjectile:GetParentPilotGuid())
	if nil~=actor and false==actor:IsNil() then
		if nil~=upFireEffect or ""~=upFireEffect then
			local iAttachParticleNo = g_world:AttachParticleSWithRotate(upFireEffect, kProjectile:GetWorldPos(), upFireEffectScale, Point3(0,0,0))
			kProjectile:SetParamValue("UpFireAttachParticleNo", iAttachParticleNo)
		end
		if nil~=upFireSound or ""~=upFireSound then
			g_world:AttachSound(upFireSound,kProjectile:GetWorldPos())
		end

		kProjectile:SetParamValue("USE_FLYING_START", "FALSE")
		kProjectile:SetMovingType(0)
		if upProjectileSpeed then
			kProjectile:SetSpeed(upProjectileSpeed)
		end
		kProjectile:SetAlpha(upProjectileAlpha)
		kProjectile:SetHide(1==upProjectileAlpha)
		kProjectile:SetScale(upProjectileScale)
		kProjectile:SetParentActionInfo(upActionNo,kProjectile:GetParentActionInstanceID(),kProjectile:GetParentActionTimeStamp());
		
		local kTargetPos = kProjectile:GetWorldPos()
		kTargetPos:SetZ(kTargetPos:GetZ()+20)
		kProjectile:LoadToPosition(kTargetPos)
		kTargetPos:SetZ(kTargetPos:GetZ()+upProjectileHeight)
		kProjectile:SetTargetLoc(kTargetPos)
		kProjectile:Fire()
		
	end

	kProjectile:SetParamValue("UpActionNo", 0)
	return true
end

function Act_Mon_Touch_up_OnEvent(actor,textKey)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());

	local kAction = actor:GetAction()
		
	CheckNil(kAction==nil);
	CheckNil(kAction:IsNil());

	if kAction:GetActionParam() == AP_CASTING then
		return true
	end
	
	--ODS("Act_Mon_Touch_up_OnEvent textKey:"..textKey.."\n",false,6482)	
	
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

		local fire_delay = actor:GetAnimationInfo("FIRE_TERM")
		if nil == fire_delay or "" == fire_delay then
			fire_delay = 0 -- 기본 딜레이 없음
		else
			fire_delay = tonumber(fire_delay)
		end		

		local	upActionNo = actor:GetAnimationInfo("UP_ACTION_NO")
		if nil == upActionNo or "" == upActionNo then
			upActionNo = kAction:GetActionNo()
		else
			upActionNo = tonumber(upActionNo)
		end
		
		local upFireEffect = actor:GetAnimationInfo("UP_FIRE_EFFECT")	
		if nil == upFireEffect then
			upFireEffect = ""
		end
		
		local upFireEffectScale = actor:GetAnimationInfo("UP_FIRE_EFFECT_SCALE")
		if nil == upFireEffectScale then
			upFireEffectScale = 1
		else
			upFireEffectScale = tonumber(upFireEffectScale)
		end

		local upFireSound = actor:GetAnimationInfo("UP_FIRE_SOUND")
		if nil == upFireSound then
			upFireSound = ""
		end

		local fZ = actor:GetAnimationInfo("Z_FACTOR")
		if nil == fZ or "" == fZ then
			fZ = 2
		else
			fZ = tonumber(fZ)
		end
		
		local iDistance = actor:GetAnimationInfo("DISTANCE")
		if nil == iDistance or "" == iDistance then
			iDistance = 250
		else
			iDistance = tonumber(iDistance)
		end
		
		--ODS("UpActionNo:"..upActionNo..", FireEffect:"..upFireEffect..", fZ:"..fZ..", Distance:"..iDistance.."\n",false,6482)
		
		local fProjectileSpeed = actor:GetAnimationInfo("PROJECTILE_SPEED")
		if fProjectileSpeed ~= nil then
			fProjectileSpeed = tonumber(fProjectileSpeed)
		end
		
		local upProjectileSpeed = actor:GetAnimationInfo("UP_PROJECTILE_SPEED")
		if upProjectileSpeed == nil then
			upProjectileSpeed = 700
		else
			upProjectileSpeed = tonumber(upProjectileSpeed)
		end

		local upProjectileScale = actor:GetAnimationInfo("UP_PROJECTILE_SCALE")
		if upProjectileScale == nil then
			upProjectileScale = 1
		else
			upProjectileScale = tonumber(upProjectileScale)
		end
		
		local upProjectileAlpha = actor:GetAnimationInfo("UP_PROJECTILE_ALPHA")
		if upProjectileAlpha == nil then
			upProjectileAlpha = 0
		else
			upProjectileAlpha = tonumber(upProjectileAlpha)
		end
		
		local upProjectileHeight = actor:GetAnimationInfo("UP_PROJECTILE_HEIGHT")
		if upProjectileHeight == nil then
			upProjectileHeight = 200
		else
			upProjectileHeight = tonumber(upProjectileHeight)
		end
		
		local Projectile_cnt = actor:GetAnimationInfo("PROJECTILE_COUNT")
		if nil == Projectile_cnt or "" == Projectile_cnt then
			Projectile_cnt = 1
		else
			Projectile_cnt = tonumber(Projectile_cnt)
		end

		local   iArrowCount = 0
		while   iArrowCount<Projectile_cnt do
		
			local kArrow1 = Act_Mon_Touch_up_LoadToWeapon(actor, kAction, 0)
			
			if fProjectileSpeed then
				kArrow1:SetSpeed(fProjectileSpeed)
			end
			
			-- 관통
			local iPentrationCount = kAction:GetAbil(AT_PENETRATION_COUNT);
			kArrow1:SetPenetrationCount(iPentrationCount);-- 몇번을 관통할것인지 세팅
			if iPentrationCount ~= 0 then
				kArrow1:SetMultipleAttack(true);
			end

			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			local	kTargetLoc = kAction:GetParamAsPoint(0)
			if kTargetLoc:IsZero() and kAction:GetTargetCount() > 0 then
					--	타겟을 찾아 바라본다
				local	kTargetGUID = kAction:GetTargetGUID(0)
				local	iTargetABVShapeIndex = kAction:GetTargetABVShapeIndex(0)
				local 	kTargetActor = g_world:FindActor(kTargetGUID)
				if kTargetActor:IsNil() == false then
					local pt = kTargetActor:GetABVShapeWorldPos(iTargetABVShapeIndex)
					actor:LookAt(pt,true,true)
					kTargetLoc = kTargetActor:GetPos()
				end
			end

			if nil==kTargetLoc or kTargetLoc:IsZero() then
				local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
				kTargetLoc = kMovingDir:_Multiply(iDistance);
				kTargetLoc:Add(actor:GetPos());
			end
			local	ptpoint = Point3(kTargetLoc:GetX(), kTargetLoc:GetY(), kTargetLoc:GetZ() + 100)
			kTargetLoc = g_world:ThrowRay(ptpoint, Point3(0,0,-1), 300)
			kTargetLoc:SetZ(kTargetLoc:GetZ()+1)

			kArrow1:SetTargetLoc(kTargetLoc)
			
			kArrow1:SetParamValue("Z_FACTOR", fZ)
			kArrow1:SetParamValue("UpActionNo", upActionNo)
			kArrow1:SetParamValue("UpProjectileSpeed", upProjectileSpeed)
			kArrow1:SetParamValue("UpProjectileScale", upProjectileScale)
			kArrow1:SetParamValue("UpProjectileAlpha", upProjectileAlpha)
			kArrow1:SetParamValue("UpProjectileHeight", upProjectileHeight)			
			kArrow1:SetParamValue("UpFireEffect", upFireEffect)
			kArrow1:SetParamValue("UpFireEffectScale", upFireEffectScale)
			kArrow1:SetParamValue("UpFireSound", upFireSound)
			
			if fire_delay then
				kArrow1:DelayFire(iArrowCount*fire_delay, true)
			else
				kArrow1:Fire(true)
			end

			iArrowCount=iArrowCount+1
		end
		

		kAction:ClearTargetList()

	end
	
	if textKey == "quake" then
		Act_Mon_QuakeCamera(actor)
	end

	return	true
end

function Act_Mon_Touch_up_OnTargetListModified(actor,action,bIsBefore)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
end