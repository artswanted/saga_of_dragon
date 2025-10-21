--[[
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Script들에서 너무도 반복되고 action의 Param에 저장되는 값들이 제각각이라 너무 헷깔린다.
우선적으로 기존에 반복적으로 사용되는 기능들과
Param 값들에 대해 정리하여 늘어 놓고자 한다.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
]]
--[[
///////////////
@ ActionParam @
///////////////
	action:SetParam(0, "GoNext") 		-- 현재 액션이 끝나지 않은 상태에서, 
	                                       OnLeave에 들어온 액션이 전이 가능한 액션이라 판단되면, (이부분에서 "GoNext"를 세팅해줌)
										   액션 종료 수순을 밟을수 읽게 Update로 보냄	
	action:SetParam(1, "end")			-- "end"가 세팅되어야 OnLeave에서 종료 시킴
	action:SetParamInt(2,0) 			-- 몇대 때렸는지 저장해두는 Hit 카운트
	action:SetParam(3,"1") 				-- 문자열로 "0": 캐스팅 완료, "1": 캐스팅 중 (int로 바꾸고 싶지만 기존의 방식과 일단 같은 형태로 간다)
	
	action:GetParamInt(15)  			-- 실제는 1방 데미지인데 이걸 iTotalHit으로 쪼개서 한방에 들어갈 데미지를 저장함	
	
	action:SetParamInt(30,iTotalHit)	-- 총 몇대를 때릴수 있는지 확인 하는값(DB에 들어가있어서 얘를 굳이 써야 할까 싶다)
	
	10000 ~	50000 - 커스텀 용도
	20000 - 몇번이나 공격을 Fire 할수 있는가
	20001 - 현재 공격을 Fire 한 횟수
	60000 - 시점 파티클
	100000, 전이 스킬
///////////////					
@ 파티클 Slot @
///////////////
		7211 - HIT_TARGET_EFFECT_ID		: 타겟이 맞을때 붙는 이펙트
		7212 - FIRE_CASTER_EFFECT_ID	: 공격 동작에 붙여줄 파티클 1
	    7213 - FIRE_CASTER_EFFECT_ID2	: 공격 동작에 붙여줄 파티클 2
		7214 - HIT_CASTER_EFFECT_ID 	: HIT 키에서 캐스터에게 붙일 파티클 표현
		7215 - CASTING_EFFECT_EFFECT_ID : 캐스팅시 붙여줄 파티클
	    7216 - CLEANUP_CASTER_EFFECT_ID : 클린업때 붙여줄 파티클
]]
--이하는 기능 부분 ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

function SkillHelpFunc_InitMaxFireCnt(action)
-- 몇번이나 공격을 Fire 할수 있는가
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	local iMaxFireCnt = action:GetAbil(AT_COUNT)
	if(0 == iMaxFireCnt) then iMaxFireCnt = 1 end	
	action:SetParamInt(20000, iMaxFireCnt)
	return true
end

function SkillHelpFunc_GetMaxFireCnt(action)
-- 몇번이나 공격을 Fire 할수 있는가
	if( false == SkillHelpFunc_IsNotNil(action)) then return 0 end
	local iMaxFireCnt = action:GetParamInt(20000)
	if(0 == iMaxFireCnt) then iMaxFireCnt = 1 end	
	return iMaxFireCnt
end

function SkillHelpFunc_GetCurFireCnt(action)
-- 몇번이나 공격을 Fire 했는가
	if( false == SkillHelpFunc_IsNotNil(action)) then return 0 end
	local iFireCnt = action:GetParamInt(20001)	
	return iFireCnt
end

function SkillHelpFunc_IncCurFireCnt(action, iInc)
if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	local iFireCnt = action:GetParamInt(20001)+iInc	
	local iMaxCnt = SkillHelpFunc_GetMaxFireCnt(action)
	action:SetParamInt(20001, iFireCnt)	
	if(iFireCnt > iMaxCnt) then 
		return false
	end
	return true
end

