-- Melee
function Act_Mon_MassiveProjectile_OnCheckCanEnter(actor,action)

	return		true;
	
end
function Act_Mon_MassiveProjectile_OnCastingCompleted(actor, action)

	--ODS("Act_Mon_MassiveProjectile_OnCastingCompleted\n");
	
	Act_Mon_Melee_SetState(actor,action,"FIRE")
	Act_Mon_MassiveProjectile_Fire(actor,action);
	

end

function	Act_Mon_MassiveProjectile_LoadToWeapon(actor,action,kStartPos)
	
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
			local scale = tonumber(actor:GetAnimationInfo("RANDOM_SCALE"))
			if nil == scale then
				scale = fProjectileScale
			else
	
				if 0==scale then 
					scale = fProjectileScale 
				else
					scale = fProjectileScale + (Random()%(scale*2) - scale) -- scale값이 랜덤 범위
					local min_scale = actor:GetAnimationInfo("RANDOM_MIN_SCALE")
					if nil~=min_scale then
						min_scale = tonumber(min_scale)
					else
						min_scale = 0
					end
					if scale < min_scale then
						scale = min_scale
					end
				end
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
	
	kNewArrow:LoadToPosition(kStartPos);	--	장전
	return kNewArrow;

end

function Act_Mon_MassiveProjectile_Fire(actor,action)

end

function Act_Mon_MassiveProjectile_TopPosZ(kPos, actor)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kAddZ = 300
	if actor:IsEqualObjectName("m_cleef_VanHelsing") == true then
		kAddZ = 400
	elseif actor:IsEqualObjectName("m_elga03") == true then
		kAddZ = 500
	else
		local action = actor:GetAction()
		if false==action:IsNil() then
			local Height = actor:GetAnimationInfoFromAniName("TOP_RANGE_EFFECT_HEIGHT",actor:GetAction():GetSlotAnimName(1))
			if nil~=Height and false==Height:IsNil() then
				kAddZ = tonumber(Height:GetStr())
			end
		end
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kTopDir = Point3(0,0,1)
	--ODS("ptTargetPos] X: "..kPos:GetX()..", Y: "..kPos:GetY()..", Z: "..kPos:GetZ().."\n",false,6482)
	local kFireTargetPos = g_world:ThrowRay(kPos,kTopDir,kAddZ)
	if kFireTargetPos:GetZ() ~= -1 then
		kAddZ = kFireTargetPos:GetZ()-kPos:GetZ()-1
	end
	if kAddZ < 0 then
		kAddZ = 0
	end
	return kAddZ
end

