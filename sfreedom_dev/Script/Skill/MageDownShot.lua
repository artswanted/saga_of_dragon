function Mage_Down_Shot_OnCastingCompleted(actor,action)
end

function Mage_Down_Shot_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local kCurAction = actor:GetAction()
	if( false == kCurAction:IsNil() ) then
		local weapontype = actor:GetEquippedWeaponType();
		if weapontype == 0 then
			kCurAction:SetNextActionName("a_idle")
			return false
		end
	end
	
	return true
	
end
function Mage_Down_Shot_FindTarget(kActor, kAction)
	
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());	
	
	local	kTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo());
	local	iFound = Mage_Down_Shot_OnFindTarget(kActor,kAction,kTargets);

	return	kTargets;
end
function Mage_Down_Shot_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local iAttackRange = action:GetSkillRange(0,actor)

	local	kPos = actor:GetPos();
	local	kMovingDir = actor:GetLookingDir();
	kMovingDir:Multiply(iAttackRange);
	kPos:Add(kMovingDir);
	kPos:SetZ(kPos:GetZ());		
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local	kPos2 = g_world:ThrowRay(kPos,Point3(0,0,-1),500);	--	바닥으로 레이를 쏘자
	if kPos2:GetX() ~= -1 then

		local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
		if 0==iParam2 then
			iParam2 = g_fMage_JumpDown_Meteor_Blow_Range
		end

		local kParam = FindTargetParam();
		kParam:SetParam_1(kPos2,actor:GetLookingDir());
		kParam:SetParam_2(0,30,iParam2,0);	
		kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN+FTO_BLOWUP);
		
		return	action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets)
	
	end
	
	return	0;
end

function Mage_Down_Shot_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local	actorID = actor:GetID()
--	local	actionID = action:GetID()
--	local	weapontype = actor:GetEquippedWeaponType();

	--ODS("Mage_Down_Shot_OnEnter weapontype : "..weapontype.."\n");

	if actor:IsMyActor() == true then
	
		if actor:GetFloatHeight(40)<50 then
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;	
		end
		
		if actor:GetParam("JUMP_DOWN_ATTACK_ENABLE") ~= nil and
		    actor:GetParam("JUMP_DOWN_ATTACK_ENABLE") == "FALSE" then
		    
			action:ChangeToNextActionOnNextUpdate(true,true);
			
			return	true;		
			    
		end
		
		actor:SetParam("JUMP_DOWN_ATTACK_ENABLE","FALSE");
		
			
		action:SetParam(3,"");

	end

	Mage_Down_Shot_Fire(actor,action);
	actor:FreeMove(true);
	actor:StopJump();
	
	action:SetParam(3,-1);
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());	
	end		
	
	return true
end
function Mage_Down_Shot_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(5,1)	
	if actor:IsMyActor() == true then
		local kTargets = Mage_Down_Shot_FindTarget(actor,action);
		action:SetTargetList(kTargets);
		DeleteActionTargetList(kTargets);	
	end
	
	local	iTargetCount = action:GetTargetCount();
	--ODS("Mage_Down_Shot_Fire iTargetCount : "..iTargetCount.."\n");

	local dis = 0
	local lookNum = 0

	for i = 0, iTargetCount - 1 do
		local	kTargetGUID = action:GetTargetGUID(i);
		local	targetobject = g_pilotMan:FindPilot(kTargetGUID);
		local	iTargetABVShapeIndex = action:GetTargetABVShapeIndex(i);
		if targetobject:IsNil() == false then
			local pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
			pt:Subtract(actor:GetTranslate())
			if dis < pt:Length() then
				dis = pt:Length()
				lookNum = i
			end
		end
	end
	
	if iTargetCount>0 then
			--	타겟을 찾아 바라본다
		local	kTargetGUID = action:GetTargetGUID(lookNum);
		local	targetobject = g_pilotMan:FindPilot(kTargetGUID);
		local	iTargetABVShapeIndex = action:GetTargetABVShapeIndex(lookNum);
		if targetobject:IsNil() == false then
			local pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
			actor:LookAt(pt);
		end
	end

	GetActionResultCS(action, actor)
	return	true;
end

