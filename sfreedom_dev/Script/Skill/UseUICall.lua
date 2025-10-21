-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [CASTING_SOUND_ID] : 캐스팅 시에 플레이 할 사운드 ID
-- [MULTI_HIT_NUM] : 여러번 때리는 경우, 때리는 횟수
-- [FIND_TARGET_TYPE] : 타겟 찾는 방법 : RAY(DEFAULT),BAR,SPHERE
-- [HIT_CASTER_EFFECT_TARGET_NODE] : HIT 키에서 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [HIT_CASTER_EFFECT_ID] : HIT 키에서 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_TARGET_NODE] : 스킬이 실제 발동될때 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [FIRE_CASTER_EFFECT_ID] : 스킬이 실제 발동될때 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_TARGET_NODE2] : 스킬이 실제 발동될때 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [FIRE_CASTER_EFFECT_ID2] : 스킬이 실제 발동될때 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_ID_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
-- [FIRE_CASTER_EFFECT_ID2_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
-- [CASTING_EFFECT_TARGET_NODE] : 캐스팅 시에 이펙트를 붙을때, 붙일 노드
-- [CASTING_EFFECT_EFFECT_ID] : 캐스팅 시에 이펙트 붙일 ID
-- [DETACH_FIRE_EFFECT_AT_FINISH] : 스킬이 끝날때 FIRE_CASTER_EFFECT 를 제거할 것인가
-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID
-- [NO_WEAPON_TRAIL] : 웨폰 트레일 사용하지 않을것인가
-- [TRANSFORM_XML] : 변신 XML ID
-- [TRANSFORM_ID] : 변신 Status Effect ID
-- [CALL_UI_NAME] : 호출할 UI 이름

-- Melee
function Act_Use_UICall_OnCheckCanEnter(actor,action)
	return		true;
end

function Act_Use_UICall_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Act_Use_UICall_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function Act_Use_UICall_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("RestoreItemModel Act_Use_UICall_Fire\n");
	
	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
	
	if bUseTLM == false then
		action:CreateActionTargetList(actor);
	end

	
	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"));
		ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n");
	end
	
    local   kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
    local   kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
    
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPoint(7212,kDir,kFireEffectID);
            
        else
            actor:AttachParticle(7212,kFireEffectTargetNodeID,kFireEffectID);
        end
    
    end	

    kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID2");
    kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE2");
	
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPoint(7213,kDir,kFireEffectID);
            
        else
            actor:AttachParticle(7213,kFireEffectTargetNodeID,kFireEffectID);
        end
    
    end	
		
	if action:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:StartWeaponTrail();
	end
	
	--  변신 적용
    local kTransformXMLID = action:GetScriptParam("TRANSFORM_XML");
    if kTransformXMLID ~= "" then
    
        local   iTransformEffectID = tonumber(action:GetScriptParam("TRANSFORM_ID"));
        if iTransformEffectID ~= nil then
        
            local   kMan = GetStatusEffectMan();
            kMan:AddStatusEffectToActor(actor:GetPilotGuid(),kTransformXMLID,iTransformEffectID,0);
        
        end
    
    end
    
	
end
function Act_Use_UICall_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100
	end
	
	return  Act_Use_UICall_FindTargetEx(actor,action,action:GetActionNo(),iAttackRange,kTargets);
	
end
function    Act_Use_UICall_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)
	
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

function Act_Use_UICall_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();

	local kUIName = action:GetScriptParam("CALL_UI_NAME");

	if actor:IsMyActor() then
		CallUI(kUIName);
	end
	
	ODS("Act_Use_UICall_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");

	local   kFireEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
    local   kFireEffectTargetNodeID = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
    
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPoint(7215,kDir,kFireEffectID);
            
        else
            actor:AttachParticle(7215,kFireEffectTargetNodeID,kFireEffectID);
        end
    
    end	
	
	local	kSoundID = action:GetScriptParam("CASTING_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2785,kSoundID);
	end	
	
	action:SetParamInt(2,0);
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Act_Use_UICall_OnCastingCompleted(actor,action);
	
	if actor:IsMyActor() and actionID == "a_archer_melee" then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
	end
	
	return true
end

function Act_Use_UICall_OnUpdate(actor, accumTime, frameTime)
	
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

function Act_Use_UICall_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    local   kCurAction = actor:GetAction();
	
	CheckNil(nil==kCurAction);
	CheckNil(kCurAction:IsNil());
	
    
	ODS("RestoreItemModel Act_Use_UICall_ReallyLeave\n");
	--	무기 원래대로
	if kCurAction:GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON);
	end


	if kCurAction:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:EndWeaponTrail();
	end
	
	if kCurAction:GetScriptParam("DETACH_FIRE_EFFECT_AT_FINISH") == "TRUE" then
	
		local DefaultDetaChID = false;
		local DefaultDetaChID2 = false;
		if kCurAction:GetScriptParam("FIRE_CASTER_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID = true;
		end

		if kCurAction:GetScriptParam("FIRE_CASTER_EFFECT_ID2_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID2 = true;
		end		

	    actor:DetachFrom(7212,DefaultDetaChID)
	    actor:DetachFrom(7213,DefaultDetaChID2)
		actor:DetachFrom(7214)
	end
	
	--  모습 원래대로 (이걸 사용한후 curAction을 얻어와 쓰면 절대로 안됨!!!)
    local kTransformXMLID = kCurAction:GetScriptParam("TRANSFORM_XML");
    if kTransformXMLID ~= "" then
    
        local   iTransformEffectID = tonumber(kCurAction:GetScriptParam("TRANSFORM_ID"));
        if iTransformEffectID ~= nil then
        
            local   kMan = GetStatusEffectMan();
            kMan:RemoveStatusEffectFromActor(actor:GetPilotGuid(),iTransformEffectID);
        
        end
    
    end
    	
	return true;
end

function Act_Use_UICall_OnLeave(actor, action)
	return true
end

function Act_Use_UICall_OnTargetListModified(actor,action,bIsBefore)
    
end

function Act_Use_UICall_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
	
		if actor:IsUnderMyControl() then
		end	
	end

	return	true;
end
