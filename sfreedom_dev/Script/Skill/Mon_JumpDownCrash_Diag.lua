function FlyHeight(action)	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	return action:GetAbil(AT_JUMP_HEIGHT)
end

-- skill
function Skill_Mon_JumpDownCrash_Diag_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	ODS("Skill_Mon_JumpDownCrash_Diag_OnEnter ActionParam : "..action:GetActionParam().."\n");
	actor:FreeMove(true);	
	actor:StopJump();
	actor:SetNoWalkingTarget(false);
	actor:ResetAnimation();
	
	
	--	attack target pos
	local	ptTargetPos = 0;
	
	if IsSingleMode() then
		ptTargetPos = GetMyActor():GetPos();
	else
		local	kPacket = action:GetParamAsPacket();
		if kPacket == nil or kPacket:IsNil() then
			ODS("[SkillError] Skill_Mon_JumpDownCrash_Diag_OnEnter, kPacket is not valid. actor : "..actor:GetPilotGuid():GetString().." action:"..action:GetID().."\n");
			ptTargetPos = actor:GetPos()
		else
			ptTargetPos = Point3(kPacket:PopFloat(),kPacket:PopFloat(),kPacket:PopFloat()+30);
			ptTargetPos = g_world:ThrowRay(ptTargetPos,Point3(0,0,-1),500)
			ptTargetPos:SetZ(ptTargetPos:GetZ()+25)
			ODS("Skill_Mon_JumpDownCrash_Diag_OnEnter CurrentPos : "..actor:GetPos():GetX()..","..actor:GetPos():GetY()..","..actor:GetPos():GetZ().."\n");
			ODS("Skill_Mon_JumpDownCrash_Diag_OnEnter  TargetPos : "..ptTargetPos:GetX()..","..ptTargetPos:GetY()..","..ptTargetPos:GetZ().."\n");
		end
		
	end
	
	local	kSkillDef = GetSkillDef(action:GetActionNo())

	local kScale = 100.0
	if nil~=kSkillDef and false==kSkillDef:IsNil() then
		kScale = kSkillDef:GetAbil(AT_2ND_AREA_PARAM1)*2	--반지름이라서. 먼저 효과거리부터 체크
		if 0==kScale then
			kScale = action:GetSkillRange(0, actor)--kSkillDef:GetAbil(AT_2ND_AREA_PARAM1)*2	--반지름이라서. 없으면
			if 0==kScale then
				kScale = 100.0
			end
		end
	end

	kScale = kScale*0.01

	local Effect = actor:GetAnimationInfoFromAction("GROUND_EFFECT",action)
	if nil==Effect or ""==Effect then
		Effect = "ef_Blizzard_mon_01"
	end

	if "NONE"~=Effect then
		local EffectScale = actor:GetAnimationInfoFromAction("GROUND_EFFECT_SCALE",action)
		if nil==EffectScale or ""==EffectScale then
			EffectScale = 1.0
		else
			EffectScale = tonumber(EffectScale)
		end
	
		actor:AttachParticleToPointS(81190,ptTargetPos,Effect,kScale*EffectScale)
	end
	
	-- Look at targetpos
	actor:LookAt(ptTargetPos,true,false);
	
	-- Save the targetpos
	action:SetParamAsPoint(0,ptTargetPos);

	if actor:HaveAnimationTextKey("fly",action:GetSlotAnimName(0)) then
		action:SetParamInt(0,0)	--	State
	else
		action:SetParamInt(0,1)	--	State
	end
	
	
	action:SetParamFloat(1,g_world:GetAccumTime());	--	Start Time
	action:SetParamAsPoint(1,actor:GetPos());	--	Start Pos	

	local ptNow = actor:GetPos()
	ptNow:SetZ(ptNow:GetZ()+FlyHeight(action))
	local Dist = ptTargetPos:Distance(ptNow)
	action:SetParamFloat(3, Dist)
	local Dir = ptTargetPos:_Subtract(ptNow)
	Dir:Unitize()
	action:SetParamAsPoint(3, Dir)	--내리찍을 방향

	local CastTime = actor:GetAnimationInfoFromAction("IS_CAST_TIME",action)
	if nil==CastTime or ""==CastTime or "FALSE"==CastTime then
		fTotalFlyToSkyTime = actor:GetAnimationLength(action:GetSlotAnimName(0))
	else
		fTotalFlyToSkyTime = action:GetAbil(AT_CAST_TIME) / 1000
	end
	
	action:SetParamFloat(5, fTotalFlyToSkyTime)
	return true
end


function Skill_Mon_JumpDownCrash_Diag_OnCastingCompleted(actor,action)	
	ODS("Skill_Mon_JumpDownCrash_Diag_OnCastingCompleted\n");
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	action:SetSlot(1);
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	
	action:SetParamFloat(5, 0.1)

	Skill_Mon_JumpDownCrash_Diag_Fire(actor,action);
	
end

function Skill_Mon_JumpDownCrash_Diag_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	-- now, start to fly to the sky!!	

	actor:SetCanHit(false);
	
	Skill_Mon_JumpDownCrash_Diag_SetState(actor,action,3)

	local	kTargetPos = action:GetParamAsPoint(1)
	kTargetPos:SetZ(kTargetPos:GetZ()+FlyHeight(action))
	actor:SetTranslate(kTargetPos);
end

