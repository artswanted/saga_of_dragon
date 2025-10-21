
function Act_Mon_Shot_Sixshot_OnCheckCanEnter(actor,action)
	return		true
end

function Act_Mon_Shot_Sixshot_SetState(actor,action,kState)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
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

function Act_Mon_Shot_Sixshot_OnCastingCompleted(actor, action)

	ODS("Act_Mon_Shot_Sixshot_OnCastingCompleted\n")
	
	Act_Mon_Shot_Sixshot_SetState(actor,action,"FIRE")
	Act_Mon_Shot_Sixshot_Fire(actor,action)

end

-- function	Act_Mon_Shot_Sixshot_LoadToWeapon(actor,action,iProjectileIndex)

	-- if( CheckNil(nil==actor) ) then return end
	-- if( CheckNil(actor:IsNil()) ) then return end
	
	-- if( CheckNil(nil==action) ) then return end
	-- if( CheckNil(action:IsNil()) ) then return end
	
	-- local	kAttachID=""
	
	-- if iProjectileIndex>0 then
		-- kAttachID = ""..iProjectileIndex;
	-- end
	
	-- local	kProjectileMan = GetProjectileMan()
	-- local	kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID"..kAttachID)
	-- local	fProjetileScale = tonumber(actor:GetAnimationInfo("PROJECTILE_SCALE"..kAttachID))
	-- if fProjetileScale == nil then
		-- fProjetileScale = 1.0
	-- end
	
	-- if iProjectileIndex>0 and kProjectileID==nil then
		-- return	nil
	-- end
	
	-- if kProjectileID == nil then
		-- kProjectileID = "Projectile_arrow_02_arrow_head"
	-- end
	
	-- local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid())
	-- if kNewArrow:IsNil() then
		-- return nil
	-- end
	
	-- local	kTargetNodeID = actor:GetAnimationInfo("FIRE_START_NODE"..kAttachID)
	-- if kTargetNodeID == nil then
		-- kTargetNodeID = "p_ef_heart"
	-- end
	
	-- local	fProjectileSpeed = actor:GetAnimationInfo("PROJECTILE_SPEED"..kAttachID)
	-- if fProjectileSpeed ~= nil then
		-- fProjectileSpeed = tonumber(fProjectileSpeed)
		-- kNewArrow:SetSpeed(fProjectileSpeed)
	-- end
		
	-- kNewArrow:SetScale(fProjetileScale*kNewArrow:GetScale())	
	
	-- kNewArrow:LoadToHelper(actor,kTargetNodeID)	--	장전
	-- return kNewArrow

-- end

function Act_Mon_Shot_Sixshot_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	--	is this boomerang type?
	local	bIsBoomerang = (actor:GetAnimationInfo("PROJECTILE_TYPE") == "BOOMERANG")
	if bIsBoomerang then
		action:SetParamInt(3,1)	--	this is boomerang type
	end
end

