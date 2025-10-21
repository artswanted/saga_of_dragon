function Skill_Melee_Charge_Attack_OnCheckCanEnter(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
		return	false;
	end
	if actor:CheckStatusEffectExist("se_transform_to_metamorphosis")
	or actor:CheckStatusEffectExist("se_transform_to_Dancer")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Fig")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Mag")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Arc")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Thi")then
		return false
	end
	return		true;
	
end
function Skill_Melee_Charge_Attack_OnCastingCompleted(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(5,1);
	if actor:IsMyActor() == false then
		Skill_Melee_Charge_Attack_Fire(actor,action);
	end
		
end
function Skill_Melee_Charge_Attack_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(1,0);
	action:SetParamInt(5,0);
	
	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
			return	false;
		end
	end
		
	if actor:CheckStatusEffectExist("se_transform_to_metamorphosis")
	or actor:CheckStatusEffectExist("se_transform_to_Dancer")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Fig")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Mag")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Arc")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Thi")then
		return false
	end
	
	if( IsAbleSubPlayerMeleeAttack2(actor) ) then
		TryMustChangeSubPlayerAction( actor, "a_twin_charge_shot1_s", action:GetDirection() )
	end
	--actor:StartWeaponTrail();
	
	action:SetAutoBroadCastWhenCastingCompleted(false);
	action:SetParamInt(3,0);
	
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
		, g_fChargeFocusFilterValue[2]
		, g_fChargeFocusFilterValue[3]
		, g_fChargeFocusFilterValue[4]
		,true
		,true)
	end
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
	end
	
   return true
end
function Skill_Melee_Charge_Attack_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(1,1);

	local iBaseClassID = actor:GetPilot():GetBaseClassID();
	
	actor:PlayNext();
	
	local	kActorID = actor:GetID();
	
	if iBaseClassID == CT_THIEF then
		actor:AttachParticle(10,"p_ef_center","ef_thi_charge_01_result");			
	elseif iBaseClassID == CT_FIGHTER then
		actor:AttachParticle(10,"p_ef_center","e_fighter_charge_02");			
	elseif iBaseClassID == CT_DOUBLE_FIGHTER then
		local kSubActor = actor:GetSubPlayer()
		if( false == kSubActor:IsNil() ) then
			local kSubActorAction = kSubActor:GetAction()
			if( false == kSubActorAction:IsNil() ) then 
				if( "a_twin_charge_shot1_s"== kSubActorAction:GetID() ) then 				
					TryMustChangeSubPlayerAction( actor, "a_twin_charge_shot2_s", action:GetDirection() )
				end
			end
		end
	end
		
	if actor:IsMyActor() == true then
		
        action:CreateActionTargetList(actor);

		action:SetActionInstanceID();
		action:BroadCast(actor:GetPilot());
	
	end

end
function Skill_Melee_Charge_Attack_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iAttackRange = action:GetSkillRange(0,actor)

	local	kParam = FindTargetParam();

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = g_fChargingAttackAngle
	end
	
	
	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,g_fChargingAttackAngle,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);
		
end

function Skill_Melee_Charge_Attack_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction();
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	curSlot = action:GetCurrentSlot();
	local	iState = action:GetParamInt(3);
	
	local	bKeyCheck =  KeyIsDown( KEY_CHARGING_UKEY )
	
	if actor:IsMyActor() == false then
		bKeyCheck = true;
	end
	local iCheckLog = 0
	if(bKeyCheck) then iCheckLog = 1 end
	
	--ODS("iState:"..iState.." curSlot:"..curSlot.." Key:"..iCheckLog.." param:"..action:GetParamInt(5) .."\n", false, 912)
	if iState == 0 then
	
		if animDone == true then
		
			if curSlot == 0 then
				actor:PlayNext();
				
				if iBaseClassID == CT_THIEF then
					actor:AttachParticle(10,"p_wp_l_hand","ef_thi_charge_01_loop_p_wp_l_hand");
				elseif iBaseClassID == CT_FIGHTER then
					actor:AttachParticle(10,"p_wp_r_hand","e_fighter_charge_01");
				--elseif iBaseClassID == CT_DOUBLE_FIGHTER then
				end
			end
			if curSlot == 2 then
				action:SetParamInt(3,1);
				action:SetParamFloat(4,accumTime);
				return true;
			end
		
		end
		
		if bKeyCheck == false then
		
			if curSlot == 0 then
				local iBaseClassID = actor:GetPilot():GetBaseClassID();
				if iBaseClassID == CT_DOUBLE_FIGHTER then
					TryMustChangeSubPlayerAction( actor, "a_twin_sub_repos", action:GetDirection() )
				end
				return	false;
			end
			
			if curSlot == 1 and action:GetParamInt(5) == 1 then
			
				Skill_Melee_Charge_Attack_Fire(actor,action);
			else
				return	false;
			
			end	
		end
	else
	
		local	fElapsedTime = accumTime - action:GetParamFloat(4);
		if fElapsedTime>=g_fChargingFreezeTime then
			action:SetParam(2,"end");
			return	false;
		end				
	
	end

	return true
end

function Skill_Melee_Charge_Attack_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(10);
	actor:EndWeaponTrail();
	
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
		, g_world:GetWorldFocusFilterAlpha()
		, 0
		, g_fChargeFilterReturnTime
		,false
		,true);
	end

	local kPilot = actor:GetPilot()
	if( false == kPilot:IsNil() ) then 		
		local	iBaseActorType = kPilot:GetBaseClassID()
		if(iBaseActorType == CT_DOUBLE_FIGHTER) then
			local kSubActor = actor:GetSubPlayer()
			if( false == kSubActor:IsNil() ) then	
				local kSubActorAction = kSubActor:GetAction()
				if( false == kSubActorAction:IsNil() ) then
					if(kSubActorAction:GetID() ~= "a_twin_sub_repos" and kSubActorAction:GetID() ~= "a_twin_sub_trace_ground") then
						TryMustChangeActorAction(kSubActor, "a_twin_sub_trace_ground")
						return true;
					end
				end
			end
		end
	end
	
	return true;
end


function Skill_Melee_Charge_Attack_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetEnable() == false then
		return	false;
	end
	
	if action:GetID() == "a_jump" then
		return	false;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	
	
	local	curAction = actor:GetAction();
	if curAction:GetParamInt(1) == 1 and curAction:GetParam(2)~="end" then
		return		false;
	end
	
   return true;
   
end

function Skill_Melee_Charge_Attack_OnEvent(actor,textKey,seqID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local	action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if textKey == "hit" and action:GetActionParam() == AP_FIRE then
	
		SkillHelpFunc_DefaultHitOneTime(actor,action, true);
	
	end

	return	true;
end
