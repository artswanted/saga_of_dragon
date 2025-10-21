-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

function MULTISTRAPE_LAST_ARROW_OnLoadingStart(kProjectile)
	return	true;
end
function MULTISTRAPE_LAST_ARROW_OnLoading(kProjectile)
	return	true;
end
function MULTISTRAPE_LAST_ARROW_OnFlyingStart(kProjectile)
	return	true;
end
function MULTISTRAPE_LAST_ARROW_OnFlying(kProjectile)	
	return	true;
end

function MULTISTRAPE_LAST_ARROW_OnCollision(kProjectile,kCollideActor,kTargets)
	return 0
end

function MULTISTRAPE_LAST_ARROW_OnTargetListModified(kProjectile)
	return	false;

end

function MULTISTRAPE_LAST_ARROW_OnArrivedAtTarget(kProjectile)

	return	false;
end

function MULTISTRAPE_LAST_ARROW_OnAnimationEvent(kProjectile)
	return	true
end
