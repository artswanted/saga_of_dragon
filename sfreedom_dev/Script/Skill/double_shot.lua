-- [PROJECTILE_ID] : 발사할 발사체 ID, "" 일 경우 무기에 붙어있는 발사체 ID 를 사용한다.
-- [ATTACH_NODE_ID] : 발사체를 붙일 액터의 노드 ID, "" 일 경우 캐릭터 좌표에서 발사된다.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [NO_AUTO_CONNECT] : 다음 액션으로 부드럽게 연결해줄 것인가

-- Melee
function Skill_DoubleShot_OnCheckCanEnter(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	return		true;
	
end
function Skill_DoubleShot_OnCastingCompleted(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	Skill_DoubleShot_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function	Skill_DoubleShot_LoadToWeapon(actor,action)

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

function Skill_DoubleShot_Fire(actor,action)
end

function Skill_DoubleShot_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	action:SetParamInt(2,0);    --  Fire Counter
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	actor:ResetAnimation();

	Skill_DoubleShot_OnCastingCompleted(actor,action);
	
	return true
end

function Skill_DoubleShot_OnUpdate(actor, accumTime, frameTime)

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

function Skill_DoubleShot_OnCleanUp(actor,action)
	return true;
end

function Skill_DoubleShot_OnLeave(actor, action)

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

function Skill_DoubleShot_OnEvent(actor,textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	local kAction = actor:GetAction();
			
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	ODS("Skill_DoubleShot_OnEvent textKey:"..textKey.."\n");
	
	if textKey == "hit" or textKey == "fire" then
	
	    local   iFireCount = kAction:GetParamInt(2);
	    
	    local   iMaxFireCount = 2;
	    
	    if iFireCount == iMaxFireCount then 
	        return
	    end
	    
	    kAction:SetParamInt(2,iFireCount+1);

		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end
		
		local	iAttackRange = kAction:GetSkillRange(0,actor);
		
		if iAttackRange == 0 then
			iAttackRange = 200; 
		end
		
		local	kArrow = Skill_DoubleShot_LoadToWeapon(actor,kAction);

		if kArrow:IsNil() == false then
		
			local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
			
			kMovingDir:Multiply(iAttackRange);
			kMovingDir:Add(arrow_worldpos);
			kArrow:SetTargetLoc(kMovingDir);	
			
			kArrow:SetParamValue("HIT_COUNT",""..iFireCount);
			
			kArrow:Fire();
			
		else
			
			ODS("Arrow is nil\n");
		
		end
					
	end

	return	true;
end
