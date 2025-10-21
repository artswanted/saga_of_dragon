-- skill

function Skill_Mon_Breath_OnCheckCanEnter(actor, action)
	return true
end

function Skill_Mon_Breath_OnEnter(actor, action)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--ODS("Skill_Mon_Breath_OnEnter ActionParam : "..action:GetActionParam().."\n")
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Skill_Mon_Breath_OnCastingCompleted(actor,action)
	else
	
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then	--캐스팅 시작 동작이 없으면
			action:SetSlot(1)
		end

		local	kTargetList = action:GetTargetList()
		local	iTargetCount = kTargetList:size()
	
		--ODS("Skill_Mon_Breath Casting Start iTargetCount :"..iTargetCount.."\n")
		
		if iTargetCount>0 then
			local	kTargetInfo = kTargetList:GetTargetInfo(0)
			if kTargetInfo:IsNil() == false then
			
				local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID())
				if kTargetPilot:IsNil() == false then
				
					local	kTargetActor = kTargetPilot:GetActor()
					if kTargetActor:IsNil() == false then
						actor:LookAt(kTargetActor:GetPos(),true,true)
					end
				end
			end
		end
		
		actor:ResetAnimation()
		actor:PlayCurrentSlot()

		--ODS("Skill_Mon_Breath_OnEnter "..action:GetSlotAnimName(action:GetCurrentSlot()).." \n", false, 1509)

		local Effect = actor:GetAnimationInfoFromAction("CAST_EFFECT",action,4294967294)--4294967294는 시퀀스 아이디 에러 값임. 현재 액션을 정확하게 사용하기 위해서임
		if nil~=Effect and ""~=Effect then
			local EffectNode = actor:GetAnimationInfoFromAction("CAST_EFFECT_NODE",action)
			if nil==EffectNode or ""==EffectNode then
				EffectNode = "char_root"
			end
			
			actor:AttachParticle(582,EffectNode,Effec);
		end
		
	end

	action:SetParamInt(10,0)

	return true
end
function Skill_Mon_Breath_OnCastingCompleted(actor,action)
	--ODS("Skill_Mon_Breath_OnCastingCompleted\n")

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	action:SetSlot(2)
	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	
	Skill_Mon_Breath_Fire(actor,action);
	
end
function  Skill_Mon_Breath_OnTargetListModified(actor,action,bIsBefore)

    --ODS("Skill_Mon_Breath_OnTargetListModified\n");
    
    if bIsBefore == false then
        --  때린다.
        Actor_Mob_Base_DoDefaultMeleeAttack(actor,action);   --  타격 효과
        action:GetTargetList():ApplyActionEffects();    
    end
end

function Skill_Mon_Breath_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(0,g_world:GetAccumTime())	--	Rotation Start Time
	action:SetParamFloat(1,actor:GetLookingDir():GetX());
	action:SetParamFloat(2,actor:GetLookingDir():GetY());
	action:SetParamFloat(3,actor:GetLookingDir():GetZ());
	
	action:SetParamInt(4,0);
	
	local bUseTime	= actor:GetAnimationInfo("USE_ABIL_TIME")
	if nil~=bUseTime and "TRUE"~=bUseTime then
		action:SetParamInt(14,1)
	end

	local fireStartNode	= actor:GetAnimationInfo("FIRE_EFFECT_NODE")	
	if fireStartNode ~= nil then
		local fireEffect = actor:GetAnimationInfo("FIRE_EFFECT")	
		if fireEffect == nil then
			fireEffect = "e_m_Mahalka_skill_01_char_root"
		end					
		--ODS("Skill_Mon_Breath_Fire OK. NODE : " .. fireStartNode .. " EFF : " .. fireEffect ..  "\n", false, 1509)
		actor:AttachParticle(581,fireStartNode,fireEffect);
	end
	action:SetParamInt(10,1)

	actor:DetachFrom(582)	--캐스팅 이펙트 제거
end

function Skill_Mon_Breath_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--ODS("Skill_Mon_Breath_OnUpdate accumTime:"..accumTime.."\n");

	if action:GetActionParam() == AP_FIRE then
		local	fElapsedTime = accumTime - action:GetParamFloat(0);

		if 0 ~= action:GetParamInt(10) and actor:IsAnimationDone() then
			if action:GetCurrentSlot() == 2 then
				local anilen = actor:GetAnimationLength(action:GetSlotAnimName(3))
				if 0 < anilen then	--리턴이 있으면	
					action:SetSlot(3)
					actor:ResetAnimation()
					actor:PlayCurrentSlot()
					return true
				end
			end

			return false
		end
		
		if 0==action:GetParamInt(14) then
			local	fTotalTime = action:GetAbil(AT_MAINTENANCE_TIME)/1000.0;
			ODS("Skill_Mon_Breath_OnUpdate fTotalTime:"..fTotalTime.."\n",false,6482);
			if fElapsedTime > fTotalTime then
				return false
			end
		end
		
	elseif action:GetActionParam() == AP_CASTING then
	
		if action:GetCurrentSlot() == 0 then
			if actor:IsAnimationDone() then
				action:SetSlot(1)
				actor:ResetAnimation();
				actor:PlayCurrentSlot();				
			end
		end
	end
	
	--ODS("Skill_Mon_Breath_OnUpdate OK\n")
	return true
end

function Skill_Mon_Breath_OnCleanUp(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	actor:DetachFrom(231)
	actor:DetachFrom(581)
	actor:DetachFrom(582)	--캐스팅 이펙트 제거
end

function Skill_Mon_Breath_OnLeave(actor, action)
	return	true
end

function Skill_Mon_Breath_OnEvent(actor,textKey)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local kAction = actor:GetAction();
	
	if( CheckNil(nil==kAction) ) then return end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end
		local QuakeTime = actor:GetAnimationInfo("QUAKE_TIME")
		if nil ~= QuakeTime and "" ~= QuakeTime then
			local QuakeFactor = actor:GetAnimationInfo("QUAKE_FACTOR")	
			if  nil == QuakeFactor then
				QuakeFactor = 1.0
			else
				QuakeFactor = tonumber(QuakeFactor)
			end
			QuakeCamera(QuakeTime, QuakeFactor)
		end

		local fireEffect = actor:GetAnimationInfo("FIRE_EFFECT")	
		if fireEffect ~= nil then
			local fireStartNode	= actor:GetAnimationInfo("FIRE_EFFECT_NODE")	
			
			if fireStartNode == nil then
				fireStartNode = "p_ef_heart"
			end
			
			actor:AttachParticle(231, fireStartNode, fireEffect)
		end			
	end
end
