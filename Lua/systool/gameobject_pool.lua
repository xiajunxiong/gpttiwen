GameObjectPool = GameObjectPool or BaseClass()

local unityGameObjPool = Game.GameObjectPool.Singleton()

function GameObjectPool:__init()
	if GameObjPool ~= nil then
		Debuger.LogError("[GameObjectPool] attempt to create singleton twice!")
		return
	end
	GameObjPool = self

	self.path_to_asset_id = {}
end

function GameObjectPool:__delete()
	if GameObjPool == self then
		GameObjPool = nil
	end
end

--callback===function(string[error])
function GameObjectPool:RegisterPool(path,callback,freeload)
	if freeload == nil then freeload = true end
	unityGameObjPool:RegisterPool(path,freeload,System.Action_string(callback))
end

--parent===GameObject 
--return GameObject
function GameObjectPool:Get(path,parent)
	return unityGameObjPool:Get(path,parent)
end

--callback===function(GameObject)
function GameObjectPool:AsyncGet(path,callback,freeload)
	if freeload == nil then freeload = true end
	unityGameObjPool:AsyncGet(path,System.Action_UnityEngine_GameObject(callback),freeload)
end

function GameObjectPool:Release(obj)
	unityGameObjPool:Release(obj)
end

function GameObjectPool:UnregisterPool(path)
	if G_IsRebooting == true then
		return
	end
	unityGameObjPool:UnregisterPool(path)
end