function SkillHelpFunc_IfCastingCancelThenDoBattleIdle(actor)
-- 캐스팅중 캔슬되었나를 확인하고 대기 상태로 바꾸는 기능. "_CleanUp"에서 사용 한다
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end
	local   action = actor:GetAction()
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	
	if true == SkillHelpFunc_IsCastingState(action) then
		--캐스팅 중에 캔슬된 상태이면
		-- 캐스팅 파티클 제거 하고
		SkillHelpFunc_DetachCastingEffect(actor, action)
		if( false == actor:PlayNext()) then 
		-- 공격 대기 동작으로
			action:SetNextActionName("a_battle_idle")			
		end
		-- 다음update때 바로 전환한다
		action:ChangeToNextActionOnNextUpdate(true,true)
		return true
	end	
	return false
end
	
function SkillHelpFunc_DoDividedDamage(actor, action, iHitCount)
-- 실제는 1방 때린것이지만, 클라이언트에서 여러대 때리는것으로 연출하는 함수
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local kTargetList = action:GetTargetList()
	local iTargetCount = kTargetList:size()
	local iTotalHit = SkillHelpFunc_GetTotalHitCount(action)
	
	if 0 < iTargetCount then
	
		local bWeaponSoundPlayed = false
		local i =0
		while i<iTargetCount do
			local actionResult = action:GetTargetActionResult(i)
			if actionResult:IsNil() == false then
			
				if iHitCount == iTotalHit-1 then
					local iOneDmg = actionResult:GetValue()/(iTotalHit)					
					SkillHelpFunc_SetDividedDamage(action, iOneDmg)	-- 이게 왜 있어야 하는가? 추후에 봐서 사용되는곳이 없으면 닦아버릴것
					actionResult:SetValue( actionResult:GetValue() - iOneDmg*(iTotalHit-1))
				end
			
				local kTargetGUID = action:GetTargetGUID(i)
				local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)
			
				if kTargetPilot:IsNil() == false and actionResult:IsMissed() == false then
					local actorTarget = kTargetPilot:GetActor()
					
					if bWeaponSoundPlayed == false then
						bWeaponSoundPlayed = true
						-- 피격 소리 재생
						SkillHelpFunc_PlayWeaponSound(actor, action, actionResult:GetCritical(), actorTarget, action:GetTargetInfo(i))
					end
					SkillHelpFunc_DefaultDoDamage(actor,actorTarget,actionResult, action:GetTargetInfo(i))
				end
			end
			i=i+1
		end
		
		if iHitCount == iTotalHit-1 then
			--ODS("쪼개진 데미지 막타 적용\n", false, 912)	
			action:GetTargetList():ApplyActionEffects()
		else
			--ODS("쪼개진 데미지 적용중\n", false, 912)
			action:GetTargetList():ApplyOnlyDamage(iTotalHit)
		end
	end
end

function SkillHelpFunc_IsReservedNextAction(kCurAction, kNetxtAction)
	if( false == SkillHelpFunc_NilCheck_Action(kCurAction)) then return false end
	if( false == SkillHelpFunc_NilCheck_Action(kNetxtAction)) then return false end	
	if kCurAction:GetNextActionName() ~= kNetxtAction:GetID() then
		--ODS("예정된 액션:"..kCurAction:GetNextActionName().."현재들어온 액션"..kNetxtAction:GetID().."\n",false, 912)
		return false;
	end
	return true
end

--이하는 상태 설정및 체크 부분 ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

--///////////////////////////////////////////////////--
-- 때린 횟수(HitCount)
function SkillHelpFunc_InitHitCount(action)
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	action:SetParamInt(2,0)
	return true
end

function SkillHelpFunc_SetHitCount(action, iHitCount)
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	action:SetParamInt(2,iHitCount)
	return true
end

function SkillHelpFunc_GetHitCount(action)
	if( false == SkillHelpFunc_IsNotNil(action)) then return -1 end
	return action:GetParamInt(2)
end

function SkillHelpFunc_IncHitCount(action, iVal)
	if( false == SkillHelpFunc_IsNotNil(action)) then return -1 end
	local iNewHitCnt = SkillHelpFunc_GetHitCount(action)+iVal		-- 이런 연속적인 호출이 변수값을 온건히 유지 할수 있을까?
	if(true == SkillHelpFunc_SetHitCount(action, iNewHitCnt)) then
		return iNewHitCnt
	end
	return -1
end

