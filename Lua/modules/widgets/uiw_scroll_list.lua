UIWScrollList = UIWScrollList or DeclareMono("UIWScrollList")

---ItemMono---------UIWidgetBaseItem

---运行时加载对象为子item index_item
---ItemPath====String
---TypeName====String

--加载型子节点的序列化参数，会将值传给每个实例化的子节点
---CPKeys====String[]		
---CPVals==object[]

---LayoutGroup------ScrollLayoutGroup
---SelectGroup------UIWSelectGroup
---ContentRect------RectTransform
---UseSmartData===bool Item是否是SmartData，勾上item本身会自动监听数据并设置值 勾选的话必需勾上OnDestroy用于监听销毁

function UIWScrollList:UIWScrollList()

	self.data = nil
	self.is_wait_ready = false

	self.scroll2data_index = {}

	self.item_index = {}
	self.index_item = {}

	self.data_index = {}

	self.created_items = {}

	-- self.selected_data = {}

	self.clone_obj = nil

	self.clone_mono = nil
end

function UIWScrollList:OnDestroy()
	TimeHelper:CancelTimer(self.timer_jtd)
	TimeHelper:CancelTimer(self.timer_jtt)
	TimeHelper:CancelTimer(self.timer_jump)
end

function UIWScrollList:SetData(data)
	if data == nil then
		return
	end
	self.data = data
	self.data_t = {}
	for k, v in pairs(self.data) do
		if type(k) == "number" then
			self.data_t[v] = k
		end
	end
	local key_tab = {}
	self.scroll2data_index = {}
	self.data_index = {}
	local data_count = 0
	for k,v in pairs(self.data) do
		data_count = data_count + 1
		table.insert(key_tab,k)
		if self.SelectGroup ~= nil then
			self.SelectGroup:Register(v,BindTool.Bind(self.OnGroupSelect,self))
			self:CancelSelect()--数据刷新要全部取消
		end
		self.data_index[v] = k
	end

	table.sort(key_tab)
	for i=0 ,#key_tab-1 do
		self.scroll2data_index[i] = key_tab[i+1]
	end
	self:CheckReady()

	if self.clone_obj ~= nil and self.clone_obj ~= false then
		self.LayoutGroup:SetItemCount(data_count,true)
	else     
		if not self.is_wait_ready then
			self.is_wait_ready = true
			Runner.Instance:RunUntilTrue(function()
					local ready = self.clone_obj ~= nil and self.clone_obj ~= false
					if ready then
						self.is_wait_ready = false
						self.LayoutGroup:SetItemCount(data_count,true)
					end
					return ready
				end)
		end
	end
end

function UIWScrollList:CheckReady()
	if self.clone_obj ~= nil then
		return
	else
		if self.ContentParent == nil then
			self.ContentParent = self.LayoutGroup.gameObject
		end
		self.clone_obj = false

		if not self:CacheLoad() then
			self.clone_obj = self.ItemMono.gameObject
			self.clone_mono = self.ItemMono
		else
			GameObjPool:RegisterPool(self.ItemPath,function(error)
					self.clone_obj = true
				end,false)
		end
	end
end

function UIWScrollList:ClickItem(index)
	if self.data == nil then
		return
	end
	local da = self.data[index]
	if da == nil then
		return
	end
	local item = self.index_item[index]
	if item ~= nil then
		item:Click()
		return true
	else
		if self.SelectGroup ~= nil then
			local old_sel = self.SelectGroup:Get(da) or false
			self:SetectItem(index,not old_sel)
		end
		return false
	end	
end

--根据传入的数据点击ListItem
function UIWScrollList:ClickItemData(data)
	local index = self.data_t[data]
	if nil == index then return end
	self:ClickItem(index)
end

function UIWScrollList:OnGroupSelect(data,selected)
	local index = self.data_index[data]
	local itemMono = self.index_item[index]
	if itemMono ~= nil then
		itemMono:SetSelected(selected)
	end
end

