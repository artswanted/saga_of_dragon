-- [FIRE_CASTER_EFFECT_ID] : 스킬이 실제 발동될때 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS] : 스킬이 실제 발동될때 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [CASTING_EFFECT_EFFECT_ID] : 캐스팅 시에 이펙트 붙일 ID
-- [FIRE_SOUND_ID] : 시전시에 플레이 할 사운드 ID
function Skill_Dragon_Dive_OnCastingCompleted(actor,action)
end
	
function Skill_Dragon_Dive_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	local weapontype = actor:GetEquippedWeaponType()
	
	if weapontype == 0 then
		return	false
	end

	if actor:IsMeetFloor() then
		AddWarnDataStr(GetTextW(239), 2, true)
		return	false
	end
	
	if actor:GetFloatHeight(70)<70 then
		return	false	
	end
	
	return		true	
end

function Skill_Dragon_Dive_OnEnter(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
--	local	actorID = actor:GetID()
--	local	actionID = action:GetID()
--	local	weapontype = actor:GetEquippedWeaponType()
	local   kEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
	if kEffectID ~= "" then
		local kDir = actor:GetPos();
		kDir:SetZ(kDir:GetZ()-28);
		actor:AttachParticleToPoint(201,kDir, kEffectID)
	end

	--actor:AttachParticle(204,"char_root", "ef_skill_Ascension_Dragon_02_char_root");
	
	local kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
	
	actor:StopJump()
	actor:FreeMove(true)
	action:SetParamInt(0, -1)
	action:SetParamFloat(2,300)	-- Drop Speed!
	actor:StartWeaponTrail()
	action:SetParamAsPoint(10,actor:GetPos())
	
	action:SetParamInt(15, action:GetAbil(AT_COUNT)) -- 총 몇대를 타격 할것인가
	action:SetParamFloat(16, 0.7)
	action:SetParamInt(17, 0)
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(100, g_world:GetAccumTime())	-- 시작 시간
	action:SetParamAsPoint(101,actor:GetPos())			-- 시작 위치

	action:SetParamFloat(103,-360.0*DEGTORAD)		-- 초당 회전력 / 일반 점프일떄 360정도가 맞음
	action:SetParamAsPoint(200, actor:GetLookingDir())
	
	action:SetParamFloat(300, action:GetAbil(AT_SKILL_MAXIMUM_SPEED)) -- g_fMaxLength = 300	
	action:SetParamFloat(301, action:GetParamFloat(300)) -- g_fFrontLenth =g_fMaxLength	
	action:SetParamFloat(302, 1) -- g_fDownLenth = 1	
	action:SetParamFloat(303, 100) -- g_fDelta = 10	
	
	action:SetParamFloat(400, 1000)
	
	actor:PlayCurrentSlot()

	if actor:IsMyActor() then
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 0.7, 0.4,true,true);
	end

	return true
end

function Skill_Dragon_Dive_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local iAttackRange = action:GetSkillRange(0,actor)
	
	if iAttackRange == 0 then
		iAttackRange = 70
	end
	
	-- 이하 부분 적절한 값을 찾아 넣을것
	local	kPos = action:GetParamAsPoint(10)	
	local kParam = FindTargetParam()
	kParam:SetParam_1(kPos,Point3(0,0,-1));
	kParam:SetParam_2(400,iAttackRange*2,0,0);
	kParam:SetParam_3(true,FTO_NORMAL+FTO_BLOWUP);
	return action:FindTargetsEx(iActionNo,TAT_BAR,kParam,kTargets,kTargets);	
end

function Skill_Dragon_Dive_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
--	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	if actor:IsMeetFloor() == true then
		action:SetParam(1, "end")
		return false
	end
	
	local	animDone = actor:IsAnimationDone()
	local	slotNum = action:GetCurrentSlot()
	local	iState = action:GetParamInt(0)
	--ODS("******Slot :"..slotNum.."*********\n", false, 912)
	
	local kDownVec = Point3(0,0,-1)
	local kFrontVec = actor:GetLookingDir()
	local fMaxLenth = action:GetParamFloat(300)	
	
