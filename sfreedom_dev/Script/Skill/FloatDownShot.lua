
-- 대각선아래방향으로 세발의 화살을 동시에 발사!
-- [PROJECTILE_ID] : 발사할 프로젝틸 ID. 빈문자열일 경우 기본 프로젝틸이 발사된다.
-- [ATTACH_NODE] : 프로젝틸을 붙일 노드. 빈문자열일 경우 무기에 붙이게 된다.
function Skill_FloatDownShot_OnCastingCompleted(actor,action)
end

function Skill_FloatDownShot_OnCheckCanEnter(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local weapontype = actor:GetEquippedWeaponType();
	if weapontype == 0 then
		return false
	end
	
	if actor:IsMeetFloor()  then
	    return  false
    end

	if actor:GetPilot():IsCorrectClass(CT_RANGER) then
		local	iShotCount = tonumber(actor:GetParam("RANGER_FLOATDOWNSHOT_COUNT"));
		if iShotCount==nil then
			iShotCount = 0
		end

		local iMaxShotCount = 1;

		local kSkillDef = GetSkillDef(action:GetActionNo());
		if kSkillDef:IsNil() == false then
			local iMaxCount = kSkillDef:GetAbil(AT_COUNT);
			if iMaxCount > 0 then
				iMaxShotCount = iMaxCount;
			end
		end

		if iShotCount >= iMaxShotCount then
			return false;
		end	
	end

	return true
	
end
function Skill_FloatDownShot_OnEnter(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Skill_FloatDownShot_OnEnter\n");

	if actor:IsMyActor() == true then
		if actor:GetFloatHeight(40)<50 then
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;	
		end
		
		local	iShotCount = tonumber(actor:GetParam("RANGER_FLOATDOWNSHOT_COUNT"));
		if iShotCount==nil then
			iShotCount = 0
		end
		
		ODS("Skill_FloatDownShot_OnEnter iShotCount:"..iShotCount.."\n");
		
		actor:SetParam("RANGER_FLOATDOWNSHOT_COUNT",""..(iShotCount+1))
		
	end

	
	actor:ResetAnimation();
	actor:StopJump();
	actor:FreeMove(true);
	
	return true
end


function Skill_FloatDownShot_LoadToWeapon(actor,action)

	if( CheckNil(nil==actor) ) then return nil end
	if( CheckNil(actor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==action) ) then return nil end
	if( CheckNil(action:IsNil()) ) then return nil end
	
	local	kProjectileID = actor:GetEquippedWeaponProjectileID();
	local	kAttackNodeName = "";
	
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	end
	
	kAttackNodeName = "p_ef_heart";
	
	local	kPilotGuid = actor:GetPilotGuid();
	local	kProjectileMan = GetProjectileMan();
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,kPilotGuid);
	if kAttackNodeName == "" then
		kNewArrow:LoadToWeapon(actor);	--	장전
	else
		kNewArrow:LoadToHelper(actor,kAttackNodeName);
	end
	
	return kNewArrow;
end

function Skill_FloatDownShot_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	animDone = actor:IsAnimationDone()
	local	nextActionName = action:GetNextActionName()
--	local	iMaxActionSlot = action:GetSlotCount();
--	local	weapontype = actor:GetEquippedWeaponType();
--	local	actionID = action:GetID();

	if animDone == true or actor:IsMeetFloor() then
	
--		local	curActionSlot = action:GetCurrentSlot()
		local	curActionParam = action:GetParam(0)
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
function Skill_FloatDownShot_OnCleanUp(actor,action)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	actor:FreeMove(false);
	
	return true;
end
function Skill_FloatDownShot_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	if( CheckNil(nil==curAction) ) then return true end
	if( CheckNil(curAction:IsNil()) ) then return true end
	
	local	curParam = curAction:GetParam(1)
	local	actionID = action:GetID()
	
	ODS("Skill_FloatDownShot_OnLeave actionID:"..actionID.."\n");
	
	if actor:IsMyActor() == false then
	
		if actionID == "a_jump" then
			action:SetSlot(2)
		end
	
		return true;
	end
	if action:GetActionType()=="EFFECT"  then
		return true;
	end	
	if curParam == "end" and actionID == "a_jump" then 
		action:SetSlot(2)
		return  true;
	end

	if action:GetEnable() == false then
		
		if curAction:GetParam(1)==actionID then
			curAction:SetParam(0,"");
		end
		
	end	
	if action:GetEnable() == true then
		
		if curParam == "end" then 
			return true;
		end
		
		if actionID == "a_archer_shot_01" then
		    return  false
        end
	
		curAction:SetParam(0,"GoNext");
		curAction:SetParam(1,actionID);
		
		return false;
	
	elseif curParam == "end" and
		(actionID == "a_run_right" or
		actionID == "a_run_left") then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Skill_FloatDownShot_OnEvent(actor, textKey)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
		
	if textKey == "fire" or textKey=="hit" then
		local	action = actor:GetAction()
		
		if( CheckNil(nil==action) ) then return end
		if( CheckNil(action:IsNil()) ) then return end
	
		local	iAttackRange = action:GetSkillRange(0,actor);
		
		if iAttackRange == 0 then
			iAttackRange = 200
		end
		
		ODS("Skill_FloatDownShot_OnEvent iAttackRange : "..iAttackRange.."\n");
		
--		local	kProjectileMan = GetProjectileMan();
		
--		local	iTargetCount = action:GetTargetCount();
--		local	iTargetABVShapeIndex = 0;

		-- 회전축 구하기
		local	kRotateAxis = actor:GetLookingDir();
		kRotateAxis:Cross(Point3(0,0,1));
		kRotateAxis:Unitize();
		fRotateAngle = -40*3.141592/180.0;			
		
		local	kArrow = Skill_FloatDownShot_LoadToWeapon(actor,action);
		if(nil == kArrow ) then return end
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
	
		local	kFireTarget = actor:GetLookingDir();
		kFireTarget:Multiply(iAttackRange);
		kFireTarget:Rotate(kRotateAxis,fRotateAngle);
		kFireTarget:Add(arrow_worldpos);			
		kArrow:SetTargetLoc(kFireTarget);	
		kArrow:Fire();--	발사!

	end
end
