function ArrowDisplay(wndSelf, wndWrapperSelf, iCount, fTotal, fDelay, bDoing, fTime)
	local kSize = wndSelf:GetSize()
	local kPos = wndSelf:GetLocation()
	if bDoing == nil or fTime == nil then
		kSize:SetY(0)
		kPos:SetX(0)
		wndSelf:SetImgSize(kSize)
		wndSelf:SetLocation(kPos)
		return 
	end
	if bDoing == true then
		--local wnd = ItemUpgradeWnd(wndSelf)
		local iNeed = wndWrapperSelf:GetNowNeedItemCount(iCount)
		if iNeed > 0 then
			local fDelta = GetAccumTime() -fTime - fDelay
			fDelta = math.max(0, fDelta)
			if fTotal >= fDelta then
				local iH = fDelta/fTotal*kSize:GetX()
				kSize:SetX(iH)
				kPos:SetX(wndSelf:GetSize():GetX()-iH)
			else
				kPos:SetX(0)
			end
		else
			kSize:SetY(0)
			kPos:SetX(0)
		end	
	else
		local iData = wndSelf:GetCustomDataAsInt()
		if iData <= 0 then
			kSize:SetY(0)
			kPos:SetX(0)
		end
	end
	wndSelf:SetImgSize(kSize)
	wndSelf:SetLocation(kPos)
end

function ArrowReset(wndSelf)	-- 버튼이 아니라 UI몸통이라고 가정하자
	local wndColor = wndSelf:GetControl("SFRM_COLOR")
	if wndColor:IsNil() == false then
		local wndSdw = wndColor:GetControl("SFRM_SHADOW")
		if wndSdw:IsNil() == false then
			for i=1,4 do
				local wndArrowBG = wndSdw:GetControl("FRM_ARROW_BG" .. i)
				if wndArrowBG:IsNil() == false then
					local wndArrow = wndArrowBG:GetControl("IMG_ARROW")
					if wndArrow:IsNil() == false then
						local kSize = wndArrow:GetSize()
						local kPos = wndArrow:GetLocation()
						wndArrow:SetImgSize(kSize)
						kPos:SetX(0)
						wndArrow:SetLocation(kPos)
					end
				end
			end
		end
	end
end
