-- [PROJECTILE_ID] : 발사할 발사체 ID, "" 일 경우 무기에 붙어있는 발사체 ID 를 사용한다.
-- [ATTACH_NODE_ID] : 발사체를 붙일 액터의 노드 ID, "" 일 경우 캐릭터 좌표에서 발사된다.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [FIRE_SOUND_ID] : 시전시에 플레이 할 사운드 ID
-- [NO_AUTO_CONNECT] : 다음 액션으로 부드럽게 연결해줄 것인가
-- [PIERCING] : 관통형인가
-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_CASTER_EFFECT_TARGET_NODE] : HIT 키에서 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [HIT_CASTER_EFFECT_ID] : HIT 키에서 캐스터에게 붙일 이펙트 ID
-- [CASTING_EFFECT_TARGET_NODE] : 캐스팅 시에 이펙트를 붙을때, 붙일 노드
-- [CASTING_EFFECT_EFFECT_ID] : 캐스팅 시에 이펙트 붙일 ID
-- [SKILL_LEVEL_TO_SIZE] : 스킬 레벨에 따른 사이즈 변화
-- [SKILL_LEVEL_TO_SIZE_START_EFFECT_SIZE] : 스킬 레벨에 따른 사이즈가 변화 하는 발사체의 시작 크기
-- [SKILL_LEVEL_TO_SIZE_DOT_POINT] : 크기 증가시에 얼마씩 증가 하는가
-- [SKILL_LEVEL_TO_SIZE_LEVEL_POINT] : 스킬이 몇 증가할때마다 크기가 증가 하는가
-- [FIRE_CASTER_EFFECT_ID] : 스킬 발동시에 붙일 이펙트
-- [FIRE_CASTER_EFFECT_TARGET_NODE] : 스킬 발동시에 붙일 이펙트 노드
-- [DETACH_FIRE_CASTER_EFFECT_AT_HIT] : Hit 타이밍에 때어낼 것인가.
-- [HIDE_WEAPON] : 무기를 숨길때
-- [PENETRATION_COUNT] : 몇번을 관통할것인지 세팅

-- Melee
function Projectile_Default_OnCheckCanEnter(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Projectile_Default_OnCastingCompleted(actor, action)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	Projectile_Default_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
end

function	Projectile_Default_LoadToWeapon(actor,action)
	
	if( CheckNil(nil==actor) ) then return nil end
	if( CheckNil(actor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==action) ) then return nil end
	if( CheckNil(action:IsNil()) ) then return nil end
	
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	
	if kProjectileID == "" then
		kProjectileID = actor:GetEquippedWeaponProjectileID();
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	local	kTargetNodeID = action:GetScriptParam("ATTACH_NODE_ID");
	
	kNewArrow:LoadToHelper(actor,kTargetNodeID);	--	장전
	return	kNewArrow;
end

function Projectile_Default_Fire(actor,action)
	
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"));
		ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n");
	end	

	if action:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, true);
	end

	local kStartSize = 1;
	local kDotPoint = 0;
	local kLevelUpPoint = 0;
	local iSkillLevel = 0;

	local kUseSkillSize = action:GetScriptParam("SKILL_LEVEL_TO_SIZE");
	if kUseSkillSize == "TRUE" then
		kStartSize = tonumber(action:GetScriptParam("SKILL_LEVEL_TO_SIZE_START_EFFECT_SIZE"));
		kDotPoint = tonumber(action:GetScriptParam("SKILL_LEVEL_TO_SIZE_DOT_POINT"));
		kLevelUpPoint =  tonumber(action:GetScriptParam("SKILL_LEVEL_TO_SIZE_LEVEL_POINT"));

		if kStartSize == nil then
			kStartSize = 1;
		end

		if kLevelUpPoint == nil then
			kLevelUpPoint = 1;
		end

		if kDotPoint == nil then
			kDotPoint = 0;
		end

		iSkillLevel = action:GetAbil(AT_LEVEL);
		if 0 == iSkillLevel then
			iSkillLevel = 1;
		end
		iSkillLevel = iSkillLevel / kLevelUpPoint;
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
			actor:AttachParticleS(7212,kFireEffectTargetNodeID,kFireEffectID, kStartSize + (iSkillLevel * kDotPoint));
        end
    
    end	

	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
end


function Projectile_Default_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
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

	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	actor:ResetAnimation();

	Projectile_Default_OnCastingCompleted(actor,action);
	
	return true
end

function Projectile_Default_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
		
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
	
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

