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
function Skill_Club_Mania_OnCheckCanEnter(actor,action)

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

function Skill_Club_Mania_OnTimer(actor,fAccumTime,action,iTimerID)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	if 0 == iTimerID then
		local	iHitCount = SkillHelpFunc_GetHitCount(action)
		local	iTotalHit = SkillHelpFunc_GetTotalHitCount(action)
		if iHitCount == iTotalHit then
			return true
		end
		
		local iCreateLimitCnt = action:GetParamInt(10)
		if(0 < iCreateLimitCnt) then
			ODS("iCreateLimitCnt 들어옴\n", false, 912)
			action:CreateActionTargetList(actor);		
			iCreateLimitCnt  = iCreateLimitCnt-1
			action:SetParamInt(10, iCreateLimitCnt)			
			action:BroadCastTargetListModify(actor:GetPilot());
			action:GetTargetList():SetActionEffectApplied(true)		
		end
		
		local i = 0;
		local kTargetList = action:GetTargetList();
		local iTargetCount = kTargetList:size();
		while i<iTargetCount do
			local   kTargetInfo = kTargetList:GetTargetInfo(i);
			local   kTargetGUID = kTargetInfo:GetTargetGUID();
			local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
			if kTargetPilot:IsNil() == false then
				if actor:IsMyActor() then
					if( CheckNil(g_world == nil) ) then return false end
					if( CheckNil(g_world:IsNil()) ) then return false end
					g_world:AddDrawActorFilter(kTargetGUID);
				end
			end
			i = i + 1;
		end
		
		if IsSingleMode() then
	    
			SkillHelpFunc_DefaultHitOneTime(actor,action)
			Skill_Club_Mania_OnTargetListModified(actor,action,false)
	        
			return true;
		else

			-- 실제는 1방 때린것이지만, 클라이언트에서 여러대 때리는것으로 연출
			SkillHelpFunc_DoDividedDamage(actor, action, iHitCount)
			if iHitCount == iTotalHit-1 then
				action:SetParam(1, "end")
			end
		end
		SkillHelpFunc_IncHitCount(action, 1)
		
	elseif 1 == iTimerID then -- 드래곤 러쉬 전
		local iSkillRange = action:GetParamInt(21);
		local iIndex = action:GetParamInt(20);

		local kPos = actor:GetPos();	

		kPos:SetX(kPos:GetX() + math.random(-iSkillRange,iSkillRange));
		kPos:SetY(kPos:GetY() + math.random(-iSkillRange,iSkillRange));
		kPos:SetZ(kPos:GetZ() + 200);

		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local ptcl = g_world:ThrowRay(kPos, Point3(0,0,-1),500);
		if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
			--위로도 쏴 본다.
			ptcl = g_world:ThrowRay(kPos, Point3(0,0,1),500);
			if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
				return true;
			end			
		end

		actor:AttachParticleToPointS(1000 + iIndex, ptcl, "ef_skill_dragon_rush_lightning_01", math.random(-1.0, 1));

		action:SetParamInt(20, iIndex + 1);
	end

	return true;

end

function Skill_Club_Mania_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Club_Mania_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	SkillHelpFunc_SetNoneCastingState(action)			-- 캐스팅 완료	
end

function Skill_Club_Mania_Fire(actor,action)
	ODS("RestoreItemModel Skill_Club_Mania_Fire\n");
	
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
	
	actor:SeeFront(true);
	actor:AttachParticle(7217 ,"char_root", "ef_skill_Club_Mania_03_char_root")

	action:SetParamInt(31,0); -- hit_timer를 사용하는지 Check
	action:SetParamInt(20,0);

	local iInstanceID = GetStatusEffectMan():AddStatusEffectToActor(actor:GetPilotGuid(),"se_ClubMania",0,1);
	actor:SetParam(200, tostring(iInstanceID));

end

function Skill_Club_Mania_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local actionID = action:GetID()
	
	ODS("Skill_Club_Mania_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n",false, 912);
	SkillHelpFunc_InitHitCount(action)					-- HitCount 초기화		
	ODS("Club Mania Enter 1\n",false, 912);
	SkillHelpFunc_ExpressCastingEffect(actor, action)	-- 캐스팅때 파티클이 붙어있는지 확인하기 위해서	
	ODS("Club Mania Enter 2\n",false, 912);
	SkillHelpFunc_PlayCastingSound(actor, action) 		-- 캐스팅시 사운드
	ODS("Club Mania Enter 3\n",false, 912);
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Club_Mania_OnCastingCompleted(actor,action);
	ODS("Club Mania Enter 4\n");
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 1.0, 0.4,true,true);
		g_world:AddDrawActorFilter(actor:GetPilotGuid());
	end

	action:CreateActionTargetList(actor,true);
	local i = 0;
	local kTargetList = action:GetTargetList();
	local iTargetCount = kTargetList:size();
	while i<iTargetCount do
		local   kTargetInfo = kTargetList:GetTargetInfo(i);
		local   kTargetGUID = kTargetInfo:GetTargetGUID();
		local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
		if kTargetPilot:IsNil() == false then
			if actor:IsMyActor() then
				if( CheckNil(g_world == nil) ) then return false end
				if( CheckNil(g_world:IsNil()) ) then return false end
				g_world:AddDrawActorFilter(kTargetGUID);
			end
		end
		i = i + 1;
	end
	action:ClearTargetList();	
	actor:SetCanHit(true);
	
	local kMulptiple = action:GetScriptParam("MULTIPLE_ATTACK")
	if kMulptiple~="" then
		action:SetParamInt(100, 1)
		action:SetParamInt(10,1) -- Timer 사용시 CreateTargetList를 1번만 사용하게 하기 위한 값
	end	
	
	return true
