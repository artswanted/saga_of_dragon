-- Melee
function Skill_ShadowLeap_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	return		true;
	
end
function Skill_ShadowLeap_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_ShadowLeap_Fire(actor,action);
	action:SetSlot(0);
	actor:PlayCurrentSlot();

end
function Skill_ShadowLeap_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:CreateActionTargetList(actor);
	
	--  펑 이펙트
	actor:AttachParticleToPoint(8776,actor:GetNodeWorldPos("char_root"),"ef_Shadowleap_01_01_char_root");
	
	--  사라진다.
	actor:HideNode("Scene Root",true);
	
	--  사라진 시간
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(4,g_world:GetAccumTime());
	
	--  State
	action:SetParamInt(3,0);

	actor:AttachSound(106,"Shadow");
	

end
function Skill_ShadowLeap_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 200
	end

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = iAttackRange
	end	

	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();

	local kParam = FindTargetParam();
	
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,iAttackRange,0,0);
	
	kParam:SetParam_3(true,FTO_NORMAL);
	local iTargets = action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
	ODS("Skill_ShadowLeap_OnFindTarget iTargets:"..iTargets.."\n");
	
	return iTargets;
end

function Skill_ShadowLeap_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local actorID = actor:GetID()
	local actionID = action:GetID()
	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_ShadowLeap_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end
	
	action:SetParamInt(100,30) -- 타겟 대상으로 부터 이동할 거리
	
	Skill_ShadowLeap_OnCastingCompleted(actor,action);
	
	return true
end

function Skill_ShadowLeap_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()

	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
	local   iState =action:GetParamInt(3);
	local iCurrentSlot = action:GetCurrentSlot();
	
	if iState == 0 then
	    local   fHideTime = action:GetParamFloat(4);
	    if accumTime - fHideTime > 0.4 then
    	
	        actor:HideNode("Scene Root",false);
    	    
	        --  리스트의 첫번째 놈 앞으로 순간이동한다.
	        local   kTargetList = action:GetTargetList();
	        local   iTargetCount = kTargetList:size();
	        if iTargetCount>0 then
        	
	            local   kTargetInfo = kTargetList:GetTargetInfo(0);
	            local   kTargetGuid = kTargetInfo:GetTargetGUID();
	            local   kTargetPilot = g_pilotMan:FindPilot(kTargetGuid);
	            if kTargetPilot:IsNil() == false then

                    local   kTargetActor = kTargetPilot:GetActor();
                    if kTargetActor:IsNil() == false then
					
					local   kTargetPos = kTargetActor:GetPos();
					
                        local kMyPos = actor:GetPos();
						local iMoveDist = action:GetParamInt(100)
                        local kDir = kTargetPos:_Subtract(kMyPos);
						kDir:Unitize();
						
						if actor:IsToLeft() == kTargetActor:IsToLeft() then
							kDir:Multiply(-iMoveDist);
						elseif actor:IsToLeft() ~= kTargetActor:IsToLeft() then
							kDir:Multiply(iMoveDist);
						end
					
						actor:ToLeft(kTargetActor:IsToLeft())
					
                        local   kNewPos = kTargetPos:_Add(kDir);
                        kNewPos = g_world:FindActorFloorPos(kNewPos);
						
						local fDistToMovePos = kNewPos:_Subtract(kMyPos)
						fDistToMovePos = fDistToMovePos:Length()
						
						local bUseNewPos = true
						kDir = kTargetPos:_Subtract(kMyPos);
						kDir:Unitize();
						--[[
						local kThrowRayPos = actor:GetLookingDir()
						kThrowRayPos:Multiply(10)
						kThrowRayPos:Add(kMyPos)
						]]
						
						local ptcl = g_world:ThrowRay(kMyPos, kDir, fDistToMovePos,1) --이동할 방향으로						
						if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then --  공간이 비어있고							
							ptcl = g_world:ThrowRay(kNewPos, Point3(0,0,-1), 500,3)			-- 바닥도 공간이 비어있으면
							if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
								ptcl = g_world:ThrowRay(kNewPos, Point3(0,0,1),iMoveDist,3); 				-- 위로도 쏴쐈는데
								if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ()== -1 then -- 공간이 없다면 
									bUseNewPos = false 							--바닥을 전혀 못찾았다면, 적 위치에 이동시킨다
									--ODS("바닥 못 찾음\n", false, 912)
								else
									--ODS("위 찾음\n", false, 912)
								end
							else
								--ODS("바닥 찾음\n", false, 912)
							end
							
						else
							--ODS("이동할 방향 벽있음\n", false, 912)
							bUseNewPos = false
						end
						
						if(bUseNewPos) then
							actor:SetTranslate(kNewPos);
						else
							actor:SetTranslate(kTargetPos);
						end
                        
	                    actor:AttachParticle(8777,"char_root","ef_Shadowleap_01_02_char_root");
						action:SetSlot(action:GetCurrentSlot()+1);
						actor:PlayCurrentSlot();
                    end
	            end
			else
				action:SetParamInt(3,1);
				action:SetParam(1, "end")
				return false;
			end

            action:SetParamInt(3,1);
	    end	
	else
		if iCurrentSlot == 1 then
			if animDone == true then
				action:SetParam(1, "end");
				return false
			end
		else		
			local   fHideTime = action:GetParamFloat(4);
			if accumTime - fHideTime > 0.4 then
				action:SetParam(1, "end")
			end
		end

	    --if animDone == true then
			
		   -- action:SetParam(1, "end")
		    --return false
	    --end
	
	end

	return true
end

function Skill_ShadowLeap_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:HideNode("Scene Root",false);
    	
	return true;
end

function Skill_ShadowLeap_OnLeave(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if actionID == "a_jump" then 
		return false;
	end
	
	if action:GetActionType()=="EFFECT" then
		
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end
	
	return false 
end


function Skill_ShadowLeap_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
		SkillHelpFunc_DefaultHitOneTime(actor,kAction, true);
	end

	return	true;
end