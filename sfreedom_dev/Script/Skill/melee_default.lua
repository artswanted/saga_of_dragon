-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [HIT_SOUND_ID_MALE] : HIT 키에서 플레이 할 사운드 ID - 남성
-- [HIT_SOUND_ID_FEMALE] : HIT 키에서 플레이 할 사운드 ID - 여성
-- [FIRE_SOUND_ID] : 시전시에 플레이 할 사운드 ID
-- [CASTING_SOUND_ID] : 캐스팅 시에 플레이 할 사운드 ID
-- [MULTI_HIT_NUM] : 여러번 때리는 경우, 때리는 횟수
-- [FIND_TARGET_TYPE] : 타겟 찾는 방법 : RAY(DEFAULT),BAR,SPHERE
-- [HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS] : HIT 키에서 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE] : HIT 키에서 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [HIT_CASTER_EFFECT_TARGET_NODE] : HIT 키에서 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [HIT_CASTER_EFFECT_ID] : HIT 키에서 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS] : 스킬이 실제 발동될때 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE] : 스킬이 실제 발동될때 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [FIRE_CASTER_EFFECT_TARGET_NODE] : 스킬이 실제 발동될때 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [FIRE_CASTER_EFFECT_ID] : 스킬이 실제 발동될때 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_TARGET_NODE2] : 스킬이 실제 발동될때 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [FIRE_CASTER_EFFECT_ID2] : 스킬이 실제 발동될때 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_ID_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
-- [FIRE_CASTER_EFFECT_ID2_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
-- [CASTING_EFFECT_TARGET_NODE_TO_WORLD_POS] : 캐스팅 시에 이펙트를 붙을때, 붙일 노드
-- [CASTING_EFFECT_TARGET_NODE] : 캐스팅 시에 이펙트를 붙을때, 붙일 노드
-- [CASTING_EFFECT_EFFECT_ID] : 캐스팅 시에 이펙트 붙일 ID
-- [CASTING_EFFECT_ID_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
-- [CASTING_EFFECT_SIZE_TO_SKILLRANGE] : 캐스팅 시에 이펙트 Size를 스킬 범위에 따라 스케일링 할때
-- [CASTING_EFFECT_SIZE_TO_SKILLRANGE_FACTOR] : 캐스팅 시에 이펙트 Size를 스킬 범위에 따라 스케일링 할때 사용하는 Factor
-- [DETACH_FIRE_EFFECT_AT_FINISH] : 스킬이 끝날때 FIRE_CASTER_EFFECT 를 제거할 것인가
-- [DETACH_CASTING_EFFECT_AT_FINISH] : 스킬이 끝날때 FIRE_CASTER_EFFECT 를 제거할 것인가
-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID
-- [NO_DEFAULT_HIT_EFFECT] : 기본 타격 이펙트를 사용하지 않을것인가.
-- [NO_WEAPON_TRAIL] : 웨폰 트레일 사용하지 않을것인가
-- [NO_DAMAGE_HOLD] : 대미지 타이밍에 경직효과를 사용하지 않을것인가
-- [TRANSFORM_XML] : 변신 XML ID
-- [TRANSFORM_ID] : 변신 Status Effect ID
-- [MULTI_HIT_RANDOM_OFFSET] : 데미지를 어려번 나누어 때릴 경우 수치가 뜨는곳에 랜덤 오프셋을 준다.
-- [HIT_TARGET_EFFECT_RANDOM_OFFSET] : 타겟에 붙는 Hit이펙트의 위치에 랜덤 오프셋을 준다.
-- [APPLY_EFFECT_ONLY_LAST_HIT] : 마지막 힛트시에만 이펙트 처리를 해줄것인가.
-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)
-- [HIDE_WEAPON] : 무기를 숨길때
-- [CLEANUP_CASTER_EFFECT_ID] : 클린업시에 붙는 이펙트 ID
-- [CLEANUP_CASTER_EFFECT_TARGET_NODE] : 클린업시에 붙는 이펙트 노드
-- [CLEANUP_CASTER_EFFECT_SCALE] : 클린업시에 붙는 이펙트 크기
-- [SHOW_TEXT_TARGETLIST_COUNT_NUM] : 타겟리스트의 일정 카운트 수가 맞으면
-- [SHOW_TEXT_TARGETLIST_TEXTNO] : 텍스트를 출력

-- Melee
function Act_Melee_Default_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	-- 중국일 경우는 기존 그대로 유지
	local kCurAction = actor:GetAction()
	if( false == kCurAction:IsNil() ) then	
		if "a_telejump" == kCurAction:GetID() or 
			false == (false == actor:IsDownState() and false == actor:IsBlowUp() and true == actor:IsMeetFloor()) or
			"a_jump" == kCurAction:GetID() or 
			"a_dash_jump" == kCurAction:GetID() then
			if "a_Change Ninja" == action:GetID() then
				return false;
			end
		end
	end

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	if action:GetID()=="a_Couple_Lover" then
		return Couple_OnCheckCanEnter(actor, action)
	end

	return		true;
	
end
function Act_Melee_Default_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Act_Melee_Default_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	action:SetParam(3,"0");
end
function Act_Melee_Default_Fire(actor,action)
	--ODS("RestoreItemModel Act_Melee_Default_Fire\n");
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
	
	if bUseTLM == false then
		--ODS("Act_Melee_Default_Fire(actor,action)\n", false, 912)
		action:CreateActionTargetList(actor);
		if action:GetScriptParam("SHOW_TEXT_TARGETLIST_COUNT_NUM") ~= "" then
			local iTargetCount = action:GetTargetCount();
			if iTargetCount == tonumber(action:GetScriptParam("SHOW_TEXT_TARGETLIST_COUNT_NUM")) then
				AddWarnDataStr(GetTextW(tonumber(action:GetScriptParam("SHOW_TEXT_TARGETLIST_TEXTNO"))),2);		
			end
		end
	end

	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
	
	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"));
		--ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n");
	end
	
	if action:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, true);
	end

    local   kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
    local   kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
	local   kFireEffectTargetNodeIDToPos = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS");
	local   kFireEffectTargetNodeIDToPosWithRotate = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE");

    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= ""  or kFireEffectTargetNodeIDToPosWithRotate ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPoint(7212,kDir,kFireEffectID);            
        elseif kFireEffectTargetNodeID ~= "" then
            actor:AttachParticle(7212,kFireEffectTargetNodeID,kFireEffectID);
		elseif kFireEffectTargetNodeIDToPos ~= "" then
			actor:AttachParticleToPoint(7212,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID);			
		else
		    actor:AttachParticleToPointWithRotate(7212,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPosWithRotate), kFireEffectID, actor:GetRotateQuaternion())
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
function Act_Melee_Default_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100
	end
	
	return  Act_Melee_Default_FindTargetEx(actor,action,action:GetActionNo(),iAttackRange,kTargets);
	
