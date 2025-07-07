AttachObj = AttachObj or BaseClass()
AttachObj.EmptyResId = 109		--无显示的资源id

----附属品逻辑个体（用于宝箱打开后展示的物品）
function AttachObj:__init(attached_scene_obj, vo)
	self.vo = vo
	if vo == nil then
		LogError("AttachObj __init vo is nil")
	end
	self.body = nil
	self.frame_pos = Vector3.zero		--同帧多次取位置缓存
	self.attached_scene_obj = attached_scene_obj
	self.collider = nil
	local attachRootPath = "actor/__AttachRoot.prefab"
	GameObjPool:RegisterPool(attachRootPath, BindTool.Bind(self.onResLoaded, self), true)
end

function AttachObj:Init(vo)
	if vo == nil then
		LogError("AttachObj Init vo is nil")
	end
	-- LogError("Init vo",vo)
	self.vo = vo
end

function AttachObj:__delete()
	-- if self.delayHide ~= nil then
	-- 	TimeHelper:CancelTimer(self.delayHide)
	-- 	self.delayHide = nil
	-- end
	if self.body ~= nil then
		self.body:DeleteMe()
		self.body = nil
	end
	if self.collider ~= nil then
		self.collider.enabled = true
	end
	if self.root_node ~= nil then
		if self.nav_agent_enabled == true then
			self.nav_agent.enabled = false
			self.nav_agent_enabled = false
		end
		GameObjPool:Release(self.root_node)
	end
	if self.vo then
		if self.vo.Val then
			self.vo:Val().root_obj = nil
		else
			self.vo.root_obj = nil
		end
	end
	self.vo = nil
    if self.res_path ~= nil then
		GameObjPool:UnregisterPool(self.res_path)
	end
end

function AttachObj:onResLoaded()
	self.root_node = GameObjPool:Get("actor/__AttachRoot.prefab",self.attached_scene_obj.root_node)

	self.anim = self.root_node:GetComponent(typeof(UnityAnim))
	-- sm data 特殊赋值
	if self.vo then
		if self.vo.Val then
			self.vo:Val().root_obj = self.root_node
		else
			self.vo.root_obj = self.root_node
		end
	else
		self.vo = CommonStruct.AttachParam({})
		self.vo.root_obj = self.root_node
	end
	self.body = self:CreateDrawer()
	-- self.root_node:SetParent(self.attached_scene_obj.root_node)
	-- self.root_node:SetLocalPosition(Vector3.zero)
	self:FlushDrawer()
end

function AttachObj:CreateDrawer()
	return SceneObjDrawer.New(self.root_node:FindChild("AttachPoint"))
end

function AttachObj:FlushDrawer()
	local data, config = AttachObj.GetDrawerData(self.vo)
	if config ~= nil and config.res_id ~= nil and config.res_id == AttachObj.EmptyResId then
		self:SetTouchAble(false)
	end
	self.body:SetData(data)
	self.body:SetLoadedCallback(function ()
		self.anim:SetTrigger(APH("Show"))
		-- self.delayHide = TimeHelper:AddDelayTimer(function ()
		-- 	self.anim:SetTrigger(APH("Hide"))
		-- end,5)
	end)
	
end

-- 默认数据
function AttachObj.GetDrawerData(vo)
	local main_p = nil
	local type = vo ~= nil and vo.obj_type or SceneObjType.None
	main_p = DrawerHelper.GetNpcPath(vo.res_id)
	if main_p == nil then
		main_p = "actor/monster/test.prefab"
	end
	return main_p
end

function AttachObj:Vo()
	return self.vo
end

function AttachObj:AttachedObjId()
	return self.attached_scene_obj.vo.obj_id
end

function AttachObj:SetVisible(is_visible)
	self.body:getObj():SetActive(is_visible)
end