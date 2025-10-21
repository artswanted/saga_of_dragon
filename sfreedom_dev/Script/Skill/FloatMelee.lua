-- Melee
-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)

function Act_FloatMelee_OnCastingCompleted(actor, action)
end

function Act_FloatMelee_OnCheckCanEnter(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	
	if actor:IsMeetFloor() then
		return	false
	end
	
	if actor:GetFloatHeight(30)<40 then
		ODS("Act_FloatMelee_OnCheckCanEnter actor:GetFloatHeight(30):"..actor:GetFloatHeight(30).."\n");
		return	false;	
	end
	
	if actor:GetParam("FLOAT_ATTACK_ENABLE")=="FALSE" then
		return false;
	end

	return		true;
	
end

function Act_FloatMelee_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Act_FloatMelee_OnEnter action : .."..action:GetID().."\n");

	if actor:IsMyActor() == true then
		action:SetParam(4, "");
		
		if action:GetID() ~= "a_float_melee_01" and
		    action:GetID() ~= "a_float_melee_02" and
		    action:GetID() ~= "a_float_melee_03" and
		    action:GetID() ~= "a_fig_floatdownblow1" and
			action:GetID() ~= "a_fig_floatdownblow2" and
            action:GetID() ~= "a_MagicianFloatShot_01" and
		    action:GetID() ~= "a_MagicianFloatShot_02" and
		    action:GetID() ~= "a_MagicianFloatShot_03" and
			action:GetID() ~= "a_magefloatshot_01" and
			action:GetID() ~= "a_magefloatshot_02" and
			action:GetID() ~= "a_magefloatshot_03" and
            action:GetID() ~= "a_nj_float_melee_01" and
		    action:GetID() ~= "a_nj_float_melee_02" and
		    action:GetID() ~= "a_nj_float_melee_03" and		   		    
		    action:GetID() ~= "a_nj_float_melee_04" and		   		    
		    action:GetID() ~= "a_nj_float_melee_05" and
			-- 격투가
			action:GetID() ~= "a_twin_float_melee_01" and
			action:GetID() ~= "a_twin_float_melee_02" and
			action:GetID() ~= "a_twin_float_melee_03"
		    then
		    
			actor:SetComboCount(0);	
			action:ChangeToNextActionOnNextUpdate(true,true);					
			return true;		    
		    
		end
		
		local iBaseClassID = actor:GetPilot():GetBaseClassID();
		if iBaseClassID == CT_FIGHTER then
			actor:SetNormalAttackActionID("a_melee_01");
		elseif iBaseClassID == CT_MAGICIAN then
			actor:SetNormalAttackActionID("a_MagicianShot_01");
		elseif iBaseClassID == CT_ARCHER then
			actor:SetNormalAttackActionID("a_archer_shot_01");
		elseif iBaseClassID == CT_THIEF then
			actor:SetNormalAttackActionID("a_thi_melee_01");
		elseif iBaseClassID == CT_DOUBLE_FIGHTER then
			actor:SetNormalAttackActionID("a_twin_melee_01_m");
		end

		local	iComboCount = actor:GetComboCount();
		local	iNextComboCount = iComboCount+1;

		ODS("Act_FloatMelee_OnEnter iComboCount : "..iComboCount.."\n");
		
		local	iMaxComboCount = 3
		if actor:CheckStatusEffectExist("se_transform_to_metamorphosis") then
			iMaxComboCount = 5;
		end
		
		if iNextComboCount >= iMaxComboCount then
			iNextComboCount = 0
			actor:SetParam("FLOAT_ATTACK_ENABLE","FALSE");
		end

		actor:SetComboCount(iNextComboCount);
		
		action:SetParam(0,"");
		
		local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
		
		if bUseTLM == false then
			action:CreateActionTargetList(actor);
		end
		
		GetComboAdvisor():OnNewActionEnter(action:GetID());
				
		if iBaseClassID == CT_FIGHTER or iBaseClassID == CT_THIEF then
		    GetComboAdvisor():AddNextAction("a_melee_drop");
    		
		    if action:GetID() == "a_float_melee_01" then
		        GetComboAdvisor():AddNextAction("a_float_melee_02");
		    elseif action:GetID() == "a_float_melee_02" then
		        GetComboAdvisor():AddNextAction("a_float_melee_03");

				local kActionName = "a_fig_floatdownblow";
				if actor:GetPilot():IsCorrectClass(CT_KNIGHT,true) then
					kActionName = kActionName.."1";
					GetComboAdvisor():AddNextAction(kActionName);
				elseif actor:GetPilot():IsCorrectClass(CT_WARRIOR,true) then
					kActionName = kActionName.."2";
					GetComboAdvisor():AddNextAction(kActionName);
				end		        
		    end
    		
		elseif iBaseClassID == CT_MAGICIAN then
		    GetComboAdvisor():AddNextAction("a_magician_down_shot");
		    if action:GetID() == "a_MagicianFloatShot_01" then
		        GetComboAdvisor():AddNextAction("a_MagicianFloatShot_02");
		    elseif action:GetID() == "a_MagicianFloatShot_02" then
		        GetComboAdvisor():AddNextAction("a_MagicianFloatShot_03");
		    end
		elseif iBaseClassID == CT_DOUBLE_FIGHTER then
		    GetComboAdvisor():AddNextAction("");
		    if action:GetID() == "a_twin_float_melee_01" then
		        GetComboAdvisor():AddNextAction("a_twin_float_melee_02");
		    elseif action:GetID() == "a_twin_float_melee_02" then
		        GetComboAdvisor():AddNextAction("a_twin_float_melee_03");
		    end
		end
		
	end
	
	actor:ResetAnimation();
	actor:FreeMove(true);	
	actor:StopJump();
	
	if actor:GetPilot():GetBaseClassID() == CT_THIEF then
		actor:StartWeaponTrail();
	end

	actor:SetParam("DOUBLE_JUMP","FALSE"); -- 공격하면 더블 점프 불가능
	
	return true
end

function Act_FloatMelee_OnFindTarget(actor,action,kTargets)

	if( CheckNil(nil==actor) ) then return 0 end
	if( CheckNil(actor:IsNil()) ) then return 0 end
	
	if( CheckNil(nil==action) ) then return 0 end
	if( CheckNil(action:IsNil()) ) then return 0 end
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 60
	end
	
	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(action:GetSkillRange(0,actor),iParam2,0,0);
	kParam:SetParam_3(true,FTO_BLOWUP);
	
	local iTargetCount = action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
	ODS("Act_FloatMelee_OnFindTarget iTargetCount:"..iTargetCount.."\n");
	
	return	iTargetCount;
	
end

function Act_FloatMelee_Finished(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local	actionID = action:GetID();
--	local iComboCount = actor:GetComboCount();
	local nextActionName = action:GetNextActionName()
	
	actor:SetNormalAttackEndTime();
	
	nextActionName = action:GetParam(1);

			
	local curActionParam = action:GetParam(0)
	if curActionParam == "GoNext" then
		
		action:SetNextActionName(nextActionName)
		
		if nextActionName ~= "a_melee_01" and 
			nextActionName ~= "a_thi_melee_01" and
			nextActionName ~= "a_MagicianShot_01" and
			nextActionName ~= "a_twin_melee_01_m" then
			actor:SetComboCount(0)
			actor:FreeMove(false);
			ODS("actor:SetComboCount(0) 0\n");
			actor:SetParam("FLOAT_ATTACK_ENABLE","FALSE");
		end
	else
		actor:FreeMove(false);
	end
	
	if action:GetParam(2) ~= "ATTACKED" then
		actor:SetComboCount(0)
		actor:FreeMove(false);
		actor:SetParam("FLOAT_ATTACK_ENABLE","FALSE");
		
		ODS("actor:SetComboCount(0) 1\n");
	end
	
	
	action:SetParam(0, "null")
	action:SetParam(1, "end")

	return false
end

function Act_FloatMelee_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local animDone = actor:IsAnimationDone()
	
	if action:GetParam(1) == "end" then
		return	false;
	end
	
	if animDone == true then
		return Act_FloatMelee_Finished(actor,action);
	end

	return true
end

function Act_FloatMelee_OnCleanUp(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	actor:FreeMove(false);
	actor:EndWeaponTrail();

	if action:GetID() == "a_jump" then
		action:SetSlot(2)
		action:SetDoNotBroadCast(true)
	end

	return true;
end

function Act_FloatMelee_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local curAction = actor:GetAction();
	
	if( CheckNil(nil==curAction) ) then return true end
	if( CheckNil(curAction:IsNil()) ) then return true end
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if curParam == "end" and actionID == "a_jump" then 
		action:SetSlot(2)
		action:SetDoNotBroadCast(true)
		return  true;
	end
	
	if action:GetActionType()=="EFFECT" then
		
		actor:SetComboCount(0);	
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end
	
	if action:GetEnable() == false then
		
		if kNextActionName==actionID then
			curAction:SetParam(0,"");
		end
		
	end
	
	if action:GetEnable() == true then
	
		local	bCorrect = false;
		
		if 	actionID == "a_run" then
			
			bCorrect = false;
			
		else
			if curAction:GetParam(2) == "ATTACKED" then
			
				bCorrect = true;
			
			end
		end
	
		if bCorrect then
	
			curAction:SetParam(0,"GoNext");
			curAction:SetParam(1,actionID);
			
		end

		return false;
	
	elseif curParam == "end" and actionID == "a_run" then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Act_FloatMelee_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)

	CheckNil(nil==kActionResult);
	CheckNil(nil==kActionTargetInfo);
	
	local kAction = actor:GetAction();
			
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	local	iSphereIndex = kActionTargetInfo:GetABVIndex();
	local pt = actorTarget:GetABVShapeWorldPos(iSphereIndex);
	
	local	iBaseClassID = actor:GetPilot():GetBaseClassID();
	if iBaseClassID == CT_FIGHTER  or
		iBaseClassID == CT_THIEF then
	
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
	end
	
end
function Act_FloatMelee_DoDamageToTargetList(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end	

	SkillHelpFunc_DefaultHitOneTime(actor,action, true);
end

function Act_FloatMelee_OnTargetListModified(actor,action,bIsBefore)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
    if bIsBefore == false then

        if action:GetParam(2) == "ATTACKED" then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            if actor:IsMyActor()  then
				action:GetTargetList():ApplyActionEffects(true,true);
            else
                Act_FloatMelee_DoDamageToTargetList(actor,action);
            end
        
        end        
    
    end

end

function Act_FloatMelee_OnEvent(actor,textKey,seqID)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	local kAction = actor:GetAction();
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	

	if textKey == "fire_end" then
	
		actor:DetachFrom(10);
	
	end

	if ( textKey == "hit" or textKey == "fire" )and kAction:GetParamInt(20) == 0 then
	
		if actor:IsUnderMyControl() then
		
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
			if bUseTLM then
			
				kAction:CreateActionTargetList(actor);
				if IsSingleMode() then
					Act_FloatMelee_OnTargetListModified(actor,kAction,false)
				else
					kAction:BroadCastTargetListModify(actor:GetPilot());
				end			
			
			end
			
		end
	
	
		kAction:SetParam(2,"ATTACKED");		
		
		kAction:SetParamInt(20,1);	--	hit 키가 두개 있네!! 이런, 그래도 한번만 때리게 하자.
	
		local iBaseClassID = actor:GetPilot():GetBaseClassID();
	
		if  iBaseClassID == CT_FIGHTER then
					
			if kAction:GetID() == "a_float_melee_03" and kAction:GetTargetCount()>0 then
				if actor:IsMyActor() then
					QuakeCamera(g_fMeleeDropQuakeValue[1]
					, g_fMeleeDropQuakeValue[2]
					, g_fMeleeDropQuakeValue[3]
					, g_fMeleeDropQuakeValue[4]
					, g_fMeleeDropQuakeValue[5])
				end	
			end			
			if kAction:GetID() == "a_float_melee_01" then
				actor:AttachParticle(32,"char_root","edge_swd_01_01");
			elseif kAction:GetID() == "a_float_melee_02" then
				actor:AttachParticle(33,"char_root","edge_swd_01_02");
			else
				actor:AttachParticle(34,"char_root","edge_swd_01_07");
			end
		elseif iBaseClassID == CT_MAGICIAN then
		
			local actionName = kAction:GetID()
			
			if string.sub(actionName, 1, 19) == "a_MagicianFloatShot" then
				actor:AttachParticle(10,"char_root","ef_mag_lightning_char_root");
			else
				local ActorPos = actor:GetNodeWorldPos("char_root");
				local ActorQuat = actor:GetRotateQuaternion();
				local kRotateAxis = Point3(0, 0, 1);
				local fRotateAngle = 20.0 * math.pi/180.0;
				
				local CtQuat = ActorQuat:Multiply(Quaternion(0, kRotateAxis));
				local LtQuat = ActorQuat:Multiply(Quaternion(-fRotateAngle, kRotateAxis));
				local RtQuat = ActorQuat:Multiply(Quaternion(fRotateAngle, kRotateAxis));
			
				--actor:AttachParticleToPoint(10, ActorPos, "ef_mag_lightning_char_root");
				actor:AttachParticleToPointWithRotate(10,ActorPos,"ef_mag_lightning_char_root",CtQuat,1.0);
				actor:AttachParticleToPointWithRotate(11,ActorPos,"ef_mag_lightning_char_root",LtQuat,1.0);
				actor:AttachParticleToPointWithRotate(12,ActorPos,"ef_mag_lightning_char_root",RtQuat,1.0);
			end
		end
	
		Act_FloatMelee_DoDamageToTargetList(actor,kAction);
		
	end
	return	true;
end