end
function    Act_Melee_Default_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;

	local kParam = FindTargetParam();	
		
	local   strFindTargetType = action:GetScriptParam("FIND_TARGET_TYPE");
	if strFindTargetType == "" then
	    strFindTargetType = "RAY"
	end

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	
	local iFindTargetType = TAT_RAY_DIR;
	
	if strFindTargetType == "RAY" then
		if 0==iParam2 then
			iParam2 = 30
		end	
	
		kDir:Multiply(-fBackDistance);
		kPos:Add(kDir);
	    iFindTargetType = TAT_RAY_DIR;
    	kParam:SetParam_1(kPos,actor:GetLookingDir());
    	kParam:SetParam_2(iAttackRange,iParam2,0,0);
	
	elseif strFindTargetType == "BAR" then
		if 0==iParam2 then
			iParam2 = 80
		end	
		kDir:Multiply(-fBackDistance);
		kPos:Add(kDir);
	    iFindTargetType = TAT_BAR;
    	kParam:SetParam_1(kPos,actor:GetLookingDir());
    	kParam:SetParam_2(iAttackRange,iParam2,0,0);
	
	elseif strFindTargetType == "SPHERE" then
		if 0==iParam2 then
			iParam2 = 30
		end	

		iFindTargetType = TAT_SPHERE;

		if 3 == action:GetAbil(AT_1ST_AREA_TYPE) then
			kDir:Multiply(iAttackRange);
			kPos:Add(kDir);
			kParam:SetParam_1(kPos,actor:GetLookingDir());
    		kParam:SetParam_2(0,0,iParam2,0);
		else
			kParam:SetParam_1(actor:GetPos(), actor:GetLookingDir());
    		kParam:SetParam_2(0,iParam2,iAttackRange,0);
		end
	end
	
	kParam:SetParam_3(true,FTO_NORMAL);

	if "a_Raid" == action:GetID() then
		kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN+FTO_BLOWUP);	
	end

	local iTargets = action:FindTargetsEx(iActionNo,iFindTargetType,kParam,kTargets,kTargets);
	
	return iTargets;
