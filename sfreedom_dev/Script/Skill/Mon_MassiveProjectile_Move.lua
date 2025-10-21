-- Melee
function Act_Mon_MassiveProjectile_Move_OnCheckCanEnter(actor,action)

	return		true;
	
end
function Act_Mon_MassiveProjectile_Move_OnCastingCompleted(actor, action)

	ODS("Act_Mon_MassiveProjectile_Move_OnCastingCompleted\n");
	
	Act_Mon_Melee_SetState(actor,action,"FIRE")
	Act_Mon_MassiveProjectile_Move_Fire(actor,action);
	

end

function Act_Mon_MassiveProjectile_Move_Fire(actor,action)

end

function Act_Mon_MassiveProjectile_Move_OnEnter(actor, action)
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
	action:SetParamFloat(5,0)
	action:SetParamInt(1,0)	--파티클을 붙인적 있냐?
	action:SetParamInt(2,0)	--파티클 발사 갯수.

	if action:GetActionParam() == AP_FIRE then
		Act_Mon_MassiveProjectile_Move_OnCastingCompleted(actor,action)
	else
	
	    ODS("Act_Mon_MassiveProjectile_Move_OnEnter Start Casting\n");

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
				ODS("Act_Mon_MassiveProjectile_Move_OnEnter X : "..ptTargetPos:GetX().." Y : "..ptTargetPos:GetY().." Z : "..ptTargetPos:GetZ().."\n", false, 1509)
			end
		end
		
		-- Look at targetpos
		actor:LookAt(ptTargetPos)
		actor:SetMovingDir(ptTargetPos)
		
		-- Save the targetpos
		action:SetParamAsPoint(0,ptTargetPos)

		ptTargetPos:Subtract(actor:GetTranslate())
		local kLookDir = actor:GetLookingDir()

		local angle = (1-ptTargetPos:Dot(kLookDir))*(math.pi*0.5)
--		actor:IncRotate(angle)
	end
	
	return true
end

function Act_Mon_MassiveProjectile_Move_OnUpdate(actor, accumTime, frameTime)

--	ODS("Act_Mon_MassiveProjectile_Move_OnUpdate\n", false, 1509)
	
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


	local iRangeType = action:GetAbil(AT_2ND_AREA_TYPE)
	local iRange = action:GetAbil(AT_2ND_AREA_PARAM1)--action:GetSkillRange(action:GetActionNo(), actor)
	local iRange2 = action:GetAbil(AT_2ND_AREA_PARAM2)
--	ODS("iRange : "..iRange.." 2 : "..iRange2.."\n", false, 1509)
	local ptActorPos = actor:GetPos()
	local ptActorPos2 = actor:GetPos()
--	ODS(" ActorPos X : "..ptActorPos:GetX().." Y : "..ptActorPos:GetY().."\n", false, 1509)
	
	local	ptTargetMovePos = action:GetParamAsPoint(0)	--혹시나. 원래는 엔티티의 소환시간이 다되서 끝나야 된다EffectScaleEffectScale
	ptTargetMovePos:Subtract(ptActorPos)--actor:GetLookingDir()
	ptTargetMovePos:SetZ(0)
	ptTargetMovePos:Unitize()
	local kLookDir = ptTargetMovePos

	local IsParticle = action:GetParamInt(1)
	if 0==IsParticle then	--아직 파티클을 붙인적이 없으면
		local EffectNode = actor:GetAnimationInfo("FIRE_EFFECT_NODE")
		local Effect = actor:GetAnimationInfo("FIRE_EFFECT")
		local EffectScale = actor:GetAnimationInfo("FIRE_EFFECT_SCALE")
		action:SetParamInt(1, 1)
		if nil~=Effect then
			if nil==EffectNode or ""==EffectNode then
				EffectNode = "char_root"
			end
			if nil==EffectScale or ""==EffectScale then
				EffectScale = 1
			else
				EffectScale = tonumber(EffectScale)
			end
			actor:AttachParticleS(12500, EffectNode, Effect, EffectScale)
		end
	end
	
	if action:GetActionParam() == AP_FIRE then
		local fLastFireTime = action:GetParamFloat(5)
		local iInter = actor:GetAnimationInfo("PROJECTILE_INTER")
		if nil==iInter or ""==iInter then
			iInter = 0.1
		else
			iInter = tonumber(iInter)
		end
		local iDeltaTime = accumTime - fLastFireTime
		local ArriveEffect = actor:GetAnimationInfo("ARRIVE_EFFECT")
		if iInter < iDeltaTime then
			action:SetParamFloat(5, accumTime)
			local iFireCount = 1--iDeltaTime/iInter	--발사횟수 동기화
			for i=0,2 do
				local ptDropPos = ptActorPos
				local w = 0
				local h = 0
				if 0==iRangeType then	--원형
					w = Random()%iRange
					h = Random()%iRange
					w = w - iRange*0.5
					h = h - iRange*0.5
				else					--사각형
					w = Random()%(iRange*2)
					h = Random()%iRange2
					w=w-iRange	--중점에 위치시킬려고
					h=h-(iRange2*0.5)
					local kRight = Point3(1,0,0)
					local angle = (1-kRight:Dot(kLookDir))*(math.pi*0.5)