--///////////////////////////////////////////////////--
-- 총 때려야할 횟수(TotalHit)
function SkillHelpFunc_GetTotalHitCount(action)
	if( false == SkillHelpFunc_IsNotNil(action)) then return 0 end
	local kSkillDef = GetSkillDef(action:GetActionNo())	
	if kSkillDef:IsNil() == false then
		return kSkillDef:GetAbil(AT_COUNT)
	end	
	return 0
end

--///////////////////////////////////////////////////--
-- 한방 데미지를 여러방으로 연출할 경우 한방 데미지 값
function SkillHelpFunc_SetDividedDamage(action, iOneDmg)
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	action:SetParamInt(15,iOneDmg)
	return true
end

function SkillHelpFunc_GetDividedDamage(action)
	if( false == SkillHelpFunc_IsNotNil(action)) then return 0 end
	return action:GetParamInt(15)
end

--///////////////////////////////////////////////////--
-- 캐스팅 상태
function SkillHelpFunc_SetCastingState(action)
-- 캐스팅 하는 상태 값으로 설정
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	action:SetParam(3,"1")
	return true
end	

function SkillHelpFunc_SetNoneCastingState(action)
-- 캐스팅 안하는 상태 값으로 설정
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	action:SetParam(3,"0")
	return true
end

function SkillHelpFunc_IsCastingState(action)
-- 캐스팅중인가(캐스팅중에 종료되었는가 확인하려는 용도)?
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	if("1" == action:GetParam(3)) then
		return true
	end
	return false
end	

--///////////////////////////////////////////////////--
-- 4차 직업 스킬중 화면에 보이는 커맨드 입력 갯수가 몇개나 되는가?
function SkillHelpFunc_GetTranslateCmdCnt(action)
	if( false == SkillHelpFunc_IsNotNil(action)) then return 0 end
	local iCmdCnt_InputedByDeveloper = action:GetAbil(AT_TRANSLATE_SKILL_CMD_CNT) -- 직접 입력된 커맨드 카운트
	----ODS("iCmdCnt_InputedByDeveloper:"..iCmdCnt_InputedByDeveloper.."\n", 0, 912)
	local iTranslateSkillCnt = action:GetTranslateSkillCnt()					  -- 전이될수 있는 스킬들의 종류	
	----ODS("iCmdCnt_InputedByDeveloper:"..iCmdCnt_InputedByDeveloper.." iTranslateSkillCnt:"..iTranslateSkillCnt.."\n", 0, 912)
	
	if(iTranslateSkillCnt >= iCmdCnt_InputedByDeveloper) then
		return iTranslateSkillCnt												  -- 커맨드의 갯수는 최소한 전이될 스킬들의 수가 되어야함
	end
	
	return iCmdCnt_InputedByDeveloper
end

