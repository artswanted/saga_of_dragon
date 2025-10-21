-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [HIT_SOUND_ID_MALE] : HIT 키에서 플레이 할 사운드 ID - 남성
-- [HIT_SOUND_ID_FEMALE] : HIT 키에서 플레이 할 사운드 ID - 여성
-- [FIRE_SOUND_ID] : 시전시에 플레이 할 사운드 ID
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
-- [CASTING_EFFECT_ID_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
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
function Skill_MPTransition_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_MPTransition_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_MPTransition_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	action:SetParam(3,"0");
end
function Skill_MPTransition_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("RestoreItemModel Skill_MPTransition_Fire\n");
	
	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
	
	if bUseTLM == false then
		action:CreateActionTargetList(actor);
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
		ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n");
	end
	
	if action:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, true);
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
function Skill_MPTransition_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100
	end
	
	return  Skill_MPTransition_FindTargetEx(actor,action,action:GetActionNo(),iAttackRange,kTargets);
	
end
function    Skill_MPTransition_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)
	
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

function Skill_MPTransition_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_MPTransition_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");

	action:SetParam(3,"0");
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
		action:SetParam(3,"1");
    end	
	
	local	kSoundID = action:GetScriptParam("CASTING_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2785,kSoundID);
	end	
	
	action:SetParamInt(2,0);
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_MPTransition_OnCastingCompleted(actor,action);
	
	if actor:IsMyActor() and actionID == "a_archer_melee" then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
	end
	
	return true
end

function Skill_MPTransition_OnUpdate(actor, accumTime, frameTime)
	
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

function Skill_MPTransition_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
			
    local   kCurAction = actor:GetAction();
	
	CheckNil(nil==kCurAction);
	CheckNil(kCurAction:IsNil());
			
	ODS("RestoreItemModel Skill_MPTransition_ReallyLeave\n");
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

function Skill_MPTransition_OnLeave(actor, action)

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

function Skill_MPTransition_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)

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

function Skill_MPTransition_HitOneTime(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iHitCount = action:GetParamInt(2);
	
	if iHitCount == 0 then
	
	    local   kHitEffectID = action:GetScriptParam("HIT_CASTER_EFFECT_ID");
	    local   kHitEffectTargetNodeID = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE");
	    
	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
	    
	        actor:AttachParticle(7214,kHitEffectTargetNodeID,kHitEffectID);
	    
	    end
	
	
	end

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
function Skill_MPTransition_OnTargetListModified(actor,action,bIsBefore)
	
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
            Skill_MPTransition_HitOneTime(actor,action);
        
        end        
    
    end

end

function Skill_MPTransition_OnEvent(actor,textKey)

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
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
			if bUseTLM then
			
				kAction:CreateActionTargetList(actor);
				if kAction:GetScriptParam("SHOW_TEXT_TARGETLIST_COUNT_NUM") ~= "" then
					local iTargetCount = kAction:GetTargetCount();
					if iTargetCount == tonumber(kAction:GetScriptParam("SHOW_TEXT_TARGETLIST_COUNT_NUM")) then
						AddWarnDataStr(GetTextW(tonumber(kAction:GetScriptParam("SHOW_TEXT_TARGETLIST_TEXTNO"))),2);		
					end
				end

				if IsSingleMode() then
		        
					Skill_MPTransition_HitOneTime(actor,kAction)
					Skill_MPTransition_OnTargetListModified(actor,kAction,false)
		            
					return;
				else
		        
					kAction:BroadCastTargetListModify(actor:GetPilot());
		            
				end			
			
			end	
		end
	
        --Skill_MPTransition_HitOneTime(actor,kAction)

--		local	kProjectileMan = GetProjectileMan();
		local	kArrow = Skill_MPTransition_LoadToWeapon(actor,kAction);
		
		local	targetobject = nil;

		if kArrow:IsNil() == false then
		
			local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			
			local	iTargetCount = kAction:GetTargetCount();
			local	iTargetABVShapeIndex = 0;
			local	kActionTargetInfo = nil
			local	kTargetABVPos = nil;
			if iTargetCount>0 then
					--	타겟을 찾아 바라본다
				local	kTargetGUID = kAction:GetTargetGUID(0);
				iTargetABVShapeIndex = kAction:GetTargetABVShapeIndex(0);
				kActionTargetInfo = kAction:GetTargetInfo(0);
				targetobject = g_pilotMan:FindPilot(kTargetGUID);
				if targetobject:IsNil() == false then
					kTargetABVPos = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
					actor:LookAt(kTargetABVPos);
				end						
			end
			
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
			
			if targetobject == nil or targetobject:IsNil()==true or kActionTargetInfo==nil or kActionTargetInfo:IsNil() == true then				
				kMovingDir:Multiply(iAttackRange);
				kMovingDir:Add(arrow_worldpos);
				kArrow:SetTargetLoc(kMovingDir);				
			else
				kArrow:SetTargetObjectList(kAction:GetTargetList());	
				
				if targetobject ~= nil and targetobject:IsNil() == false then
				
					local	kArrowTargetPos = Point3(kTargetABVPos:GetX(),kTargetABVPos:GetY(),arrow_worldpos:GetZ());
					kArrow:SetTargetLoc(kArrowTargetPos);				
				end				
			end
			
			kArrow:Fire();			
		else			
			ODS("Arrow is nil\n");		
		end
		
		kAction:ClearTargetList();
		actor:ClearIgnoreEffectList();
	end

	return	true;
end

function	Skill_MPTransition_LoadToWeapon(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	
	if kProjectileID == "" then
		kProjectileID = actor:GetEquippedWeaponProjectileID();
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	
	local	kTargetNodeID = action:GetScriptParam("ATTACH_NODE_ID");

	kNewArrow:LoadToHelper(actor,kTargetNodeID);	--	장전
	return kNewArrow;
end
