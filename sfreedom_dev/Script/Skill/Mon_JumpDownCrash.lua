-- skill
function Skill_Mon_JumpDownCrash_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	ODS("Skill_Mon_JumpDownCrash_OnEnter ActionParam : "..action:GetActionParam().."\n");
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
			ODS("[SkillError] Skill_Mon_JumpDownCrash_OnEnter, kPacket is not valid. actor : "..actor:GetPilotGuid():GetString().." action:"..action:GetID().."\n");
			ptTargetPos = actor:GetPos()
		else
			ptTargetPos = Point3(kPacket:PopFloat(),kPacket:PopFloat(),kPacket:PopFloat()+30);
			ptTargetPos = g_world:ThrowRay(ptTargetPos,Point3(0,0,-1),500)
			ptTargetPos:SetZ(ptTargetPos:GetZ()+25)
			ODS("Skill_Mon_JumpDownCrash_OnEnter CurrentPos : "..actor:GetPos():GetX()..","..actor:GetPos():GetY()..","..actor:GetPos():GetZ().."\n");
			ODS("Skill_Mon_JumpDownCrash_OnEnter  TargetPos : "..ptTargetPos:GetX()..","..ptTargetPos:GetY()..","..ptTargetPos:GetZ().."\n");
		end
		
	end
	
	local	kSkillDef = GetSkillDef(action:GetActionNo())

	local kScale = 100.0
	if nil~=kSkillDef and false==kSkillDef:IsNil() then
		kScale = kSkillDef:GetAbil(AT_2ND_AREA_PARAM1)*2	--반지름이라서
		if 0==kScale then
			kScale = 100.0
		end
	end

	kScale = kScale*0.01

	local Effect = actor:GetAnimationInfoFromAction("GROUND_EFFECT",action)
	if nil==Effect or ""==Effect then
		Effect = "ef_dmg_ground_01"
	end	

	if "NONE"~=Effect then
		actor:AttachParticleToPointS(81190,ptTargetPos,Effect,kScale)
	end
	
	-- Look at targetpos
	actor:LookAt(ptTargetPos,true,false);
	
	-- Save the targetpos
	action:SetParamAsPoint(0,ptTargetPos);

	action:SetParamInt(0,0)	--	State
	action:SetParamFloat(1,g_world:GetAccumTime());	--	Start Time
	action:SetParamAsPoint(1,actor:GetPos());	--	Start Pos	
	return true
end


function Skill_Mon_JumpDownCrash_OnCastingCompleted(actor,action)	
	ODS("Skill_Mon_JumpDownCrash_OnCastingCompleted\n");
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	action:SetSlot(1);
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	
	Skill_Mon_JumpDownCrash_Fire(actor,action);
	
end

function Skill_Mon_JumpDownCrash_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	-- now, start to fly to the sky!!	

	actor:SetCanHit(false);

	local	fFlyHeight = 300
	
	Skill_Mon_JumpDownCrash_SetState(actor,action,3)

	local	kTargetPos = action:GetParamAsPoint(0)
	local	kMovingVec = Point3(0,0,1)
	local	fMovingDist = fFlyHeight;
	kMovingVec:Multiply(fMovingDist);
	
	local	kNewPos = kTargetPos:_Add(kMovingVec);
	
	actor:SetTranslate(kNewPos);
end

function Skill_Mon_JumpDownCrash_SetState(actor,action,iState)

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
			fireEffect = "ef_sambhava_skill_03_char_root"
		end
		actor:AttachParticleToPoint(28713,actor:GetPos(),fireEffect);

		QuakeCamera(0.5,5,0,0.1,5);
		actor:SetCanHit(true)
		
		local	kSoundID = actor:GetAnimationInfo("SOUND_ID")
		if nil~=kSoundID and kSoundID~="" then
			actor:AttachSound(2784,kSoundID)
		end	

		Act_Mon_Melee_DoDamage(actor,action);
	end	

end


function Skill_Mon_JumpDownCrash_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	fTotalFlyToSkyTime = 0.1
	local	iState = action:GetParamInt(0);
	local	fFlyHeight = 300
	local	fElapsedTime = accumTime - action:GetParamFloat(1)
	

	if action:GetActionParam() == AP_FIRE then

		if iState == 3 then -- fly to the ground
		
			local	fRate  = fElapsedTime/fTotalFlyToSkyTime;
			
			if fRate>1.0 then
				fRate = 1.0
			end
			
			local	kStartPos = action:GetParamAsPoint(0)
			local	kMovingVec = Point3(0,0,1)
			local	fMovingDist = (1.0 - fRate)*fFlyHeight;
			kMovingVec:Multiply(fMovingDist);
			
			local	kNewPos = kStartPos:_Add(kMovingVec);
			
			actor:SetTranslate(kNewPos);
			
			if fRate == 1.0 then
				Skill_Mon_JumpDownCrash_SetState(actor,action,4)
			end
		elseif iState == 4 then -- stay with the pose while sometimes
			if fElapsedTime >= 1.0 then
				Skill_Mon_JumpDownCrash_SetState(actor,action,5)
				
				actor:PlayNext();		
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
			
			if fRate>1.0 then
				fRate = 1.0
			end
			
			local	kStartPos = action:GetParamAsPoint(1)
			local	kMovingVec = Point3(0,0,1)
			local	fMovingDist = fRate*fFlyHeight;
			kMovingVec:Multiply(fMovingDist);
			
			local	kNewPos = kStartPos:_Add(kMovingVec);
			
			actor:SetTranslate(kNewPos);
			
			if fRate == 1.0 then
			
				Skill_Mon_JumpDownCrash_SetState(actor,action,2)
				actor:PlayNext();
			
			end
		
		elseif iState == 2 then -- Wait for the packet from server
		
			if IsSingleMode() then
			
				if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then

					action:SetActionParam(AP_FIRE);
					Skill_Mon_JumpDownCrash_OnCastingCompleted(actor,action)
				
				end
			
			end
		end
		
	
	end	
	return true
end

function Skill_Mon_JumpDownCrash_OnCleanUp(actor, action)
	
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

function Skill_Mon_JumpDownCrash_OnLeave(actor, action)
	
	return	true;
	
end

function Skill_Mon_JumpDownCrash_OnEvent(actor, textkey)
		
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	ODS("Skill_Mon_JumpDownCrash_OnEvent textkey:"..textkey.."\n");
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if action:GetParamInt(0) == 0 then
		if textkey == "fly" or textkey == "end" then
		
			QuakeCamera(g_fMeleeDropQuakeValue[1]
			, g_fMeleeDropQuakeValue[2]
			, g_fMeleeDropQuakeValue[3]
			, g_fMeleeDropQuakeValue[4]
			, g_fMeleeDropQuakeValue[5])	
			
			Skill_Mon_JumpDownCrash_SetState(actor,action,1)
			
		end
	end
	
	return	true;
	
end

function Act_Mon_JumpDownCrash_OnTargetListModified(actor,action,bIsBefore)
end
