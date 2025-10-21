
function Skill_Multi_Strape_Fire_OnCheckCanEnter(kActor,kAction)
	
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	local weapontype = kActor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	
	-- 안전거품 공격 막기
	if g_pilotMan:IsLockMyInput() == true then
		return
	end	
			
	return		true;
	
end
function Skill_Multi_Strape_Fire_FindTarget(kActor,kAction)
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
end

function Skill_Multi_Strape_Fire_OnFindTarget(kActor,kAction,kTargets)

	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
end

function Skill_Multi_Strape_Fire_OnCastingCompleted(kActor,kAction)
end

function Skill_Multi_Strape_Fire_OnEnter(kActor, kAction)

	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
		
	ODS("Skill_Multi_Strape_Fire_OnEnter\n", false, 912);
	kActor:FreeMove(false)
	
	Skill_Multi_Strape_Fire_OnCastingCompleted(kActor, kAction)
	
	if kAction:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end
	kAction:SetParamInt(99, 0)						--발사 방식 0~2 
	kAction:SetParamInt(100,0)						--현재 발사 횟수
	kAction:SetParamInt(101,15)						--최대 발사 횟수
	kActor:PlayCurrentSlot()
---------------------------------------------------------------------------------
--[[
	kAction:ClearTargetList()
	kAction:CreateActionTargetList(kActor,false);
	local kTargetList = kAction:GetTargetList();
	if(true == kTargetList:IsNil()) then return true end
	local iTargetCount = kTargetList:size();

	local STD_INDEX = 1000
	kActor:SetParam(STD_INDEX, tostring(iTargetCount))
	
	local i = 0
	while i<iTargetCount do
		local   kTargetInfo = kTargetList:GetTargetInfo(i);
		local   kTargetGUID = kTargetInfo:GetTargetGUID();
		local iCurIndex = (STD_INDEX+1)+i
		kActor:SetParam(iCurIndex, kTargetGUID:GetString())
		
		ODS("MultiStrape_Fire : Index:"..(iCurIndex).." 타겟 GUID:"..kTargetGUID:GetString().."\n", false, 912)
		local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
--		if kTargetPilot:IsNil() == false then
			local actorTarget = kTargetPilot:GetActor();
		--	if false == actorTarget:IsNil() then
				local kTargetPos = actorTarget:GetPos();
				ODS("Index"..iCurIndex.." X:"..kTargetPos:GetX().." Y:"..kTargetPos:GetY().." Z:"..kTargetPos:GetZ().."\n", false, 912)
			--	kNextAction:SetParamAsPoint(1000+i, kTargetPos)
			--end
	--	end
		i = i + 1;
	end
	-- 타겟 인덱스 초기화
	if(g_TestValue == false) then 
		kActor:SetParam("MULTISTRAPE_TARGET_INDEX", tostring(STD_INDEX))
		g_TestValue = true
	end
]]
--------------------------------------------------------------------
	local animDone = kActor:IsAnimationDone()
	local nextActionName = kAction:GetNextActionName()
		
	
	if kAction:GetParam(1) == "end" then
		return	false;
	end
	
	if animDone == true then		
		local curActionParam = kAction:GetParam(0)
		if curActionParam == "GoNext" then
			nextActionName = kAction:GetParam(1);

			kAction:SetParam(0, "null")
			kAction:SetParam(1, "end")			
			kAction:SetNextActionName(nextActionName)
			return false;
		else
			kAction:SetParam(1, "end")
			return false
		end
	end

	return true
end

function	Skill_Multi_Strape_Fire_LoadArrowToWeapon(kActor,kAction)

	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	local	kProjectileMan = GetProjectileMan();
	local	kNewArrow = kProjectileMan:CreateNewProjectile("PROJECTILE_MULTISTRAPE_ARROW",kAction,kActor:GetPilotGuid());
	
	-- 발사체가 타겟을 잡지 못하더라도 삭제되지 않게함 (XML로 옮김)
	-- local kProjectileOption = kNewArrow:GetOptionInfo()
	-- if(false == kProjectileOption:IsNil()) then
		-- kProjectileOption:SetAutoRemoveIfNoneTarget(false)
		-- kNewArrow:SetOptionInfo(kProjectileOption)
	-- end
	kNewArrow:LoadToWeapon(kActor) -- 무기 위치에 화살을 생성함
	return	kNewArrow;	
