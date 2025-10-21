-- Melee
function Act_Mon_Paris_Laser_OnCheckCanEnter(actor,action)

	return		true;
	
end
function Act_Mon_Paris_Laser_OnCastingCompleted(actor, action)

	ODS("Act_Mon_Paris_Laser_OnCastingCompleted\n");
	
	Act_Mon_Melee_SetState(actor,action,"FIRE")
	Act_Mon_Paris_Laser_Fire(actor,action);
	actor:DetachFrom(91)	
	
end

function	Act_Mon_Paris_Laser_LoadToWeapon(actor,action,iProjectileIndex)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID")
	
	if kProjectileID == nil then
--		kProjectileID = "PROJECTILE_ANTARES_LASER"
		kProjectileID = "PROJECTILE_MAGE_DEFAULT_SHOT"
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	
	local	kTargetNodeID = actor:GetAnimationInfo("FIRE_START_NODE"..iProjectileIndex);
	if kTargetNodeID == nil then
		kTargetNodeID = "p_ef_weapon"
	end
	
	kNewArrow:LoadToHelper(actor,kTargetNodeID);	--	장전
	return kNewArrow;

end

function Act_Mon_Paris_Laser_Fire(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local	kEffect = actor:GetAnimationInfo("FIRE_EFFECT")
	if nil==kEffect or ""==kEffect then
		--kEffect = "e_b_palis_skill_02_BlackBeem_char_roor"
		return
	end

	local kFireCount = action:GetParamInt(11)
	local kNotFirePos = action:GetParamInt(12)
	for i=0,kFireCount do
		local	kTargetNodeID = actor:GetAnimationInfoFromAniName("FIRE_START_NODE"..i, "skill_shot_01")
		if nil~=kTargetNodeID and false==kTargetNodeID:IsNil() and i~=kNotFirePos then
			actor:AttachParticle(231+i, kTargetNodeID:GetStr(), kEffect)
		end
	end
end

function Act_Mon_Paris_Laser_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--ODS("Act_Mon_Melee_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");
		
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end

	if action:GetActionParam() == AP_FIRE then
		Act_Mon_Paris_Laser_OnCastingCompleted(actor,action)
	else
		local Effect = actor:GetAnimationInfoFromAniName("CAST_EFFECT", "btlidle_01_skill_shot")
		if false==Effect:IsNil() and ""~=Effect:GetStr() then
			action:SetParam(88,Effect:GetStr())
			local EffectNode = actor:GetAnimationInfoFromAniName("CAST_EFFECT_NODE", "btlidle_01_skill_shot")
			if true==EffectNode:IsNil() or ""==EffectNode:GetStr() then
				EffectNode = "char_root"
			end
			action:SetParam(89,EffectNode:GetStr())
			local EffectScale = actor:GetAnimationInfoFromAniName("CAST_EFFECT_SCALE", "btlidle_01_skill_shot")
			if false==EffectScale:IsNil() and ""~=EffectScale:GetStr() then
				EffectScale = 1
			else
				EffectScale = tonumber(EffectScale:GetStr())
			end
			action:SetParamFloat(90,EffectScale)
		end

		if( Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
			local Effect = action:GetParam(88)
			if nil~=Effect and ""~=Effect and "null"~=Effect then
				actor:AttachParticleS(91, action:GetParam(89), Effect, action:GetParamFloat(90))
			end
		end	
				
		actor:SeeFront(true, true)	
	end

	action:SetParamFloat(4, 0)	--시간 저장
	action:SetParamInt(5, 0)	--프로젝틸 인덱스 저장
	action:SetParam(6, "end")	--발사, 중단 상태 저장
	action:SetParamFloat(7, 0)	--각도 저장

	local	fSpeed = actor:GetAnimationInfo("ROT_SPEED")	--초당
	if nil==fSpeed or ""==fSpeed then
		fSpeed = 180
	else
		fSpeed = tonumber(fSpeed)
	end
	local	fTotalAngle = actor:GetAnimationInfo("ROT_ANGLE")
	if nil==fTotalAngle or 0==fTotalAngle then
		fTotalAngle = 90
	end

	action:SetParamFloat(8, fSpeed)	--각속도
	action:SetParamFloat(9, fTotalAngle)	--최대각
	local iMainTime = action:GetAbil(AT_MAINTENANCE_TIME)
	if 0>=iMainTime  then
		iMainTime = 3000
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(10, g_world:GetAccumTime() + iMainTime*0.001)	--처음시간

	actor:SeeFront(true)

	local kFireCount = action:GetAbil(AT_CUSTOMDATA1)
	if 0==kFireCount then
		kFireCount = 3
	end

	action:SetParamInt(11, kFireCount)	--발사위치 

	local kNotFirePos = action:GetAbil(AT_CUSTOMDATA2)
	if 0==kNotFirePos then	--0번째 위치는 무조건 발사해야 한다는 단점이;;;;;;
		kNotFirePos = -1
	end
	action:SetParamInt(12, kNotFirePos)	--발사하면 안되는위치 
	
	return true
end 

function Act_Mon_Paris_Laser_FireProjectile(actor, action, iProjectileIndex)
	local	kTargetNodeID = actor:GetAnimationInfo("FIRE_START_NODE"..iProjectileIndex);
	if nil == kTargetNodeID then
		kTargetNodeID = "p_ef_weapon"
	end

	local	kArrow = Act_Mon_Paris_Laser_LoadToWeapon(actor,action,iProjectileIndex);
	if kArrow == nil or kArrow:IsNil() then
		return false
	end

	if kArrow:IsNil() == false then
		kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp())
	end

	local weaponrot = actor:GetNodeRotateAxis(kTargetNodeID,2, true)
	weaponrot:Multiply(-1)
	local weaponpos = actor:GetNodeWorldPos(kTargetNodeID)
	local kEndnode = kTargetNodeID.."_end"
	local weaponendpos = actor:GetNodeWorldPos(kEndnode)
	local dir = Point3(weaponendpos:GetX() - weaponpos:GetX(), weaponendpos:GetY() - weaponpos:GetY(), weaponendpos:GetZ() - weaponpos:GetZ())
	
	dir:Unitize()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local TargetPos = g_world:ThrowRay(weaponpos,dir,500)	-- 발사 방향으로 레이
	if -1==TargetPos:GetX() and -1==TargetPos:GetY() and -1==TargetPos:GetZ() then	--충돌하는 곳이 없으면
		dir:Multiply(action:GetAbil(AT_ATTACK_RANGE))
		TargetPos = weaponpos
		TargetPos:Add(dir)
	end

	kArrow:SetTargetLoc(TargetPos)	
	kArrow:Fire(true);
	action:SetParamInt(5, action:GetParamInt(5)+1)

	return true
end

function Act_Mon_Paris_Laser_OnUpdate(actor, accumTime, frameTime)

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
		actor:IncRotate(action:GetParamFloat(8)*math.pi/180*frameTime)
		local kFireCount = action:GetParamInt(11)
		local kNotFirePos = action:GetParamInt(12)
		if animDone == true then
			action:SetParam(1, "end")
			for i=0,kFireCount do 
				if i~=kNotFirePos then
	 				actor:DetachFrom(231+i) 
				end
			end
			return false
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

			local inter = actor:GetAnimationInfo("SHOT_INTER_TIME")
			if inter == nil then
				inter = 0.3
			else
				inter = tonumber(inter)
			end

			if inter < delta then

				action:SetParamFloat(4, accumTime) 

				for i=0,kFireCount do
					if i~=kNotFirePos then
						Act_Mon_Paris_Laser_FireProjectile(actor, action, i)
					end
				end
			end

		end

	elseif action:GetActionParam() == AP_CASTING then
	
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
				local Effect = action:GetParam(88)
				if nil~=Effect and ""~=Effect and "null"~=Effect  then
					actor:AttachParticleS(91, action:GetParam(89), Effect, action:GetParamFloat(90))
				end
			end
		end
			
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Paris_Laser_OnCastingCompleted(actor,action)
			
			end
		end
		
	end
	return true
end

function Act_Mon_Paris_Laser_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	if actor:GetAction():GetParamInt(4) == 1 then
		actor:HideParts(EQUIP_POS_WEAPON, false)	--	칼 숨김 해제
		actor:GetAction():SetParamInt(4,0);
	end	
	local kFireCount = actor:GetAction():GetParamInt(11)
	for i=0,kFireCount do actor:DetachFrom(231+i) end
	return true;
end

function Act_Mon_Paris_Laser_OnLeave(actor, action)
	local kFireCount = actor:GetAction():GetParamInt(11)

	for i=0,kFireCount do actor:DetachFrom(231+i) end
	return true;
end

function Act_Mon_Paris_Laser_OnEvent(actor,textKey)

	ODS("Act_Mon_Paris_Laser_OnEvent textKey:"..textKey.."\n");

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local kAction = actor:GetAction();
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	if textKey == "hit" or textKey == "fire" then

		kAction:SetParam(6,textKey)

		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
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

	end

	return	true;
end

function Act_Mon_Paris_Laser_OnTargetListModified(actor,action,bIsBefore)
end
