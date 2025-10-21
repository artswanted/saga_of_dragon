function Skill_Magician_Charge_Attack_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
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

function Skill_Magician_Charge_Attack_OnCastingCompleted(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(5,1);
	actor:CutSkillCasting();
	if actor:IsMyActor() == false then
--		Skill_Magician_Charge_Attack_Fire(actor,action);
	end				
end

function Skill_Magician_Charge_Attack_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(3,0);
	action:SetParamInt(1,0);
	action:SetParamInt(5,0);
				
	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
			return	false;
		end
	end
	
	if action:GetActionParam() == AP_CASTING then
		-- 양손에 캐스팅 이펙트를 붙인다.
		actor:AttachParticle(11,"p_wp_r_hand","ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand");	
		actor:AttachParticle(12,"p_wp_l_hand","ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand");	
		return true;
	end
		
--	action:SetAutoBroadCastWhenCastingCompleted(false);	
	action:SetParamInt(6,0);
	
	--[[if actor:IsMyActor() then
		g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
		, g_fChargeFocusFilterValue[2]
		, g_fChargeFocusFilterValue[3]
		, g_fChargeFocusFilterValue[4]
		,true
		,true)
	end]]--
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
    end
    
   return true
end

function Skill_Magician_Charge_Attack_Fire(actor,action)
		
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(1,1);
	
	actor:PlayNext();
	
	actor:DetachFrom(11);
	actor:DetachFrom(12);
	

	ODS("Skill_Magician_Charge_Attack_Fire ef_mag_charge01_result\n");
	action:SetParamInt(3,0);		
	
	if actor:IsMyActor() == true then
		action:CreateActionTargetList(actor);
		
--		action:SetActionInstanceID();
--		action:BroadCast(actor:GetPilot());
	end

	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
	    GetComboAdvisor():AddNextAction("a_jump");
    end    
    
end

function Skill_Magician_Charge_Attack_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kMovingVec = actor:GetLookingDir();
	local	fDistance = action:GetSkillRange(0,actor);
	kMovingVec:Multiply(fDistance);
	kMovingVec:Add(actor:GetPos());
	kMovingVec:SetZ(kMovingVec:GetZ() - 20)			
	
	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 74
	end
	
	local kParam = FindTargetParam();
	kParam:SetParam_1(kMovingVec,actor:GetLookingDir());
	kParam:SetParam_2(0,30,iParam2,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return	action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
end

function Skill_Magician_Charge_Attack_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction();
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	curSlot = action:GetCurrentSlot();
	local	bKeyCheck = KeyIsDown( KEY_CHARGING_UKEY )
	local	iState = action:GetParamInt(6);
	
	if actor:IsMyActor() == false then
		bKeyCheck = true;
	end
	
	if iState == 0 then
	
		if animDone == true then
			if curSlot == 0 then
				actor:PlayNext();
				local	kMovingVec = actor:GetLookingDir();
				local	fDistance = action:GetSkillRange(0,actor);
				if 0 == fDistance then
					fDistance = 100
				end
				kMovingVec:Multiply(fDistance);
				kMovingVec:Add(actor:GetTranslate());
				kMovingVec:SetZ(kMovingVec:GetZ() - 20)	
				actor:AttachParticleToPoint(9,kMovingVec,"ef_mag_charge01_loop");		
				action:SetParamInt(3,1);
				actor:AttachSound(2784, "PointBust");
			end
			if curSlot == 2 then
				action:SetParamInt(6,1);
				action:SetParamFloat(7,accumTime);
				return true;
			end
		end

		-- 키를 텠고, 캐스팅이 끝났다면 챠지 어택 시전.
		if curSlot == 1 and action:GetParamInt(5) == 1 then
			Skill_Magician_Charge_Attack_Fire(actor,action);
		end	
		
	else
	
		local	fElapsedTime = accumTime - action:GetParamFloat(7);
		if fElapsedTime>=g_fChargingFreezeTime then
			action:SetParam(2,"end");
			return	false;					
		end		
		
	
	end

	return true
end

function Skill_Magician_Charge_Attack_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	actor:DetachFrom(9);
	actor:DetachFrom(11);
	actor:DetachFrom(12);
	
	actor:EndWeaponTrail();
	return true;
end


function Skill_Magician_Charge_Attack_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetEnable() == false then
		return	false;
	end
	
	if action:GetID() == "a_jump" or 
	action:GetID() == "a_MagicianShot_01" or 
	action:GetID() == "a_st_combo_a" or 
	action:GetID() == "a_sp_combo_a" or
	action:GetID() == "a_st_combo_a_Doubleshot" or 
	action:GetID() == "a_st_combo_a_ThreeWay" then
	
		if actor:GetAction():GetParam(10) == "HIT" then
			return true;
		end
	
		return	false;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	
		
	local	curAction = actor:GetAction();
	if curAction:GetParamInt(1) == 1 and curAction:GetParam(2)~="end" then
		return false;
	end

	return true;
end

function Skill_Magician_Charge_Attack_OnEvent(actor,textKey,seqID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()
			
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	
	if action:GetParamInt(1) ~= 1 then
		return true;
	end
	
	if textKey == "hit" then
	
		action:SetParam(10,"HIT");
	
		local	kMovingVec = actor:GetLookingDir();
		local	fDistance = action:GetSkillRange(0,actor);
		if 0 == fDistance then
			fDistance = 100
		end
		kMovingVec:Multiply(fDistance);
		kMovingVec:Add(actor:GetTranslate());
		kMovingVec:SetZ(kMovingVec:GetZ() - 20)	
		actor:DetachFrom(9);
		actor:AttachParticleToPoint(10, kMovingVec, "ef_mag_charge01_result");	
				
		if actor:IsUnderMyControl() then
			action:BroadCastTargetListModify(actor:GetPilot());		
		end
	
	end
	
	return	true;
end

function Skill_Magician_Charge_Attack_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if actor:IsMyActor()  then
			action:GetTargetList():ApplyActionEffects(true,true);
			return;
		end

		local	iTotalHit = tonumber(action:GetScriptParam("MULTI_HIT_NUM"));
		if iTotalHit == nil then
			iTotalHit = 1
		end
		
        if iTotalHit>1 or action:GetParamInt(1) > 0  then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            SkillHelpFunc_DefaultHitOneTime(actor,action, true);
        
        end        
    
    end

end
