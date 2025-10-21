-- Melee
function Act_Mon_RangeAttack_OnCheckCanEnter(actor,action)
	return		true
end

function Act_Mon_Shot_Around_Finish_SetState(actor,action,kState)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iNewState = -1
	if kState == "BATTLEIDLE_START" then
		--ODS("BATTLEIDLE_START ----- Act_Mon_Melee_SetState SlotName["..action:GetSlotAnimName(0).."]\n", false, 3851)
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			return	false	
		end
		action:SetSlot(0)
	elseif kState == "FIRE" then
		--ODS("FIRE ----------- Act_Mon_Melee_SetState SlotName["..action:GetSlotAnimName(1).."]\n", false, 3851)
		action:SetSlot(1)
	elseif kState == "RETURN" then
		--ODS("RETURN ---------- AniName : " ..action:GetSlotAnimName(3).."\n", false, 3851)
		action:SetSlot(3)
	end

	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	return	true
end

function Act_Mon_Shot_Around_Finish_OnCastingCompleted(actor, action)

	ODS("Act_Mon_Shot_Around_Finish_OnCastingCompleted\n")
	
	Act_Mon_Shot_Around_Finish_SetState(actor,action,"FIRE")
	Act_Mon_Shot_Around_Finish_Fire(actor,action)

end

function	Act_Mon_Shot_Around_Finish_LoadToWeapon(actor,action,iProjectileIndex)

	if( CheckNil(nil==actor) ) then return nil end
	if( CheckNil(actor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==action) ) then return nil end
	if( CheckNil(action:IsNil()) ) then return nil end
	
	local	kAttachID=""
	
	if iProjectileIndex>0 then
		kAttachID = ""..iProjectileIndex;
	end
	
	local	kProjectileMan = GetProjectileMan()
	local	kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID"..kAttachID)
	local	kTargetNodeID = actor:GetAnimationInfo("FIRE_START_NODE"..kAttachID)
	local	fProjetileScale = tonumber(actor:GetAnimationInfo("PROJECTILE_SCALE"..kAttachID))
	
	if fProjetileScale == nil then
		fProjetileScale = 1.0
	end
	
	if kProjectileID == nil then
		kProjectileID = "Projectile_arrow_02_arrow_head"
	end
	
	if kTargetNodeID == nil then
		kTargetNodeID = "p_ef_heart"
	end
	--ODS("Name: "..actor:GetID()..", ProjectileIndex: "..iProjectileIndex..", AniName: "..action:GetCurrentSlotAnimName()..", ProjectileID: "..kProjectileID..", kTargetNodeID: "..kTargetNodeID.."\n",false,6482)
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid())
	if kNewArrow:IsNil() then
		return nil
	end
	
	local	fProjectileSpeed = actor:GetAnimationInfo("PROJECTILE_SPEED"..kAttachID)
	if fProjectileSpeed == nil then
		fProjectileSpeed = actor:GetAnimationInfo("PROJECTILE_SPEED")
	end
	if fProjectileSpeed ~= nil then
		fProjectileSpeed = tonumber(fProjectileSpeed)
		kNewArrow:SetSpeed(fProjectileSpeed)
	end
		
	kNewArrow:SetScale(fProjetileScale*kNewArrow:GetScale())	
	
	kNewArrow:LoadToHelper(actor,kTargetNodeID)	--	장전
	
	local	kTargetEffect = actor:GetAnimationInfo("HIT_TARGET_EFFECT_ID")
	if nil~=kTargetEffect and ""~=kTargetEffect then
		kNewArrow:SetParamValue("HIT_TARGET_EFFECT_ID",kTargetEffect)
	end
	local	kTargetEffectNode = actor:GetAnimationInfo("HIT_TARGET_EFFECT_TARGET_NODE")
	if nil~=kTargetEffectNode and ""~=kTargetEffectNode then
		kNewArrow:SetParamValue("HIT_TARGET_EFFECT_TARGET_NODE",kTargetEffectNode)
	end
	
	return kNewArrow

end