--	ODS("앞으로!! : "..fFrontLenth.."\n", false, 912)
	--ODS("밑으로!! : "..fDownLength.."\n", false, 912)

	if iState == -1 then 
		if animDone == true then
			actor:PlayNext()
			action:SetParamInt(0, 0)
			Skill_Dragon_Dive_Fire(actor,action)
		else
			--키 입력에따른 속력 조절
			-- if actor:IsMyActor() then			
				-- local iMetaDist = 0
				-- local fScalar = action:GetParamFloat(400)
				-- if(KeyIsDown(KEY_DOWN)) then 
					-- iMetaDist = fScalar* frameTime					
				-- elseif(KeyIsDown(KEY_UP)) then
					-- iMetaDist = -(fScalar*frameTime)/2				
				-- end
				-- if iMetaDist ~= 0 then
					-- fMaxLenth =  fMaxLenth+iMetaDist
					-- action:SetParamFloat(300, fMaxLenth)
				-- end
			--end	
		end
	end
	--ODS("fMaxlenth:"..fMaxLenth.."\n", false, 912)
	local fFrontLenth = action:GetParamFloat(301)
	local fDownLength = action:GetParamFloat(302, 1)
	local fDownLenth_local = (fMaxLenth-fFrontLenth)
	local fFrontLenth_local = (fMaxLenth-fDownLength)
	
	if iState == 0 then

		local	kLookingDir = actor:GetLookingDir()
		local	kRotateAxis = actor:GetLookingDir()
		--local fSpeed = action:GetParamFloat(2)
				
		kRotateAxis:Cross(Point3(0,0,1))
		kRotateAxis:Unitize()
		local	fRotateAngle = action:GetParamFloat(103)* frameTime
		--ODS("각도:"..fRotateAngle*(180/math.pi).."\n", false, 912)
		
		kLookingDir = action:GetParamAsPoint(200)
		--local	kMovingDelta = kLookingDir:_Multiply(fSpeed)

		local	kMovingDelta = kLookingDir:_Multiply(fFrontLenth_local)
		
		
		
		local kDownVec = Point3(0,0,-1)
		kDownVec:Multiply(fDownLenth_local)
		kMovingDelta:Rotate(kRotateAxis,fRotateAngle)
		kMovingDelta:Add(kDownVec)		
		
		kFrontVec:Multiply(fFrontLenth_local/2)
		kMovingDelta:Add(kFrontVec)		
		--ODS("총 속력 : "..kMovingDelta:Length().."\n", false, 912)
		actor:SetMovingDelta(kMovingDelta)
		
		-- 회전된 축 저장
		kMovingDelta:Unitize()		
		--플레이어가 바라보는 방향(로컬 개념상 (1,0,0)), 회전된 축의 내적이 0이하 이면 무빙 델타 바꿈
		kLookingDir = actor:GetLookingDir()
		local fDot = kLookingDir:Dot(kMovingDelta)
--		fDot = math.acos(fDot)		
--		fDot = fDot*(180/math.pi)
		--ODS("내적:"..fDot.."\n", false, 912)
		if( 0 >= fDot) then
			kMovingDelta = Point3(0,0,-1)
			kMovingDelta:Multiply(fMaxLenth)
			actor:SetMovingDelta(kMovingDelta)
			--action:SetParamFloat(2, 1000)
			
			--ODS("---------------------------------------------공격 동작 그만---------------------------------------------\n", false, 912)
		--	actor:PlayNext()	-- 점프 모션으로 바꾸는게 어떨까
		--	action:SetParamInt(0,2)			
		else
--			kMovingDelta:_Multiply(fSpeed)
			action:SetParamAsPoint(200, kMovingDelta)			
			--ODS("=============================================적용됨=============================================\n", false, 912)
		end		
		
		local fDelta = action:GetParamFloat(303)
		fDelta = fDelta+fDelta*frameTime
		action:SetParamFloat(303, fDelta) -- g_fDelta =g_fDelta+g_fDelta*frameTime
		
		action:SetParamFloat(301, fMaxLenth-fDelta) -- g_fFrontLenth = fMaxLenth-g_fDelta				
		fFrontLenth = action:GetParamFloat(301)
		
		action:SetParamFloat(302, fMaxLenth-fFrontLenth)
		fDownLength = action:GetParamFloat(302)			--g_fDownLenth  = fMaxLenth-fFrontLenth
	end

	return true
end

