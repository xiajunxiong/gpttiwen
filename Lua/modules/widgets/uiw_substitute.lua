UIWSubstitute = UIWSubstitute or DeclareMono("UIWSubstitute")

----ResPath=====String
----TypeName====String---[nil=GameObject]
----CacheLoad===Bool

--加载型子节点的序列化参数，会将值传给每个实例化的子节点
---CPKeys====String[]		
---CPVals==object[]


--OrderedCanvasKey=====String[]

--替身本身的字段名，可以返回空
local substitute_keys = 
{
	sub_uobj = true,
	sub_subject = true,
	sub_state = true,
	loading_calls = true,
}

local call_original_index_keys = 
{
	OnEnable = true,
	OnDisable = true,
	OnDestroy = true,
}
local handle_loading
local str_data

function UIWSubstitute:UIWSubstitute()
	self.sub_subject = nil
	self.sub_state = 0	--0:unload ,1:loading ,2:loaded

	self.loading_calls = nil	--尚未load完成的时候，被调用成员函数
	self.delay_timer = nil

	local meta_tab = getmetatable(self)
	local meta_tab_index = meta_tab.__index
	-- local meta_tab_newindex = meta_tab.__newindex
	meta_tab.__index = function (mono_inst,key)
			local reval

			if not substitute_keys[key] then
				if call_original_index_keys[key] then
					reval = meta_tab_index(mono_inst,key)
				else
					local subj = rawget(mono_inst,"sub_subject")
					if subj ~= nil then
						reval = subj[key]
						if reval == nil then
							reval = meta_tab_index(mono_inst,key)
						end
					else
						reval = meta_tab_index(mono_inst,key)
						if reval == nil then
							local raw_unity_mono = rawget(mono_inst,"unityMono")
							if raw_unity_mono ~= nil and raw_unity_mono:Equals(nil) == false then
								reval = BindTool.Bind(UIWSubstitute.onUnloadSubjectCall,key)
							end
						end
					end
				end
			end

			-- local reval = meta_tab_index(mono_inst,key)
			-- if reval == nil then
			-- 	if not substitute_keys[key] then				
			-- 		local subj = rawget(mono_inst,"sub_subject")
			-- 		if subj ~= nil then
			-- 			reval = subj[key]
			-- 			-- Debuger.LogErrorSD("get tofrom subject==" .. tostring(key))
			-- 		else
			-- 			-- Debuger.LogErrorSD("get tofrom SELF==" .. tostring(key))
			-- 			reval = BindTool.Bind(mono_inst.onUnloadSubjectCall,key)
			-- 		end
			-- 	end
			-- end
			return reval
		end
	meta_tab.__newindex = function(mono_inst, key, value)
			if substitute_keys[key] then
				-- Debuger.LogErrorSD("set to SELF===" .. tostring(key) ..",val===" .. tostring(value))
				rawset(mono_inst,key,value)
			else
				local subj = rawget(mono_inst,"sub_subject")
				if subj ~= nil then
					-- Debuger.LogErrorSD("set subsubj===" .. tostring(key) ..",val===" .. tostring(value))
					mono_inst.sub_subject[key] = value 
				end
			end
		end
end

function UIWSubstitute:OnEnable()
	if self.sub_state == 0 then
		self.sub_state = 1
		if self.CacheLoad == true then
			GameObjPool:AsyncGet(self.ResPath,BindTool.Bind(self.OnLoaded,self),false)
		else
			str_data = string.split(self.ResPath,"/")
			if str_data[2] == "views" then
				TimeHelper:CancelTimer(handle_loading)
				-- BuglyReport("WaitLoading SubView "..self.ResPath)
				handle_loading = TimeHelper:AddDelayTimer(BindTool.Bind(self.BeginWaitLoading, self), 0.4)
				-- self:BeginWaitLoading()
			end
			ResMgr:LoadGameObject(self.ResPath,BindTool.Bind(self.OnLoaded,self),false)
		end
	end
end
function UIWSubstitute:BeginWaitLoading()
	if handle_loading == nil then
		return
	end
	self.loaded = false
	PublicPopupCtrl.Instance:ShowWait(Language.PublicPopup.ViewLoading)
end
function UIWSubstitute:EndWaitLoading()
	str_data = nil
	TimeHelper:CancelTimer(handle_loading)
	handle_loading = nil
	self.loaded = true
	PublicPopupCtrl.Instance:HideWait()
end

function UIWSubstitute:OnDestroy()
	if self.sub_uobj ~= nil and self.CacheLoad then
		if self.sub_subject.OnRelease ~= nil then
			self.sub_subject:OnRelease()
		end
		if self.delay_timer ~= nil then
			-- self.sub_uobj:SetLocalPosition(Vector3.zero)
			if self.original_canvas_attached then
				self.can_group.enabled = true
			else
				-- UnityObj.Destroy(self.can_group_order)
				-- self.can_group_order = nil
				UnityObj.Destroy(self.can_group)
			end
			self.can_group = nil
			TimeHelper:CancelTimer(self.delay_timer)
			self.delay_timer = nil
		end
		GameObjPool:Release(self.sub_uobj)
	end
	self.sub_uobj = nil
	self.sub_subject = nil
	self.loading_calls = nil
