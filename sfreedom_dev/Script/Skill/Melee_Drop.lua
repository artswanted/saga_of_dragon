-- Melee
-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)

use_cam_effect = false;
function Skill_Melee_Drop_OnCastingCompleted(actor, action)
end

function Skill_Melee_Drop_OnCheckCanEnter(actor,action)

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
function Skill_Melee_Drop_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

--	local	actorID = actor:GetID()
--	local	actionID = action:GetID()
--	local	weapontype = actor:GetEquippedWeaponType();
	

	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
	
	if bUseTLM == false then
		action:CreateActionTargetList(actor);
	end
	
	--actor:StartWeaponTrail();
	action:SetParamInt(0,0);
	
	-- 카메라 처리
	if use_cam_effect == true then
	
			local	trn = GetCameraTrn()
			local	rot = GetCameraRot()
			trn:SetY(trn:GetY() - 80)
			trn:SetZ(trn:GetZ() - 120)
			if actor:IsToLeft() == true then
				rot:SetX(-90)
			else
				rot:SetX(90)
			end
			rot:SetZ(rot:GetZ() + 20)
			SetCameraSetting(trn, rot)
	
	end
	
	actor:StopJump();
	actor:FreeMove(true);
	
	action:SetParamInt(4,0);
	
	action:SetParamFloat(2,-400);	-- Drop Speed!
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());	
	end
	
	return true
end
function Skill_Melee_Drop_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local iAttackRange = action:GetSkillRange(0,actor)
	
	local	kTargetPos = actor:GetLookingDir();
	kTargetPos:Multiply(iAttackRange);
	kTargetPos:Add(actor:GetPos());
	
	local actorID = actor:GetID();
	if actorID == "c_thief_male" or actorID == "c_thief_female" then
		kTargetPos = actor:GetPos();
	end

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = iAttackRange
	end

	local kParam = FindTargetParam();
	kParam:SetParam_1(kTargetPos,Point3(0,0,-1));
	kParam:SetParam_2(0,30,iParam2,0);
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN+FTO_BLOWUP);
	
	return	action:FindTargets(TAT_RAY_DIR_COLL_PATH,kParam,kTargets,kTargets);
end

function Skill_Melee_Drop_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

--	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	animDone = actor:IsAnimationDone()
	local	slotNum = action:GetCurrentSlot();
	local	iState = action:GetParamInt(4);
	
	ODS("iState : "..iState.." Param[0] : "..action:GetParamInt(0).." slotNum:"..slotNum.." curActionParam:"..action:GetParam(3).."\n");
	if animDone then 
	
	    ODS("Skill_Melee_Drop_OnUpdate animDone\n");
	
	end
	
	if iState == 0 then
		
		if animDone == true then
		
			if slotNum == 1 or slotNum == 2 then
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
		
		if action:GetParamInt(0) == 1 then
			
			local	fVel = action:GetParamFloat(2)
			actor:SetMovingDelta(Point3(0,0,fVel))

			action:SetParamFloat(2, fVel+ fVel * 10 * frameTime )
			
			if actor:IsMeetFloor() == true then
				Skill_Melee_Drop_OnMeetFloor(actor,action,accumTime);
			end
		end
		
	else
	
		local	fElapsedTime = accumTime - action:GetParamFloat(5);
		if fElapsedTime>=g_fMeleeDropFreezeTime then
			actor:PlayCurrentSlot();
			action:SetParamInt(4,0);
		end		
		
	end
	return true
end
function Skill_Melee_Drop_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	actor:FreeMove(false);

	actor:SetAnimOriginalSpeed()
	--actor:EndWeaponTrail();
	return true;
end
function Skill_Melee_Drop_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	curAction = actor:GetAction();
	local	curParam = curAction:GetParam(1)
	local	actionID = action:GetID()
	
	ODS("Skill_Melee_Drop_OnLeave actionID:"..actionID.."\n");
	
	if actor:IsMyActor() == false then
		return true;
	end

	if actionID == "a_jump" then 
        if curAction:GetParam(6) == "HIT" then
			return	true;
		end
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
			
		elseif actionID == "a_telejump" or 
			actionID == "a_teleport" then
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

function Skill_Melee_Drop_DoDamage(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(4,1);
	action:SetParamFloat(5,accumTime);		

	local	iTargetCount = action:GetTargetCount();
	action:SetSlot(1);	
	
	if 	iTargetCount == 0 then
		return true;
	end
	SkillHelpFunc_DefaultHitOneTime(actor,action, false);
end

function Skill_Melee_Drop_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
        if action:GetParam(6) == "HIT" then
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            --if actor:IsMyActor()  then
				--action:GetTargetList():ApplyActionEffects(true,true);
            --else
                Skill_Melee_Drop_DoDamage(actor,action);
            --end
        end
    end
end

function Skill_Melee_Drop_OnMeetFloor(actor,action,accumTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(6,"HIT");
	action:SetParamInt(0,2);	--	 Stop Drop!
	actor:FindPathNormal();
	
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
	
		local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");
		
		if bUseTLM then
		
			action:CreateActionTargetList(actor);
			if IsSingleMode() then
				Act_Melee_OnTargetListModified(actor,action,false)
			else
				action:BroadCastTargetListModify(actor:GetPilot());
			end			
		
		end	
		
	end
	
	action:SetSlot(1);		
end


function Skill_Melee_Drop_OnEvent(actor,textKey,seqID)

    ODS("Skill_Melee_Drop_OnEvent textKey:"..textKey.."\n");
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local	action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if (textKey == "drop" or textKey == "end") and action:GetParamInt(0) == 0 then
		
		if actor:GetPilot():GetBaseClassID() == CT_FIGHTER then
			
			actor:AttachParticle(32,"char_root","edge_swd_01_08");
		
		end	
			
--		local	actorID = actor:GetID();
	
		action:SetParamInt(0,1);		--	Start Drop!
		action:SetParamFloat(2,-700);	-- Drop Speed!
		
	end

	if textKey == "hit" or textKey == "fire" then
		local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end
	end

	return	true;
end
