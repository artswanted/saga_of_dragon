-- [PROJECTILE_ID] : 발사할 발사체 ID, "" 일 경우 무기에 붙어있는 발사체 ID 를 사용한다.
-- [ATTACH_NODE_ID] : 발사체를 붙일 액터의 노드 ID, "" 일 경우 캐릭터 좌표에서 발사된다.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [NO_AUTO_CONNECT] : 다음 액션으로 부드럽게 연결해줄 것인가

-- Melee
function MagicMissle_OnCheckCanEnter(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function MagicMissle_OnCastingCompleted(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	MagicMissle_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function MagicMissle_Fire(actor,action)
	
end
function MagicMissle_OnFindTarget(actor,action,kTargets)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
    local   iArrowCount = action:GetAbil(AT_LEVEL) * 2;
    if iArrowCount == 0 then
        return  0
    end

	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);
	
	action:SetParam(0,"");
	local kParam = FindTargetParam();
	
    kParam:SetParam_1(kPos,actor:GetLookingDir());
    kParam:SetParam_2(action:GetSkillRange(0,actor),30,5);
    kParam:SetParam_3(true,FTO_NORMAL);
    
    local	kTempTargets = CreateActionTargetList(actor:GetPilotGuid(),action:GetActionInstanceID(),action:GetActionNo());
    local iFound = action:FindTargets(TAT_RAY_DIR,kParam,kTempTargets,kTempTargets);		
    
    if iFound == 0 then
    
        DeleteActionTargetList(kTempTargets);
        return  0
    end
    
    
    local   iArrowNum = 0;
    local   kTargetInfo = nil;
    local   iTargetNum=0;
    while iArrowNum<iArrowCount do

        kTargetInfo = kTempTargets:GetTargetInfo(iTargetNum);
        kTargets:AddTarget(kTargetInfo);
    
        iArrowNum=iArrowNum+1;
        iTargetNum = iTargetNum+1
        if iTargetNum>=iFound then
            iTargetNum = 0;
        end
    end
    
    
    DeleteActionTargetList(kTempTargets);
    
    return  kTargets:size();
	
end

function MagicMissle_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	actor:ResetAnimation();

	MagicMissle_OnCastingCompleted(actor,action);

	action:SetParam(10,"");
	
	return true
end

function MagicMissle_OnUpdate(actor, accumTime, frameTime)
	
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

function MagicMissle_OnCleanUp(actor)
    return true;
end

function MagicMissle_OnLeave(actor, action)

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
		--if( false == actor:PlayNext()) then 
			actor:GetAction():SetNextActionName("a_battle_idle");
		--end
		actor:GetAction():ChangeToNextActionOnNextUpdate(true,true);
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

function MagicMissle_OnEvent(actor,textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local kAction = actor:GetAction();
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	ODS("MagicMissle_OnEvent textKey:"..textKey.."\n");
	
	if textKey == "hit" or textKey == "fire" then

		if kAction:GetParam(10) == "hit" then
			return;
		end

		actor:ClearIgnoreEffectList();
		kAction:CreateActionTargetList(actor, true);
        
		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end
		
		local   iTotalArrow = kAction:GetAbil(AT_LEVEL) * 2;
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
--		local   iTargetNum = 0;
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
--		local   kLookDir = actor:GetLookingDir();
		local   fFrontDist = 30;
		local   fCircleRadius = 20;
		local   kProjectileCreationCenter = actor:GetLookingDir();
		kProjectileCreationCenter:Multiply(fFrontDist);
		kProjectileCreationCenter:Add(kCharPos);
		
		local   iArrowCount = 0
		while   iArrowCount<iTotalArrow do
		
		    kTargetInfo = kTargetList:GetTargetInfo(0);
		    
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
		        kTargetList:DeleteTargetInfo(0);
		    end
		    
		    kNewArrow:DelayFire(iArrowCount*0.05);

			ODS("발사수 : ".. iArrowCount+1 .."\n")
    
		    iArrowCount=iArrowCount+1;
		    
		end

		kTargetList:clear();

		kAction:SetParam(10,"hit");
	end

	return	true;
end