function Mage_Down_Shot_LoadArrowToWeapon(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = actor:GetEquippedWeaponProjectileID();
	local	kAttackNodeName = action:GetScriptParam("ATTACH_NODE");	
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	end
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());

	local	iAttackRange = action:GetSkillRange(0,actor);
	local	kPos = actor:GetPos();
	local	kMovingDir = actor:GetLookingDir();
	local	iRandom = math.random(-100,100);
	if iRandom == 0 then
		iRandom = 1;
	end

	kMovingDir:Multiply(iAttackRange+iRandom);
	kPos:Add(kMovingDir);
	kPos:SetZ(kPos:GetZ()+100);
	
	kNewArrow:SetWorldPos(kPos);
	
	return kNewArrow;
end

function Mage_Down_Shot_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local	actorID = actor:GetID()
	local	action = actor:GetAction()

	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local	animDone = actor:IsAnimationDone()
	local	nextActionName = action:GetNextActionName()
--	local	iMaxActionSlot = action:GetSlotCount();
--	local	weapontype = actor:GetEquippedWeaponType();
--	local	actionID = action:GetID();

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
function Mage_Down_Shot_OnCleanUp(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:FreeMove(false);

	if action:GetID() == "a_jump" then
		action:SetSlot(2)
		action:SetDoNotBroadCast(true)
	end
	
	return true;
end
function Mage_Down_Shot_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	--ODS("Mage_Down_Shot_OnLeave NEXTACTION : "..actionID.."\n");
	if actor:IsMyActor() == false then
		return true;
	end
	
	ODS("123\n")
	if curParam == "end" and actionID == "a_jump" then 
		--action:SetSlot(2)
		--action:SetDoNotBroadCast(true)
		return  true;
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
	if curParam == "end" then 
		return true;
	end		

	if action:GetEnable() == true then

		curAction:SetParam(0,"GoNext");
		curAction:SetParam(1,actionID);

		return false;
	
	elseif curParam == "end" and
		actionID == "a_run" then
		return true;
	elseif actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Mage_Down_Shot_DoEffect(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local	action = actor:GetAction()
			
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local	iAttackRange = action:GetSkillRange(0,actor);
	
	ODS("Mage_Down_Shot_DoEffect iAttackRange:"..iAttackRange.."\n");
	
	action:SetParam(4,"HIT");
	
	local	kProjectileMan = GetProjectileMan();
	
	for i = 1, 3 do
		local	kArrow = Mage_Down_Shot_LoadArrowToWeapon(actor,action);
		
		local	targetobject = nil;

		if kArrow:IsNil() == false then
		
			kArrow:SetParamValue("CNT", tostring(i))
			
			if i == 2 then
				kArrow:SetParamValue("NON_TARGET", "FALSE")
			else
				kArrow:SetParamValue("NON_TARGET", "TRUE")
			end
			
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			local	kPos = actor:GetPos();
			local	kMovingDir = actor:GetLookingDir();
			local	kGab = iAttackRange / 2;
			kMovingDir:Multiply(iAttackRange + ((i - 2) * kGab) + math.random(-10,10));
			kPos:Add(kMovingDir);
			kPos:SetZ(kPos:GetZ()-20);		
			
			local	kPos2 = g_world:ThrowRay(kPos,Point3(0,0,-1),500);	--	바닥으로 레이를 쏘자
			
			ODS("___________TargetLoc : " .. kPos2:GetX() .. " , " .. kPos2:GetY() .. ", " .. kPos2:GetZ() .. "\n")

			
			local	iTargetCount = action:GetTargetCount();
			
			actor:LookAt(kPos);
		
			kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());
			
			if iTargetCount>0 then
				kArrow:SetTargetObjectList(action:GetTargetList());	
			end

			if kPos2:GetX() == -1 and 
				kPos2:GetY() == -1 and
				kPos2:GetZ() == -1 then
				kPos2 = g_world:ThrowRay(actor:GetPos(), Point3(0,0,-1), 500)
			end

			kArrow:SetTargetLoc(kPos2);		
			kArrow:Fire();
		
		end
	end
	action:ClearTargetList();
end

function Mage_Down_Shot_OnEvent(actor, textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	--ODS("Mage_Down_Shot_OnEvent key : "..textKey.."\n");
	if (textKey == "fire" or textKey=="hit") and actor:GetAction():GetParam(4)~="HIT" then
		
		Mage_Down_Shot_DoEffect(actor);

	end
end
