-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [MULTI_HIT_NUM] : 여러번 때리는 경우, 때리는 횟수
-- [FIND_TARGET_TYPE] : 타겟 찾는 방법 : RAY(DEFAULT),BAR,SPHERE
-- [HIT_CASTER_EFFECT_TARGET_NODE] : HIT 키에서 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [HIT_CASTER_EFFECT_ID] : HIT 키에서 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_TARGET_NODE] : 스킬이 실제 발동될때 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [FIRE_CASTER_EFFECT_ID] : 스킬이 실제 발동될때 캐스터에게 붙일 이펙트 ID
-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID
-- [NO_DEFAULT_HIT_EFFECT] : 기본 타격 이펙트를 사용하지 않을것인가.
-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)

-- Melee
function Skill_Roulette_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	return		true;
	
end
function Skill_Roulette_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Roulette_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	
end
function Skill_Roulette_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--ODS("RestoreItemModel Skill_Roulette_Fire\n");

	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
	
	if bUseTLM == false then
		action:CreateActionTargetList(actor);
	end
	
	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"));
		--ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n");
	end   
			
	actor:StartWeaponTrail();	
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
    action:SetParamFloat(4,g_world:GetAccumTime());
	
end
function Skill_Roulette_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100
	end
	
	return  Skill_Roulette_FindTargetEx(actor,action,action:GetActionNo(),iAttackRange,kTargets);
	
end
function    Skill_Roulette_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);

	local kParam = FindTargetParam();
	
	local   strFindTargetType = action:GetScriptParam("FIND_TARGET_TYPE");
	if strFindTargetType == "" then
	    strFindTargetType = "RAY"
	end
	
	local iFindTargetType = TAT_RAY_DIR;
	
	if strFindTargetType == "RAY" then
	
	    iFindTargetType = TAT_RAY_DIR;
    	kParam:SetParam_1(kPos,actor:GetLookingDir());
    	kParam:SetParam_2(iAttackRange,30,0,0);
	
	elseif strFindTargetType == "BAR" then
	    iFindTargetType = TAT_BAR;
    	kParam:SetParam_1(kPos,actor:GetLookingDir());
    	kParam:SetParam_2(iAttackRange,60,0,0);
	
	elseif strFindTargetType == "SPHERE" then
	    iFindTargetType = TAT_SPHERE;
    	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
    	kParam:SetParam_2(0,30,iAttackRange,0);
	
	end
	
	kParam:SetParam_3(true,FTO_NORMAL);
	local iTargets = action:FindTargetsEx(iActionNo,iFindTargetType,kParam,kTargets,kTargets);
	
	return iTargets;
end

function Skill_Roulette_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();

	local   kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
    local   kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
    
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
		actor:AttachParticle(7212,kFireEffectTargetNodeID,kFireEffectID);
    end	
	
	--ODS("Skill_Roulette_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	action:SetParamInt(2,0);
	action:SetParamFloat(4,-1);
		
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Roulette_OnCastingCompleted(actor,action);
	
	if actor:IsMyActor() and actionID == "a_archer_melee" then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
	end
	
	return true
end

function Skill_Roulette_OnUpdate(actor, accumTime, frameTime)
	
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

function Skill_Roulette_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    actor:DetachFrom(2889);

	--ODS("RestoreItemModel Skill_Roulette_ReallyLeave\n");
	--	무기 원래대로
	if actor:GetAction():GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON);
	end

	actor:EndWeaponTrail();
	return true;
end

function Skill_Roulette_OnLeave(actor, action)

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
	
	if action:GetActionType()=="EFFECT" then
		
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end
	
	if action:GetEnable() == false then
		if curParam == actionID then
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

