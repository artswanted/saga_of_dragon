
-- Melee
function Skill_Raid_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local weapontype = actor:GetEquippedWeaponType()
	
	if weapontype == 0 then
		return	false
	end

	return		true
	
end
function Skill_Raid_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	Skill_Raid_Fire(actor,action)
	action:SetSlot(action:GetCurrentSlot()+1)
	actor:PlayCurrentSlot()
	action:SetParam(1,"start")
	
	--local	kTargetPos = actor:GetNodeWorldPos("char_root")
	--actor:AttachParticleToPointWithRotate(8,kTargetPos,"ef_ScrewUpper_01_char_root", actor:GetRotateQuaternion())

end


function Skill_Raid_Fire(actor,action)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	ODS("RestoreItemModel Skill_Raid_Fire\n")
	if action:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:StartWeaponTrail()
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(2,g_world:GetAccumTime()) --  시작 시간
	action:SetParamFloat(6,0)
	action:SetParamInt(9,0)	--	Hit Count

	
	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID")
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID)
	end	
	
	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"))
		ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n")
	end
	
	if action:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, true)
	end

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
		
	if action:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:StartWeaponTrail()
	end
	
	--  변신 적용
    local kTransformXMLID = action:GetScriptParam("TRANSFORM_XML")
    if kTransformXMLID ~= "" then
    
        local   iTransformEffectID = tonumber(action:GetScriptParam("TRANSFORM_ID"))
        if iTransformEffectID ~= nil then
        
            local   kMan = GetStatusEffectMan()
            kMan:AddStatusEffectToActor(actor:GetPilotGuid(),kTransformXMLID,iTransformEffectID,0)        
        end
    
    end

end
function Skill_Raid_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor)	
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local  iAttackRange = action:GetSkillRange(0,actor)
	
	ODS("Skill_Raid_OnFindTarget iAttackRange:"..iAttackRange.."\n")
	if iAttackRange == 0 then
	    iAttackRange = 100
	end

	local	kPos = actor:GetPos()
	local	kDir = actor:GetLookingDir()
	local	fBackDistance = 10
	kDir:Multiply(-fBackDistance)
	kPos:Add(kDir)

	local kParam = FindTargetParam()
	
    local iFindTargetType = TAT_SPHERE
	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir())
	kParam:SetParam_2(0,0,iAttackRange,0)
	
	kParam:SetParam_3(true,action:GetAbil(AT_ATTACK_UNIT_POS))
	local iTargets = action:FindTargets(iFindTargetType,kParam,kTargets,kTargets)
	
	return iTargets
end

function Skill_Raid_OnEnter(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType()
	
	ODS("Skill_Raid_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n")
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true
	end

	Skill_Raid_OnCastingCompleted(actor,action)
	
	return true
end
function Skill_Raid_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
    if bIsBefore == false then

        if actor:IsMyActor()  then
			action:GetTargetList():ApplyActionEffects(true,true)
			return
		end
		
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    
    end

end

function Skill_Raid_OnTimer(actor,fAccumTime,action,iTimerID)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	local	iHitCount = action:GetParamInt(9)
	local	iTotalHit = action:GetAbil(AT_COUNT)
	if iHitCount == iTotalHit then	
		return
	end

	if actor:IsUnderMyControl() then
		action:CreateActionTargetList(actor)
		action:BroadCastTargetListModify(actor:GetPilot())
	end
		
	iHitCount = iHitCount + 1
	action:SetParamInt(9,iHitCount)
	
	if actor:IsMyActor() then
		QuakeCamera(0.1,1,0,0.01,10)
	end	
	
	return	true	

end

function Skill_Raid_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local actionID = action:GetID()
	local movingSpeed = 0
	local bAnimDone = actor:IsAnimationDone()
	local iSlot = action:GetCurrentSlot()
	local CurState = action:GetParam(1)
	
	if bAnimDone then
		action:SetParam(1,"end")
		return false
	elseif CurState == "start" then
        action:SetParam(1,"timerSet")
		if actor:IsMyActor() then
			action:StartTimer(1.0,0.09,0)
		end	
	end
	
	return true
end

function Skill_Raid_OnCleanUp(actor)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	local   kCurAction = actor:GetAction()

	if kCurAction:GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON)
	end

	if kCurAction:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, false)
	end

	if kCurAction:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:EndWeaponTrail()
	end
	
	if kCurAction:GetScriptParam("DETACH_FIRE_EFFECT_AT_FINISH") == "TRUE" then
	
		local DefaultDetaChID = false
		local DefaultDetaChID2 = false
		if kCurAction:GetScriptParam("FIRE_CASTER_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID = true
		end

		if kCurAction:GetScriptParam("FIRE_CASTER_EFFECT_ID2_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID2 = true
		end		

	    actor:DetachFrom(7212,DefaultDetaChID)
	    actor:DetachFrom(7213,DefaultDetaChID2)
		actor:DetachFrom(7214)
	end
	
	if kCurAction:GetScriptParam("DETACH_CASTING_EFFECT_AT_FINISH") == "TRUE" then
		local DefaultDetaChID = false
		if kCurAction:GetScriptParam("CASTING_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID = true
		end
		actor:DetachFrom(7215,DefaultDetaChID)
	end
		
	local   kCleanUpEffectID = kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_ID")
    local   kCleanUpEffectTargetNodeID = kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_TARGET_NODE")
	local	fCleanUpScale = tonumber(kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_SCALE"))

	if fCleanUpScale == 0 or fCleanUpScale == nil then
		fCleanUpScale = 1.0
	end
    
    if kCleanUpEffectID ~= "" and kCleanUpEffectTargetNodeID ~= "" then
		 actor:AttachParticleS(7216,kCleanUpEffectTargetNodeID,kCleanUpEffectID, fCleanUpScale)
	end

	if "1" == kCurAction:GetParam(3) then
		--캐스팅 중에 캔슬된 상태
		actor:DetachFrom(7215,true)
		if( false == actor:PlayNext()) then 
			kCurAction:SetNextActionName("a_battle_idle")			
		end
		kCurAction:ChangeToNextActionOnNextUpdate(true,true)
	end
	--  모습 원래대로 (이걸 사용한후 curAction을 얻어와 쓰면 절대로 안됨!!!)
    local kTransformXMLID = kCurAction:GetScriptParam("TRANSFORM_XML")
    if kTransformXMLID ~= "" then
    
        local   iTransformEffectID = tonumber(kCurAction:GetScriptParam("TRANSFORM_ID"))
        if iTransformEffectID ~= nil then
        
            local   kMan = GetStatusEffectMan()
            kMan:RemoveStatusEffectFromActor(actor:GetPilotGuid(),iTransformEffectID)
        
        end
    
    end
	return true
end

function Skill_Raid_OnLeave(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local curAction = actor:GetAction()
	
	CheckNil(nil==curAction)
	CheckNil(curAction:IsNil())
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true
	end
	
	if actionID == "a_jump" then 
		if curParam == "end" then 
			action:SetSlot(2)
			action:SetDoNotBroadCast(true)
			return true
		end
		return	false
	end
	
	
	if action:GetActionType()=="EFFECT" then
		
		return true
	end
	
	if curParam == "end" then 
		return true
	end

	return false 
end

function Skill_Raid_OnEvent(actor,textKey)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())

	local kAction = actor:GetAction()
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	if kAction:GetActionParam() == AP_CASTING then
		return true
	end

	return	true
end