end

function Skill_Club_Mania_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	actor:LockBidirection(false);
	actor:SeeFront(true);
		
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
	
	if action:GetParam(1) == "end" then
		return	false;
	end
	
	if animDone == true then
		if(false == actor:PlayNext()) then	-- 더이상 애니메이션이 없으면
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
	end

	return true
end

function Skill_Club_Mania_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    local   kCurAction = actor:GetAction();
	
	ODS("RestoreItemModel Skill_Club_Mania_ReallyLeave\n", false, 912);
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
	return true;
end

function Skill_Club_Mania_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()

	if curAction:GetNextActionName() ~= actionID then
		ODS("다른게 들어옴:"..actionID.."\n",false, 912)
		return false;
	end
	if actor:IsMyActor() == false then
		return true;
	end
	
	if action:GetActionType()=="EFFECT" then
		if actor:IsMyActor() then
			if( CheckNil(g_world == nil) ) then return true end
			if( CheckNil(g_world:IsNil()) ) then return true end
			g_world:SetShowWorldFocusFilterColorAlpha(0xFFFFFF, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
			g_world:ClearDrawActorFilter()
		end
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

function Skill_Club_Mania_OnTargetListModified(actor,action,bIsBefore)

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

function Skill_Club_Mania_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
		
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());

	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then

--		if actor:IsUnderMyControl() then
			local iTimerTime = kAction:GetScriptParamAsFloat("HIT_TIMER_TIME");
			if 0 == iTimerTime then
				if 0 == kAction:GetParamInt(31) then -- hit이 여러번 있을 수 있기 때문에 처음 한번만 발동 시켜줘야 한다.
					kAction:CreateActionTargetList(actor);
					ODS("Skill_Club_Mania_OnEvent : 1\n", false, 912)
					local i = 0;
					local kTargetList = kAction:GetTargetList();
					local iTargetCount = kTargetList:size();
					while i<iTargetCount do
						local   kTargetInfo = kTargetList:GetTargetInfo(i);
						local   kTargetGUID = kTargetInfo:GetTargetGUID();
						local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
						if kTargetPilot:IsNil() == false then
							if actor:IsMyActor() then
								if( CheckNil(g_world == nil) ) then return false end
								if( CheckNil(g_world:IsNil()) ) then return false end
								g_world:AddDrawActorFilter(kTargetGUID);
							end
						end
						i = i + 1;
					end

					if IsSingleMode() then
			        
						SkillHelpFunc_DefaultHitOneTime(actor,kAction)
						Skill_Club_Mania_OnTargetListModified(actor,kAction,false)
			            
						return;
					else
						kAction:BroadCastTargetListModify(actor:GetPilot());
					end

					kAction:SetParamInt(31,1);
					Skill_Club_Mania_OnTargetListModified(actor,kAction,false)
				end
			else -- 타이머를 사용하는 경우
				local iTimerick = kAction:GetScriptParamAsFloat("HIT_TIMER_TICK");
				kAction:StartTimer(iTimerTime, iTimerick,0);
				QuakeCamera(3.8, 1, 1, 1, 1);
			end
--		end	
	elseif textKey == "summon" then
		if "" ~= kAction:GetScriptParam("SUMMON_CLASSNO") then
			local iSummonClassNo = tonumber(kAction:GetScriptParam("SUMMON_CLASSNO"));
			local kSummonReserveAction = kAction:GetScriptParam("SUMMON_RESERVE_ACTION");
			Skill_Club_Mania_Create_Summon(actor,iSummonClassNo,kSummonReserveAction);
			QuakeCamera(2, 3, 1, 1, 1);
		end
	end

	return	true;
end

function Skill_Club_Mania_Create_Summon(actor, iClassNo, kReserveAction)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kTargetPos = actor:GetPos();
	
	local guid = GUID("123")
	guid:Generate()
	
	local pilot = g_pilotMan:NewPilot(guid, iClassNo, 0)
	if pilot:IsNil() == false then
		pilot:SetUnit(guid,UT_ENTITY,1,1,0);
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, pilot:GetActor(), kTargetPos, 9);
	
	    if actor:IsMyActor() then
	        pilot:GetActor():SetUnderMyControl(true);
	    end

		pilot:GetUnit():SetCaller(actor:GetPilotGuid());
		pilot:GetActor():FreeMove(true)
		pilot:GetActor():ReserveTransitAction(kReserveAction);
		--ODS("액션 : "..kReserveAction.. "\n",false, 912)
		pilot:GetActor():ClearActionState();
		pilot:GetActor():FindPathNormal();
		pilot:GetActor():LockBidirection(false);
		pilot:GetActor():ToLeft(actor:IsToLeft());
		pilot:GetActor():SeeFront(true,true);
		pilot:GetActor():SetTargetAlpha(0,1,0.2)
		
		if actor:IsMyActor() then
	        g_world:AddDrawActorFilter(guid);
	    end		
	end	

end
