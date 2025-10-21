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

function Skill_GroundZero_Creature_Idle_OnCastingCompleted(kActor, kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	Skill_GroundZero_Creature_Idle_Fire(kActor,kAction)
	kAction:SetSlot(kAction:GetCurrentSlot()+1)
	kActor:PlayCurrentSlot()
	kAction:SetParam(3,"0")
end

function Skill_GroundZero_Creature_Idle_Fire(kActor,kAction)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
end

function Skill_GroundZero_Creature_Idle_OnEnter(kActor, kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())

	local actionID = kAction:GetID()
	
	kAction:SetParam(3,"0")
	kAction:SetParamInt(2,0)
	kAction:SetParamInt(20, 0)	
	kAction:SetParamInt(30, 2000)
		
	kActor:LockBidirection(false)
	kActor:SeeFront(true)
	
	local	kSoundID = kAction:GetScriptParam("CASTING_SOUND_ID")
	if kSoundID~="" then
		kActor:AttachSound(2785,kSoundID)
	end	
	
	if kAction:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true
	end

	Skill_GroundZero_Creature_Idle_OnCastingCompleted(kActor,kAction)
	QuakeCamera(4, 0.2, 1, 1, 1)
	return true
end

function Skill_GroundZero_Creature_Idle_OnUpdate(kActor, accumTime, frameTime)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	local kAction = kActor:GetAction()
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())

	kActor:LockBidirection(false)
	kActor:SeeFront(true)
	
	local animDone = kActor:IsAnimationDone()
	local nextActionName = kAction:GetNextActionName()
	
	if animDone == true then
		return false
	end	
	
	return true
end

function Skill_GroundZero_Creature_Idle_OnCleanUp(kActor)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	local   kCurAction = kActor:GetAction()
	
	CheckNil(nil==kCurAction)
	CheckNil(kCurAction:IsNil())
	
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(kActor:GetPilotGuid())

	-- 총구 이펙트 제거
	local i = 1000
	while(i <= 1018) do
		kActor:DetachFrom(i, true)
		i = i+1 
	end
	
	-- 주변 이펙트 제거
	i = 0
	local iMax = kCurAction:GetParamInt(20)
	if(i < iMax) then 	
		while(i < iMax) do
			kActor:DetachFrom(i, true)
			i = i+1 
		end	
	end

	return true
	
end

function Skill_GroundZero_Creature_Idle_OnLeave(kActor, kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
		
	return true
end


function Skill_GroundZero_Creature_Idle_OnTargetListModified(kActor,kAction,bIsBefore)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
    if bIsBefore == false then

        if kActor:IsMyActor()  then
			kAction:GetTargetList():ApplyActionEffects(true,true)
			return
		end
    end
end

function Skill_GroundZero_Creature_Idle_OnEvent(kActor,textKey)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
		
	local kAction = kActor:GetAction()
		
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())

	
	if kAction:GetActionParam() == AP_CASTING then
		return true
	end
	