-- 이함수는 추가 작업하면서 변형이 있을것임, 현재는 처리 명목상 생성
function SkillHelpFunc_GetNextTranslateActionName(action, iCorrectCmdCnt)	
	if( false == SkillHelpFunc_IsNotNil(action)) then return "" end
	----ODS("SkillHelpFunc_GetNextTranslateActionName\n", 0, 912)
	local iTotalCmdCnt = SkillHelpFunc_GetTranslateCmdCnt(action)	--전체 커맨드 중에
	----ODS("iTotalCmdCnt:"..iTotalCmdCnt.."\n", 0, 912)
	
	if(0 < iTotalCmdCnt) then
		----ODS("if문 통과\n", 0, 912)
		local iTotalTranslatSkillCnt = action:GetTranslateSkillCnt() 	-- 전이될수 있는 스킬이 몇개나 되는가		
		if(iTotalTranslatSkillCnt == 0) then iTotalTranslatSkillCnt = 1 end
		local fUnitSkillRate = 1/iTotalTranslatSkillCnt	 				-- 전이될 스킬을 결정하기 위해 비율(/전체 스킬수
		
		local fCmdCorrectRate = iCorrectCmdCnt/iTotalCmdCnt			 	-- 커맨드의 몇%가 정확하게 입력했는가
		local i =0
		while(i < iTotalTranslatSkillCnt) do
			local fSkillRate = (i+1)*fUnitSkillRate
			if(fSkillRate > fCmdCorrectRate) then
				local iTranslateActionNo = action:GetAbil(AT_TRANSLATE_SKILL01+i)
				if(0 < iTranslateActionNo) then
					local kSkillDef = GetSkillDef(iTranslateActionNo);
					if kSkillDef:IsNil() == false then
						----ODS("여기 들어옴\n", 0, 912)
						local kActionName = kSkillDef:GetActionName()						
						return 	kActionName:GetStr()-- 실행 되어야 할 스킬
					end
					break
				end
			end
			i = i+1
		end		
	end
	return ""
end

--이하는 연출 부분 ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

--/////////////////////////////////////////////////////////////////////////////////////////////////////////--
-- Particle 표현 부분
function SkillHelpFunc_AttachParticleToNode(iSlot, ParticleScriptParam, TargetNodeScriptParam, iScale, actor, action)
-- 타겟 노드에 파티클 붙이기
	if(false == SkillHelpFunc_CheckParam(iSlot, ParticleScriptParam, TargetNodeScriptParam, iScale, actor, action))  then
		return false
	end
	
	local   kEffectID = action:GetScriptParam(ParticleScriptParam)
    local   kEffectTargetNodeID = action:GetScriptParam(TargetNodeScriptParam)
    
    if kEffectID ~= "" and kEffectTargetNodeID ~= "" then
    
        if kEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(actor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            
            actor:AttachParticleToPointS(iSlot,kDir,kEffectID, iScale)
            
        else
            actor:AttachParticleS(iSlot,kEffectTargetNodeID,kEffectID, iScale)
        end    
    end	
	return true
end


function SkillHelpFunc_AttachParticleToNodePos(iSlot, ParticleScriptParam, TargetNodeScriptParam, iScale, actor, action)
-- 타겟 노드 위치에 파티클 붙이기
	if(false == SkillHelpFunc_CheckParam(iSlot, ParticleScriptParam, TargetNodeScriptParam, iScale, actor, action))  then
		return false
	end
	
	local   kEffectID = action:GetScriptParam(ParticleScriptParam)
    local   kEffectTargetNodeID = action:GetScriptParam(TargetNodeScriptParam)
	--ODS("SkillHelpFunc_AttachParticleToNodePos-ffectID:"..kEffectID.."\n", false, 912)
    --ODS("SkillHelpFunc_AttachParticleToNodePos-TargetNode:"..kEffectTargetNodeID.."\n", false, 912)
	
    if kEffectID ~= "" and kEffectTargetNodeID ~= "" then
    --[[[
        if kEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(actor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            
            actor:AttachParticleToPointS(iSlot,kDir,kEffectID, iScale)
        ]]    
        --else	
			actor:AttachParticleToPointS(iSlot, actor:GetNodeWorldPos(kEffectTargetNodeID),kEffectID, iScale)
        --end   
    end	
	return true
end

function SkillHelpFunc_ExpressCastingEffect(actor, action)
-- 캐스팅시작시 파티클 표현
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	
	SkillHelpFunc_SetNoneCastingState(action)
	
	local   kCastingEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID")
    local   kCastingEffectTargetNodeID = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE")
	local   kCastingEffectTargetNodeIDToPos = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE_TO_WORLD_POS")
    
    if kCastingEffectID ~= "" and kCastingEffectTargetNodeID ~= "" or kCastingEffectTargetNodeIDToPos ~= "" then
    
		local iSize = 1.0
		if "TRUE" == action:GetScriptParam("CASTING_EFFECT_SIZE_TO_SKILLRANGE") then		
			local iFactor = 100
			local kFactor = action:GetScriptParam("CASTING_EFFECT_SIZE_TO_SKILLRANGE_FACTOR")
			if("" ~= kFactor) then
				iFactor = tonumber(kFactor)				
			end	
			iSize = iSize * (action:GetSkillRange(0,actor) / iFactor)
		end
		
        if kCastingEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(actor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            
            actor:AttachParticleToPointS(7215,kDir,kCastingEffectID, iSize)
            
        elseif kCastingEffectTargetNodeID ~= "" then
            actor:AttachParticleS(7215,kCastingEffectTargetNodeID,kCastingEffectID,iSize)
		else
			actor:AttachParticleToPointS(7215,actor:GetNodeWorldPos(kCastingEffectTargetNodeIDToPos),kCastingEffectID,iSize)
        end	
		SkillHelpFunc_SetCastingState(action)
    end
	
	return true
end

function SkillHelpFunc_ExpressFireEffect(actor, action)
-- 공격 할때 파티클 표현
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	
	local   kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID")
    local   kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE")
	local   kFireEffectTargetNodeIDToPos = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS")
	local   kFireEffectTargetNodeIDToPosWithRotate = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE")

    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= ""  or kFireEffectTargetNodeIDToPosWithRotate ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(actor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            
            actor:AttachParticleToPoint(7212,kDir,kFireEffectID)            
        elseif kFireEffectTargetNodeID ~= "" then
            actor:AttachParticle(7212,kFireEffectTargetNodeID,kFireEffectID)
		elseif kFireEffectTargetNodeIDToPos ~= "" then
			actor:AttachParticleToPoint(7212,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID)
		else
		    actor:AttachParticleToPointWithRotate(7212,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPosWithRotate), kFireEffectID, actor:GetRotateQuaternion())
        end
    
    end	

    kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID2")
    kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE2")
	
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(actor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            
            actor:AttachParticleToPoint(7213,kDir,kFireEffectID)
            
        else
            actor:AttachParticle(7213,kFireEffectTargetNodeID,kFireEffectID)
        end    
    end
	
	return true
end

function SkillHelpFunc_DetachFireEffectWhenSkillEnd(actor, action)
-- 스킬이 끝날때 공격 동작 파티클 제거
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	
	if action:GetScriptParam("DETACH_FIRE_EFFECT_AT_FINISH") == "TRUE" then	
		return SkillHelpFunc_DetachFireEffect(actor, action)
	end
	
	return false
end
function SkillHelpFunc_DetachFireEffect(actor, action)
-- 공격 동작 파티클 제거
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	
	local DefaultDetaChID = false
	local DefaultDetaChID2 = false
	
	if action:GetScriptParam("FIRE_CASTER_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
		DefaultDetaChID = true
	end
	if action:GetScriptParam("FIRE_CASTER_EFFECT_ID2_DEFAULT_DETACH") == "TRUE" then
		DefaultDetaChID2 = true
	end
    actor:DetachFrom(7212,DefaultDetaChID)
    actor:DetachFrom(7213,DefaultDetaChID2)
	return true
end

function SkillHelpFunc_DetachCastingEffectWhenSkillEnd(actor, action)
-- 스킬이 끝날때 캐스팅 파티클 제거
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	-- [DETACH_CASTING_EFFECT_AT_FINISH] : 스킬이 끝날때 CASTING_EFFECT_EFFECT_ID 를 제거할 것인가
	if action:GetScriptParam("DETACH_CASTING_EFFECT_AT_FINISH") == "TRUE" then
		return SkillHelpFunc_DetachCastingEffect(actor, action)		
	end
	return false
end

function SkillHelpFunc_DetachCastingEffect(actor, action)
-- 캐스팅 파티클 제거
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	local DefaultDetaChID = false
	-- [CASTING_EFFECT_ID_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
	if action:GetScriptParam("CASTING_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
		DefaultDetaChID = true
	end
	actor:DetachFrom(7215,DefaultDetaChID)
	return true
end

function SkillHelpFunc_ExpressCleanUpEffect(actor, action)
-- CleanUp시(스킬 종료시) 표현할 파티클
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	
	local   kCleanUpEffectID = action:GetScriptParam("CLEANUP_CASTER_EFFECT_ID")
    local   kCleanUpEffectTargetNodeID = action:GetScriptParam("CLEANUP_CASTER_EFFECT_TARGET_NODE")
	local	fCleanUpScale = tonumber(action:GetScriptParam("CLEANUP_CASTER_EFFECT_SCALE"))

	if fCleanUpScale == 0 or fCleanUpScale == nil then
		fCleanUpScale = 1.0
	end
    
    if kCleanUpEffectID ~= "" and kCleanUpEffectTargetNodeID ~= "" then
		 actor:AttachParticleS(7216,kCleanUpEffectTargetNodeID,kCleanUpEffectID, fCleanUpScale)
		 return true
	end
	
	return false
end

function SkillHelpFunc_ExpressHitTargetEffect(actor, action, actorTarget, bCri)
--타겟이 맞을때 붙는 이펙트
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	if( false == SkillHelpFunc_IsNotNil(actorTarget)) then return false end
	
	local   kHitTargetEffectTargetNodeID = action:GetScriptParam("HIT_TARGET_EFFECT_TARGET_NODE");
	local DamageEff = ""
	if false == bCri then	--크리 아닐때
		local   kHitTargetEffectID = action:GetScriptParam("HIT_TARGET_EFFECT_ID");
		if kHitTargetEffectID ~= "" and kHitTargetEffectTargetNodeID ~= "" then
			DamageEff = kHitTargetEffectID
		end
	else--크리일때
		local   kCriticlaHitTargetEffectID = action:GetScriptParam("CRITICAL_HIT_TARGET_EFFECT_ID");
		if kCriticlaHitTargetEffectID ~= "" then
			DamageEff = kCriticlaHitTargetEffectID
		end
	end
    if DamageEff ~= "" and kHitTargetEffectTargetNodeID ~= "" then    
        actorTarget:AttachParticle(7211,kHitTargetEffectTargetNodeID,DamageEff)
		return true
    end
	return false
end


function SkillHelpFunc_ExpressHitCasterEffect(actor, action)
-- 때릴때 캐스터에게 붙일 파티클 표현
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
		
    local   kHitEffectID = action:GetScriptParam("HIT_CASTER_EFFECT_ID")
    local   kHitEffectTargetNodeID = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE")
	local   kHitEffectTargetNodeIDToPos = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS")
	local   kHitEffectTargetNodeIDToPosWithRotate = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE")
    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" or kHitEffectTargetNodeIDToPos ~= ""  or kHitEffectTargetNodeIDToPosWithRotate ~= "" then
		if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
			actor:AttachParticle(7214,kHitEffectTargetNodeID,kHitEffectID)
		elseif kHitEffectTargetNodeID ~= "" then
	           actor:AttachParticle(7214,kHitEffectTargetNodeID,kHitEffectID)
		elseif kHitEffectTargetNodeIDToPos ~= "" then
			actor:AttachParticleToPoint(7214,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPos),kHitEffectID)			
		else
		    actor:AttachParticleToPointWithRotate(7214,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPosWithRotate), kHitEffectID, actor:GetRotateQuaternion())
	    end
		return true
	end
	
	return false
end

function SkillHelpFunc_DetachHitCasterEffect(actor)
-- 때릴때 캐스터에게 붙은 파티클 제거
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	actor:DetachFrom(7214)
	return true 
end
function SkillHelpFunc_ChangeWeapon(actor, action)
-- 무기 모양을 바꾼다
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"))
		--ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n")
	end		
	return true
end

function SkillHelpFunc_RestoreWeapon(actor, action)
-- 바꾼 무기 모양을 복구한다
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON)
	end
	return true
end

function SkillHelpFunc_HideWeapon(actor, action)
-- 무기를 감춘다
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	--ODS("SkillHelpFunc_HideWeapon\n", false, 912)
	if action:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, true)
		--ODS("무기 감춤\n", false, 912)
	end
	return true
end

function SkillHelpFunc_ShowWeapon(actor, action)
-- 감춘 무기를 보여준다
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	if action:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, false)
	end
end

function SkillHelpFunc_AttachWeaponTrail(actor, action)
-- 무기의 꼬리 파티클을 연결
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	if action:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:StartWeaponTrail()
	end
	return true
end

function SkillHelpFunc_DetachWeaponTrail(actor, action)
-- 무기의 꼬리 파티클을 제거함
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	if action:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:EndWeaponTrail()
	end
	return true
end

function SkillHelpFunc_TransformShape(actor, action)
--  변신 적용
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	
    local kTransformXMLID = action:GetScriptParam("TRANSFORM_XML")
    if kTransformXMLID == "" then
		return false
	end
    
    local   iTransformEffectID = tonumber(action:GetScriptParam("TRANSFORM_ID"))
     if nil == iTransformEffectID then
		return false
	 end
	
    local kMan = GetStatusEffectMan()
    kMan:AddStatusEffectToActor(actor:GetPilotGuid(),kTransformXMLID,iTransformEffectID,0)
    
	return true
end

function SkillHelpFunc_RestoreShape(actor, action)
--  변신 복구
   if( false == SkillHelpFunc_IsNotNil(actor)) then return false end
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	
    local kTransformXMLID = action:GetScriptParam("TRANSFORM_XML")
    if kTransformXMLID == "" then
		return false
	end
    
       
    local   iTransformEffectID = tonumber(action:GetScriptParam("TRANSFORM_ID"))
     if nil == iTransformEffectID then
		return false
	 end
	--  모습 원래대로 (이걸 사용한후 curAction을 얻어와 쓰면 절대로 안됨!!!)
    local kMan = GetStatusEffectMan()
    kMan:RemoveStatusEffectFromActor(actor:GetPilotGuid(),iTransformEffectID)
	
	return true
end
--/////////////////////////////////////////////////////////////////////////////////////////////////////////--
-- SOUND

function SkillHelpFunc_PlayCastingSound(actor, action)
-- 캐스팅 사운드 표현
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	
	--일반 사운드
	local	kSoundID = action:GetScriptParam("CASTING_SOUND_ID")
	if kSoundID~="" then
		actor:AttachSound(2785,kSoundID)
	end
	
	if 1 == actor:GetAbil(AT_GENDER) then
	--남성용 사운드
		kSoundID = action:GetScriptParam("CASTING_SOUND_ID_MALE")
		if kSoundID~="" then
			actor:AttachSound(2786,kSoundID)
			--ODS("남자사운드\n", false, 912)
		end	
	
	else
	--여성용 사운드
		kSoundID = action:GetScriptParam("CASTING_SOUND_ID_FEMALE")
		if kSoundID~="" then
			actor:AttachSound(2786,kSoundID)
			--ODS("여자사운드\n", false, 912)
		end	
	end
	
	return true
end

function SkillHelpFunc_PlayFireSound(actor, action)
-- 공격할때 사운드
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	
	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID")
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID)
	end	
	
	--남성용 사운드
	if 1 == actor:GetAbil(AT_GENDER) then
		local	kSoundID2 = action:GetScriptParam("FIRE_SOUND_ID_MALE")
		if kSoundID2~="" then
			actor:AttachSound(2785,kSoundID2)
		end	
	--여성용 사운드
	else
		local	kSoundID2 = action:GetScriptParam("FIRE_SOUND_ID_FEMALE")
		if kSoundID2~="" then
			actor:AttachSound(2785,kSoundID2)
		end	
	end
	
	return true
end 
	
function SkillHelpFunc_AttachSoundToActorPos(SoundIDScriptParam, actor, action)
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	if( false == SkillHelpFunc_NilCheck_World()) then return false end
	if(nil == SoundIDScriptParam) then return false end
	--ODS("SkillHelpFunc_AttachSoundToActorPos-SoundIDScriptParam:"..SoundIDScriptParam.."\n", false, 912)
	local	kSoundID = action:GetScriptParam(SoundIDScriptParam)
	if kSoundID ~= "" then	
		--ODS("SkillHelpFunc_AttachSoundToActorPos-kSoundID:"..kSoundID.."\n", false, 912)
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AttachSound(kSoundID, actor:GetPos())
		--ODS("SkillHelpFunc_AttachSoundToActorPos 성공\n", false, 912)
		return true
	end
	return false
end

function SkillHelpFunc_PlayHitSound(actor, action)
-- 때릴때 사운드
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	local	kSoundID = action:GetScriptParam("HIT_SOUND_ID")
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID)
	end

	if 1 == actor:GetAbil(AT_GENDER) then
	--남성용 사운드
		local	kSoundID2 = action:GetScriptParam("HIT_SOUND_ID_MALE")
		if kSoundID2~="" then
			actor:AttachSound(2785,kSoundID2)
		end		
	else
	--여성용 사운드
		local	kSoundID2 = action:GetScriptParam("HIT_SOUND_ID_FEMALE")
		if kSoundID2~="" then
			actor:AttachSound(2785,kSoundID2)
		end	
	end
	
	return true
end

function SkillHelpFunc_PlayWeaponSound(actor, action, bCritical, TargetActor, actionTargetInfo)
-- 피격소리를 재생하는 부분
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	local actionName = action:GetID()
	if bCritical then
		actionName = "Critical_dmg"
	end
    TargetActor:PlayWeaponSound(WST_HIT, actor, actionName, 0, actionTargetInfo)
	return true
end


function SkillHelpFunc_PlaySound(actor, action, SoundScriptParam)
-- 캐스팅 사운드 표현
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	if(nil == SoundScriptParam) then return false end
	
	--일반 사운드
	local	kSoundID = action:GetScriptParam(SoundScriptParam)
	if kSoundID~="" then
		actor:AttachSound(2785,kSoundID)
	end
	
	if 1 == actor:GetAbil(AT_GENDER) then
	--남성용 사운드
	
		kSoundID = action:GetScriptParam(SoundScriptParam.."_MALE")
		if kSoundID~="" then
			actor:AttachSound(2786,kSoundID)
			--ODS("남자사운드\n", false, 912)
		end	
	
	else
	--여성용 사운드
		kSoundID = action:GetScriptParam(SoundScriptParam.."_FEMALE")
		if kSoundID~="" then
			actor:AttachSound(2786,kSoundID)
			--ODS("여자사운드\n", false, 912)
		end	
	end
	
	return true
end	

function SkillHelpFunc_StopSound(actor, action, SoundScriptParam)
-- 캐스팅 사운드 표현
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end	
	if(nil == SoundScriptParam) then return false end
	
	--일반 사운드
	local	kSoundID = action:GetScriptParam(SoundScriptParam)
	if kSoundID~="" then
		StopSoundByID(kSoundID)
	end
	
	if 1 == actor:GetAbil(AT_GENDER) then
	--남성용 사운드
	
		kSoundID = action:GetScriptParam(SoundScriptParam.."_MALE")
		if kSoundID~="" then
			StopSoundByID(kSoundID)
			--ODS("남자사운드 멈춤\n", false, 912)
		end	
	
	else
	--여성용 사운드
		kSoundID = action:GetScriptParam(SoundScriptParam.."_FEMALE")
		if kSoundID~="" then
			StopSoundByID(kSoundID)
			--ODS("여자사운드 멈춤\n", false, 912)
		end	
	end
	
	return true
end	
--/////////////////////////////////////////////////////////////////////////////////////////////////////////--
-- 배경
function SkillHelpFunc_MakeDarkBG(actor)
-- 배경 어둡게
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_NilCheck_World()) then return false end
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 0.7, 0.4,true,true)
	end
	return true
