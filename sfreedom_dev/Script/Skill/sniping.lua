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
function Sniping_OnCheckCanEnter(actor,action)

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

function Sniping_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	SkillHelpFunc_SetNoneCastingState(action)			-- 캐스팅 완료	
	action:SetParamInt(102, 0)
end

function Sniping_Fire(actor,action)
	ODS("RestoreItemModel Sniping_Fire\n");
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	SkillHelpFunc_PlayFireSound(actor, action)	-- 공격시 사운드		
	SkillHelpFunc_HideWeapon(actor, action)		-- 무기 감추기 
	SkillHelpFunc_ChangeWeapon(actor, action)	-- 무기 바꾸기 처리	
	SkillHelpFunc_ExpressFireEffect(actor, action)	-- 공격시 파티클 표현
    SkillHelpFunc_AttachWeaponTrail(actor, action)	-- 웨폰 트레일 
	SkillHelpFunc_TransformShape(actor, action)		--  변신 적용

	SkillHelpFunc_InitHitCount(action) -- Hit Count
	--action:SetParamInt(30, SkillHelpFunc_GetTotalHitCount(action))
	action:SetParamInt(31,0); -- hit_timer를 사용하는지 Check

end

function Sniping_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local actionID = action:GetID()
	
	--ODS("Sniping_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	ODS("action:GetSlotCount() :"..action:GetSlotCount().."\n", false, 912);
	
	SkillHelpFunc_InitHitCount(action)					-- HitCount 초기화	
	SkillHelpFunc_ExpressCastingEffect(actor, action)	-- 캐스팅때 파티클이 붙어있는지 확인하기 위해서	
	SkillHelpFunc_PlayCastingSound(actor, action) 		-- 캐스팅시 사운드	
		
	if action:GetActionParam() == AP_CASTING then		--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end
	Sniping_OnCastingCompleted(actor,action);
	SkillHelpFunc_InitMaxFireCnt(action)

	local kMulptiple = action:GetScriptParam("MULTIPLE_ATTACK")
	if kMulptiple~="" then
		action:SetParamInt(100, 1)
		action:SetParamInt(10,1) -- Timer 사용시 CreateTargetList를 1번만 사용하게 하기 위한 값
	end
	
	SkillHelpFunc_AttachParticleToNode(10001, "START_EFFECT_ID", "START_EFFECT_TARGET_NODEI", 1, actor, action)
	return true
end

function Sniping_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
		
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
	local iSlot = action:GetCurrentSlot()
	local iMaxActionSlot = action:GetSlotCount();
	
	if action:GetParam(1) == "end" then
		return	false;
	end
	
	local iMaxAttackCnt = SkillHelpFunc_GetMaxFireCnt(action)
	local iCurAttackCnt = SkillHelpFunc_GetCurFireCnt(action)		

	if animDone == true then
		if(action:GetParamInt(102) == 0 ) then 
			action:SetParamInt(102, 1)			-- 불필요한 애니메이션 종료
			--action:SetSlot(action:GetCurrentSlot()+1);		actor:PlayCurrentSlot();
			actor:PlayNext()
			action:SetSlot(action:GetCurrentSlot()+1);			
			return true
		end

		if(action:GetCurrentSlot() ~= action:GetSlotCount()) then
			actor:PlayCurrentSlot();
		end	
		if iCurAttackCnt >= iMaxAttackCnt then		
			action:SetParam(1, "end")
			return false;
		end		
		SkillHelpFunc_IncCurFireCnt(action, 1) 
	end


	return true
end

function Sniping_OnCleanUp(actor, kNextAction)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(10001, true)
	
    local   kCurAction = actor:GetAction();
	if("end" ~= kCurAction:GetParam(1)) and true == actor:IsUnderMyControl() then
		if(nil ~= kNextAction )  then
			if( false == kNextAction:IsNil() ) then
				if( "a_dmg" ~= kNextAction:GetID() ) then
					return
				end
			end
		end		
	end
		
	SkillHelpFunc_RestoreWeapon(actor, kCurAction)					-- 무기를 변경했으면 복구하고	
	SkillHelpFunc_ShowWeapon(actor, kCurAction)						-- 무기가 감춰져있으면 복구하고	
	SkillHelpFunc_DetachWeaponTrail(actor, kCurAction)				-- 무기 꼬리 파티클 제거	
    SkillHelpFunc_RestoreShape(actor, kCurAction)					-- 모습 원래대로	
	SkillHelpFunc_DetachFireEffectWhenSkillEnd(actor, kCurAction)	-- 스킬이 끝날때 공격 동작 파티클 제거	
	SkillHelpFunc_DetachCastingEffectWhenSkillEnd(actor, kCurAction)-- 스킬이 끝날때 캐스팅 이펙트 제거	
	SkillHelpFunc_ExpressCleanUpEffect(actor, kCurAction)			-- CleanUp시(스킬 종료시) 표현할 파티클이 있으면 붙여주고	
	SkillHelpFunc_IfCastingCancelThenDoBattleIdle(actor)			-- 캐스팅 하다 캔슬 되었으면 공격 대기 상태로 바꿈	
	SkillHelpFunc_DetachHitCasterEffect(actor)						-- 때릴때 캐스터에게 붙은 파티클 제거


	
	return true;
end

function Sniping_OnLeave(actor, action)

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
	
	if curParam == "end" then 
		return true;
	end
	
	if action:GetActionType()=="EFFECT" 
		or "a_run" == actionID 	or "a_jump" == actionID 
	then
		curAction:SetParam(1,"end");
		return true;
	end	
	
	if action:GetEnable() == false then
		if curParam == actionID then
			curAction:SetParam(0,"");
		end
	end
	
	if action:GetEnable() == true then
	-- 다시 Update로 돌려보내서 끝내기 처리를 할수 있게함
		curAction:SetParam(0,"GoNext");
		return false;
	end

	return false 
end

function Sniping_OnTargetListModified(actor,action,bIsBefore)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if(0 == action:GetParamInt(100) ) then
		
		if bIsBefore == false then	
			--  hit 키 이후라면
			--  바로 대미지 적용시켜준다.
			SkillHelpFunc_DefaultHitOneTime(actor,action);
		end
		
	end
	
end

function Sniping_OnEvent(actor,textKey)

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
			Sniping_Fire(actor,kAction);
			
			local iTimerTime = kAction:GetScriptParamAsFloat("HIT_TIMER_TIME");
			if 0 == iTimerTime then
				kAction:CreateActionTargetList(actor);								
				
				local i = 0;
				local kTargetList = kAction:GetTargetList();
				local iTargetCount = kTargetList:size();
				while i<iTargetCount do
					local   kTargetInfo = kTargetList:GetTargetInfo(i);
					local   kTargetGUID = kTargetInfo:GetTargetGUID();
					local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
					if kTargetPilot:IsNil() == false then						
						local kTargetActor = kTargetPilot:GetActor();
						--kTargetActor:AttachParticle(7215, "char_root", "eff_skill_Sniping_00");
						--kTargetActor:AttachParticle(7215, "p_ef_heart", "eff_skill_Sniping_00");
						kTargetActor:AttachParticle(7215, "p_ef_heart", "ef_select_01");
					end
					i = i + 1;
				end
								
				kAction:BroadCastTargetListModify(actor:GetPilot());
		
			else -- 타이머를 사용하는 경우
				local iTimerick = kAction:GetScriptParamAsFloat("HIT_TIMER_TICK");
				kAction:StartTimer(iTimerTime, iTimerick,0);
			end

		end		
	end
			
	return	true;
end

function Sniping_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100
	end
	
	return  Sniping_FindTargetEx(actor,action,action:GetActionNo(),iAttackRange,kTargets);
	
end
function    Sniping_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)

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

	local iTargets = action:FindTargetsEx(iActionNo,iFindTargetType,kParam,kTargets,kTargets);
	
	return iTargets;
end