function Act_Mon_MassiveProjectile_OnEnter(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	ODS("Act_Mon_MassiveProjectile_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");

	local	prevAction = actor:GetAction()
	if prevAction:GetID() ~= "a_jump" then
		actor:Stop()
	end

	local	kSkillDef = GetSkillDef(action:GetActionNo())
	local	fSkillRange = 100
	local	fSkillTime = 5.0
	if nil~=kSkillDef and false==kSkillDef:IsNil() then
		fSkillRange = kSkillDef:GetAbil(AT_1ST_AREA_PARAM2)
		if 0==fSkillRange then
			fSkillRange = 100
		end
		
		local fSkillTime = kSkillDef:GetAbil(AT_MAINTENANCE_TIME) / 1000.0
		--ODS("Act_Mon_MassiveProjectile_OnEnter fSkillTime: "..fSkillTime.."\n",false,6482)
		if 0==fSkillTime then
			fSkillTime = 5.0
		end
	end
		
	action:SetParamInt(4,fSkillRange)
	action:SetParamFloat(10,fSkillTime)
	action:SetParamInt(5,0) --천장이펙트 표시유무
	action:SetParamInt(6,0) --바닥이펙트 표시유무
	action:SetParamInt(7,0) --바닥이펙트 디폴트 이펙트 사용유무(0: 사용, 1: 미사용)
	for i=0, action:GetSlotCount() do
		local Effect = actor:GetAnimationInfoFromAniName("RANGE_EFFECT", action:GetSlotAnimName(i))
		if false==Effect:IsNil() then
			action:SetParamInt(7,1)
			break
		end
	end

	local RangeEffectDelay = actor:GetAnimationInfoFromAniName("RANGE_EFFECT_DELAY", action:GetCurrentSlotAnimName())
	if nil~=RangeEffectDelay and false==RangeEffectDelay:IsNil() then
		RangeEffectDelay = tonumber(RangeEffectDelay:GetStr()) / 1000.0
	else
		RangeEffectDelay = 0
	end
	action:SetParamFloat(777,RangeEffectDelay+g_world:GetAccumTime())

	if action:GetActionParam() == AP_FIRE then
		Act_Mon_MassiveProjectile_OnCastingCompleted(actor,action)
	else
	
	    ODS("Act_Mon_MassiveProjectile_OnEnter Start Casting\n");

		if( Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
				
		local	ptTargetPos = 0;
		
		if IsSingleMode() then
			ptTargetPos = GetMyActor():GetPos();
		else
			local 	IsTargetPosBySelf = actor:GetAnimationInfoFromAniName("IS_TARGET_POS_BY_SELF", action:GetCurrentSlotAnimName()):GetStr()
			local	kPacket = action:GetParamAsPacket();
			if kPacket == nil or kPacket:IsNil() or (nil~=IsTargetPosBySelf and "TRUE"==IsTargetPosBySelf) then
				ptTargetPos = actor:GetPos()
			else
				ptTargetPos = Point3(kPacket:PopFloat(),kPacket:PopFloat(),kPacket:PopFloat());
			end
		end

		-- Look at targetpos
		actor:LookAt(ptTargetPos,true,false);
		
		-- Save the targetpos
		action:SetParamAsPoint(0,ptTargetPos)

		-- Save the 발사체가 떨어지는 높이 위치
		action:SetParamInt(10,Act_Mon_MassiveProjectile_TopPosZ(ptTargetPos,actor))

		local	fRange =action:GetParamInt(4);
		local	iTotalProjectile = 20;
		local count = actor:GetAnimationInfoFromAniName("PROJECTILE_COUNT",action:GetSlotAnimName(2))
		if nil~=count and false==count:IsNil() then
			iTotalProjectile = tonumber(count:GetStr())
		end
		action:SetParamInt(81190, iTotalProjectile)

		for i=1, iTotalProjectile do
			local	kStartPos = Point3(ptTargetPos:GetX()+math.random(-fRange/2,fRange/2),
									ptTargetPos:GetY()+math.random(-fRange/2,fRange/2),
									ptTargetPos:GetZ()+10);

			-- local	kFireDir = Point3(0,0,-1)
			-- local	kFireTargetPos = g_world:ThrowRay(kStartPos,kFireDir,1000)
			-- if kFireTargetPos:GetX() ~= -1 then
				-- kStartPos:SetZ(kFireTargetPos:GetZ()+1)
			-- end
			action:SetParamAsPoint(81190+i,kStartPos)
			--ODS(i.."] OnEnter "..kStartPos:GetX()..", "..kStartPos:GetY()..", "..kStartPos:GetZ().."\n",false,6482)
		end
	end
	
	return true
end

function Act_Mon_MassiveProjectile_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
	local	iState = action:GetParamInt(0)

	local isRange = action:GetParamInt(6)
	if 0==isRange then	--바닥 범위이펙트를 표시한 적이 없으면
		if g_world:GetAccumTime() >= action:GetParamFloat(777) then
			local RangeEffect = actor:GetAnimationInfo("RANGE_EFFECT")
			if 0==action:GetParamInt(7) and nil==RangeEffect or ""==RangeEffect then
				RangeEffect = "ef_Shark_king_abdula_skill_03_03_char_root"
			end
			
			if nil==RangeEffect or ""==RangeEffect then
				RangeEffect = "NONE"
			end

			local RangeEffectEach = actor:GetAnimationInfo("IS_RANGE_EFFECT_EACH")
			if nil==RangeEffectEach or "TRUE"~=RangeEffectEach then
				if actor:IsEqualObjectName("m_cleef_VanHelsing") == false and "NONE"~=RangeEffect then
					action:SetParamInt(6,1)
					local fSkillRange = action:GetParamInt(4)
					local fEffectScale = fSkillRange*0.009
					actor:AttachParticleToPointS(81190,action:GetParamAsPoint(0),RangeEffect, fEffectScale)
				end
			else
				if "NONE"~=RangeEffect then
					action:SetParamInt(6,1)
					local iTotalCount = action:GetParamInt(81190)
					for i=1, iTotalCount do
						local kStartPos = action:GetParamAsPoint(81190+i)					
						actor:AttachParticleToPointS(81190+i, kStartPos, RangeEffect, 1)
					end
				end
			end
		end
	end

	local isRange = action:GetParamInt(5)
	if 0==isRange then	--범위이펙트를 표시한 적이 없으면
		action:SetParamInt(5,1)
				
		local fSkillRange = action:GetParamInt(4)
		local fEffectScale = fSkillRange*0.009

		--ODS("Actor:ID: " ..action:GetActionName().."\n",false,6482) --massive_projectile_btlidle
		local TopRangeEffect = actor:GetAnimationInfo("TOP_RANGE_EFFECT")
		if nil~=TopRangeEffect and ""~=TopRangeEffect then
			local TopRangeEffectScale = actor:GetAnimationInfo("TOP_RANGE_EFFECT_SCALE")
			if  nil==TopRangeEffectScale or ""==TopRangeEffectScale then
				TopRangeEffectScale = 1.0
			else
				TopRangeEffectScale = tonumber(TopRangeEffectScale)
			end
			local TopPos = action:GetParamAsPoint(0)
			TopPos:SetZ(TopPos:GetZ()+action:GetParamInt(10))

			local fSkillTime = 0
			local TopEffectTime = actor:GetAnimationInfo("TOP_RANGE_EFFECT_TIME")
			if nil~=TopEffectTime and TopEffectTime~="" then
				fSkillTime = tonumber(TopEffectTime)
			end

			if 0==fSkillTime then
				fSkillTime = action:GetParamFloat(10)
			end
			--ODS("AttachParticleSWithRotate: "..TopRangeEffect..", X:"..TopPos:GetX()..", Y:"..TopPos:GetY()..", Z:"..TopPos:GetZ().."\n",false,6482)
			g_world:AttachParticleSWithRotate(TopRangeEffect,TopPos,fEffectScale*TopRangeEffectScale,Point3(0,0,0),fSkillTime)
		end
	end
	
	if action:GetActionParam() == AP_FIRE then

		if true == animDone then
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
			if true == animDone then
				Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
			
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_MassiveProjectile_OnCastingCompleted(actor,action)
			
			end
		end
		
	
	end
	return true
end

function Act_Mon_MassiveProjectile_OnCleanUp(actor)
	actor:DetachFrom(81190)
	return true;
end

function Act_Mon_MassiveProjectile_OnLeave(actor, action)
	--ODS("Act_Mon_MassiveProjectile_OnLeave\n");
	return true;
end

function Act_Mon_MassiveProjectile_FireArrow(actor,action,fFireDelay, iCount)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local	fRange =action:GetParamInt(4);
	local	kTargetCenterPos = action:GetParamAsPoint(0);	
	local	kStartPos = action:GetParamAsPoint(81190+iCount)

	-- local	kStartPos = Point3(kTargetCenterPos:GetX()+math.random(-fRange/2,fRange/2),
								-- kTargetCenterPos:GetY()+math.random(-fRange/2,fRange/2),
								-- kTargetCenterPos:GetZ()+10);
	
	local	kFireDir = Point3(0,0,-1)
	local	kFireTargetPos = g_world:ThrowRay(kStartPos,kFireDir,1000)
	if kFireTargetPos:GetX() == -1 then
		return false
	end
	kFireTargetPos:SetX(kStartPos:GetX())
	kFireTargetPos:SetY(kStartPos:GetY())

	local	iSkillRange = action:GetSkillRange(0,actor)
	local	kAddZ = Act_Mon_MassiveProjectile_TopPosZ(kFireTargetPos,actor)
	kStartPos:SetZ(kFireTargetPos:GetZ()+kAddZ)

	local	kProjectile = Act_Mon_MassiveProjectile_LoadToWeapon(actor,action,kStartPos);
	
	local QuakeTime = actor:GetAnimationInfo("QUAKE_TIME")
	if nil ~= QuakeTime and "" ~= QuakeTime then
		kProjectile:SetParamValue("IS_QUAKE_CAMERA","FALSE")
	end

	kProjectile:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());
	kProjectile:SetTargetLoc(kFireTargetPos);
	kProjectile:DelayFire(fFireDelay,true);
	
	local	iFireCount = action:GetParamInt(3);
	action:SetParamInt(3,iFireCount+1);

	return	true;
end

function Act_Mon_MassiveProjectile_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	--ODS("Act_Mon_MassiveProjectile_OnEvent textKey:"..textKey.."\n");

	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	
	if textKey == "hit" or textKey == "fire" or textKey == "shot" then

		local	iTotalProjectile = kAction:GetParamInt(81190);

		local	fFireTerm = 0.1
		local term = actor:GetAnimationInfo("FIRE_TERM")
		if nil ~= term then
			fFireTerm = tonumber(term)
		end

		local	kSoundID = actor:GetAnimationInfo("FIRE_SOUND_ID")
		if kSoundID~="" then
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			g_world:AttachSound(kSoundID, kAction:GetParamAsPoint(0))
		end

		local	i=1
		while i<=iTotalProjectile do
		
			Act_Mon_MassiveProjectile_FireArrow(actor,kAction,fFireTerm*i, i);
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
