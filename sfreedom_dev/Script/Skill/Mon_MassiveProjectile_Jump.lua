-- Melee
function Act_Mon_MassiveProjectile_Jump_OnCheckCanEnter(actor,action)

	return		true;
	
end
function Act_Mon_MassiveProjectile_Jump_OnCastingCompleted(actor, action)

	ODS("Act_Mon_MassiveProjectile_Jump_OnCastingCompleted\n");
	
	Act_Mon_Melee_SetState(actor,action,"FIRE")
	Act_Mon_MassiveProjectile_Jump_Fire(actor,action);
	

end

function	Act_Mon_MassiveProjectile_Jump_LoadToWeapon(actor,action,kStartPos)
	
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
			if nil == scale or ""==scale then
				scale = fProjectileScale
			else
				scale = fProjectileScale - scale + Random()%scale*2 -- scale값이 랜덤 범위
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
	
	local	kEffect = actor:GetAnimationInfo("ARRIVED_EFFECT")
	if nil~=kEffect and ""~=kEffect then
		kNewArrow:SetParamValue("ARRIVED_EFFECT", kEffect)
	end

	local	kScale = actor:GetAnimationInfo("ARRIVED_EFFECT_SCALE")
	if nil~=kScale and ""~=kScale then
		kNewArrow:SetParamValue("ARRIVED_EFFECT_SCALE", kScale)
	end

	if actor:IsEqualObjectName("m_MetalKamparling") == true then
		kNewArrow:SetParamValue("ARRIVED_EFFECT_Z", 0)
	end

	kNewArrow:LoadToPosition(kStartPos);	--	장전
	return kNewArrow;

end

function Act_Mon_MassiveProjectile_Jump_Fire(actor,action)

end

