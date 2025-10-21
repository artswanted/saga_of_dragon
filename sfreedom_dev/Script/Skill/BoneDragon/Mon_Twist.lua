-- 발사체 두방, 베지어곡선 이용, 땅에 붙여서

function Act_Mon_Shot_Twist_SetState(actor,action,kState)

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
		if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
			ODS("Act_Mon_Melee_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n")
			action:SetSlot(3)	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		iNewState = 1
	elseif kState == "FIRE" then
		action:SetSlot(2)
		iNewState = 2
	elseif kState == "RETURN" then
		
		action:SetSlot(4)
		iNewState = 3
	end

	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	action:SetParamInt(0,iNewState)
	return	true
end

function Act_Mon_Shot_Twist_OnCastingCompleted(actor, action)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action==nil);
	CheckNil(action:IsNil());
	
	ODS("Act_Mon_Shot_Twist_OnCastingCompleted\n")
	
	Act_Mon_Shot_Twist_SetState(actor,action,"FIRE")
	Act_Mon_Shot_Twist_Fire(actor,action)

end

function	Act_Mon_Shot_Twist_LoadToWeapon(actor,action,iProjectileIndex)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action==nil);
	CheckNil(action:IsNil());
	
	local	kAttachID=""
	
	if iProjectileIndex>0 then
		kAttachID = ""..iProjectileIndex;
	end
	
	local	kProjectileMan = GetProjectileMan()
	local	kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID"..kAttachID)
	local	fProjetileScale = tonumber(actor:GetAnimationInfo("PROJECTILE_SCALE"..kAttachID))
	if fProjetileScale == nil then
		fProjetileScale = 1.0
	end

	if iProjectileIndex>0 and kProjectileID==nil then
		return	nil
	end

	if kProjectileID == nil then
		kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID")
	end
	
	if kProjectileID == nil then
		kProjectileID = "Projectile_arrow_02_arrow_head"
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
	
	--kNewArrow:LoadToHelper(actor,kTargetNodeID)	--	장전
	--kNewArrow:LoadToPosition(actor:GetPos())
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local startpos = g_world:GetObjectPosByName("bone_start2")
	kNewArrow:LoadToPosition(startpos)
	return kNewArrow

end

function Act_Mon_Shot_Twist_Fire(actor,action)

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

function Act_Mon_Shot_Twist_OnEnter(actor, action)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action==nil);
	CheckNil(action:IsNil());
	
	ODS("Act_Mon_Melee_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n" )
	for i=0,9 do
		action:SetParamInt(10+i,-1)	--UID 저장
		action:SetParamInt(30+i,0)	--발사 여부 저장
	end
	action:SetParamInt(20,0)	--액션ID저장
	action:SetParamInt(21,0)	--프로젝틸 로드 여부
	
	local	prevAction = actor:GetAction()
	if prevAction:GetID() ~= "a_jump" then
		actor:Stop()
	end
	
	action:SetParamInt(40,0)
	local kPacket = action:GetParamAsPacket()
	if kPacket ~= nil and false == kPacket:IsNil() then
		local kNum = kPacket:PopByte()
		if kNum ~= 0 then
			local kIndex=0
			for i=0,kNum do
				local kGuid = kPacket:PopGuid()
				if nil ~= kGuid and false == kGuid:IsNil() then
					action:SetParam(10+kIndex,kGuid:GetString())
					kIndex = kIndex + 1
				end
			end
			action:SetParamInt(40,kIndex)
		end
	end
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_Shot_Twist_OnCastingCompleted(actor,action)
	else
	
	    ODS("Act_Mon_Shot_Twist_OnEnter Start Casting\n")

		if( Act_Mon_Shot_Twist_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Shot_Twist_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
				
		-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
		local	kTargetList = action:GetTargetList()
		local	iTargetCount = kTargetList:size()
	
		ODS("Act_Mon_Shot_Twist Casting Start iTargetCount :"..iTargetCount.."\n")
		
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

	action:SetParam(1, "none")

	return true
end

function Act_Mon_Shot_Twist_OnUpdate(actor, accumTime, frameTime)

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
				if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then
					return false
				else
					Act_Mon_Shot_Twist_SetState(actor,action,"RETURN")
				end
			end
			
			if iState == 3 then
				return false
			end
		end
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_RangeAttack_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
			
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Shot_Twist_OnCastingCompleted(actor,action)
			
			end
		end
		
	
	end
	return true
end

function Act_Mon_Shot_Twist_OnCleanUp(actor)
	return true
end

function Act_Mon_Shot_Twist_OnLeave(actor, action)
	ODS("Act_Mon_Shot_Twist_OnLeave\n")
	return true
end

function	Act_Mon_Shot_Twist_Projectile_Default_LoadToWeapon(actor,action)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action==nil);
	CheckNil(action:IsNil());
	
	local	kProjectileMan = GetProjectileMan();
	--local	kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	local	kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID")--..kAttachID)
	
	if kProjectileID == "" then
		kProjectileID = actor:GetEquippedWeaponProjectileID();
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	local	kTargetNodeID = action:GetScriptParam("ATTACH_NODE_ID");
	
	kNewArrow:LoadToHelper(actor,kTargetNodeID);	--	장전
	return	kNewArrow;
