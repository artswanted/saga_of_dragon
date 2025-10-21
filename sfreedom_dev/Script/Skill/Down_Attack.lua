-- [EFFECT_ID] : 타겟 위치에 붙일 이펙트 ID
-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)

function Skill_Down_Attack_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if false == actor:IsMeetFloor() then
		return	false
	end
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end


function Skill_Down_Attack_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iBaseActorType = actor:GetPilot():GetBaseClassID()

	ODS("Skill_Down_Attack_OnEnter actionID: "..action:GetID().."\n");

--	local	actorID = actor:GetID()
--	local	actionID = action:GetID()
--	local	weapontype = actor:GetEquippedWeaponType();

	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
	
	if bUseTLM == false then
		action:CreateActionTargetList(actor);
	end
	
	if iBaseActorType == CT_FIGHTER or iBaseActorType == CT_THIEF then
		actor:StartWeaponTrail();
	end
	
	action:SetParamInt(0,0);
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());	
	end	
	
	actor:SetComboCount(0);
	return true
end
function Skill_Down_Attack_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	return	Skill_Down_Attack_FindTargetEx(actor,action,action:GetActionNo(),action:GetSkillRange(0,actor),kTargets);
end
function Skill_Down_Attack_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kTargetPos = actor:GetLookingDir();
	kTargetPos:Multiply(iAttackRange);
	kTargetPos:Add(actor:GetPos());
	kTargetPos:SetZ(kTargetPos:GetZ()-20);

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = iAttackRange
	end	
			
	local kParam = FindTargetParam();
	kParam:SetParam_1(kTargetPos,actor:GetLookingDir());
	kParam:SetParam_2(0,30,iParam2,0);
	kParam:SetParam_3(true,FTO_DOWN);
	
	return	action:FindTargetsEx(iActionNo,TAT_SPHERE,kParam,kTargets,kTargets);
end
function Skill_Down_Attack_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	animDone = actor:IsAnimationDone()
--	local	slotNum = action:GetCurrentSlot();
	
	if animDone == true then
		
		ODS("Skill_Down_Attack_OnUpdate animDone Param : "..action:GetParam(1).."\n");
		
		actor:SetParam("LAST_DOWN_ATTACK_TIME",""..accumTime);
		
		if action:GetParam(1) == "GoNext" then
			action:SetNextActionName(action:GetParam(0));
		end

		action:SetParam(1,"end");
		
		return	false;
	end

	return true
end
function Skill_Down_Attack_OnCleanUp(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:EndWeaponTrail();
	ODS("Skill_Down_Attack_OnCleanUp NextAction : "..action:GetID().."\n");
	
	return true;
end
function Skill_Down_Attack_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local	curParam = curAction:GetParam(1)
	local	actionID = action:GetID()
	if action:GetEnable() then
		ODS("Skill_Down_Attack_OnLeave NextAction : "..actionID.."\n");
	end
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end

	if curParam == "end" then 
		return true;
	end	
	
	if action:GetEnable() == true then
		
		if curParam == "HIT" then
			if action:GetID() == "a_jump" then
				return	true
			end
			curAction:SetNextActionName(actionID);
			curAction:SetNextActionInputSlotInfo(action:GetInputSlotInfo());
		end

		return false;
	elseif curParam == "end" and (actionID == "a_right_run" or actionID == "a_left_run") then
		return true;
	elseif 	actionID == "a_level_up" or
		actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Skill_Down_Attack_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if action:GetParam(1) == "HIT" then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            --if actor:IsMyActor()  then
				--action:GetTargetList():ApplyActionEffects(true,true);
            --else
                SkillHelpFunc_DefaultHitOneTime(actor,action, false);
            --end
        
        end        
    
    end

end

function Skill_Down_Attack_OnEvent(actor,textKey,seqID)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	ODS("Skill_Melee_Knock_Back_OnEvent textKey:"..textKey.."\n");

	if textKey == "hit" then
		
		local kAction = actor:GetAction();
		
		CheckNil(nil==kAction);
		CheckNil(kAction:IsNil());
	
		local	kActionID = kAction:GetID();
		
		if actor:IsUnderMyControl() then
		
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
			if bUseTLM then
			
				kAction:CreateActionTargetList(actor);
				if IsSingleMode() then
					Skill_Down_Attack_OnTargetListModified(actor,kAction,false)
				else
					kAction:BroadCastTargetListModify(actor:GetPilot());
				end			
			
			end		
			
		end
		kAction:SetParam(1,"HIT")
		
		local	iBaseClassID = actor:GetPilot():GetBaseClassID();
		
		
		if kActionID == "a_down_attack_fly" then
			actor:AttachParticle(31,"char_root","edge_swd_01_09");		
		end
		
		if actor:IsMyActor() and kAction:GetTargetCount()>0 then
		
			if kActionID == "a_down_attack_fly" or
				kActionID == "a_arc_down_attack_fly" or
				kActionID == "a_mag_down_attack_fly" or
				kActionID == "a_thi_down_attack_fly" then

				QuakeCamera(g_fBlowUpQuakeValue[1]
				, g_fBlowUpQuakeValue[2]
				, g_fBlowUpQuakeValue[3]
				, g_fBlowUpQuakeValue[4]
				, g_fBlowUpQuakeValue[5])		
				
			end
		
		end
		
		
		local	kActorID = actor:GetID();
		
		local	fFrontMoveRange = 10;
		local	kTargetPos = actor:GetLookingDir();
		kTargetPos:Multiply(fFrontMoveRange);
		kTargetPos:Add(actor:GetPos());
		kTargetPos:SetZ(kTargetPos:GetZ()-20);
		
		ODS("kActorID : "..kActorID.."\n");
		
		if kActorID == "c_thief_male" or kActorID == "c_thief_female" then
			actor:AttachParticleToPoint(10,kTargetPos,"ef_thi_down_attk");			
		elseif kActorID == "c_magician_male" or kActorID == "c_magician_female" then
			if "a_mag_down_attack_fly" == kAction:GetID() then
				kTargetPos = actor:GetLookingDir();
				kTargetPos:Multiply(kAction:GetSkillRange(0,actor));
				kTargetPos:Add(actor:GetPos());
				actor:AttachParticleToPoint(10,kTargetPos,"ef_mag_attk_down_fly_01_char_root");
			end		
		elseif kActorID == "c_archer_male" or kActorID == "c_archer_female" then
			if "a_arc_down_attack_fly" == kAction:GetID() then
				actor:AttachParticle(10, "char_root","ef_attk_down_fly_01_char_root");
			end
		end
	
	
	--	if actor:IsMyActor() then
	--		QuakeCamera(g_fDownAttackQuakeValue[1]
	--		, g_fDownAttackQuakeValue[2]
	--		, g_fDownAttackQuakeValue[3]
	--		, g_fDownAttackQuakeValue[4]
	--		, g_fDownAttackQuakeValue[5])
	--	end

	end

	return	true;
end

function Skill_Down_Attack_OnCastingCompleted(actor, action)
end