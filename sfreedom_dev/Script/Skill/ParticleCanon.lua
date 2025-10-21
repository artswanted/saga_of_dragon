
-- Melee
function Skill_ParticleCanon_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return true;
end

function Skill_ParticleCanon_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	local   action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if textKey == "fire" or textKey=="hit" then		

		action:SetParam(2,"HIT");
		
		--ODS("타이머 세팅!. 타격수:"..action:GetParamInt(30).."\n", false, 912)		
		action:StartTimer(1.0, 1.0 / action:GetParamInt(30),0);
	end
	
	return true;
end

function Skill_ParticleCanon_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local guid = GUID("123")
	guid:Generate()
	local pilot = g_pilotMan:NewPilot(guid, 1000654, 0) -- sa_particlecanon
	if false == pilot:IsNil() then
		pilot:SetUnit(guid,UT_ENTITY,1,1,0);
		local kCanon = pilot:GetActor()
		if false == kCanon:IsNil() then
			kCanon:FreeMove(true)
			local kTrans = actor:GetTranslate()
			kTrans:SetZ(kTrans:GetZ()+10)
			g_world:AddActor(guid, kCanon, kTrans, 9);
			pilot:GetUnit():SetCaller(actor:GetPilotGuid())
			local	kLookAt = actor:GetLookingDir()
			kCanon:LookAt(actor:GetTranslate():_Add(kLookAt), true)	--캐릭터랑 같은 방향으로
			kCanon:ReserveTransitAction("a_particle_canon_sa")
			kCanon:SetTargetScale(1, 0);
			pilot:GetActor():ClearActionState();
			if actor:IsMyActor() then
	        	pilot:GetActor():SetUnderMyControl(true);
		    end

			-- local fscale = (action:getskilllevel()%10)*0.05 + 1.0	--스킬레벨에 따라 커지게
			-- kCanon:settargetscale(fscale, 0.56)
		end
	end

	Skill_ParticleCanon_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
end

function Skill_ParticleCanon_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	action:SetParamFloat(2, g_world:GetAccumTime()); --  시작 시간
	action:SetParamInt(9,0);	--	Hit Count
	action:SetParamInt(30, action:GetAbil(AT_COUNT))	--	Max Count
end

function Skill_ParticleCanon_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_ParticleCanon_OnCastingCompleted(actor,action);

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 0.7, 0.4,true,true);
	end

	return true
end
function Skill_ParticleCanon_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	
    if bIsBefore == false then

        if action:GetParam(2) == "HIT" then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            if actor:IsMyActor()  then
				action:GetTargetList():ApplyActionEffects(true,true);
            else
				SkillHelpFunc_DefaultHitOneTime(actor,action, true);
            end
        
        end        
    
    end		
end

function Skill_ParticleCanon_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local bAnimDone = actor:IsAnimationDone();
	local iSlot = action:GetCurrentSlot();
	local nextActionName = action:GetNextActionName();
	
	if iSlot == 1 then
		if bAnimDone then
			local   fMoveSpeed = actor:GetAbil(AT_C_MOVESPEED)*1.8;   --  이동속도의 2배
			
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			action:SetParamFloat(0, g_world:GetAccumTime());	-- Start Time
			action:SetParamFloat(1, fMoveSpeed);	-- Start Velocity
			
			action:SetParamFloat(6,actor:GetLookingDir():GetX());
			action:SetParamFloat(7,actor:GetLookingDir():GetY());
			action:SetParamFloat(8,actor:GetLookingDir():GetZ());
			actor:SetMovingDelta(Point3(0,0,0));

			action:SetSlot(action:GetCurrentSlot()+1);
			actor:PlayCurrentSlot();
		end
	elseif iSlot == 2 then
		if bAnimDone then
			action:SetSlot(action:GetCurrentSlot()+1);
			actor:PlayCurrentSlot();			
		else
			local fAccel = -100 * frameTime
			local fVelocity = action:GetParamFloat(1)
			
			fVelocity = fVelocity + fAccel
			action:SetParamFloat(1, fVelocity)

			local fElapsedTime = accumTime - action:GetParamFloat(0);
			if fElapsedTime < 1.2 or fVelocity > 0 then
				local kMovingDir = Point3(action:GetParamFloat(6),
											action:GetParamFloat(7),
											action:GetParamFloat(8));
											
				kMovingDir:Multiply(-fVelocity)
				actor:SetMovingDelta(kMovingDir);
			end
		end
	else
		if bAnimDone then        
			action:SetParam(1,"end");
			return false        
		end    
	end

	return true
end

function Skill_ParticleCanon_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(199)

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end	
	
	return true;
end

function Skill_ParticleCanon_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if actionID == "a_jump" then 
		return false;
	end
	
	if action:GetActionType()=="EFFECT" then
		
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end

	return false 
end

function Skill_ParticleCanon_OnTimer(actor,fAccumTime,action,iTimerID)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("Skill_ParticleCanon fAccumTime:"..fAccumTime.."\n");
	local iComboCount = actor:GetComboCount();
	--ODS("Combo:"..iComboCount.."\n", false, 912);

	local	iHitCount = action:GetParamInt(9);
	local	iTotalHit = action:GetParamInt(30);	
	
	if iHitCount == iTotalHit then
		return true
	end

	if actor:IsMyActor() then	
		action:CreateActionTargetList(actor);		
		local	kTargetList = action:GetTargetList();
		local	iTargetCount = kTargetList:size();
		if 0 < iTargetCount then
			action:BroadCastTargetListModify(actor:GetPilot());
		end
	end
	
	iHitCount = iHitCount + 1;
	action:SetParamInt(9,iHitCount);
	
	return	true	
end

-- Ebtity
function Skill_ParticleCanonSA_OnCheckCanEnter(actor,action)
	return true
end

function Skill_ParticleCanonSA_OnEnter(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	action:SetDoNotBroadCast(true)
	
	local kPos = actor:GetTranslate()
	local kLookPos = actor:GetLookingDir()
	kLookPos:Multiply(35);
	kPos:Add(kLookPos);
	kPos:SetZ(kPos:GetZ() - 20);
	actor:AttachParticleToPointS(7213, kPos, "ef_smog_04", 1);
	actor:AttachParticleS(7212,"p_ef_shot_01","ef_skill_particlecanon_01", 1);
	return true
end

function Skill_ParticleCanonSA_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	actor:PlayCurrentSlot();
	action:SetParamFloat(2, g_world:GetAccumTime());
end

function Skill_ParticleCanonSA_Shot_Fire(actor,action)

end


function Skill_ParticleCanonSA_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil())
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	return not actor:IsAnimationDone();
end
function Skill_ParticleCanonSA_OnCleanUp(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end

	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())

	CheckNil(nil==action);
	CheckNil(action:IsNil());
	action:SetDoNotBroadCast(true)

	return true;
end
function Skill_ParticleCanonSA_OnLeave(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	actor:DetachFrom(7212)
	actor:DetachFrom(7213)

	return true
end

function Skill_ParticleCanonSA_OnEvent(actor, textKey)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
		
	if textKey == "fire" or textKey=="hit" then
	elseif textKey == "alpha_end" then
		actor:SetTargetAlpha(actor:GetAlpha(),0,0.5);
	end
end