-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [FIRE_SOUND_ID] : 스킬이 FIRE 될때 플레이 할 사운드 ID
-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

-- Melee
function Skill_DemolitionCharge_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_DemolitionCharge_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_DemolitionCharge_Fire(actor,action);
	action:SetSlot(0);
	actor:PlayCurrentSlot();

end
function Skill_DemolitionCharge_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	action:CreateActionTargetList(actor);
	
end

function Skill_DemolitionCharge_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local actionID = action:GetID()
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_DemolitionCharge_OnCastingCompleted(actor,action);

	actor:StopJump()

	local guid = GUID("123")
	guid:Generate()
	local pilot = g_pilotMan:NewPilot(guid, 1000656, 0) -- sa_demolitioncharge
	if false == pilot:IsNil() then
		pilot:SetUnit(guid,UT_ENTITY,1,1,0);
		local kBird = pilot:GetActor()
		if false == kBird:IsNil() then
			kBird:FreeMove(true)
			local kTrans = actor:GetTranslate()
			kTrans:SetZ(kTrans:GetZ()+5)
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			g_world:AddActor(guid, kBird, kTrans, 9);
			pilot:GetUnit():SetCaller(actor:GetPilotGuid())
			local	kLookAt = actor:GetLookingDir()
			kBird:LookAt(actor:GetTranslate():_Add(kLookAt), true)	--캐릭터랑 같은 방향으로
			kBird:ReserveTransitAction("a_DemolitionChargeSA")
			pilot:GetActor():ClearActionState();
			if actor:IsMyActor() then
	        	pilot:GetActor():SetUnderMyControl(true);
		    end

			local fScale = (action:GetSkillLevel()%10)*0.05 + 1.0	--스킬레벨에 따라 커지게
			kBird:SetTargetScale(fScale, 0.56)
			action:SetParam(123,guid:GetString())	--GUID 저장
			g_pilotMan:InsertPilot(guid, pilot)
		end
	end

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 0.7, 0.4,true,true);
	end

	return true
end

function Skill_DemolitionCharge_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()

	local curAnimSlot = action:GetCurrentSlot();

	if curAnimSlot == 3 then
		if animDone == true then
			action:SetParam(1, "end");
			return false
		end	
	else
		if action:GetActionParam() == AP_FIRE then
			if animDone == true then
				action:SetParam(1, "end");
				if actor:IsMyActor() then
					if 0==curAnimSlot then
						actor:AttachParticle(123, "char_root", "ef_skill_Demolition_charge_01_char_root")
					elseif 2==curAnimSlot then
						actor:DetachFrom(123)
					end
				end
				actor:PlayNext()
			end
		end
	end

	return true
end

function Skill_DemolitionCharge_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end	
	
	Skill_DemolitionCharge_RemoveEntity(actor:GetAction())
	actor:DetachFrom(123)
	
	return true;
end

function Skill_DemolitionCharge_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true;
	end

	if action:GetID() == "a_battle_idle" then
		return true;
	end

	if action:GetID() ~= "a_act" then
		return false;
	end
	
	if action:GetActionType()=="EFFECT" then
		
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
		curAction:SetParam(0,"GoNext");
		return false;
	end

	return false 
end

function Skill_DemolitionCharge_RemoveEntity(curAction)
	local guid = GUID(curAction:GetParam(123))
	local Death = g_pilotMan:FindPilot(guid)	--키반응으로 액션이 끊겼을 경우 엔티티는 끝나는 액션
	if false==Death:IsNil() and false==Death:GetActor():IsNil() then
		if 3==Death:GetActor():GetAction():GetCurrentSlot() then
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			g_world:RemoveActorOnNextUpdate(guid)			
		else
			Death:GetActor():GetAction():SetSlot(3)
			Death:GetActor():PlayCurrentSlot()
			Death:GetActor():AttachParticle(125, "char_root", "ef_skill_demolition_charge_03_char_root")
		end
	end
end

function Skill_DemolitionCharge_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	Act_Melee_Default_OnEvent(actor,textKey)
	if textKey == "hit" or textKey == "fire" then
		QuakeCamera(0.5, 1.8)
		local kPos = actor:GetTranslate()
		local kDir = actor:GetLookingDir()
		kDir:Unitize()
		kDir:Multiply(90)
		kPos:Add(kDir)
		actor:AttachParticleToPointS(124, kPos, "ef_boom_06",0.7)
		local action = actor:GetAction()
		CheckNil(nil==action);
		CheckNil(action:IsNil());
		local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2783,kSoundID);
		end		
	end
	return	true;
end

function Skill_DemolitionCharge_OnTargetListModified(actor,action,bIsBefore)
	Act_Melee_Default_OnTargetListModified(actor,action,bIsBefore)
end