end

function SkillHelpFunc_RemoveDarkBG(actor)
-- 배경 바꾼거(어둡게,) 원상태로 복구
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_NilCheck_World()) then return false end
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false)
	end
	return true
end

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
-- -- -- 이하는 SkillHelpFunc에서 반복는 부분을 함수로 만들어놓은 부분  -- -- -- -- 
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 


function SkillHelpFunc_CheckParam(iSlot, ParticleScriptParam, TargetNodeScriptParam, iScale, actor, action)
--기본적인 파라메터 체크해주는 함수
	if( false == SkillHelpFunc_IsNotNil(actor)) then return false end	
	if( false == SkillHelpFunc_IsNotNil(action)) then return false end
	
	if(nil == ParticleScriptParam) then return false end
	if(nil == TargetNodeScriptParam) then return false end	
	--ODS("SkillHelpFunc_CheckParam-ParticleScriptParam:"..ParticleScriptParam.."\n", false, 912)
    --ODS("SkillHelpFunc_CheckParam-TargetNodeScriptParam:"..TargetNodeScriptParam.."\n", false, 912)
	
	if(0 > iSlot) then return false end
	return true
end

function SkillHelpFunc_IsNotNil(kObj)
	if(nil == kObj) then 
		--ODS("nil임\n", false, 912)
		SetBreak()
		return false 
	end
	
	if(true == kObj:IsNil()) then 
		--ODS("NULL임\n", false, 912)
		SetBreak()
		return false 
	end
		
	return true
end

function SkillHelpFunc_NilCheck_World()
	if(nil == g_world) then return false end
	return true
end