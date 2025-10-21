function Skill_Archer_Charge_Arrow_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
		return	false;
	end	

	return		true;
	
end
function Skill_Archer_Charge_Arrow_OnCastingCompleted(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(5,1);	
	if actor:IsMyActor() == false then
		Skill_Archer_Charge_Arrow_Fire(actor,action);
	end		
end
function Skill_Archer_Charge_Arrow_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("Skill_Archer_Charge_Arrow_OnEnter\n");
	
	action:SetParamInt(3,0);
	action:SetParamInt(1,0);
	action:SetParamInt(5,0);
				
	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
			return	false;
		end
		
	else
		if action:GetActionParam() == AP_FIRE then
			Skill_Archer_Charge_Arrow_Fire(actor,action);
		end
		
	end
		
	action:SetAutoBroadCastWhenCastingCompleted(false);	
	action:SetParamInt(6,0);
	
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
		, g_fChargeFocusFilterValue[2]
		, g_fChargeFocusFilterValue[3]
		, g_fChargeFocusFilterValue[4]
		,true
		,true)
		
		GetComboAdvisor():OnNewActionEnter(action:GetID());
		
		
	end

   return true
end
function Skill_Archer_Charge_Arrow_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	action:SetParamInt(1,1);
	
	action:SetSlot(2);
	actor:PlayCurrentSlot();
	
	actor:AttachParticle(10,"char_root","ef_arrow_start01");
	action:SetParamInt(3,0);		
	
	if actor:IsMyActor() == true then
	
	    action:CreateActionTargetList(actor);
		action:SetActionInstanceID();
		action:BroadCast(actor:GetPilot());
	
	end
end

function Skill_Archer_Charge_Arrow_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local kParam = FindTargetParam();
	kParam:SetParam_1(actor:GetTranslate(),actor:GetLookingDir());
	kParam:SetParam_2(action:GetSkillRange(0,actor),g_fChargingAttackAngle,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);
	
end

function Skill_Archer_Charge_Arrow_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	curSlot = action:GetCurrentSlot();
	local	iState = action:GetParamInt(6);
	local	bKeyCheck = KeyIsDown(action:GetActionKeyNo(0))
	
	if actor:IsMyActor() == false then
		bKeyCheck = true;
	end
	
	if iState == 0 then
	
		if animDone == true then
		
			if curSlot == 0 then
				actor:PlayNext();
				actor:AttachParticle(10,"p_ef_center","ef_arc_charge01_loop");		
				action:SetParamInt(3,1);							
			end
			if curSlot == 2 then
				action:SetParamInt(6,1);
				action:SetParamFloat(7,accumTime);
				return true;
			end
		
		end
		
		if bKeyCheck == false then
			if curSlot == 0 then
				return	false;
			end
			
			if curSlot == 1 and action:GetParamInt(5) == 1 then
			
				Skill_Archer_Charge_Arrow_Fire(actor,action);
			else
				return	false;
			
			end	
		end
	else
	
		local	fElapsedTime = accumTime - action:GetParamFloat(7);
		if fElapsedTime>=g_fChargingFreezeTime then
			action:SetParam(2,"end");
			return	false;					
		end		

	end
	

	return true
end


function	Skill_Archer_Charge_Arrow_LoadArrowToWeapon(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kProjectileMan = GetProjectileMan();
	local	kNewArrow = kProjectileMan:CreateNewProjectile("Projectile_Phoenix",action,actor:GetPilotGuid());
	kNewArrow:LoadToWeapon(actor);	--	장전
	return kNewArrow;
end
function	Skill_Archer_Charge_Arrow_FireArrow(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	ODS("Skill_Archer_Charge_Arrow_Fire\n");
	
	local	kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	
	local	iAttackRange = kAction:GetSkillRange(0,actor);
	local	kArrow = Skill_Archer_Charge_Arrow_LoadArrowToWeapon(actor,kAction);
	
	local	targetobject = nil;

	if kArrow:IsNil() == false then
	
		local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
		
		local	iTargetCount = kAction:GetTargetCount();
		local	iTargetABVShapeIndex = 0;
		local	kActionTargetInfo = nil
		local   pt = nil;
		
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
		
		if targetobject == nil or targetobject:IsNil()==true or kActionTargetInfo==nil or kActionTargetInfo:IsNil() == true then
			
			kMovingDir:Multiply(iAttackRange);
			kMovingDir:Add(arrow_worldpos);
			kArrow:SetTargetLoc(kMovingDir);	
			
		else
			kArrow:SetTargetObjectList(kAction:GetTargetList());	
			
			if pt ~= nil then
			
				local	kTargetPoint = Point3(pt:GetX(),pt:GetY(),arrow_worldpos:GetZ());
				kArrow:SetTargetLoc(kTargetPoint);
			
			end
		end
		
		kArrow:Fire();
		
	else
		
		ODS("Arrow is nil\n");
	
	end
	
	kAction:ClearTargetList();
	actor:ClearIgnoreEffectList();
				
end


function Skill_Archer_Charge_Arrow_OnCleanUp(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

	if actor:GetAction():GetParamInt(3) == 1 then
		actor:DetachFrom(10);
	end
	
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
		, g_world:GetWorldFocusFilterAlpha()
		, 0
		, g_fChargeFilterReturnTime
		,false
		,true);
	end
	
	actor:EndWeaponTrail();
	return true;
end

function Skill_Archer_Charge_Arrow_OnLeave(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetEnable() == false then
		return	false;
	end
	
	if action:GetID() == "a_jump" then
		return	false;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	
		
	local	curAction = actor:GetAction();
	if curAction:GetParamInt(1) == 1 and curAction:GetParam(2)~="end" then
		return		false;
	end


   return true;
   
end

function Skill_Archer_Charge_Arrow_OnEvent(actor,textKey,seqID)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local	action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if textKey == "hit" and action:GetActionParam() == AP_FIRE then
	
		Skill_Archer_Charge_Arrow_FireArrow(actor);
	
	end

	return	true;
end