function Act_Mon_Shot_Around_Finish_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--	is this boomerang type?
	local	bIsBoomerang = (actor:GetAnimationInfo("PROJECTILE_TYPE") == "BOOMERANG")
	if bIsBoomerang then
		action:SetParamInt(3,1)	--	this is boomerang type
	end
end

function Act_Mon_Shot_Around_Finish_OnEnter(actor, action)

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
		local kPacketType = kPacket:PopByte()
		local kNum = kPacket:PopInt()
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
		Act_Mon_Shot_Around_Finish_OnCastingCompleted(actor,action)
	else
	    --ODS("Act_Mon_Shot_Around_Finish_OnEnter Start Casting\n", false, 3851)
				
		-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
		local	kTargetList = action:GetTargetList()
		local	iTargetCount = kTargetList:size()
	
		--ODS("Act_Mon_Shot_Around_Finish Casting Start iTargetCount :"..iTargetCount.."\n")
		
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

	local iAttackCount = action:GetAbil(AT_COUNT)
	action:SetParamInt(AT_COUNT, iAttackCount)
	
	return true
end

function Act_Mon_Shot_Around_Finish_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID()

	local animDone = actor:IsAnimationDone()
	
	if true == animDone then		
		if 3 == action:GetCurrentSlot() then				
			ODS("end Ani\n", false, 3851)
			action:SetParam(1, "end")
			return false;
		end
	end
	
	if action:GetActionParam() == AP_FIRE then
	
		if animDone == true then
			--ODS("animDone / CurrentAction = "..action:GetSlotAnimName(action:GetCurrentSlot()).."\n", false, 3851);
			local iAttackCount = action:GetParamInt(AT_COUNT)
			if 1 < iAttackCount then
				Act_Mon_Shot_Around_Finish_SetState(actor,action,"FIRE")
				--ODS("Repeat Attack !!!!!         iAttackCount = "..iAttackCount.."\n", false, 3851)
				action:SetParamInt( AT_COUNT, iAttackCount-1 )

				return true
			else
				--ODS("End Attack !!!!!         iAttackCount = "..iAttackCount.."\n", false, 3851)
				if 0 ~= action:GetAbil(AT_COUNT) then
					Act_Mon_Shot_Around_Finish_SetState(actor,action,"RETURN")
					return true
				end
			end
			
		end
		
	elseif action:GetActionParam() == AP_CASTING then
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				--ODS("Act_Mon_Shot_Around_Finish_SetState2\n", false, 3851)
				Act_Mon_Shot_Around_Finish_OnCastingCompleted(actor,action)
			
			end
		end
		
	end
	return true
end

function Act_Mon_Shot_Around_Finish_OnCleanUp(actor)
	actor:DetachFrom(231)			
	return true
end

function Act_Mon_Shot_Around_Finish_OnLeave(actor, action)
	--ODS("Act_Mon_Shot_Around_Finish_OnLeave\n")
	--ODS("OnLeave ---------- AniName : " ..action:GetSlotAnimName(action:GetCurrentSlot()).."\n", false, 3851)
	return true
end

function Act_Mon_Shot_Around_Finish_SwordReturn(actor,iActionInstanceID)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if action:GetActionInstanceID() == iActionInstanceID then
	
		if action:GetParamInt(4) == 1 then
			actor:HideParts(EQUIP_POS_WEAPON, false)	--	칼 숨김 해제
			action:SetParamInt(4,0)
		end
	
		if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then	--리턴이 없으면 
			action:SetParam(1,"end")										--그냥 끝내고
		else
			Act_Mon_Shot_Around_Finish_SetState(actor,action,"RETURN")				--있으면
			action:SetParam(1,"wait_animdone")								--리턴애니 끝날때까지 기다림
		end
	
	end

end

function	Act_Mon_Shot_Around_Finish_Projectile_Default_LoadToWeapon(actor,action)

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

