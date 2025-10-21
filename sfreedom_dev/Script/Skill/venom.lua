-- Melee
-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)


function Skill_Venom_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
			
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_Venom_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Venom_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function Skill_Venom_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
	
	if bUseTLM == false then
		action:CreateActionTargetList(actor);
	end

	
	actor:HideParts(5,true);
	actor:HideParts(6,true);
	
	actor:AttachParticle(7,"p_wp_l_hand","ef_skill_sterilizer_01_p_wp_l_hand");
	--actor:AttachParticle(8,"char_root","ef_Venom_char_root");		
	--actor:AttachParticle(9,"char_root","ef_Venom_p_ef_sterilizer"); --밑에 이펙트로 바뀜
	actor:AttachParticle(8,"char_root","ef_Venom_Dummy01");
	
	local iInstanceID = GetStatusEffectMan():AddStatusEffectToActor(actor:GetPilotGuid(),"se_venom_mask",0,1);
	action:SetParamInt(3,iInstanceID);
	
	actor:StartWeaponTrail();
end

function Skill_Venom_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 30
	end	
	
	local kParam = FindTargetParam();
	kParam:SetParam_1(actor:GetTranslate(),actor:GetLookingDir());
	kParam:SetParam_2(action:GetSkillRange(0,actor),iParam2,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);		
end

function Skill_Venom_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Venom_OnCastingCompleted(actor,action);
	
	return true
end

function Skill_Venom_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
--	local iComboCount = actor:GetComboCount();
	
	if action:GetParam(1) == "end" then
		return	false;
	end
	
	if animDone == true then
	
	
		local curActionParam = action:GetParam(0)
		if curActionParam == "GoNext" then
			nextActionName = action:GetParam(1);

			action:SetParam(0, "null")
			action:SetParam(1, "end")
			
			action:SetNextActionName(nextActionName)
			return false;
		else
		
			action:SetParam(1, "end")
			return false
		end
	end

	return true
end

function Skill_Venom_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:HideParts(5,false);
	actor:HideParts(6,false);
	actor:DetachFrom(7);
	
	local	kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	local	iInstanceID = kAction:GetParamInt(3);
	GetStatusEffectMan():RemoveStatusEffectFromActor2(actor:GetPilotGuid(),iInstanceID);

	actor:EndWeaponTrail();
	return true;
end

function Skill_Venom_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
		
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if actionID == "a_jump" then 
		return false;
	end
	
	if action:GetActionType()=="EFFECT"  then
		
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end
	
	
	if action:GetEnable() == false then
		
		if curParam==actionID then
			curAction:SetParam(0,"");
		end
		
	end
		
	if action:GetEnable() == true then
	

		curAction:SetParam(0,"GoNext");
		curAction:SetParam(1,actionID);

		return false;
	
	elseif curParam == "end" and (actionID == "a_run_right" or actionID == "a_run_left") then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Skill_Venom_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)

	CheckNil(nil==kActionResult);
	CheckNil(nil==kActionTargetInfo);
	
	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	local kActionID = kAction:GetID()
	
	local	iABVIndex = kActionTargetInfo:GetABVIndex();
	local pt = actorTarget:GetABVShapeWorldPos(iABVIndex);
	
	if kActionResult:GetCritical() then
		actorTarget:AttachParticleToPoint(12, pt, "e_dmg_cri")
	else
		actorTarget:AttachParticleToPoint(12, pt, "e_dmg")
	end	
	
	-- 충격 효과 적용
	local iShakeTime = g_fAttackHitHoldingTime * 1000;
	actor:SetAnimSpeedInPeriod(0.01, iShakeTime);
	actor:SetShakeInPeriod(5, iShakeTime/2);
	actorTarget:SetShakeInPeriod(5, iShakeTime);		
	
	actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, kActionTargetInfo);
	
end
function Skill_Venom_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if action:GetParam(2) == "HIT" then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
			SkillHelpFunc_DefaultHitOneTime(actor,action, true);        
        end        
    
    end

end
function Skill_Venom_DoDamageToTargetList(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local iTargetCount = action:GetTargetCount();
	if iTargetCount>0 then

		local i = 0;
		while i<iTargetCount do
			
			local actionResult = action:GetTargetActionResult(i);
			if actionResult:IsNil() == false then
			
				local kTargetGUID = action:GetTargetGUID(i);
				local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
				if kTargetPilot:IsNil() == false then
					
					local actorTarget = kTargetPilot:GetActor();
					Skill_Venom_DoDamage(actor,actorTarget,actionResult,action:GetTargetInfo(i));
				end
			end
			
			i=i+1;
			
		end
	
	end
	
	if action:GetTargetList():IsNil() == false then
		action:GetTargetList():ApplyActionEffects();
	end
	action:ClearTargetList();		
end

function Skill_Venom_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
		
		actor:AttachSound(9284,"Venom");		
		
		if actor:IsUnderMyControl() then
		
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
			
			if bUseTLM then
			
				kAction:CreateActionTargetList(actor);
				if IsSingleMode() then
					Skill_Venom_OnTargetListModified(actor,kAction,false)
				else
					kAction:BroadCastTargetListModify(actor:GetPilot());
				end			
			
			end
			
		end
		kAction:SetParam(2,"HIT");
				
		--Skill_Venom_DoDamageToTargetList(actor,kAction);
			
	end

	return	true;
end
