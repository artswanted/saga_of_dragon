-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID

-- Melee
function Skill_HealingWave_OnCheckCanEnter(kActor,kAction)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
		
	local weapontype = kActor:GetEquippedWeaponType()
	
	if weapontype == 0 then
		return	false
	end
	return		true	
end

function Skill_HealingWave_OnCastingCompleted(kActor, kAction)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	Skill_HealingWave_Fire(kActor,kAction)
	--kAction:SetSlot(kAction:GetCurrentSlot())
	kActor:PlayCurrentSlot()
	kActor:CutSkillCasting()
end

function Skill_HealingWave_OnTargetListModified(kActor,kAction,bIsBefore)	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())	
    --ODS("Skill_HealingWave_OnTargetListModified\n", false, 912)
    --서버에서 보내온 타겟 수정 리스트에 있는 타겟들에게
    if bIsBefore then
    else
	    Skill_HealingWave_CreateLinkedPlane(kActor,kAction)   --  LinkedPlane 재생성	    
	    kAction:GetTargetList():ApplyActionEffects()
		local kTargetList = kAction:GetTargetList()		
		local iTargetCount = kTargetList:size()
		if(0 == iTargetCount) then return end
		local i =0
		--힐 되는 파티클 효과를 붙여 줌
		while i<iTargetCount do			
			local   kTargetInfo = kTargetList:GetTargetInfo(i)
			if(true == kTargetInfo:IsNil()) then return end
			local   kTargetGUID = kTargetInfo:GetTargetGUID()
			if(true == kTargetGUID:IsNil()) then return end			
			local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)
			--local Name = kTargetPilot:GetName():GetStr()
			--ODS("찾은 캐릭 이름 :"..Name.."\n",false, 912)
			
			if kTargetPilot:IsNil() == false then
				local kTargetActor = kTargetPilot:GetActor()
				kTargetActor:AttachParticleS(200,"char_root", "ef_slowheal_02_char_root", 1) 
			else
				--ODS("kTargetPilot이 닐\n", false, 912)
			end
			i=i+1
		end
		
		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID")
		if kSoundID~="" then
			kActor:AttachSound(2784,kSoundID)
		end	
    end
end


function Skill_HealingWave_Fire(kActor,kAction)
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
--	ODS("RestoreItemModel Skill_HealingWave_Fire\n", false, 912)
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	kAction:SetParamInt(2,-1)    --  그룹 번호
	kAction:SetParamFloat(3,g_world:GetAccumTime()-1)  --  타겟리스트 생성 시간	
	kAction:SetParamInt(4,0) -- 이전 타겟리스트의 사이즈
end

function Skill_HealingWave_UpdateTarget(kActor,kAction,accumTime)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
    if kActor:IsMyActor() == false then
        return
    end

    --  1초에 한번씩 새로 타겟을 찾아 서버로 보낸다.
    local   fLastUpdateTime = kAction:GetParamFloat(3)
    if accumTime - fLastUpdateTime <= 1.0 then
        return
    end
   
    kAction:SetParamFloat(3,accumTime)
	Skill_HealingWave_OnFindTarget(kActor, kAction, kAction:GetTargetList())
	--  서버로 브로드캐스팅
	if kActor:IsMyActor() == true then
		--kAction:CreateActionTargetList(kActor)		
		local kTargetList = kAction:GetTargetList()		
		local iTargetCount = kTargetList:size()
		local iMaxTargetCnt = kAction:GetAbil(AT_MAX_TARGETNUM)
		local i = iMaxTargetCnt
		if(iMaxTargetCnt < iTargetCount) then 			
			while(i < iTargetCount) do 
				kTargetList:DeleteTargetInfo(i)
				i=i+1
			end
		end
		kAction:BroadCastTargetListModify(kActor:GetPilot(),true)		
	end
	
	Skill_HealingWave_CreateLinkedPlane(kActor,kAction)   --  LinkedPlane 재생성
end

