UIWidgetList = UIWidgetList or DeclareMono("UIWidgetList",UIWidgetBaseItem)
---ContentParent----GameObject	可以为空，为空表示控件挂载对象就是父结点
---AutoBottom----bool	可以为空，自动拉至底部 需要ScrollRect 必须为OnDestroy
---AutoTop----bool		可以为空，自动拉至顶部 需要ScrollRect 必须为OnDestroy
---ScrollRect===SrollRect
---ItemMono---------UIWidgetBaseItem

---运行时加载对象为子item item_list
---ItemPath====String
---TypeName====String
---CacheLoad===Bool

--加载型子节点的序列化参数，会将值传给每个实例化的子节点
---CPKeys====String[]
---CPVals==object[]

--StepCount===int 每帧最多加载几个item 默认值3个，设置为<=0表示 不分步加载
--StepFrameSpace===int 每隔几帧做一次step ,0为每帧都step set
--UseSmartData===bool Item是否是SmartData，勾上item本身会自动监听数据并设置值 勾选的话必需勾上OnDestroy用于监听销毁
--DelayFrameCount===int 刷新位置是否要延迟不为空则为延迟帧数，空或0则为同帧(示例：role_skill中的SkillInfoList)
function UIWidgetList:UIWidgetList()
	self.item_list = {}

	self.clone_obj = nil
	self.data = nil
	self.data_t = nil

	self.is_wait_ready = false
	self.cahce_index = nil

	self.items_cache = {}
	self.step_set_data_timer = nil
	self.setuped_index = nil
	self.total_count = 0
	self.key_list = nil	

	self.wait_setup_item = nil

	self.setup_complete = false

	self.select_cmd_cache = {}

	self.part_record = -1
	--默认滑动方向 1横向 2纵向
	self.dirc = 1
	--计算得到的content的宽高(一般用于需要同帧刷新位置但Content的size还没有更新的时候，通过子物体的LayoutElement)
	--要求子物体上挂载了LayoutElement并命名为ItemElement（具体参考Role_Skill预设的RoleViewSkillInfoItem）
	--如果子物体的宽高是固定长度则不用考虑
	self.ContentCountSize = Vector2.zero
end

function UIWidgetList:CheckReady()
	if self.clone_obj ~= nil then
		return
	else
		if self.ContentParent == nil then
			self.ContentParent = self.gameObject
		end
		if self.StepCount == nil then
			self.StepCount = 3
		end
		self.clone_obj = false
		if self.ItemMono ~= nil then
			self.clone_obj = self.ItemMono.gameObject
		elseif not self.CacheLoad then
			ResMgr:LoadGameObject(self.ItemPath,function(obj)
				self.clone_obj = obj
				end,false,false)
		else
			GameObjPool:RegisterPool(self.ItemPath,function(error)
					self.clone_obj = true
				end,false)
		end
	end
end

function UIWidgetList:SetupData()
	self.setuped_index = 0
	self.part_record = -1
	self.item_list = {}
	for i,item in pairs(self.items_cache) do
		if self.data[i] == nil then
			item:SetObjActive(false)
			item:SetSelected(false)
		end
	end
	if self.StepCount <= 0 then	
		self:stepSetupItems()
		self:CreateItemsCallBack()
	else
		if self.step_set_data_timer == nil then
			self.step_set_data_timer = Runner.Instance:RunUntilTrue(BindTool.Bind(self.stepSetupItems,self))
		end
	end
end

function UIWidgetList:stepSetupItems()
	if self:Destroyed() then
		return true
	end
	if self.setuped_index > #self.key_list then	--分步加载完成后延一帧再通知外部加载完成，防止content尺寸没有扩展完成导致的一些控制ScrollRect上的错误
		self.step_set_data_timer = nil
		self:CreateItemsCallBack()
		return true
	end	
	if self.wait_setup_item ~= nil then
		if self.wait_setup_item.SetupCompleted ~= nil and not self.wait_setup_item:SetupCompleted() then
			return false
		else
			self.wait_setup_item = nil
		end
	end

	local count = 0
	if self.StepFrameSpace ~= nil and self.StepFrameSpace > 1 then
		if self.part_record >= 0 then
			if self.part_record >= self.StepFrameSpace then
				self.part_record = 0
			else
				self.part_record = self.part_record + 1
				return false
			end
		else
			self.part_record = 0
		end
	end

	while true do
		count = count + 1
		self.setuped_index = self.setuped_index + 1
		if self.setuped_index > #self.key_list then
			-- self.step_set_data_timer = nil
			-- self:CreateItemsCallBack()
			return false
		end
		local data_key = self.key_list[self.setuped_index]
		local obj_mono = self:getOrCraeteItem(data_key)
		self:SetDataToMonoItem(obj_mono,self.data[data_key])
		self.item_list[data_key] = obj_mono

		if self.cahce_index == data_key then
			obj_mono:Click()
			self.cahce_index = nil
		end	

		local cache_selected = self.select_cmd_cache[data_key]
		self.select_cmd_cache[data_key] = nil
		if cache_selected ~= nil then
			obj_mono:SetSelected(cache_selected)
		end

		if obj_mono.SetupCompleted ~= nil and not obj_mono:SetupCompleted() then
			self.wait_setup_item = obj_mono
			return false
		end 

		if self.StepCount > 0 and count >= self.StepCount then
			return false
		end
	end
