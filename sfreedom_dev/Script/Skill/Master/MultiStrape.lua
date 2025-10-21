-- [SUMMON_CLASSNO] : 소환해야하는 엔티티의 ClassNo
-- [SUMMON_RESERVE_ACTION] : 소환해야하는 엔티티의 시작 액션
-- [CASTING_SOUND_ID] : 스킬 시작 할때 
-- [FIRE_SOUND_ID] : 새가 나타날때
-- [FIRE_SOUND_ID1] :새가 사라질때
-- [CASTING_SOUND_ID_MALE] : 캐스팅 시에 플레이 할 사운드 ID - 남성
-- [CASTING_SOUND_ID_FEMALE] : 캐스팅 시에 플레이 할 사운드 ID - 여성

function Skill_Multi_Strape_OnCheckCanEnter(kActor, kAction)
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	local weapontype = kActor:GetEquippedWeaponType()
	
	if weapontype == 0 then
		return	false
	end
	
	-- 안전거품 공격 막기
	if g_pilotMan:IsLockMyInput() == true then
		return
	end
	
	return true
end

function Skill_Multi_Strape_OnCastingCompleted(kActor, kAction)
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	kActor:FreeMove(false)
	
	local	kSoundID = kAction:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		kActor:AttachSound(2784,kSoundID);
	end
	QuakeCamera(3, 0.6, 1, 1, 2)
--	Act_Melee_Default_OnCastingCompleted(kActor,kAction); 
	--캐스팅이 완료가 되면 타켓을 찾음
	kAction:CreateActionTargetList(kActor, false, true)	-- 액션리절트 계산하는것 사용하지 않음(랜덤 시드와 관련, 핸덤 시드는 서버에서만 씀 클라는 안씀)
	
	-- 화면 어두워지는 필터에 공격받을 타겟들이 가려지지 않게함
	local i = 0;
	local kTargetList = kAction:GetTargetList();
	local iTargetCount = kTargetList:size();
	while i<iTargetCount do
		local   kTargetInfo = kTargetList:GetTargetInfo(i);
		local   kTargetGUID = kTargetInfo:GetTargetGUID();
		local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
		if kTargetPilot:IsNil() == false then
			if kActor:IsMyActor() then
				if( CheckNil(g_world == nil) ) then return false end
				if( CheckNil(g_world:IsNil()) ) then return false end
				g_world:AddDrawActorFilter(kTargetGUID);
			end
		end
		i = i + 1;
	end	
	
	Skill_Master_Summon_Common_Fire(kActor,kAction)
end

function Skill_Multi_Strape_OnEnter(kActor, kAction)
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
		
	ODS("Skill_Multi_Strape_OnEnter\n", false, 912);
	
	kActor:FreeMove(false)
	kActor:StartWeaponTrail() -- 막타칠때 없앤다
	
	if kActor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 1.0, 0.4,true,true)
		g_world:AddDrawActorFilter(kActor:GetPilotGuid());
	end
	
	local	kSoundID = kAction:GetScriptParam("CASTING_SOUND_ID");
	if kSoundID~="" then
		kActor:AttachSound(2783,kSoundID);
	end
	
	--남성용 사운드
	if 1 == kActor:GetAbil(AT_GENDER) then
		local	kSoundID2 = kAction:GetScriptParam("CASTING_SOUND_ID_MALE");
		if kSoundID2~="" then
			kActor:AttachSound(2786,kSoundID2);
			ODS("남자사운드\n", false, 912)
		end	
	--여성용 사운드
	else
		local	kSoundID2 = kAction:GetScriptParam("CASTING_SOUND_ID_FEMALE");
		if kSoundID2~="" then
			kActor:AttachSound(2786,kSoundID2);
			ODS("여자사운드\n", false, 912)
		end	
	end
	
	if kAction:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true
	end
	
	Skill_Multi_Strape_OnCastingCompleted(kActor, kAction);	-- 캐스팅 스킬이 아닐때의 대비

	kActor:StopJump()
		  
	kActor:SetCanHit(false);

	return true
end