function Act_Mon_Shot_Around_Finish_OnEvent(actor,textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	ODS("Act_Mon_Shot_Around_Finish_OnEvent textKey:"..textKey.."\n")

	local kAction = actor:GetAction()
	
	if( CheckNil(nil==kAction) ) then return end
	if( CheckNil(kAction:IsNil()) ) then return false end

	Act_Mon_Cast_Effect(actor,textKey)
	
	if kAction:GetActionParam() == AP_CASTING then
		return true
	end
	
	if textKey == "start" then
		local EnterEffect = actor:GetAnimationInfo("ENTER_EFFECT")	
		if EnterEffect ~= nil then
			local EnterStartNode	= actor:GetAnimationInfo("ENTER_EFFECT_NODE")	
			
			if EnterStartNode == nil then
				EnterStartNode = "p_ef_heart"
			end

			local EffectScale = actor:GetAnimationInfo("ENTER_EFFECT_SCALE")
			if nil==EffectScale or ""==EffectScale then
				EffectScale = 1
			else
				EffectScale = tonumber(EffectScale)
			end
			
			actor:AttachParticle(231, EnterStartNode, EnterEffect, EffectScale)
		end		
	elseif textKey == "end" then
		local EnterEffect = actor:GetAnimationInfo("ENTER_EFFECT")	
		if EnterEffect ~= nil then
			actor:DetachFrom(231)
		end
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
		
		local fire_delay = actor:GetAnimationInfo("FIRE_TERM")
		if nil == fire_delay or "" == fire_delay then
			fire_delay = 0 -- 기본 딜레이 없음
		else
			fire_delay = tonumber(fire_delay)
		end
		
		local Projectile_cnt = actor:GetAnimationInfo("PROJECTILE_COUNT")
		if nil == Projectile_cnt or "" == Projectile_cnt then
			Projectile_cnt = 8
		else
			Projectile_cnt = tonumber(Projectile_cnt)
		end
		
		local fire_random = actor:GetAnimationInfo("FIRE_RANDOM")
		if nil == fire_random or "" == fire_random then
			fire_random = 3
		else
			fire_random = tonumber(fire_random)
		end
		
		
		local	kProjectileMan = GetProjectileMan()

		local	i = 0;
		local fRotateAngle = math.pi / (Projectile_cnt/2)
		local	kRotateAxis = Point3(0,0,1);
		
		
		local UID = kAction:GetParamInt(10+i)

		local	kArrow = nil
		--kArrow = Act_Mon_Shot_Around_Finish_LoadToWeapon(actor,kAction, i)		
		local LocZ = actor:GetAnimationInfo("TARGET_LOC_Z")
		if nil~=LocZ and ""~=LocZ then
			LocZ = tonumber(LocZ)
		end
		while i<Projectile_cnt do
			--kArrow = Act_Mon_Shot_Around_Finish_Projectile_Default_LoadToWeapon(actor, kAction)
			local iProjectaleIndex = Random()%fire_random
			--ODS("ProjectaleIndex: "..iProjectaleIndex.."\n",false,6482)
			kArrow = Act_Mon_Shot_Around_Finish_LoadToWeapon(actor, kAction, iProjectaleIndex)
			if nil~=kArrow and false==kArrow:IsNil() then
				local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
		
				local	kFireTarget = actor:GetLookingDir()
				if nil~=LocZ then
					arrow_worldpos:SetZ(arrow_worldpos:GetZ()+LocZ)
					kFireTarget = arrow_worldpos
				else
					kFireTarget:Multiply(iAttackRange);
					kFireTarget:Rotate(kRotateAxis,fRotateAngle*i);
					kFireTarget:Add(arrow_worldpos);			
				end
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
				if 0 ~= fire_delay then
					kArrow:DelayFire(i*fire_delay)
				else
					--ODS("-----------------------Fire kArrow---------------------------\n", false, 3851)
					kArrow:Fire();--	발사!
				end
				--ODS("Act_Mon_Shot_Around_Finish_OnEvent "..i.. " angle : "..fRotateAngle.." Target X : "..kFireTarget:GetX().." Y : "..kFireTarget:GetY().." Z : "..kFireTarget:GetZ().."\n", false, 987)
			end

			i=i+1;
		end		

		kAction:ClearTargetList()
					
	end

	return	true
end
