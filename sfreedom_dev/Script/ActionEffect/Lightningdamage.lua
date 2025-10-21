-- damage

function Lightning_Default_Dmg_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	if actor:IsRidingPet() then --라이딩펫: 탑승 중 피격당하면 강제 하차→피격 액션 진행
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
		end
	end

	actor:Stop()
	actor:ResetAnimation();
	action:SetParam(0,"");
	
	-- DB에 몇초 간격으로 적용시킬것인지 확인
	local fLightining_DurTime = action:GetAbil(AT_CUSTOMDATA1)
	if(0 == fLightining_DurTime) then 
		ODS("얼레 값이 없음\n", false, 912)
		fLightining_DurTime = 420
	end
	fLightining_DurTime = fLightining_DurTime/1000
	action:SetParamFloat(1, fLightining_DurTime)
	
	local dmgAni = 0
	-- 왼쪽을 바라보고 있을 때는 0, 오른쪽을 바라보고 있을 때는 1
	if actor:IsToLeft() == true then
		dmgAni = 0
	else
		dmgAni = 1
	end
	
	if dmgAni >= action:GetSlotCount() then
		dmgAni = action:GetSlotCount()-1
	end

	action:SetSlot(dmgAni);	
	
	local	animname = action:GetSlotAnimName(dmgAni);
	
	if actor:GetAnimationLength(animname) == 0 then
		action:SetSlot(0);	
	end

	local nUnitSize = actor:GetAbil(AT_UNIT_SIZE);
	if nUnitSize == 0 then
		nUnitSize = 1; --유닛 크기가 설정되어있지 않으면 기본으로 지정한다.
	end

	local nEffectScale = 1.0;
	if 1 == nUnitSize then --1:Small
		nEffectScale = 1.0;
		ODS("Small Monster\n");
	elseif 2 == nUnitSize then --2:Middle
		nEffectScale = 1.5;
		ODS("Middle Monster\n");
	elseif 3 == nUnitSize then --3:Large
		nEffectScale = 2.0;
		ODS("Large Monster\n");
	end
		
    actor:AttachParticleS(12,kHitTargetEffectTargetNodeID,kHitTargetEffectID, nEffectScale);
	if( false == actor:IsTransformed() ) then
		actor:HideNode("Scene Root",true);
	end
	actor:AttachParticleS(513,"char_root","ef_mag_lightning_shock", nEffectScale);
	action:SetParamFloat(14,g_world:GetAccumTime());	
	
	return true
end
function Lightning_Default_Dmg_OnUpdate(actor, accumTime, frameTime,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	fElapsedTime = accumTime - action:GetParamFloat(14);
	local fLightining_DurTime = action:GetParamFloat(1)
	ODS("fLightining_DurTime:"..fLightining_DurTime.."\n", false, 912)
	if fElapsedTime > fLightining_DurTime then
		if( false == actor:IsTransformed() ) then
			actor:HideNode("Scene Root",false);
		end
		actor:DetachFrom(513);
		return false;
	end
	
	--if actor:IsAnimationDone() == true then
	--	action:SetParam(0,"end");
	--	return false;
	--end
	
	return true
end
function Lightning_Default_Dmg_OnCleanUp(actor, action)

	return true
end

function Lightning_Default_Dmg_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(action == nil) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	if( false == actor:IsTransformed() ) then
		actor:HideNode("Scene Root",false);
	end
	actor:DetachFrom(513);

	return true
end