end

function Act_Melee_Default_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	--ODS("Act_Melee_Default_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");

	action:SetParam(3,"0");
	local   kFireEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
    local   kFireEffectTargetNodeID = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	local   kFireEffectTargetNodeIDToPos = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE_TO_WORLD_POS");
    
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= "" then
    
		local fSize = 1.0
		if "TRUE" == action:GetScriptParam("CASTING_EFFECT_SIZE_TO_SKILLRANGE") then		
			local fFactor = 100
			local kFactor = action:GetScriptParam("CASTING_EFFECT_SIZE_TO_SKILLRANGE_FACTOR")
			if("" ~= kFactor) then
				fFactor = tonumber(kFactor)				
			end	
			fSize = fSize * (action:GetSkillRange(0,actor) / fFactor)
		end
		
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPointS(7215,kDir,kFireEffectID, fSize);
            
        elseif kFireEffectTargetNodeID ~= "" then
            actor:AttachParticleS(7215,kFireEffectTargetNodeID,kFireEffectID,fSize);
		else
			actor:AttachParticleToPointS(7215,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID,fSize);
        end
		action:SetParam(3,"1");
    end	
	
	local	kSoundID = action:GetScriptParam("CASTING_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2785,kSoundID);
	end

	kSoundID = action:GetScriptParam("ENTER_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end

	local   kMarkEffectID = action:GetScriptParam("MARK_EFFECT_ID");
	if(kMarkEffectID ~= "") then
		local kPos = actor:GetPos()
		--kPos:SetZ(kPos:GetZ()+ 70)
		kPos:SetZ(kPos:GetZ()-20)
		actor:AttachParticleToPointWithRotate(128, kPos, kMarkEffectID, Quaternion(0, Point3(0,0,1)), 5)
	end
	
	action:SetParamInt(2,0);
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Act_Melee_Default_OnCastingCompleted(actor,action);
	
	if actor:IsMyActor() and actionID == "a_archer_melee" then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
	end
	
	return true
end

function Act_Melee_Default_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
--	local actionID = action:GetID();
--	local iComboCount = actor:GetComboCount();
	
	if action:GetParam(1) == "end" then
		return	false;
	end
	
	if animDone == true then
		local nextActionName = action:GetNextActionName()
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

function Act_Melee_Default_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    local   kCurAction = actor:GetAction();
	
	--ODS("RestoreItemModel Act_Melee_Default_ReallyLeave\n");
	--	무기 원래대로
	if kCurAction:GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON);
	end

	if kCurAction:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, false);
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
	
	if kCurAction:GetScriptParam("DETACH_CASTING_EFFECT_AT_FINISH") == "TRUE" then
		local DefaultDetaChID = false;
		if kCurAction:GetScriptParam("CASTING_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID = true;
		end
		actor:DetachFrom(7215,DefaultDetaChID)
	end
		
	local   kCleanUpEffectID = kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_ID");
    local   kCleanUpEffectTargetNodeID = kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_TARGET_NODE");
	local	fCleanUpScale = tonumber(kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_SCALE"));

	if fCleanUpScale == 0 or fCleanUpScale == nil then
		fCleanUpScale = 1.0;
	end
    
    if kCleanUpEffectID ~= "" and kCleanUpEffectTargetNodeID ~= "" then
		 actor:AttachParticleS(7216,kCleanUpEffectTargetNodeID,kCleanUpEffectID, fCleanUpScale);
	end

	if "1" == kCurAction:GetParam(3) then
		--캐스팅 중에 캔슬된 상태
		actor:DetachFrom(7215,true)
		if( false == actor:PlayNext()) then 
			kCurAction:SetNextActionName("a_battle_idle");			
		end
		kCurAction:ChangeToNextActionOnNextUpdate(true,true);
	end
	
-- 모습 원래대로(이걸 사용한후 curAction을 얻어와 쓰면 절대로 안됨!!!)
    local kTransformXMLID = kCurAction:GetScriptParam("TRANSFORM_XML");
    if kTransformXMLID ~= "" then
    
        local   iTransformEffectID = tonumber(kCurAction:GetScriptParam("TRANSFORM_ID"));
        if iTransformEffectID ~= nil then
        
            local   kMan = GetStatusEffectMan();
            kMan:RemoveStatusEffectFromActor(actor:GetPilotGuid(),iTransformEffectID);
        
        end
    
    end

	local   kMarkEffectID = kCurAction:GetScriptParam("MARK_EFFECT_ID");
	if(kMarkEffectID ~= "") then
		actor:DetachFrom(128,true)
	end
	return true;
end

function Act_Melee_Default_OnLeave(actor, action)

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
		if( false == actor:PlayNext()) then 
			actor:GetAction():SetNextActionName("a_battle_idle");
		end
		actor:GetAction():ChangeToNextActionOnNextUpdate(true,true);
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
		actionID == "a_rp_idle" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Act_Melee_Default_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
	
	CheckNil(nil==kActionResult);
	CheckNil(nil==kActionTargetInfo);
	
	local kAction = actor:GetAction()
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
		local   kHitTargetEffectOffset = tonumber(kAction:GetScriptParam("HIT_TARGET_EFFECT_RANDOM_OFFSET"));
		
		if kHitTargetEffectOffset ~= nil then
			pt:SetX(pt:GetX() + math.random(-kHitTargetEffectOffset,kHitTargetEffectOffset))
			pt:SetY(pt:GetY() + math.random(-kHitTargetEffectOffset,kHitTargetEffectOffset))
			pt:SetZ(pt:GetZ() + math.random(-kHitTargetEffectOffset,kHitTargetEffectOffset))
		end

		local	iHitCount = actor:GetAction():GetParamInt(2);

	    if kActionResult:GetCritical() then
		    actorTarget:AttachParticleToPoint(12 + iHitCount, pt, "e_dmg_cri")
	    else
		    actorTarget:AttachParticleToPoint(12 + iHitCount, pt, "e_dmg")
	    end	
	    
	    
	    if kAction:GetScriptParam("NO_DAMAGE_HOLD") ~= "TRUE" then
    	    
	        -- 충격 효과 적용
	        local iShakeTime = g_fAttackHitHoldingTime * 1000;
	        actor:SetAnimSpeedInPeriod(0.01, iShakeTime);
	        actor:SetShakeInPeriod(5, iShakeTime/2);
	        actorTarget:SetShakeInPeriod(5, iShakeTime);		
	    end
	end
end
function ActMelee_Default_AttachParticleOnHit(actor,action)
	
	if( CheckNil(nil==actor)  ) then return  end
	if( CheckNil(actor:IsNil()) ) then return  end
	
	if( CheckNil(nil==action) ) then return  end
	if( CheckNil(action:IsNil()) ) then return  end
	
	local	iHitCount = action:GetParamInt(2);
	
	if iHitCount == 0 then
	
	    local   kHitEffectID = action:GetScriptParam("HIT_CASTER_EFFECT_ID");
	    local   kHitEffectTargetNodeID = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE");
		local   kHitEffectTargetNodeIDToPos = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS");
		local   kHitEffectTargetNodeIDToPosWithRotate = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE");
		local 	iHitEffectSizeFactor = action:GetScriptParam("HIT_CASTING_EFFECT_SIZE_TO_SKILLRANGE_FACTOR");
		local fSize = 1
		if("" ~= iHitEffectSizeFactor) then 
			iHitEffectSizeFactor = tonumber(iHitEffectSizeFactor)
			fSize = fSize * (action:GetSkillRange(0,actor) / iHitEffectSizeFactor)
		end				
	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" or kHitEffectTargetNodeIDToPos ~= ""  or kHitEffectTargetNodeIDToPosWithRotate ~= "" then
			if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
				actor:AttachParticleS(7214,kHitEffectTargetNodeID,kHitEffectID, fSize);
			elseif kHitEffectTargetNodeID ~= "" then
	            actor:AttachParticleS(7214,kHitEffectTargetNodeID,kHitEffectID, fSize);
			elseif kHitEffectTargetNodeIDToPos ~= "" then
				actor:AttachParticleToPointS(7214,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPos),kHitEffectID, fSize);
			else
			    actor:AttachParticleToPointWithRotate(7214,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPosWithRotate), kHitEffectID, actor:GetRotateQuaternion(), fSize)
		    end
		end
		
		
	    kHitEffectID = action:GetScriptParam("HIT_CASTER_EFFECT_ID2");
	    kHitEffectTargetNodeID = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE2");
		kHitEffectTargetNodeIDToPos = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS2");
		kHitEffectTargetNodeIDToPosWithRotate = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE2");
		iHitEffectSizeFactor = action:GetScriptParam("HIT_CASTING_EFFECT_SIZE_TO_SKILLRANGE_FACTOR2");
		
		local fSize = 1
		if("" ~= iHitEffectSizeFactor) then 
			iHitEffectSizeFactor = tonumber(iHitEffectSizeFactor)
			fSize = fSize * (action:GetSkillRange(0,actor) / iHitEffectSizeFactor)
		end		
		
	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" or kHitEffectTargetNodeIDToPos ~= ""  or kHitEffectTargetNodeIDToPosWithRotate ~= "" then
			if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
				actor:AttachParticleS(7217,kHitEffectTargetNodeID,kHitEffectID,fSize);
			elseif kHitEffectTargetNodeID ~= "" then
	            actor:AttachParticleS(7217,kHitEffectTargetNodeID,kHitEffectID,fSize);
			elseif kHitEffectTargetNodeIDToPos ~= "" then
				actor:AttachParticleToPoint(7217,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPos),kHitEffectID,fSize);			
			else
			    actor:AttachParticleToPointWithRotate(7217,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPosWithRotate), kHitEffectID, actor:GetRotateQuaternion(),fSize)
		    end
		end
	end
	