end

function UIWidgetList:SetupCompleted()
	return self.setup_complete
end

function UIWidgetList:IsLoaded()
	return self.setup_complete
end

function UIWidgetList:CreateItemsCallBack()
	self.setup_complete = true

    function ScrollTo()
        if self.ScrollRect then
            if self.AutoBottom then
                self.ScrollRect.verticalNormalizedPosition = 0
            elseif self.AutoTop then
	                self.ScrollRect.verticalNormalizedPosition = 1
            end
            if self.AutoLeft then
                self.ScrollRect.horizontalNormalizedPosition = 0
            elseif self.AutoRight then
                self.ScrollRect.horizontalNormalizedPosition = 1
            end
        end
    end

    if self.ScrollRect then
        if self.StepCount == 0 then
			self.step_end_timer = TimeHelper:AddDelayFrameTimer(function()
				ScrollTo()
			end, 1)
        else
            ScrollTo()
        end
    end
	if self.cahce_index_as then
		self:AutoSlideRealy(self.cahce_index_as)
		self.cahce_index_as = nil
	end
	for k,v in pairs(self.item_list) do
		if v.SetupAllComplete then
			v:SetupAllComplete()
		end
	end
	if self.complete_callback ~= nil then
		self.complete_callback()
	end
end

function UIWidgetList:ItemTypeName()
	if self.ItemMono ~= nil then
		return self.ItemMono:GetTypeName()
	else
		return self.TypeName
	end
end

function UIWidgetList:getOrCraeteItem(index)
	local cache_item = self.items_cache[index]
	if cache_item == nil then	
		local item_obj
		if self.CacheLoad then
			item_obj = GameObjPool:Get(self.ItemPath,self.ContentParent)
		else
			item_obj = self.clone_obj:Clone(self.ContentParent,false)
			item_obj:SetActive(true)
		end	
		cache_item = item_obj:GetLuaComponent(self:ItemTypeName())
		if self.CacheLoad then
			if self.CPKeys~=nil then
				for i = 1,self.CPKeys:Length() do
					local cp_keys = self.CPKeys[i]
					cache_item[cp_keys] = self.CPVals[i]
					cache_item._serialize_checked[cp_keys] = true 	--序列化字段设置好让它可以被 luaMono 垃圾回收机制销毁
				end
			end
			if cache_item.OnSpawn ~= nil then
				cache_item:OnSpawn()
			end
		end
		local siblin_key = nil
		for k,v in pairs(self.items_cache) do
			if k > index and (siblin_key == nil or k < siblin_key) then
				siblin_key = k
			end
		end
		if siblin_key ~= nil then
			item_obj:SetSiblingIndex(self.items_cache[siblin_key].gameObject:GetSiblingIndex())
		end
		self.items_cache[index] = cache_item
	else
		cache_item:SetObjActive(true)
	end
	return cache_item
end

