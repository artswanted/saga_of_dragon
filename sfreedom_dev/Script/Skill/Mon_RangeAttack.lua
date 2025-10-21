-- Melee
function Act_Mon_RangeAttack_OnCheckCanEnter(actor,action)
	return		true
end

function Act_Mon_RangeAttack_SetState(actor,action,kState)

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
		if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then
			return false
		end		
		action:SetSlot(4)
		iNewState = 3

	end

	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	action:SetParamInt(0,iNewState)
	return	true
end

function Act_Mon_RangeAttack_OnCastingCompleted(actor, action)

	ODS("Act_Mon_RangeAttack_OnCastingCompleted\n")
	
	Act_Mon_RangeAttack_SetState(actor,action,"FIRE")
	Act_Mon_RangeAttack_Fire(actor,action)

end

function	Act_Mon_RangeAttack_LoadToWeapon(actor,action,iProjectileIndex)

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
	local	fProjetileScale = tonumber(actor:GetAnimationInfo("PROJECTILE_SCALE"..kAttachID))
	if fProjetileScale == nil then
		fProjetileScale = 1.0
	end
	
	if iProjectileIndex>0 and kProjectileID==nil then
		return	nil
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

	local	fZ = actor:GetAnimationInfo("Z_FACTOR")
	if nil~=fZ and ""~=fZ then
		kNewArrow:SetParamValue("Z_FACTOR", fZ)
	end

	local	kSound = actor:GetAnimationInfo("ARRIVED_SOUND")
	if nil~=kSound and ""~=kSound then
		kNewArrow:SetParamValue("ARRIVED_SOUND", kSound)
	end

	local	kEffect = actor:GetAnimationInfo("ARRIVED_EFFECT")
	if nil~=kEffect and ""~=kEffect then
		kNewArrow:SetParamValue("ARRIVED_EFFECT", kEffect)
	end

	local	kScale = actor:GetAnimationInfo("ARRIVED_EFFECT_SCALE")
	if nil~=kScale and ""~=kScale then
		kNewArrow:SetParamValue("ARRIVED_EFFECT_SCALE", kScale)
	end

	local	kFactor = actor:GetAnimationInfo("PROJECTILE_QUAKE_FACTOR")
	if nil~=kFactor and ""~=kFactor then
		kNewArrow:SetParamValue("QUAKE_FACTOR", kFactor)
	end
	
	local	kEndNode = actor:GetAnimationInfo("PROJECTILE_END_NODE"..kAttachID)
	if nil~=kEndNode and ""~=kEndNode then
		kNewArrow:SetTargetEndNode(kEndNode)
	end

	kNewArrow:LoadToHelper(actor,kTargetNodeID)	--	장전
	return kNewArrow

end

function Act_Mon_RangeAttack_Fire(actor,action)

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