end

function Act_Melee_Default_HitOneTime(actor,action)
	
	if( CheckNil(nil==actor)  ) then return  end
	if( CheckNil(actor:IsNil()) ) then return  end
	
	if( CheckNil(nil==action) ) then return  end
	if( CheckNil(action:IsNil()) ) then return  end
	
	local	iHitCount = action:GetParamInt(2);
	ActMelee_Default_AttachParticleOnHit(actor,action)
	
	local	iTotalHit = tonumber(action:GetScriptParam("MULTI_HIT_NUM"));
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
function Act_Melee_Default_OnTargetListModified(actor,action,bIsBefore)

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
		
        if iTotalHit>1 or action:GetParamInt(2) > 0  then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            Act_Melee_Default_HitOneTime(actor,action);
        
        end        
    
    end

end

function Act_Melee_Default_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
		
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());

	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
		SkillHelpFunc_PlayHitSound(actor, kAction);
		
		if actor:IsUnderMyControl() then
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
			if bUseTLM then
				--ODS("Act_Melee_Default_OnEvent(actor,action)\n", false, 912)
				kAction:CreateActionTargetList(actor);
				if kAction:GetScriptParam("SHOW_TEXT_TARGETLIST_COUNT_NUM") ~= "" then
					local iTargetCount = kAction:GetTargetCount();
					if iTargetCount == tonumber(kAction:GetScriptParam("SHOW_TEXT_TARGETLIST_COUNT_NUM")) then
						AddWarnDataStr(GetTextW(tonumber(kAction:GetScriptParam("SHOW_TEXT_TARGETLIST_TEXTNO"))),2);		
					end
				end

				if IsSingleMode() then
		        
					Act_Melee_Default_HitOneTime(actor,kAction)
					Act_Melee_Default_OnTargetListModified(actor,kAction,false)
		            
					return;
				else
		        
					kAction:BroadCastTargetListModify(actor:GetPilot());
		            
				end			
			
			end	
		end
		ActMelee_Default_AttachParticleOnHit(actor,kAction)       
	end

	return	true;
end