function UIWScrollList:SetectItem(index,selected)
	local itemMono = self.index_item[index]
	if itemMono ~= nil then
		itemMono:SetSelected(selected)
	else
		if self.SelectGroup ~= nil then
			self.SelectGroup:Set(self.data[index],selected)
		end	
	end
end

--清理一遍smdata
function UIWScrollList:ClearSmData()
	if self.index_item then
		for k, v in pairs(self.index_item) do
			v:uncareData()
		end
	end
end

function UIWScrollList:GetSelectedData()
	local selected_data = {}
	for k, v in pairs(self.index_item)do
		if v:IsSelected() then
			table.insert(selected_data,self:GetByIndex(k))
		end
	end
	return selected_data
end

function UIWScrollList:GetItemMonoByIndex(index)
	if self.index_item[index] ~= nil then 
		return self.index_item[index]
	end
	return nil
end

function UIWScrollList:GetItemMono()
	if self.index_item then return self.index_item else return nil end
end

function UIWScrollList:GetData()
	if self.data ~= nil then
		return self.data
	end
end

function UIWScrollList:SelectAll(selected)
	for k,v in pairs(self.index_item) do
		self:SetectItem(k, selected)
	end
end

function UIWScrollList:GetAll()
	local all_data = DataHelper.TableCopy(self.data)
	if all_data ~= nil then
		return all_data
	end
	return nil
end

function UIWScrollList:GetByIndex(index)
	if self.data[index] ~= nil then
		return self.data[index]
	end
	return nil
end
function UIWScrollList:GetByData(data)
	if self.data_index[data] ~= nil then
		return self.data_index[data]
	end
	return 1
end
function UIWScrollList:GetDataIndex()
	return self.data_index
end

--when item's selected event been trigger
function UIWScrollList:OnItemSelected(itemMono,selected)
	if self.SelectGroup ~= nil then
		local index = self.item_index[itemMono]
		self.SelectGroup:Set(self.data[index],selected)
	end
end

--index 从1开始
function UIWScrollList:OnSetItemData(itemMono,scroll_index)
	local data_index = self.scroll2data_index[scroll_index]
	if data_index == nil then return end
	local tab = itemMono.Table
	local set_data = self.data[data_index]
	if self.UseSmartData == true then
		tab:SetSmartData(set_data)
	else
		tab:SetData(set_data)
	end
	local old_index = self.item_index[tab]
	self.item_index[tab] = data_index
	if old_index ~= nil then
		local old_tab = self.index_item[old_index]
		if old_tab == tab then
			self.index_item[old_index] = nil
		end
	end
	self.index_item[data_index] = tab

	if self.SelectGroup ~= nil then
		local selected = self.SelectGroup:Get(set_data) or false
		tab:SetSelected(selected)
	end
end

function UIWScrollList:ItemTypeName()
	if self.ItemMono ~= nil then
		return self.ItemMono:GetTypeName()
	else
		return self.TypeName
	end
end

function UIWScrollList:CacheLoad()
	return self.ItemMono == nil
end

--设置带选中的数据 默认选中第一个
function UIWScrollList:SetDataList(data,index)
	if data ~= nil then
		self:SetData(data)
	end
	self:SetectItem(index or 1,true)
end

--取消选中
function UIWScrollList:CancelSelect()
	self.SelectGroup:CancelSelect()
end

--外部设置SelType 0:单选，1:多选，2:可取消的单选
function UIWScrollList:SetSelType(type)
	self.SelectGroup:SetSelType(type)
end

function UIWScrollList:IsHasItem()
	return #self.index_item > 0
end

--SelectGroup selected_data
function UIWScrollList:GetSelectGroupData()
	local selected_data = {}
	for k, v in pairs(self.SelectGroup:GetSelectedData())do
		if v.sel then
			table.insert(selected_data,k)
		end
	end
	return selected_data
