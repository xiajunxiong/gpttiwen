require("managers/view_masker")

ViewManager = ViewManager or BaseClass()

ViewLayer = {
	Buttom = 10,
	Normal = 20,
	Top = 30,
	IndepTop = 32701,	--最上层的层级，不会纳入层级栈，目前只给省电模式使用
}

--窗口弹窗等级,决定窗口之间的互斥
ViewLevel = {
	L0 = 1,		--主界面，组队等全屏界面
	L1 = 2,		--1级弹框，背包，人物等
	Lu = 3,		--唯一界面 npc对话界面等,打开时所有其他界面关闭或隐藏
	Ls = 4,		--小界面 不设置默认为小界面
	Lt = 5,		--不参与互斥的界面(除了Lu) top_layer_view等
}

--窗口遮罩方式
ViewMask = {
	None = 0,
	Block = 1,				--只遮挡下层事件
	BgBlock = 2,			--遮挡下层事件并显示半透明背景
	BlockClose = 3,			--遮挡下层事件并点击关闭
	BgBlockClose = 4,		--遮挡下层事件并显示半透明背景且点击关闭
	BgBlockNoAnim = 5,		--遮挡下层事件并显示半透明背景(无蒙版动画)
	BgBlockCloseNoAnim = 6,	--遮挡下层事件并显示半透明背景且点击关闭(无蒙版动画)
}

ViewState = {
--	None = -1,
	Closed = 0,		--关闭中，持续状态
	Loading = 1,	--正在加载，异步状态
	Opening = 2,	--正在打开，数据装载，动画播放等。异步状态
	Opened = 3,		--打开中，持续状态
	Closing = 4,	--正在关闭，数据卸载，动画播放等，异步状态
}

ViewManager.TouchDownType = {
	None = -1,
	Scene = 0,
	CommonUI = 1,
	MainTask = 2,
	ItemInfo = 3,
	RbButton = 4,
	GQRN = 5,--公会答题同屏人数
}

local uiMgr = Game.UIManager.Singleton()
local uiMgrObj = uiMgr.gameObject

function ViewManager:__init()
	if ViewMgr ~= nil then
		Debuger.LogError("[ViewManager] attempt to create singleton twice!")
		return
	end
	ViewMgr = self

	self.view_list = {}	--key:view_tab,value:view_info

	self.view_stack = {}	--view_info stack  upper view has bigger index 
	self.level_view_stack = {
		[ViewLevel.L0] = {},
		[ViewLevel.L1] = {},
		[ViewLevel.Ls] = {},
		[ViewLevel.Lu] = {},
		[ViewLevel.Lt] = {},
	}
	self.layer_parents = {}

	self.wait_list = {}

	self.flush_list = {}

	local mask_obj = uiMgrObj:FindChild("Mask")
	self.mask = mask_obj:GetLuaComponent(ViewMasker:GetTypeName())

	self.sorting_order = {}
	self.open_by_key_func = {}
	self.view_open_close_cmd = {}
	self.reslution_data = SmartData.New({val = false})
	self.touch_down_data = SmartData.New({frameCount = 0,type = ViewManager.TouchDownType.None})
end

function ViewManager:__delete()
	if ViewMgr == self then
		ViewMgr = nil
	end
end

local function get_view_object(view_info,callback)
	-- view_info.flush_tab[BaseView.FlushAll] = true
	if view_info.cache_obj ~= nil then
		view_info.cache_obj:SetActive(true)
		callback(view_info.cache_obj)
		TimeHelper:CancelTimer(view_info.handle_loading)
	else
		local c_b = nil
		if view_info.view_tab.cache == true then
			c_b = function (obj)
				obj:SetParent(uiMgrObj,false)
				view_info.view_mono = obj:GetLuaComponent(view_info.view_tab:GetTypeName())
				view_info.cache_obj = obj
				callback(view_info.cache_obj)
			end
		else
			c_b = function (obj)
				obj:SetParent(uiMgrObj,false)
				view_info.view_mono = obj:GetLuaComponent(view_info.view_tab:GetTypeName())
				callback(obj)
			end
		end
		ResMgr:LoadGameObject(view_info.view_tab.res_path,c_b,true,false, function ()
			PublicPopupCtrl.Instance:Center("界面打开失败请重试")
			ViewMgr:EndWaitLoading()
			TimeHelper:CancelTimer(view_info.handle_loading)
			view_info.state = ViewState.Opened
			ViewMgr:closeViewInternal(view_info.view_tab)
		end)
	end