end

function Skill_Multi_Strape_Fire_Finished(kActor,kAction)
	
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	ODS("Skill_Multi_Strape_Fire_Finished\n", false, 912);
		
	local	nextActionName = kAction:GetNextActionName()
--	local	iMaxActionSlot = kAction:GetSlotCount();
--	local	actionID = kAction:GetID();

	kActor:SetNormalAttackEndTime();

	local	curActionParam = kAction:GetParam(0)
	if curActionParam == "GoNext" then
		nextActionName = kAction:GetParam(1);

		kAction:SetParam(0, "null")
		kAction:SetParam(1, "end")
		
		kAction:SetNextActionName(nextActionName)
		return false;
	else

		kAction:SetParam(1, "end")

		return false
	end
	
	return	true;
end

function Skill_Multi_Strape_Fire_OnUpdate(kActor, accumTime, frameTime)
	
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());

	local	kAction = kActor:GetAction()
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	kActor:LockBidirection(false);
	kActor:SeeFront(true);
	kActor:FreeMove(false);
	
	local	animDone = kActor:IsAnimationDone()
	local	weapontype = kActor:GetEquippedWeaponType();

	local iMaxFireTime = kAction:GetParamInt(101)				--최대 발사 횟수
	if(iMaxFireTime == kAction:GetParamInt(100)) then
--		kAction:SetParamInt(7,1)	-- 더이상 발사 못하게 수정
		kAction:SetParam(0, "null")
		kAction:SetParam(1, "end")
		return	Skill_Multi_Strape_Fire_Finished(kActor,kAction);
	end

	--if animDone == true then
	if kAction:GetParamInt(7) == 1 then	
--		kActor:PlayCurrentSlot();
	--	kAction:SetParamInt(100, kAction:GetParamInt(100)+1)
--		kAction:SetParamInt(7,0)
	end

	return true
end

function Skill_Multi_Strape_Fire_OnCleanUp(kActor,kAction)
	
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	ODS("Skill_Multi_Strape_Fire_ReallyLeave nextaction : "..kAction:GetID().."\n", false, 912);
	
	kActor:FreeMove(false);
	kActor:EndWeaponTrail();
	return true
end

function Skill_Multi_Strape_Fire_OnLeave(kActor, kNextAction)

	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kNextAction);
	CheckNil(kNextAction:IsNil());
	
	local kCurAction = kActor:GetAction();
	
	CheckNil(nil==kCurAction);
	CheckNil(kCurAction:IsNil());
	
	local curParam = kCurAction:GetParam(1)
	local actionID = kNextAction:GetID()
	
	ODS("Skill_Multi_Strape_Fire_OnLeave NEXTACTION : "..actionID.." curParam:"..curParam.."\n")
	if kCurAction:GetNextActionName() ~= actionID then
		ODS("다른게 들어옴:"..actionID.."\n",false, 912)
		return false;
	end
	
	if kActor:IsMyActor() == false then
		return true;
	end
	
	if kNextAction:GetActionType()=="EFFECT" then
		if kActor:IsMyActor() then
			if( CheckNil(g_world == nil) ) then return true end
			if( CheckNil(g_world:IsNil()) ) then return true end
			g_world:SetShowWorldFocusFilterColorAlpha(0xFFFFFF, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
			g_world:ClearDrawActorFilter()
		end	
		return true;
	end
	
	if curParam == "end" then 
		return true
	end
	
	if kNextAction:GetEnable() == false then
		if curParam == actionID then
			kCurAction:SetParam(0,"")
		end
	end
	
	if kNextAction:GetEnable() == true then
		kCurAction:SetParam(0,"GoNext")
		return false;
	end
	return false
end

function Skill_Multi_Strape_Fire_OnEvent(kActor, textKey)
	
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	local	kAction = kActor:GetAction()
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	ODS("Skill_Multi_Strape_Fire_OnEvent textKey:"..textKey.."\n", false, 912);
	if textKey == "fire" then	
--		if kAction:GetParamInt(7) == 0 then	-- 발사가 가능 한가
	--		kAction:SetParamInt(7, 1)
			if(0 == kAction:GetParamInt(99)) then 
				Skill_Multi_Strape_Fire_DoEffect(kActor);
			elseif(1 == kAction:GetParamInt(99)) then 
				Skill_Multi_Strape_Fire_DoEffect1(kActor);
			elseif(2 == kAction:GetParamInt(99)) then 
				Skill_Multi_Strape_Fire_DoEffect2(kActor);
			end
			kAction:SetParamInt(100, kAction:GetParamInt(100)+1)
--		end
	end
end

function Skill_Multi_Strape_Fire_DoEffect(kActor)

	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
    kActor:FreeMove(false);

	local	kAction = kActor:GetAction()
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());

	--	kAction:SetParam(4,"HIT");
	
