function MagicianFloatShot_OnCastingCompleted(actor,action)
end

function MagicianFloatShot_OnCheckCanEnter(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end	
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	
	if actor:IsMeetFloor() == false then
		return	false;	
	end
		
	return		true;
	
end
function MagicianFloatShot_FindTarget(kActor,kAction)
	
	if( CheckNil(nil==kActor) ) then return end
	if( CheckNil(kActor:IsNil()) ) then return end
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	local	kTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo());
	local	iFound = MagicianFloatShot_OnFindTarget(kActor,kAction,kTargets);
	
	return	kTargets;
end
function MagicianFloatShot_OnFindTarget(actor,action,kTargets)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iAttackRange = action:GetSkillRange(0,actor)
	
	return	MagicianFloatShot_OnFindTargetEx(actor,action,action:GetActionNo(),iAttackRange,kTargets);
end

function MagicianFloatShot_OnFindTargetEx(actor,action,iActionID,iAttackRange,kTargets)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	kParam = FindTargetParam();
	
	local	kPos = actor:GetPos();
	local	kMovingDir = actor:GetLookingDir();
	
	local	fFrontMove = 60;
	
	kMovingDir:Multiply(fFrontMove);
	
	kPos:Add(kMovingDir);
	kPos:SetZ(kPos:GetZ()+60);

	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(0,0,150,0);
	kParam:SetParam_3(true,FTO_BLOWUP);
	local iTargetCount = action:FindTargetsEx(iActionID,TAT_SPHERE,kParam,kTargets,kTargets);
	
	ODS("MagicianFloatShot_OnFindTargetEx iTargetCount:"..iTargetCount.."\n");
	
	return	iTargetCount;
	
end


function MagicianFloatShot_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local	actorID = actor:GetID()
--	local	actionID = action:GetID()
--	local	weapontype = actor:GetEquippedWeaponType();
	
	ODS("MagicianFloatShot_OnEnter\n");
		
	MagicianFloatShot_Fire(actor,action);
	
	actor:ResetAnimation();
		
	action:SetParamInt(6,0);
	
	return true
end
function MagicianFloatShot_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iTargetCount = 0;

	if actor:IsMyActor() == true then
	
		local kTargets = MagicianFloatShot_FindTarget(actor,action);
		action:SetTargetList(kTargets);
		DeleteActionTargetList(kTargets);
		
		iTargetCount = kTargets:size();
		
	end
	
	iTargetCount = action:GetTargetCount();
	ODS("MagicianFloatShot_Fire iTargetCount : "..iTargetCount.."\n");	
	
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

function	MagicianFloatShot_LoadArrowToWeapon(actor,action)

	if( CheckNil(nil==actor) ) then return nil end
	if( CheckNil(actor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==action) ) then return nil end
	if( CheckNil(action:IsNil()) ) then return nil end
	
	local	kProjectileMan = GetProjectileMan();
	local	kNewArrow = kProjectileMan:CreateNewProjectile("PROJECTILE_MAGICIAN_DEFAULT_SHOT",action,actor:GetPilotGuid());

	local	kPosition = actor:GetNodeWorldPos("p_ef_heart");
	local	kDir = actor:GetLookingDir();
	local	fMoveDistance = 30.0;
	
	kDir:Multiply(fMoveDistance);
	kPosition:Add(kDir);
	
	kNewArrow:SetWorldPos(kPosition);
	
	return kNewArrow;
end

function MagicianFloatShot_Finished(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	nextActionName = action:GetNextActionName()
--	local	iMaxActionSlot = action:GetSlotCount();
--	local	actionID = action:GetID();
			
	actor:SetNormalAttackEndTime();

	local	curActionSlot = action:GetCurrentSlot()
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

function MagicianFloatShot_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
--	local	actorID = actor:GetID()
	local	action = actor:GetAction()
			
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	animDone = actor:IsAnimationDone()
--	local	weapontype = actor:GetEquippedWeaponType();

	if animDone == true then
		return	MagicianFloatShot_Finished(actor,action);
	end
	
	return true
end
function MagicianFloatShot_OnCleanUp(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("MagicianFloatShot_ReallyLeave\n");

	if action:GetID() == "a_jump" then
		action:SetSlot(2)
		action:SetDoNotBroadCast(true)
		return true
	end	
	
	return true;
end
function MagicianFloatShot_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local curAction = actor:GetAction();
	
	if( CheckNil(nil==curAction) ) then return true end
	if( CheckNil(curAction:IsNil()) ) then return true end
			
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	ODS("MagicianFloatShot_OnLeave NEXTACTION : "..actionID.."\n");
	if actor:IsMyActor() == false then
		return true;
	end

	if action:GetActionType()=="EFFECT" then
		return true;
	end	
	
	local	kNextActionName = curAction:GetParam(1);
	if action:GetEnable() == false then
		
		if kNextActionName==actionID then
			curAction:SetParam(0,"");
		end
		
	end
		
	if action:GetEnable() == true then
		
		if curParam == "end" then 
			return true;
		end

		return false;
	
	elseif curParam == "end" and
		actionID == "a_run" then
		return true;
	elseif actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" or
		actionID == "a_jump" then
		return true;
	end

	return false 
end

function MagicianFloatShot_DoEffect(actor)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then	--	아직 DB 입력이 안되었으므로...
		iAttackRange = 200
	end
	
	action:SetParam(4,"HIT");
	
	actor:AttachParticle(10,"p_wp_l_hand","ef_mag_lightning");
	ODS("ef_mag_lightning\n");
	
	local	kProjectileMan = GetProjectileMan();
	local	kArrow = MagicianFloatShot_LoadArrowToWeapon(actor,action);
	

	local	targetobject = nil;

	if kArrow:IsNil() == false then
	
		local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
		
		actor:AttachParticle(421,"char_root","shot_rebound_01");
		
		
		local	kTargetList = action:GetTargetList();
		local	iTargetCount = kTargetList:size();

		local	iTargetABVShapeIndex = 0;
		local	kActionTargetInfo = nil
		local	pt = nil;
		if iTargetCount>0 then
				--	타겟을 찾아 바라본다
			kActionTargetInfo = kTargetList:GetTargetInfo(0);

			local	kTargetGUID = kActionTargetInfo:GetTargetGUID();
			iTargetABVShapeIndex = kActionTargetInfo:GetABVIndex();
			targetobject = g_pilotMan:FindPilot(kTargetGUID);
			if targetobject:IsNil() == false then
				pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
				actor:LookAt(pt);
			end						
		end
		
		kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());
		
		if targetobject == nil or targetobject:IsNil()==true or kActionTargetInfo==nil or kActionTargetInfo:IsNil() == true then
			
			kMovingDir:Multiply(iAttackRange);
			kMovingDir:Add(arrow_worldpos);
			kArrow:SetTargetLoc(kMovingDir);	
			
		else
				
			kArrow:SetTargetObjectList(kTargetList);	
			
		end
		
		kArrow:Fire();
		ODS("MagicianShot Arrow Fire\n");
		
		kTargetList:clear();
	
	end
	
	
end

function MagicianFloatShot_OnEvent(actor, textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if textKey == "fire" or textKey=="hit" then
		
		MagicianFloatShot_DoEffect(actor);

	end
end