function Act_Mon_Shot_Sixshot_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Act_Mon_Melee_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n" )
	for i=0,9 do
		action:SetParamInt(10+i,-1)	--UID 저장
		action:SetParamInt(30+i,0)	--발사 여부 저장
	end
	action:SetParamInt(20,0)	--액션ID저장
	action:SetParamInt(21,0)	--프로젝틸 로드 여부
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
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
		Act_Mon_Shot_Sixshot_OnCastingCompleted(actor,action)
	else
	
	    ODS("Act_Mon_Shot_Sixshot_OnEnter Start Casting\n")

		if( Act_Mon_Shot_Sixshot_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Shot_Sixshot_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
				
		-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
		local	kTargetList = action:GetTargetList()
		local	iTargetCount = kTargetList:size()
	
		ODS("Act_Mon_Shot_Around Casting Start iTargetCount :"..iTargetCount.."\n")
		
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

function Act_Mon_Shot_Sixshot_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID()
	local	bIsBoomerang = (actor:GetAnimationInfo("PROJECTILE_TYPE") == "BOOMERANG")
	local	iState = action:GetParamInt(0)
			local kParam1 = action:GetParam(1)	
			
	
	if action:GetActionParam() == AP_FIRE then
		if bIsBoomerang or action:GetParamInt(3) == 1 then

			if kParam1 == "end" then
				return	false
			elseif kParam1 == "wait_animdone" then
				if animDone == true then
					action:SetParam(1, "end")
					return false
				end
			elseif kParam1 == "none" then
				if animDone and actor:GetAnimationLength(action:GetSlotAnimName(5)) ~= 0 then	--루프애니
					action:SetSlot(5)
					actor:ResetAnimation()
					actor:PlayCurrentSlot()
					action:SetParam(1, "")
				end
			end	
			return	true
		end
		
		if animDone == true then
			action:SetParam(1, "end")
			return false
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_Shot_Sixshot_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
			
		elseif iState == 1 and 0 == action:GetParamInt(21) then
			
			action:SetParamInt(21,1)
			local load = actor:GetAnimationInfo("PROJECTILE_LOAD")
			if nil~=load then
				local	iProjectileIndex = 0	
				while iProjectileIndex<8 do	
					local	kArrow = Act_Mon_Shot_Sixshot_LoadToWeapon(actor,action,iProjectileIndex)
					if nil ~= kArrow and false == kArrow:IsNil() then
						action:SetParamInt(10+iProjectileIndex, kArrow:GetUID())
					end
	
					iProjectileIndex = iProjectileIndex + 1
				end
				action:SetParamInt(20, action:GetActionInstanceID())
			end

			local eff = actor:GetAnimationInfo("FIRE_EFFECT") -- 캐스팅 이팩트 붙이기
			if nil~=eff then
				local node = actor:GetAnimationInfo("FIRE_EFFECT_NODE")
				if nil==node then
					node = "char_root"
				end
				actor:AttachParticle(230, node, eff)
			end
		end
			
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Shot_Sixshot_OnCastingCompleted(actor,action)
			
			end
		end
		
	
	end
	return true
end

function Act_Mon_Shot_Sixshot_OnCleanUp(actor)
	return true
end

function Act_Mon_Shot_Sixshot_OnLeave(actor, action)
	ODS("Act_Mon_Shot_Sixshot_OnLeave\n")
	return true
end
function Act_Mon_Shot_Sixshot_SwordReturn(actor,iActionInstanceID)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	local action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	if action:GetActionInstanceID() == iActionInstanceID then
	
		if action:GetParamInt(4) == 1 then
			actor:HideParts(EQUIP_POS_WEAPON, false)	--	칼 숨김 해제
			action:SetParamInt(4,0)
		end
	
		if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then	--리턴이 없으면 
			action:SetParam(1,"end")										--그냥 끝내고
		else
			Act_Mon_Shot_Sixshot_SetState(actor,action,"RETURN")				--있으면
			action:SetParam(1,"wait_animdone")								--리턴애니 끝날때까지 기다림
		end
	
	end

end

function	Act_Mon_Shot_Sixshot_Projectile_Default_LoadToWeapon(actor,action)

	if( CheckNil(nil==actor) ) then return nil end
	if( CheckNil(actor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==action) ) then return nil end
	if( CheckNil(action:IsNil()) ) then return nil end
	
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

function Act_Mon_Shot_Sixshot_OnEvent(actor,textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	ODS("Act_Mon_Shot_Sixshot_OnEvent textKey:"..textKey.."\n")

	local kAction = actor:GetAction()
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if kAction:GetActionParam() == AP_CASTING then
		return true
	end

	
	if textKey == "hit" or textKey == "fire" or textKey == "shot" then

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
		
		local	kProjectileMan = GetProjectileMan()

		local	i = 0;
		local fRotateAngle = math.pi/6.0
		local	kRotateAxis = Point3(0,0,1);
		
		
		local UID = kAction:GetParamInt(10+i)

		local	kArrow = nil

		while i<6 do
			--kArrow = Act_Mon_Shot_Sixshot_Projectile_Default_LoadToWeapon(actor, kAction)
			local iProjectaleIndex = Random()%3
			kArrow = Act_Mon_Shot_Around_LoadToWeapon(actor, kAction, iProjectaleIndex)
			if nil~=kArrow and false==kArrow:IsNil() then
				local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
		
				local	kFireTarget = actor:GetLookingDir()
				kFireTarget:Multiply(iAttackRange);
				local k = i
				local pl = 0
				if k > 2 then
					k = k - 3
					pl = math.pi
				end
				
				kFireTarget:Rotate(kRotateAxis,fRotateAngle*(k-1)+pl);
				kFireTarget:Add(arrow_worldpos);			
				kArrow:SetTargetLoc(kFireTarget);	

				local kNum = kAction:GetParamInt(40)
				if 0~= kNum then
					local kRan = math.random(1,100)%kNum
					local kGuidStr = kAction:GetParam(10+kRan)
					local kGuid = GUID()
					kGuid:Set(kGuidStr)
					local target = g_pilotMan:FindPilot(kGuid)
					if nil~=target and false == target:IsNil() then
						kArrow:SetTargetGuidFromServer(kGuid)
					end
				end
				
				kArrow:Fire();--	발사!
				ODS("Act_Mon_Shot_Sixshot_OnEvent "..i.. " angle : "..fRotateAngle.." Target X : "..kFireTarget:GetX().." Y : "..kFireTarget:GetY().." Z : "..kFireTarget:GetZ().."\n", false, 987)
			end

			i=i+1;
		end		

		kAction:ClearTargetList()
					
	end

	return	true
end