--	local	kProjectileMan = GetProjectileMan();

	-- 회전축 구하기
		local	kRotateAxis = Point3(0,0,1);		
		local	kPlayerRightVec = Point3(0,0,1);
		kPlayerRightVec:Cross(kActor:GetLookingDir())
		local fRotateAngle = 12.0*DEGTORAD
		local	i = 0;
		local	iAttackRange = 300 --kAction:GetSkillRange(0,kActor);
		
-- 여기서 발사	
--				local iPentrationCount = kActor:GetAbil(AT_PENETRATION_COUNT); -- 몇번 관통 할것인지 세팅
--[[	local kPos1 = kActor:GetNodeWorldPos("p_ef_heart");
	ODS("X:"..kPos1:GetX().." Y:"..kPos1:GetY().." Z:"..kPos1:GetZ().."\n", false, 912)]]
	local iMaxFireTime = kAction:GetParamInt(101)				--최대 발사 횟수
	local iFiredTime = kAction:GetParamInt(100)		
	local fRate = (iFiredTime*2)/iMaxFireTime
	local fForce = 250
	if(fRate < 1)  then 
		-- 1 to 0
		fRate = 1 - fRate		-- 좌측 방향에서 가운데로
		fForce = -fForce*fRate
	else		
		-- 0 to 1				-- 가운데 방향에서 우측 방향으로
		fRate = fRate -1 
		fForce = fForce*fRate
	end
		
		
	local iTestArrowNum = 5
	while i<iTestArrowNum do
	local	kArrow = Skill_Multi_Strape_Fire_LoadArrowToWeapon(kActor,kAction);
	
		if kArrow:IsNil() == false then		
--			kArrow:SetPenetrationCount(iPentrationCount);-- 몇번을 관통할것인지 세팅
		--[[	
			if iPentrationCount ~= 0 then
				kArrow:SetMultipleAttack(true);
			end
]]		
--[[			local kAEffect = kActor:GetAttackEffect(kAction:GetID())
			if false==kAEffect:IsNil() and ""~=kAEffect:GetEffectName() then
				kActor:AttachParticleS(421,kAEffect:GetNodeName(),kAEffect:GetEffectName(),kAEffect:GetScale())
			end
]]			
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
			
			local	kFireTarget = kActor:GetLookingDir()
			kFireTarget:Multiply(250);
			
			kFireTarget:Rotate(kPlayerRightVec, -45.0*DEGTORAD);
	--		ODS(" 발사체에 곱하는 값: "..(iAttackRange-30).."\n", false, 912)
			local iRotate = tonumber(kActor:GetParam("MULTISTRAPE"))
			if(nil == iRotate) then iRotate = 1 end
			if(20 < iRotate) then 
				iRotate = 0
			end
--			ODS("iRotate :"..iRotate.."\n", false, 912)
			kFireTarget:Rotate(kRotateAxis,fRotateAngle*(iRotate*math.random(2,10)));			
			--kFireTarget:Rotate(kRotateAxis,fRotateAngle*(iRotate)*0.7);
			
			kActor:SetParam("MULTISTRAPE", tostring(iRotate+1))
			kFireTarget:Add(kArrow:GetWorldPos());
--			ODS("X:"..kFireTarget:GetX().." Y:"..kFireTarget:GetY().." Z:"..kFireTarget:GetZ().."\n", false, 912)
			kArrow:SetTargetLoc(kFireTarget);
			
--------------------------------------------------------------------
		kArrow:Fire(true);		
		kArrow:SetMovingType(2); -- 베지어 곡선으로 이동 한다.
		local kPos1 = kArrow:GetFireStartPos();
		local kPos4 = kArrow:GetTargetLoc();
		
