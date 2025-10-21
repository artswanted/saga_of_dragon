
-- Melee
function Skill_RainOfFire_OnCheckCanEnter(kActor,kAction)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	local weapontype = kActor:GetEquippedWeaponType()
	
	if weapontype == 0 then
		return	false
	end

	return		true
	
end

function Skill_RainOfFire_OnEnter(kActor, kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
--	local actorID = kActor:GetID()
	local actionID = kAction:GetID()
--	local weapontype = kActor:GetEquippedWeaponType()
	
	ODS("Skill_RainOfFire_OnEnter actionID:"..actionID.." GetActionParam:"..kAction:GetActionParam().."\n")
		ODS("Skill_InstallDefault_OnEnter actionID:"..actionID.." GetActionParam:"..kAction:GetActionParam().."\n");
	local   kFireEffectID = kAction:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
    local   kFireEffectTargetNodeID = kAction:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	local   kFireEffectTargetNodeIDToPos = kAction:GetScriptParam("CASTING_EFFECT_TARGET_NODE_TO_WORLD_POS");
	local   kFireEffectTargetNodeIDToPosWithRotate = kAction:GetScriptParam("CASTING_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE");

	local   kFireEffectID1 = kAction:GetScriptParam("CASTING_EFFECT_EFFECT_ID1");
    local   kFireEffectTargetNodeID1 = kAction:GetScriptParam("CASTING_EFFECT_TARGET_NODE1");
	
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= ""  or kFireEffectTargetNodeIDToPosWithRotate ~= "" 
	   or (kFireEffectID1 ~= "" and  kFireEffectTargetNodeID1 ~="")
	   then    
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = kActor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(kActor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            kActor:AttachParticleToPoint(7215,kDir,kFireEffectID);
            
        elseif kFireEffectTargetNodeID ~= "" then
            kActor:AttachParticle(7215,kFireEffectTargetNodeID,kFireEffectID);
		elseif kFireEffectTargetNodeIDToPos ~= "" then
			kActor:AttachParticleToPoint(7215,kActor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID);			
		else
		    kActor:AttachParticleToPointWithRotate(7215,kActor:GetNodeWorldPos(kFireEffectTargetNodeIDToPosWithRotate), kFireEffectID, kActor:GetRotateQuaternion())
        end
		
		if kFireEffectTargetNodeID1 ~= "" then 
			kActor:AttachParticle(7216,kFireEffectTargetNodeID1,kFireEffectID1)
		end
    end	
	
	local	kSoundID = kAction:GetScriptParam("CASTING_SOUND_ID");
	if kSoundID~="" then
		kActor:AttachSound(2785,kSoundID);
	end
	
	if kAction:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true
	end

	Skill_RainOfFire_OnCastingCompleted(kActor,kAction)
	
	return true
end

function Skill_RainOfFire_OnCastingCompleted(kActor, kAction)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	Skill_RainOfFire_Fire(kActor,kAction)
	kAction:SetSlot(kAction:GetCurrentSlot()+1)	

	--kAction:SetParam(1,"end")	
	--local	kTargetPos = kActor:GetNodeWorldPos("char_root")
	--kActor:AttachParticleToPointWithRotate(8,kTargetPos,"ef_ScrewUpper_01_char_root", kActor:GetRotateQuaternion())

end


function Skill_RainOfFire_Fire(kActor,kAction)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	ODS("RestoreItemModel Skill_RainOfFire_Fire\n")
	if kAction:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    kActor:StartWeaponTrail()
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	kAction:SetParamFloat(2,g_world:GetAccumTime()) --  시작 시간
	kAction:SetParamFloat(6,0)
	kAction:SetParamInt(9,0)	--	Hit Count

	
	local	kSoundID = kAction:GetScriptParam("FIRE_SOUND_ID")
	if kSoundID~="" then
		kActor:AttachSound(2783,kSoundID)
	end	
	
	--	무기 바꾸기 처리
	if kAction:GetScriptParam("WEAPON_XML") ~= "" then
		kActor:ChangeItemModel(ITEM_WEAPON, kAction:GetScriptParam("WEAPON_XML"))
		ODS("ChangeItemModel "..kAction:GetScriptParam("WEAPON_XML").."\n")
	end
	
	if kAction:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		kActor:HideParts(EQUIP_POS_WEAPON, true)
	end

    local   kFireEffectID = kAction:GetScriptParam("FIRE_CASTER_EFFECT_ID")
    local   kFireEffectTargetNodeID = kAction:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE")
	local   kFireEffectTargetNodeIDToPos = kAction:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS")
	local   kFireEffectTargetNodeIDToPosWithRotate = kAction:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE")

    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= ""  or kFireEffectTargetNodeIDToPosWithRotate ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = kActor:GetLookingDir()
            local   fDistance = 100
            kDir:Multiply(fDistance)
            kDir:Add(kActor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)            
            --kActor:AttachParticleToPoint(7212,kDir,kFireEffectID)
			kActor:AttachParticleToPointS( 7212, kDir, kFireEffectID, 0.7)
			
			--kActor:AttachParticleToPoint(7220,kDir,"ef_Pattern_RainOfFire_char_root")	-- 마법진 이펙트
			kActor:AttachParticleToPointS(7220, kDir,"ef_Pattern_RainOfFire_char_root", 0.7)	-- 마법진 이펙트
        elseif kFireEffectTargetNodeID ~= "" then
            kActor:AttachParticle(7212,kFireEffectTargetNodeID,kFireEffectID)
		elseif kFireEffectTargetNodeIDToPos ~= "" then
			kActor:AttachParticleToPoint(7212,kActor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID)			
		else
		    kActor:AttachParticleToPointWithRotate(7212,kActor:GetNodeWorldPos(kFireEffectTargetNodeIDToPosWithRotate), kFireEffectID, kActor:GetRotateQuaternion())
        end
    
    end	

    kFireEffectID = kAction:GetScriptParam("FIRE_CASTER_EFFECT_ID2")
    kFireEffectTargetNodeID = kAction:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE2")
	
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = kActor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(kActor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)            
            kActor:AttachParticleToPoint(7213,kDir,kFireEffectID)
        else
            kActor:AttachParticle(7213,kFireEffectTargetNodeID,kFireEffectID)
        end
    
    end	
		
	if kAction:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    kActor:StartWeaponTrail()
	end
	
	--  변신 적용
    local kTransformXMLID = kAction:GetScriptParam("TRANSFORM_XML")
    if kTransformXMLID ~= "" then
    
        local   iTransformEffectID = tonumber(kAction:GetScriptParam("TRANSFORM_ID"))
        if iTransformEffectID ~= nil then
        
            local   kMan = GetStatusEffectMan()
            kMan:AddStatusEffectToActor(kActor:GetPilotGuid(),kTransformXMLID,iTransformEffectID,0)        
        end
    
    end
	
	if kActor:IsMyActor() then
		kAction:SetParamFloat(2,accumTime);
		kAction:StartTimer(1.0,0.1,0);
	end
end

function Skill_RainOfFire_OnTimer(kActor,fAccumTime,kAction,iTimerID)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())

	local	iHitCount = kAction:GetParamInt(9)
	local	iTotalHit = kAction:GetAbil(AT_COUNT)
	if iHitCount == iTotalHit then	
		return
	end
			
	if kActor:IsMyActor() then
		kAction:CreateActionTargetList(kActor)
		kAction:BroadCastTargetListModify(kActor:GetPilot())
		kAction:ClearTargetList()
	end
	
	if kActor:IsMyActor() then
		QuakeCamera(0.1,1,0,0.01,10)
	end	
	
	
	iHitCount = iHitCount + 1
	kAction:SetParamInt(9,iHitCount)
	
	return	true
end

function Skill_RainOfFire_OnUpdate(kActor, accumTime, frameTime)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	local actorID = kActor:GetID()
	local kAction = kActor:GetAction()
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	--local actionID = kAction:GetID()
	--local movingSpeed = 0
	local bAnimDone = kActor:IsAnimationDone()
	local iSlot = kAction:GetCurrentSlot()
	--local CurState = kAction:GetParam(1)
	
--	ODS("슬롯 : "..iSlot.."\n", false, 912)
	if iSlot == 1 then
		if bAnimDone then
			kActor:PlayNext()	
			kAction:SetParam(1,"end")
--[[			if kAction:GetScriptParam("DETACH_CASTING_EFFECT_AT_FINISH") == "TRUE" then
				local DefaultDetaChID = false
				if kAction:GetScriptParam("CASTING_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
					DefaultDetaChID = true
				end
				kActor:DetachFrom(7215,DefaultDetaChID)
				kActor:DetachFrom(7216,DefaultDetaChID)
			end			
	]]
			return false
		end
	end
	return true
end

function Skill_RainOfFire_OnCleanUp(kActor)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	local   kCurAction = kActor:GetAction()

	if kCurAction:GetScriptParam("WEAPON_XML") ~= "" then
		kActor:RestoreItemModel(ITEM_WEAPON)
	end

	if kCurAction:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		kActor:HideParts(EQUIP_POS_WEAPON, false)
	end

	if kCurAction:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    kActor:EndWeaponTrail()
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

	    kActor:DetachFrom(7212,DefaultDetaChID)
	    kActor:DetachFrom(7213,DefaultDetaChID2)
		kActor:DetachFrom(7214)
	end
	
	if kCurAction:GetScriptParam("DETACH_CASTING_EFFECT_AT_FINISH") == "TRUE" then
		local DefaultDetaChID = false
		if kCurAction:GetScriptParam("CASTING_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID = true
		end
		kActor:DetachFrom(7215,DefaultDetaChID)
		kActor:DetachFrom(7216,DefaultDetaChID)
	end
		
	local   kCleanUpEffectID = kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_ID")
    local   kCleanUpEffectTargetNodeID = kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_TARGET_NODE")
	local	fCleanUpScale = tonumber(kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_SCALE"))

	if fCleanUpScale == 0 or fCleanUpScale == nil then
		fCleanUpScale = 1.0
	end
    
    if kCleanUpEffectID ~= "" and kCleanUpEffectTargetNodeID ~= "" then
		 kActor:AttachParticleS(7216,kCleanUpEffectTargetNodeID,kCleanUpEffectID, fCleanUpScale)
	end

	if "1" == kCurAction:GetParam(3) then
		--캐스팅 중에 캔슬된 상태
		kActor:DetachFrom(7215,true)
		kActor:DetachFrom(7216,true)
		if( false == kActor:PlayNext()) then 
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
            kMan:RemoveStatusEffectFromActor(kActor:GetPilotGuid(),iTransformEffectID)
        end
    end
	return true
end

function Skill_RainOfFire_OnLeave(kActor, kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	local curAction = kActor:GetAction()
	
	CheckNil(nil==curAction)
	CheckNil(curAction:IsNil())
	
	local curParam = curAction:GetParam(1)
	local actionID = kAction:GetID()
	
	if kActor:IsMyActor() == false then
		return true
	end
	
	if actionID == "a_jump" then 
		if curParam == "end" then 
			kAction:SetSlot(2)
			kAction:SetDoNotBroadCast(true)
			return true
		end
		return	false
	end
	
	
	if kAction:GetActionType()=="EFFECT" then
		
		return true
	end
	
	if curParam == "end" then 
		return true
	end

	return false 
end

function Skill_RainOfFire_OnEvent(kActor,textKey)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())

	local kAction = kActor:GetAction()
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	if kAction:GetActionParam() == AP_CASTING then
		return true
	end

	return	true
end

function Skill_RainOfFire_OnTargetListModified(kActor,kAction,bIsBefore)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
    if bIsBefore == false then
        SkillHelpFunc_DefaultHitOneTime(kActor,kAction, true);
    end

end
