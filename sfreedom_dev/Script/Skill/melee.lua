-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)

-- Melee
function Act_Melee_OnCastingCompleted(actor, action)
end

function Act_Melee_OnCheckCanEnter(actor,action)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	--펫탑승 중이라면 펫탑승 공격 스킬로 대체
	if actor:IsRidingPet() == true then
		local kPetActor = actor:GetMountTargetPet()
		if kPetActor:IsNil() == true then
			return false
		end
		if kPetActor:CheckCanRidingAttack() == true then
			kPetActor:ReserveTransitAction("a_pet_melee_01")
		else
			kPetActor:ReserveTransitAction("a_pet_melee_02")
		end
		return false
	end
	
	local	iBaseActorType = actor:GetPilot():GetBaseClassID()
	if(iBaseActorType == CT_DOUBLE_FIGHTER) then
		local kSubActor = actor:GetSubPlayer()
		if( false == kSubActor:IsNil() ) then
			local kSubActorAction = kSubActor:GetAction()
			if( kSubActorAction:IsNil() ) then return false end
			--local	iComboCount = actor:GetComboCount()
			if ("a_twin_melee_03_s" == kSubActorAction:GetID()) then			
				kSubActorAction:SetNextActionName("a_twin_melee_05_s")
			end	
		end
	end
		
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	
	if actor:IsMeetFloor() == false then
		if actor:GetFloatHeight(70)<70 then
			return	false;	
		end
	end
	
	if actor:IsMeetFloor() == false then	
		if Act_Melee_ToFloatAttackCheck(actor,action) == false then
			return	false;
		end
	end	
	
	-- 안전거품 공격막기
	if g_pilotMan:IsLockMyInput() == true then
		return false;
	end
	
	return		true;
	
end

function Act_Melee_ToFloatAttackCheck(actor,action)
	ODS("Act_Melee_ToFloatAttackCheck actor:IsMeetFloor() == false\n");
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
    local	iBaseActorType = actor:GetPilot():GetBaseClassID()
    
    local	kNextActionName ="";

	if (actor:GetJumpTime()>0.5 and actor:GetComboCount() == 0) or Act_Melee_IsToDownAttack(actor,action) then
		
		if actor:CheckStatusEffectExist("se_transform_to_Dancer") then
			kNextActionName = "a_dc_melee_drop_01";
		elseif iBaseActorType == CT_MAGICIAN then
			kNextActionName = "a_magician_down_shot";
		elseif iBaseActorType == CT_THIEF then
			kNextActionName = "a_thi_melee_drop";
		elseif iBaseActorType == CT_DOUBLE_FIGHTER then
			kNextActionName = "a_twin_bunker_buster";
		else
			kNextActionName = "a_melee_drop";
		end
	end
	
	if kNextActionName~="" and action:CheckCanEnter(actor,kNextActionName) then
		return	true;
	end
	
	kNextActionName ="";
	
	if actor:GetParam("FLOAT_ATTACK_ENABLE")~="FALSE" then
	
	    local iComboCount = actor:GetComboCount();
	    
		if(2 < iComboCount) then 
			iComboCount = 1  
			--ODS("오버해서 넘김되서 리셋1\n", false, 912) 
		end
		
	    if actor:CheckStatusEffectExist("se_transform_to_metamorphosis") then
			kNextActionName = "a_nj_float_melee_0"..(iComboCount+1);
		elseif actor:CheckStatusEffectExist("se_transform_to_Dancer") then
			return false;
	    elseif iBaseActorType == CT_THIEF then
			kNextActionName = "a_thief_float_melee_01";
		elseif iBaseActorType == CT_MAGICIAN then
			local MageSkillName = "a_magefloatshot_0"..(iComboCount+1);
			if action:CheckCanEnter(actor, MageSkillName) then
				kNextActionName = MageSkillName;
			else
				kNextActionName = "a_MagicianFloatShot_0"..(iComboCount+1);
			end
		elseif iBaseActorType == CT_DOUBLE_FIGHTER then
			kNextActionName = "a_twin_float_melee_0"..(iComboCount+1);
		else
			kNextActionName = "a_float_melee_0"..(iComboCount+1);
			--ODS("a_float_melee_0 + "..iComboCount.."\n",false, 912)
        end
	end			
	
	if kNextActionName~="" and action:CheckCanEnter(actor,kNextActionName) then
		return	true;
	end
		
	return	false;
