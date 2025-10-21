-- Melee
function Act_Mon_Antares_Laser_OnCheckCanEnter(actor,action)

	return		true;
	
end
function Act_Mon_Antares_Laser_OnCastingCompleted(actor, action)

	ODS("Act_Mon_Antares_Laser_OnCastingCompleted\n");
	
	Act_Mon_Melee_SetState(actor,action,"FIRE")
	Act_Mon_Antares_Laser_Fire(actor,action);
	

end

function	Act_Mon_Antares_Laser_LoadToWeapon(actor,action,iProjectileIndex)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kAttachID="";
	
	if iProjectileIndex>0 then
		kAttachID = ""..(iProjectileIndex);
	end

	local	kTargetNodeID = actor:GetAnimationInfo("FIRE_START_NODE"..kAttachID);
	if kTargetNodeID == nil then
		if 0==iProjectileIndex then
			kTargetNodeID = actor:GetAnimationInfo("FIRE_START_NODE");
		else
			return nil
		end
	end
	if kTargetNodeID == nil then
		kTargetNodeID = "p_ef_weapon"
	end
	
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID"..kAttachID)
	
	if kProjectileID == nil then
		kProjectileID = "PROJECTILE_ANTARES_LASER"
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	
	kNewArrow:LoadToHelper(actor,kTargetNodeID);	--	장전
	return kNewArrow;

end

function Act_Mon_Antares_Laser_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--	is this boomerang type?
	local	bIsBoomerang = (actor:GetAnimationInfo("PROJECTILE_TYPE") == "BOOMERANG");
	if bIsBoomerang then
		action:SetParamInt(3,1)	--	this is boomerang type
	end
end

function Act_Mon_Antares_Laser_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("Act_Mon_Melee_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");
		
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end

	if action:GetActionParam() == AP_FIRE then
		Act_Mon_Antares_Laser_OnCastingCompleted(actor,action)
	else
	
	    ODS("Act_Mon_Antares_Laser_OnEnter Start Casting\n");

		if( Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
				
		-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
		local	kTargetList = action:GetTargetList();
		local	iTargetCount = kTargetList:size();
	
		ODS("Act_Mon_RangeAttack Casting Start iTargetCount :"..iTargetCount.."\n");

		
		if iTargetCount>0 then
			local	kTargetInfo = kTargetList:GetTargetInfo(0);
			if kTargetInfo:IsNil() == false then
			
				local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());
				if kTargetPilot:IsNil() == false then
				
					local	kTargetActor = kTargetPilot:GetActor();
					if kTargetActor:IsNil() == false then
					
						--actor:LookAtBidirection(kTargetActor:GetPos());
						actor:LookAt(kTargetActor:GetPos(),true,true);
					
					end
				end
			end
		end
	end

	action:SetParamFloat(4, 0)	--시간 저장
	action:SetParamInt(5, 0)	--프로젝틸 인덱스 저장
	action:SetParam(6, "end")	--발사, 중단 상태 저장
	
	return true
end

function Act_Mon_Antares_Laser_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
	local actionID = action:GetID();
	local	bIsBoomerang = (actor:GetAnimationInfo("PROJECTILE_TYPE") == "BOOMERANG");
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then

		if bIsBoomerang then
			if action:GetParam(1) == "end" then
				return	false;
			elseif action:GetParam(1) == "wait_animdone" then
				if animDone == true then
					action:SetParam(1, "end")
					actor:DetachFrom(231)
					return false
				end
			end	
			return	true;
		end

		if animDone == true then
			if iState==2 then
				if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then
					action:SetParam(1, "end")
					actor:DetachFrom(231)
					return false
				else
					Act_Mon_Melee_SetState(actor,action,"RETURN")
					action:SetParam(1,"wait_animdone")	
				end
			else
				action:SetParam(1, "end")
				actor:DetachFrom(231)
				return false
			end
		end
		local pasttime = action:GetParamFloat(4)
		local delta = accumTime - action:GetParamFloat(4)

		if "fire"==action:GetParam(6) or "hit"==action:GetParam(6) then

			local inter = actor:GetAnimationInfo("SHOT_INTER_TIME")
			if inter == nil then
				inter = 0.3
			else
				inter = tonumber(inter)
			end

			if inter < delta then

				action:SetParamFloat(4, accumTime)

				for iLoop=0,3 do --일단 4번만 돌아보자

					local	kArrow = Act_Mon_Antares_Laser_LoadToWeapon(actor,action,iLoop);
					if kArrow == nil or kArrow:IsNil() then
						return true
					end

					if bIsBoomerang then
						kArrow:SetMultipleAttack(true);
					end
			
					if kArrow:IsNil() == false then
						kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp())
					end

					local	kAttachID="";
	
					if iLoop>0 then
						kAttachID = ""..iLoop;
					end
			
					local fZ = actor:GetAnimationInfo("Z_FACTOR")
					if nil ~= fZ then
						fZ = tonumber(fZ)
						kArrow:SetMovingType(1)
						kArrow:SetParam_SinCurveLineType(fZ, 0)
					end
					
					local	kTargetNodeID = actor:GetAnimationInfo("FIRE_START_NODE"..kAttachID);
					if nil == kTargetNodeID then
						kTargetNodeID = "p_ef_weapon"
					end
					
					local	kScale = actor:GetAnimationInfo("PROJECTILE_SCALE"..kAttachID);
					if nil==kScale or ""==kScale then
						kScale = 1.0
					else
						kScale = tonumber(kScale)
					end

					local weaponrot = actor:GetNodeRotateAxis(kTargetNodeID,2, true)
					weaponrot:Multiply(-1)
					local weaponpos = actor:GetNodeWorldPos(kTargetNodeID)
					local kEndnode = kTargetNodeID.."_end"
					local weaponendpos = actor:GetNodeWorldPos(kEndnode)
					local dir = Point3(weaponendpos:GetX() - weaponpos:GetX(), weaponendpos:GetY() - weaponpos:GetY(), weaponendpos:GetZ() - weaponpos:GetZ())
			
					dir:Unitize()
					local TargetPos = nil
					local kEndFront = actor:GetAnimationInfo("FIRE_END_NODE_FRONT"..kAttachID);
					if nil~=kEndFront then
						kEndFront = tonumber(kEndFront)
						if kEndFront>0 then
							local	kFireTarget = dir
							kFireTarget:SetZ(0)
							kFireTarget:Multiply(kEndFront)
							kFireTarget:Add(weaponpos)
							TargetPos = kFireTarget
						end
					end

					if nil==TargetPos then
						if( CheckNil(g_world == nil) ) then return false end
						if( CheckNil(g_world:IsNil()) ) then return false end
						TargetPos = g_world:ThrowRay(weaponpos,dir,500)	-- 아래로 레이
					end

					kArrow:SetScale(kScale)
					kArrow:SetTargetLoc(TargetPos)	
					kArrow:Fire(true);
					action:SetParamInt(5, action:GetParamInt(5)+1)
					if actor:IsEqualObjectName("m_cleef_gatekeeper") then
						actor:AttachParticleToPoint(Random(),TargetPos,"ef_gatekeeper_skill_02_03")
					else
						local kGroundEff = actor:GetAnimationInfo("GROUND_EFFECT");
						if nil==kGroundEff then
							actor:AttachParticleToPoint(Random(),TargetPos,"ef_antares_skill_02_02_char_root")
						elseif ""~=kGroundEff and "NONE"~=kGroundEff then
							actor:AttachParticleToPoint(Random(),TargetPos,kGroundEff)
						end
					end
				end
			end
		end

	elseif action:GetActionParam() == AP_CASTING then
	
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
			
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Antares_Laser_OnCastingCompleted(actor,action)
			
			end
		end
		
	end
	return true