function Projectile_Default_OnCleanUp(actor)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
		
	--	무기 원래대로
	local curAction = actor:GetAction();
	
	if( CheckNil(nil==curAction) ) then return end
	if( CheckNil(curAction:IsNil()) ) then return end	

	if curAction:GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON);
	end

	if curAction:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, false);
	end
	
	actor:DetachFrom(7212,true)

	return true;
end

function Projectile_Default_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local curAction = actor:GetAction();
	
	if( CheckNil(nil==curAction) ) then return true end
	if( CheckNil(curAction:IsNil()) ) then return true end
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if actionID == "a_jump" then 
		return false;
	end
	
	if action:GetActionType()=="EFFECT"  then
		
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
		if curAction:GetScriptParam("NO_AUTO_CONNECT") ~= "TRUE" then
			curAction:SetParam(0,"GoNext");
			curAction:SetParam(1,actionID);
		end

		return false;
	
	elseif curParam == "end" and (actionID == "a_run_right" or actionID == "a_run_left") then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Projectile_Default_OnEvent(actor,textKey)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local kAction = actor:GetAction();
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	ODS("Projectile_Default_OnEvent textKey:"..textKey.."\n");
	
	if textKey == "hit" or textKey == "fire" then

		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end

		if kAction:GetScriptParam("DETACH_FIRE_CASTER_EFFECT_AT_HIT") == "TRUE" then
		    actor:DetachFrom(7212,true)
		end
		
	    local   kHitEffectID = kAction:GetScriptParam("HIT_CASTER_EFFECT_ID");
	    local   kHitEffectTargetNodeID = kAction:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE");
	    
	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
	        actor:AttachParticle(7213,kHitEffectTargetNodeID,kHitEffectID);
	    end
	    		
		local	iAttackRange = kAction:GetSkillRange(0,actor);
		
		if iAttackRange == 0 then
			iAttackRange = 200; 
		end
		
		local	kProjectileMan = GetProjectileMan();
		local	kArrow = Projectile_Default_LoadToWeapon(actor,kAction);

		local kUseSkillSize = kAction:GetScriptParam("SKILL_LEVEL_TO_SIZE");
			
		if kUseSkillSize == "TRUE" then
			local kStartSize = tonumber(kAction:GetScriptParam("SKILL_LEVEL_TO_SIZE_START_EFFECT_SIZE"));
			local kDotPoint = tonumber(kAction:GetScriptParam("SKILL_LEVEL_TO_SIZE_DOT_POINT"));
			local kLevelUpPoint =  tonumber(kAction:GetScriptParam("SKILL_LEVEL_TO_SIZE_LEVEL_POINT"));

			if kStartSize == nil then
				kStartSize = 1;
			end

			if kLevelUpPoint == nil then
				kLevelUpPoint = 1;
			end

			if kDotPoint == nil then
				kDotPoint = 0;
			end


			local	iSkillLevel = kAction:GetAbil(AT_LEVEL);
			if 0 == iSkillLevel then
				iSkillLevel = 1;
			end
			iSkillLevel = iSkillLevel / kLevelUpPoint;
			kArrow:SetScale(kStartSize + (iSkillLevel * kDotPoint));
		end
		
		local	targetobject = nil;

		if kArrow:IsNil() == false then
		
			local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치

			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
		
			kMovingDir:Multiply(iAttackRange);
			kMovingDir:Add(arrow_worldpos);
			kArrow:SetTargetLoc(kMovingDir);	
			
			local iPentrationCount = kAction:GetScriptParam("PENETRATION_COUNT");
			if iPentrationCount == nil then
				iPentrationCount = "0";
			end

			kArrow:SetPenetrationCount(tonumber(iPentrationCount))-- 몇번을 관통할것인지 세팅
			if iPentrationCount ~= 0 then
				kArrow:SetMultipleAttack(true);
			end

			kArrow:Fire();
			
		else
			
			ODS("Arrow is nil\n");
		
		end
		
		actor:ClearIgnoreEffectList();
					
	end

	return	true;
end
function	Act_Mon_Shot_Around_Projectile_Default_LoadToWeapon(actor,action)

	if( CheckNil(nil==actor) ) then return nil end
	if( CheckNil(actor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==action) ) then return nil end
	if( CheckNil(action:IsNil()) ) then return nil end
	
	local	kProjectileMan = GetProjectileMan();
	--local	kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	local	kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID")--..kAttachID)
	
	if kProjectileID == "" then
		kProjectileID = actor:GetEquippedWeaponProjectileID();
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	local	kTargetNodeID = action:GetScriptParam("ATTACH_NODE_ID");
	
	kNewArrow:LoadToHelper(actor,kTargetNodeID);	--	장전
	return	kNewArrow;
end