end
function Act_Melee_ToFloatAttack(actor,action)
	ODS("Act_Melee_ToFloatAttack actor:IsMeetFloor() == false\n");
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
    local	iBaseActorType = actor:GetPilot():GetBaseClassID()
    
    local	kNextActionName ="";

	if (actor:GetJumpTime()>0.5 and actor:GetComboCount() == 0) or Act_Melee_IsToDownAttack(actor,action) then
		
		if actor:CheckStatusEffectExist("se_transform_to_Dancer") then
			kNextActionName = "a_dc_melee_drop_01";
		elseif iBaseActorType == CT_MAGICIAN then
			kNextActionName = "a_magician_down_shot";
		elseif iBaseActorType == CT_THIEF then
			kNextActionName = "a_thi_melee_drop";
		elseif iBaseActorType == CT_DOUBLE_FIGHTER then
			kNextActionName = "a_twin_bunker_buster";
		else
			kNextActionName = "a_melee_drop";
		end
	end
	
	ODS("Magician Shot kNextActionName : "..kNextActionName.."\n");
	
	if kNextActionName~="" and action:CheckCanEnter(actor,kNextActionName) then
		actor:SetComboCount(0);	
		action:SetNextActionName(kNextActionName);
		action:ChangeToNextActionOnNextUpdate(true, true)
		return	true;
	end
	
	kNextActionName ="";
	
	if actor:GetParam("FLOAT_ATTACK_ENABLE")~="FALSE" then
	
	    local iComboCount = actor:GetComboCount();
	    
		if(2 < iComboCount) then 
			iComboCount = 1  
			--ODS("오버해서 넘김되서 리셋2\n", false, 912) 
		end
		
	    if actor:CheckStatusEffectExist("se_transform_to_metamorphosis") then
			kNextActionName = "a_nj_float_melee_0"..(iComboCount+1);
		elseif actor:CheckStatusEffectExist("se_transform_to_Dancer") then
			kNextActionName = "a_dc_melee_0"..(iComboCount+1);	    
	    elseif iBaseActorType == CT_THIEF then
			kNextActionName = "a_thief_float_melee_01";
		elseif iBaseActorType == CT_MAGICIAN then
			local MageSkillName = "a_magefloatshot_0"..(iComboCount+1);
			if action:CheckCanEnter(actor, MageSkillName) then
				kNextActionName = MageSkillName;
			else
				kNextActionName = "a_MagicianFloatShot_0"..(iComboCount+1);
			end
		elseif iBaseActorType == CT_DOUBLE_FIGHTER then
			kNextActionName = "a_twin_float_melee_0"..(iComboCount+1);
		else
			kNextActionName = "a_float_melee_0"..(iComboCount+1);
        end
	end			
	
	if kNextActionName~="" and action:CheckCanEnter(actor,kNextActionName) then
		action:SetNextActionName(kNextActionName);
		action:ChangeToNextActionOnNextUpdate(true, true)
		return	true;
	end
		
	return	false;
end

function Act_Melee_ToDownAttack(actor,action,bFlyAttack)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
    local	iBaseActorType = actor:GetPilot():GetBaseClassID()

	local	iComboCount = tonumber(actor:GetParam("DOWN_ATTACK_COMBO_COUNT"));
	if iComboCount == nil then
		iComboCount = 0
	end
	
	
	--local	kCurActionID = actor:GetAction():GetID();
	
	local	fLastDownAttackTime = tonumber(actor:GetParam("LAST_DOWN_ATTACK_TIME"));
	if fLastDownAttackTime == nil then
		fLastDownAttackTime = 0.0
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local	fNowTime = g_world:GetAccumTime();
	
	if fNowTime - fLastDownAttackTime > 0.5 then
		iComboCount = 0;
	end		
	
	if  bFlyAttack then
		if iComboCount < 2 then
			return false
		end
	end
	
	local	iNextComboCount = iComboCount+1;
	local iMaxCount = 3;
	if actor:CheckStatusEffectExist("se_transform_to_Dancer") then
		iMaxCount = 2;
	end

	if iNextComboCount >= iMaxCount then
		iNextComboCount = 0
	end
	
	actor:SetParam("DOWN_ATTACK_COMBO_COUNT",""..iNextComboCount);
	
	local	kNextActionName = "";
	
	if bFlyAttack then
		if iComboCount < 2 then
			return	false
		end
		if iBaseActorType == CT_MAGICIAN then
			kNextActionName = "a_mag_down_attack_fly";
		elseif iBaseActorType == CT_ARCHER then
			kNextActionName = "a_arc_down_attack_fly";
		elseif iBaseActorType == CT_THIEF then
			kNextActionName = "a_thi_down_attack_fly";
		else
			kNextActionName = "a_down_attack_fly";
		end
	else
		if actor:CheckStatusEffectExist("se_transform_to_Dancer") then
			kNextActionName = ("a_dc_down_0"..(iComboCount+1));	
		elseif iBaseActorType == CT_MAGICIAN then
			kNextActionName = ("a_mag_down_attack_0"..(iComboCount+1));
		elseif iBaseActorType == CT_ARCHER then
			kNextActionName = ("a_arc_down_attack_0"..(iComboCount+1));
		elseif iBaseActorType == CT_THIEF then
			kNextActionName = ("a_thi_down_attack_0"..(iComboCount+1));
		elseif iBaseActorType == CT_SHAMAN then
			kNextActionName = ("a_Sum_Down_Attack_0"..(iComboCount+1));
		elseif iBaseActorType == CT_DOUBLE_FIGHTER then
			kNextActionName = ("a_twin_down_attack_0"..(iComboCount+1));
		else
		    kNextActionName = ("a_down_attack_0"..(iComboCount+1));
		end
    end
    

	if action:CheckTargetExist(actor,kNextActionName) == false or 
		action:CheckCanEnter(actor,kNextActionName) == false then
		
		actor:SetParam("DOWN_ATTACK_COMBO_COUNT",""..0);
		return	false
	end
    action:SetNextActionName(kNextActionName);
    action:ChangeToNextActionOnNextUpdate(true,true);
    
    actor:SetComboCount(0);
    return true;		
