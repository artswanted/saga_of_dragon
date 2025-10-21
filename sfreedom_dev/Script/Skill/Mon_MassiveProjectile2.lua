-- Melee
function Act_Mon_MassiveProjectile2_OnCheckCanEnter(actor,action)

	return		true;
	
end
function Act_Mon_MassiveProjectile2_OnCastingCompleted(actor, action)

	Act_Mon_Melee_SetState(actor,action,"FIRE")
	Act_Mon_MassiveProjectile2_Fire(actor,action);
	
end

function	Act_Mon_MassiveProjectile2_LoadToWeapon(actor,action,kStartPos)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local	kAttachID="";
	
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID")
	local	fProjectileScale = tonumber(actor:GetAnimationInfo("PROJECTILE_SCALE"));
	
	if kProjectileID == nil then
		kProjectileID = "Projectile_arrow_02_arrow_head"
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	if false == kNewArrow:IsNil() then
		
		if nil~=fProjectileScale and ""~=fProjectileScale then
			local scale = actor:GetAnimationInfo("RANDOM_SCALE")
			if nil == scale then
				scale = fProjectileScale
			else
				scale = fProjectileScale-scale + Random()%scale*2 -- scale값이 랜덤 범위
			end
			kNewArrow:SetScale(scale)
		end

		local rotate = actor:GetAnimationInfo("RANDOM_ROTATE")
		if nil ~= rotate then
			local w = (Random()%(math.pi*0.5))
			local x = (Random()%20-10)*0.1
			local y = (Random()%20-10)*0.1
			local z = (Random()%20-10)*0.1
			local point = Point3(x,y,z)
			point:Unitize()
			kNewArrow:SetRotate(Quaternion(w,point))
		end
	end

	local	fProjectileSpeed = tonumber(actor:GetAnimationInfo("PROJECTILE_SPEED"));
	if fProjectileSpeed ~= nil then
		kNewArrow:SetSpeed(fProjectileSpeed)
	end
	
	kNewArrow:LoadToPosition(kStartPos);	--	장전
	return kNewArrow;

end

function Act_Mon_MassiveProjectile2_Fire(actor,action)

end

