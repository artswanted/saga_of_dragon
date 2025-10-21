g_iMinPosX = 7
g_iMaxPosX = 203
g_iArrowPosX = 0
g_iArrowVelocity = 4

--g_iHitPointPosByPixel = 0 --24, 44, 64, ...
--g_iHitPointPosByLevel = 0 --0, 1, 2, ..., 8

function InitBasicManufactureGaugeUI()
	g_iMinPosX = 3
	g_iMaxPosX = 202
	g_iArrowPosX = g_iMinPosX
	g_iArrowVelocity = 4
	
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
end

function UpdateBasicManufactureGaugeUI(kMainWnd)
	if kMainWnd == nil or kMainWnd:IsNil() then
		return
	end
	kMainWnd:SetLocationToMyActor(Point2(-104, -100), "p_ef_star")
	
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

function OnActionKeyBasicManufactureGaugeUI(kMainWnd)
	if g_iArrowVelocity ~= 0 then --게이지가 작동 중이라면 세우고 히트체크
		StopAndHitCheckBasicManufactureGaugeUI(kMainWnd)
	else --정지 상태라면 리셋하고 다시 작동
		InitBasicManufactureGaugeUI()
	end
end

function StopAndHitCheckBasicManufactureGaugeUI(kMainWnd)
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
	--CommonMsgBox("iDist: "..iDist..", iGrade: "..iGrade)
	
	return iGrade
end