function Skill_Dragon_Dive_OnCleanUp(actor)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	actor:FreeMove(false)
	actor:EndWeaponTrail()

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end	

	return true
end

function Skill_Dragon_Dive_OnLeave(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local	curAction = actor:GetAction()
			
	CheckNil(nil==curAction)
	CheckNil(curAction:IsNil())
	
	local	curParam = curAction:GetParam(1)
	local	actionID = action:GetID()
	
	ODS("Skill_Dragon_Dive_OnLeave actionID:"..actionID.."\n")
	
	if actor:IsMyActor() == false then
		return true
	end

	if actionID == "a_jump" then 
		return false
	end
	
	if action:GetActionType()=="EFFECT" then
		return true
	end

	if curParam == "end" then 
		return true
	end	

	if action:GetEnable() == false then
		
		if curParam==actionID then
			curAction:SetParam(1,"")
		end
		
	end	
	
	if action:GetEnable() == true then
		
		if actionID == "a_run" or 
			string.sub(actionID,1,7) == "a_melee" or 
			string.sub(actionID,1,11) == "a_thi_melee" then
	
			curAction:SetParam(3,"GoNext")
			curAction:SetParam(1,actionID)	
			
		elseif actionID == "a_telejump" then
			actor:DetachFrom(32)
			return true
		end
	
		return false
	
	elseif curParam == "end" and actionID == "a_run" then
		return true
	elseif actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		return true
	end
	return false 
end

function Skill_Dragon_Dive_Fire(actor,action)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())	
	
	local	iTotalHit = action:GetParamInt(15) 
	if iTotalHit == nil or iTotalHit == 0 then
	    iTotalHit = 1;
	end
	
	local	iHitCount = action:GetParamInt(17)
	if iHitCount >= iTotalHit then
		return
	end
			ODS("iHitCount:"..iHitCount.." 최대Hit:"..iTotalHit.."\n", false, 912)
	
	local   kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID")
    local   kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE")
	
	if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
		actor:AttachParticleToPointWithRotate(202,actor:GetPos(),kFireEffectID,actor:GetRotateQuaternion(),1.0)
	end
	
	-- if actor:IsMyActor() then
	    -- GetComboAdvisor():OnNewActionEnter(action:GetID())
		 --action:StartTimer(0.5,0.1,0)
	    --GetComboAdvisor():AddNextAction("a_jump")
    -- end
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
--[[		local totalTime = action:GetParamFloat(16);
		local termTime = totalTime / (action:GetParamInt(15) + 1);
		action:StartTimer(totalTime,termTime,0);
		]]
		 action:StartTimer(0.3,0.05,0)
    end

end

function Skill_Dragon_Dive_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
     if bIsBefore == false then		
		Skill_Dragon_Dive_HitOneTime(actor,action)  
	end

end