end

function Act_Melee_ToBlowUpAttack(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
    local	iBaseActorType = actor:GetPilot():GetBaseClassID()

	local	iComboCount = actor:GetComboCount();
	--if iComboCount<2 or iBaseActorType == CT_MAGICIAN then
	
	--	if Act_Melee_ToDownAttack(actor,action,true) then
	--		return	true
	--	end
	
	--	return	false
	--end
	
	local	kNextActionName = "";
	
	if iBaseActorType == CT_THIEF then
		kNextActionName = "a_thi_melee_blow_up"
	else
		kNextActionName = "a_melee_blow_up"
	end
	
	if kNextActionName ~= "" then
		if action:CheckCanEnter(actor,kNextActionName) then
			action:SetNextActionName(kNextActionName);
			action:ChangeToNextActionOnNextUpdate(true,true);
			return true;	
		end
    end
    
    return	false;
end

function Act_Melee_ToKnockBackAttack(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iBaseActorType = actor:GetPilot():GetBaseClassID()
	
	if iBaseActorType == CT_MAGICIAN then
		return	false
	end

	--local	iComboCount = actor:GetComboCount();
	--if iComboCount<2 then
	--	return	false
	--end
	
	local	kNextActionName = "";
	
	if iBaseActorType == CT_THIEF then
		kNextActionName = "a_thi_melee_knock_back"
	else
		kNextActionName = "a_melee_knock_back"
	end
	
	if kNextActionName ~= "" then	
		if action:CheckCanEnter(actor,kNextActionName) then
		   action:SetNextActionName(kNextActionName);
		   action:ChangeToNextActionOnNextUpdate(true,true);
		   return true;
		end
    end
    
    return	false;
end

function Act_Melee_IsToDownAttack(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	if action == nil then
		return	false
	end
	
	if	KeyIsDown(KEY_DOWN)	then
		return	true
	end
	
	local	kInputSlotInfo = action:GetInputSlotInfo();
	if kInputSlotInfo:IsNil() == false then
	
		if	kInputSlotInfo:GetUKey() == KEY_DOWN_ATTACK then
			return	true
		end
	
	end
	
	return	false;
	
end

function Act_Melee_GetMaxComboCount(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local weapontype = actor:GetEquippedWeaponType();
	local	iBaseActorType = actor:GetPilot():GetBaseClassID()
	
	if weapontype == WT_BTS	then	--	양손검은 4타
		return	4
	elseif weapontype == WT_SPR then	--	창은 3타
		return	3
	end
	
	if(iBaseActorType == CT_DOUBLE_FIGHTER) then 
		return 3
	end
		
	if actor:CheckStatusEffectExist("se_transform_to_metamorphosis") then
		return	3;
	end

	if actor:CheckStatusEffectExist("se_transform_to_Dancer") then
		return 5;
	end	    

	if actor:CheckStatusEffectExist("se_Bloody_Lore") then
		return	3	
	end
	
	return	5;

end

function Act_Melee_IsToUpAttack(actor,action)
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if	KeyIsDown(KEY_UP)	then
		return	true
	end
	
	local	kInputSlotInfo = action:GetInputSlotInfo();
	if kInputSlotInfo:IsNil() == false then
	
		if	kInputSlotInfo:GetUKey() == KEY_UP_ATTACK then
			return	true
		end
	
	end
	
	return	false;
	
end
function Act_Melee_IsToLeftAttack(actor,action)
		
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if	KeyIsDown(KEY_LEFT)	then
		return	true
	end
	
	local	kInputSlotInfo = action:GetInputSlotInfo();
	if kInputSlotInfo:IsNil() == false then
	
		if	kInputSlotInfo:GetUKey() == KEY_LEFT_ATTACK then
			return	true
		end
	
	end
	
	return	false;
	
end
function Act_Melee_IsToRightAttack(actor,action)
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if	KeyIsDown(KEY_RIGHT)	then
		return	true
	end
	
	local	kInputSlotInfo = action:GetInputSlotInfo();
	if kInputSlotInfo:IsNil() == false then
	
		if	kInputSlotInfo:GetUKey() == KEY_RIGHT_ATTACK then
			return	true
		end
	
	end
	
	return	false;
	
end
function Act_Melee_OnEnter(actor, action)
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();

	ODS("Act_Melee_OnEnter action:"..action:GetID().."\n");
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	action:SetParam(1, "");
	
	if actor:IsMyActor() == true then		
		local	iBaseActorType = actor:GetPilot():GetBaseClassID()
		
		action:SetParam(4, "");
		
		if actor:IsMeetFloor() == false then	
			if Act_Melee_ToFloatAttack(actor,action) then
				return	true
			end
			return false;
		end
		
		if Act_Melee_IsToDownAttack(actor,action) then
			if	Act_Melee_ToDownAttack(actor,action) then
				return	true
			end
		end
		
		if Act_Melee_IsToUpAttack(actor,action) then
			if Act_Melee_ToBlowUpAttack(actor,action) then
				return	true
			end
		end
		
		action:SetParam(0,"");
		
		-- New SkillCombo System -----------------------------------------------------------
		if 0 == Config_GetValue(HEADKEY_ETC, "USE_OLD_COMBO") then
			local iWeaponType = actor:GetEquippedWeaponType();
			local bUseCombo = false
			if actor:CheckStatusEffectExist("se_transform_to_metamorphosis") then	
				--action:SetNextActionName("a_nj_melee_01");
			elseif actor:CheckStatusEffectExist("se_transform_to_Dancer") then
				--action:SetNextActionName("a_dc_melee_01");
			elseif iBaseActorType == CT_FIGHTER then
				if 1 == iWeaponType then
					actor:ReserveTransitAction("a_ohs_combo_a");
					bUseCombo = true
				elseif 2 == iWeaponType then
					actor:ReserveTransitAction("a_ths_combo_a");
					bUseCombo = true
				end
			elseif iBaseActorType == CT_THIEF then
				if 7 == iWeaponType then
					actor:ReserveTransitAction("a_cl_combo_a");
					bUseCombo = true
				elseif 8 == iWeaponType then
					actor:ReserveTransitAction("a_cu_combo_a");
					bUseCombo = true
				end
			elseif iBaseActorType == CT_DOUBLE_FIGHTER then				
					actor:ReserveTransitAction("a_pg_combo_a");
					bUseCombo = true
			end
			if true == bUseCombo then
				actor:ResetAnimation();
				return false;
			end
		end
		-- End : New SkillCombo System --------------------------------------------------------
		
		local	iComboCount = actor:GetComboCount();	
		local	iNextComboCount = iComboCount+1;
		
		if iComboCount == 0 and 
			action:GetID() ~= "a_melee_01" and 
			action:GetID() ~= "a_Bloody_melee_01" and 
			action:GetID() ~= "a_nj_melee_01" and 
		    action:GetID() ~= "a_dc_melee_01" and 
			action:GetID() ~= "a_thi_melee_01" 	and
			action:GetID() ~= "a_mag_melee_01" 	and
			action:GetID() ~= "a_twin_melee_01_m"
			then				   
			actor:SetComboCount(0);	
		
			if actor:CheckStatusEffectExist("se_transform_to_metamorphosis") then	
				action:SetNextActionName("a_nj_melee_01");
			elseif actor:CheckStatusEffectExist("se_transform_to_Dancer") then
				action:SetNextActionName("a_dc_melee_01");
			elseif actor:CheckStatusEffectExist("se_Bloody_Lore") then
				action:SetNextActionName("a_Bloody_melee_01");
			elseif iBaseActorType == CT_FIGHTER then
				action:SetNextActionName("a_melee_01");
			elseif iBaseActorType == CT_THIEF then
				action:SetNextActionName("a_thi_melee_01");
			elseif iBaseActorType == CT_MAGICIAN then
				action:SetNextActionName("a_mag_melee_01");
			elseif iBaseActorType == CT_DOUBLE_FIGHTER then
				action:SetNextActionName("a_twin_melee_01_m");
			end
		
			action:ChangeToNextActionOnNextUpdate(true,true);					
			
			return true;
		end
		
		if iComboCount > 0 then
		
			if actor:CanNowConnectToNextComboAttack(0.4) == false 	 then
				actor:SetComboCount(0);	
				
				if actor:CheckStatusEffectExist("se_transform_to_metamorphosis") then	
					action:SetNextActionName("a_nj_melee_01");
				elseif actor:CheckStatusEffectExist("se_transform_to_Dancer") then
					action:SetNextActionName("a_dc_melee_01");
				elseif actor:CheckStatusEffectExist("se_Bloody_Lore") then
					action:SetNextActionName("a_Bloody_melee_01");
				elseif iBaseActorType == CT_FIGHTER then
					action:SetNextActionName("a_melee_01");
				elseif iBaseActorType == CT_THIEF then
					action:SetNextActionName("a_thi_melee_01");
				elseif iBaseActorType == CT_DOUBLE_FIGHTER then				
					action:SetNextActionName("a_twin_melee_01_m");
				end
				
				action:ChangeToNextActionOnNextUpdate(true,true);					
				return true;
			end	
			
			if iNextComboCount >= Act_Melee_GetMaxComboCount(actor,action) then
				iNextComboCount = 0;
			end
		
		end
				
		local	kMeleeActions = {"a_melee_01","a_melee_02","a_melee_03","a_melee_04","a_melee_05"};
		
		if actor:CheckStatusEffectExist("se_transform_to_metamorphosis") then
			kMeleeActions = {"a_nj_melee_01","a_nj_melee_02","a_nj_melee_03"};
		elseif actor:CheckStatusEffectExist("se_transform_to_Dancer") then
			kMeleeActions = {"a_dc_melee_01","a_dc_melee_02","a_dc_melee_03","a_dc_melee_04","a_dc_melee_05"};
		elseif actor:CheckStatusEffectExist("se_Bloody_Lore") then
			kMeleeActions = {"a_Bloody_melee_01","a_Bloody_melee_02","a_Bloody_melee_03"};
		elseif iBaseActorType == CT_THIEF then
			kMeleeActions = {"a_thi_melee_01","a_thi_melee_02","a_thi_melee_03","a_thi_melee_04","a_thi_melee_05"};
		elseif iBaseActorType == CT_MAGICIAN then
			kMeleeActions = {"a_mag_melee_01","a_mag_melee_02","a_mag_melee_03"};
		elseif iBaseActorType == CT_DOUBLE_FIGHTER then
			kMeleeActions = {"a_twin_melee_01_m", "a_twin_melee_02_m", "a_twin_melee_04_m"};
		end
		
		local iNextActionIndex = iComboCount+1;
		if iNextActionIndex == 0 
			or	iNextActionIndex > table.maxn(kMeleeActions)
		then
		    iNextActionIndex = 1;
		end
		
		local kPrevAction = actor:GetAction()
		if(false == kPrevAction:IsNil() and iBaseActorType == CT_DOUBLE_FIGHTER) then
			local kSubActor = actor:GetSubPlayer()
			if( false == kSubActor:IsNil() ) then
				local kSubActorAction = kSubActor:GetAction()
				if( false == kSubActorAction:IsNil() ) then 						
					if (1 == iNextActionIndex) then	
						if ("a_twin_melee_03_s" == kSubActorAction:GetID()) then						
							kSubActorAction:SetNextActionName("a_twin_melee_05_s")
						elseif( IsAbleSubPlayerMeleeAttack( kSubActorAction:GetID() ) ) then
							TryMustChangeSubPlayerAction(actor, "a_twin_sub_repos", action:GetDirection())
						end
					elseif(3 == iNextActionIndex) then
						if( IsAbleSubPlayerMeleeAttack( kSubActorAction:GetID() ) ) then
							TryMustChangeSubPlayerAction(actor, "a_twin_melee_03_s", kPrevAction:GetDirection())
						end
					end	
				end
			end
		end
		
		if kMeleeActions[iNextActionIndex] ~= action:GetID() then
		
			action:SetNextActionName(kMeleeActions[iNextActionIndex]);
			action:ChangeToNextActionOnNextUpdate(true,true);					
			return true;		    
		    
		end		
		actor:SetComboCount(iNextComboCount);		
		
		local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
		
		if bUseTLM == false then
			action:CreateActionTargetList(actor);
		end
		
	end
	
	actor:ResetAnimation();
	
	if actor:IsMyActor() then     
	
	     
	    ODS("GetComboAdvisor():OnNewActionEnter : "..action:GetID().."\n");
	    
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
	    
	    if actor:GetComboCount()<Act_Melee_GetMaxComboCount(actor,action) then
	    
	        local kNormalAttackActionID = actor:GetNormalAttackActionID();
            GetComboAdvisor():AddNextAction(kNormalAttackActionID);
    
	        if actor:GetComboCount()>=2 then
    	    
    			if iBaseActorType == CT_THIEF then

					GetComboAdvisor():AddNextAction("a_thi_melee_blow_up");
					GetComboAdvisor():AddNextAction("a_thi_melee_knock_back");

    			else
    			
					GetComboAdvisor():AddNextAction("a_melee_blow_up");
					GetComboAdvisor():AddNextAction("a_melee_knock_back");
		            
	            end
        	    
	        end
    	    
	    end
	
	end
	
	
	action:SetParamInt(3,0)	--	Hit Count
	
	return true
end

function Act_Melee_OnFindTarget(actor,action,kTargets)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 30
	end
	
	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(action:GetSkillRange(0,actor),iParam2,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
end

function Act_Melee_Finished(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local nextActionName = action:GetNextActionName()
	local curActionParam = action:GetParam(0)
	
	ODS("Act_Melee_Finished curActionParam:"..curActionParam.." nextActionName : "..nextActionName.."\n");
	
	actor:SetNormalAttackEndTime();
			
	if action:GetParam(2) ~= "HIT" then
		actor:SetComboCount(0)
	end
	
	if actor:IsMyActor() then
	
		if KeyIsDown(KEY_FIRE) == true then
			actor:ReserveTransitAction(actor:GetNormalAttackActionID(),DIR_NONE);
			action:SetParam(1, "end")
			return	false;
		end
	end
	
	if nextActionName ~= "a_battle_idle" or
		KeyIsDown(KEY_LEFT) or 
		KeyIsDown(KEY_RIGHT) or
		KeyIsDown(KEY_UP) or
		KeyIsDown(KEY_DOWN) then
		
		action:SetParam(1, "end")
		
		return	false;
	else
		action:SetParam(1, "delay")
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		action:SetParamFloat(0,g_world:GetAccumTime());
	end
	
	return true
end

function Act_Melee_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local action = actor:GetAction()
	local animDone = actor:IsAnimationDone()
	local param = action:GetParam(1)
		
	if("end" == param) then
		return false
	end
	
	if	action:GetParam(1) == "delay" then
		local	fElapsedTime = accumTime - action:GetParamFloat(0)
		if fElapsedTime > 0.1 then
			action:SetParam(1, "end")
			return	false
		end
		return	true;
	end
	
	if animDone == true then
		return Act_Melee_Finished(actor,action);
	end

	return true
end

function Act_Melee_OnCleanUp(actor)
	return true;
end

function Act_Melee_OnLeave(actor, action)
	
	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local curAction = actor:GetAction();
	if( CheckNil(nil==curAction) ) then return true end
	if( CheckNil(curAction:IsNil()) ) then return true end
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	local	iBaseActorType = actor:GetPilot():GetBaseClassID()
	
	ODS("Act_Melee_OnLeave actionID:"..actionID.." curParam:"..curParam.."\n");
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if actionID == "a_jump" then 
		return false;
	end
	

	if action:GetActionType()=="EFFECT" then
		
		actor:SetComboCount(0);	
		
		if iBaseActorType == CT_FIGHTER then
			actor:SetNormalAttackActionID("a_melee_01");
		elseif iBaseActorType == CT_THIEF then
			actor:SetNormalAttackActionID("a_thi_melee_01");
		end
		
		return true;
	end
	
	if curParam == "end" or curParam == "null" then 
		return true;
	end
	
	if action:GetEnable() == false then
	    ODS("Enable false\n");
		
		if kNextActionName==actionID then
			curAction:SetParam(0,"");
		end
		
	end
	
	if action:GetEnable() == true then
	
		if curParam == "delay" then
			return	true
		end
		
		if curAction:GetParam(2) == "HIT" or 
			actionID == "a_run" then
			
			curAction:SetNextActionName(actionID);
			curAction:SetNextActionInputSlotInfo(action:GetInputSlotInfo());
			
		end

		return false;
	
	elseif curParam == "end" and actionID == "a_run" then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_handclaps" then
		
		if iBaseActorType == CT_FIGHTER then
			actor:SetNormalAttackActionID("a_melee_01");
		elseif iBaseActorType == CT_THIEF then
			actor:SetNormalAttackActionID("a_thi_melee_01");
		end
		
		return true;
	end

	return false 
end

function Act_Melee_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)

	CheckNil(nil==kActionResult);
	CheckNil(nil==kActionTargetInfo);
	
	local kAction = actor:GetAction();
--	local actionID = kAction:GetID()
	local iComboCount = actor:GetComboCount();
		
	kAction:SetParam(4,"DoDamage");

	local	iSphereIndex = kActionTargetInfo:GetABVIndex();
	local pt = actorTarget:GetABVShapeWorldPos(iSphereIndex);
	
	local bCri = kActionResult:GetCritical()
	local DamageEff = actor:GetDamageEffectID(bCri)
	if nil==DamageEff or 0==string.len(DamageEff) then
		if bCri then
			DamageEff = "e_dmg_cri"			
		else
			DamageEff = "e_dmg"
		end
	end
	actorTarget:AttachParticleToPoint(12, pt, DamageEff)
	
	-- 충격 효과 적용
	local iShakeTime = g_fAttackHitHoldingTime * 1000;
	local	iHoldTime = iShakeTime;
	
	if iComboCount == Act_Melee_GetMaxComboCount(actor,kAction)-1 then	--	5타째에는 다르게 적용
		iHoldTime = g_fThirdAttackFreezeTime * 1000;
	end
	
	actor:SetAnimSpeedInPeriod(0.0001, iHoldTime);
	actor:SetShakeInPeriod(5, iShakeTime/2);
	actorTarget:SetShakeInPeriod(5, iShakeTime/2);		
	

end

function Act_Melee_IsDoubleAttack(actor,action)
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local iTargetCount = action:GetTargetCount();
	
	if iTargetCount == 0 then
	    return  false
    end
    
    local actionResult = action:GetTargetActionResult(0);
    local	iHitCount = actionResult:GetAbil(AT_HIT_COUNT)
    
    ODS("Act_Melee_IsDoubleAttack iHitCount:"..iHitCount.."\n");
    
    return  (iHitCount == 2);

end

BASE_ATTACK_EFFECT = {}
BASE_ATTACK_EFFECT[WT_SWORD] = {["a_melee_01"] = "edge_swd_01_01", ["a_melee_02"] = "edge_swd_01_02",
["a_melee_03"] = "edge_swd_01_03", ["a_melee_04"] = "edge_swd_01_031", ["a_melee_05"] = "edge_swd_01_032"}
BASE_ATTACK_EFFECT[WT_BTS] = {["a_melee_01"] = "edge_swd_02_01", ["a_melee_02"] = "edge_swd_02_02",
["a_melee_03"] = "edge_swd_02_03", ["a_melee_04"] = "edge_swd_02_04", ["a_melee_05"] = "edge_swd_02_05"}
BASE_ATTACK_EFFECT[WT_CLAW] = {["a_thi_melee_01"] = "edge_claw_01_01", ["a_thi_melee_02"] = "edge_claw_01_02",
["a_thi_melee_03"] = "edge_claw_01_03", ["a_thi_melee_04"] = "edge_claw_01_04", ["a_thi_melee_05"] = "edge_claw_01_05"}
BASE_ATTACK_EFFECT[WT_KAT] = {["a_thi_melee_01"] = "edge_kattar_01_01", ["a_thi_melee_02"] = "edge_kattar_01_02",
["a_thi_melee_03"] = "edge_kattar_01_03", ["a_thi_melee_04"] = "edge_kattar_01_04", ["a_thi_melee_05"] = "edge_kattar_01_05"}

function Act_Melee_HitOneTime(actor,action,bIsDoubleAttack)

	ODS("Act_Melee_HitOneTime\n");
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	
    action:SetParam(2,"HIT");	

	local iTotalHit = 1;
	if bIsDoubleAttack then
		iTotalHit= 2
	end
	
	local	iCurrentHitCount = action:GetParamInt(3)
	
	if iCurrentHitCount >= iTotalHit then
		return
	end
	
	local	bIsFinalHit = (iCurrentHitCount>=(iTotalHit-1));
	local	bIsFirstHit = (iCurrentHitCount == 0)
	

	if bIsFinalHit then
		actor:ClearIgnoreEffectList();
		
    end
    
    
    if bIsFirstHit then
		local weapontype = actor:GetEquippedWeaponType()
		local szNode = "char_root"
		local iSlot = 32+math.random(0,20)
		local kScale = 1.0
		local szActionID = action:GetID()
		local szEffName = "edge_swd_01_01"
		if nil~=BASE_ATTACK_EFFECT[weapontype] and nil~=BASE_ATTACK_EFFECT[weapontype][szActionID] then
			szEffName = BASE_ATTACK_EFFECT[weapontype][szActionID]
		end

		--if actor:GetPilot():GetBaseClassID() == CT_FIGHTER  and false == actor:CheckStatusEffectExist("se_Bloody_Lore") then
		if actor:GetPilot():GetBaseClassID() == CT_FIGHTER then
			local kAEffect = actor:GetAttackEffect(szActionID)
			if false==kAEffect:IsNil() and ""~=kAEffect:GetEffectName() then
				szEffName = kAEffect:GetEffectName()
				szNode = kAEffect:GetNodeName()
				kScale = kAEffect:GetScale()
			end

			actor:AttachParticleToPointWithRotate(iSlot,actor:GetNodeWorldPos(szNode),szEffName,actor:GetRotateQuaternion(), kScale)
		
		elseif actor:GetPilot():GetBaseClassID() == CT_THIEF and false == actor:CheckStatusEffectExist("se_transform_to_metamorphosis") and false == actor:CheckStatusEffectExist("se_transform_to_Dancer") then
			local kAEffect = actor:GetAttackEffect(szActionID)
			if false==kAEffect:IsNil() and ""~=kAEffect:GetEffectName() then
				szEffName = kAEffect:GetEffectName()
				szNode = kAEffect:GetNodeName()
				kScale = kAEffect:GetScale()
			end

			actor:AttachParticleToPointWithRotate(iSlot,actor:GetNodeWorldPos(szNode),szEffName,actor:GetRotateQuaternion(), kScale)
		
		end

	end
	
		
	if bIsFinalHit then
	
		local iTargetCount = action:GetTargetCount();
		local i =0;
		if iTargetCount>0 then
			
			local bWeaponSoundPlayed = false;
			
			while i<iTargetCount do
			
				local actionResult = action:GetTargetActionResult(i);
				if actionResult:IsNil() == false then
				
					action:SetParamInt(4,actionResult:GetValue()/(iTotalHit));
					local	iOneDmg = action:GetParamInt(4);
				
					actionResult:SetValue(
						actionResult:GetValue() - iOneDmg*(iTotalHit-1));
						
					local kTargetGUID = action:GetTargetGUID(i);
					local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
					if kTargetPilot:IsNil() == false and actionResult:IsMissed() == false then
						
						local actorTarget = kTargetPilot:GetActor();
						
	                    if bWeaponSoundPlayed == false then
	                        bWeaponSoundPlayed = true;
                            -- 피격 소리 재생
                            local actionName = action:GetID();
							if actionResult:GetCritical() then
								actionName = "Critical_dmg"
							end
							actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, action:GetTargetInfo(i));
                        end				
						Act_Melee_DoDamage(actor,actorTarget,actionResult,action:GetTargetInfo(i));
					end
					
				end
				
				i=i+1;
			
			end
		end
				
		action:GetTargetList():ApplyActionEffects();
		actor:ClearIgnoreEffectList();
	
	else
	
		action:GetTargetList():ApplyOnlyDamage(iTotalHit);
		
		local iTargetCount = action:GetTargetCount();
		local i =0;
		if iTargetCount>0 then
			
			local bWeaponSoundPlayed = false;
			while i<iTargetCount do
			
				local actionResult = action:GetTargetActionResult(i);
				if actionResult:IsNil() == false then
				
					local kTargetGUID = action:GetTargetGUID(i);
					local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
					if kTargetPilot:IsNil() == false and actionResult:IsMissed() == false then
						
						local actorTarget = kTargetPilot:GetActor();
						
	                    if bWeaponSoundPlayed == false then
	                        bWeaponSoundPlayed = true;
                            -- 피격 소리 재생
							local actionName = action:GetID();
							if actionResult:GetCritical() then
								actionName = "Critical_dmg"
							end
							actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, action:GetTargetInfo(i));
                        end							
						
						Act_Melee_DoDamage(actor,actorTarget,actionResult,action:GetTargetInfo(i));
					end
				end
				
				i=i+1;
			
			end
		end
		
	end
	
	
	iCurrentHitCount = iCurrentHitCount + 1;
	action:SetParamInt(3,iCurrentHitCount);

end

function Act_Melee_OnTargetListModified(actor,action,bIsBefore)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	
    if bIsBefore == false then

        if action:GetParam(2) == "HIT" then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            if actor:IsMyActor()  then
				action:GetTargetList():ApplyActionEffects(true,true);
            else
                Act_Melee_HitOneTime(actor,action,false);
            end
        
        end        
    
    end

end

function Act_Melee_OnEvent(actor,textKey,seqID)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local kAction = actor:GetAction();
	ODS("Act_Melee_OnEvent textKey:"..textKey.."\n");
	
	if textKey == "hit" or textKey == "end" then

		local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
		
		local	bIsDoubleAttack = Act_Melee_IsDoubleAttack(actor,kAction);
		if bIsDoubleAttack then
			ODS("Act_Melee_OnEvent bIsDoubleAttack is true\n");
		end
		
		if bIsDoubleAttack == false then
			if textKey ~= "hit" then
				return
			end
		end
		
	
		if bIsDoubleAttack then	--	더블어택일 경우에는 TLM 을 사용할 수 없다.
			bUseTLM = false
		end
		
		if actor:IsUnderMyControl() then
		
			if bUseTLM then
				kAction:CreateActionTargetList(actor);
				if IsSingleMode() then
					Act_Melee_OnTargetListModified(actor,kAction,false)
				else
					kAction:BroadCastTargetListModify(actor:GetPilot());
				end			
			
			end
			
		end
		
		Act_Melee_HitOneTime(actor,kAction,bIsDoubleAttack)
		
	end
	return	true;
end

function IsAbleSubPlayerMeleeAttack(strAtionID)
	if( "a_twin_sub_trace_ground" == strAtionID
		or "a_twin_sub_repos" == strAtionID
		or "a_idle" == strAtionID
		or "a_battle_idle" == strAtionID
	) then
		return true
	end	
	return false
end

function IsAbleSubPlayerMeleeAttack2(kOwnerActor)
	if( nil == kOwnerActor ) then return false end	
	if( kOwnerActor:IsNil() ) then return false end	
	local kSubActor = kOwnerActor:GetSubPlayer()
	if( kSubActor:IsNil() ) then return false end	
	local kSubActorAction = kSubActor:GetAction()
	if( kSubActorAction:IsNil() ) then return false end
	
	return IsAbleSubPlayerMeleeAttack( kSubActorAction:GetID() )
end
