-- Melee
-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)

-- [DIRECTION] : 공격할 방향  "LEFT" or "RIGHT"

function Skill_Melee_Knock_Back_OnCastingCompleted(actor, action)
end

function Skill_Melee_Knock_Back_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local	iBaseActorType = actor:GetPilot():GetBaseClassID()

    if actor:IsMyActor() == false then
        return true
    end

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	if actor:IsMeetFloor() == false then
		if actor:GetJumpAccumHeight()<50 then	
			return	false;
		end		
	end
	
	--if actor:GetComboCount()<2 then
	--	if iBaseActorType == CT_FIGHTER then	
	--		actor:ReserveTransitAction("a_melee_01");
	--	elseif iBaseActorType == CT_THIEF then	
	--		actor:ReserveTransitAction("a_thi_melee_01");
	--	end
		
	--	return	false;
	--end		

	return		true;
	
end
function Skill_Melee_Knock_Back_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	SkillFunc_InitUseDmgEffect(action)
	
	local	actorID = actor:GetID()
--	local	actionID = action:GetID()
--	local	weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_Melee_Knock_Back_OnEnter\n");
	
	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
			if actorID=="c_fighter_male" or actorID=="c_fighter_female" then
				action:SetNextActionName("a_fighter_jump_attack");
			elseif actorID =="c_thief_male" or actorID =="c_thief_female" then
				action:SetNextActionName("a_thief_jump_kick");
			end
			
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;	
		end
		
		
		actor:SetComboCount(0);

		local	iBaseActorType = actor:GetPilot():GetBaseClassID()
		
		if iBaseActorType == CT_FIGHTER then
			actor:SetNormalAttackActionID("a_melee_01");		
		elseif iBaseActorType == CT_THIEF then
			actor:SetNormalAttackActionID("a_thi_melee_01");		
		end
		
	end
		

	if actor:IsMyActor() == true then
		GetComboAdvisor():OnNewActionEnter(action:GetID());
	end
	
	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
	
	if bUseTLM == false then
		action:CreateActionTargetList(actor);
	end

	--actor:StartWeaponTrail();
	
	return true
end
function Skill_Melee_Knock_Back_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kPos = actor:GetPos();
--	local	kDir = actor:GetLookingDir();

	local	kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 30
	end
	kParam:SetParam_2(action:GetSkillRange(0,actor),iParam2,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);

	return	action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
end

function Skill_Melee_Knock_Back_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	local	animDone = actor:IsAnimationDone()
--	local	nextActionName = action:GetNextActionName()
--	local	iMaxActionSlot = action:GetSlotCount();
--	local	weapontype = actor:GetEquippedWeaponType();
--	local	actionID = action:GetID();
	if action:GetParam(1) == "end" then
		return	false;
	end

	if animDone == true then
	
		action:SetParam(1, "end")
		return false
		
	end
	
	return true
end
function Skill_Melee_Knock_Back_OnCleanUp(actor)
	--actor:EndWeaponTrail();
	return true;
end
function Skill_Melee_Knock_Back_OnLeave(actor, action)

    ODS("Skill_Melee_Knock_Back_OnLeave action : "..action:GetID().."\n");
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
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
		
	if action:GetEnable() == true then
		return false;
	elseif curParam == "end" and actionID == "a_run" then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		actor:SetNormalAttackActionID("a_melee_01");
		return true;
	end

	return false 
end
function Skill_Melee_Knock_Back_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if action:GetParam(2) == "HIT" then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            if actor:IsMyActor()  then
				action:GetTargetList():ApplyActionEffects(true,true);
            else
				SkillHelpFunc_DefaultHitOneTime(actor,action, true);
            end
        
        end        
    
    end

end
function Skill_Melee_Knock_Back_OnEvent(actor,textKey,seqID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	if textKey == "hit" then
	
	    actor:ClearIgnoreEffectList();
		if actor:GetPilot():GetBaseClassID() == CT_FIGHTER then
			
			actor:AttachParticle(31,"char_root","edge_swd_01_06");
		
		end

		local kAction = actor:GetAction();
					
		CheckNil(nil==kAction);
		CheckNil(kAction:IsNil());
	
		if actor:IsUnderMyControl() then
		
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
			if bUseTLM then
			
				kAction:CreateActionTargetList(actor);
				if IsSingleMode() then
					Skill_Melee_Knock_Back_OnTargetListModified(actor,kAction,false)
				else
					kAction:BroadCastTargetListModify(actor:GetPilot());
				end			
			
			end
			
		end
		kAction:SetParam(2,"HIT");
			
		if actor:IsMyActor() then
			if actor:IsToLeft() == true then
				QuakeCamera(g_fMeleeKnockBackQuakeValue_Left[1]
				, g_fMeleeKnockBackQuakeValue_Left[2]
				, g_fMeleeKnockBackQuakeValue_Left[3]
				, g_fMeleeKnockBackQuakeValue_Left[4]
				, g_fMeleeKnockBackQuakeValue_Left[5])  
			else
				QuakeCamera(g_fMeleeKnockBackQuakeValue_Right[1]
				, g_fMeleeKnockBackQuakeValue_Right[2]
				, g_fMeleeKnockBackQuakeValue_Right[3]
				, g_fMeleeKnockBackQuakeValue_Right[4]
				, g_fMeleeKnockBackQuakeValue_Right[5])  
			end
		end
	end

	return	true;
end
