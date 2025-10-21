-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [HIT_SOUND_ID_MALE] : HIT 키에서 플레이 할 사운드 ID - 남성
-- [HIT_SOUND_ID_FEMALE] : HIT 키에서 플레이 할 사운드 ID - 여성
-- [FIRE_SOUND_ID] : 시전시에 플레이 할 사운드 ID
-- [LOOP_SOUND_ID] : 시전시에 플레이 할 사운드 ID
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
-- [DETACH_FIRE_EFFECT_AT_LEAVE] : _OnLeave 시점에 이펙트를 제거할것인가
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
function AwakeCharge_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	local weapontype = actor:GetEquippedWeaponType()
	
	if(1 == actor:GetAbil(AT_AWAKE_STATE)) then	
		return false
	end
	
	if weapontype == 0 then
		return	false
	end
	
	return true	
end

function AwakeCharge_OnEnter(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())

--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType()
	
	ODS("AwakeCharge_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n")

	action:SetParam(3,"0")
	local   kFireEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID")
    local   kFireEffectTargetNodeID = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE")
	local   kFireEffectTargetNodeIDToPos = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE_TO_WORLD_POS")
    
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= "" then
    
		local iSize = 1.0
		if "TRUE" == action:GetScriptParam("CASTING_EFFECT_SIZE_TO_SKILLRANGE") then
			iSize = iSize * (action:GetSkillRange(0,actor) / 100)
			iSize = iSize * 2 
		end
		
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(actor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            
            actor:AttachParticleToPointS(7215,kDir,kFireEffectID, iSize)
            
        elseif kFireEffectTargetNodeID ~= "" then
            actor:AttachParticleS(7215,kFireEffectTargetNodeID,kFireEffectID,iSize)
		else
			actor:AttachParticleToPointS(7215,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID,iSize)
        end
		action:SetParam(3,"1")
    end	
	
	local	kSoundID = action:GetScriptParam("CASTING_SOUND_ID")
	if kSoundID~="" then
		actor:AttachSound(2785,kSoundID)
	end	
	
	action:SetParamInt(2,0)
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true
	end

	AwakeCharge_OnCastingCompleted(actor,action)
	
	if actor:IsMyActor() and actionID == "a_archer_melee" then
	    GetComboAdvisor():OnNewActionEnter(action:GetID())
	end
		
	return true
end

function AwakeCharge_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID()
--	local iComboCount = actor:GetComboCount()
	
	if action:GetParam(1) == "end" then
		return	false
	end
	
	if(1 == actor:GetAbil(AT_AWAKE_STATE)) then	
		action:SetParam(1, "end")
		return false
	end	
	
	if true == animDone or "DoEndAni" == action:GetParam(1)then
--		actor:PlayCurrentSlot()
			
		if("DoEndAni" == action:GetParam(1)) then
			action:SetParam(1, "EndAniProcessing")
		end
		
		if(action:GetSlotCount() == action:GetCurrentSlot()) then
			action:SetParam(1, "end")
			return false
		end
		
		local iNextSlot = action:GetCurrentSlot()+1
		if(1 == iNextSlot) then --0번 슬롯의 애니메이션이 끝나면 루프 사운드를 붙여준다
			local	kSoundID = action:GetScriptParam("LOOP_SOUND_ID")
			if kSoundID~="" then
				actor:AttachSound(2783,kSoundID)
			end	
		end
		action:SetSlot(iNextSlot)
		actor:PlayCurrentSlot()
		
		local curActionParam = action:GetParam(0)
		if curActionParam == "GoNext" then
			nextActionName = action:GetParam(1)

			action:SetParam(0, "null")
			action:SetParam(1, "end")
			
			action:SetNextActionName(nextActionName)
			return false
		end
	end

	return true
end


function AwakeCharge_OnLeave(actor, action)
	ODS("AwakeCharge_OnLeave\n", false, 912)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	local curAction = actor:GetAction()
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	ODS("actionID:"..actionID.."\n", false, 912)
	
	if( actor:IsMyActor() == false 
		or actionID == "a_battle_idle"
		or actionID == "a_run"
		or action:GetActionType()=="EFFECT"
		or curParam == "end"
	)
	then 
		curAction:SetParam(1, "end")		
		if curAction:GetScriptParam("DETACH_FIRE_EFFECT_AT_LEAVE") == "TRUE" then
			actor:DetachFrom(7212,true)	
			actor:DetachFrom(7213,true)
		end		
		return true
	end

	if action:GetEnable() == false then
		if curParam == actionID then
			curAction:SetParam(0,"")
		end
	end

	return false 
end

function AwakeCharge_OnCleanUp(actor)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
    local   kCurAction = actor:GetAction()
	CheckNil(nil==kCurAction)
	CheckNil(kCurAction:IsNil())
	
	--local iLevel = kCurAction:GetSkillLevel()-1
	--ODS("iLevel:"..iLevel.."\n", false, 912)
	local kEffectID = kCurAction:GetEffectNo()
	--ODS("kEffectID:"..kEffectID.."\n", false, 912)
	if kEffectID ~= 0 and actor:IsMyActor() then -- 이펙트 삭제요청
		local	kPacket = NewPacket(12130)
		kPacket:PushGuid(actor:GetPilotGuid())
		kPacket:PushInt(kEffectID)
		Net_Send(kPacket)
		DeletePacket(kPacket)
	end	
		
	ODS("RestoreItemModel AwakeCharge_ReallyLeave\n")
	--	무기 원래대로
	if kCurAction:GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON)
	end

	if kCurAction:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, false)
	end

	if kCurAction:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:EndWeaponTrail()
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

	    actor:DetachFrom(7212,DefaultDetaChID)
	    actor:DetachFrom(7213,DefaultDetaChID2)
		actor:DetachFrom(7214)
	end
	
	if kCurAction:GetScriptParam("DETACH_CASTING_EFFECT_AT_FINISH") == "TRUE" then
		local DefaultDetaChID = false
		if kCurAction:GetScriptParam("CASTING_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID = true
		end
		actor:DetachFrom(7215,DefaultDetaChID)
	end
		
	local   kCleanUpEffectID = kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_ID")
    local   kCleanUpEffectTargetNodeID = kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_TARGET_NODE")
	local	fCleanUpScale = tonumber(kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_SCALE"))

	if fCleanUpScale == 0 or fCleanUpScale == nil then
		fCleanUpScale = 1.0
	end
    
    if kCleanUpEffectID ~= "" and kCleanUpEffectTargetNodeID ~= "" then
		 actor:AttachParticleS(7216,kCleanUpEffectTargetNodeID,kCleanUpEffectID, fCleanUpScale)
	end

	if "1" == kCurAction:GetParam(3) then
		--캐스팅 중에 캔슬된 상태
		actor:DetachFrom(7215,true)
		if( false == actor:PlayNext()) then 
			kCurAction:SetNextActionName("a_battle_idle")			
		end
		kCurAction:ChangeToNextActionOnNextUpdate(true,true)
	end
	
	actor:DetachFrom(7212,true)	
	actor:DetachFrom(7213,true)
	
	-- 모습 원래대로(이걸 사용한후 curAction을 얻어와 쓰면 절대로 안됨!!!)
    local kTransformXMLID = kCurAction:GetScriptParam("TRANSFORM_XML")
    if kTransformXMLID ~= "" then
    
        local   iTransformEffectID = tonumber(kCurAction:GetScriptParam("TRANSFORM_ID"))
        if iTransformEffectID ~= nil then
        
            local   kMan = GetStatusEffectMan()
            kMan:RemoveStatusEffectFromActor(actor:GetPilotGuid(),iTransformEffectID)
        
        end
    
    end	
	return true
end


function AwakeCharge_OnCastingCompleted(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	AwakeCharge_Fire(actor,action)
	action:SetParam(3,"0")
end

function AwakeCharge_Fire(actor,action)
	ODS("RestoreItemModel AwakeCharge_Fire\n")
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE")		
	
	if bUseTLM == false then
		action:CreateActionTargetList(actor)
		if action:GetScriptParam("SHOW_TEXT_TARGETLIST_COUNT_NUM") ~= "" then
			local iTargetCount = action:GetTargetCount()
			if iTargetCount == tonumber(action:GetScriptParam("SHOW_TEXT_TARGETLIST_COUNT_NUM")) then
				AddWarnDataStr(GetTextW(tonumber(action:GetScriptParam("SHOW_TEXT_TARGETLIST_TEXTNO"))),2)		
			end
		end
	end

	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID")
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID)
	end	
		
	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"))
		ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n")
	end
	
	if action:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, true)
	end

    local   kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID")
    local   kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE")
	local   kFireEffectTargetNodeIDToPos = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS")
	local   kFireEffectTargetNodeIDToPosWithRotate = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE")

    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= ""  or kFireEffectTargetNodeIDToPosWithRotate ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(actor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            
            actor:AttachParticleToPoint(7212,kDir,kFireEffectID)            
        elseif kFireEffectTargetNodeID ~= "" then
            actor:AttachParticle(7212,kFireEffectTargetNodeID,kFireEffectID)
		elseif kFireEffectTargetNodeIDToPos ~= "" then
			actor:AttachParticleToPoint(7212,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID)			
		else
		    actor:AttachParticleToPointWithRotate(7212,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPosWithRotate), kFireEffectID, actor:GetRotateQuaternion())
        end
    
    end	

    kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID2")
    kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE2")
	
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(actor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            
            actor:AttachParticleToPoint(7213,kDir,kFireEffectID)
            
        else
            actor:AttachParticle(7213,kFireEffectTargetNodeID,kFireEffectID)
        end
    
    end	
		
	if action:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:StartWeaponTrail()
	end
	
	--  변신 적용
    local kTransformXMLID = action:GetScriptParam("TRANSFORM_XML")
    if kTransformXMLID ~= "" then
    
        local   iTransformEffectID = tonumber(action:GetScriptParam("TRANSFORM_ID"))
        if iTransformEffectID ~= nil then
        
            local   kMan = GetStatusEffectMan()
            kMan:AddStatusEffectToActor(actor:GetPilotGuid(),kTransformXMLID,iTransformEffectID,0)        
        end
    
    end
end


function AwakeCharge_OnEvent(actor,textKey)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
		
	local kAction = actor:GetAction()
		
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())

	
	if kAction:GetActionParam() == AP_CASTING then
		return true
	end

	return	true
end