--		kPos1:SetZ(kPos1:GetZ()+ math.random(0,5))
		kArrow:SetParam(350,50); 		 --속력, 가속력, 중력
		
		local kPos2 = kArrow:GetFireStartPos();
		local kPos3 = kArrow:GetTargetLoc();
		kPos2:SetZ(kPos2:GetZ()+50);	
		kPos2:SetX(kPos2:GetX()+fForce)		
		
		--kPos2:SetZ(kPos2:GetZ() + math.random(100, 120));		
		-- kPos3:SetZ(kPos3:GetZ() - math.random(0, 30));		
		-- kPos2:SetX(kPos2:GetX() + math.random(0, 50));
		-- kPos3:SetX(kPos3:GetX() - math.random(0, 50));		
		-- kPos2:SetY(kPos2:GetY() + math.random(0, 50));
		-- kPos3:SetY(kPos3:GetY() - math.random(0, 50));

		local kDistance = kPos1:Distance(kPos4);
		local kDir = kArrow:GetDirection();
		kDir:Multiply(kDistance*-0.20);
		kPos2:Add(kDir);
		
		kPos3:SetX(kPos3:GetX()-fForce)
		kDir = kArrow:GetDirection();
		kDir:Multiply(kDistance*0.20);
		kPos3:Add(kDir);
		
		kArrow:SetParam_Bezier4SplineType(kPos1, kPos2, kPos3, kPos4);
-----------------------------------------------------------------------
		end
--		kActor:ClearIgnoreEffectList();	
		i=i+1;
	end	
end

function Skill_Multi_Strape_Fire_DoEffect1(kActor)

	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
    kActor:FreeMove(false);

	local	kAction = kActor:GetAction()
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());

	--	kAction:SetParam(4,"HIT");
	
--	local	kProjectileMan = GetProjectileMan();

	-- 회전축 구하기
		local	kRotateAxis = Point3(0,0,1);		
		local	kPlayerRightVec = Point3(0,0,1);
		kPlayerRightVec:Cross(kActor:GetLookingDir())
		local fRotateAngle = 12.0*DEGTORAD
		local	i = 0;
		local	iAttackRange = 300 --kAction:GetSkillRange(0,kActor);
		
-- 여기서 발사	
--				local iPentrationCount = kActor:GetAbil(AT_PENETRATION_COUNT); -- 몇번 관통 할것인지 세팅
--[[	local kPos1 = kActor:GetNodeWorldPos("p_ef_heart");
	ODS("X:"..kPos1:GetX().." Y:"..kPos1:GetY().." Z:"..kPos1:GetZ().."\n", false, 912)]]
	local iMaxFireTime = kAction:GetParamInt(101)				--최대 발사 횟수
	local iFiredTime = kAction:GetParamInt(100)		
	local fRate = (iFiredTime*2)/iMaxFireTime
	local fForce = 250
	if(fRate < 1)  then 
		-- 1 to 0
		fRate = 1 - fRate		-- 좌측 방향에서 가운데로
		fForce = -fForce*fRate
	else		
		-- 0 to 1				-- 가운데 방향에서 우측 방향으로
		fRate = fRate -1 
		fForce = fForce*fRate
	end
		
		
	local iTestArrowNum = 5
	while i<iTestArrowNum do
	local	kArrow = Skill_Multi_Strape_Fire_LoadArrowToWeapon(kActor,kAction);
	
		if kArrow:IsNil() == false then		
--			kArrow:SetPenetrationCount(iPentrationCount);-- 몇번을 관통할것인지 세팅
		--[[	
			if iPentrationCount ~= 0 then
				kArrow:SetMultipleAttack(true);
			end
]]		
--[[			local kAEffect = kActor:GetAttackEffect(kAction:GetID())
			if false==kAEffect:IsNil() and ""~=kAEffect:GetEffectName() then
				kActor:AttachParticleS(421,kAEffect:GetNodeName(),kAEffect:GetEffectName(),kAEffect:GetScale())
			end
]]			
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
			
			local	kFireTarget = kActor:GetLookingDir()
			kFireTarget:Multiply(250);
			
			kFireTarget:Rotate(kPlayerRightVec, -45.0*DEGTORAD);
	--		ODS(" 발사체에 곱하는 값: "..(iAttackRange-30).."\n", false, 912)
			local iRotate = tonumber(kActor:GetParam("MULTISTRAPE"))
			if(nil == iRotate) then iRotate = 1 end
			if(20 < iRotate) then 
				iRotate = 0
			end
			kFireTarget:Rotate(kRotateAxis,fRotateAngle*(iRotate)*0.7);
			
			kActor:SetParam("MULTISTRAPE", tostring(iRotate+1))
			kFireTarget:Add(kArrow:GetWorldPos());