function Skill_HealingWave_OnFindTarget(kActor,kAction,kTargets)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())	
	
	kAction:ClearTargetList()   -- 클리어하고 
	
	local  iAttackRange = kAction:GetSkillRange(0,kActor)
	if iAttackRange == 0 then
	    iAttackRange = 100
	end
--	ODS("범위 :"..iAttackRange.."\n", false, 912)
	
    local   kMyHandPos = kActor:GetNodeWorldPos("p_wp_l_hand")
	local kParam = FindTargetParam()
	
	-- 위치값으로 부터 첫 타겟을 찾음
	kParam:SetParam_1(kMyHandPos,kActor:GetLookingDir())
	kParam:SetParam_2(iAttackRange,kAction:GetAbil(AT_1ST_AREA_PARAM2),0,1)
	kParam:SetParam_3(true,FTO_NORMAL)
	local	kTempTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo())
	local iTargets = kAction:FindTargets(TAT_BAR,kParam,kTempTargets,kTempTargets)
	
	-- 찾지 못했다면 그냥 리턴~
	if iTargets == 0 then
		return  iTargets
	end
	--ODS("타겟 찾기 시작---------------------------------------------------------\n", false, 912)
	local kTargetInfo = Skill_HealingWave_GetNearest_NoMarked_TargetInfo(kActor, kAction, kMyHandPos, kTempTargets)
    if nil == kTargetInfo then return 0 end
	if true == kTargetInfo:IsNil() then return 0 end

    --첫번째 타겟을 추가 하고
	Skill_HealingWave_AddTarget(kAction, kTargetInfo)
		
	-- 여기서 부터 반복 되는 형식이다. 
	local   kTargetGUID = kTargetInfo:GetTargetGUID()
	if(true == kTargetGUID:IsNil()) then return end
	local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)	
	local 	kActorTarget = kTargetPilot:GetActor()		
	local kBeginPos = kActorTarget:GetPos()
	Skill_HealingWave_FindTarget(kActor, kAction, kBeginPos)
	
	local kCurTargetList = kAction:GetTargetList()
	local iTargetCount = kCurTargetList:size()	
	--ODS("타겟 찾기 종료 :갯수"..iTargetCount.."---------------------------------------------------------\n", false, 912)	
	return iTargetCount
end


function Skill_HealingWave_CreateLinkedPlane(kActor,kAction)

	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
    local   kMan = GetLinkedPlaneGroupMan()
       
    local   iGroupNum = kAction:GetParamInt(2)
    local   kGroup
    if iGroupNum == -1 then
        --  그룹을 새로 생성한다.
        kGroup = kMan:CreateNewLinkedPlaneGroup()
        kGroup:SetTexture("../Data/5_Effect/9_Tex/ef_healing_wave_01.dds")
        kAction:SetParamInt(2,kGroup:GetInstanceID())
        
    else
        --  이미 그룹이 있으면 삭제하고
        kGroup = kMan:GetLinkedPlaneGroup(iGroupNum)
        kGroup:ReleaseAllPlane()
    end

    local kPlane = kGroup:AddNewPlane(127)    --  기본으로 1개는 생성한다.
    kPlane:SetWidth(20)
    
    local   kTargetList = kAction:GetTargetList()
    local   iTargetCount = kTargetList:size()    
    local   i=0

    while i<iTargetCount-1 do
        local kPlane = kGroup:AddNewPlane(127)
        kPlane:SetWidth(20)
        i=i+1
    end

end
function Skill_HealingWave_UpdatePlanePos(kActor,kAction)
-- 서버에서 타겟 리스트에 들어갔던 액터들의 위치를 변경시키기도 하기 때문에
-- 연결 이펙트 위치를 TargetList에 가장 가까운 위치에 있는 타겟 위치로 부터 시작하고
-- 다시 그타겟으로 부터 가장 가까운 타겟을 찾아가는 순서로 연결 이펙트를 붙일 위치를 재정렬하여 사용 한다
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
    local   kMan = GetLinkedPlaneGroupMan()

    local   iGroupNum = kAction:GetParamInt(2)

    local kGroup = kMan:GetLinkedPlaneGroup(iGroupNum)