end

function Act_Mon_Antares_Laser_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	if actor:GetAction():GetParamInt(4) == 1 then
		actor:HideParts(EQUIP_POS_WEAPON, false)	--	칼 숨김 해제
		actor:GetAction():SetParamInt(4,0);
	end	

	return true;
end

function Act_Mon_Antares_Laser_OnLeave(actor, action)
	ODS("Act_Mon_Antares_Laser_OnLeave\n");
	return true;
end
function Act_Mon_Antares_Laser_SwordReturn(actor,iActionInstanceID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetActionInstanceID() == iActionInstanceID then
	
		if action:GetParamInt(4) == 1 then
			actor:HideParts(EQUIP_POS_WEAPON, false)	--	칼 숨김 해제
			action:SetParamInt(4,0);
		end
	
		if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then
			action:SetParam(1,"end");
		else
		
			Act_Mon_Melee_SetState(actor,action,"RETURN")
			action:SetParam(1,"wait_animdone")
		
		end
	
	end

end
function Act_Mon_Antares_Laser_OnEvent(actor,textKey)

	ODS("Act_Mon_Antares_Laser_OnEvent textKey:"..textKey.."\n");

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local kAction = actor:GetAction();
	if kAction:GetActionParam() == AP_CASTING then
		Act_Mon_Cast_Effect(actor,textKey)
		return true;
	end

	if textKey == "hit" or textKey == "fire" or textKey == "end" then
		kAction:SetParam(6,textKey)
	end

	if textKey == "hit" or textKey == "fire" then

		local	bIsBoomerang = (actor:GetAnimationInfo("PROJECTILE_TYPE") == "BOOMERANG");

		if bIsBoomerang then
			actor:HideParts(EQUIP_POS_WEAPON, true)	--	칼 숨기자
			kAction:SetParamInt(4,1);	--	칼 숨겼음		
		end
		
		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end

		local fireEffect = actor:GetAnimationInfo("FIRE_EFFECT")	
		if fireEffect ~= nil and "NONE" ~= fireEffect then
			local fireStartNode	= actor:GetAnimationInfo("FIRE_EFFECT_NODE")	
			
			if fireStartNode == nil then
				fireStartNode = "p_ef_heart"
			end
			
			actor:AttachParticle(231, fireStartNode, fireEffect)
		end					
		
		local	iAttackRange = kAction:GetSkillRange(0,actor);
		
		if iAttackRange == 0 then
			iAttackRange = 200; 
		end
		
--		local	kProjectileMan = GetProjectileMan();
		
--		local	iProjectileIndex = 0

		local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
				
		local	iTargetCount = kAction:GetTargetCount();
		local	iTargetABVShapeIndex = 0;
		local	kActionTargetInfo = nil

		if iTargetCount>0 then
				--	타겟을 찾아 바라본다
			local	kTargetGUID = kAction:GetTargetGUID(0);
			iTargetABVShapeIndex = kAction:GetTargetABVShapeIndex(0);
			kActionTargetInfo = kAction:GetTargetInfo(0);
			targetobject = g_pilotMan:FindPilot(kTargetGUID);
			if targetobject:IsNil() == false then
				local pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
				actor:LookAt(pt,true,true);
			end						
		end
	end

	return	true;
end