end

function Act_Mon_Shot_Twist_OnEvent(actor,textKey)

	CheckNil(actor==nil);
	CheckNil(actor:IsNil());
		
	ODS("Act_Mon_Shot_Twist_OnEvent textKey:"..textKey.."\n")

	local kAction = actor:GetAction()
		
	CheckNil(kAction==nil);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true
	end

	
	if textKey == "hit" or textKey == "fire" then

		actor:DetachFrom(230)	--캐스팅 이펙트 제거
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
		
		local	iAttackRange = kAction:GetSkillRange(0,actor)
		
		if iAttackRange == 0 then
			iAttackRange = 200 
		end
		
		local fire_delay = actor:GetAnimationInfo("FIRE_DELAY")
		if nil == fire_delay or "" == Projectile_cnt then
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
		
			local kArrow1 = Act_Mon_Shot_Twist_LoadToWeapon(actor, kAction, 0)
			local kArrow2 = Act_Mon_Shot_Twist_LoadToWeapon(actor, kAction, 0)
			local arrow_worldpos1 = kArrow1:GetWorldPos()
			local arrow_worldpos2 = kArrow2:GetWorldPos()
			local kTargetDir = actor:GetLookingDir()
			kTargetDir:Unitize()
			local kCrossDir = actor:GetLookingDir()
			kCrossDir:Unitize()
			kCrossDir:Cross(Point3(0,0,1))
			
			local kLength = 450 - math.random(0,200)--400
			local kPos1 = Point3(kCrossDir:GetX(),kCrossDir:GetY(),kCrossDir:GetZ())
			kPos1:Multiply(kLength)
			local kPos2 = Point3(kCrossDir:GetX(),kCrossDir:GetY(),kCrossDir:GetZ())
			kPos2:Multiply(-kLength)

			--ODS("1 X:"..kPos1:GetX().." Y : "..kPos1:GetY().." Z : "..kPos1:GetZ().."\n",false,987)
			--ODS("2 X:"..kPos2:GetX().." Y : "..kPos2:GetY().." Z : "..kPos2:GetZ().."\n",false,987)
			
			local iMax = math.max((1500/kLength)*9, 20)
			local fMulti = math.random(10,iMax)*0.1

			local kArrow1Pos1 = Point3(kTargetDir:GetX(),kTargetDir:GetY(),kTargetDir:GetZ())
			kArrow1Pos1:Multiply(kLength)
			kArrow1Pos1:Add(arrow_worldpos1)
			kArrow1Pos1:Add(kPos1)
			local kArrow1Pos2 = Point3(kTargetDir:GetX(),kTargetDir:GetY(),kTargetDir:GetZ())
			kArrow1Pos2:Multiply(kLength*fMulti)
			kArrow1Pos2:Add(arrow_worldpos1)
			kArrow1Pos2:Add(kPos2)
			
			ODS("1 X:"..kArrow1Pos1:GetX().." Y : "..kArrow1Pos1:GetY().." Z : "..kArrow1Pos1:GetZ().."\n",false,987)
			ODS("2 X:"..kArrow1Pos2:GetX().." Y : "..kArrow1Pos2:GetY().." Z : "..kArrow1Pos2:GetZ().."\n",false,987)
			
			local kArrow2Pos1 = Point3(kTargetDir:GetX(),kTargetDir:GetY(),kTargetDir:GetZ())
			kArrow2Pos1:Multiply(kLength)
			kArrow2Pos1:Add(arrow_worldpos2)
			kArrow2Pos1:Add(kPos2)
			local kArrow2Pos2 = Point3(kTargetDir:GetX(),kTargetDir:GetY(),kTargetDir:GetZ())
			kArrow2Pos2:Multiply(kLength*fMulti)
			kArrow2Pos2:Add(arrow_worldpos2)
			kArrow2Pos2:Add(kPos1)
			
			local kFireTarget = actor:GetLookingDir()
			kFireTarget:Unitize()
			kFireTarget:Multiply(1500)
			kFireTarget:Add(arrow_worldpos1)
			
			kArrow1:SetMovingType(2)
			kArrow1:SetParam_Bezier4SplineType(arrow_worldpos1, kArrow1Pos1, kArrow1Pos2, kFireTarget)
			kArrow1:SetTargetLoc(kFireTarget)
			--kArrow1:Fire()
			kArrow1:DelayFire(iArrowCount*fire_delay)
			
			kArrow2:SetMovingType(2)
			kArrow2:SetParam_Bezier4SplineType(arrow_worldpos2, kArrow2Pos1, kArrow2Pos2, kFireTarget)
			kArrow2:SetTargetLoc(kFireTarget)
			--kArrow2:Fire()
			kArrow2:DelayFire(iArrowCount*fire_delay)
			
			iArrowCount=iArrowCount+1
		end
		

		kAction:ClearTargetList()
					
	end
	
	if textKey == "quake" then
		Act_Mon_QuakeCamera(actor)
	end

	return	true
end