--    kGroup:SetTextureCoordinateUpdateDelay(0.05)
    local   kTargetList = kAction:GetTargetList()
	if(true == kTargetList:IsNil()) then 
		ODS("Skill_HealingWave_UpdatePlanePos: 타겟 리스트가 없다\n", false, 912)
		return
	end
    local   iTargetCount = kTargetList:size()
    
    local   i=0
    
    local   kPlane = kGroup:GetPlane(i)

    --  첫번째 플레인의 StartPos 는 나의 좌표이다.
	local kDist = kActor:GetLookingDir()
    local   kMyHandPos = kActor:GetNodeWorldPos("p_wp_r_hand")
	kDist:Multiply(-12)
	kMyHandPos:Add(kDist)	
    kPlane:SetStartPos(kMyHandPos)
		
	local  iAttackRange = kAction:GetSkillRange(0,kActor)
	if iAttackRange == 0 then
	    iAttackRange = 100
	end
	
    if iTargetCount == 0 then
    
       --  타겟이 하나도 없다면 AttackRange 만큼 앞으로 한줄기의 라이트닝 플레인을 만든다.
        local kLookDir = kActor:GetLookingDir()		
        kLookDir:Multiply(iAttackRange)
        kLookDir:Add(kMyHandPos)        
        kPlane:SetEndPos(kLookDir)    
		kPlane:SetEnable(true)
		--ODS("타겟이 없어 플랜을 만듬\n", false, 912)
        return
    end    
    
    local   kFirstPos = kMyHandPos
    local 	kPrevPos = kMyHandPos
	--ODS("플랜 타겟 수 :"..iTargetCount.."\n", false, 912)
	
	--여기서 타겟 리스트 재정렬함
	
	--ODS("플랜 연결 시작-----------------------------"..iTargetCount.."\n", false, 912)
    while i<iTargetCount do
	
        -- local   kTargetInfo = kTargetList:GetTargetInfo(i)
		-- if(nil == kTargetInfo) then return end
		-- if(true == kTargetInfo:IsNil()) then return end
        -- local   kTargetGUID = kTargetInfo:GetTargetGUID()
		-- if(true == kTargetGUID:IsNil()) then return end
        -- local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)
        local   bActorInvalid = true
        
--      if kTargetPilot:IsNil() == false then
		--	local Name = kTargetPilot:GetName():GetStr()
	--		ODS("찾은 캐릭 이름 :"..Name.."\n",false, 912)
			
            --local   kTargetActor = kTargetPilot:GetActor()
			local kTargetActor = Skill_HealingWave_GetNearestActor(kActor, kAction, kFirstPos, kTargetList)
			if(nil == kTargetActor) then return end
            if kTargetActor:IsNil() == false then				
				local kCurPos = kTargetActor:GetPos()
                kPlane:SetEndPos(kCurPos)
                kPlane:SetEnable(true)
                bActorInvalid = false
                
                --if kFirstPos == nil then
                    kFirstPos = kTargetActor:GetPos()				
                --end
				--[[
				local fDist = kPrevPos:Distance(kCurPos)
				kPrevPos = kTargetActor:GetPos()	-- 갱신
				if(fDist > iAttackRange) then
					bActorInvalid = true
					--ODS("레인지보다 거리가 멈\n", false, 912)
					kTargetList:DeleteTargetInfo(i)
				end
				]]
				--ODS("iAttackRange:"..iAttackRange.."\n", false, 912)					
               
            end
		--end 
        kPlane:SetEnable(bActorInvalid == false)   --  유효하지 않은 액터라면 플레인을 비활성화시킨다.
                
        i=i+1
        
        kPlane = kGroup:GetPlane(i)
   
        if kPlane:IsNil() then
            break
        end

        if kFirstPos ~= nil then
            kPlane:SetStartPos(kFirstPos)
        else
            kPlane:SetStartPos(kMyHandPos)
        end
        
    end
	--ODS("플랜 연결 끝--------------------------------------------\n", false, 912)
