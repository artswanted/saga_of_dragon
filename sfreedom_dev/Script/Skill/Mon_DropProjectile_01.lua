-- Melee
function Act_Mon_DropProjectile_01_OnCheckCanEnter(actor,action)

	return		true;
	
end
function Act_Mon_DropProjectile_01_OnCastingCompleted(actor, action)

	ODS("Act_Mon_DropProjectile_01_OnCastingCompleted\n");
	
	Act_Mon_Melee_SetState(actor,action,"FIRE")
--	Act_Mon_DropProjectile_01_Fire(actor,action);
end

function	Act_Mon_DropProjectile_01_LoadToWeapon(actor,action,kStartPos)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local	kAttachID="";
	
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID")
	local	fProjectileScale = tonumber(actor:GetAnimationInfo("PROJECTILE_SCALE"));
	if fProjectileScale == nil then
		fProjectileScale = 1.0
	end
	
	if kProjectileID == nil then
		kProjectileID = "Projectile_arrow_02_arrow_head"
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	
	kNewArrow:SetScale(fProjectileScale*kNewArrow:GetScale());
	kNewArrow:LoadToPosition(kStartPos);	--	장전
	return kNewArrow;

end

function Act_Mon_DropProjectile_01_Fire(actor,action)

end

function Act_Mon_DropProjectile_01_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("Act_Mon_DropProjectile_01_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end

	if action:GetActionParam() == AP_FIRE then
		Act_Mon_DropProjectile_01_OnCastingCompleted(actor,action)
	else
	
	    ODS("Act_Mon_DropProjectile_01_OnEnter Start Casting\n");

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
				ptTargetPos = Point3(kPacket:PopFloat(),kPacket:PopFloat(),kPacket:PopFloat())
			end
		end
		
		-- Look at targetpos
		actor:LookAt(ptTargetPos,true,false);
		
		-- Save the targetpos
		action:SetParamAsPoint(0,ptTargetPos);		
		
		actor:AttachParticleToPoint(81190,ptTargetPos,"ef_Blizzard_mon");
		
	end
	
	return true
end

function Act_Mon_DropProjectile_01_OnUpdate(actor, accumTime, frameTime)

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
--	local	iTotalProjectile = 30;
	
	if action:GetActionParam() == AP_FIRE then

		if animDone == true then
			action:SetParam(1, "end")
			return false
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
				Act_Mon_DropProjectile_01_OnCastingCompleted(actor,action)
			
			end
		end
		
	
	end
	return true
end

function Act_Mon_DropProjectile_01_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(81190);
	return true;
end

function Act_Mon_DropProjectile_01_OnLeave(actor, action)
	ODS("Act_Mon_DropProjectile_01_OnLeave\n");
	return true;
end

function Act_Mon_DropProjectile_01_FireArrow(actor,action,fFireDelay)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	
	local	fRange = 100.0;
	local	kTargetCenterPos = action:GetParamAsPoint(0);	
	
	local	kStartPos = kTargetCenterPos
	kStartPos:SetZ(kStartPos:GetZ()+30)
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local	kFireDir = Point3(0,0,-1)
	local	kFireTargetPos = g_world:ThrowRay(kStartPos,kFireDir,1000)
	if kFireTargetPos:GetX() == -1 then
		return true
	end
	
	local	iSkillRange = action:GetSkillRange(0,actor)
	
	kStartPos:SetZ(kFireTargetPos:GetZ()+300)
	
	local	kProjectile = Act_Mon_DropProjectile_01_LoadToWeapon(actor,action,kStartPos);
	
	kProjectile:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp())

	kFireTargetPos:SetZ(kFireTargetPos:GetZ()+10)
	kProjectile:SetTargetLoc(kFireTargetPos)
	local effect = actor:GetAnimationInfo("EFFECT_TO_GROUND")
	if nil==effect then
		effect = "ef_woody_attk"
	end
	kProjectile:SetParamValue("EFFECT_TO_GROUND", effect)
	local qtime = actor:GetAnimationInfo("QUAKE_TIME")
	if nil==qtime then
		qtime = "0.5"
	end
	kProjectile:SetParamValue("QUAKE_TIME", qtime)
	local qfactor = actor:GetAnimationInfo("QUAKE_FACTOR")
	if nil==qfactor then
		qfactor = "2.1"
	end
	kProjectile:SetParamValue("QUAKE_FACTOR", qfactor)

	local kSoundID = actor:GetAnimationInfo("SOUND_ID")
	if nil~=kSoundID and ""~=kSoundID then
		kProjectile:SetParamValue("SOUND_ID", kSoundID)
	end

	kProjectile:Fire(true)
	
	return	true;
end

function Act_Mon_DropProjectile_01_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	ODS("Act_Mon_DropProjectile_01_OnEvent textKey:"..textKey.."\n");

	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	
	if textKey == "hit" or textKey == "fire" then

		Act_Mon_DropProjectile_01_FireArrow(actor,kAction,0);
		
		if kAction:GetTargetList():IsNil() == false then
			kAction:GetTargetList():ApplyActionEffects()
		end
		kAction:ClearTargetList();	
					
	end

	return	true;
end