end
--------------------For ScrollLayoutGroup START---------------
function UIWScrollList:OnCreateItem(parent)
	local item_obj
	if self:CacheLoad() then
		item_obj = GameObjPool:Get(self.ItemPath,parent)
	else
		item_obj = self.clone_obj:Clone(parent)		
	end
	item_obj:SetActive(true)
	local obj_mono = item_obj:GetLuaComponent(self:ItemTypeName())
	if self:CacheLoad() then
		if self.CPKeys~=nil then
			for i = 1,self.CPKeys:Length() do
				local cp_keys = self.CPKeys[i]
				obj_mono[cp_keys] = self.CPVals[i]
				obj_mono._serialize_checked[cp_keys] = true 	--序列化字段设置好让它可以被 luaMono 垃圾回收机制销毁
			end
		end
		if obj_mono.OnSpawn ~= nil then
			obj_mono:OnSpawn()
		end
	end
	obj_mono._on_selected = BindTool.Bind(self.OnItemSelected,self)
	table.insert(self.created_items,obj_mono)
	return obj_mono.unityMono
end

function UIWScrollList:ReleaseAllItem()
	if self:CacheLoad() then
		for k,v in pairs(self.created_items) do
			if v.OnRelease ~= nil then
				v:OnRelease()
			end
			GameObjPool:Release(v.gameObject)
		end
	else
		for k,v in pairs(self.created_items) do
			UnityGameObj.Destroy(v.gameObject)
		end
	end	
	self.created_items = {}	
end

function UIWScrollList:GetItemSize(scroll_index,axis)
	if self.data == nil then
		return 0
	end
	if self.clone_mono.ExpendSize ~= nil then
		local data_index = self.scroll2data_index[scroll_index]
		local set_data = self.data[data_index]
		if set_data ~= nil then
			return self.clone_mono:ExpendSize(set_data,axis)
		else
			return 0
		end
	else
		return 0
	end
end

--ScrollList必须勾选OnDestroy ScrollRect ScrollRect
function UIWScrollList:JumpToDown(value)
	TimeHelper:CancelTimer(self.timer_jtd)
	TimeHelper:CancelTimer(self.timer_jtt)
	self.timer_jtd = TimeHelper:AddDelayFrameTimer(function ()
		if self.ScrollRect ~= nil and self.ScrollRect:Equals(nil) == false then
			self.ScrollRect.verticalNormalizedPosition = value or 0
		end
	end)
end

--ScrollList必须勾选OnDestroy
function UIWScrollList:JumpToTop()
	TimeHelper:CancelTimer(self.timer_jtd)
	TimeHelper:CancelTimer(self.timer_jtt)
	self.timer_jtt = TimeHelper:AddDelayFrameTimer(function ()
		if self.ScrollRect ~= nil and self.ScrollRect:Equals(nil) == false then
			self.ScrollRect.verticalNormalizedPosition = 1
		end
	end)
end

--ScrollList必须勾选OnDestroy ScrollRect ScrollRect
function UIWScrollList:JumpToRight(value)
	TimeHelper:CancelTimer(self.timer_jtd)
	TimeHelper:CancelTimer(self.timer_jtt)
	self.timer_jtd = TimeHelper:AddDelayFrameTimer(function ()
		if self.ScrollRect ~= nil and self.ScrollRect:Equals(nil) == false then
			self.ScrollRect.horizontalNormalizedPosition = value or 0
		end
	end)
end

--ScrollList必须勾选OnDestroy
function UIWScrollList:JumpToLeft()
	TimeHelper:CancelTimer(self.timer_jtd)
	TimeHelper:CancelTimer(self.timer_jtt)
	self.timer_jtt = TimeHelper:AddDelayFrameTimer(function ()
		if self.ScrollRect ~= nil and self.ScrollRect:Equals(nil) == false then
			self.ScrollRect.horizontalNormalizedPosition = 1
		end
	end)
end
--==========================UIWSelectGroup============================
UIWSelectGroup = UIWSelectGroup or DeclareMono("UIWSelectGroup")

----SelType=====Int(0:单选，1:多选，2:可取消的单选)

function UIWSelectGroup:UIWSelectGroup()
	self.cur_select = nil	--单选有效
	self.selected_data = {}