end

function Skill_HealingWave_OnEnter(kActor, kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	local actorID = kActor:GetID()
	local actionID = kAction:GetID()
	local weapontype = kActor:GetEquippedWeaponType()
	
--	ODS("Skill_HealingWave_OnEnter actionID:"..actionID.." GetActionParam:"..kAction:GetActionParam().."\n", false, 912)
	if kAction:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true
	end

	Skill_HealingWave_OnCastingCompleted(kActor,kAction)
	kActor:SetParam("shot", "false")
	
	kAction:SetParamInt(10, 0)	-- 스킬 사용 유지	
	local iMaxTargetCnt = kAction:GetAbil(AT_MAX_TARGETNUM)
	if(0 == iMaxTargetCnt) then 
		iMaxTargetCnt = 3
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	kAction:SetParamFloat(30, g_world:GetAccumTime())
	local  iAttackRange = kAction:GetSkillRange(0,kActor)

	return true
end

function Skill_HealingWave_OnUpdate(kActor, accumTime, frameTime)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
		
--	local actorID = kActor:GetID()
	local kAction = kActor:GetAction()
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	--ODS("값 :"..kAction:GetParamInt(10).."\n", false, 912)
	
	local animDone = kActor:IsAnimationDone()
	

--	local nextActionName = kAction:GetNextActionName()
--	local actionID = kAction:GetID()
	local iCurrentSlot = kAction:GetCurrentSlot()

	if kActor:IsMyActor() then
		local	bKeyUp = KeyIsDown(KEY_UP)
		local	bKeyDown = KeyIsDown(KEY_DOWN)
		local	bKeyLeft = KeyIsDown(KEY_LEFT)
		local	bKeyRight = KeyIsDown(KEY_RIGHT)
		
		local	iDir = 0
		if bKeyUp then 
			iDir = iDir + DIR_UP
		end
		if bKeyDown then
			iDir = iDir + DIR_DOWN
		end
		if bKeyLeft then
			iDir = iDir + DIR_LEFT
		end
		if bKeyRight then
			iDir = iDir + DIR_RIGHT
		end

		if iDir ~= 0 then
			local iMoveSpeed = kAction:GetAbil(AT_MOVESPEED)
			--ODS("스피드"..iMoveSpeed.."\n", false, 912)
			local kDirection = kActor:GetDirectionVector(iDir)

			kDirection:Multiply(iMoveSpeed)
			kActor:SetMovingDelta(kDirection)
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			local fTimeDelta = g_world:GetAccumTime() - kAction:GetParamFloat(30)
			if(fTimeDelta > 0.02) then -- 서버로 이동 위치 갱신 보내는 시간 간격
				g_pilotMan:BroadcastDirection(kActor:GetPilot(), iDir)
				kAction:SetParamFloat(30, g_world:GetAccumTime())
			end
		end
	end	
	
	if(1 == kAction:GetParamInt(10)) then 
		kAction:SetParamInt(10, 2)
		kActor:PlayNext()
		--ODS("업데이트 종료\n", false, 912)
		return true
	elseif (2 == kAction:GetParamInt(10) and (true == animDone) ) then
		kAction:SetParamInt(10, 3)
		return false
	end
	
	
	if animDone == true then
		--ODS("update:애니돈\n", false, 912)
		kAction:SetSlot(kAction:GetCurrentSlot()+1)
		kActor:PlayCurrentSlot()
		kActor:SetParam("shot", "true")
		kActor:AttachParticle(9221,"p_wp_r_hand","ef_skill_HealingWave_01")
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		Skill_HealingWave_UpdateTarget(kActor,kAction,g_world:GetAccumTime())

		Skill_HealingWave_CreateLinkedPlane(kActor,kAction)
		kAction:SetParamInt(10, 4)
	end

	if iCurrentSlot == 1 then
		if kAction:GetActionParam() == AP_FIRE and kActor:GetParam("shot") == "true" then
		--ODS("update:슬롯 1\n", false, 912)
			if(false == Skill_IsHealingEffect(kActor, kAction)) then
				return false
			end
			Skill_HealingWave_UpdateTarget(kActor,kAction,accumTime)
			Skill_HealingWave_UpdatePlanePos(kActor,kAction)
		end
	end
	
	return true
end

function Skill_HealingWave_OnCleanUp(kActor)
	--ODS("클린업~\n",false, 912)
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
    --  그룹을 제거한다.
    local   iGroupNum = kActor:GetAction():GetParamInt(2)
    if iGroupNum ~= -1 then
        local   kMan = GetLinkedPlaneGroupMan()
        kMan:ReleaseLinkedPlaneGroup(iGroupNum)
        kActor:GetAction():SetParamInt(2,-1)
    end 
   
    kActor:DetachFrom(9221)    
	return true
end

function Skill_HealingWave_OnLeave(kActor, kAction)
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	local kCurAction = kActor:GetAction()
	
	CheckNil(nil==kCurAction)
	CheckNil(kCurAction:IsNil())
	
	local curParam = kCurAction:GetParam(1)
	local actionID = kAction:GetID()
	
--	ODS("액션 네임:"..actionID.."\n",false, 912)
	if kActor:IsMyActor() == false then
		return true
	end

	if(3 == kCurAction:GetParamInt(10)) then
		-- 종료 애니메이션이 완료되었으면 종료 함
		return true
	end
	
	if kAction:GetActionType()=="EFFECT" then
		return true
	end
	
	if curParam == "end" then 
		return true
	end
	
    if "a_run" == actionID or  "a_dash" == actionID or "a_hi" == actionID then
		-- 다음 액션이 이동, 대시이면 이 스킬은 계속 유지됨
        return  false
	else
		-- 종료 애니메이션 시작
		if(4 == kCurAction:GetParamInt(10)) then			
			kCurAction:SetParamInt(10, 1)
		end
		return false
    end
			
	return false 
end
  
function Skill_HealingWave_FindTarget(kActor, kAction, kBeginPos)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())	

	-- MAX개만큼 추가 되어있지 않으면
	local kTargetList = kAction:GetTargetList()	
	local iMaxTarget = kAction:GetAbil(AT_MAX_TARGETNUM)
	if(iMaxTarget <= kTargetList:size()) then
		return
	end	
		
	local kParam = FindTargetParam()
	--kBeginPos 위치로 타겟을 찾고
	kParam:SetParam_1(kBeginPos,kActor:GetLookingDir())
	kParam:SetParam_2(0,0,kAction:GetAbil(AT_2ND_AREA_PARAM1),kMaxTarget)
	kParam:SetParam_3(true,FTO_NORMAL)	
	
	local	kTempTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo())
	iTargets = kAction:FindTargets(TAT_SPHERE,kParam,kTempTargets,kTempTargets)
	if(0 == iTargets) then 
	-- 타겟을 찾지 못했으면 임시 타겟 리스트 지움
		DeleteActionTargetList(kTempTargets)
		return 
	end
	
	-- 찾은 타겟들을 얻어 놓은 리스트를 돌면서 
	local kNewTargetInfo = nil
	local i = 0
	local iTargetCount = kTempTargets:size()
	while i<iTargetCount do
		local   kTargetInfo = kTempTargets:GetTargetInfo(i)
		if(true == kTargetInfo:IsNil()) then return end
		local   kTargetGUID = kTargetInfo:GetTargetGUID()
		if(true == kTargetGUID:IsNil()) then return end
		--새로운 타겟을 찾았다면
		kNewTargetInfo = Skill_HealingWave_GetNearest_NoMarked_TargetInfo(kActor, kAction, kBeginPos, kTempTargets)
		if(nil ~= kNewTargetInfo) then
			break
		end		
		i = i + 1
	end
	
	if(nil == kNewTargetInfo) then 