function Skill_Dragon_Dive_HitOneTime(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iHitCount = action:GetParamInt(17);
	
	if iHitCount == 0 then
	
	    local   kHitEffectID = action:GetScriptParam("HIT_CASTER_EFFECT_ID");
	    local   kHitEffectTargetNodeID = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE");
		local   kHitEffectTargetNodeIDToPos = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS");
		local   kHitEffectTargetNodeIDToPosWithRotate = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE");

	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" or kHitEffectTargetNodeIDToPos ~= ""  or kHitEffectTargetNodeIDToPosWithRotate ~= "" then
			if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
				actor:AttachParticle(7214,kHitEffectTargetNodeID,kHitEffectID);
			elseif kHitEffectTargetNodeID ~= "" then
	            actor:AttachParticle(7214,kHitEffectTargetNodeID,kHitEffectID);
			elseif kHitEffectTargetNodeIDToPos ~= "" then
				actor:AttachParticleToPoint(7214,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPos),kHitEffectID);			
			else
			    actor:AttachParticleToPointWithRotate(7214,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPosWithRotate), kHitEffectID, actor:GetRotateQuaternion())
		    end
		end
	end

	local	iTotalHit = action:GetParamInt(15) --tonumber(action:GetScriptParam("MULTI_HIT_NUM"));
	if iTotalHit == nil or iTotalHit == 0 then
	    iTotalHit = 1;
	end
	
	if iHitCount >= iTotalHit then
		ODS("힛온 타임 더이상 진행 안함\n", false, 912)
		return
	end
	
	local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end	
	
	--남성용 사운드
	if 1 == actor:GetAbil(AT_GENDER) then
		local	kSoundID2 = action:GetScriptParam("HIT_SOUND_ID_MALE");
		if kSoundID2~="" then
			actor:AttachSound(2785,kSoundID2);
		end	
	--여성용 사운드
	else
		local	kSoundID2 = action:GetScriptParam("HIT_SOUND_ID_FEMALE");
		if kSoundID2~="" then
			actor:AttachSound(2785,kSoundID2);
		end	
	end
		
	local iTargetCount = action:GetTargetCount();
	local i =0;
	if iTargetCount>0 then
		
		local bWeaponSoundPlayed = false;
		
		while i<iTargetCount do
		
			local actionResult = action:GetTargetActionResult(i);
			if actionResult:IsNil() == false then
			
				local kTargetGUID = action:GetTargetGUID(i);
				local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
				if kTargetPilot:IsNil() == false and actionResult:IsMissed() == false then
					
					local actorTarget = kTargetPilot:GetActor();
					
                    if bWeaponSoundPlayed == false then
                        bWeaponSoundPlayed = true;
                        -- 피격 소리 재생
						local actionName = action:GetID();
						if actionResult:GetCritical() then
							actionName = "Critical_dmg"
						end
                        actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, action:GetTargetInfo(i));
                    end
					Act_Melee_Default_DoDamage(actor,actorTarget,actionResult, action:GetTargetInfo(i));
				end
				
			end
			
			i=i+1;
		
		end
	end

	action:GetTargetList():ApplyActionEffects();
	actor:ClearIgnoreEffectList();
	
	iHitCount = iHitCount + 1;
	action:SetParamInt(17,iHitCount);
	ODS(iHitCount.."회 때림\n", false, 912)
	
end

function   Skill_Dragon_Dive_OnTimer(actor,accumtime,action,iTimerID)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	action:CreateActionTargetList(actor)
	action:BroadCastTargetListModify(actor:GetPilot())

	local	iTargetCount = action:GetTargetCount()
	action:SetSlot(1)	
	
	if 	iTargetCount == 0 then
		return true
	end
	
	local hitcount = action:GetParamInt(17);
	if hitcount >= action:GetParamInt(15) then
		return true;
	end
	
	
	local	i = 0
	
	local bWeaponSoundPlayed = false
		
	while i<iTargetCount do
		
		local	kTargetGUID = action:GetTargetGUID(i)
		local	kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)
		if kTargetPilot:IsNil() == false then
			
			local	actorTarget = kTargetPilot:GetActor()
			if actorTarget:IsNil() == false and actorTarget:GetPilotGuid() ~= actor:GetPilotGuid() then
			
				local kActionResult = action:GetTargetActionResult(i)
			
				if kActionResult:IsMissed() == false then
				
	                if bWeaponSoundPlayed == false then
	                    bWeaponSoundPlayed = true
                        -- 피격 소리 재생
                        actorTarget:PlayWeaponSound(WST_HIT, actor, action:GetID(), 0, action:GetTargetInfo(i))
                    end				
				
--					local	kActionTargetInfo = action:GetTargetInfo(i)	
					
					local	iSphereIndex = action:GetTargetABVShapeIndex(i)
					local pt = actorTarget:GetABVShapeWorldPos(iSphereIndex)		
					if kActionResult:GetCritical() then
						actorTarget:AttachParticleToPoint(12, pt, "e_dmg_cri")
					else
						actorTarget:AttachParticleToPoint(12, pt, "e_dmg")
					end

--					local	atAction = actorTarget:GetAction()
					
--					local	guidPilot = actorTarget:GetPilotGuid()
--					local	pilot = g_pilotMan:FindPilot(guidPilot)
					
					local iShakeTime = g_fAttackHitHoldingTime * 1000
						
					-- 충격 효과 적용
					actor:SetShakeInPeriod(5,iShakeTime/2)
					actorTarget:SetShakeInPeriod(5,iShakeTime)		
				
				end
				
			end
		end
		
		i = i + 1
		
	end
	
	if action:GetTargetList():IsNil() == false then
		action:GetTargetList():ApplyActionEffects()
	end
	action:ClearTargetList()
	actor:ClearIgnoreEffectList()

	return true

end