--			ODS("X:"..kFireTarget:GetX().." Y:"..kFireTarget:GetY().." Z:"..kFireTarget:GetZ().."\n", false, 912)
			kArrow:SetTargetLoc(kFireTarget);
			
--------------------------------------------------------------------
		kArrow:Fire(true);		
		kArrow:SetMovingType(2); -- 베지어 곡선으로 이동 한다.
		local kPos1 = kArrow:GetFireStartPos();
		local kPos4 = kArrow:GetTargetLoc();
		
--		kPos1:SetZ(kPos1:GetZ()+ math.random(0,5))
		kArrow:SetParam(350,50); 		 --속력, 가속력, 중력
		
		local kPos2 = kArrow:GetFireStartPos();
		local kPos3 = kArrow:GetTargetLoc();
		kPos2:SetZ(kPos2:GetZ()+50);
						-- if(i%2 == 0 ) then
							-- --kPos2:SetX(kPos2:GetX()+math.random(10, 50))
							-- kPos2:SetX(kPos2:GetX()+50)
						-- else
							-- --kPos2:SetX(kPos2:GetX()-math.random(10, 50))
							-- kPos2:SetX(kPos2:GetX()-50)
						-- end
		kPos2:SetX(kPos2:GetX()+fForce)
		
		
		--kPos2:SetZ(kPos2:GetZ() + math.random(100, 120));		
		-- kPos3:SetZ(kPos3:GetZ() - math.random(0, 30));		
		-- kPos2:SetX(kPos2:GetX() + math.random(0, 50));
		-- kPos3:SetX(kPos3:GetX() - math.random(0, 50));		
		-- kPos2:SetY(kPos2:GetY() + math.random(0, 50));
		-- kPos3:SetY(kPos3:GetY() - math.random(0, 50));

		local kDistance = kPos1:Distance(kPos4);
		local kDir = kArrow:GetDirection();
		kDir:Multiply(kDistance*-0.20);
		kPos2:Add(kDir);
		
		kPos3:SetX(kPos3:GetX()-fForce)
		kDir = kArrow:GetDirection();
		kDir:Multiply(kDistance*0.20);
		kPos3:Add(kDir);
		
		kArrow:SetParam_Bezier4SplineType(kPos1, kPos2, kPos3, kPos4);
-----------------------------------------------------------------------
		end
--		kActor:ClearIgnoreEffectList();	
		i=i+1;
	end	
end

function Skill_Multi_Strape_Fire_DoEffect2(kActor)

	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
    kActor:FreeMove(false);

	local	kAction = kActor:GetAction()
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());

	--	kAction:SetParam(4,"HIT");
	
--	local	kProjectileMan = GetProjectileMan();

	-- 회전축 구하기
		local	kRotateAxis = Point3(0,0,1);		
		local	kPlayerRightVec = Point3(0,0,1);
		kPlayerRightVec:Cross(kActor:GetLookingDir())
		local fRotateAngle = 12.0*DEGTORAD
		local	i = 0;
		local	iAttackRange = 300 --kAction:GetSkillRange(0,kActor);
		
-- 여기서 발사	
--				local iPentrationCount = kActor:GetAbil(AT_PENETRATION_COUNT); -- 몇번 관통 할것인지 세팅
--[[	local kPos1 = kActor:GetNodeWorldPos("p_ef_heart");
	ODS("X:"..kPos1:GetX().." Y:"..kPos1:GetY().." Z:"..kPos1:GetZ().."\n", false, 912)]]
	local iMaxFireTime = kAction:GetParamInt(101)				--최대 발사 횟수
	local iFiredTime = kAction:GetParamInt(100)		
	-- local fRate = (iFiredTime*2)/iMaxFireTime
	-- local fForce = 250
	-- if(fRate < 1)  then 
		-- -- 1 to 0
		-- fRate = 1 - fRate		-- 좌측 방향에서 가운데로
		-- fForce = -fForce*fRate
	-- else		
		-- -- 0 to 1				-- 가운데 방향에서 우측 방향으로
		-- fRate = fRate -1 
		-- fForce = fForce*fRate
	-- end
		
		
	local iTestArrowNum = 5
	while i<iTestArrowNum do
	local	kArrow = Skill_Multi_Strape_Fire_LoadArrowToWeapon(kActor,kAction);
	
		if kArrow:IsNil() == false then		
