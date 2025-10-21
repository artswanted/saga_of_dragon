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
function Skill_CrossBlade_Play_OnCheckCanEnter(kActor,kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())

	return		true
	
end
function Skill_CrossBlade_Play_OnCastingCompleted(kActor, kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	Skill_CrossBlade_Play_Fire(kActor,kAction)
	kAction:SetParamInt(10, math.random(0, 1))
	--local iSlotCount = kAction:GetSlotCount()
	kAction:SetSlot(kAction:GetParamInt(10))
	kActor:PlayCurrentSlot()
	kAction:SetParam(3,"0")	
--	kActor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",500,100,500)
--	kActor:StartWeaponTrail()
end
function Skill_CrossBlade_Play_Fire(kActor,kAction)
	ODS("RestoreItemModel Skill_CrossBlade_Play_Fire\n")
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	local	kSoundID = kAction:GetScriptParam("FIRE_SOUND_ID")
	if kSoundID~="" then
		kActor:AttachSound(2783,kSoundID)
	end	
	
    local   kFireEffectID = kAction:GetScriptParam("FIRE_CASTER_EFFECT_ID")
    local   kFireEffectTargetNodeID = kAction:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE")
	local   kFireEffectTargetNodeIDToPos = kAction:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS")
	local   kFireEffectTargetNodeIDToPosWithRotate = kAction:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE")

    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= ""  or kFireEffectTargetNodeIDToPosWithRotate ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = kActor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(kActor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            
            kActor:AttachParticleToPoint(7212,kDir,kFireEffectID)            
        elseif kFireEffectTargetNodeID ~= "" then
            kActor:AttachParticle(7212,kFireEffectTargetNodeID,kFireEffectID)
		elseif kFireEffectTargetNodeIDToPos ~= "" then
			kActor:AttachParticleToPoint(7212,kActor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID)			
		else
		    kActor:AttachParticleToPointWithRotate(7212,kActor:GetNodeWorldPos(kFireEffectTargetNodeIDToPosWithRotate), kFireEffectID, kActor:GetRotateQuaternion())
        end
    
    end	

    kFireEffectID = kAction:GetScriptParam("FIRE_CASTER_EFFECT_ID2")
    kFireEffectTargetNodeID = kAction:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE2")
	
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = kActor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(kActor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            
            kActor:AttachParticleToPoint(7213,kDir,kFireEffectID)
            
        else
            kActor:AttachParticle(7213,kFireEffectTargetNodeID,kFireEffectID)
        end
    
    end	
		
	if kAction:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    kActor:StartWeaponTrail()
	end

end

function Skill_CrossBlade_Play_OnEnter(kActor, kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())

--	local actorID = kActor:GetID()
	local actionID = kAction:GetID()
--	local weapontype = kActor:GetEquippedWeaponType()
	kAction:SetParamFloat(11,0)
	
	kAction:SetParamInt(100,0) -- 평타 랜덤 범위. 사용안함
	kAction:SetParamInt(101,2)
	
	kAction:SetParamInt(102,3) -- 막타 랜덤 범위
	kAction:SetParamInt(103,3)
	
	
	ODS("Skill_CrossBlade_Play_OnEnter actionID:"..actionID.." GetActionParam:"..kAction:GetActionParam().."\n")
	kAction:SetParamInt(AT_COUNT, 0)
	kAction:SetParam(3,"0")
	local   kFireEffectID = kAction:GetScriptParam("CASTING_EFFECT_EFFECT_ID")
    local   kFireEffectTargetNodeID = kAction:GetScriptParam("CASTING_EFFECT_TARGET_NODE")
	local   kFireEffectTargetNodeIDToPos = kAction:GetScriptParam("CASTING_EFFECT_TARGET_NODE_TO_WORLD_POS")
    
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= "" then
    
		local iSize = 1.0
		if "TRUE" == kAction:GetScriptParam("CASTING_EFFECT_SIZE_TO_SKILLRANGE") then
			iSize = iSize * (kAction:GetSkillRange(0,kActor) / 100)
			iSize = iSize * 2 
		end
		
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = kActor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(kActor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            
            kActor:AttachParticleToPointS(7215,kDir,kFireEffectID, iSize)
            
        elseif kFireEffectTargetNodeID ~= "" then
            kActor:AttachParticleS(7215,kFireEffectTargetNodeID,kFireEffectID,iSize)
		else
			kActor:AttachParticleToPointS(7215,kActor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID,iSize)
        end
		kAction:SetParam(3,"1")
    end	
	
	local	kSoundID = kAction:GetScriptParam("CASTING_SOUND_ID")
	if kSoundID~="" then
		kActor:AttachSound(2785,kSoundID)
	end	
	
	kAction:SetParamInt(2,0)
	
	if kAction:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true
	end

	Skill_CrossBlade_Play_OnCastingCompleted(kActor,kAction)
	
	if kActor:IsMyActor() and actionID == "a_archer_melee" then
	    GetComboAdvisor():OnNewActionEnter(kAction:GetID())
	end

--	local fRandom = math.random(25, 50)
--	fRandom =fRandom*0.01
--	kActor:AttachParticleS(7217, "char_root" , "ef_skill_cross_blade_03_char_root", 0.5)	-- 발끌리는 이펙트 
	kAction:StartTimer(10, 0.5, 0)
	return true
end

function Skill_CrossBlade_Play_OnUpdate(kActor, accumTime, frameTime)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
--	local actorID = kActor:GetID()
	local kAction = kActor:GetAction()
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
--	ODS("현재 슬롯:"..kAction:GetCurrentSlot().."\n",false, 912)
	local animDone = kActor:IsAnimationDone()
	local nextActionName = kAction:GetNextActionName()
--	local actionID = kAction:GetID()
--	local iComboCount = kActor:GetComboCount()
	
	if kAction:GetParam(1) == "end" then
		return	false
	end

	if kAction:GetParam(1) == "wait_for_end" then
		if(1 < GetWorldAccumTime()-kAction:GetParamFloat(11)) then 
			kAction:SetParam(1, "end")
			kActor:SetHide(true)
		end
		return true
	end

	if animDone == true then
		
		--local iSlotCount = kAction:GetSlotCount()
		--kAction:SetSlot(math.random(0, iSlotCount))		
		local iSlot = kAction:GetParamInt(10)
		
		if(kAction:GetParam(1) == "finish_attack") then
		-- 마지막 공격 애니메이션 설정
			local iMin = kAction:GetParamInt(102)
			local iMax = kAction:GetParamInt(103)
			iSlot = math.random(iMin, iMax)
			kAction:SetParam(1, "add_smoke")
			kAction:DeleteTimer(0)
		elseif kAction:GetParam(1) == "add_smoke"  then
		-- 마지막 공격 애니메이션이 끝났을때
			iSlot = kAction:GetSlotCount()-1
			kAction:SetParamFloat(11, GetWorldAccumTime())
			kAction:SetParam(1, "wait_for_end")
			g_pilotMan:RemoveReservedPilot(kActor:GetPilotGuid())
			kActor:AttachParticleS(7216,"char_root","ef_smog_03_char_root", 1)
			kActor:SetTargetAlpha(kActor:GetAlpha(),0, 0.5)
		else
		-- 평타 때릴때
		--[[
			if(0 == iSlot) then 
				iSlot = 1
			else
				iSlot = 0
			end
		]]
			local iMin = kAction:GetParamInt(100)
			local iMax = kAction:GetParamInt(101)
			iSlot = math.random(iMin, iMax)
			kAction:SetParamInt(10, iSlot)
		end
		kAction:SetSlot(iSlot)
		kActor:PlayCurrentSlot()
	end
	--[[
	if(kAction:GetParam(1) == "finish_attack" or kAction:GetParam(1) == "add_smoke") then
		return true
	end
	]]
				
	local iMoveSpeed = kAction:GetAbil(AT_MOVESPEED)	
	if (0 == iMoveSpeed) then
		iMoveSpeed = 60
	end
	
	local kDirection = kActor:GetLookingDir()
	kDirection:Multiply(iMoveSpeed)
	kActor:SetMovingDelta(kDirection)
	return true
end

function Skill_CrossBlade_Play_OnCleanUp(kActor)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
    local   kCurAction = kActor:GetAction()
	
	ODS("RestoreItemModel Skill_CrossBlade_Play_ReallyLeave\n")
--	kActor:EndBodyTrail()
--	kActor:EndWeaponTrail()
	--	무기 원래대로
	if kCurAction:GetScriptParam("WEAPON_XML") ~= "" then
		kActor:RestoreItemModel(ITEM_WEAPON)
	end

	if kCurAction:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		kActor:HideParts(EQUIP_POS_WEAPON, false)
	end

	if kCurAction:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    kActor:EndWeaponTrail()
	end
	
	if kCurAction:GetScriptParam("DETACH_FIRE_EFFECT_AT_FINISH") == "TRUE" then
	
		local DefaultDetaChID = false
		local DefaultDetaChID2 = false
		if kCurAction:GetScriptParam("FIRE_CASTER_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID = true
		end

		if kCurAction:GetScriptParam("FIRE_CASTER_EFFECT_ID2_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID2 = true
		end		

	    kActor:DetachFrom(7212,DefaultDetaChID)
	    kActor:DetachFrom(7213,DefaultDetaChID2)
		kActor:DetachFrom(7214)
		kActor:DetachFrom(7217, true) -- 발끌리는 이펙트
	end
	
	if kCurAction:GetScriptParam("DETACH_CASTING_EFFECT_AT_FINISH") == "TRUE" then
		local DefaultDetaChID = false
		if kCurAction:GetScriptParam("CASTING_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID = true
		end
		kActor:DetachFrom(7215,DefaultDetaChID)
	end
		
	local   kCleanUpEffectID = kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_ID")
    local   kCleanUpEffectTargetNodeID = kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_TARGET_NODE")
	local	fCleanUpScale = tonumber(kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_SCALE"))

	if fCleanUpScale == 0 or fCleanUpScale == nil then
		fCleanUpScale = 1.0
	end
    
    if kCleanUpEffectID ~= "" and kCleanUpEffectTargetNodeID ~= "" then
		 kActor:AttachParticleS(7216,kCleanUpEffectTargetNodeID,kCleanUpEffectID, fCleanUpScale)
	end

	if "1" == kCurAction:GetParam(3) then
		--캐스팅 중에 캔슬된 상태
		kActor:DetachFrom(7215,true)
		if( false == kActor:PlayNext()) then 
			kCurAction:SetNextActionName("a_battle_idle")			
		end
		kCurAction:ChangeToNextActionOnNextUpdate(true,true)
	end
	
	-- 모습 원래대로(이걸 사용한후 curAction을 얻어와 쓰면 절대로 안됨!!!)
    local kTransformXMLID = kCurAction:GetScriptParam("TRANSFORM_XML")
    if kTransformXMLID ~= "" then
    
        local   iTransformEffectID = tonumber(kCurAction:GetScriptParam("TRANSFORM_ID"))
        if iTransformEffectID ~= nil then
        
            local   kMan = GetStatusEffectMan()
            kMan:RemoveStatusEffectFromActor(kActor:GetPilotGuid(),iTransformEffectID)
        
        end
    
    end	
	return true
end

function Skill_CrossBlade_Play_OnLeave(kActor, kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())

	local curAction = kActor:GetAction()
	local curParam = curAction:GetParam(1)
	local actionID = kAction:GetID()
	
	if kActor:IsMyActor() == false then
		return true
	end
	
	if actionID == "a_jump" then 
		if( false == kActor:PlayNext()) then 
			kActor:GetAction():SetNextActionName("a_battle_idle")
		end
		kActor:GetAction():ChangeToNextActionOnNextUpdate(true,true)
		return false
	end
	
	if kAction:GetActionType()=="EFFECT" then
		
		return true
	end
	
	if curParam == "end" then 
		return true
	end
	
	if kAction:GetEnable() == false then
		if curParam == actionID then
			curAction:SetParam(0,"")
		end
	end
	
	if kAction:GetEnable() == true then
	

		curAction:SetParam(0,"GoNext")
		curAction:SetParam(1,actionID)

		return false
	
	elseif curParam == "end" and (actionID == "a_run_right" or actionID == "a_run_left") then
		return true
	elseif 	actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true
	end

	return false 
end

function Skill_CrossBlade_Play_DoDamage(kActor,actorTarget,kActionResult,kActionTargetInfo)
	
	CheckNil(nil==kActionResult)
	CheckNil(nil==kActionTargetInfo)
	
	local kAction = kActor:GetAction()
	local kActionID = kAction:GetID()
	
	if kActor:GetPilotGuid():IsEqual(actorTarget:GetPilotGuid()) then
		return
	end
	
	local	iABVIndex = kActionTargetInfo:GetABVIndex()
	local pt = actorTarget:GetABVShapeWorldPos(iABVIndex)
	
    local   kHitTargetEffectID = kAction:GetScriptParam("HIT_TARGET_EFFECT_ID")
    local   kHitTargetEffectTargetNodeID = kAction:GetScriptParam("HIT_TARGET_EFFECT_TARGET_NODE")
    
    if kHitTargetEffectID ~= "" and kHitTargetEffectTargetNodeID ~= "" then
    
        actorTarget:AttachParticle(7211,kHitTargetEffectTargetNodeID,kHitTargetEffectID)
    
    end	
	
	
	if kAction:GetScriptParam("NO_DEFAULT_HIT_EFFECT") ~= "TRUE" then
		local   kHitTargetEffectOffset = tonumber(kAction:GetScriptParam("HIT_TARGET_EFFECT_RANDOM_OFFSET"))
		
		if kHitTargetEffectOffset ~= nil then
			pt:SetX(pt:GetX() + math.random(-kHitTargetEffectOffset,kHitTargetEffectOffset))
			pt:SetY(pt:GetY() + math.random(-kHitTargetEffectOffset,kHitTargetEffectOffset))
			pt:SetZ(pt:GetZ() + math.random(-kHitTargetEffectOffset,kHitTargetEffectOffset))
		end

		local	iHitCount = kActor:GetAction():GetParamInt(2)

	    if kActionResult:GetCritical() then
		    actorTarget:AttachParticleToPoint(12 + iHitCount, pt, "e_dmg_cri")
	    else
		    actorTarget:AttachParticleToPoint(12 + iHitCount, pt, "e_dmg")
	    end	
	    
	    
	    if kAction:GetScriptParam("NO_DAMAGE_HOLD") ~= "TRUE" then
    	    
	        -- 충격 효과 적용
	        local iShakeTime = g_fAttackHitHoldingTime * 1000
	        kActor:SetAnimSpeedInPeriod(0.01, iShakeTime)
	        kActor:SetShakeInPeriod(5, iShakeTime/2)
	        actorTarget:SetShakeInPeriod(5, iShakeTime)		
	    end
	end
end

function Skill_CrossBlade_Play_HitOneTime(kActor,kAction)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	local	iHitCount = kAction:GetParamInt(2)
	
	if iHitCount == 0 then
	
	    local   kHitEffectID = kAction:GetScriptParam("HIT_CASTER_EFFECT_ID")
	    local   kHitEffectTargetNodeID = kAction:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE")
		local   kHitEffectTargetNodeIDToPos = kAction:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS")
		local   kHitEffectTargetNodeIDToPosWithRotate = kAction:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE")

	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" or kHitEffectTargetNodeIDToPos ~= ""  or kHitEffectTargetNodeIDToPosWithRotate ~= "" then
			if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
				kActor:AttachParticle(7214,kHitEffectTargetNodeID,kHitEffectID)
			elseif kHitEffectTargetNodeID ~= "" then
	            kActor:AttachParticle(7214,kHitEffectTargetNodeID,kHitEffectID)
			elseif kHitEffectTargetNodeIDToPos ~= "" then
				kActor:AttachParticleToPoint(7214,kActor:GetNodeWorldPos(kHitEffectTargetNodeIDToPos),kHitEffectID)			
			else
			    kActor:AttachParticleToPointWithRotate(7214,kActor:GetNodeWorldPos(kHitEffectTargetNodeIDToPosWithRotate), kHitEffectID, kActor:GetRotateQuaternion())
		    end
		end
	end

--[[
	local	iTotalHit = tonumber(kAction:GetScriptParam("MULTI_HIT_NUM"))
	if iTotalHit == nil or iTotalHit == 0 then
	    iTotalHit = 1
	end
]]
	
	if iHitCount == iTotalHit then
		return
	end
	
	local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID")
	if kSoundID~="" then
		kActor:AttachSound(2784,kSoundID)
	end	
	
	--남성용 사운드
	if 1 == kActor:GetAbil(AT_GENDER) then
		local	kSoundID2 = kAction:GetScriptParam("HIT_SOUND_ID_MALE")
		if kSoundID2~="" then
			kActor:AttachSound(2785,kSoundID2)
		end	
	--여성용 사운드
	else
		local	kSoundID2 = kAction:GetScriptParam("HIT_SOUND_ID_FEMALE")
		if kSoundID2~="" then
			kActor:AttachSound(2785,kSoundID2)
		end	
	end
		
	local iTargetCount = kAction:GetTargetCount()
	local i =0
	if iTargetCount>0 then
		
		local bWeaponSoundPlayed = false
		
		while i<iTargetCount do
		
			local actionResult = kAction:GetTargetActionResult(i)
			if actionResult:IsNil() == false then
			
				local kTargetGUID = kAction:GetTargetGUID(i)
				local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)
				if kTargetPilot:IsNil() == false and actionResult:IsMissed() == false then
					
					local actorTarget = kTargetPilot:GetActor()
					
                    if bWeaponSoundPlayed == false then
                        bWeaponSoundPlayed = true
                        -- 피격 소리 재생
						local actionName = kAction:GetID()
						if actionResult:GetCritical() then
							actionName = "Critical_dmg"
						end
                        actorTarget:PlayWeaponSound(WST_HIT, kActor, actionName, 0, kAction:GetTargetInfo(i))
                    end
					Skill_CrossBlade_Play_DoDamage(kActor,actorTarget,actionResult, kAction:GetTargetInfo(i))
				end
				
			end
			
			i=i+1
		
		end
	end

	kAction:GetTargetList():ApplyActionEffects()
	kAction:ClearTargetList();
	kActor:ClearIgnoreEffectList()
	
	iHitCount = iHitCount + 1
	kAction:SetParamInt(2,iHitCount)
	
end
function Skill_CrossBlade_Play_OnTargetListModified(kActor,kAction,bIsBefore)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
    if bIsBefore == false then

        if kActor:IsMyActor()  then
			kAction:GetTargetList():ApplyActionEffects(true,true)
			return
		end
--[[
		local	iTotalHit = tonumber(kAction:GetScriptParam("MULTI_HIT_NUM"))
		if iTotalHit == nil then
			iTotalHit = 1
		end
		
        if iTotalHit>1 or kAction:GetParamInt(2) > 0  then
    ]]    
		if kAction:GetParamInt(2) > 0  then
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            Skill_CrossBlade_Play_HitOneTime(kActor,kAction)
        
        end        
    
    end

end

function Skill_CrossBlade_Play_OnEvent(kActor,textKey)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
		
	local kAction = kActor:GetAction()
		
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())

	
	if kAction:GetActionParam() == AP_CASTING then
		return true
	end
	
	if textKey == "hit" or textKey == "fire" then
		
		--공격 횟수 제한 체크
		local iMaxAttackCnt = kAction:GetAbil(AT_COUNT)
		local iCurAttackCnt = kAction:GetParamInt(AT_COUNT)+1
		kAction:SetParamInt(AT_COUNT, iCurAttackCnt)
		
		if kActor:IsUnderMyControl() then
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE")
			if bUseTLM then
			
				kAction:CreateActionTargetList(kActor)
				if kAction:GetScriptParam("SHOW_TEXT_TARGETLIST_COUNT_NUM") ~= "" then
					local iTargetCount = kAction:GetTargetCount()
					if iTargetCount == tonumber(kAction:GetScriptParam("SHOW_TEXT_TARGETLIST_COUNT_NUM")) then
						AddWarnDataStr(GetTextW(tonumber(kAction:GetScriptParam("SHOW_TEXT_TARGETLIST_TEXTNO"))),2)		
					end
				end

				if IsSingleMode() then
		        
					Skill_CrossBlade_Play_HitOneTime(kActor,kAction)
					Skill_CrossBlade_Play_OnTargetListModified(kActor,kAction,false)
		            
					return
				else
		        
					kAction:BroadCastTargetListModify(kActor:GetPilot())
					kAction:ClearTargetList();
				end			
			
			end	
		end	
        Skill_CrossBlade_Play_HitOneTime(kActor,kAction)		
		if(iMaxAttackCnt <= iCurAttackCnt) then 			
			kAction:SetParamInt(AT_COUNT)
		elseif(iMaxAttackCnt-1 == iCurAttackCnt) then
			--이제 마지막 공격 애니메이션만 남았다
			kAction:SetParam(1, "finish_attack")
		end
		
	end
	
	return	true
end


function Skill_CrossBlade_Play_OnTimer(kActor,fAccumTime,kAction,iTimerID)
	
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
		
	if 0 == iTimerID then	
		kActor:AttachParticleS(7217,"char_root","ef_skill_rollingground_03_char_root", 0.5)
	end

	return true

end
--[[
function Skill_CrossBlade_Play_OnFindTarget(kActor,kAction,kTargets)
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
    local   kParam = FindTargetParam();

	kParam:SetParam_1(kActor:GetPos(),kActor:GetLookingDir());
	--kParam:SetParam_2(0,0,kAction:GetSkillRange(0,kActor),0);
	kParam:SetParam_2(kAction:GetSkillRange(0,kActor),
					  kAction:GetAbil(AT_1ST_AREA_PARAM2),
					  kAction:GetAbil(AT_1ST_AREA_PARAM2),
					  0
					  );
	kParam:SetParam_3(true,FTO_NORMAL);
		
	return kAction:FindTargets(TAT_BAR,kParam,kTargets,kTargets);	
end
]]