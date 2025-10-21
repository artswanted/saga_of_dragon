-- Melee
function Act_Mon_AirStrike_OnCheckCanEnter(actor,action)
	return		true
end

function Act_Mon_AirStrike_SetState(actor,action,kState)

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
			--ODS("Act_Mon_AirStrike_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n")
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

function Act_Mon_AirStrike_OnCastingCompleted(actor, action)

	--ODS("Act_Mon_AirStrike_OnCastingCompleted\n",false,6482)
	
	Act_Mon_AirStrike_SetState(actor,action,"FIRE")
	Act_Mon_AirStrike_Fire(actor,action)

end

function Act_Mon_AirStrike_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

end

function Act_Mon_AirStrike_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Act_Mon_AirStrike_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().." CurrentSlotAnimName : "..action:GetCurrentSlotAnimName().."\n",false,6482 )
	
	local	kPacket = action:GetParamAsPacket()
	if kPacket ~= nil and false == kPacket:IsNil() then
		local kSize = kPacket:PopInt()
		local kSafeLoc = nil
		
		action:SetParamInt(2999,kSize)
		
		for i = 0, kSize do
			kSafeLoc = kPacket:PopPoint3()
			if nil~=kSafeLoc and false==kSafeLoc:IsZero() then
				--ODS("Act_Mon_AirStrike_OnEnter Zone X: "..kSafeLoc:GetX()..", Y: "..kSafeLoc:GetY()..", Z: "..kSafeLoc:GetZ().."\n",false,6482)
				action:SetParamAsPoint(3000+i,kSafeLoc)
			end
		end
	end
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_AirStrike_OnCastingCompleted(actor,action)
	else
	
	    --ODS("Act_Mon_AirStrike_OnEnter Start Casting\n")

		if( Act_Mon_AirStrike_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_AirStrike_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	

	end

	action:SetParamInt(21, 0)

	return true
end

function Act_Mon_AirStrike_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	local action = actor:GetAction()
		
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	animDone = actor:IsAnimationDone()
	local	iState = action:GetParamInt(0)

	if action:GetActionParam() == AP_FIRE then

		if "air_strike_fire"==action:GetParam(100) then
			Act_Mon_AirStrike_DoProjectile(actor, action, frameTime)
		end

		if true==animDone then

			if 2==iState then
				if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then
					return false
				else
					local kSize = action:GetParamInt(2999)
					if( CheckNil(g_world == nil) ) then return false end
					if( CheckNil(g_world:IsNil()) ) then return false end
					for i = 0, kSize do
						g_world:DetachParticle(action:GetParamInt(3000+i))
					end
					Act_Mon_AirStrike_SetState(actor,action,"RETURN")
				end
			elseif iState == 3 then
				return false
			end

		end
		
	elseif action:GetActionParam() == AP_CASTING then
	
		if iState == 0 then
			if true==animDone then
				Act_Mon_AirStrike_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		elseif iState == 1 and 0 == action:GetParamInt(21) then
			action:SetParamInt(21,1)

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
				Act_Mon_AirStrike_OnCastingCompleted(actor,action)
			
			end
		end
		
	
	end
	return true
end

function Act_Mon_AirStrike_OnCleanUp(actor)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	local action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	local kSize = action:GetParamInt(2999)
	for i = 0, kSize do
		g_world:DetachParticle(action:GetParamInt(3000+i))
	end	
	
	return true
end

function Act_Mon_AirStrike_OnLeave(actor, action)
	ODS("Act_Mon_AirStrike_OnLeave\n")
	return true
end

function SafezoneEffect(actor, action, textKey)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	CheckNil(nil==textKey);
	
	if textKey == "cast_ef_start" then
		local safe_zone = actor:GetAnimationInfo("SAFEZONE_EFFECT") -- 이팩트 붙이기
		if nil~=safe_zone and ""~=safe_zone then
			local kSize = action:GetParamInt(2999)
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			for i = 0, kSize do
				local kSafeLoc = action:GetParamAsPoint(3000+i)
				if nil~=kSafeLoc and false==kSafeLoc:IsZero() then
					--ODS("SafezoneEffect Zone X: "..kSafeLoc:GetX()..", Y: "..kSafeLoc:GetY()..", Z: "..kSafeLoc:GetZ().."\n",false,6482)
					local iAttachParticleNo = g_world:AttachParticle(safe_zone, kSafeLoc)
					action:SetParamInt(3000+i,iAttachParticleNo)
				end
			end
		end
	end
end

function Act_Mon_AirStrike_DoProjectile(actor, action, frameTime)
	local delay = actor:GetAnimationInfo("DELAY")
	local kSaveTime = action:GetParamFloat(101)
	if nil~=delay and tonumber(delay)>kSaveTime then
		--ODS("kSaveTime: "..kSaveTime..", frameTime: "..frameTime.."\n",false,6482)
		action:SetParamFloat(101, frameTime+kSaveTime)
		return
	end
	action:SetParamFloat(101, 0.0)
	
	local	iAttackRange = action:GetSkillRange(0,actor)
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
	local fRotateAngle = math.pi/4.0
	local	kRotateAxis = Point3(0,0,1);
	
	
	local UID = action:GetParamInt(10+i)

	local	kArrow = nil
	--kArrow = Act_Mon_Shot_Around_LoadToWeapon(actor,action, i)
	local LocZ = actor:GetAnimationInfo("TARGET_LOC_Z")
	if nil~=LocZ and ""~=LocZ then
		LocZ = tonumber(LocZ)
	end
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);

	local	kTargets = CreateActionTargetList(actor:GetPilotGuid(),action:GetActionInstanceID(),action:GetActionNo());

	local kParam = FindTargetParam();
	
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(0,0,action:GetSkillRange(0,actor),0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	local iFoundNum = action:FindTargetsEx2(action:GetActionNo(), TAT_SPHERE, kParam, kTargets, kTargets, actor:GetPilot());
	--local iFoundNum = action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
	--ODS("iFoundNum: "..iFoundNum.."\n",false,6482)
	for idx=0, iFoundNum do
		local	kTargetInfo = kTargets:GetTargetInfo(idx);
		if kTargetInfo:IsNil() then
			break
		end
		
		local	kTargetGUID = kTargetInfo:GetTargetGUID();
		if kTargetGUID:IsNil() then
			break
		end

		local fRotateAngle = math.pi/4.0
		
		for cnt=0, Projectile_cnt-1 do
			local iProjectaleIndex = Random()%fire_random
			kArrow = Act_Mon_Shot_Around_LoadToWeapon(actor, action, iProjectaleIndex)
			if nil~=kArrow and false==kArrow:IsNil() then
				local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
	
				local	kFireTarget = actor:GetLookingDir()
				kFireTarget:Multiply(iAttackRange);
				kFireTarget:Rotate(Point3(0,0,1),fRotateAngle*cnt);
				kFireTarget:Add(arrow_worldpos);
				kArrow:SetTargetLoc(kFireTarget);
			
				kArrow:SetTargetGuidFromServer(kTargetGUID)
				
				if cnt*fire_delay then
					kArrow:DelayFire(cnt*fire_delay,true)
				else
					kArrow:Fire(true);--	발사!
				end
				--ODS("Act_Mon_Shot_Around_OnEvent "..cnt.. " angle : "..fRotateAngle.." Target X : "..kFireTarget:GetX().." Y : "..kFireTarget:GetY().." Z : "..kFireTarget:GetZ().."\n", false, 987)
			end
		end

		action:ClearTargetList()

	end
	DeleteActionTargetList(kTargets);
end

function Act_Mon_AirStrike_OnEvent(actor,textKey)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local kAction = actor:GetAction()
	
	if( CheckNil(nil==kAction) ) then return end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	ODS("Act_Mon_AirStrike_OnEvent textKey:"..textKey.."\n")	

	SafezoneEffect(actor, kAction, textKey)
	
	if kAction:GetActionParam() == AP_CASTING then
		return true
	end

	if textKey == "hit" or textKey == "fire" or textKey == "shot" then
	
		actor:DetachFrom(230)	--캐스팅 이펙트 제거
		
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


		local	iState = kAction:GetParamInt(0)
		if 2==iState then
			kAction:SetParam(100, "air_strike_fire")
			kAction:SetParamFloat(101, 0.0)
		end
	elseif textKey == "hit_end" or textKey == "end" or textKey == "sound06" then
		kAction:SetParam(100, "")
	end

	return	true
end

function Act_Mon_AirStrike_OnTargetListModified(actor,action,bIsBefore)
	-- if( CheckNil(nil==actor) ) then return false end
	-- if( CheckNil(actor:IsNil()) ) then return false end
	
	-- if( CheckNil(nil==action) ) then return false end
	-- if( CheckNil(action:IsNil()) ) then return false end
end
