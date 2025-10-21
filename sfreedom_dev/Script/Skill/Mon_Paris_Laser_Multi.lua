-- Melee
function Act_Mon_Paris_Laser_Multi_OnCheckCanEnter(actor,action)

	return		true;
	
end
function Act_Mon_Paris_Laser_Multi_OnCastingCompleted(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());		
	Act_Mon_Paris_Laser_OnCastingCompleted(actor, action)
	actor:DetachFrom(91)
end

function	Act_Mon_Paris_Laser_Multi_LoadToWeapon(actor,action,iProjectileIndex)

	return Act_Mon_Paris_Laser_LoadToWeapon(actor,action,iProjectileIndex)

end

function Act_Mon_Paris_Laser_Multi_Fire(actor,action)
	Act_Mon_Paris_Laser_Fire(actor,action)
end

function Act_Mon_Paris_Laser_Multi_OnEnter(actor, action)
	
	if false==Act_Mon_Paris_Laser_OnEnter(actor, action) then
		return false
	end

	action:SetParamFloat(8, 0)	--회전하지 말자
	action:SetParamInt(11,0)	--앞으로만 나가자
	
	local kProjCount = action:GetAbil(AT_COUNT)
	if 0==kProjCount then
		kProjCount = 3
	end
	action:SetParamInt(13, kProjCount)	--한곳에서 발사하는 갯수

	local	iInterAngle = actor:GetAbil(AT_MON_PROJECTILE_ANGLE)
	if nil==iInterAngle or 0==iInterAngle then
		iInterAngle = 10
	end

	iInterAngle = iInterAngle*math.pi/180.0
	action:SetParamFloat(140,iInterAngle) 

	if action:GetActionParam() == AP_FIRE then
		Act_Mon_Paris_Laser_Multi_OnCastingCompleted(actor,action)
	else
		local Effect = actor:GetAnimationInfoFromAniName("CAST_EFFECT", "btlidle_02_skill_shot")
		if false==Effect:IsNil() and ""~=Effect:GetStr() then
			action:SetParam(88,Effect:GetStr())
			local EffectNode = actor:GetAnimationInfoFromAniName("CAST_EFFECT_NODE", "btlidle_02_skill_shot")
			if true==EffectNode:IsNil() or ""==EffectNode:GetStr() then
				EffectNode = "char_root"
			end
			action:SetParam(89,EffectNode:GetStr())
			local EffectScale = actor:GetAnimationInfoFromAniName("CAST_EFFECT_SCALE", "btlidle_02_skill_shot")
			if false==EffectScale:IsNil() and ""~=EffectScale:GetStr() then
				EffectScale = 1
			else
				EffectScale = tonumber(EffectScale:GetStr())
			end
			action:SetParamFloat(99,EffectScale)
		end
		if( Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
			local Effect = action:GetParam(89)
			if nil~=Effect and ""~=Effect and "null"~=Effect then
				actor:AttachParticleS(91, action:GetParam(90), Effect, action:GetParamFloat(99))
			end
		end	
				
		-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
		local	kTargetList = action:GetTargetList()
		local	iTargetCount = kTargetList:size()
	
		if iTargetCount>0 then
			local	kTargetInfo = kTargetList:GetTargetInfo(0)
			if kTargetInfo:IsNil() == false then
			
				local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID())
				if kTargetPilot:IsNil() == false then
				
					local	kTargetActor = kTargetPilot:GetActor()
					if kTargetActor:IsNil() == false then
					
						actor:LookAt(kTargetActor:GetPos(),true,true)
					end
				end
			end
		end
	end
	
	return true
end 