--					ODS("angle : "..angle.." Look X : "..kLookDir:GetX().." Y : "..kLookDir:GetY().." Z : "..kLookDir:GetZ().."\n", false, 1509)
					local TempVec = Point3(w, h, 0)
					TempVec:Rotate(Point3(0,0,1), angle)
					w = TempVec:GetX()
					h = TempVec:GetY()
				end
				ptDropPos:SetX(ptActorPos2:GetX()+w)
				ptDropPos:SetY(ptActorPos2:GetY()+h)
				ptDropPos:SetZ(ptActorPos2:GetZ()+200)
--				ptDropPos:SetZ(200)
--				ODS("FIRE iDeltaTime : "..iDeltaTime.." X : "..ptDropPos:GetX().." Y : "..ptDropPos:GetY().." Z : "..ptDropPos:GetZ().."\n", false, 1509)
--				ODS("w : "..w.." h : "..h.." ActorPos X : "..ptActorPos2:GetX().." Y : "..ptActorPos2:GetY().."\n", false, 1509)				

				if( CheckNil(g_world == nil) ) then return false end
				if( CheckNil(g_world:IsNil()) ) then return false end
				local kFireTargetPos = g_world:ThrowRay(ptDropPos, Point3(0,0,-1), 500)
				if nil~=kFireTargetPos and false==kFireTargetPos:IsZero() and -1~=kFireTargetPos:GetX() and -1~=kFireTargetPos:GetY() then
					local kProjectile = Act_Mon_MassiveProjectile_LoadToWeapon(actor, action, ptDropPos)
					if nil~=kProjectile and false==kProjectile:IsNil() then
						kProjectile:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());

						local iIndex = action:GetParamInt(2)
						kProjectile:SetParamValue("index", tostring(iIndex))
						if nil~=ArriveEffect and ""==ArriveEffect then
							kProjectile:SetParamValue("effect", ArriveEffect)
						end
						ODS("kFireTargetPos X : "..kFireTargetPos:GetX().." Y : "..kFireTargetPos:GetY().." Z : "..kFireTargetPos:GetZ().."\n", false, 1509)
						kProjectile:SetTargetLoc(kFireTargetPos);
						kProjectile:DelayFire(iInter*i,true);
--						kProjectile:Fire(true)					
						action:SetParamInt(2, action:GetParamInt(2)+1)
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
				Act_Mon_MassiveProjectile_Move_OnCastingCompleted(actor,action)
			
			end
		end
	end

	local	fMoveSpeed = actor:GetAbil(AT_MOVESPEED)*(1+action:GetAbil(AT_R_MOVESPEED)/ABILITY_RATE_VALUE)
	ptTargetMovePos:Multiply(fMoveSpeed)
	
	actor:SetMovingDelta(ptTargetMovePos)

	return true
end

function Act_Mon_MassiveProjectile_Move_OnCleanUp(actor)
	ODS("Act_Mon_MassiveProjectile_Move_OnCleanUp\n", false, 1509)
	actor:DetachFrom(125)
	return true;
end

function Act_Mon_MassiveProjectile_Move_OnLeave(actor, action)
	ODS("Act_Mon_MassiveProjectile_Move_OnLeave\n");
	return true;
end

function Act_Mon_MassiveProjectile_Move_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	ODS("Act_Mon_MassiveProjectile_Move_OnEvent textKey:"..textKey.."\n");

	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	
	if textKey == "hit" or textKey == "fire" or textKey == "shot" then
		
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

function Act_Mon_MassiveProjectile_Move_OnTargetListModified(actor,action,bIsBefore)
end