--			kArrow:SetPenetrationCount(iPentrationCount);-- 몇번을 관통할것인지 세팅
		--[[	
			if iPentrationCount ~= 0 then
				kArrow:SetMultipleAttack(true);
			end
]]		
--[[			local kAEffect = kActor:GetAttackEffect(kAction:GetID())
			if false==kAEffect:IsNil() and ""~=kAEffect:GetEffectName() then
				kActor:AttachParticleS(421,kAEffect:GetNodeName(),kAEffect:GetEffectName(),kAEffect:GetScale())
			end
]]			
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
			
			local	kFireTarget = kActor:GetLookingDir()
			kFireTarget:Multiply(250);
			
			kFireTarget:Rotate(kPlayerRightVec, -45.0*DEGTORAD);
	--		ODS(" 발사체에 곱하는 값: "..(iAttackRange-30).."\n", false, 912)
			local iRotate = tonumber(kActor:GetParam("MULTISTRAPE"))
			if(nil == iRotate) then iRotate = 1 end
			if(20 < iRotate) then 
				iRotate = 0
			end
--			ODS("iRotate :"..iRotate.."\n", false, 912)
			kFireTarget:Rotate(kRotateAxis,fRotateAngle*(iRotate*math.random(2,10)));			
			--kFireTarget:Rotate(kRotateAxis,fRotateAngle*(iRotate)*0.7);
			
			kActor:SetParam("MULTISTRAPE", tostring(iRotate+1))
			kFireTarget:Add(kArrow:GetWorldPos());
--			ODS("X:"..kFireTarget:GetX().." Y:"..kFireTarget:GetY().." Z:"..kFireTarget:GetZ().."\n", false, 912)
			kArrow:SetTargetLoc(kFireTarget);
			
--------------------------------------------------------------------
		kArrow:Fire(true);		
		kArrow:SetMovingType(2); -- 베지어 곡선으로 이동 한다.
		local kPos1 = kArrow:GetFireStartPos();
		local kPos4 = kArrow:GetTargetLoc();
		
--		kPos1:SetZ(kPos1:GetZ()+ math.random(0,5))
		kArrow:SetParam(350,50); 		 --속력, 가속력, 중력
		
		local kPos2 = kArrow:GetFireStartPos();
		local kPos3 = kArrow:GetTargetLoc();
		kPos2:SetZ(kPos2:GetZ()+50);
						 if(i%2 == 0 ) then
							 --kPos2:SetX(kPos2:GetX()+math.random(10, 50))
							 kPos2:SetX(kPos2:GetX()+50)
						 else
							 --kPos2:SetX(kPos2:GetX()-math.random(10, 50))
							 kPos2:SetX(kPos2:GetX()-50)
						 end
		
		
		
		--kPos2:SetZ(kPos2:GetZ() + math.random(100, 120));		
		-- kPos3:SetZ(kPos3:GetZ() - math.random(0, 30));		
		-- kPos2:SetX(kPos2:GetX() + math.random(0, 50));
		-- kPos3:SetX(kPos3:GetX() - math.random(0, 50));		
		-- kPos2:SetY(kPos2:GetY() + math.random(0, 50));
		-- kPos3:SetY(kPos3:GetY() - math.random(0, 50));

		local kDistance = kPos1:Distance(kPos4);
		local kDir = kArrow:GetDirection();
		kDir:Multiply(kDistance*-0.20);
		kPos2:Add(kDir);
		
		--kPos3:SetX(kPos3:GetX()-fForce)
		kDir = kArrow:GetDirection();
		kDir:Multiply(kDistance*0.20);
		kPos3:Add(kDir);
		
		kArrow:SetParam_Bezier4SplineType(kPos1, kPos2, kPos3, kPos4);
-----------------------------------------------------------------------
		end
--		kActor:ClearIgnoreEffectList();	
		i=i+1;
	end	
end