function Act_Mon_Paris_Laser_Multi_FireProjectile(actor, action, iProjectileIndex)

	local	kTargetNodeID = actor:GetAnimationInfo("FIRE_START_NODE"..iProjectileIndex);
	if nil == kTargetNodeID then
		kTargetNodeID = "p_ef_weapon"
	end

	local weaponrot = actor:GetNodeRotateAxis(kTargetNodeID,2, true)
	weaponrot:Multiply(-1)
	local weaponpos = actor:GetNodeWorldPos(kTargetNodeID)
	local kEndnode = kTargetNodeID.."_end"
	local weaponendpos = actor:GetNodeWorldPos(kEndnode)
	--ODS("FireProjectile : " .. weaponendpos:GetX() .. ", " .. weaponendpos:GetY() .. ", " .. weaponendpos:GetZ() .. "\n")
	local dir
	if 0~=weaponendpos:GetX() and 0~=weaponendpos:GetY() and 0~= weaponendpos:GetZ() then
		dir = Point3(weaponendpos:GetX() - weaponpos:GetX(), weaponendpos:GetY() - weaponpos:GetY(), weaponendpos:GetZ() - weaponpos:GetZ())
	else
		dir = actor:GetLookingDir()
	end
	
	dir:Unitize()
	local kOrigin = Point3(dir:GetX(), dir:GetY(), dir:GetZ())
	local	kRotateAxis = Point3(0,0,1)

	local kProjCount = action:GetParamInt(13)
	local iInterAngle = action:GetParamFloat(140)
	local iInitRot = (iInterAngle*0.5)*(kProjCount-1)
	dir:Rotate(kRotateAxis, iInitRot)	--한쪽 방향으로 끝까지 돌린 다음에
	for i=1,kProjCount do
		dir:Rotate(kRotateAxis, -iInterAngle)	
	
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local TargetPos = g_world:ThrowRay(weaponpos,dir,500)	-- 발사 방향으로 레이
		if -1==TargetPos:GetX() and -1==TargetPos:GetY() and -1==TargetPos:GetZ() then	--충돌하는 곳이 없으면
			dir:Multiply(action:GetAbil(AT_ATTACK_RANGE))
			TargetPos = weaponpos
			TargetPos:Add(dir)
		end

		local	kArrow = Act_Mon_Paris_Laser_Multi_LoadToWeapon(actor,action,iProjectileIndex);
		if kArrow == nil or kArrow:IsNil() then
			return false
		end

		if 0 < kArrow:GetHomingTime() then
			local	kTargetList = action:GetTargetList()
			local	iTargetCount = kTargetList:size()
			if iTargetCount>0 then
				local	kRandIdx = math.random(0,iTargetCount-1)
				local	kTargetInfo = kTargetList:GetTargetInfo(kRandIdx)
				if kTargetInfo:IsNil() == false then
					local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID())
					if kTargetPilot:IsNil() == false and kTargetPilot:IsAlive() then
						kArrow:SetTargetGuidFromServer(kTargetInfo:GetTargetGUID())
					end
				end
			end
		end

		kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp())
		kArrow:SetTargetLoc(TargetPos)	
		kArrow:Fire(true);
	end
	action:SetParamInt(5, action:GetParamInt(5)+1)


	return true
end

function Act_Mon_Paris_Laser_Multi_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
	local actionID = action:GetID();
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
		local kFireCount = action:GetParamInt(11)
		local kNotFirePos = action:GetParamInt(12)
		if animDone == true then
			action:SetParam(1, "end")
			for i=0,kFireCount do 
				if i~=kNotFirePos then
	 				actor:DetachFrom(231+i) 
				end
			end

			if 2==iState then
				Act_Mon_Melee_SetState(actor,action,"RETURN")
			else
				return false
			end
		else
			if accumTime > action:GetParamFloat(10) then
				for i=0,kFireCount do 
					if i~=kNotFirePos then
						actor:DetachFrom(231+i) 
					end
				end
				return false
			end
		end
		local pasttime = action:GetParamFloat(4)
		local delta = accumTime - action:GetParamFloat(4)

		if "fire"==action:GetParam(6) or "hit"==action:GetParam(6) then

			local bOneShot = actor:GetAnimationInfo("IS_FIRE_ONE_SHOT")
			if bOneShot == "TRUE" then
				action:SetParam(6, "end")
			end

			local inter = actor:GetAnimationInfo("SHOT_INTER_TIME")
			if inter == nil then
				inter = 0.4
			else
				inter = tonumber(inter)
			end

			if inter < delta then

				action:SetParamFloat(4, accumTime) 

				for i=0,kFireCount do
					if i~=kNotFirePos then
						Act_Mon_Paris_Laser_Multi_FireProjectile(actor, action, i)
					end
				end
			end

		end

	elseif action:GetActionParam() == AP_CASTING then
	
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
				local Effect = action:GetParam(88)
				if nil~=Effect and ""~=Effect and "null"~=Effect then
					actor:AttachParticleS(91, action:GetParam(89), Effect, action:GetParamFloat(99))
				end
			end
		end
			
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Paris_Laser_Multi_OnCastingCompleted(actor,action)
			
			end
		end
		
	end
	return true

end

function Act_Mon_Paris_Laser_Multi_OnCleanUp(actor)
	return Act_Mon_Paris_Laser_OnCleanUp(actor)
end

function Act_Mon_Paris_Laser_Multi_OnLeave(actor, action)
	return Act_Mon_Paris_Laser_OnLeave(actor, action)
end

function Act_Mon_Paris_Laser_Multi_OnEvent(actor,textKey)
	return Act_Mon_Paris_Laser_OnEvent(actor,textKey)
end

function Act_Mon_Paris_Laser_Multi_OnTargetListModified(actor,action,bIsBefore)
end