--		ODS("새로운 타겟이 없네요\n", false, 912)
		return
	end
	
--	local kCurTargetList = kAction:GetTargetList()
--	kCurTargetList:AddTarget(kNewTargetInfo)
	Skill_HealingWave_AddTarget(kAction, kNewTargetInfo)
	
	local   kTargetGUID = kNewTargetInfo:GetTargetGUID()
	if(true == kTargetGUID:IsNil()) then return end

	local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)
	local   kTargetActor = nil	
	if kTargetPilot:IsNil() == false then
		kTargetActor = kTargetPilot:GetActor()
	end
	if(nil == kTargetActor) then return false end
	local kTargetPos = kTargetActor:GetPos()
	
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	--타겟이 존재 하면, 또 찾는다
	Skill_HealingWave_FindTarget(kActor,kAction, kTargetPos)
	DeleteActionTargetList(kTempTargets) -- 다 썼으니 임시 타겟 리스트 지움
end

function Skill_HealingWave_IsInActionTargetList(kActor,kAction, kCompareGUID)
-- kCompareGUID의 GUID를 가진 타겟이 이미 리스트에 추가되어있는가 확인
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	local i = 0
	local kTargetList = kAction:GetTargetList()
	local iTargetCount = kTargetList:size()
--	ODS("현재 액션 리스트의 갯수 : "..iTargetCount.."\n", false, 912)
	while i<iTargetCount do
		local   kTargetInfo = kTargetList:GetTargetInfo(i)
		if(false == kTargetInfo:IsNil()) then 
			local   kTargetGUID = kTargetInfo:GetTargetGUID()
			if(false == kTargetGUID:IsNil()) then 
				if(kTargetGUID:GetString() == kCompareGUID:GetString()) then
	--			ODS("존재함\n", false, 912)
					return true
				end
			end
		end		
		i = i + 1
	end	