function Skill_Roulette_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
	
	CheckNil(nil==kActionResult);
	CheckNil(nil==kActionTargetInfo);
	
	local kAction = actor:GetAction()
		
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	local kActionID = kAction:GetID()
	
	if actor:GetPilotGuid():IsEqual(actorTarget:GetPilotGuid()) then
		return
	end
	
	local	iABVIndex = kActionTargetInfo:GetABVIndex();
	local pt = actorTarget:GetABVShapeWorldPos(iABVIndex);
	
    local   kHitTargetEffectID = kAction:GetScriptParam("HIT_TARGET_EFFECT_ID");
    local   kHitTargetEffectTargetNodeID = kAction:GetScriptParam("HIT_TARGET_EFFECT_TARGET_NODE");
    
    if kHitTargetEffectID ~= "" and kHitTargetEffectTargetNodeID ~= "" then
    
        actorTarget:AttachParticle(7211,kHitTargetEffectTargetNodeID,kHitTargetEffectID);
    
    end	
	
	
	if kAction:GetScriptParam("NO_DEFAULT_HIT_EFFECT") ~= "TRUE" then
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
    	
	end
	
    -- 피격 소리 재생
	local actionName = kActionID
	if kActionResult:GetCritical() then
		actionName = "Critical_dmg"
	end
	actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, kActionTargetInfo);
end

function Skill_Roulette_HitOneTime(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iHitCount = action:GetParamInt(2);
	
	if iHitCount == 0 then
	
	    local   kHitEffectID = action:GetScriptParam("HIT_CASTER_EFFECT_ID");
	    local   kHitEffectTargetNodeID = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE");
	    
	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
	        actor:AttachParticle(7213,kHitEffectTargetNodeID,kHitEffectID);
	    end
	end

	local iDiceValue = -1;
	local iTargetCount = action:GetTargetCount();
	if iTargetCount>0 then
		local actionResult = action:GetTargetActionResult(0);	
		if actionResult:IsNil() == false then
		    iDiceValue = actionResult:GetAbil(AT_DICE_VALUE);
		end
    end

	--ODS("AT_DICE_VALUE : "..iDiceValue.."\n");

	if iDiceValue == 0 then
		actor:AttachParticleS(7214,"p_ef_star","ef_gamble_002_1_p_ef_star",2.0);
	elseif iDiceValue == 1 then
		actor:AttachParticleS(7214,"p_ef_star","ef_gamble_002_2_p_ef_star",2.0);
	elseif iDiceValue == 2 then
		actor:AttachParticleS(7214,"p_ef_star","ef_gamble_002_3_p_ef_star",2.0);
	elseif iDiceValue == 3 then
		actor:AttachParticleS(7214,"p_ef_star","ef_gamble_002_4_p_ef_star",2.0);
	elseif iDiceValue == 4 then
		actor:AttachParticleS(7214,"p_ef_star","ef_gamble_002_5_p_ef_star",2.0);
	elseif iDiceValue == 5 then
		actor:AttachParticleS(7214,"p_ef_star","ef_gamble_002_6_p_ef_star",2.0);
	end

	local	iTotalHit = tonumber(action:GetScriptParam("MULTI_HIT"));
	if iTotalHit == nil or iTotalHit == 0 then
	    iTotalHit = 1;
	end
	
	if iHitCount == iTotalHit then
		return
	end
	
	SkillHelpFunc_DefaultHitOneTime(actor,action, true);
	
	iHitCount = iHitCount + 1;
	action:SetParamInt(2,iHitCount);
	
end
function Skill_Roulette_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
        if action:GetParamInt(2) >0 then        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            if actor:IsMyActor()  then
				action:GetTargetList():ApplyActionEffects(true,true);
            else
                Skill_Roulette_HitOneTime(actor,action);
            end        
        end    
    end
end
function Skill_Roulette_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then

		actor:DetachFrom(7212,true);
		actor:AttachParticle(7210, "p_ef_star", "ef_gamble_003_p_ef_star");
	
		if actor:IsUnderMyControl() then
		
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
			if bUseTLM then
			
				kAction:CreateActionTargetList(actor);
				if IsSingleMode() then
					Skill_Roulette_OnTargetListModified(actor,kAction,false)
				else
					kAction:BroadCastTargetListModify(actor:GetPilot());
				end			
			
			end
			
		end
		
        Skill_Roulette_HitOneTime(actor,kAction)
	end

	return	true;
end