--【危险！！】只有CacheLoad类的UIWList在销毁时才会执行这个函数
function UIWidgetList:OnDestroy()
	if self.step_set_data_timer ~= nil then
		Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer)
		self.step_set_data_timer = nil
	end
	if self.delayChangeMovementType ~= nil then
		TimeHelper:CancelTimer(self.delayChangeMovementType)
		self.delayChangeMovementType = nil
	end

	if self.delaySlide ~= nil then
		TimeHelper:CancelTimer(self.delaySlide)
		self.delaySlide = nil
	end

	if self.OffectTimer ~= nil then
		TimeHelper:CancelTimer(self.OffectTimer)
		self.OffectTimer = nil
	end

	if self.step_end_timer ~= nil then
		TimeHelper:CancelTimer(self.step_end_timer)
		self.step_end_timer = nil
	end

	if self.CacheLoad then
		for k,v in pairs(self.items_cache) do
			if v.OnRelease ~= nil then
				v:OnRelease()
			end
			GameObjPool:Release(v.gameObject)
		end
		if self.clone_obj ~= nil then
			GameObjPool:UnregisterPool(self.ItemPath)
		end
	else
		for k,v in pairs(self.items_cache) do
			if v.OnRelease ~= nil then
				v:OnRelease()
			end
		end
	end
end

function UIWidgetList:SetData(data)
	if data == nil then
		return
	end
	self.setup_complete = false
	self.data = data
	self.total_count = 0
	self.ContentCountSize = Vector2.zero
	self.data_t = {}
	self.key_list = {}
	self.select_cmd_cache = {}
	for k, v in pairs(self.data) do
		if type(k) == "number" then
			self.data_t[v] = k
			self.total_count = self.total_count + 1
			table.insert(self.key_list,k)
		end
	end
	table.sort(self.key_list)
	self:CheckReady()
	if self.clone_obj ~= nil and self.clone_obj ~= false then
		self:SetupData()
	else
		if not self.is_wait_ready then
			self.is_wait_ready = true
			Runner.Instance:RunUntilTrue(function()
					local ready = self.clone_obj ~= nil and self.clone_obj ~= false
					if ready then
						self.is_wait_ready = false
						self:SetupData()
					end
					return ready
				end)
		end
	end
end

--根据key点击ListItem
function UIWidgetList:ClickItem(index)
	local item = self.item_list[index]
	if item ~= nil then 
		item:Click()
	else
		self.cahce_index = index
	end
end

function UIWidgetList:SelectAll(selected)
	if TableIsEmpty(self.data) then
		return
	end
	for k,da in pairs(self.data) do
		local item = self.item_list[k]
		if item ~= nil then
			item:SetSelected(selected) 	
		else
			self.select_cmd_cache[k] = selected
		end
	end
end

function UIWidgetList:SetectItem(index,selected)
	local item = self.item_list[index]
	if item ~= nil then
		self.item_list[index]:SetSelected(selected) 
	elseif self.data[index] ~= nil then
		self.select_cmd_cache[index] = selected
	end
end

--根据传入的数据点击ListItem
function UIWidgetList:ClickItemData(data)
	local index = self.data_t[data]
	local item = self.item_list[index]
	if item ~= nil then
		item:Click()
	else
		self.cahce_index = index
	end
end

--替换单个List数据， to_data 传nil表示删除对应的数据
function UIWidgetList:SetItemData(from_data,to_data)
	local key = self.data_t[from_data]
	if key ~= nil then
		self:SetItemDataByKey(key,to_data)
	end
end

--设置单个key对应的数据 ，传nil表示删除该key对应的数据
function UIWidgetList:SetItemDataByKey(key,da)
	local old_data = self.data[key]
	if da == nil and old_data == nil then
		return 
	end
	self.data[key] = da

	if old_data ~= nil and self.data_t[old_data] == key then
		self.data_t[old_data] = nil
	end	
	self.data_t[da] = key
	if da ~= nil and old_data == nil then	
		for i,v in pairs(self.key_list) do
			if v == key then
				table.remove(self.key_list,i)
				self.setuped_index = self.setuped_index - 1		--分步加载步数往前退1
				break
			end
		end
		self.item_list[key] = nil
		local remove_item = self.items_cache[key]
		if remove_item ~= nil then
			remove_item:SetObjActive(false)
			remove_item:SetSelected(false)	
		end
	elseif da == nil and old_data ~= nil then	
		local key_list_co = #self.key_list
		if key_list_co == 0 or self.key_list[key_list_co] < key then
			table.insert(self.key_list,key)
		else
			for i=1,key_list_co do 	
				if self.key_list[i] > key then
					table.insert(self.key_list,i,key)
					break
				end
			end
		end
		local obj_mono = self:getOrCraeteItem(key)
		self:SetDataToMonoItem(obj_mono,da)
		self.item_list[key] = obj_mono
	else 
		local obj_mono = self.item_list[key]
		if obj_mono ~= nil then
			self:SetDataToMonoItem(obj_mono,da)
		end
	end