end

local function remove_view_object(view_info,obj)
	if view_info.view_tab.cache == true then
		obj:SetActive(false)
	else
		UnityGameObj.Destroy(obj)
		view_info.view_mono = nil
	end
end

function ViewManager:AddOrder(layer)
	local order
	if layer == ViewLayer.IndepTop then
		order = layer
	else
		local order_stack = self.sorting_order[layer]
		if order_stack == nil then
			order_stack = {}
			self.sorting_order[layer] = order_stack
		end
		if #order_stack == 0 then
			order = layer * 1000
		else
			order = order_stack[#order_stack] + 100
		end
		table.insert(order_stack,order)
		return order, order_stack
	end
	return order
end

function ViewManager:RemoveOrder(order, order_stack)
	if order == ViewLayer.IndepTop then
		return
	end
	local layer = math.floor(order / 1000)
	order_stack = order_stack or self.sorting_order[layer]
	if order_stack ~= nil then
		for i=#order_stack,1,-1 do
			if order_stack[i] == order then
				table.remove(order_stack,i)
				return
			end
		end
	else
		for _, stack in pairs(self.sorting_order) do
			for i, value in ipairs(stack) do
				if value == order then
					table.remove(stack, i)
					return
				end
			end
		end
	end
end

function ViewManager:getViewInfo(view_key)
	local view_info = self.view_list[view_key]
	if view_info == nil then
		view_info = {
			view_tab = view_key,		--view type tab
			view_mono = nil,			--view luamonobehaviour
			state = ViewState.Closed,	--state
			data = {},					--open data
			is_done = false,			--has state done,goto next state if has done 
			behaviour_cache = nil,		--open/close behaviour cache
			-- act_cache = nil 
			sorting_order = 0,
			flush_tab = {},
			cach_obj = nil,		--cache gameobject
			}
		self.view_list[view_key] = view_info
	end
	return view_info
end

local function createBehaviourCache()
	return {
		is_open = false,
		data = nil,
	}
end
--注册对应界面的mod_key打开时需要调用的接口（如穹顶界面不能直接OpenView）
function ViewManager:RegisterOpenKeyFunc(mod_key,func)
	self.open_by_key_func[mod_key] = func
end
--================================ ViewMgr 外部接口================================
function ViewManager:OpenViewByKey(mod_key, dont_check)
	mod_key = ModuleChangeConfig[mod_key] or mod_key

	if dont_check == nil then
		local opened, open_tip = FunOpen.Instance:GetFunIsOpened(mod_key)
		if not opened then
			PublicPopupCtrl.Instance:Center(open_tip)
			return
		end
	end
	local key, param_t = Mod.ParseKey(mod_key)
	local func = ModOpenViewFunc[mod_key] or ModOpenViewFunc[key]
	if func ~= nil and func(mod_key, key, param_t) then
		return
	end
	if Mod.IsView(mod_key) then
		if self:IsOpen(ModToView[mod_key]) then
			self:SelectTabbar(mod_key)
		elseif self.open_by_key_func[mod_key] ~= nil then
			self.open_by_key_func[mod_key](param_t)
		else
			ViewMgr:OpenView(ModToView[mod_key],param_t)
		end
		return
	end
	if Mod.IsView(key) then
		if self:IsOpen(ModToView[key]) then
			self:SelectTabbar(mod_key)
		else
			ViewMgr:OpenView(ModToView[key],param_t)
		end
	end
end
function ViewManager:SelectTabbar(mod_key)
	local key, param_t = Mod.ParseKey(mod_key)
	if ModToView[key] == nil then
		return
	end
	local view_info = self:getViewInfo(ModToView[key])
	if view_info and view_info.view_mono then
		view_info.view_mono:SelectTabbar(mod_key)
	else
		view_info.select_info = param_t
	end
end
--只能关闭系统模块界面，其他mod_key关闭用CloseView
function ViewManager:CloseViewByKey(mod_key)
	local key, param_t = Mod.ParseKey(mod_key)
	ViewMgr:CloseView(ModToView[key])
end

function ViewManager:OpenView(view,data)
	if not uiMgrObj.activeSelf then
		return
	end
	if IS_AUDIT_VERSION and ViewToMod[view] ~= nil then
		RoleCtrl.Instance:SendAuditBehaviourSubmitReq(ViewToMod[view])
		-- LogError("ViewToMod ",ViewToMod[view]," == nil is",ViewToMod[view] == nil)
	end
	if view == nil then
		Debuger.LogErrorSD("ViewManager:OpenView view is nil")
	end
	for i,v in ipairs(self.view_open_close_cmd) do
		if v.view == view then
			table.remove(self.view_open_close_cmd,i)
			break
		end
	end
	table.insert(self.view_open_close_cmd,{view=view,data=data or {},open=true})
end

function ViewManager:CloseView(view)
	if view == nil then
		Debuger.LogErrorSD("ViewManager:CloseView")
	end	
	for i,v in ipairs(self.view_open_close_cmd) do
		if v.view == view then
			table.remove(self.view_open_close_cmd,i)
			break
		end
	end
	table.insert(self.view_open_close_cmd,{view=view,open=false})
end

function ViewManager:openViewInternal(view,data)

	local view_info = self:getViewInfo(view)
	if view_info.state == ViewState.Closing then
		local b_cache = createBehaviourCache()
		b_cache.is_open = true
		b_cache.data = data
		view_info.behaviour_cache = b_cache
		return
	elseif view_info.state ~= ViewState.Closed then	--has open
		if view_info.behaviour_cache ~= nil then
			view_info.behaviour_cache = nil
		end
		return
	end
	YinYangCtrl.Instance:WhenViewChange(view)
	view_info.data = data or {};
	view_info.behaviour_cache = nil
	view_info.is_done = true
	local view_tab = view_info.view_tab
	local order, order_stack = self:AddOrder(view_tab.layer)
	view_info.sorting_order = order
	view_info.order_stack = order_stack

	--open block mask
	if ViewMasker.IsBlock(view_tab.block_mask) then
		self.mask:BlockMask(view_tab,ViewMasker.BlankClose(view_tab.block_mask),view_info.sorting_order)
	end 

	table.insert(self.view_stack,view_info)
	table.insert(self.wait_list, 1, view_info)

	--存在打开的界面 等关掉后再执行寻路后的行为，除了巡逻
	--还是不好判断啊。艹
	--npc对话无法直接打开
	--[[ if MainPlayer and not SceneLogic.Instance:MoveCache().guaji then
		local other = false
		for i=#self.view_stack,1,-1 do
			local value = self.view_stack[i]
			if value.view_tab.view_level ~= ViewLevel.Lt then
				if value.view_tab._type_name ~= "MainView" and
				value.view_tab._type_name ~= "NpcDialogView" then
					other = true
					break
				end
			end
		end
		if other then
			SceneLogic.Instance:BreakOff(true)
		end
	end ]]
	--[[ if NotHinderList[view_info.view_tab._type_name] == nil then
		MainPlayer:StopMove()
	end ]]
end

function ViewManager:GetCanvasOrder(view)
	local view_info = self:getViewInfo(view)
	return view_info.sorting_order
end

function ViewManager:closeViewInternal(view)
	local view_info = self:getViewInfo(view)
	if view_info.state == ViewState.Loading or view_info.state == ViewState.Opening then
		local b_cache = createBehaviourCache()
		b_cache.is_open = false
		view_info.behaviour_cache = b_cache
		return
	elseif view_info.state ~= ViewState.Opened then
		if view_info.behaviour_cache ~= nil then
			view_info.behaviour_cache = nil
		end
		return
	end
	YinYangCtrl.Instance:WhenViewChange(view, true)
	view_info.behaviour_cache = nil
	view_info.is_done = true
	table.insert(self.wait_list, 1, view_info)

	for i=#self.view_stack,1,-1 do
		if self.view_stack[i] == view_info then
			table.remove(self.view_stack,i)
			break
		end
	end
	--[[ if MainPlayer and not SceneLogic.Instance:MoveCache().guaji then
		--关闭界面的时候，判断是否存在打开的界面
		local other = false
		for i=#self.view_stack,1,-1 do
			local value = self.view_stack[i]
			if value.view_tab.view_level ~= ViewLevel.Lt then
				if value.view_tab._type_name ~= "MainView" then
					other = true
					break
				end
			end
		end
		if other == false then
			SceneLogic.Instance:Continue()
		end
	end ]]
	TaskCtrl.Instance:ViewCloseCheck(view.mod_key)
	NpcDialogCtrl.Instance:ViewCloseCheck()
end

function ViewManager:FlushView(view,...)
	local view_info = self:getViewInfo(view)
	local arg = { ... }
	if #arg == 0 then
		view_info.flush_tab[BaseView.FlushAll] = true
	else
		for k,v in pairs(arg) do
			view_info.flush_tab[v] = true
		end
	end
	if view_info.state == ViewState.Opening or view_info.state == ViewState.Opened then
		table.insert(self.flush_list,view_info)
	end
end

function ViewManager:IsOpen(view)
	local view_info = self:getViewInfo(view)
	return view_info.state == ViewState.Opened
end

function ViewManager:IsOpenByModKey(mod_key)
	local key, param_t = Mod.ParseKey(mod_key)
	return self:IsOpen(ModToView[key])
end

function ViewManager:IsClosed(view)
	local view_info = self:getViewInfo(view)
	return view_info.state == ViewState.Closed
end

function ViewManager:Update()
	if #self.view_open_close_cmd > 0 then
		for i,v in ipairs(self.view_open_close_cmd) do
			if v.open == true then
				self:openViewInternal(v.view,v.data)
			else
				self:closeViewInternal(v.view)
			end
		end
		self.view_open_close_cmd = {}
	end

	local wait_count = #self.wait_list
	if wait_count > 0 then
		for i = wait_count,1,-1 do
			local view_info = self.wait_list[i]
			if view_info.is_done == true then
				if view_info.state == ViewState.Closed then
					self:threadLoading(view_info)

				elseif view_info.state == ViewState.Loading then
					if view_info.behaviour_cache ~= nil and view_info.behaviour_cache.is_open == false then
						view_info.behaviour_cache = nil
						for i=#self.view_stack,1,-1 do
							if self.view_stack[i] == view_info then
								table.remove(self.view_stack,i)
								break
							end
						end
						self:onCloseLevelAndMaskBg(view_info)
						self:threadClosed(view_info)
						table.remove(self.wait_list,i)
					else
						self:threadOpening(view_info)
					end

				elseif view_info.state == ViewState.Opening then
					self:threadOpened(view_info)
					table.remove(self.wait_list,i)
					if view_info.behaviour_cache ~= nil and view_info.behaviour_cache.is_open == false then
						view_info.behaviour_cache = nil
						self:CloseView(view_info.view_tab)
					end

				elseif view_info.state == ViewState.Opened then
					self:threadClosing(view_info)

				-- elseif view_info.state == ViewState.Closing then
				-- 	self:threadClosed(view_info)
				-- 	table.remove(self.wait_list,i)
				-- 	if view_info.behaviour_cache ~= nil and view_info.behaviour_cache.is_open == true then
				-- 		local cache_data = view_info.behaviour_cache.data
				-- 		view_info.behaviour_cache = nil
				-- 		self:OpenView(view_info.view_tab,cache_data)
				-- 	end

				end
			end
		end
	end
	local flush_count = #self.flush_list
	if flush_count > 0 then
		local fl = self.flush_list
		self.flush_list = {}
		for i = flush_count,1,-1 do
			local view_info = fl[i]
			if view_info.view_mono ~= nil then
				local flush_tab = view_info.flush_tab
				view_info.flush_tab = {}
				view_info.view_mono:Flush(flush_tab)
			end
		end
	end
end


function ViewManager:threadLoading(view_info)
	view_info.state = ViewState.Loading
	view_info.is_done = false

	local view_tab = view_info.view_tab
	get_view_object(view_info,function (view_obj)

		local canvas = view_obj:GetComponent(typeof(UnityEngine.Canvas))
		canvas.overrideSorting = true
		canvas.sortingOrder = view_info.sorting_order-- self:AddOrder(view_tab.layer)
		canvas.enabled = false
		view_info.view_mono.canvas = canvas
		if view_info.view_tab._type_name ~= "WaitView" then
			TimeHelper:CancelTimer(view_info.handle_loading)
			self:EndWaitLoading()
		end
		if view_info.select_info then
			view_info.data = view_info.select_info
			view_info.select_info = nil
		end
		view_info.view_mono:StartLoading(function()
			view_info.is_done = true
		end,view_info.data)
	end)
	if ViewMgr:IsOpen(MainView) and view_tab._type_name ~= "WaitView" and view_info.cache_obj == nil then
		view_info.handle_loading = TimeHelper:AddDelayTimer(BindTool.Bind(self.BeginWaitLoading, self, view_info), 2)
	end
end
function ViewManager:BeginWaitLoading(view_info)
	self.loaded = false
	PublicPopupCtrl.Instance:ShowWait(Language.PublicPopup.ViewLoading)
	BuglyReport("WaitLoading View "..view_info.view_tab._type_name)
end
function ViewManager:EndWaitLoading()
	self.loaded = true
	PublicPopupCtrl.Instance:HideWait()
end

function ViewManager:threadOpening(view_info)
	view_info.state = ViewState.Opening
	view_info.is_done = false
	local view_tab = view_info.view_tab
	if ViewMasker.ShowBg(view_tab.block_mask) then
		self.mask:BgMask(view_tab,view_info.sorting_order)
	end
	if view_tab.view_level ~= nil then
		if ViewLevel.Lu == view_tab.view_level then
			for level, monos in pairs(self.level_view_stack) do
				if ViewLevel.L0 == level and 0 ~= #monos then
					for i = 1 ,#monos do
						if monos[i].view_mono ~= nil then
							monos[i].view_mono:SetVisible(false)
							if monos[i].view_tab._type_name == "MainView" then
								break
							end
						end
					end
					-- local stack = self.level_view_stack[ViewLevel.L0]
					-- local view_mono = stack[1].view_mono
					-- if view_mono ~= nil then
					-- 	view_mono:SetVisible(false)
					-- end
				elseif ViewLevel.Lt == level then
					local stack = self.level_view_stack[ViewLevel.Lt]
					for k, v in pairs(stack) do
						local view_mono = v.view_mono
						if view_mono ~= nil and LtNotJoinLuLogic[view_mono._type_name] ~= 1 then
							view_mono:SetVisible(false)
						end
					end
				else
					for _, mono in pairs(monos) do
						self:CloseView(mono.view_tab)
					end
				end
			end
			self.mask:BgMask(view_tab, view_info.sorting_order)
			local stack = self.level_view_stack[view_tab.view_level]
			table.insert(stack,1,view_info)
		else
			local stack = self.level_view_stack[view_tab.view_level]
			if view_tab.view_level ~= ViewLevel.Lt then
				if #stack ~= 0 then
					local view_mono = stack[1].view_mono
					if view_mono ~= nil then
						view_mono:SetVisible(false)
					end
				end
			end
			table.insert(stack,1,view_info)
		end
	else
		local stack = self.level_view_stack[ViewLevel.Ls]
		stack[view_info] = view_info
	end

	view_info.view_mono:StartOpening(function()
			view_info.is_done = true
		end,view_info.data)
	view_info.data = {}

	-- table.insert(self.flush_list,view_info)
end

function ViewManager:threadOpened(view_info)
	view_info.state = ViewState.Opened
	view_info.is_done = false
end

function ViewManager:threadClosing(view_info)
	view_info.state = ViewState.Closing
	
	self:onCloseLevelAndMaskBg(view_info)

	view_info.is_done = false
	if view_info.view_mono == nil then
		--直接清理
		view_info.is_done = true
		self:threadClosed(view_info)
		for i=1,#self.wait_list do
			if self.wait_list[i] == view_info then
				table.remove(self.wait_list,i)
				break
			end
		end
		return
	end
	view_info.view_mono:StartClosing(function()
			view_info.is_done = true
			self:threadClosed(view_info)

			for i=1,#self.wait_list do
				if self.wait_list[i] == view_info then
					table.remove(self.wait_list,i)
					break
				end
			end
			if view_info.behaviour_cache ~= nil and view_info.behaviour_cache.is_open == true then
				local cache_data = view_info.behaviour_cache.data
				view_info.behaviour_cache = nil
				self:OpenView(view_info.view_tab,cache_data)
			end
		end)
end

function ViewManager:threadClosed(view_info)
	--move view mask
	if view_info.view_mono then
		view_info.view_mono:CloseCallback()
	end
	if self.mask:BlockViewTab() == view_info.view_tab then
		local next_block_view_info = nil
		for i=#self.view_stack,1,-1 do
			if self.view_stack[i] ~= view_info then
				local vi = self.view_stack[i]
				if ViewMasker.IsBlock(vi.view_tab.block_mask) then
					next_block_view_info = vi
					break
				end
			end
		end
		if next_block_view_info ~= nil then
			local vt = next_block_view_info.view_tab
			self.mask:BlockMask(vt,ViewMasker.BlankClose(vt.block_mask),next_block_view_info.sorting_order)
		else
			self.mask:CloseBlock()
		end
	end

	view_info.state = ViewState.Closed
	view_info.is_done = false
	self:RemoveOrder(view_info.sorting_order, view_info.order_stack)
	if view_info.view_mono then
		remove_view_object(view_info,view_info.view_mono.gameObject)
	end
end

--关闭界面的时候的level和mask_bg设置，当界面正在打开途中被关闭的情况下会被调用，与threadClosed配合服用
function ViewManager:onCloseLevelAndMaskBg(view_info)
	if view_info.view_tab.view_level ~= nil then
		local stack = self.level_view_stack[view_info.view_tab.view_level]
		if #stack ~= 0 then
			for index, value in ipairs(stack) do
				if value == nil or value.view_mono == nil then
					table.remove(stack, index)
				end
			end
			if stack[1] == view_info then
				table.remove(stack,1)
			end
--------------------因为lu会排斥lt，所以在lt关闭时如果有lu在场则不自动打开下层lt继续保持关闭状态
-------------------#49940 需求 G6-LineA2（新版本开发线-2） 部分界面解除省电模式后，快捷窗口问题
			local check_mutex = true
			if view_info.view_tab.view_level == ViewLevel.Lt then
				check_mutex = (#self.level_view_stack[ViewLevel.Lu] == 0)
			end
------------------------------------------------------------------------------------------
			if check_mutex and #stack > 0 then
				local view_mono = stack[1].view_mono
				if view_mono ~= nil then
					view_mono:SetVisible(true)
				end
			end
		end
		if ViewLevel.Lu == view_info.view_tab.view_level then
			local stack_l0 = self.level_view_stack[ViewLevel.L0]
			if #stack_l0 ~= 0 then
				if #stack_l0 > 0 then
					local view_mono = stack_l0[1].view_mono
					if view_mono ~= nil then
						view_mono:SetVisible(true)
					end
				end
			end
			local stack_lt = self.level_view_stack[ViewLevel.Lt]
			for k, v in pairs(stack_lt) do
				local view_mono = v.view_mono
				if view_mono ~= nil then
					view_mono:SetVisible(true)
				end
			end
		end
	else
		local stack = self.level_view_stack[ViewLevel.Ls]
		stack[view_info] = nil
	end

	--	Lu判断:极限情况下,在Lu打开的瞬间打开L1界面,在L1关闭的时候存在L0时不会隐藏,导致Lu被L0透了,这边的操作是在L1关闭时如果存在Lu则重新检测存在的L0并隐藏
	-- LogError(" #self.level_view_stack[ViewLevel.Lu] > 0 ", #self.level_view_stack[ViewLevel.Lu] > 0 )
	if #self.level_view_stack[ViewLevel.Lu] > 0 then
		local stack1 = self.level_view_stack[ViewLevel.L0][1]
		if stack1 then
			local view_mono = stack1.view_mono
			local view_tab = stack1.view_tab
			if view_tab and "MainView" == view_tab._type_name then
				if view_mono ~= nil then
					view_mono:SetVisible(false)
				end
			end
		end
	end
	--	Lu判断

	--move bgmask 
	if self.mask:BgMaskViewTab() == view_info.view_tab then
		local next_mask_view_info = nil
		for i=#self.view_stack,1,-1 do
			if self.view_stack[i] ~= view_info then
				local vi = self.view_stack[i]
				if ViewMasker.ShowBg(vi.view_tab.block_mask) then
					next_mask_view_info = vi
					break
				end
			end
		end
		if next_mask_view_info ~= nil then
			self.mask:BgMask(next_mask_view_info.view_tab,next_mask_view_info.sorting_order)
		else
			self.mask:BgMask(nil)
		end
	end
end

function ViewManager:SetScenePos(tran,pos)
	uiMgr:PosSceneToUI(tran,pos)
end

local unload_gamelogic_white = nil
function ViewManager:OnUnloadGameLogic()
	if unload_gamelogic_white == nil then
		unload_gamelogic_white = {
			[MainView] = true,
			[TopLayerView] = true,
			[WaitView] = true,
			[LoginView] = true,
			[RoleCreateView] = true,
			[RoleSelectView] = true,
			[WelcomeView] = true,
			[LoadingView] = true,
			[GuideView] = true,
		}
	end
	for _,v_info in pairs(self.view_stack) do
		if unload_gamelogic_white[v_info.view_tab] == true then
		else
			self:CloseView(v_info.view_tab)
		end 
	end
end

function ViewManager:MainViewVisible(value)
	if self:IsOpen(MainView) then
		local stack = self.level_view_stack[ViewLevel.L0]
		-- 如果已经打开了唯一类型的界面则不显示主界面
		if value and next(self.level_view_stack[ViewLevel.Lu]) then return end
		for _, view_info in pairs(stack) do
			local view_tab = view_info.view_tab
			if view_tab and "MainView" == view_tab._type_name then
				local view_mono = view_info.view_mono
				if view_mono ~= nil then
					view_mono:SetVisible(value)
				end
				break
			end
		end
	else
		if value then
			self:OpenView(MainView)
		else
			self:CloseView(MainView)
		end
	end
end

function ViewManager:ExistOtherOpenView()
	local other_open = false
	-- local test_list = {}
	-- for i=#self.view_stack,1,-1 do
	-- 	local value = self.view_stack[i]
	-- 	table.insert( test_list, value.view_tab )
	-- end 
	if GuideManager.Instance:IsGuide() then
		return true
	end

	for i=#self.view_stack,1,-1 do
		local value = self.view_stack[i]
		if value.view_tab.view_level ~= ViewLevel.Lt then
			if value.view_tab._type_name ~= "MainView" and
			value.view_tab._type_name ~= "NpcDialogView" and
			value.view_tab._type_name ~= "KeyUseView" and
			value.view_tab._type_name ~= "GuildHongBaoTipView" then
				other_open = true
				break
			end
		elseif value.view_tab._type_name == "CowBoyGameView" or
			value.view_tab._type_name == "CowBoyLevelView" then
				other_open = true
				break
		end
	end
	return other_open
end

--关闭某些界面
function ViewManager:CloseViewInBattle()
	self.view_cache = nil
	local top_i =0
	for i=#self.view_stack,1,-1 do
		local value = self.view_stack[i]
		if (type(value.view_tab.mod_key) == "number"
		and Config.fight_other_auto.battle_close[value.view_tab.mod_key]) 
		or Config.fight_other_auto.battle_close[value.view_tab._type_name] then 
			if i >= top_i then
				top_i = i
				self.view_cache = value.view_tab
			end
			self:CloseView(value.view_tab)
		end
	end
end
function ViewManager:TipInBattle()
	if #self.view_stack >= 4 then
		PublicPopupCtrl.Instance:Center(Language.Main.Battle.WarningInBattle)
	end
end

-- LtNotAutoTask的View是否有打开
function ViewManager:IsOpenLtNotAutoTaskView()
	self.view_cache = nil
	local top_i =0
	for i=#self.view_stack,1,-1 do
		local value = self.view_stack[i]
		if LtNotAutoTask[value.view_tab._type_name] then
			if self:IsOpen(value.view_tab) then
				return true
			end
		end
	end
	return false
end

function ViewManager:CloseAllViewInBattle()
	self.view_cache = nil
	local top_i =0
	for i=#self.view_stack,1,-1 do
		local value = self.view_stack[i]
		if NoCloseInStory[value.view_tab._type_name] then
		else
			if i >= top_i then
				top_i = i
				self.view_cache = value.view_tab
			end
			self:CloseView(value.view_tab)
		end
	end
end

function ViewManager:CloseViewInGuide(guideId)
	self.view_cache = nil
	local top_i =0
	for i=#self.view_stack,1,-1 do
		local value = self.view_stack[i]
		if NoCloseInGuide[value.view_tab._type_name] or NoCloseInGuideId[guideId] then
		else
			if i >= top_i then
				top_i = i
				self.view_cache = value.view_tab
			end
			self:CloseView(value.view_tab)
		end
	end
end
function ViewManager:ViewCache()
	return self.view_cache
end

function ViewManager:ViewOverrideSortingOpen()
	for level, monos in pairs(self.level_view_stack) do
		for _, view_info in pairs(monos) do
			local view_tab = view_info.view_tab
			if view_tab then
				local view_mono = view_info.view_mono
				if view_mono ~= nil then
					view_info.view_mono.canvas.overrideSorting = true
				end
			end
		end
	end
end

function ViewManager:CareTouchDownEvent(func)
	return self.touch_down_data:Care(func,"frameCount")
end

function ViewManager:UncareTouchDownEvent(handle)
	self.touch_down_data:Uncare(handle)
end

function ViewManager:OnTouchDownTrigger(type)
	if math.abs(self.touch_down_data.frameCount - Time.frameCount) <= (IS_IOS() and 1 or 0) then
		return
	end
	self.touch_down_data.frameCount = Time.frameCount
	self.touch_down_data.type = type
end

function ViewManager:CheckReslutionData()
	self.reslution_data.val = not self.reslution_data.val
end

--获取UI主摄像机 MainCanvas GameObject
function ViewManager:GetMainCanvas()
	if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
	return self.mainCanvas
end

--获取UI主摄像机 MainCanvas Canvas
function ViewManager:GetMainCanvasCom()
    if self:GetMainCanvas() and self.mainCanvasCom == nil then
        self.mainCanvasCom = self:GetMainCanvas():GetComponent(typeof(UnityEngine.Canvas))
    end
	return self.mainCanvasCom
end

--获取UI主摄像机 MainCanvas RectTransform
function ViewManager:GetMainCanvasRect()
    if self:GetMainCanvas() and self.mainCanvasRect == nil then
        self.mainCanvasRect = self:GetMainCanvas():GetComponent(typeof(UnityEngine.RectTransform))
    end
    return self.mainCanvasRect
end