end

function UIWSubstitute:OnLoaded(_obj)
	if str_data and str_data[2] == "views" then
		self:EndWaitLoading()
	end
	local raw_unity_mono = rawget(self,"unityMono")
	if raw_unity_mono == nil or raw_unity_mono:Equals(nil) then
	-- if self:Destroyed() then
		self.loading_calls = nil
		if self.CacheLoad == true then
			GameObjPool:Release(_obj)
		end
		return
	end
	self:SetEnable(false)
	local obj
	if self.CacheLoad == true then
		obj = _obj
	else
		_obj:SetActive(false)
		obj = UnityGameObj.Instantiate(_obj)
		_obj:SetActive(true)
	end
	self.sub_uobj = obj
	obj:SetParent(self.gameObject,false)
	if self.TypeName == nil or self.TypeName == "GameObject" then
		self.sub_subject = obj
	elseif self.TypeName == "Transform" then
		self.sub_subject = obj.transform
	else
		self.sub_subject = obj:GetLuaComponent(self.TypeName)
		if self.sub_subject ~= nil  then
			if self.CPKeys ~= nil then
				for i = 1,self.CPKeys:Length() do
					local cp_key = self.CPKeys[i]
					self.sub_subject[cp_key] = self.CPVals[i]
					self.sub_subject._serialize_checked[cp_key] = true	--序列化字段设置好让它可以被 luaMono 垃圾回收机制销毁
				end
			end
			self.unityMono = self.sub_subject.unityMono
			if self.sub_subject.OnSpawn ~= nil then
				self.sub_subject:OnSpawn()
			end
		end
	end
	if not self.CacheLoad then obj:SetActive(true) end
	if self.sub_subject ~= nil then
		if self.loading_calls ~= nil then
			for key,param in pairs(self.loading_calls) do
				local call_func = self.sub_subject[key]
				if call_func ~= nil and type(call_func) == "function" then
					call_func(self.sub_subject, unpack(param,1,param.paramCount))
				end
			end
			self.loading_calls = nil
		end
		if self.CacheLoad ~= true then	--标签页刷新会晚1~2帧，需要隐藏表现用于刷新界面防闪
			self.can_group = self.sub_uobj:GetComponent(typeof(UnityEngine.Canvas))
			self.original_canvas_attached = self.can_group ~= nil
			if not self.original_canvas_attached then
				self.can_group = self.sub_uobj:AddComponent(typeof(UnityEngine.Canvas))
				-- self.can_group_order = self.sub_uobj:AddComponent(typeof(Game.UIOrderCanvasOverrider))
				-- self.can_group_order.BlockParentEnable = true
			end
			if self.OrderedCanvasKey ~= nil then
				for i=1, self.OrderedCanvasKey:Length() do
					self.sub_subject[self.OrderedCanvasKey[i]].overrideSorting = false	--因为orderedCanvas组件 的原因canvas无法直接关闭，曲线救国关voerrideSorting
				end
			end
			self.can_group.enabled = false
				-- self.can_group.alpha = 0
				-- self.can_group.interactable = false
				-- self.sub_uobj:SetLocalPosition(Vector3.New(99999,99999,0))
				self.delay_timer = TimeHelper:AddDelayFrameTimer(function ()
					if not self:Destroyed() then
						self.can_group.enabled = true
						if self.OrderedCanvasKey ~= nil then
							for i=1, self.OrderedCanvasKey:Length() do
								self.sub_subject[self.OrderedCanvasKey[i]].overrideSorting = true
							end
						end
						if not self.original_canvas_attached then
							-- UnityObj.Destroy(self.can_group_order)
							-- self.can_group_order = nil
							UnityObj.Destroy(self.can_group)
						end
					end
					self.can_group = nil
					-- self.sub_uobj:SetLocalPosition(Vector3.zero)
					self.delay_timer = nil
				end,2)
			
		end
	else
		Debuger.LogError("UIWSubstitute:Load Failed,Path=%s,Type=%s",tostring(self.ResPath),tostring(self.TypeName))
	end
	self._type_tab = nil
	self.sub_state = 2


end

function UIWSubstitute:IsLoaded()
	if self.sub_state ~= 2 then
		return false
	else
		if self.sub_subject == nil then
			return true
		else
			return self.sub_subject.IsLoaded == nil or self.sub_subject:IsLoaded()
		end
	end
end

-- function UIWSubstitute:IsActiveAndEnabled(_obj)


function UIWSubstitute.onUnloadSubjectCall(func_name,sub_mono,...)
	if sub_mono.loading_calls == nil then
		sub_mono.loading_calls = {}
	end
	sub_mono.loading_calls[func_name] = {paramCount=select('#', ...), ...}
	return nil
end