end

function UIWidgetList:SetDataToMonoItem(mono_item,data)
	if self.UseSmartData == true then
		mono_item:SetSmartData(data)
	else
		mono_item:SetData(data)
	end
	if mono_item.ItemElement and type(mono_item.ItemElement) ~= "function" then
		self.ContentCountSize.x = self.ContentCountSize.x + mono_item.ItemElement.preferredWidth
		self.ContentCountSize.y = self.ContentCountSize.y + mono_item.ItemElement.preferredHeight
	end
	if self.SupplyMask then
		mono_item:SupplyMask()
	end
end

--自动滑动至选中item简易版 慎用【列表 ContentCount 锚点 在中心节点 情况使用】 
-- StepCount这玩意不能为0，否则需要自己延迟一帧去调用
-- 注意使用自动滑动时需要在MonoBehavior添加，否则报错
-- 需要挂载
-- ItemPosition :BaseItem的RectTransform
-- ContentPosition:会自动伸展的那个RectTransform
-- ListPosition:Sroll框体的RectTransform
function UIWidgetList:AutoSlide(data, horizontal)
	if horizontal ~= nil then
		self.dirc = horizontal and 1 or 2
	end
	local index = self.data_t[data]
	self.cahce_index_as = index
	if self.cahce_index_as ~= nil and self:SetupCompleted() then
		self:AutoSlideRealy(self.cahce_index_as)
		self.cahce_index_as = nil
	end
end

function UIWidgetList:CancelAutoSlide()
	self.cahce_index_as = nil
end
-- 注意使用自动滑动时需要在MonoBehavior添加，否则报错 慎用
-- ItemPosition :BaseItem的RectTransform
-- ContentPosition:会自动伸展的那个RectTransform
-- ListPosition:Sroll框体的RectTransform
function UIWidgetList:AutoSlideRealy(index, horizontal)
	if horizontal ~= nil then
		self.dirc = horizontal and 1 or 2
	end
	--如果列表中的子格子数量会变化的话，建议加上这个否则可能会在不同数量切换时导致设置位置失效（超过极值时被自动复位的效果覆盖）
	--设置位置前先关闭ScrollRect控件的自动复位功能
	local movementType
	if self.ScrollRect then
		movementType = self.ScrollRect.movementType
		self.ScrollRect.movementType = 0
	end
	local slideFunc = function ()
		if self:Destroyed() then
			return
		end
		local contentRectSize = self.ContentPosition.sizeDelta
		local listRectSize = self.ListPosition.sizeDelta
		--横向单行
		if self.dirc == 1 then
			local showCountLimit = math.floor(listRectSize.x / self.ItemPosition.sizeDelta.x)
			local x = 0
			--复杂滑动（每个item的宽度不一致）
			if self.ComplicatedSlide then
				--用ipairs必须按照列表item的顺序
				for i,v in ipairs(self.item_list) do
					if i < index - 1 then
						local rect = v.gameObject:GetComponent(typeof(UnityEngine.RectTransform))
						x = x + rect.sizeDelta.x
						--如果跳跃距离过大会触发超拉自动归位而导致显示效果不好
						if contentRectSize.x - x <= listRectSize.x then
							x = contentRectSize.x - listRectSize.x
							break
						end
					else
						break
					end
				end
			else
				x = self.ItemPosition.sizeDelta.x * (index - 1)
				if index > self.total_count - showCountLimit then
					if self.ContentCountSize.x <= 0 then
						x = contentRectSize.x - listRectSize.x
					else
						x = self.ContentCountSize.x - listRectSize.x
					end
				end
			end
			local max_off = contentRectSize.x - self.ListPosition.rect.width
			max_off = math.max(max_off,0)
			self.ContentPosition.anchoredPosition = Vector2.New(-math.min(x,max_off), self.ContentPosition.anchoredPosition.y)
		else
		    --纵向单列
			local showCountLimit = math.floor(listRectSize.y / self.ItemPosition.sizeDelta.y)
			local y = 0 
			--复杂滑动（每个item的高度不一致）
			if self.ComplicatedSlide then
				--用ipairs必须按照列表item的顺序
				for i,v in ipairs(self.item_list) do
					if i < index - 1 then
						local rect = v.gameObject:GetComponent(typeof(UnityEngine.RectTransform))
						y = y + rect.sizeDelta.y
						--如果跳跃距离过大会触发超拉自动归位而导致显示效果不好
						if contentRectSize.y - y <= listRectSize.y then
							y = contentRectSize.y - listRectSize.y
							break
						end
					else
						break
					end
				end
			else
				y = self.ItemPosition.sizeDelta.y * (index - 1)
				--如果跳跃距离过大会触发超拉自动归位而导致显示效果不好
				if index > self.total_count - showCountLimit then
					if self.ContentCountSize.y <= 0 then
						y = contentRectSize.y - listRectSize.y
					else
						y = self.ContentCountSize.y - listRectSize.y
					end
				end
			end
			self.ContentPosition.anchoredPosition = Vector2.New(self.ContentPosition.anchoredPosition.x, y)
		end
		--设置位置后延迟一帧打开/恢复ScrollRect控件的自动复位功能
		if self.ScrollRect then
			self.delayChangeMovementType = TimeHelper:AddDelayFrameTimer(function ()
				if  not self:Destroyed() then
					self.ScrollRect.movementType = movementType
				end
			end)
		end

		-- if self.can_group ~= nil then 
		-- 	if not self.original_canvas_attached then
		-- 		UnityObj.Destroy(self.can_group)
		-- 	else
		-- 		self.can_group.enabled = true
		-- 	end
		-- 	self.can_group = nil
		-- end
	end
	-- if self.DelayFrameCount ~= nil and self.DelayFrameCount ~= 0 then
	-- 	self.delaySlide = TimeHelper:AddDelayFrameTimer(slideFunc,self.DelayFrameCount)
	-- else
		slideFunc()
	-- end