end

function UIWSelectGroup:Register(data,on_val_change)
	local sel_data = self.selected_data[data]
	if sel_data == nil then
		sel_data = {sel=false}
		self.selected_data[data] = sel_data
	end
	sel_data.on_val_change = on_val_change
end

function UIWSelectGroup:Get(data)
	local sel_data = self.selected_data[data]
	return sel_data ~= nil and sel_data.sel or false
end

function UIWSelectGroup:Set(data,selected)
	local sel_data = self.selected_data[data]
	if sel_data == nil then
		return
	end
	if sel_data.sel == selected then
		return
	end
	sel_data.sel = selected
	if self.SelType ~= 1 then
		if self.cur_select == data then
			if self.SelType ~= 2 and selected == false then
				sel_data.sel = true
				sel_data.on_val_change(data,true)
			end
		else
			local old_sel_data = self.cur_select
			self.cur_select = data
			if old_sel_data ~= nil then
				local old_sel = self.selected_data[old_sel_data]
				old_sel.sel = false
				old_sel.on_val_change(old_sel_data,false)
			end
		end
	end
end

-- 取消选中
function UIWSelectGroup:CancelSelect()
	if self.SelType ~= 1 then
		if nil ~= self.cur_select then
			local old_sel = self.selected_data[self.cur_select]
			old_sel.sel = false
			old_sel.on_val_change(self.cur_select,false)
		end
	else
		for k,v in pairs(self.selected_data) do
			v.sel = false
			v.on_val_change(k,false)
		end
	end
end

function UIWSelectGroup:SetSelType(type)
	self.SelType = type or 0
end

function UIWSelectGroup:GetSelectedData()
	return self.selected_data or {}
end

--获取列表item数量
function UIWScrollList:GetItemCount()
	return table.nums(self.index_item)
end

--【index Item的位置索引 axis = column】慎用
function UIWScrollList:JumpVertIndex(index, axis)
	local height = self.LayoutGroup:CacuSize(index - (axis or 1),axis or 1)
	self:ForceJumpVertical(height)
end

--【condition条件func(数字时为index) axis = column or col】慎用
function UIWScrollList:JumpVertCondition(condition,axis)
	local index = tonumber(condition) and condition or DataHelper.SimpleCondIndex(self.data,condition)
	local height = self.LayoutGroup:CacuSize(index - (axis or 1),axis or 1)
	self:ForceJumpVertical(height)
end

--指引获取UI使用 ClickItem按钮Obj 慎用
function UIWScrollList:GuideVertical(condition,axis)
	local index = tonumber(condition) and condition or DataHelper.SimpleCondIndex(self.data,condition)
	self:JumpVertCondition(index, axis)
	if self.index_item and self.index_item[index] then
		return self.index_item[index].ClickItem or self.index_item[index]
	end
end

--外部不可调用 设置ContentRect位置
function UIWScrollList:SetContentPosition(tab)
	if not self.ContentRect then
		self.ContentRect = self.LayoutGroup.gameObject:GetComponent(typeof(UnityEngine.RectTransform))
	end
	local pos = self.ContentRect.localPosition
	pos.x = tab.width or pos.x
	pos.y = tab.height or pos.y
	self.ContentRect.localPosition = pos
end

--纵向强制跳跃 直接输入heigh可以直接到对应位置
function UIWScrollList:ForceJumpVertical(height)
	self:SetContentPosition({height = height})
end

--横向强制跳跃 直接输入width可以直接到对应位置
function UIWScrollList:ForceJumpHorizontal(width)
	self:SetContentPosition({width = width})
end

--纵向跳跃到index Item的位置索引
function UIWScrollList:JumpVerticalIndex(index,height,offset)
	self:SetContentPosition({height = height * (index - 1) + (offset or 0)})
end

--横向跳跃index Item的位置索引
function UIWScrollList:JumpHorizontalIndex(index,width,offset)
	self:SetContentPosition({width = width * (index - 1) + (offset or 0)})
end