function Act_Mon_MassiveProjectile_Jump_OnEnter(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	ODS("Act_Mon_MassiveProjectile_Jump_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");

	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end

	action:SetParamInt(4,100)
	action:SetParamInt(5,0)

	if action:GetActionParam() == AP_FIRE then
		Act_Mon_MassiveProjectile_Jump_OnCastingCompleted(actor,action)
	else
	
	    ODS("Act_Mon_MassiveProjectile_Jump_OnEnter Start Casting\n");

		if( Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
				
		local	ptTargetPos = 0;
		
		if IsSingleMode() then
			ptTargetPos = GetMyActor():GetPos();
		else
			local	kPacket = action:GetParamAsPacket();
			if kPacket == nil or kPacket:IsNil() then
				ptTargetPos = actor:GetPos()
			else
				ptTargetPos = Point3(kPacket:PopFloat(),kPacket:PopFloat(),kPacket:PopFloat());
			end
		end
		ODS("X:"..ptTargetPos:GetX().." Y:"..ptTargetPos:GetY().." Z:"..ptTargetPos:GetZ().."\n")
		
		-- Look at targetpos
		actor:LookAt(ptTargetPos,true,false);
		
		-- Save the targetpos
		local kPos = ptTargetPos
		kPos:SetZ(kPos:GetZ() + 10)
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local ptcl = g_world:ThrowRay(kPos, Point3(0,0,-1), 300);
		if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
			ptcl = ptTargetPos
		else
			ptTargetPos:SetZ(ptcl:GetZ()+25)
		end
		
		action:SetParamAsPoint(0,ptTargetPos)
		action:SetParamAsPoint(1,actor:GetPos())
		action:SetParamFloat(1,g_world:GetAccumTime());	--	Start Time
		action:SetParamInt(8,0)
		action:SetParamInt(9,0)
		
		actor:FreeMove(true);	
		actor:StopJump();
		actor:SetNoWalkingTarget(false)
	end
	
	return true
end

function Act_Mon_MassiveProjectile_Jump_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
	local	iState = action:GetParamInt(0)
	
	local kCurAction = actor:GetAction()
	local iCurrnetSlot = kCurAction:GetCurrentSlot()
	if iCurrnetSlot == 0 or iCurrnetSlot == 1 then
		local	fFlyHeight = 300
		local	fElapsedTime = accumTime - action:GetParamFloat(1)
		--ODS("Time:"..fElapsedTime.."\n",false,987)
		local	fRate  = fElapsedTime;
			
		if fRate>1.0 then
			fRate = 1.0
			--actor:SetHide(true)
		end
		local	kStartPos = action:GetParamAsPoint(1)
		local	kMovingVec = Point3(0,0,1)
		local	fMovingDist = fRate*fFlyHeight*4;
		if fMovingDist > 450 then
			fMovingDist = 450
		end
		kMovingVec:Multiply(fMovingDist);
		
		local	kNewPos = kStartPos:_Add(kMovingVec);
		
		actor:SetTranslate(kNewPos);
	end
	
	local kEnd = action:GetParamInt(9)
	if iCurrnetSlot == 4 and kEnd == 0 then
		local kInit = action:GetParamInt(8)
		if kInit == 0 then
			action:SetParamFloat(2,g_world:GetAccumTime())
			action:SetParamInt(8,1)
			--actor:SetHide(false)
		end
		local	fFlyHeight = 300
		local	fElapsedTime = accumTime - action:GetParamFloat(2)
		local	fRate  = fElapsedTime;
		
		local 	kEndAniTime = 0.6		
		if fRate>kEndAniTime then
			fRate = kEndAniTime
			actor:FreeMove(false)
			action:SetParamInt(9,1)
		end
		
		local	kStartPos = action:GetParamAsPoint(0)
		local	kMovingVec = Point3(0,0,1)
		local	fMovingDist = (kEndAniTime - fRate)*fFlyHeight*4;
		kMovingVec:Multiply(fMovingDist);
		
		local	kNewPos = kStartPos:_Add(kMovingVec);
		--kNewPos:SetZ(kNewPos:GetZ() + 5)
		actor:SetTranslate(kNewPos);
		--ODS("NewPos X:"..kNewPos:GetX().."NewPos Y:"..kNewPos:GetY().."NewPos Z:"..kNewPos:GetZ().."\n",false,987)
	end

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

		local kTargetPos = action:GetParamAsPoint(0)
		kTargetPos = g_world:ThrowRay(kTargetPos, Point3(0,0,-1), 300);
		if kTargetPos:GetX() == -1 and kTargetPos:GetY() == -1 and kTargetPos:GetZ() == -1 then
			kTargetPos = action:GetParamAsPoint(0)
		end

		action:SetParamInt(4,fSkillRange)
		if actor:IsEqualObjectName("m_cleef_VanHelsing") == false then
			actor:AttachParticleToPointS(81190, kTargetPos, RangeEffect, fSkillRange*0.009)
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
				ODS("End\n",false,987)
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
				Act_Mon_MassiveProjectile_Jump_OnCastingCompleted(actor,action)
			
			end
		end
		
	
	end
	return true
end

function Act_Mon_MassiveProjectile_Jump_OnCleanUp(actor, action)
	actor:DetachFrom(81190)
	actor:FreeMove(false);	
	ODS("Act_Mon_MassiveProjectile_Jump_OnCleanUp\n",false,987)
	
	return true;
end

function Act_Mon_MassiveProjectile_Jump_OnLeave(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	ODS("Act_Mon_EnergyExplosion_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
	if action:GetActionType()=="EFFECT" or
		actionID == "a_freeze" or
		actionID == "a_reverse_gravity" or
		actionID == "a_blow_up" or
		actionID == "a_blow_up_small" or
		actionID == "a_knock_back" or
		actionID == "a_die" or
		actionID == "a_idle" or
		actionID == "a_run_right" or
		actionID == "a_run_left" or
		actionID == "a_handclaps" then
		return true
	end
	return false 
end

function Act_Mon_MassiveProjectile_Jump_FireArrow(actor,action,fFireDelay)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local	fRange =action:GetParamInt(4);
	local	kTargetCenterPos = action:GetParamAsPoint(0);	
	
	local	kStartPos = Point3(kTargetCenterPos:GetX(),
								kTargetCenterPos:GetY(),
								kTargetCenterPos:GetZ()+300);
	
	local	kFireDir = Point3(0,0,-1)
	local	kFireTargetPos = g_world:ThrowRay(kStartPos,kFireDir,1000)
	if kFireTargetPos:GetX() == -1 then
		return true
	end

	local	iSkillRange = action:GetSkillRange(0,actor)
	
	kStartPos:SetZ(kFireTargetPos:GetZ()+300)
	
	local	kProjectile = Act_Mon_MassiveProjectile_Jump_LoadToWeapon(actor,action,kStartPos);
	
	kProjectile:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());
	kProjectile:SetTargetLoc(kFireTargetPos);
	kProjectile:DelayFire(fFireDelay,true);
	
	local	iFireCount = action:GetParamInt(3);
	action:SetParamInt(3,iFireCount+1);

	return	true;
end

function Act_Mon_MassiveProjectile_Jump_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	ODS("Act_Mon_MassiveProjectile_Jump_OnEvent textKey:"..textKey.."\n");

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
		
		--actor:SetHide(true)

		local	fFireTerm = 0.1
		local term = actor:GetAnimationInfo("FIRE_TERM")
		if nil ~= term then
			fFireTerm = tonumber(term)
		end

		local	i=0
		while i<iTotalProjectile do
		
			Act_Mon_MassiveProjectile_Jump_FireArrow(actor,kAction,fFireTerm*i);
			i=i+1;
		
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
