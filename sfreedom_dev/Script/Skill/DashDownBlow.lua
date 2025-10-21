-- Melee
function Skill_DashDownBlow_OnCastingCompleted(actor, action)
end

function Skill_DashDownBlow_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	if actor:IsMeetFloor() then
		return	false
	end
	
	if actor:GetFloatHeight(70)<70 then
		return	false;	
	end
	
	return		true;
	
end
function Skill_DashDownBlow_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local	actorID = actor:GetID()
--	local	actionID = action:GetID()
--	local	weapontype = actor:GetEquippedWeaponType();

	actor:StopJump();
	actor:FreeMove(true);
	
	action:SetParamFloat(2,500);	-- Drop Speed!
	actor:StartWeaponTrail();

	local iBaseClassID = actor:GetPilot():GetBaseClassID();
	if iBaseClassID == CT_FIGHTER then
		actor:AttachParticle(13, "char_root", "ef_jump_dash_attk_02_char_root");
	end
	
	return true
end
function Skill_DashDownBlow_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local iAttackRange = action:GetSkillRange(0,actor)
	
	if iAttackRange == 0 then
		iAttackRange = 70
	end
	
	local kParam = FindTargetParam();
	kParam:SetParam_1(actor:GetPos(),Point3(0,0,-1));
	kParam:SetParam_2(0,0,iAttackRange,0);
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN);
	
	return	action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
end

function Skill_DashDownBlow_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	animDone = actor:IsAnimationDone()
	local	slotNum = action:GetCurrentSlot();
	local	iState = action:GetParamInt(0)

	if iState == 0 then
		
		local	kLookingDir = actor:GetLookingDir();
		local	kRotateAxis = actor:GetLookingDir();
		kRotateAxis:Cross(Point3(0,0,1));
		kRotateAxis:Unitize();
		local	fRotateAngle = -40.0*3.141592/180.0;
		
		local	fVel = action:GetParamFloat(2)
		local	kMovingDelta = kLookingDir:_Multiply(fVel);
		kMovingDelta:Rotate(kRotateAxis,fRotateAngle);
		
		ODS("fVel : "..fVel.." kLookingDir:"..kLookingDir:GetX()..","..kLookingDir:GetY()..","..kLookingDir:GetZ().."\n");
		
		actor:SetMovingDelta(kMovingDelta)

		action:SetParamFloat(2, fVel+fVel*0.5*frameTime)
		
		if actor:IsMeetFloor() == true then
			Skill_DashDownBlow_OnMeetFloor(actor,action,accumTime);
		end

	elseif iState == 1 then	
	
		local	fElapsedTime = accumTime - action:GetParamFloat(5)
		if fElapsedTime>0.05 then
		
			actor:PlayNext();
			action:SetParamInt(0,2)
			return	true;
		
		end
	
	elseif iState == 2 then
		if animDone == true then
		
			if slotNum == 1 then
				local	curActionParam = action:GetParam(3)	
				if curActionParam == "GoNext" then
					nextActionName = action:GetParam(1);

					action:SetParam(3, "null")
					action:SetParam(1, "end")
					if nextActionName~="" then
						action:SetNextActionName(nextActionName)
					end
					
					return false;
				else
					action:SetParam(1, "end")
					return false
				end
			end
		
		end
	
	end
	

	return true
end
function Skill_DashDownBlow_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:FreeMove(false);
	actor:EndWeaponTrail();

	return true;
end
function Skill_DashDownBlow_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	curAction = actor:GetAction();
			
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local	curParam = curAction:GetParam(1)
	local	actionID = action:GetID()
	
	ODS("Skill_DashDownBlow_OnLeave actionID:"..actionID.."\n");
	
	if actor:IsMyActor() == false then
		return true;
	end

	if actionID == "a_jump" then 
		return false;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end

	if curParam == "end" then 
		return true;
	end	

	if action:GetEnable() == false then
		
		if curParam==actionID then
			curAction:SetParam(1,"");
		end
		
	end	
	
	if action:GetEnable() == true then
		
		if actionID == "a_run" or 
			string.sub(actionID,1,7) == "a_melee" or 
			string.sub(actionID,1,11) == "a_thi_melee" then
	
			curAction:SetParam(3,"GoNext");
			curAction:SetParam(1,actionID);	
			
		elseif actionID == "a_telejump" then
			actor:DetachFrom(32)
			return true
		end
	
		return false;
	
	elseif curParam == "end" and actionID == "a_run" then
		return true;
	elseif actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Skill_DashDownBlow_DoDamage(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

	local	iTargetCount = action:GetTargetCount();
	action:SetSlot(1);	
	
	if 	iTargetCount == 0 then
		return true;
	end

	SkillHelpFunc_DefaultHitOneTime(actor,action, true);
	actor:ClearIgnoreEffectList();
end

function Skill_DashDownBlow_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
    
        if actor:IsMyActor()  then
			action:GetTargetList():ApplyActionEffects(true,true);
			return;
		end

        if action:GetParam(6) == "HIT" then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.

            Skill_DashDownBlow_DoDamage(actor,action);
        
        end        
    
    end

end

function Skill_DashDownBlow_OnMeetFloor(actor,action,accumTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(6,"HIT");
	action:SetParamInt(0,1);	--	 Stop Drop!
	actor:FindPathNormal();
	action:SetParamFloat(5,accumTime);		

	actor:FreeMove(false);
	
	if actor:IsMyActor() then
		QuakeCamera(g_fMeleeDropQuakeValue[1]
		, g_fMeleeDropQuakeValue[2]
		, g_fMeleeDropQuakeValue[3]
		, g_fMeleeDropQuakeValue[4]
		, g_fMeleeDropQuakeValue[5])
	end
	
	local	kPos = actor:GetPos();
	
	if actor:GetPilot():GetBaseClassID() == CT_FIGHTER then
		
		kPos = actor:GetLookingDir();
		local	fDistance = 50;
		kPos:Multiply(fDistance);
		kPos:Add(actor:GetPos());
		
	end

	actor:AttachParticleToPoint(12, kPos, "ef_thi_jump_down_attk")
	
	if actor:IsUnderMyControl() then
	
		action:CreateActionTargetList(actor);
		if IsSingleMode() then
			Act_Melee_OnTargetListModified(actor,action,false)
		else
			action:BroadCastTargetListModify(actor:GetPilot());
		end			
	
	end
	
	
	Skill_DashDownBlow_DoDamage(actor,action);
	
end