--	ODS("새로운 타겟임\n", false, 912)
	return false
end

function Skill_HealingWave_AddTarget(kAction, kTargetInfo)
-- MAX개를 넘지 않았으면 타겟을 추가한다
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	local kTargetList = kAction:GetTargetList()
	if(true == kTargetList:IsNil()) then return false end
	
	local iMaxTarget = kAction:GetAbil(AT_MAX_TARGETNUM)
	--ODS("MaxTarget:"..iMaxTarget.."\n", false, 912)
	if(iMaxTarget > kTargetList:size()) then
		kTargetList:AddTarget(kTargetInfo)
		-- local   kTargetGUID = kTargetInfo:GetTargetGUID()
		-- if(false == kTargetGUID:IsNil()) then 		
			-- local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)
			-- if kTargetPilot:IsNil() == false then				
			-- end
		-- end
		--ODS("찾은 타겟 갯수:"..kTargetList:size().."\n", false, 912)		
	else
		return false
	end	
	return true 
end

function Skill_HealingWave_GetNearest_NoMarked_TargetInfo(kActor, kAction, kFromPos, kTargetList)
--kFromPos로 부터 
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())	
	if(nil == kFromPos) then return nil end

	if(nil == kTargetList) then return nil end
	if(true == kTargetList:IsNil()) then return nil end
	
	local kFoundTarget = nil	
	local fMinDist = kAction:GetSkillRange(0,kActor)
	if fMinDist == 0 then
	    fMinDist = 100
	end
	
	local i = 0	
	local iTargetCount = kTargetList:size()
	if(0 == iTargetCount) then 
		--ODS("Skill_HealingWave_GetNearest_NoMarked_TargetInfo 타겟이 없네요\n", false, 912)
		return nil
	end
	--local Name  = ""
	
	--TargetList에 있는 타겟들중
	while i<iTargetCount do
		local   kTargetInfo = kTargetList:GetTargetInfo(i)
		if(false == kTargetInfo:IsNil()) then 
			local   kTargetGUID = kTargetInfo:GetTargetGUID()
			if(false == kTargetGUID:IsNil()) then 		
				local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)
				if kTargetPilot:IsNil() == false then
					local kTargetActor = kTargetPilot:GetActor()
					local fDist = kFromPos:Distance(kTargetActor:GetPos())
					--이미 타겟으로 선택되지 않은 대상을 찾아
					if(false == Skill_HealingWave_IsInActionTargetList(kActor,kAction, kTargetGUID)) then
						if(fMinDist >= fDist) then 
							fMinDist = fDist
							kFoundTarget = kTargetInfo  --	Name = kTargetPilot:GetName():GetStr()
						end
					end
				end
			end
		end		
		i = i + 1
	end	
	
	-- if(nil == kFoundTarget) then 
		-- ODS("Skill_HealingWave_GetNearest_NoMarked_TargetInfo: 타겟을 찾지 못했습니다.\n", false, 912)
	-- else
		-- ODS("찾은 캐릭 이름 :"..Name.."\n",false, 912)
	-- end
	
	--반환한다
	return kFoundTarget
