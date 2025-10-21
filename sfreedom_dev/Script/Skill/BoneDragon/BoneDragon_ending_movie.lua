function Act_BoneDragon_Ending_Movie_OnEnter(actor, action)

	action:SetParamInt(20,1)
	
	action:SetSlot(0)
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(1,0)

	actor:SetParam("movie", "none")
	
	return true
end

function Act_BoneDragon_Ending_Movie_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end

	action = actor:GetAction()		-- 지금 엑션 객체를 반환
	curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번

	if curSlot == 0 and actor:IsAnimationDone() then	-- 처음 에니이고 끝났다면
		local kPlay = action:GetParamInt(20)
		if kPlay == 1 then
			--무비가 있는지 확인
			local movie = actor:GetAnimationInfoFromAniName("DIE_MOVIE", "ending_movie")
			if nil~=movie and false==movie:IsNil() then
				local kParam = actor:GetParam("movie")
				if kParam=="none" then	--아직 재생 안됬다면
					g_MovieMgr:Play(movie:GetStr())
					actor:SetParam("movie", "start")
					actor:SetHide(true);
					actor:SetHideShadow(true);
					return true
				else--새생이 됬다면
					if g_MovieMgr:IsPlay() == true then	--재생 중이면 걍 리턴
						return true
					end
				end
			end
			action:SetParamInt(20,0)
			Act_BoneDragon_Ending_Movie_RemoveActor(actor)
		end
		return false
	end

	return true
end

function Act_BoneDragon_Ending_Movie_RemoveEndingActor(iNo)
	if nil==iNo or 0==iNo then
		iNo = 6000952
	end
	local kActor = g_pilotMan:FindActorByClassNo(iNo);
	
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:RemoveActorOnNextUpdate(kActor:GetPilotGuid())
	g_world:RemoveCameraAM()
	return true
end

function Act_BoneDragon_Ending_Movie_RemoveActor(actor)
	OnRecoveryMiniQuest(false)
	-- 사운드
	PlayBgSound(0)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 1.0, 0.0, 3.0, false )

	Act_BoneDragon_Ending_Movie_RemoveEndingActor(actor:GetAbil(AT_CLASS))

	if false==GetMyActor():IsNil() then
		g_world:SetCameraMode(1, GetMyActor())
	end
	g_world:SetCameraFixed(true) -- 카메라 고정

	CloseUI("FRM_MOVIE_IN")
	UI_DefaultActivate( true )
	UnLockPlayerInputMove(8) -- for aram lock
	UnLockPlayerInput(8)
	CallMapMove(false)
	g_world:SetCameraZoom(1.0) -- 줌최대뒤로
	actor:SetParam("ending", "")
end

function Act_BoneDragon_Ending_Movie_OnCleanUp(actor, action)
	Act_BoneDragon_Ending_Movie_RemoveActor(actor)
end

function Act_BoneDragon_Ending_Movie_OnLeave(actor, action)
   	if actor == nil then
		return true
	end
	if actor:IsAnimationDone() ~= true then
		return false
	end

	return true
end
