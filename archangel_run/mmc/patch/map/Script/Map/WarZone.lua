function UnitMgr_Init2000(arg)
	InfoLog(9, "UnitMgr_Init1001     00")



	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)

	kWorldAction = mng:NewWorldAction(WATYPE_BASE, WA_MOVE_TO_START_LINE)
	mng:AddWorldAction(kWorldAction)
	



	local act2 = NewActArg()
	local unitmng = arg:Get(ACTARG_UNITMNG)


	unitmng = UnitMgr(unitmng)


	act2:Set(ACTARG_UNITMNG, unitmng:GetObject())
	act2:Set(ACTARG_PUPPETMNG, mng:GetObject())


	mng:BeginPuppetTick2(kWorldAction, ETICK_INTERVAL_1S, act2)
	DeleteActArg(act2)


	InfoLog(7, "UnitManager Init lua   (Unit_Init1001) success..")
	return true
end


---------------------------------------------------------------
function WA_MOVE_TO_START_LINE_OnEnter(dwElapsedTime, arg)


	


--	local kPVP = GetPVPMgr()
--	kPVP:LOG()




	return true
end

---------------------------------------------------------------
function WA_MOVE_TO_START_LINE_OnTick(dwElapsedTime, arg)




	






	
	return true
end













