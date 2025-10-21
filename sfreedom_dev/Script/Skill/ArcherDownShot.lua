
-- 대각선아래방향으로 세발의 화살을 동시에 발사!
-- [PROJECTILE_ID] : 발사할 프로젝틸 ID. 빈문자열일 경우 기본 프로젝틸이 발사된다.
-- [ATTACH_NODE] : 프로젝틸을 붙일 노드. 빈문자열일 경우 무기에 붙이게 된다.
function Archer_Down_Shot_OnCheckCanEnter(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

--	local kCurAction = actor:GetAction()

	local weapontype = actor:GetEquippedWeaponType();
	if weapontype == 0 then
		return false
	end
	
	return true
	
end
function Archer_Down_Shot_OnEnter(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if actor:IsMyActor() == true then
	
		if actor:GetFloatHeight(40)<50 then
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;	
		end
		
		if actor:GetParam("JUMP_DOWN_ATTACK_ENABLE") ~= nil and
		    actor:GetParam("JUMP_DOWN_ATTACK_ENABLE") == "FALSE" then
		    
		    action:SetNextActionName("a_battle_idle");
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;		
			    
		end
		
		actor:SetParam("JUMP_DOWN_ATTACK_ENABLE","FALSE");		
	
	end
	
--	local	actorID = actor:GetID()
--	local	actionID = action:GetID()
	local	weapontype = actor:GetEquippedWeaponType();
	
	ODS("Archer_Down_Shot_OnEnter weapontype : "..weapontype.."\n");

	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
	end
	actor:StopJump();
	actor:FreeMove(true);
	
	actor:SetParam("FLOAT_ATTACK_ENABLE","FALSE");
	
	local iShotTime = action:GetAbil(AT_COUNT)-- 반복해서 쏠 횟수
	if(iShotTime <= 0) then iShotTime = 1 end
	action:SetParamInt(100, iShotTime)
	action:SetParamInt(101, iShotTime) -- 남은 발사 횟수
	action:SetSlot(0)
	actor:PlayCurrentSlot()
	return true
end


function Archer_Down_Shot_LoadToWeapon(actor,action)
	
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

function Archer_Down_Shot_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
--	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	animDone = actor:IsAnimationDone()
	local	nextActionName = action:GetNextActionName()
	local	iMaxActionSlot = action:GetSlotCount();
	local	iCurActionSlot = action:GetCurrentSlot()
--	local	weapontype = actor:GetEquippedWeaponType();
--	local	actionID = action:GetID();

	if animDone == true or actor:IsMeetFloor() then
		local iReaminShotTime = action:GetParamInt(101)
		
		if(iReaminShotTime <= 0) then
			--action:SetSlot(2) 
			actor:PlayNext() -- 마지막 마감 애니메이션
		end
		
		if(actor:IsMeetFloor() or iCurActionSlot == iMaxActionSlot-1) then 
			actor:FreeMove(false);	-- 이제 공중에서 떨어지거나, 움직일수 있음
		
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
		elseif(iReaminShotTime > 0) then -- 화살 발사 애니메이션
			iReaminShotTime = iReaminShotTime - 1
			action:SetSlot(1)				
			actor:PlayCurrentSlot()			
			action:SetParamInt(101, iReaminShotTime)			
			return true
		end
	end

	return true
end
function Archer_Down_Shot_OnCleanUp(actor,action)
	
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end

	actor:FreeMove(false);
	
	return true;
end
function Archer_Down_Shot_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local	curAction = actor:GetAction();
	
	if( CheckNil(nil==curAction) ) then return true end
	if( CheckNil(curAction:IsNil()) ) then return true end
	
	local	curParam = curAction:GetParam(1)
	local	actionID = action:GetID()
	
	ODS("Archer_Down_Shot_OnLeave actionID:"..actionID.."\n");
	
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

function Archer_Down_Shot_OnEvent(actor, textKey)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	if textKey == "fire" or textKey=="hit" then
		local	action = actor:GetAction()
		
		if( CheckNil(nil==action) ) then return false end
		if( CheckNil(action:IsNil()) ) then return false end
	
		local	iAttackRange = action:GetSkillRange(0,actor);
		
		ODS("Archer_Down_Shot_OnEvent iAttackRange : "..iAttackRange.."\n");
		--actor:FreeMove(false);	
		
--		local	kProjectileMan = GetProjectileMan();
		
--		local	iTargetCount = action:GetTargetCount();
--		local	iTargetABVShapeIndex = 0;

		-- 회전축 구하기
		local	kRotateAxis = actor:GetLookingDir();
		kRotateAxis:Cross(Point3(0,0,1));
		kRotateAxis:Unitize();
		fRotateAngle = -20*DEGTORAD;			
		local	i = 0;
		local iShotTime = action:GetParamInt(100)
		if(iShotTime == 0) then iShotTime = 1 end
		local iProjectilNum =  action:GetAbil(AT_PROJECTILE_NUM)
		local iMaxProjectileNum =  iProjectilNum/ iShotTime
		
		
		if(iMaxProjectileNum <= 0) then iMaxProjectileNum = 3 end 		
		local	kActionTargetList = action:GetTargetList();
		
		while i<iMaxProjectileNum do
		
			local	kArrow = Archer_Down_Shot_LoadToWeapon(actor,action);
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
		
			local	kFireTarget = actor:GetLookingDir();
			kFireTarget:Multiply(iAttackRange);
			kFireTarget:Rotate(kRotateAxis,fRotateAngle*(i+1));
			kFireTarget:Add(arrow_worldpos);			
			kArrow:SetTargetLoc(kFireTarget);	
			kArrow:Fire();--	발사!

			i=i+1;
		end
		
		actor:ClearIgnoreEffectList();

	end
end