function Act_Mon_MassiveProjectile2_OnEnter(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end

	action:SetParamInt(4,100)
	action:SetParamInt(5,0)
	action:SetParamInt(11,0)	--패킷에 실려온 총 타겟 갯수

	if action:GetActionParam() == AP_FIRE then
		Act_Mon_MassiveProjectile2_OnCastingCompleted(actor,action)
	else
	
		if( Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
				
		local	ptTargetPos = actor:GetPos()
		
		if IsSingleMode() then
			ptTargetPos = GetMyActor():GetPos();
		else
			local	kPacket = action:GetParamAsPacket();
			if kPacket == nil or kPacket:IsNil() then
				ptTargetPos = actor:GetPos()
			else
				local iCount = kPacket:PopInt()
				action:SetParamInt(11,iCount)
				for i = 1, iCount do
					ptTargetPos = Point3(kPacket:PopFloat(),kPacket:PopFloat(),kPacket:PopFloat())
					action:SetParamAsPoint(i,ptTargetPos)					
				end
				ptTargetPos = Point3(kPacket:PopFloat(),kPacket:PopFloat(),kPacket:PopFloat());
			end
		end
		
		-- Look at targetpos
		actor:LookAt(ptTargetPos,true,false);
		
		-- Save the targetpos
		if 0==action:GetParamInt(11) then
			action:SetParamAsPoint(0,ptTargetPos)
		end
	end
	
	return true
end

function Act_Mon_MassiveProjectile2_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
	local	iState = action:GetParamInt(0)

	local isRange = action:GetParamInt(5)
	if 0==isRange then	--범위이펙트를 표시한 적이 없으면
		action:SetParamInt(5,1)
		local RangeEffect = actor:GetAnimationInfo("RANGE_EFFECT")
		if nil==RangeEffect or ""==RangeEffect then
			RangeEffect = "ef_Shark_king_abdula_skill_03_03_char_root"
		end
		local kSkillDef = GetSkillDef(action:GetActionNo())
		local	fSkillRange = 100
		if nil~=kSkillDef and false==kSkillDef:IsNil() then
			fSkillRange = kSkillDef:GetAbil(AT_1ST_AREA_PARAM2)
			if 0==fSkillRange then
				fSkillRange = 100
			end
		end
		action:SetParamInt(4,fSkillRange)
		if actor:IsEqualObjectName("m_cleef_VanHelsing") == false then
			local iTargetCount = action:GetParamInt(11)
			if 0==iTargetCount then
				actor:AttachParticleToPointS(81190,action:GetParamAsPoint(0),RangeEffect, fSkillRange*0.009)
			else
				for i=1, iTargetCount do
					actor:AttachParticleToPointS(81190+i,action:GetParamAsPoint(i),RangeEffect, fSkillRange*0.009)
				end
			end
		end
	end
	
	if action:GetActionParam() == AP_FIRE then

		if animDone == true then
			if 2==iState then	--Fire가 끝났으면 Return해줘야지
				if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then --없으면 끝
					action:SetParam(1, "end")
					return false
				else --있으면
					Act_Mon_Melee_SetState(actor,action,"RETURN")
				end
			elseif 3==iState then	--리턴이 끝났으면
				action:SetParam(1, "end")	--진짜 끝
				return false
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
				Act_Mon_MassiveProjectile2_OnCastingCompleted(actor,action)
			
			end
		end
		
	
	end
	return true
end

function Act_Mon_MassiveProjectile2_OnCleanUp(actor)
	return true;
end

function Act_Mon_MassiveProjectile2_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==actor:GetAction());
	CheckNil(actor:GetAction():IsNil());
	actor:DetachFrom(81190)
	local iTargetCount = actor:GetAction():GetParamInt(11)
	for i=1,iTargetCount do
		actor:DetachFrom(81190+i)
	end
	return true;
end

function Act_Mon_MassiveProjectile2_FireArrow(actor,action,fFireDelay,iParam)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if nil==iParam then
		iParam = 0
	end
	
	local	fRange =action:GetParamInt(4);
	local	kTargetCenterPos = action:GetParamAsPoint(iParam);	
	
	local	kStartPos = Point3(kTargetCenterPos:GetX()+math.random(-fRange/2,fRange/2),
								kTargetCenterPos:GetY()+math.random(-fRange/2,fRange/2),
								kTargetCenterPos:GetZ()+300);
	
	local	kFireDir = Point3(0,0,-1)
	local	kFireTargetPos = g_world:ThrowRay(kStartPos,kFireDir,1000)
	if kFireTargetPos:GetX() == -1 then
		return true
	end
	kFireTargetPos:SetX(kStartPos:GetX())
	kFireTargetPos:SetY(kStartPos:GetY())

	local	iSkillRange = action:GetSkillRange(0,actor)
	
	kStartPos:SetZ(kFireTargetPos:GetZ()+300)
	
	local	kProjectile = Act_Mon_MassiveProjectile2_LoadToWeapon(actor,action,kStartPos);
	
	kProjectile:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());
	kProjectile:SetTargetLoc(kFireTargetPos);
	kProjectile:DelayFire(fFireDelay,true);
	
	local	iFireCount = action:GetParamInt(3);
	action:SetParamInt(3,iFireCount+1);

	return	true;
end

function Act_Mon_MassiveProjectile2_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	
	if textKey == "hit" or textKey == "fire" or textKey == "shot" then

		local	iTotalProjectile = 20;
		
		local count = actor:GetAnimationInfo("PROJECTILE_COUNT")
		if nil ~= count then
			iTotalProjectile = tonumber(count)
		end

		local	fFireTerm = 0.1
		local term = actor:GetAnimationInfo("FIRE_TERM")
		if nil ~= term then
			fFireTerm = tonumber(term)
		end

		local iTargetCount = kAction:GetParamInt(11)
		for ii = 1,iTargetCount do
			local	i=0
			while i<iTotalProjectile do
				Act_Mon_MassiveProjectile2_FireArrow(actor,kAction,fFireTerm*i,ii)
				i=i+1;
			end
		end
		
		if kAction:GetTargetList():IsNil() == false then
			kAction:GetTargetList():ApplyActionEffects();
		end
		kAction:ClearTargetList()

		local QuakeTime = actor:GetAnimationInfo("QUAKE_TIME")
		if nil ~= QuakeTime and "" ~= QuakeTime then
			local QuakeFactor = actor:GetAnimationInfo("QUAKE_FACTOR")	
			if  nil == QuakeFactor then
				QuakeFactor = 1.0
			else
				QuakeFactor = tonumber(QuakeFactor)
			end
			QuakeCamera(QuakeTime, QuakeFactor)
		end

	elseif textKey == "cam" then
		local QuakeTime = actor:GetAnimationInfo("QUAKE_TIME")
		if nil ~= QuakeTime and "" ~= QuakeTime then
			local QuakeFactor = actor:GetAnimationInfo("QUAKE_FACTOR")	
			if  nil == QuakeFactor then
				QuakeFactor = 1.0
			else
				QuakeFactor = tonumber(QuakeFactor)
			end
			QuakeCamera(QuakeTime, QuakeFactor)
		end
					
	end

	return	true;
end
