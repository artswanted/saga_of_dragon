function CalcComboBonusRate(iCombo)
	local iBonusRate = 1.0--Base Rate
	
	if 100 < iCombo then
		iBonusRate = 2.0
	elseif 90 < iCombo then
		iBonusRate = 1.9
	elseif 80 < iCombo then
		iBonusRate = 1.8
	elseif 70 < iCombo then
		iBonusRate = 1.7
	elseif 60 < iCombo then
		iBonusRate = 1.6
	elseif 50 < iCombo then
		iBonusRate = 1.5
	elseif 40 < iCombo then
		iBonusRate = 1.4
	elseif 30 < iCombo then
		iBonusRate = 1.3
	elseif 20 < iCombo then
		iBonusRate = 1.2
	elseif 10 < iCombo then
		iBonusRate = 1.1
	elseif 0 < iCombo then
		iBonusRate = 1.0
	end
	
	return iBonusRate
end