function Skill_Mon_JumpDownCrash_Diag_SetState(actor,action,iState)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	action:SetParamInt(0,iState);
	action:SetParamFloat(1,g_world:GetAccumTime());
	

	if iState == 4 then
	
		actor:DetachFrom(81190);
		local fireEffect = actor:GetAnimationInfoFromAction("FIRE_EFFECT", action)
		if nil==fireEffect or ""==fireEffect then
			fireEffect = "ef_Blizzard_mon_02"
		end
		if "NONE"~=fireEffect then
			actor:AttachParticleToPoint(28713,actor:GetPos(),fireEffect);
		end

		QuakeCamera(0.5,5,0,0.1,5);
		actor:SetCanHit(true)
		
		local	kSoundID = actor:GetAnimationInfo("SOUND_ID")
		if nil~=kSoundID and kSoundID~="" then
			actor:AttachSound(2784,kSoundID)
		end	

		Act_Mon_Melee_DoDamage(actor,action);
	elseif iState == 5 then
		action:SetSlot(2);
		actor:PlayCurrentSlot();
	end	

end


function Skill_Mon_JumpDownCrash_Diag_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	fTotalFlyToSkyTime = action:GetParamFloat(5)
	local	iState = action:GetParamInt(0);
	local	fFlyHeight = FlyHeight(action)
	local	fElapsedTime = accumTime - action:GetParamFloat(1)
	

	if action:GetActionParam() == AP_FIRE then

		if iState == 3 then -- fly to the ground
		
			local	fRate  = fElapsedTime/fTotalFlyToSkyTime;
			
			if fRate>1.0 then
				fRate = 1.0
			end

			local	kStartPos = action:GetParamAsPoint(1)
			kStartPos:SetZ(kStartPos:GetZ()+fFlyHeight)
			local	kMovingVec = action:GetParamAsPoint(3)
			local	fMovingDist = (fRate)*action:GetParamFloat(3);
			kMovingVec:Multiply(fMovingDist);
			
			local	kNewPos = kStartPos:_Add(kMovingVec);
			
			actor:SetTranslate(kNewPos);
			
			if fRate == 1.0 then
				Skill_Mon_JumpDownCrash_Diag_SetState(actor,action,4)
			end
		elseif iState == 4 then -- stay with the pose while sometimes
			local delay_down = actor:GetAnimationInfo("DELAY_DOWN")
			if nil==delay_down then
				delay_down = 0
			else
				delay_down = tonumber(delay_down) / 1000
			end

			if fElapsedTime >= delay_down then
				Skill_Mon_JumpDownCrash_Diag_SetState(actor,action,5)
			end
		elseif iState == 5 then -- now, get up, and finish the skill
			local	kTargetPos = action:GetParamAsPoint(0)
			actor:SetTranslate(kTargetPos);		
			
			if actor:IsAnimationDone() then
			
				return	false;
			
			end
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		if iState == 0 then	--	wait for 'fly' text key event

		elseif iState == 1 then	--	fly to the sky

			local	fRate  = fElapsedTime/fTotalFlyToSkyTime;
			if fRate > 1.0 then
			
				Skill_Mon_JumpDownCrash_Diag_SetState(actor,action,2)
				actor:PlayNext();
			
			else
			
				local	kStartPos = action:GetParamAsPoint(1)
				local	kMovingVec = Point3(0,0,1)
				local	fMovingDist = fRate*fFlyHeight;
				kMovingVec:Multiply(fMovingDist);
				
				local	kNewPos = kStartPos:_Add(kMovingVec);
				
				actor:SetTranslate(kNewPos);

			end
		
		elseif iState == 2 then -- Wait for the packet from server
		
			if IsSingleMode() then
			
				if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then

					action:SetActionParam(AP_FIRE);
					Skill_Mon_JumpDownCrash_Diag_OnCastingCompleted(actor,action)
				
				end
			
			end
		end
		
	
	end	
	return true
end

function Skill_Mon_JumpDownCrash_Diag_OnCleanUp(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	local	kAction = actor:GetAction()
	if( CheckNil(nil==kAction) ) then return end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if kAction:GetParamInt(0) <4 then
		local	kTargetPos = kAction:GetParamAsPoint(0)
		actor:SetTranslate(kTargetPos);
	end
	
	actor:DetachFrom(81190);
	actor:SetCanHit(true);
	actor:FreeMove(false);
end

function Skill_Mon_JumpDownCrash_Diag_OnLeave(actor, action)
	
	return	true;
	
end

function Skill_Mon_JumpDownCrash_Diag_OnEvent(actor, textkey)
		
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	ODS("Skill_Mon_JumpDownCrash_Diag_OnEvent textkey:"..textkey.."\n");
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if action:GetParamInt(0) == 0 then
		if textkey == "fly" then
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			action:SetParamFloat(1,g_world:GetAccumTime())
		end

		if textkey == "fly" or textkey == "end" then
		
			QuakeCamera(g_fMeleeDropQuakeValue[1]
			, g_fMeleeDropQuakeValue[2]
			, g_fMeleeDropQuakeValue[3]
			, g_fMeleeDropQuakeValue[4]
			, g_fMeleeDropQuakeValue[5])	
			
			Skill_Mon_JumpDownCrash_Diag_SetState(actor,action,1)
			
		end
	end
	
	return	true;
	
end

function Act_Mon_JumpDownCrash_Diag_OnTargetListModified(actor,action,bIsBefore)
end