end

--清理一遍smdata
function UIWidgetList:ClearSmData()
	if self.item_list then
		for k,v in pairs(self.item_list) do
			v:uncareData()
		end
	end
end

function UIWidgetList:GetSelectedData()
	local sel_datas = {}
	for _, item in pairs(self.item_list) do
		if item:IsSelected() then
			table.insert(sel_datas, item:GetData())
		end
	end
	return sel_datas
end

function UIWidgetList:GetSelectedItem()
	local sel_datas = {}
	for _, item in pairs(self.item_list) do
		if item:IsSelected() then
			table.insert(sel_datas, item)
		end
	end
	return sel_datas
end

function UIWidgetList:GetData()
	if self.data then
		return self.data
	end
	return nil
end

function UIWidgetList:GetItem(index)
	if self.item_list and self.item_list[index] then
		return self.item_list[index]
	end
end

function UIWidgetList:GetSelectedIndex(data)
	if self.data_t then
		return self.data_t[data] or 1
	end
	return 1
end

--设置带选中的数据 默认选中第一个
function UIWidgetList:SetDataList(data,index)
	if data ~= nil then
		self:SetData(data)
	end
	if data and data[index or 1] then
		self:ClickItemData(data[index or 1])
	end
end

--设置选中第几个item 默认选中第一个
function UIWidgetList:SetSetectItem(index)
	if self.data and self.data[index or 1] then
		self:ClickItemData(self.data[index or 1])
	end
end

--设置列表加载完毕回调
function UIWidgetList:SetCompleteCallBack(complete_callback)
	self.complete_callback = complete_callback
end

--获取列表item数量
function UIWidgetList:GetItemCount()
	return table.nums(self.item_list)
end

--外部不可调用 设置ContentRect位置
function UIWidgetList:SetContentPosition(tab)
	if not self.ContentRect then
		if self.ContentParent == nil then return end
		self.ContentRect = self.ContentParent:GetComponent(typeof(UnityEngine.RectTransform))
	end
	local pos = self.ContentRect.localPosition
	pos.x = tab.width or pos.x
	pos.y = tab.height or pos.y
	self.ContentRect.localPosition = pos
end

--纵向强制跳跃 直接输入heigh可以直接到对应位置
function UIWidgetList:ForceJumpVertical(height)
	self:SetContentPosition({height = height})
end

--横向强制跳跃 直接输入width可以直接到对应位置
function UIWidgetList:ForceJumpHorizontal(width)
	self:SetContentPosition({width = width})
end

--纵向跳跃到index Item的位置索引
function UIWidgetList:JumpVerticalIndex(index,height,offset)
	self:SetContentPosition({height = height * (index - 1) + (offset or 0)})
end

--横向跳跃index Item的位置索引
function UIWidgetList:JumpHorizontalIndex(index,width,offset)
	self:SetContentPosition({width = width * (index - 1) + (offset or 0)})
end