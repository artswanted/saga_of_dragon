g_iMinPosX = 7
g_iMaxPosX = 203
g_iArrowPosX = 0
g_iArrowVelocity = 4

--g_iHitPointPosByPixel = 0 --24, 44, 64, ...
--g_iHitPointPosByLevel = 0 --0, 1, 2, ..., 8


bShow = true
iTwinkle = 0
iTick = 0

function InitManufactureGaugeUI()
	g_iMinPosX = 11
	g_iMaxPosX = 210
	g_iArrowPosX = g_iMinPosX
	g_iArrowVelocity = 7
	
	g_iArrowPosX = g_iMinPosX + (math.random(0, 10) * 20)
	
--	g_iHitPointPosByLevel = math.random(0, 8)
--	g_iHitPointPosByPixel = 24 + (g_iHitPointPosByLevel * 20)
	
--	local kHitPoint01 = kMainWnd:GetControl("IMG_HIT_POINT01")
--	if kHitPoint01 ~= nil and kHitPoint01:IsNil() == false then
--		kHitPoint01:SetLocation(Point2(g_iHitPointPosByPixel - 4, kHitPoint01:GetLocation():GetY()))
--	end
	
--	local kHitPoint02 = kMainWnd:GetControl("IMG_HIT_POINT02")
--	if kHitPoint02 ~= nil and kHitPoint02:IsNil() == false then
--		kHitPoint02:SetLocation(Point2(g_iHitPointPosByPixel, kHitPoint02:GetLocation():GetY()))
--	end
	local kWnd = GetUIWnd("FRM_MANUFACTURE_GAUGE")
	if kWnd ~= nil and kWnd:IsNil() == false then
		local kCtrl = kWnd:GetControl("FRM_HIT_EFFECT")
		if kCtrl ~= nil or kCtrl:IsNil() == false then
			kCtrl:Visible(false)
		end
	end
end

function OnHitEffectManufactureGaugeUI(kWnd)
	if kWnd == nil or kWnd:IsNil() then
		return
	end
	
	local kCtrl = kWnd:GetControl("FRM_HIT_EFFECT")
	if kCtrl == nil or kCtrl:IsNil() then
		return
	end
	
	kCtrl:Visible(true)
	iTwinkle = 15
	iTick = 0
end

function ShowHitEffectManufactureGaugeUI(kSelf)
	if kSelf == nil or kSelf:IsNil() or kSelf:IsVisible() == false then
		return
	end
	local kImg = kSelf:GetControl("IMG_HIT_EFFECT")
	if kImg == nil or kImg:IsNil() then
		return
	end
	
	iTick = iTick + 1
	if iTick >= 3 then
		iTick = 0
		if bShow then
			bShow = false
		else
			bShow = true
			iTwinkle = iTwinkle - 1
			if iTwinkle <= 0 then
				kSelf:Visible(false)
				return
			end
		end
		kImg:Visible(bShow)
	end
end

function UpdateManufactureGaugeUI(kMainWnd)
	if kMainWnd == nil or kMainWnd:IsNil() then
		return
	end
	kMainWnd:SetLocationToMyActor(Point2(-116, -100), "p_ef_star")
	
	if g_iArrowVelocity == 0 then
		return
	end
	
	g_iArrowPosX = g_iArrowPosX + g_iArrowVelocity
	if g_iArrowVelocity > 0 then --오른쪽 이동
		if g_iArrowPosX > g_iMaxPosX then
			g_iArrowPosX = g_iMaxPosX
			g_iArrowVelocity = g_iArrowVelocity * -1
		end
	else --왼쪽 이동
		if g_iArrowPosX < g_iMinPosX then
			g_iArrowPosX = g_iMinPosX
			g_iArrowVelocity = g_iArrowVelocity * -1
		end
	end
	
	local kArrow = UISelf:GetControl("IMG_ARROW")
	if kArrow ~= nil or kArrow:IsNil() == false then
		kArrow:SetLocation(Point2(g_iArrowPosX, kArrow:GetLocation():GetY()))
	end

end

function OnActionKeyManufactureGaugeUI()
	if g_iArrowVelocity ~= 0 then --게이지가 작동 중이라면 세우고 히트체크
		StopAndHitCheckManufactureGaugeUI()
	elseif CheckCanUseManufacture() then --정지 상태라면 리셋하고 다시 작동 (아이템 사용이 가능하다면..)
		InitManufactureGaugeUI()
	end
end

function StopAndHitCheckManufactureGaugeUI()
	g_iArrowVelocity = 0
	
	local iCenter = math.floor( ((g_iMaxPosX - g_iMinPosX) / 2) + g_iMinPosX )
	local iDist = math.abs(iCenter - g_iArrowPosX)
	local iGrade = 9
	for i = 0, 9 do --숫자가 낮을 수록 등급이 높음.
		if iDist < ((i * 10) + 10) then
			iGrade = i
			break
		end
	end
	SendReqManufacture(iGrade)
	
	return iGrade
end

function IsFinishedManufactureGaugeUI()
	return (g_iArrowVelocity == 0)
end

function GetScoreManufactureGaugeUI()
	local iCenter = math.floor( ((g_iMaxPosX - g_iMinPosX) / 2) + g_iMinPosX )
	local iDist = math.abs(iCenter - g_iArrowPosX)
	local iGrade = 9
	for i = 0, 9 do --숫자가 낮을 수록 등급이 높음.
		if iDist < ((i * 10) + 10) then
			iGrade = i
			break
		end
	end

	return iGrade
end



function OnIncCountBundleManUI(kTopWnd, bInc)
	if kTopWnd == nil or kTopWnd:IsNil() then
		return
	end
	local kMtrlWnd = kTopWnd:GetControl("ICON_MATERIAL")
	if kMtrlWnd == nil or kMtrlWnd:IsNil() or kMtrlWnd:GetCustomDataAsInt() == 0 then
		return
	end
	
	local kMtrlCount = kTopWnd:GetControl("SFRM_MTRL_COUNT")
	local kResCount = kTopWnd:GetControl("SFRM_RESULT_COUNT")
	
	if kMtrlCount:IsNil() or kResCount:IsNil() then
		return
	end
	
	local iCurCount = kMtrlCount:GetCustomDataAsInt()
	if bInc == true then
		SetBundleManMtrlCount(iCurCount+1)
	else
		if iCurCount > 1 then
			SetBundleManMtrlCount(iCurCount-1)
		end
	end
end