function Skill_Multi_Strape_OnUpdate(kActor, fAccumTime, fFrameTime)	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
		
	local	kAction = kActor:GetAction()
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	kActor:LockBidirection(false);
	kActor:SeeFront(true);
	kActor:FreeMove(false);

	local animDone = kActor:IsAnimationDone()
	local nextActionName = kAction:GetNextActionName()
	
	if kAction:GetParam(1) == "end" then
		return	false;
	end
	
	if animDone == true then
		QuakeCamera(1, 3, 1, 1, 1)
		local	kSoundID = kAction:GetScriptParam("FIRE_SOUND_ID1");
		if kSoundID~="" then
			kActor:AttachSound(2784,kSoundID);
		end
		
		local curActionParam = kAction:GetParam(0)
		if curActionParam == "GoNext" then
			kAction:SetParam(0, "null")
			kAction:SetParam(1, "end")
			return false;
		else
			kAction:SetParam(1, "end")
			return false
		end
	end
	
	return true
end

function Skill_Multi_Strape_OnCleanUp(kActor, kNextAction)	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kNextAction)
	CheckNil(kNextAction:IsNil())

	ODS("Skill_Multi_Strape_ReallyLeave nextaction : "..kNextAction:GetID().."\n", false, 912)
	
	return true
end

function Skill_Multi_Strape_OnLeave(kActor, kNextAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kNextAction)
	CheckNil(kNextAction:IsNil())

	local kCurAction = kActor:GetAction()
	local actionID = kNextAction:GetID()
	local curParam = kCurAction:GetParam(1)
	
	if kCurAction:GetNextActionName() ~= actionID then
		ODS("다른게 들어옴:"..actionID.."\n",false, 912)
		return false;
	end
	
--	kNextAction:SetTargetList(kCurAction:GetTargetList())

	local kTargetList = kCurAction:GetTargetList()
	if(true == kTargetList:IsNil()) then return true end
	local iTargetCount = kTargetList:size()
	
--	ODS("아이이디디디디디디:"..kNextAction:GetID().."\n",false, 912)
--	ODS("액션버버버버버번호:"..kNextAction:GetActionNo().."\n",false, 912)
	
	local STD_INDEX = 1000
	kActor:SetParam(STD_INDEX, tostring(iTargetCount))
	
	local i = 0
	while i<iTargetCount do
		local   kTargetInfo = kTargetList:GetTargetInfo(i)
		local   kTargetGUID = kTargetInfo:GetTargetGUID()
		local iCurIndex = (STD_INDEX+1)+i
		kActor:SetParam(iCurIndex, kTargetGUID:GetString())
		
		ODS("MultiStrape_Fire : Index:"..(iCurIndex).." 타겟 GUID:"..kTargetGUID:GetString().."\n", false, 912)
		local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)
--		if kTargetPilot:IsNil() == false then
			local actorTarget = kTargetPilot:GetActor()
		--	if false == actorTarget:IsNil() then
				local kTargetPos = actorTarget:GetPos()
				ODS("Index"..iCurIndex.." X:"..kTargetPos:GetX().." Y:"..kTargetPos:GetY().." Z:"..kTargetPos:GetZ().."\n", false, 912)
			--	kNextAction:SetParamAsPoint(1000+i, kTargetPos)
			--end
	--	end
		i = i + 1;
	end
	
	-- 타겟 인덱스 초기화
	kActor:SetParam("MULTISTRAPE_TARGET_INDEX", tostring(STD_INDEX))	
	kActor:SetParam("MULTISTRAPE_TARGET_REMAIN", tostring(iTargetCount))
	
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
		return true;
	end
	
	if kNextAction:GetEnable() == false then
		if curParam == actionID then
			kCurAction:SetParam(0,"");
		end
	end
	
	if kNextAction:GetEnable() == true then
		kCurAction:SetParam(0,"GoNext");
		return false;
	end
	return false 
end

function Skill_Multi_Strape_DoEffect(kActor)
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
end

function Skill_Multi_Strape_OnEvent(kActor, textKey)		
	return false
end
