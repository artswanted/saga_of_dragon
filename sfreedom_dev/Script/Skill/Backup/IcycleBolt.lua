-- [PROJECTILE_ID] : 발사할 발사체 ID, "" 일 경우 무기에 붙어있는 발사체 ID 를 사용한다.
-- [ATTACH_NODE_ID] : 발사체를 붙일 액터의 노드 ID, "" 일 경우 캐릭터 좌표에서 발사된다.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [NO_AUTO_CONNECT] : 다음 액션으로 부드럽게 연결해줄 것인가

-- Melee
function IcycleBolt_OnCheckCanEnter(actor,action)

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	if actor:IsMeetFloor() == false then
			return	false;
	end

	return		true;
	
end
function IcycleBolt_OnCastingCompleted(actor, action)

	IcycleBolt_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function	IcycleBolt_LoadToWeapon(actor,action)
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

function IcycleBolt_Fire(actor,action)
	
	if actor:IsMyActor() == true then

		
		local	kTargets = CreateActionTargetList(actor:GetPilotGuid(),action:GetActionInstanceID(),action:GetActionNo());
		local	iFound = IcycleBolt_OnFindTarget(actor,action,kTargets);
		if iFound > 0 then
			-- 타겟 잡고, 액션 결과 산출해내기
			local i =0 ;
			
			while i<iFound do
			
				local	kTargetInfo = kTargets:GetTargetInfo(i);
				local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());		
				if kTargetPilot:IsNil() == false then
				
					local	kTargetActor = kTargetPilot:GetActor();
				
					local kUnitArray = NewUNIT_PTR_ARRAY()
					kUnitArray:AddUnit(kTargetPilot:GetUnit())
					local kActionResult = NewActionResultVector()
					
					action:Call_Skill_Result_Func(actor:GetPilot(), kUnitArray, kActionResult);
					local kAResult = kActionResult:GetResult(kTargetInfo:GetTargetGUID(), false)
					if kAResult:IsNil() == false and kAResult:GetInvalid() == false then
						action:AddTarget(kTargetActor:GetPilotGuid(),kTargetActor:GetHitObjectABVIndex(), kAResult)
					end
					DeleteActionResultVector(kActionResult)
					DeleteUNIT_PTR_ARRAY(kUnitArray)
				end
				i=i+1;
			end
		end
		DeleteActionTargetList(kTargets);
		
	else
	    GetActionResultCS(action, actor)
	end
	
end
function IcycleBolt_OnFindTarget(actor,action,kTargets)

    local   iArrowCount = 5;

	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);
	
	action:SetParam(0,"");
	kParam = FindTargetParam();
	
    kParam:SetParam_1(kPos,actor:GetLookingDir());
    kParam:SetParam_2(action:GetSkillRange(102100901,actor),30,0,1);
    kParam:SetParam_3(true,FTO_NORMAL);
    
    local iFound = action:FindTargetsEx(102100901,TAT_RAY_DIR,kParam,kTargets,kTargets);		

    return  kTargets:size();
	
end

function IcycleBolt_OnEnter(actor, action)
	local actorID = actor:GetID()
	local actionID = action:GetID()
	local weapontype = actor:GetEquippedWeaponType();
	
	
	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then	
			
			-- 공중에서 공격을 했다면 종료
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;	
		end
	end
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	actor:ResetAnimation();

	IcycleBolt_OnCastingCompleted(actor,action);
	
	return true
end

function IcycleBolt_OnUpdate(actor, accumTime, frameTime)

	local actorID = actor:GetID()
	local action = actor:GetAction()
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
	local actionID = action:GetID();
	
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

function IcycleBolt_OnCleanUp(actor)
    return true;
end

function IcycleBolt_OnLeave(actor, action)
	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	if actor:GetAction():IsChangeToNextActionOnNextUpdate() then
		return	true;
	end	
	
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

function IcycleBolt_OnEvent(actor,textKey)

	local kAction = actor:GetAction();
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	ODS("IcycleBolt_OnEvent textKey:"..textKey.."\n");
	
	if textKey == "hit" or textKey == "fire" then

		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end
		
		local   iTotalArrow = 5;
		if iTotalArrow == 0 then
		    return
		end
		
		local	kProjectileMan = GetProjectileMan();
	    local	kProjectileID = kAction:GetScriptParam("PROJECTILE_ID");
    	
	    if kProjectileID == "" then
		    kProjectileID = actor:GetEquippedWeaponProjectileID();
	    end		
		
		local	iAttackRange = kAction:GetSkillRange(0,actor);
		
		if iAttackRange == 0 then
			iAttackRange = 200; 
		end
		
		local   kTargetList = kAction:GetTargetList();
		local	iTargetCount = kTargetList:size();
		local   iTargetNum = 0;
		local   kTargetInfo = nil;
		
		if iTargetCount>0 then
				--	타겟을 찾아 바라본다
			local	kTargetGUID = kAction:GetTargetGUID(0);
			local   iTargetABVShapeIndex = kAction:GetTargetABVShapeIndex(0);
			local   targetobject = g_pilotMan:FindPilot(kTargetGUID);
			if targetobject:IsNil() == false then
				local   pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
				actor:LookAt(pt);
			end						
		end		
		
		local   kCharPos = actor:GetPos();
		local   kLookDir = actor:GetLookingDir();
		local   fFrontDist = 30;
		local   fCircleRadius = 20;
		local   kProjectileCreationCenter = actor:GetLookingDir();
		kProjectileCreationCenter:Multiply(fFrontDist);
		kProjectileCreationCenter:Add(kCharPos);
		
		kTargetInfo = kTargetList:GetTargetInfo(0);
		
		local   iArrowCount = 0
		while   iArrowCount<iTotalArrow do
		    
		    --  프로젝틸 생성
		    local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,kAction,actor:GetPilotGuid());
		    
		    if iTotalArrow == 1 then
		    
		        --  총 1발만 발사할때는 그냥 캐릭터 앞에 생성한다.
		        kNewArrow:LoadToPosition(kProjectileCreationCenter);
		        
		    else
		    
		        --  2발 이상일 경우는 캐릭터 앞에 원을 그리며 생성된다.
		        local   fAngleDist = 360.0 / iTotalArrow;
		        local   kRotAxis = actor:GetLookingDir();
		        local   kVec = Point3(0,0,fCircleRadius)
		        kVec:Rotate(kRotAxis,fAngleDist*iArrowCount*3.141492/180.0);
		        kVec:Add(kProjectileCreationCenter);
		        
		        kNewArrow:LoadToPosition(kVec);
		    
		    end
		    
		    kNewArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
		    local	arrow_worldpos = kNewArrow:GetWorldPos();	--	화살의 위치
		    
		    if kTargetInfo:IsNil() or iTargetCount == 0 then
		    
		        local   kTargetPos = actor:GetLookingDir();
				kTargetPos:Multiply(iAttackRange);
				kTargetPos:Add(arrow_worldpos);
				kNewArrow:SetTargetLoc(kTargetPos);		    
		    
		    else
		        kNewArrow:SetTargetObject(kTargetInfo);
		        
		    end
		    
		    kNewArrow:DelayFire(iArrowCount*0.05);
		    
		    iArrowCount=iArrowCount+1;
		    
		end
		
		kTargetList:DeleteTargetInfo(0);
	end

	return	true;
end
