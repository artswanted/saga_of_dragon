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
-- [CASTING_EFFECT_TARGET_NODE_TO_WORLD_POS] : 캐스팅 시에 이펙트를 붙을때, 붙일 노드
-- [CASTING_EFFECT_TARGET_NODE] : 캐스팅 시에 이펙트를 붙을때, 붙일 노드
-- [CASTING_EFFECT_EFFECT_ID] : 캐스팅 시에 이펙트 붙일 ID
-- [CASTING_EFFECT_ID_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
-- [CASTING_EFFECT_SIZE_TO_SKILLRANGE] : 캐스팅 시에 이펙트 Size를 스킬 범위에 따라 스케일링 할때
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

-- Melee
function Skill_Master_PhantomClone_OnCheckCanEnter(actor,action)

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
function Skill_Master_PhantomClone_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Master_PhantomClone_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	action:SetParam(3,"0");
end

function Skill_Master_PhantomClone_Fire(actor,action)
	ODS("RestoreItemModel Skill_Master_PhantomClone_Fire\n");
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");
	
	if bUseTLM == false then
--		action:CreateActionTargetList(actor);
	end

	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
	
	actor:HideParts(EQUIP_POS_WEAPON, true);

	actor:AttachParticle(7212, "char_root", "ef_skill_Phantom_Warrior_01_char_root");
	actor:AttachParticle(7213, "char_root", "ef_skill_Phantom_Warrior_02_char_root");
	actor:AttachParticle(7214, "char_root", "ef_skill_Phantom_Warrior_03_char_root");
	actor:AttachParticle(7215, "char_root", "ef_skill_Phantom_Warrior_04_char_root");
		
	if action:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:StartWeaponTrail();
	end		
end

function Skill_Master_PhantomClone_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_Master_PhantomClone_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");

	action:SetParam(3,"0");
	local   kFireEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
    local   kFireEffectTargetNodeID = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	local   kFireEffectTargetNodeIDToPos = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE_TO_WORLD_POS");
    
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= "" then
    
		local iSize = 1.0
		if "TRUE" == action:GetScriptParam("CASTING_EFFECT_SIZE_TO_SKILLRANGE") then
			iSize = iSize * (action:GetSkillRange(0,actor) / 100)
			iSize = iSize * 2 
		end
		
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPointS(7215,kDir,kFireEffectID, iSize);
            
        elseif kFireEffectTargetNodeID ~= "" then
            actor:AttachParticleS(7215,kFireEffectTargetNodeID,kFireEffectID,iSize);
		else
			actor:AttachParticleToPointS(7215,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID,iSize);
        end
		action:SetParam(3,"1");
    end	
	
	local	kSoundID = action:GetScriptParam("CASTING_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2785,kSoundID);
	end	
	
	action:SetParamInt(2,0);

	action:SetParamInt(100,0);
		
	--남성용 사운드
	if 1 == actor:GetAbil(AT_GENDER) then
		local	kSoundID2 = action:GetScriptParam("CASTING_SOUND_ID_MALE");
		if kSoundID2~="" then
			actor:AttachSound(2786,kSoundID2);
			ODS("남자사운드\n", false, 912)
		end	
	--여성용 사운드
	else
		local	kSoundID2 = action:GetScriptParam("CASTING_SOUND_ID_FEMALE");
		if kSoundID2~="" then
			actor:AttachSound(2786,kSoundID2);
			ODS("여자사운드\n", false, 912)
		end	
	end
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Master_PhantomClone_OnCastingCompleted(actor,action);

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 1.0, 0.4,true,true);
		g_world:AddDrawActorFilter(actor:GetPilotGuid());
	end

	-- 뒤에 도깨비 얼굴 생성
	Skill_Master_PhantomClone_Create_Phantom_Warrior(actor);

	actor:SetCanHit(false);
	
	return true
end

function Skill_Master_PhantomClone_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	actor:LockBidirection(false);
	actor:SeeFront(true);
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()

	if 1 == action:GetParamInt(100) then
		action:SetParamInt(100, 0);

		action:CreateActionTargetList(actor);
		action:BroadCastTargetListModify(actor:GetPilot());
		
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
		
	elseif 3 == action:GetParamInt(100) then
		action:SetParamInt(100, 0);
		action:SetParam(1, "end" );
		local iSummonCount = actor:GetSummonUnitCount();
		local i = 0;

		while i < iSummonCount do
			local kSummonGUID = actor:GetSummonUnit(i);
			local kTargetPilot = g_pilotMan:FindPilot(kSummonGUID);
			if kTargetPilot:IsNil() then return true end
			local kTargetActor = kTargetPilot:GetActor()
			if kTargetActor:IsNil() then return true end

			kTargetActor:SetParam("SummonEnd", "true");

			i = i + 1;
		end
		return false;
	end
	
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

function Skill_Master_PhantomClone_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    local   kCurAction = actor:GetAction();
	
	ODS("RestoreItemModel Skill_Master_PhantomClone_ReallyLeave\n");

	actor:HideParts(EQUIP_POS_WEAPON, false);

	if kCurAction:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:EndWeaponTrail();
	end

	actor:ClearSummonUnit();
	
    actor:DetachFrom(7212)
    actor:DetachFrom(7213)
	actor:DetachFrom(7214)

	if "1" == kCurAction:GetParam(3) then
		--캐스팅 중에 캔슬된 상태
		actor:DetachFrom(7215,true)
		if( false == actor:PlayNext()) then 
			kCurAction:SetNextActionName("a_battle_idle");			
		end
		kCurAction:ChangeToNextActionOnNextUpdate(true,true);
	end

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
		g_world:ClearDrawActorFilter()
	end	
		
	actor:RestoreLockBidirection();

	actor:SetCanHit(true);

	return true;
end

function Skill_Master_PhantomClone_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if(nil == string.find(actionID, "a_run")) then
		if curAction:GetNextActionName() ~= actionID then
			ODS("다른게 들어옴:"..actionID.."\n",false, 912)
			return false;
		end
	end
	
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

function Skill_Master_PhantomClone_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
	
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

function Skill_Master_PhantomClone_HitOneTime(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iHitCount = action:GetParamInt(2);
	
	if iHitCount == 0 then
	
	    local   kHitEffectID = action:GetScriptParam("HIT_CASTER_EFFECT_ID");
	    local   kHitEffectTargetNodeID = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE");
		local   kHitEffectTargetNodeIDToPos = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS");
		local   kHitEffectTargetNodeIDToPosWithRotate = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE");

	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" or kHitEffectTargetNodeIDToPos ~= ""  or kHitEffectTargetNodeIDToPosWithRotate ~= "" then
			if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
				actor:AttachParticle(7214,kHitEffectTargetNodeID,kHitEffectID);
			elseif kHitEffectTargetNodeID ~= "" then
	            actor:AttachParticle(7214,kHitEffectTargetNodeID,kHitEffectID);
			elseif kHitEffectTargetNodeIDToPos ~= "" then
				actor:AttachParticleToPoint(7214,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPos),kHitEffectID);			
			else
			    actor:AttachParticleToPointWithRotate(7214,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPosWithRotate), kHitEffectID, actor:GetRotateQuaternion())
		    end
		end
	end

	local	iTotalHit = tonumber(action:GetScriptParam("MULTI_HIT_NUM"));
	if iTotalHit == nil or iTotalHit == 0 then
	    iTotalHit = 1;
	end
	
	if iHitCount == iTotalHit then
		return
	end
	
	local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end	
	
	--남성용 사운드
	if 1 == actor:GetAbil(AT_GENDER) then
		local	kSoundID2 = action:GetScriptParam("HIT_SOUND_ID_MALE");
		if kSoundID2~="" then
			actor:AttachSound(2785,kSoundID2);
		end	
	--여성용 사운드
	else
		local	kSoundID2 = action:GetScriptParam("HIT_SOUND_ID_FEMALE");
		if kSoundID2~="" then
			actor:AttachSound(2785,kSoundID2);
		end	
	end
		
	local iTargetCount = action:GetTargetCount();
	local i =0;
	if iTargetCount>0 then
		
		local bWeaponSoundPlayed = false;
		
		while i<iTargetCount do
		
			local actionResult = action:GetTargetActionResult(i);
			if actionResult:IsNil() == false then
			
				local kTargetGUID = action:GetTargetGUID(i);
				local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
				if kTargetPilot:IsNil() == false and actionResult:IsMissed() == false then
					
					local actorTarget = kTargetPilot:GetActor();
					
                    if bWeaponSoundPlayed == false then
                        bWeaponSoundPlayed = true;
                        -- 피격 소리 재생
						local actionName = action:GetID();
						if actionResult:GetCritical() then
							actionName = "Critical_dmg"
						end
                        actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, action:GetTargetInfo(i));
                    end
					Skill_Master_PhantomClone_DoDamage(actor,actorTarget,actionResult, action:GetTargetInfo(i));
				end
				
			end
			
			i=i+1;
		
		end
	end

	action:GetTargetList():ApplyActionEffects();
	actor:ClearIgnoreEffectList();
	
	iHitCount = iHitCount + 1;
	action:SetParamInt(2,iHitCount);
	
end
function Skill_Master_PhantomClone_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local kTargetList = action:GetTargetList();
	local iTargetCount = kTargetList:size();
	local i = 0;
	while i < iTargetCount do
		local kTargetInfo = kTargetList:GetTargetInfo(i);
		local kTargetGUID = kTargetInfo:GetTargetGUID();
		local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);

		if kTargetPilot:IsNil() == false then
			local   kTargetActor = kTargetPilot:GetActor();
			if kTargetActor:IsNil() == false then
				kTargetActor:SetAbil(AT_NOT_FLY_TO_CAMERA, 1);
			end
		end

		i = i + 1;
	end

end

function Skill_Master_PhantomClone_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
		
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	return	true;
end

function Skill_Master_PhantomClone_Create_Phantom_Warrior(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kTargetPos = actor:GetPos();
	
	local guid = GUID("123")
	guid:Generate()
	
	local pilot = g_pilotMan:NewPilot(guid, 900101, 0)
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
		pilot:GetActor():ReserveTransitAction("a_Phantom_Warrior_idle");
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