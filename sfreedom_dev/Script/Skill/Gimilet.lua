-- [Skill_Gimilet_ID] : 발사할 발사체 ID, "" 일 경우 무기에 붙어있는 발사체 ID 를 사용한다.
-- [ATTACH_NODE_ID] : 발사체를 붙일 액터의 노드 ID, "" 일 경우 캐릭터 좌표에서 발사된다.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [NO_AUTO_CONNECT] : 다음 액션으로 부드럽게 연결해줄 것인가
-- [PIERCING] : 관통형인가
-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- Melee
function Skill_Gimilet_OnCheckCanEnter(kActor,kAction)

	if( CheckNil(nil==kActor) ) then return false end
	if( CheckNil(kActor:IsNil()) ) then return false end
		
	local weapontype = kActor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_Gimilet_OnCastingCompleted(kActor, kAction)
	
	if( CheckNil(nil==kActor) ) then return false end
	if( CheckNil(kActor:IsNil()) ) then return false end
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	Skill_Gimilet_Fire(kActor,kAction);
	kAction:SetSlot(kAction:GetCurrentSlot()+1);
	kActor:PlayCurrentSlot();

end

function	Skill_Gimilet_LoadToWeapon(kActor,kAction)

	if( CheckNil(nil==kActor) ) then return nil end
	if( CheckNil(kActor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==kAction) ) then return nil end
	if( CheckNil(kAction:IsNil()) ) then return nil end
	
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = kAction:GetScriptParam("PROJECTILE_ID");
	
	if kProjectileID == "" then
		kProjectileID = kActor:GetEquippedWeaponProjectileID();
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,kAction,kActor:GetPilotGuid());
	
	local	kTargetNodeID = kAction:GetScriptParam("ATTACH_NODE_ID");
	
	kNewArrow:LoadToHelper(kActor,kTargetNodeID);	--	장전

	return	kNewArrow;
end

function Skill_Gimilet_Fire(kActor,kAction)

	if( CheckNil(nil==kActor) ) then return false end
	if( CheckNil(kActor:IsNil()) ) then return false end
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end	
	
	--	무기 바꾸기 처리
	if kAction:GetScriptParam("WEAPON_XML") ~= "" then
		kActor:ChangeItemModel(ITEM_WEAPON, kAction:GetScriptParam("WEAPON_XML"));
		ODS("ChangeItemModel "..kAction:GetScriptParam("WEAPON_XML").."\n");
	end	
	
end

function Skill_Gimilet_OnEnter(kActor, kAction)

	if( CheckNil(nil==kActor) ) then return false end
	if( CheckNil(kActor:IsNil()) ) then return false end
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
--	local actorID = kActor:GetID()
--	local actionID = kAction:GetID()
--	local weapontype = kActor:GetEquippedWeaponType();
	
	if kAction:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	kActor:ResetAnimation();

	Skill_Gimilet_OnCastingCompleted(kActor,kAction);
	
	return true
end

function Skill_Gimilet_OnUpdate(kActor, accumTime, frameTime)
		
	if( CheckNil(nil==kActor) ) then return false end
	if( CheckNil(kActor:IsNil()) ) then return false end
	
--	local actorID = kActor:GetID()
	local kAction = kActor:GetAction()
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	local animDone = kActor:IsAnimationDone()
--	local nextActionName = kAction:GetNextActionName()
--	local actionID = kAction:GetID();
	
	if animDone == true then
	
		local curActionParam = kAction:GetParam(0)
		if curActionParam == "GoNext" then
			local nextActionName = kAction:GetParam(1);

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

function Skill_Gimilet_OnCleanUp(kActor)
	
	
	if( CheckNil(nil==kActor) ) then return false end
	if( CheckNil(kActor:IsNil()) ) then return false end
	
	--	무기 원래대로
	if kActor:GetAction():GetScriptParam("WEAPON_XML") ~= "" then
		kActor:RestoreItemModel(ITEM_WEAPON);
	end

	return true;
end

function Skill_Gimilet_OnLeave(kActor, kAction)

	if( CheckNil(nil==kActor) ) then return true end
	if( CheckNil(kActor:IsNil()) ) then return true end
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	local curAction = kActor:GetAction();
	if( CheckNil(nil==kAction) ) then return true end
	if( CheckNil(kAction:IsNil()) ) then return true end
	
	local curParam = curAction:GetParam(1)
	local actionID = kAction:GetID()
	
	if kActor:IsMyActor() == false then
		return true;
	end
	
	if actionID == "a_jump" then 
		return false;
	end
	
	if kAction:GetActionType()=="EFFECT"  then
		
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end
	
	if kAction:GetEnable() == false then
		if curParam == actionID then
			curAction:SetParam(0,"");
		end
	end
		
	if kAction:GetEnable() == true then
	

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

function Skill_Gimilet_OnEvent(kActor,textKey)
		
	if( CheckNil(nil==kActor) ) then return false end
	if( CheckNil(kActor:IsNil()) ) then return false end
	
	local kAction = kActor:GetAction();
			
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	ODS("Skill_Gimilet_OnEvent textKey:"..textKey.."\n");
	
	if textKey == "hit" or textKey == "fire" then

		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			kActor:AttachSound(2784,kSoundID);
		end
		
		local	iAttackRange = kAction:GetSkillRange(0,kActor);
		
		if iAttackRange == 0 then
			iAttackRange = 200; 
		end
		
--		local	kProjectileMan = GetProjectileMan();
		local	kArrow = Skill_Gimilet_LoadToWeapon(kActor,kAction);
		
--		local	targetobject = nil;

		if kArrow:IsNil() == false then
		
			local	kMovingDir = kActor:GetLookingDir();	-- 액터가 바라보고 있는 방향
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
			
			-- 관통
			local iPentrationCount = kAction:GetAbil(AT_PENETRATION_COUNT);
			kArrow:SetPenetrationCount(iPentrationCount);-- 몇번을 관통할것인지 세팅
			if iPentrationCount ~= 0 then
				kArrow:SetMultipleAttack(true);
			end
		
			kMovingDir:Multiply(iAttackRange);
			kMovingDir:Add(arrow_worldpos);
			kArrow:SetTargetLoc(kMovingDir);	
			
			kArrow:Fire();
			--ODS("몇번:"..iPentrationCount.."\n", false, 912)
		else
			
			ODS("Arrow is nil\n");
		
		end
					
	end

	return	true;
end
