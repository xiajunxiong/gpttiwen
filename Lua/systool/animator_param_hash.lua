local hash_cache = {}
local unityAnimator = UnityEngine.Animator

function APH(param_name)
	local hash = hash_cache[param_name]
	if hash == nil then
		hash = unityAnimator.StringToHash(param_name)
		hash_cache[param_name] = hash
	end 
	return hash
end