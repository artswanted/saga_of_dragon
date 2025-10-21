-- Melee
-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)

function Skill_Melee_Blow_Up_OnCastingCompleted(actor, action)
end

function Skill_Melee_Blow_Up_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());	
	
	local	iBaseActorType = actor:GetPilot():GetBaseClassID()

    if actor:IsMyActor() == false then
        return true
    end
    
    local	kCurAction = actor:GetAction()
	if( false == kCurAction:IsNil() ) then
		if kCurAction:GetID() == "a_dash" then
			if actor:GetPilot():IsCorrectClass(CT_WARRIOR) then
				actor:ReserveTransitAction("a_dash_blowup");
				return	false;			
			end
		end
    end
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
		return	false;
	end	
	
	return		true;
	
end
function Skill_Melee_Blow_Up_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iBaseActorType = actor:GetPilot():GetBaseClassID()

--	local	actorID = actor:GetID()
--	local	actionID = action:GetID()
--	local	weapontype = actor:GetEquippedWeaponType();
	
	--ODS("Skill_Melee_Blow_Up_OnEnter\n", false, 3851);
	
	if actor:IsMyActor() == true then

		actor:SetComboCount(0);

		if iBaseActorType == CT_FIGHTER then
			actor:SetNormalAttackActionID("a_melee_01");
		elseif iBaseActorType == CT_THIEF then
			actor:SetNormalAttackActionID("a_thi_melee_01");
		elseif iBaseActorType == CT_SHAMAN then
			--actor:SetNormalAttackActionID("a_Sum_Rifle_Shot");
		end
		
		GetComboAdvisor():OnNewActionEnter(action:GetID());
		GetComboAdvisor():AddNextAction("a_jump");
				
	end
	
	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
	
	if bUseTLM == false then
		action:CreateActionTargetList(actor);
	end

	action:SetParam(2,"");

    local	iBaseActorType = actor:GetPilot():GetBaseClassID()
	
	if actor:GetPilot():GetBaseClassID() == CT_SHAMAN then			
		local	kMovingVec = actor:GetLookingDir();
		local	fDistance = -action:GetAbil(AT_SKILL_BACK_DISTANCE) +40
		
		kMovingVec:Multiply(fDistance);
		kMovingVec:Add(actor:GetNodeWorldPos("char_root"))
		actor:AttachParticleToPoint(9,kMovingVec,"eff_sum_skill_smn_ground_piston_01");	
	end
	
	return true
end
function Skill_Melee_Blow_Up_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kPos = actor:GetPos();
--	local	kDir = actor:GetLookingDir();
--
	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = g_fMeleeBlowUpAttackAngle
	end
	
		
	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(action:GetSkillRange(0,actor),iParam2,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);

	return	action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
end

function Skill_Melee_Blow_Up_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());		
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local iMaxActionSlot = action:GetSlotCount();
--	local weapontype = actor:GetEquippedWeaponType();
--	local actionID = action:GetID();

	if animDone == true then
		action:SetParam(1, "end")
		return false
	end
	return true
end
function Skill_Melee_Blow_Up_OnCleanUp(actor)
	ODS("Skill_Melee_Blow_Up_ReallyLeave\n");
	
	return true;
end
function Skill_Melee_Blow_Up_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iBaseActorType = actor:GetPilot():GetBaseClassID()

	local	curAction = actor:GetAction();
	local	curParam = curAction:GetParam(1)
	local	actionID = action:GetID()

	if actor:IsMyActor() == false then
		return true;
	end
	
	if actionID == "a_jump" then 
		if curAction:GetParam(2) == "HIT" then
			return true;
		end
		curAction:SetParam(3,"a_jump");
		return	false;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end
		
	if action:GetEnable() == true then
		
		if curParam == "end" then 
			return true;
		end
		
		return false;
	
	elseif curParam == "end" and actionID == "a_run" then
		return true;
	elseif actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		
		if iBaseActorType == CT_FIGHTER then
			actor:SetNormalAttackActionID("a_melee_01");
		elseif iBaseActorType == CT_THIEF then
			actor:SetNormalAttackActionID("a_thi_melee_01");
		elseif iBaseActorType == CT_SHAMAN then
			--actor:SetNormalAttackActionID("a_Sum_Rifle_Shot");
		end
		
		return true;
	end

	return false 
end
function Skill_Melee_Blow_Up_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if action:GetParam(2) == "HIT" then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            --if actor:IsUnderMyControl()  then
				--action:GetTargetList():ApplyActionEffects(true,true);
            --else
                SkillHelpFunc_DefaultHitOneTime(actor,action, false);

            --end
        
        end        
    
    end

end
function Skill_Melee_Blow_Up_OnEvent(actor,textKey,seqID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	
	ODS("Skill_Melee_Blow_Up_OnEvent textKey:"..textKey.."\n");

	if textKey == "hit" then
	
		if actor:IsMyActor() then
			QuakeCamera(g_fBlowUpQuakeValue[1]
			, g_fBlowUpQuakeValue[2]
			, g_fBlowUpQuakeValue[3]
			, g_fBlowUpQuakeValue[4]
			, g_fBlowUpQuakeValue[5])
		end
		
		SkillFunc_QuakeCamera(actor:GetAction(), 6)		 --Hit 메세지때 quake camera를 쓰는가
		if actor:GetPilot():GetBaseClassID() == CT_FIGHTER then
			actor:AttachParticle(31,"char_root","edge_swd_01_05");
		elseif  actor:GetPilot():GetBaseClassID() == CT_THIEF and false == actor:CheckStatusEffectExist("se_transform_to_metamorphosis") and false == actor:CheckStatusEffectExist("se_transform_to_Dancer") then
			actor:AttachParticle(31,"char_root","ef_attk_fly_01_char_root");
		end
		
		local kAction = actor:GetAction();
		
		if actor:IsUnderMyControl() then
		
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
			if bUseTLM then
			
				kAction:CreateActionTargetList(actor);
				if IsSingleMode() then
					Skill_Melee_Blow_Up_OnTargetListModified(actor,kAction,false)
				else
					kAction:BroadCastTargetListModify(actor:GetPilot());
				end			
			
			end
		
		end
		kAction:SetParam(2,"HIT");
				
		--Actor_PC_Base_DoDefaultMeleeAttack(actor,kAction);
		--actor:ClearIgnoreEffectList();
		
		if actor:IsMyActor() then
		
			if actor:GetAction():GetParam(3) == "a_jump" then
			
				actor:ReserveTransitAction("a_jump");
				
			end
			
		end
	
	end

	return	true;
end