--		ODS("액터 키네임:"..textKey.."\n", false, 912)
		
	if( textKey == "bang_03") then	-- 보라돌이 
		if(kAction:GetParamInt(2) == 0) then 
			kAction:StartTimer(4.6, 0.1, 0)
			kAction:SetParamInt(2, 1)			
		end 
		kActor:AttachParticleS(1000, "p_ef_shot_14", "ef_groundzero_fire_purple", 0.5)		
		kActor:AttachParticleS(1001, "p_ef_shot_13", "ef_groundzero_fire_purple", 0.5)		
		kActor:AttachParticleS(1002, "p_ef_shot_15", "ef_groundzero_fire_purple", 0.5)		
		kActor:AttachParticleS(1003, "p_ef_shot_12", "ef_groundzero_fire_purple", 0.5)
		
		Skill_GroundZero_AttatchCartridge(kActor, kAction, "p_ef_shot_14", math.random(2, 5)/10)
		Skill_GroundZero_AttatchCartridge(kActor, kAction, "p_ef_shot_13", math.random(2, 5)/10)
		Skill_GroundZero_AttatchCartridge(kActor, kAction, "p_ef_shot_15", math.random(2, 5)/10)
		Skill_GroundZero_AttatchCartridge(kActor, kAction, "p_ef_shot_12", math.random(2, 5)/10)
	elseif( textKey == "bang_01") then
		kActor:AttachParticleS(1009, "p_ef_shot_21", "ef_groundzero_fire_red", 0.5)		
		kActor:AttachParticleS(1010, "p_ef_shot_17", "ef_groundzero_fire_red", 0.5)		
		kActor:AttachParticleS(1011, "p_ef_shot_18", "ef_groundzero_fire_red", 0.5)		
		kActor:AttachParticleS(1012, "p_ef_shot_19", "ef_groundzero_fire_red", 0.5)		
		kActor:AttachParticleS(1013, "p_ef_shot_16", "ef_groundzero_fire_red", 0.5)		
		kActor:AttachParticleS(1014, "p_ef_shot_20", "ef_groundzero_fire_red", 0.5)

		Skill_GroundZero_AttatchCartridgeQuat(kActor, kAction, "p_ef_shot_21", math.random(2, 5)/10)
		Skill_GroundZero_AttatchCartridgeQuat(kActor, kAction, "p_ef_shot_17", math.random(2, 5)/10)
		Skill_GroundZero_AttatchCartridgeQuat(kActor, kAction, "p_ef_shot_18", math.random(2, 5)/10)
		Skill_GroundZero_AttatchCartridgeQuat(kActor, kAction, "p_ef_shot_19", math.random(2, 5)/10)
		Skill_GroundZero_AttatchCartridgeQuat(kActor, kAction, "p_ef_shot_16", math.random(2, 5)/10)
		Skill_GroundZero_AttatchCartridgeQuat(kActor, kAction, "p_ef_shot_20", math.random(2, 5)/10)
		
	elseif( textKey == "bang_02") then
		kActor:AttachParticleS(1016, "p_ef_shot_22", "ef_groundzero_fire_red", 0.5)		
		kActor:AttachParticleS(1017, "p_ef_shot_23", "ef_groundzero_fire_red", 0.5)		
		kActor:AttachParticleS(1018, "p_ef_shot_24", "ef_groundzero_fire_red", 0.5)
		Skill_GroundZero_AttatchCartridge(kActor, kAction, "p_ef_shot_22", math.random(2, 5)/10, true)
		Skill_GroundZero_AttatchCartridge(kActor, kAction, "p_ef_shot_23", math.random(2, 5)/10, true)		
		Skill_GroundZero_AttatchCartridge(kActor, kAction, "p_ef_shot_24", math.random(2, 5)/10, true)
		
	elseif( textKey == "bang_04") then
		local iIndex = kAction:GetParamInt(30)		
		kActor:AttachParticleS(1004, "p_ef_shot_11", "ef_groundzero_fire_red", 0.5)				
		kActor:AttachParticleS(1005, "p_ef_shot_25", "ef_groundzero_fire_red", 0.5)		
		kActor:AttachParticleS(1006, "p_ef_shot_09", "ef_groundzero_fire_red", 0.5)		
		kActor:AttachParticleS(1007, "p_ef_shot_26", "ef_groundzero_fire_red", 0.5)		
		kActor:AttachParticleS(1008, "p_ef_shot_10", "ef_groundzero_fire_red", 0.5)		
		kActor:AttachParticleS(1015, "p_ef_shot_21", "ef_groundzero_fire_red", 0.5)
		
		Skill_GroundZero_AttatchCartridgeQuat(kActor, kAction, "p_ef_shot_11", math.random(2, 5)/10, Quaternion(180,Point3(0,0,1)))
		Skill_GroundZero_AttatchCartridgeQuat(kActor, kAction, "p_ef_shot_25", math.random(2, 5)/10, Quaternion(180,Point3(0,0,1)))
		Skill_GroundZero_AttatchCartridgeQuat(kActor, kAction, "p_ef_shot_09", math.random(2, 5)/10, Quaternion(180,Point3(0,0,1)))
		Skill_GroundZero_AttatchCartridgeQuat(kActor, kAction, "p_ef_shot_26", math.random(2, 5)/10, Quaternion(180,Point3(0,0,1)))
		Skill_GroundZero_AttatchCartridgeQuat(kActor, kAction, "p_ef_shot_10", math.random(2, 5)/10, Quaternion(180,Point3(0,0,1)))
		Skill_GroundZero_AttatchCartridgeQuat(kActor, kAction, "p_ef_shot_21", math.random(2, 5)/10, Quaternion(180,Point3(0,0,1)))
				
			-- 탄피 이펙트 제거
	elseif( textKey == "bomb") then
		local i = 2000
		local iMax = kAction:GetParamInt(30)
		if(i < iMax) then 
			while(i < iMax) do
				kActor:DetachFrom(i, true)
				i = i+1 
			end	
		end
	end
	
	return	true
end

function Skill_GroundZero_Creature_Idle_OnTimer(kActor,fAccumTime,kAction,iTimerID)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())

	if 0 == iTimerID then	
		local kBombEffTbl = {"ef_base_dmg_ar_01", "ef_base_dmg_ar_02", "ef_boom_05", "e_dmg"}		
		local iIndex = kAction:GetParamInt(20)
		local kPos = kActor:GetPos()	
		
		kPos:SetX(kPos:GetX() + math.random(-200,200))
		kPos:SetY(kPos:GetY() + math.random(-200,200))
		kPos:SetZ(kPos:GetZ()+50)

		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local ptcl = g_world:ThrowRay(kPos, Point3(0,0,-1),500)
		if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
			ptcl = g_world:ThrowRay(kPos, Point3(0,0,1),500)
			if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
				return true
			end			
		end
		local iIdx = math.random(1 ,table.getn(kBombEffTbl))
		--ODS("이펙트:"..iIdx.." 이름:"..(kBombEffTbl[iIdx]).."\n",false, 912)
		kActor:AttachParticleToPointS(iIndex, ptcl, kBombEffTbl[iIdx], math.random(1, 1))
		kAction:SetParamInt(20, iIndex + 1)
	end	

	return true

end

function Skill_GroundZero_AttatchCartridge(kActor, kAction, strTargetNodeName, fScale, bOnPos)
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	local iIndex = kAction:GetParamInt(30)		
	if(bOnPos) then 
		kActor:AttachParticleToPointS(iIndex, kActor:GetNodeTranslate(strTargetNodeName), "ef_cartridge_01", fScale)
	else
		kActor:AttachParticleS(iIndex,strTargetNodeName, "ef_cartridge_01", fScale)
	end	

	kAction:SetParamInt(30, iIndex+1)		
end

function Skill_GroundZero_AttatchCartridgeQuat(kActor, kAction, strTargetNodeName, fScale, kRotate)
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	local iIndex = kAction:GetParamInt(30)
	local kNodePos = kActor:GetNodeTranslate(strTargetNodeName)
	local kNodeRoate = kActor:GetNodeWorldRotate(strTargetNodeName)
	if(nil ~= kRotate) then
		kNodeRoate = kRotate
	end
	kActor:AttachParticleToPointWithRotate(iIndex, kNodePos, "ef_cartridge_01", kNodeRoate, fScale)
	kAction:SetParamInt(30, iIndex+1)		
end