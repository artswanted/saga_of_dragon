iAccumAngle = 0
fRotRad = 0 
iTotalAngle = 0		-- 총 회전해야 하는 각도
fBlockExploInter = 0.4		-- 한 블럭을 회전하는데 걸리는 시간.

function Act_SP_Attk_11_OnEnter(actor, action)
	action:SetParam(0,0)
	action:SetParam(1,0.0)
	iAccumAngle = 0			-- 각도 초기화
	fRotRad = 0
	if iExplotionNum > 0 then
		iTotalAngle = math.rad(iExplotionNum * 10)
	end
	bQuakedByHand = false
	return true
end
--frameTime * 1초당 움직여야 하는 라디안 값
function Act_SP_Attk_11_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end
	Nowaction = actor:GetAction()		-- 지금 액션 객체를 반환
	curSlot = Nowaction:GetCurrentSlot()	-- 액션이 플레이하고 있는 액니 순번

	if Nowaction:GetParamFloat(0) == 0 then
		Nowaction:SetParam(0, accumTime)
	end

	if curSlot == 0 and actor:IsAnimationDone() then	-- 처음 에니이고 끝났다면
		actor:PlayNext()
--		fRotRad = 0.290888 -- math.rad(한블럭당10도)/한블럭당 0.6초
--		fRotRad = 0.581776 -- math.rad(한블럭당10도)/한블럭당 0.3초
		fRotRad = 0.4363323 -- math.rad(한블럭당10도)/한블럭당 0.3초
	elseif curSlot == 2 and actor:IsAnimationDone() then		-- 세번째 애니 끝
		if iPunchBlockNum ~= -1 then
			iAngle = iPunchBlockNum*360/iTotalBlock			-- 보스가 쳐다봐야 할 각도
			iRad = math.rad(iAngle)
			LookPoint = Point3(math.cos(iRad),math.sin(iRad),0)
			LookPoint:Unitize()					-- 단위벡터로 만들기
			LookPoint:SetX(LookPoint:GetX() * iBlockFromCentor)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
			LookPoint:SetY(LookPoint:GetY() * iBlockFromCentor)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌
			LookPoint:SetZ(0)	-- 쳐다봐야 할 위치. 단위벡터에 길이를 곱해줌

			actor:LookAt(LookPoint)			-- 쳐다보게 만들기
		end

		actor:PlayNext()
		actor:SetAnimSpeed(fInterByMulti)
		if Nowaction:GetParamFloat(1) == 0.0 then
			Nowaction:SetParam(1, accumTime)
			bQuakedByHand = false
		end
	elseif curSlot == 3 then
		if actor:IsAnimationDone() then		-- 손 다 내려 왔음
			actor:PlayNext()
			actor:SetAnimSpeed(1)
			if iDownPuppetID4 ~= -1 then	-- 
  				puppets[iDownPuppetID4]:TransitAction("up")
				iDownPuppetID4 = -1
	  		end
		elseif bQuakedByHand == false and (accumTime - Nowaction:GetParamFloat(1)) > iQuakedByHandTime/4 then
			bQuakedByHand = true		-- 화면 진동
			actor:AttachParticle(12361, "Bip01 L Finger2", "e_b_dragonsapphire_hand_attk")
		end
	elseif curSlot == 4 then
		if (accumTime - Nowaction:GetParamFloat(1)) >= iGrapTime then
			actor:PlayNext()
		end
	elseif curSlot == 5 and actor:IsAnimationDone() then		-- 애니 다 끝
		return false	
	end

	if fRotRad ~= 0 then -- and accumTime - Nowaction:GetParamFloat(0) >= i1stAniTime/1000 then
		actor:IncRotate( frameTime * fRotRad )	-- 보스 회전
		iAccumAngle = iAccumAngle + frameTime * fRotRad
	end

	if iAccumAngle >= iTotalAngle and fRotRad ~= 0 then
		actor:PlayNext()
		actor:SetAnimSpeed(fInterByMulti)	
		fRotRad = 0
	end

	return true
end

function Act_SP_Attk_11_OnCleanUp(actor, action)
end

function Act_SP_Attk_11_OnLeave(actor, action)
	if action:GetID() == "a_dmg" then
		return false
	end
	
	bQuakedByHand = false
	return true
end
