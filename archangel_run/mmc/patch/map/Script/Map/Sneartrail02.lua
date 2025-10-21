
-- 21
----------------------------------------------------------------------------
function UserAdmissionInfo0(map)


	-- 21번맵에 들어가려면 레벨은 0 <= LV <= 100 이어야 함
	map:EnterLvAdmission(0,100)

	-- 21번맵에 들어가려면 20400001번 아이템이 1개이상 있어야 함
	map:EnterItemAdmission(20400001, 1)


end