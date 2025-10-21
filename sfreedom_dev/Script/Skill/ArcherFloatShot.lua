
function ArcherFloatShot_OnCastingCompleted(actor, action)
end

function ArcherFloatShot_OnCheckCanEnter(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	
	if actor:CheckStatusEffectExist("se_transform_to_metamorphosis") 
	or actor:CheckStatusEffectExist("se_transform_to_Dancer")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Fig")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Mag")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Arc")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Thi")then
		return false;
	end
--	local	iFloatShotCount = tonumber(actor:GetParam("FLOAT_SHOT_COUNT"));
--	if iFloatShotCount == nil then
--		 iFloatShotCount = 0
--	end
--	
--	if iFloatShotCount>7 then
--		return	false
--	end
	
	--if actor:GetParam("FLOAT_ATTACK_ENABLE")=="FALSE" then
	--	return false;
	--end

		
	return		true;
	
end
function ArcherFloatShot_FindTarget(kActor,kAction)
	
	if( CheckNil(nil==kActor) ) then return false end
	if( CheckNil(kActor:IsNil()) ) then return false end
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	local	kTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo());
	local	iFound = ArcherFloatShot_OnFindTarget(kActor,kAction,kTargets);
	
	return	kTargets;
end
function ArcherFloatShot_OnFindTarget(actor,action,kTargets)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end	

	local	iAttackRange = action:GetSkillRange(0,actor)
	
	return	ArcherFloatShot_OnFindTargetEx(actor,action,action:GetActionNo(),iAttackRange,kTargets);
end

function ArcherFloatShot_OnFindTargetEx(actor,action,iActionID,iAttackRange,kTargets)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	arrow_worldpos = actor:GetPos();
	
-- 회전축 구하기
	local	kRotateAxis = actor:GetLookingDir();
	kRotateAxis:Cross(Point3(0,0,1));
	kRotateAxis:Unitize();
	local	fRotateAngle = 60.0*3.141592/180.0;

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 30
	end

	local kParam = FindTargetParam();
	kParam:SetParam_1(actor:GetTranslate(),actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,iParam2,0,0);
	kParam:SetParam_3(true,FTO_BLOWUP);		

	local	kFireTargetDir = actor:GetLookingDir();
	kFireTargetDir:Rotate(kRotateAxis,fRotateAngle);
	kParam:SetParam_1(arrow_worldpos,kFireTargetDir);
	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);
	
	return	kTargets:size();
	
end


function ArcherFloatShot_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local	actorID = actor:GetID()
	local	actionID = action:GetID()
--	local	weapontype = actor:GetEquippedWeaponType();
	
	ODS("ArcherFloatShot_OnEnter actionID:"..actionID.."\n");
		
		
	if actor:IsMyActor() == true then
		action:SetParam(4, "");
		
		if action:GetID() ~= "a_ArcherFloatShot_01" and
		    action:GetID() ~= "a_ArcherFloatShot_02" and
		    action:GetID() ~= "a_ArcherFloatShot_03" then
		    
			actor:SetComboCount(0);	
			action:ChangeToNextActionOnNextUpdate(true,true);					
			return true;		    
		    
		end
		
--		local	iFloatShotCount = tonumber(actor:GetParam("FLOAT_SHOT_COUNT"));
--		if iFloatShotCount == nil then
--			 iFloatShotCount = 0
--		end
		
--		if iFloatShotCount>7 then
--			action:ChangeToNextActionOnNextUpdate(true,true);					
--			return true;
--		end		
		
--		iFloatShotCount=iFloatShotCount+1;
--		actor:SetParam("FLOAT_SHOT_COUNT",""..iFloatShotCount);
		
	end
			
	ArcherFloatShot_Fire(actor,action);
	
	if action:GetTargetCount() == 0 then
	
	    --  때릴수 있는 녀석이 1마리도 없다면 JumpShot 으로 전환한다.
--	    actor:SetParam("Archer_Jump_Shot_Fired","FALSE");
  --      action:SetNextActionName("a_archer_jump_shot");
    --    action:ChangeToNextActionOnNextUpdate(true,true);	    
      --  return true;
	
	end
	
	actor:ResetAnimation();
			
	action:SetParamInt(6,0);
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
	end	
	
	return true
