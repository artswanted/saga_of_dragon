-- [PROJECTILE_ID] : 발사할 발사체 ID, "" 일 경우 무기에 붙어있는 발사체 ID 를 사용한다.
-- [ATTACH_NODE_ID] : 발사체를 붙일 액터의 노드 ID, "" 일 경우 캐릭터 좌표에서 발사된다.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [NO_AUTO_CONNECT] : 다음 액션으로 부드럽게 연결해줄 것인가

-- Melee
function Archer_Power_Shot_OnCheckCanEnter(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
    if actor:IsMyActor() == false then
        return true
    end
    
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	if actor:IsMeetFloor() == false then
	    actor:ReserveTransitAction("a_archer_shot_01");
		return	false;
	end

	return		true;
	
end
function Archer_Power_Shot_OnCastingCompleted(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	Archer_Power_Shot_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function	Archer_Power_Shot_LoadToWeapon(actor,action)

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

function Archer_Power_Shot_Fire(actor,action)
		
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if actor:IsMyActor() == true then
        action:CreateActionTargetList(actor);
	end
	
end
function Archer_Power_Shot_OnFindTarget(actor,action,kTargets)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);
	
	action:SetParam(0,"");
	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(action:GetSkillRange(0,actor),30,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);		
end

function Archer_Power_Shot_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	
	ODS("Archer_Power_Shot_OnEnter ComboCount : "..actor:GetComboCount().."\n");
	
	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then	
			
			-- 공중에서 공격을 했다면 종료
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;	
		end
		
		if actor:GetComboCount()<2 then
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;	
		end		
	end
	
	actor:SetComboCount(0);
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	actor:ResetAnimation();

	Archer_Power_Shot_OnCastingCompleted(actor,action);
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
	end
	
	return true
end

function Archer_Power_Shot_OnUpdate(actor, accumTime, frameTime)
	
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

function Archer_Power_Shot_OnCleanUp(actor,action)
	return true;
end

function Archer_Power_Shot_OnLeave(actor, action)

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

function Archer_Power_Shot_OnEvent(actor,textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local kAction = actor:GetAction();
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	ODS("Archer_Power_Shot_OnEvent textKey:"..textKey.."\n");
	
	if textKey == "hit" or textKey == "fire" then

		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end
		
		local	iAttackRange = kAction:GetSkillRange(0,actor);
		
		if iAttackRange == 0 then
			iAttackRange = 200; 
		end
		
--		local	kProjectileMan = GetProjectileMan();
		local	kArrow = Archer_Power_Shot_LoadToWeapon(actor,kAction);
		
		local	targetobject = nil;
		local pt = nil;

		if kArrow:IsNil() == false then
		
			local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			
			local	iTargetCount = kAction:GetTargetCount();
			local	iTargetABVShapeIndex = 0;
			local	kActionTargetInfo = nil
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

	return	true;
end
