-- [PROJECTILE_ID] : 발사할 발사체 ID, "" 일 경우 무기에 붙어있는 발사체 ID 를 사용한다.
-- [ATTACH_NODE_ID] : 발사체를 붙일 액터의 노드 ID, "" 일 경우 캐릭터 좌표에서 발사된다.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [NO_AUTO_CONNECT] : 다음 액션으로 부드럽게 연결해줄 것인가
-- [PIERCING] : 관통형인가
-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [FIRE_CASTER_EFFECT_TARGET_NODE] : HIT 키에서 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [FIRE_CASTER_EFFECT_ID] : HIT 키에서 캐스터에게 붙일 이펙트 ID
-- [WEAPON_HIDE] : 무기를 그냥 숨기기만 할때

-- Melee
function Skill_HommingMissile_OnCheckCanEnter(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_HommingMissile_OnCastingCompleted(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	Skill_HommingMissile_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function	Skill_HommingMissile_LoadToWeapon(actor,action)

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
	return kNewArrow;

end

function Skill_HommingMissile_Fire(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	action:CreateActionTargetList(actor,true);
	
	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"));
		ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n");
	end

	-- 무기 숨기기 처리
	if action:GetScriptParam("WEAPON_HIDE") == "TRUE" then
		actor:HideParts(6,true); -- 6번이 무기
	end

	local	iTargetCount = 0;

	iTargetCount = action:GetTargetCount();
	ODS("Skill_HommingMissile_Fire iTargetCount : "..iTargetCount.."\n");	
	
	if iTargetCount>0 then
			--	타겟을 찾아 바라본다
		local	kTargetGUID = action:GetTargetGUID(0);
		local	targetobject = g_pilotMan:FindPilot(kTargetGUID);
		local	iTargetABVShapeIndex = action:GetTargetABVShapeIndex(0);
		if targetobject:IsNil() == false then
			local pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
			actor:LookAt(pt);
		end
	end

	GetActionResultCS(action, actor)
	return	true;
	
end
function Skill_HommingMissile_OnFindTarget(actor,action,kTargets)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);
	local kParam = FindTargetParam();
	-- 회전축 구하기
	local	kRotateAxis = actor:GetLookingDir();
	kRotateAxis:Cross(Point3(0,0,1));
	kRotateAxis:Unitize();
	local	fRotateAngle = 60.0 * math.pi / 180.0;
	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 30
	end
	
    -- 맨 뒤에 있는 녀석을 찾는다.
    kParam:SetParam_1(kPos,actor:GetLookingDir());
    kParam:SetParam_2(action:GetSkillRange(0,actor),iParam2,0,0);
	kParam:SetParam_3(true,FTO_BLOWUP);	
	local	kFireTargetDir = actor:GetLookingDir();
	kFireTargetDir:Rotate(kRotateAxis,fRotateAngle);
	kParam:SetParam_1(kPos,kFireTargetDir);

	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);
	return	kTargets:size();
	
end

function Skill_HommingMissile_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local actorID = actor:GetID()
	local actionID = action:GetID()
	local weapontype = actor:GetEquippedWeaponType();
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	actor:ResetAnimation();

	Skill_HommingMissile_OnCastingCompleted(actor,action);

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

	
	return true
end

function Skill_HommingMissile_OnUpdate(actor, accumTime, frameTime)

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

function Skill_HommingMissile_OnCleanUp(actor)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local   kCurAction = actor:GetAction();
	
	if( CheckNil(nil==kCurAction) ) then return false end
	if( CheckNil(kCurAction:IsNil()) ) then return false end
	

	actor:DetachFrom(7212,true);

	--	무기 원래대로
	if actor:GetAction():GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON);
	end

	-- 무기를 숨겼을 경우 원래대로
	if kCurAction:GetScriptParam("WEAPON_HIDE") == "TRUE" then
		actor:HideParts(6,false); -- 6번이 무기
	end
	
	return true;
end

function Skill_HommingMissile_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
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
	
    return false 
end

function Skill_HommingMissile_OnEvent(actor,textKey)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local kAction = actor:GetAction();
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	ODS("Skill_HommingMissile_OnEvent textKey:"..textKey.."\n");
	
	if textKey == "hit" or textKey == "fire" then

		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
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
		local	kArrow = Skill_HommingMissile_LoadToWeapon(actor,kAction);
		
		local	targetobject = nil;

		if kArrow:IsNil() == false then
		
			local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			
			local	iTargetCount = kAction:GetTargetCount();
			local	iTargetABVShapeIndex = 0;
			local	kActionTargetInfo = nil
			
			local pt = nil;
			
			if iTargetCount>0 then
					--	타겟을 찾아 바라본다
				local	kTargetGUID = kAction:GetTargetGUID(0);
				iTargetABVShapeIndex = kAction:GetTargetABVShapeIndex(0);
				kActionTargetInfo = kAction:GetTargetInfo(0);
				targetobject = g_pilotMan:FindPilot(kTargetGUID);
				if targetobject:IsNil() == false then
					pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
					actor:LookAt(pt);
				end						
			end
			
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
			
			local	kRotateAxis = actor:GetLookingDir();
			kRotateAxis:Cross(Point3(0,0,1));
			kRotateAxis:Unitize();
			local	fRotateAngle = 30.0*3.141592/180.0;
			local	kFireTargetDir = actor:GetLookingDir();
			kFireTargetDir:Rotate(kRotateAxis,fRotateAngle);
			kArrow:SetVelocity(kFireTargetDir);
			kArrow:SetMovingType(3)	
			kArrow:SetParam(500,2000,0);
	    
			if targetobject == nil or targetobject:IsNil()==true or kActionTargetInfo==nil or kActionTargetInfo:IsNil() == true then
				
				kFireTargetDir:Multiply(iAttackRange-45);
				kFireTargetDir:Add(arrow_worldpos);
				kFireTargetDir:SetZ(kFireTargetDir:GetZ())
				kArrow:SetTargetLoc(kFireTargetDir);		
				
			else
				kArrow:SetTargetObjectList(kAction:GetTargetList());	
				
			    if pt~= nil then
			
				local	fVariationAmount = 10.0
				
				pt:Add(Point3(math.random(-fVariationAmount,fVariationAmount),
								math.random(-fVariationAmount,fVariationAmount),
								math.random(-fVariationAmount,fVariationAmount)));
				kArrow:SetTargetLoc(pt);
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