function Act_Mon_RangeAttack_OnEnter(actor, action)

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
	action:SetParamInt(3,0)
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end

	
	action:SetParam(9,"none")
	local	kPacket = action:GetParamAsPacket()
	if kPacket ~= nil and false == kPacket:IsNil() then

		local byPacketType = kPacket:PopByte()
		local iCount = kPacket:PopInt()
		
		if 2==byPacketType then				---SSPT_SKILL_PACKET_GUID

			local kGuid = kPacket:PopGuid()
			if nil~=kGuid and false==kGuid:IsNil() then
				action:SetParam(9,kGuid:GetString())
			end

		elseif 1==byPacketType then			---SSPT_SKILL_PACKET_POSITION

			local kTargetPos = kPacket:PopPoint3()
			if nil~=kTargetPos and false==kTargetPos:IsZero() then
				action:SetParamAsPoint(9,kTargetPos)
			end

		end

	end
	

	if action:GetActionParam() == AP_FIRE then
		Act_Mon_RangeAttack_OnCastingCompleted(actor,action)
	else
	
	    ODS("Act_Mon_RangeAttack_OnEnter Start Casting\n")

		if( Act_Mon_RangeAttack_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_RangeAttack_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
				
		-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
		local	kTargetList = action:GetTargetList()
		local	iTargetCount = kTargetList:size()
	
		ODS("Act_Mon_RangeAttack Casting Start iTargetCount :"..iTargetCount.."\n")
		
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

function Act_Mon_RangeAttack_OnUpdate(actor, accumTime, frameTime)

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
	local	kParam1 = action:GetParam(1)	
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
			if 2==iState then
				if false==Act_Mon_RangeAttack_SetState(actor,action,"RETURN") then
					return false
				end
			else
				action:SetParam(1, "end")
				return false
			end
		end
		
	elseif action:GetActionParam() == AP_CASTING then
	
		if iState == 0 then
			if animDone then
				Act_Mon_RangeAttack_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		elseif iState == 1 and 0 == action:GetParamInt(21) then
			action:SetParamInt(21,1)
			local load = actor:GetAnimationInfo("PROJECTILE_LOAD")
			if nil~=load then
				local	iProjectileIndex = 0	
				while iProjectileIndex<5 do	
					local	kArrow = Act_Mon_RangeAttack_LoadToWeapon(actor,action,iProjectileIndex)
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
				Act_Mon_RangeAttack_OnCastingCompleted(actor,action)
			
			end
		end
		
	
	end
	return true
end

function Act_Mon_RangeAttack_OnCleanUp(actor)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if actor:GetAction():GetParamInt(4) == 1 then
		actor:HideParts(EQUIP_POS_WEAPON, false)	--	칼 숨김 해제
		actor:GetAction():SetParamInt(4,0)
	end	

	local action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	kProjectileMan = GetProjectileMan()
	if nil~=action and false==action:IsNil() then
		local actionid = action:GetParamInt(20)
		for i=0,9 do 
			local uid = action:GetParamInt(10+i)
			if 0 <= uid and 0==action:GetParamInt(30+i) then	--UID가 있고 발사 안됬으면
				kProjectileMan:DeleteProjectileByUID(actor:GetPilotGuid(), actionid, uid) 			
			end
		end
	end

	return true
end

function Act_Mon_RangeAttack_OnLeave(actor, action)
	ODS("Act_Mon_RangeAttack_OnLeave\n")
	return true
end
function Act_Mon_RangeAttack_SwordReturn(actor,iActionInstanceID)
	
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
			Act_Mon_RangeAttack_SetState(actor,action,"RETURN")				--있으면
			action:SetParam(1,"wait_animdone")								--리턴애니 끝날때까지 기다림
		end
	
	end

end
function Act_Mon_RangeAttack_OnEvent(actor,textKey)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local kAction = actor:GetAction()
	
	if( CheckNil(nil==kAction) ) then return end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	ODS("Act_Mon_RangeAttack_OnEvent AniName: " .. kAction:GetCurrentSlotAnimName() .. ", textKey:"..textKey.."\n")	

	if kAction:GetActionParam() == AP_CASTING then
		return true
	end

	if textKey == "hit" or textKey == "fire" or textKey == "shot" then
	
		actor:DetachFrom(230)	--캐스팅 이펙트 제거
		local	bIsBoomerang = (actor:GetAnimationInfo("PROJECTILE_TYPE") == "BOOMERANG")
		local	kDelay = tonumber(actor:GetAnimationInfo("PROJECTILE_DELAY"))
		if nil==kDelay then
			kDelay = 0
		end

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

		local kUseAtDistance = actor:GetAnimationInfo("USE_AT_DISTANCE")
		if nil~=kUseAtDistance and "1"==kUseAtDistance then
			local iDistance = actor:GetAbil(AT_DISTANCE)
			if 0 ~= iDistance then
				iAttackRange = iDistance
			end
		end
		
		local	kProjectileMan = GetProjectileMan()
		
		local	iProjectileIndex = 0
		local	iPentrationCount = kAction:GetAbil(AT_PENETRATION_COUNT);
		
		while iProjectileIndex<5 do
		
			ODS("iProjectileIndex : "..iProjectileIndex.."\n");
			
			local	kArrow = nil
			local UID = kAction:GetParamInt(10+iProjectileIndex)
			if 0<=UID then
				kArrow = kProjectileMan:GetProjectile(actor:GetPilotGuid(), kAction:GetParamInt(20), UID)
			else
				kArrow = Act_Mon_RangeAttack_LoadToWeapon(actor,kAction,iProjectileIndex)
			end
			if kArrow == nil or kArrow:IsNil() then
				break
			end
			
			if bIsBoomerang then
				kArrow:SetMultipleAttack(true)
			end

			-- 관통
			kArrow:SetPenetrationCount(iPentrationCount);-- 몇번을 관통할것인지 세팅
			if iPentrationCount ~= 0 then
				kArrow:SetMultipleAttack(true);
			end
			
			local	targetobject = nil

			if kArrow:IsNil() == false then
			
				local	kMovingDir = actor:GetLookingDir()	-- 액터가 바라보고 있는 방향
				WriteToConsole("________MovingDir : " .. kMovingDir:GetX() .. ", " .. kMovingDir:GetY() .. ", " .. kMovingDir:GetZ() .. "\n")
				local	arrow_worldpos = kArrow:GetWorldPos()	--	화살의 위치
				
				local	iTargetCount = kAction:GetTargetCount()
				local	iTargetABVShapeIndex = 0
				local	kActionTargetInfo = nil
				if iTargetCount>0 then
						--	타겟을 찾아 바라본다
					local	kTargetGUID = kAction:GetTargetGUID(0)
					iTargetABVShapeIndex = kAction:GetTargetABVShapeIndex(0)
					kActionTargetInfo = kAction:GetTargetInfo(0)
					targetobject = g_pilotMan:FindPilot(kTargetGUID)
					if targetobject:IsNil() == false then
						local pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex)
						actor:LookAt(pt,true,true)
					end						
				end
				
				kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp())
				local	kSkillDef	= GetSkillDef(kArrow:GetParentActionNo())
				local at = false
				if false==kSkillDef:IsNil() then
					at = kSkillDef:IsSkillAtt(SAT_CLIENT_CTRL_PROJECTILE)
				end
				
				if targetobject == nil or targetobject:IsNil()==true or kActionTargetInfo==nil or kActionTargetInfo:IsNil() == true or true == at then
					--pkSkillDef->IsSkillAtt(SAT_CLIENT_CTRL_PROJECTILE)가 true면
					
					local kEndNode = kArrow:GetTargetEndNode()
					if nil~=kEndNode and ""~=kEndNode then
						kMovingDir = actor:GetNodeWorldPos(kEndNode)
					else
						kMovingDir:Multiply(iAttackRange)
						kMovingDir:Add(arrow_worldpos)
					end

					kArrow:SetTargetLoc(kMovingDir)
				else
				
					ODS("kArrow:SetTargetObjectList(kAction:GetTargetList())	\n")
				
					kArrow:SetTargetObjectList(kAction:GetTargetList())	
				end
				
				local kGuidStr = kAction:GetParam(9)
				if nil~=kGuidStr and kGuidStr~="none" then
					local kGuid = GUID()
					kGuid:Set(kGuidStr)
					local target = g_pilotMan:FindPilot(kGuid)
					if nil~=target and false == target:IsNil() then
						kArrow:SetTargetGuidFromServer(kGuid)
					end
				end
				
				local kTargetPos = kAction:GetParamAsPoint(9)
				if nil~=kTargetPos and false==kTargetPos:IsZero() then
					kArrow:SetTargetLoc(kTargetPos)
				end

				local	kEndnode = actor:GetAnimationInfo("USE_END_NODE")	--발사방향 노드를 사용할까나?
				if kEndnode ~= nil then
					local	kTargetNodeID = actor:GetAnimationInfo("FIRE_START_NODE")
					if kTargetNodeID == nil then
						kTargetNodeID = "p_ef_heart"
					end
					local weaponrot = actor:GetNodeRotateAxis(kTargetNodeID,2, true)
					if false==weaponrot:IsZero() then
						weaponrot:Multiply(-1)
						local weaponpos = actor:GetNodeWorldPos(kTargetNodeID)
						local weaponendpos = actor:GetNodeWorldPos(kEndnode)
						local dir = Point3(weaponendpos:GetX() - weaponpos:GetX(), weaponendpos:GetY() - weaponpos:GetY(), weaponendpos:GetZ() - weaponpos:GetZ())
						dir:Unitize()
						if( CheckNil(g_world == nil) ) then return false end
						if( CheckNil(g_world:IsNil()) ) then return false end
						local TargetPos = g_world:ThrowRay(weaponpos,dir,500)	-- 발사 방향으로 레이
						if false==TargetPos:IsZero() then
							kArrow:SetTargetLoc(TargetPos)
						end
					end
				end
				
				-- local fZ = actor:GetAnimationInfo("Z_FACTOR")
				--if nil~=fZ or ""~=fZ then
					--fZ = tonumber(fZ)
					-- fZ = 10
					-- kArrow:SetMovingType(1)
					-- kArrow:SetParam_SinCurveLineType(fZ, 0)
				--end

				kArrow:DelayFire(kDelay*iProjectileIndex)
				kAction:SetParamInt(30+iProjectileIndex, 1)
				
			else
				
				ODS("Arrow is nil\n")
			
			end
			
			iProjectileIndex=iProjectileIndex+1
		
		end
		
		kAction:ClearTargetList()
					
	end

	return	true
end

function Act_Mon_RangeAttack_OnTargetListModified(actor,action,bIsBefore)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
end