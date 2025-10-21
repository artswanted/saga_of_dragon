
function SetUITimer( kParentUI, iTime )

	local bFlag = false
	local index = 1
	local kUI = kParentUI:GetControl( "IMG_DIGIT_" .. index )
	while false == kUI:IsNil() do
		
		if true == bFlag then
			local iValue = iTime % 60
			local iValue2 = (iValue - (iValue % 10)) / 10
			kUI:SetUVIndex( iValue2 + 1 )
			iTime = (iTime - iValue) / 60
			
			if 0 < iValue2 or 0 < iTime then
				kUI:SetState(6)
			end
		else
			local iValue = iTime % 10
			kUI:SetUVIndex( iValue + 1 )
			
			if 0 < iTime then
				kUI:SetState(10)
			end
		end
		
		bFlag = not bFlag
		index = index + 1
		kUI = kParentUI:GetControl( "IMG_DIGIT_" .. index )
	end

end

function OnUITimer( kParentUI, index )

	local kUI = kParentUI:GetControl( "IMG_DIGIT_" .. index )
	
	local iState = kUI:GetState()
	if 0 == iState then
		return false-- 더 이상 돌지 않는다.
	end
	
	local iUVIndex = kUI:GetUVIndex()
	if 1 < iUVIndex then
		kUI:SetUVIndex( iUVIndex - 1 )
		return true
	end
	
	if true == OnUITimer( kParentUI, index + 1 ) then
		kUI:SetUVIndex(iState)
		return true
	end

	kUI:SetState(0)
	return false
end