end



function Skill_HealingWave_GetNearestActor(kActor, kAction, kFromPos, kTargetList)
--kFromActor로 부터 가장 가까운데 있는
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())	
	if(nil == kFromPos) then return nil end
	if(nil == kTargetList) then return nil end
	if(true == kTargetList:IsNil()) then return nil end
	

	local kFoundActor = nil	
	local fMinDist = kAction:GetSkillRange(0,kActor)
	if fMinDist == 0 then
	    fMinDist = 100
	end
	
	local i = 0	
	local iTargetCount = kTargetList:size()
	if(0 == iTargetCount) then
		return nil
	end
	--local Name  = ""
	
	--kTargetList 안의 액터 중	
	while i<iTargetCount do
		local   kTargetInfo = kTargetList:GetTargetInfo(i)
		if(false == kTargetInfo:IsNil()) then 
			local   kTargetGUID = kTargetInfo:GetTargetGUID()
			if(false == kTargetGUID:IsNil()) then 		
				local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)
				if kTargetPilot:IsNil() == false then
					local kTargetActor = kTargetPilot:GetActor()
					local kTargetPos = kTargetActor:GetPos()
					--위치가 같지 않은(자기자신이 아닌) 
					if(kTargetPos:GetX() ~= kFromPos:GetX() or
					   kTargetPos:GetY() ~= kFromPos:GetY() or
					   kTargetPos:GetZ() ~= kFromPos:GetZ()   ) then
					   --가장 가까운 액터를 찾아
						local fDist = kFromPos:Distance(kTargetPos)						
						if(fMinDist >= fDist) then 
							fMinDist = fDist
							kFoundActor = kTargetActor 		--Name = kTargetPilot:GetName():GetStr()
						end					
					end
				end
			end
		end		
		i = i + 1
	end	
	-- if(nil == kFoundActor) then 
		-- ODS("타겟 재정렬: 타겟을 찾지 못했습니다.\n", false, 912)
	-- else
		-- ODS("타겟 재정렬 찾은 캐릭 이름 :"..Name.."\n",false, 912)
	-- end
	
	-- 반환 한다
	return kFoundActor
end

function Skill_IsHealingEffect(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsMyActor() == false then
        return true;
    end

	local kPilot = actor:GetPilot();
	if kPilot:IsNil() == false then
		local kUnit = kPilot:GetUnit();
		if kUnit:IsNil() == false then
			local kSkillDef = GetSkillDef(action:GetActionNo());
			if kSkillDef:IsNil() == false then
				local kEffect = kUnit:GetEffect(kSkillDef:GetEffectNo(), true)
		 		if kEffect:IsNil() == false then
					return true;
				end
			end
		end
	end

	action:SetParam(1,"end")
	return false;
end
