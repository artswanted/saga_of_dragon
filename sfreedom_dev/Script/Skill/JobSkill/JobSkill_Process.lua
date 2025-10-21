-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [HIT_SOUND_ID_MALE] : HIT 키에서 플레이 할 사운드 ID - 남성
-- [HIT_SOUND_ID_FEMALE] : HIT 키에서 플레이 할 사운드 ID - 여성
-- [FIRE_SOUND_ID] : 시전시에 플레이 할 사운드 ID
-- [CASTING_SOUND_ID_MALE] : 캐스팅 시에 플레이 할 사운드 ID - 남성
-- [CASTING_SOUND_ID_FEMALE] : 캐스팅 시에 플레이 할 사운드 ID - 여성
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
-- [SUMMON_CLASSNO] : 소환해야하는 엔티티의 ClassNo
-- [SUMMON_RESERVE_ACTION] : 소환해야하는 엔티티의 시작 액션
-- [HIT_TIMER_TIME] : Hit 시에 시작할 타이머의 총 시간
-- [HIT_TIMER_TICK] : Hit 시에 시작할 타이머의 Tick 

-- Melee
function JobSkill_Process_OnCheckCanEnter(actor,action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	local weapontype = actor:GetEquippedWeaponType()
	
	if weapontype == 0 then
		return	false
	end

	return true
end

function JobSkill_Process_OnEnter(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	local actionID = action:GetID()	
	ODS("JobSkill_Process_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().." SkillLevel:"..action:GetSkillLevel().."\n", false, 912)
	
	SkillHelpFunc_InitHitCount(action)					-- HitCount 초기화	
	SkillHelpFunc_ExpressCastingEffect(actor, action)	-- 캐스팅때 파티클이 붙어있는지 확인하기 위해서	
	SkillHelpFunc_PlayCastingSound(actor, action) 		-- 캐스팅시 사운드	
	SkillHelpFunc_PlaySound(actor, action, "JOB_SKILL_TOOL_SOUND2")
	SkillHelpFunc_AttachParticleToNode(10001, "JOB_SKILL_TOOL_EFFECT_ID2", "JOB_SKILL_TOOL_TARGET_NODE2", 1, actor, action)
	--actor:AttachParticleS(10001,"p_ef_star","ef_Job_Axe", 1)
	-- 캐스팅이라면 리턴
	if action:GetActionParam() == AP_CASTING then
		return	true
	end
	
	-- 캐스팅 완료
	JobSkill_Process_OnCastingCompleted(actor,action) 

	local kMulptiple = action:GetScriptParam("EXPRESS_FAKE_MULTIPLE_ATTACK")
	if kMulptiple~="" then	-- 실제 한방이지만 연출로 여러대를 때리는가
		action:SetParamInt(100,1)
		action:SetParamInt(10,1) -- Timer 사용시 CreateTargetList를 1번만 사용하게 하기 위한 값
	end	
	
	return true
end

function JobSkill_Process_OnCastingCompleted(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	JobSkill_Process_Fire(actor,action)
	--action:SetSlot(action:GetCurrentSlot()+1)
--	actor:PlayCurrentSlot()
	
	SkillHelpFunc_SetNoneCastingState(action)	--캐스팅 완료 표시
	
	ODS("JobSkill_Process_OnCastingpleted실행 됐음\n", false, 912)
	
	
end

function JobSkill_Process_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	local action = actor:GetAction()
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
		
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
	local iSlot = action:GetCurrentSlot()
	local iMaxActionSlot = action:GetSlotCount()
	
	if action:GetParam(1) == "end" then
		return	false
	end
	
	local bKeyPress = KeyIsDown(KEY_CHARGE);	
	if(bKeyPress) then
		actor:PlayNext()
		--action:SetParam(1, "end")
		action:SetParamInt(1000, 1)
		actor:DetachFrom(10001, true)
		SkillHelpFunc_StopSound(actor, action, "JOB_SKILL_TOOL_SOUND2")
		SkillHelpFunc_PlaySound(actor, action, "JOB_SKILL_TOOL_SOUND3")		
		SkillHelpFunc_AttachParticleToNode(10002, "JOB_SKILL_TOOL_EFFECT_ID3", "JOB_SKILL_TOOL_TARGET_NODE3", 1, actor, action)
		--return false
	end
	
	if animDone == true then
		if(false == actor:PlayNext()) then	-- 더이상 애니메이션이 없으면			
			local curActionParam = action:GetParam(0)
			if curActionParam == "GoNext" then
				action:SetParam(0, "null")
				action:SetParam(1, "end")
				return false
			elseif(1 == action:GetParamInt(1000)) then
				action:SetParam(1, "end")
				return false
			end
		end		
	end	

	return true
end

function JobSkill_Process_OnLeave(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	local curAction = actor:GetAction()
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if curAction:GetNextActionName() ~= actionID then
		ODS("다른게 들어옴:"..actionID.."\n",false, 912)
		return false
	end

	SkillHelpFunc_RemoveDarkBG(actor)
	
	if actor:IsMyActor() == false then
		return true
	end
		
	if curParam == "end" then 
		return true
	end
	
	if action:GetEnable() == false then
		if curParam == actionID then
			curAction:SetParam(0,"")
		end
	end
	
	if action:GetEnable() == true then
		curAction:SetParam(0,"GoNext")
		return false
	end
	
	return false 
end

function JobSkill_Process_OnCleanUp(actor)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
    local   kCurAction = actor:GetAction()	
	ODS("RestoreItemModel JobSkill_Process_ReallyLeave\n", false, 912)		
	SkillHelpFunc_RestoreWeapon(actor, kCurAction)					-- 무기를 변경했으면 복구하고	
	SkillHelpFunc_ShowWeapon(actor, kCurAction)						-- 무기가 감춰져있으면 복구하고	
	SkillHelpFunc_DetachWeaponTrail(actor, kCurAction)				-- 무기 꼬리 파티클 제거	
    SkillHelpFunc_RestoreShape(actor, kCurAction)					-- 모습 원래대로	
	SkillHelpFunc_DetachFireEffectWhenSkillEnd(actor, kCurAction)	-- 스킬이 끝날때 공격 동작 파티클 제거	
	SkillHelpFunc_DetachCastingEffectWhenSkillEnd(actor, kCurAction)-- 스킬이 끝날때 캐스팅 이펙트 제거	
	SkillHelpFunc_ExpressCleanUpEffect(actor, kCurAction)			-- CleanUp시(스킬 종료시) 표현할 파티클이 있으면 붙여주고	
	SkillHelpFunc_IfCastingCancelThenDoBattleIdle(actor)			-- 캐스팅 하다 캔슬 되었으면 공격 대기 상태로 바꿈	
	SkillHelpFunc_DetachHitCasterEffect(actor)						-- 때릴때 캐스터에게 붙은 파티클 제거
	actor:DetachFrom(10001, true)
	actor:DetachFrom(10002, true)
	return true
end


function JobSkill_Process_Fire(actor,action)
	ODS("RestoreItemModel JobSkill_Process_Fire\n")	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	SkillHelpFunc_PlayFireSound(actor, action)	-- 공격시 사운드		
	SkillHelpFunc_HideWeapon(actor, action)		-- 무기 감추기 
	SkillHelpFunc_ChangeWeapon(actor, action)	-- 무기 바꾸기 처리	
	SkillHelpFunc_ExpressFireEffect(actor, action)	-- 공격시 파티클 표현
    SkillHelpFunc_AttachWeaponTrail(actor, action)	-- 웨폰 트레일 
	SkillHelpFunc_TransformShape(actor, action)		--  변신 적용
		
end

function JobSkill_Process_DoDamage(actor,actorTarget,kActionResult,actionTargetInfo)
	SkillHelpFunc_DefaultDoDamage(actor,actorTarget,kActionResult,actionTargetInfo)
end

function JobSkill_Process_OnTargetListModified(actor,action,bIsBefore)
	ODS("JobSkill_Process_OnTargetListModified\n", false, 912)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	if(0 == action:GetParamInt(100) ) then
		
		if bIsBefore == false then	--  hit 키 이후라면, 바로 대미지 적용시켜준다.			
			SkillHelpFunc_DefaultHitOneTime(actor,action)
	
			if actor:IsMyActor()  then
				action:GetTargetList():ApplyActionEffects(true,true)	--죽는 이펙트만 적용
				return
			end
		end
		
	end
	
end

function JobSkill_Process_OnEvent(actor,textKey)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
		
	local action = actor:GetAction()
		
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	if action:GetActionParam() == AP_CASTING then
		return true
	end
	
	if textKey == "hit" or textKey == "fire" then

		if actor:IsUnderMyControl() then
			local iTimerTotalTime = action:GetScriptParamAsFloat("HIT_TIMER_TIME")
			if 0 == action:GetParamInt(31) then -- hit이 여러번 있을 수 있기 때문에 처음 한번만 발동 시켜줘야 한다.			
				action:SetParamInt(31,1)		-- 한번만 실행하게끔 되어있는데, 
				
				if 0 == iTimerTotalTime then -- 이건 한방만 때리는 스킬이란 얘기. 여러대 때린다면 아래 처럼 Timer를 사용하게 할것이다.
					action:CreateActionTargetList(actor)
					if IsSingleMode() then    
						SkillHelpFunc_DefaultHitOneTime(actor,action)
						JobSkill_Process_OnTargetListModified(actor,action,false)
						return
					else
						action:BroadCastTargetListModify(actor:GetPilot())
					end
					
				else -- 타이머를 사용하는 경우는, 타이머를 작동 시킴
				
					local iTimeTick = action:GetScriptParamAsFloat("HIT_TIMER_TICK")
					local iTotalHit = SkillHelpFunc_GetTotalHitCount(action)
					-- iTimerTotalTime초 동안, iTimeTick 마다 OnTimer 실행. (Timer아이디는 0)
					if(0 == iTimeTick and 0 < iTotalHit) then
						action:StartTimer(iTimerTime, iTimerTime/iTotalHit, 0)
					else
						action:StartTimer(iTimerTotalTime, iTimeTick, 0)
					end
					
				end
				
			end
		end
		
	end
		
	return	true
end

function JobSkill_Process_OnTimer(actor,fAccumTime,action,iTimerID)	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	if 0 == iTimerID then
		local	iHitCount = SkillHelpFunc_GetHitCount(action)
		local	iTotalHit = SkillHelpFunc_GetTotalHitCount(action)
		
		if iHitCount == iTotalHit then
			ODS("TotalHit이랑 iHit랑 같음"..iHitCount..":"..iTotalHit.."\n", false, 912)
			return true
		end

		local iCreateLimitCnt = action:GetParamInt(10)
		if(0 < iCreateLimitCnt) then
			ODS("iCreateLimitCnt 들어옴\n", false, 912)
			action:CreateActionTargetList(actor)
			iCreateLimitCnt = iCreateLimitCnt-1
			action:SetParamInt(10, iCreateLimitCnt)			
			action:BroadCastTargetListModify(actor:GetPilot())
			action:GetTargetList():SetActionEffectApplied(true)		
		end
		
		if IsSingleMode() then	    
			SkillHelpFunc_DefaultHitOneTime(actor,action)
			JobSkill_Process_OnTargetListModified(actor,action,false)	        
			return true
		else
			-- 실제는 1방 때린것이지만, 클라이언트에서 여러대 때리는것으로 연출
			SkillHelpFunc_DoDividedDamage(actor, action, iHitCount)
			if iHitCount == iTotalHit-1 then
				action:SetParam(1, "end")
			end
		end
		SkillHelpFunc_IncHitCount(action, 1)
	end	
	return true
end