end
function ArcherFloatShot_Fire(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iTargetCount = 0;

	if actor:IsMyActor() == true then
	
		local kTargets = ArcherFloatShot_FindTarget(actor,action);
		action:SetTargetList(kTargets);
		DeleteActionTargetList(kTargets);
		
		iTargetCount = kTargets:size();
		
	end
	
	iTargetCount = action:GetTargetCount();
	ODS("ArcherFloatShot_Fire iTargetCount : "..iTargetCount.."\n");	
	
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

function	ArcherFloatShot_LoadToWeapon(actor,action)

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

function ArcherFloatShot_Finished(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local	actionID = action:GetID();
--	local iComboCount = actor:GetComboCount();
	local nextActionName = action:GetNextActionName()
	
	actor:SetNormalAttackEndTime();
			
	if actor:IsMyActor() then
	
		if KeyIsDown(KEY_CHARGE) == true then
			actor:ReserveTransitAction(action:GetID(),DIR_NONE);
			action:SetParam(0,"null");
			action:SetParam(1, "end")
			return	false;
		end
	end
				
	local curActionParam = action:GetParam(0)
	if curActionParam == "GoNext" then
		nextActionName = action:GetParam(1);
		action:SetNextActionName(nextActionName)
		
		if nextActionName ~= "a_archer_shot_01" then
			actor:SetComboCount(0)
			ODS("actor:SetComboCount(0) 0\n");
			actor:SetParam("FLOAT_ATTACK_ENABLE","FALSE");
		end
	end
	
	if action:GetParam(2) ~= "ATTACKED" then
		actor:SetComboCount(0)
		actor:SetParam("FLOAT_ATTACK_ENABLE","FALSE");
		
		ODS("actor:SetComboCount(0) 1\n");
	end
	
	
	action:SetParam(0, "null")
	action:SetParam(1, "end")
end

function ArcherFloatShot_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

--	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	animDone = actor:IsAnimationDone()
--	local	weapontype = actor:GetEquippedWeaponType();

	if animDone == true then
		return	ArcherFloatShot_Finished(actor,action);
	end
	
	return true
end
function ArcherFloatShot_OnCleanUp(actor,action)
	return true;
end
function ArcherFloatShot_OnLeave(actor, action)
	
	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
		
	local curAction = actor:GetAction();
	
	if( CheckNil(nil==curAction) ) then return true end
	if( CheckNil(curAction:IsNil()) ) then return true end
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	ODS("ArcherFloatShot_OnLeave NEXTACTION : "..actionID.."\n");
	if actor:IsMyActor() == false then
		return true;
	end
	if  actionID == "a_jump" then 
		return  false;
	end
	if action:GetActionType()=="EFFECT" then
		return true;
	end	
	
	if curParam == "end" then 
		return true;
	end	
	
	if actionID == "a_back_step" then
		return	true
	end
	
	local	kNextActionName = curAction:GetParam(1);
	if action:GetEnable() == false then
		
		if kNextActionName==actionID then
			curAction:SetParam(0,"");
		end
		
	end
		
	if action:GetEnable() == true then
	
		curAction:SetParam(0,"GoNext");
		curAction:SetParam(1,actionID);
		
		return false;
	
	elseif curParam == "end" and
		(actionID == "a_run_right" or
		actionID == "a_run_left") then
		return true;
	elseif actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function ArcherFloatShot_DoEffect(actor)
	
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
	
	
--	local	kProjectileMan = GetProjectileMan();
	local	kArrow = ArcherFloatShot_LoadToWeapon(actor,action);
	

	local	targetobject = nil;

	if kArrow:IsNil() == false then

		local iPentrationCount = actor:GetAbil(AT_PENETRATION_COUNT);
		kArrow:SetPenetrationCount(iPentrationCount);
		if iPentrationCount ~= 0 then
			kArrow:SetMultipleAttack(true);
		end

			
--		local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
		
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
				actor:LookAt(pt,true);
			end						
		end
		
		kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());
		
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
				
			kArrow:SetTargetObjectList(kTargetList);	
			
			if pt~= nil then
			
				local	fVariationAmount = 10.0
				
				pt:Add(Point3(math.random(-fVariationAmount,fVariationAmount),
								math.random(-fVariationAmount,fVariationAmount),
								math.random(-fVariationAmount,fVariationAmount)));
				kArrow:SetTargetLoc(pt);
				
			
			
			end
		
		end
		
		kArrow:Fire();
		ODS("MagicianShot Arrow Fire\n");
		
		kTargetList:clear();
		actor:ClearIgnoreEffectList();
	
	end
	
	
end

function ArcherFloatShot_OnEvent(actor, textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	ODS("ArcherFloatShot_OnEvent textKey:"..textKey.."\n");
	
	if textKey == "fire" or textKey=="hit" then
		
		
		
		ArcherFloatShot_DoEffect(actor);

	end
end
