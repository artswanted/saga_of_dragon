-- Idle

function Act_Twin_Sub_Trace_Ground_OnEnter(actor, action)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	CheckNil(action == nil);
	CheckNil(action:IsNil());
	
	action:SetDoNotBroadCast(true);	
	actor:MakePetActionQueue(-1)
	action:SetParamInt(6,0);
		
	local kCallerActor = actor:GetCallerActor()
	if kCallerActor:IsNil() then return true end
	
	actor:SetMovingDir(kCallerActor:GetMovingDir());
	actor:FindPathNormal();
		
	local iDist = actor:GetAnimationInfoFromAniName("MIN_DIST", "run"):GetStr()
	if nil==iDist or ""==iDist then
		iDist = 30
	else
		iDist = tonumber(iDist)
		if 0>=iDist then iDist = 30 end
	end

	local iFarDist = actor:GetAnimationInfoFromAniName("MAX_DIST", "run"):GetStr()
	if nil==iFarDist or ""==iFarDist then
		iFarDist = 50
	else
		iFarDist = tonumber(iFarDist)
		if 0>=iFarDist then iFarDist = 50 end
	end

	action:SetParamInt(7,iDist);
	action:SetParamInt(8,iFarDist);
	
	local kPrevAction = actor:GetAction()
	if(not kPrevAction:IsNil()) then	
		if( "a_twin_sub_repos" == kPrevAction:GetID() ) then
			action:SetSlot(3)				
			actor:PlayCurrentSlot(false)
		end
	end
		
	return true
end

function Act_Twin_Sub_Trace_Ground_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(action == nil);
	CheckNil(action:IsNil());

	local currentSlot = action:GetCurrentSlot()
	local param = action:GetParam(0)
	local iIdleType = action:GetParamInt(6);
	local iDist = action:GetParamInt(7);
	if 0>=iDist then iDist = 30 end

	local FarDist = action:GetParamInt(8);
	if 0>=FarDist then FarDist = 50 end
		
	local kCallerActor = actor:GetCallerActor()
	if kCallerActor:IsNil() then return true end
	local kPlayerPos = kCallerActor:GetPos()
	-- 이속 올라간 정도에 따라 대시하는 거리는 수정되게끔
	local fMoveSpeed = kCallerActor:GetAbil(AT_C_MOVESPEED)
	--ODS("fMoveSpeed:"..fMoveSpeed.."\n", false, 912)
	if(150 < fMoveSpeed) then	-- 기본 이동 거리가 150
		local fRate = fMoveSpeed/150
		iDist= iDist*fRate
		FarDist= FarDist*fRate
	end
	
	--Pos
	actor:SetTraceFlyTargetLoc( kPlayerPos )
	
--	ODS("Act_Twin_Sub_Trace_Ground_OnUpdate:"..kPlayerPos:GetX().." "..kPlayerPos:GetY().." "..kPlayerPos:GetZ().."\n", falsei, 912)
	
	local iNowSlot = action:GetCurrentSlot()
	local iNextSlot = iNowSlot
	
	-- 쌍둥이 main 캐릭터의 애니메이션이 무엇인지 확인하고, 쌍둥이 sub 캐릭터의 애니메이션 슬롯에 맞게 설정하고
	local iCallerSlot = -1	
	local iCallerSeqID = kCallerActor:GetAniSequenceID()
	--ODS("iCallerSeqID:"..iCallerSeqID.."\n", false, 912)
	if(512200 == iCallerSeqID or 1251200 == iCallerSeqID) then		-- idle_01
		iCallerSlot = 0
	elseif(512201 == iCallerSeqID or 1251201 == iCallerSeqID) then 	-- idle_02
		iCallerSlot = 1
	elseif(512202 == iCallerSeqID or 1251202 == iCallerSeqID) then 	-- idle_03
		iCallerSlot = 2
	elseif(512005 == iCallerSeqID) then							  	-- battle_idle
		iCallerSlot = 3
	end
	
	-- Next
	local bForceNextSlot = false
	if 0 <= iNowSlot and 6 >= iNowSlot then -- Idle
		local fDistance = actor:TraceGround( 0.5, frameTime, iDist, 10.0, true )
		if fDistance > FarDist then
			bForceNextSlot = true
			iNextSlot = 7 -- dash 애니
			actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",500,0,0);
		else
			if fDistance <= iDist then
				local bLookCaller = actor:GetAnimationInfoFromAction("LOOK_CALLER", action, 0);	
				if nil~=bLookCaller and 0~=tonumber(bLookCaller) then
					actor:LookAt(kPlayerPos, true)
				end

				bForceNextSlot = 5<iNowSlot
				iNextSlot = 0;--[[
				if true==bForceNextSlot then	--움직였다가 멈출 때
					actor:MakePetActionQueue(-1)
				end
				iNextSlot = actor:UpdatePetActionQueue(accumTime)
				]]				
				
			else
				bForceNextSlot = true
				iNextSlot = 6
			end
		end	
	elseif 7 == iNowSlot then -- Run		
		local fDistance = actor:TraceGround( 0.8, frameTime, iDist, 15.0, true )		
		if fDistance > FarDist then
		else
			--dash 애니 종료
			bForceNextSlot = true
			iNextSlot = 6
			actor:EndBodyTrail();
		end	
	elseif 8 == iNowSlot then -- opening
		local fDistance = actor:TraceGround( 1.0, frameTime, 30.0, 4.0, false )		
		if iDist < fDistance then
			bForceNextSlot = true
		end
		iNextSlot = 0
	end			

	local bFollowCallerAni = (0 <= iCallerSlot and iCallerSlot ~= currentSlot and currentSlot < 4)
	
	if actor:IsAnimationDone() or bForceNextSlot or bFollowCallerAni	then
		if(bFollowCallerAni) then
			iNextSlot = iCallerSlot
			actor:IncTimeToAniObj(0)	kCallerActor:IncTimeToAniObj(0)
		end
		
		action:SetSlot(iNextSlot)
		actor:PlayCurrentSlot(false)
	end

	local kParam = "MOVE"
	if 0<=iNextSlot and 6>iNextSlot then
		kParam = "STOP"
	end

	action:SetParam(0, kParam)
	
	return true

end
function Act_Twin_Sub_Trace_Ground_OnCleanUp(actor, action)
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());

	actor:DetachFrom(1996)
	actor:EndBodyTrail();
	return true
end

function Act_Twin_Sub_Trace_Ground_OnLeave(actor, action)
	return true
end

function ChooseSubPlayerAni(iNextSlot, actor, accumTime)
	local kCon = iNextSlot
	if 3>iNextSlot and 0<=iNextSlot then --idle
		--actor:GetAction():SetParamInt(7,iNextSlot)
		kCon = actor:UpdatePetActionQueue(accumTime)
	end
	return kCon
end
g_SubPlayerParticle = {"ef_Pet_emo_hungry_01_char_root", "ef_Pet_emo_sleep_01_char_root", "ef_Pet_emo_angry_01_char_root"}
function AttachBaloonParticleTest(actor, iNextSlot)
	if 3>iNextSlot or 5<iNextSlot then
		actor:DetachFrom(1996)
	else
		local kPath = g_SubPlayerParticle[iNextSlot-2]
		if nil~=kPath then
			actor:AttachParticle(1996, "p_ef_star", kPath)
		end
	end
end
