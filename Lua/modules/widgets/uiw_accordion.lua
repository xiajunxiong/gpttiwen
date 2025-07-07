UIWAccordion = UIWAccordion or DeclareMono("UIWAccordion")

--  手风琴组件

-- ContentParent--GameObject
-- ContentChild---GameObject
-- ItemParent-----UIWidgetBaseItem
-- ItemChild------UIWidgetBaseItem
-- StepCount------int 每帧最多加载几个item 默认值3个，设置为<=0表示 不分步加载
-- NotShow--------bool nil(展开并选中第一个) 注意:虽然不展开还是会默认帮你点击第一个

--  对外接口 SetData(data)

-- data = {
--     -- parent标识(方便获取parent表,可以不写) 
--     [parent] = {
--         name = "xxx",
--         ...         -- parent_info(name ...)
--         children = {
--             {
--                 ...     -- child1_info(parent1)
--             },
--             {
--                 ...     -- child2_info(parent1)
--             }
--             ...
--         }
--     },
--     ...
-- }

--示例 task_view_accpeted

AccordionItemType = {
    parent = 1,
    content = 2,
    child = 3
}

function UIWAccordion:UIWAccordion()
    self.item_list = {}
    self.cache_list = {}
    for _, v in pairs(AccordionItemType) do
        self.item_list[v] = {}
        self.cache_list[v] = {}
    end
    self.cache_item_c = nil

    self.item_parent_obj = nil
    self.item_child_obj = nil
    self.content_obj = nil

    self.data = nil
    self.data_p = nil
    self.data_c = nil
    self.data_cc = nil
    self.record_p = {}
    self.record_c = nil

    self.record = false
    self.step_set_data_timer = nil

    -- 创建完成回调
    self.create_finish_func = nil

    self.is_init = false
end

function UIWAccordion:CheckReady()
    self.is_ready = false
    if self.ContentParent ~= nil and self.ContentChild ~= nil and self.ItemParent ~= nil and self.ItemChild ~= nil then
        self.item_parent_obj = self.ItemParent.gameObject
        self.item_child_obj = self.ItemChild.gameObject
        self.content_obj = self.ContentChild
        self.StepCount = self.StepCount or 3
        self.is_ready = true
    end
end

function UIWAccordion:SetupData()
    if not self.is_ready then return end
    self.setuped_info = {
        type = AccordionItemType.parent,
        indexs = {},
        parent_key = nil,
        index = nil,
    }
    for _, v in pairs(AccordionItemType) do
        self.item_list[v] = {}
    end
    if next(self.data) then
        local first_children = self.data[next(self.data)].children
        if next(first_children) then
            self.first_child_data = first_children[next(first_children)]
        end
    end

    for name, item in pairs(self.cache_list[AccordionItemType.parent]) do
		if self.data_p[name] == nil then
            item:SetSelected(false)
			item:SetObjActive(false)
			item:OnRelease()	
		end
    end
    
    for info, item in pairs(self.cache_list[AccordionItemType.child]) do
        if self.data_c[info] == nil then
            item:SetObjActive(false)
            item:SetSelected(false)
            item:OnRelease()
        end
    end
    
    for name, obj in pairs(self.cache_list[AccordionItemType.content]) do
		if self.data_p[name] == nil then
			obj:SetActive(false)
		end
    end
    
    self.count_parent = 0
    self.count_content = 0
    if self.StepCount <= 0 then
        self:stepSetupItemsPP()
	else
		if self.step_set_data_timer == nil then
			self.step_set_data_timer = Runner.Instance:RunUntilTrue(BindTool.Bind(self.NotShow and self.stepSetupItemsPP or self.stepSetupItemsCP,self))
		end
	end
end

--优先加载一个父物体 该父物体下所有子物体 适合默认展开类型
function UIWAccordion:stepSetupItemsCP()
    if self:Destroyed() then return true end
	local count = 0
    while true do
        count = count + 1
        local data = self.setuped_info.type == AccordionItemType.child and self.data_c or self.data
        local pre_index = self.setuped_info.indexs[self.setuped_info.type]
        local pre_data = data[pre_index]
        local index = next(data, pre_index)
        if index == nil then
            self.step_set_data_timer = nil
            self:CreateItemsCallBack()
            return true
        else
            if self.setuped_info.type == AccordionItemType.child and pre_data and pre_data.parent_key ~= data[index].parent_key and self.setuped_info.parent_key ~= pre_data.parent_key then
                self.setuped_info.parent_key = pre_data.parent_key
                self.setuped_info.type = AccordionItemType.parent
                return false
            else
                self.setuped_info.indexs[self.setuped_info.type] = index
                if self.setuped_info.type == AccordionItemType.parent then
                    self.count_parent = self.count_parent + 1
                    local mono = self:getOrCreateParent(data[index].name, self.count_parent)
                    mono:SetData(data[index])
                    self.item_list[AccordionItemType.parent][data[index].name] = mono
                elseif self.setuped_info.type == AccordionItemType.content then
                    self.count_content = self.count_content + 1
                    local mono = self:getOrCreateContent(data[index].name, self.count_content)
                    self.item_list[AccordionItemType.content][data[index].name] = mono
                elseif self.setuped_info.type == AccordionItemType.child then
                    local mono = self:getOrCreateChild(index, data[index])
                    local mono_p = self.item_list[AccordionItemType.parent][data[index].parent_name]
                    mono:SetData(data[index].info)
                    self.item_list[AccordionItemType.child][data[index].info] = {
                        parent = mono_p,
                        mono = mono
                    }
                    if not self.record_c and data[index].info == self.first_child_data then
                        self.first_child_data = nil
                        mono_p:SetSelected(true)
                        mono:Click()
                    end
                end

                if self.setuped_info.type ~= AccordionItemType.child then
                    self.setuped_info.type = self.setuped_info.type + 1
                end

                if self.StepCount > 0 and count >= self.StepCount then
                    return false
                end
            end
        end
	end
end

--优先加载父物体 适合默认不展开的类型
function UIWAccordion:stepSetupItemsPP()
    if self:Destroyed() then return true end
	local count = 0
    while true do
        count = count + 1
        local data = self.setuped_info.type == AccordionItemType.child and self.data_c or self.data
        self.setuped_info.index = next(data, self.setuped_info.index)
        if self.setuped_info.index == nil then
            if self.setuped_info.type == AccordionItemType.child then
                self.step_set_data_timer = nil
                self:CreateItemsCallBack()
                return true
            else
                self.setuped_info.type = self.setuped_info.type + 1
                self.setuped_info.index = nil
            end
        else
            local index = self.setuped_info.index
            if self.setuped_info.type == AccordionItemType.parent then
                self.count_parent = self.count_parent + 1
                local mono = self:getOrCreateParent(data[index].name, self.count_parent)
                mono:SetData(data[index])
                self.item_list[AccordionItemType.parent][data[index].name] = mono
            elseif self.setuped_info.type == AccordionItemType.content then
                self.count_content = self.count_content + 1
                local mono = self:getOrCreateContent(data[index].name, self.count_content)
                self.item_list[AccordionItemType.content][data[index].name] = mono
            elseif self.setuped_info.type == AccordionItemType.child then
                local mono = self:getOrCreateChild(index, data[index])
                local mono_p = self.item_list[AccordionItemType.parent][data[index].parent_name]
                mono:SetData(data[index].info)
                self.item_list[AccordionItemType.child][data[index].info] = {
                    parent = mono_p,
                    mono = mono
                }
                if data[index].info == self.first_child_data then
                    self:CacheClick(self.first_child_data)
                    self.first_child_data = nil
                    -- mono_p:SetSelected(true)
                    -- mono:Click()
                end
            end

            if self.StepCount > 0 and count >= self.StepCount then
                return false
            end
        end
	end
end

function UIWAccordion:getOrCreateParent(name, count)
	local cache_item = self.cache_list[AccordionItemType.parent][name]
    if cache_item == nil then
        local item_obj = self.item_parent_obj:Clone(self.ContentParent)
		item_obj:SetActive(true)
        cache_item = item_obj:GetLuaComponent(self.ItemParent:GetTypeName())
        if cache_item.OnSpawn ~= nil then
            cache_item:OnSpawn()
        end
		self.cache_list[AccordionItemType.parent][name] = cache_item
	else
		cache_item:SetObjActive(true)
	end
    cache_item.gameObject:SetSiblingIndex(2 * count - 1)
	return cache_item
end


function UIWAccordion:getOrCreateChild(index, info)
	local cache_item = self.cache_list[AccordionItemType.child][index]
    local content = self.item_list[AccordionItemType.content][info.parent_name]
    local item_obj
    if cache_item == nil then
        item_obj = self.item_child_obj:Clone(content)
        item_obj:SetActive(true)
        cache_item = item_obj:GetLuaComponent(self.ItemChild:GetTypeName())
        if cache_item.OnSpawn ~= nil then
            cache_item:OnSpawn()
        end
    else
        item_obj = cache_item.gameObject
        item_obj:SetParent(content)
        cache_item:SetObjActive(true)
    end
    item_obj:SetSiblingIndex(index)
    self.cache_list[AccordionItemType.child][index] = cache_item
	return cache_item
end

function UIWAccordion:getOrCreateContent(name, count)
	local cache_item = self.cache_list[AccordionItemType.content][name]
    if cache_item == nil then
        local item_obj = self.content_obj:Clone(self.ContentParent)
        item_obj:SetActive(false)
        cache_item = item_obj
        item_obj:SetSiblingIndex(self.cache_list[AccordionItemType.parent][name].gameObject:GetSiblingIndex() + 1)
        self.cache_list[AccordionItemType.content][name] = item_obj
        local mono_p = self.item_list[AccordionItemType.parent][name]
        mono_p.Activator.Actives:Add(cache_item)
    end
    -- 加上这个则计算content_child的高度
    if self.ContentChildMono then
        local children_list = self.data_p[name]
        if children_list then
            local mono = self.cache_list[AccordionItemType.content][name]:GetLuaComponent(self.ContentChildMono)
            mono:SetData(children_list.children)
        end
    end
    if not self.NotShow then
        cache_item.gameObject:SetSiblingIndex(2 * count)
    end
	return cache_item
end

function UIWAccordion:CreateItemsCallBack()
    if self.record then
        for name, is_sel in pairs(self.record_p) do
            local item = self.item_list[AccordionItemType.parent][name]
            if nil ~= item then
                item:SetSelected(is_sel)
            end
        end
        if not self.item_list[AccordionItemType.child][self.record_c] then
            self.record_c = self.first_child_data
        end
        if nil ~= self.record_c then
            local item = self.item_list[AccordionItemType.child][self.record_c]
            if nil ~= item then
                item.parent:SetSelected(true)
                item.mono:Click()
            end
        end
        self.first_child_data = nil
    end
    if not self.is_init then
        self.is_init = true
        if self.show_all then
            for _, item in pairs(self.item_list[AccordionItemType.parent]) do
                item:SetSelected(true)
            end
        end
    end

    if self.record_jump ~= nil then 
    self.init_jumpTimer = TimeHelper:AddDelayFrameTimer(function()
            self.ContentRect.localPosition = self.record_jump
        end,5)
    end

    if self.create_finish_func and type(self.create_finish_func) == "function" then
        self.create_finish_func()
    end
end

function UIWAccordion:SetCreateCallBack(func)
    self.create_finish_func = func
end

function UIWAccordion:SetData(data, record, show_all)
    self.data = data
    self.data_p = self:GetDataP()
    self.data_c, self.data_cc = self:GetDataC()
    self.record = record
    self.show_all = show_all

    self:CheckReady()
    self:SetupData()
end

function UIWAccordion:GetDataP()
    local list = {}
    for _, v in pairs(self.data) do
        list[v.name] = v
    end
    return list
end

function UIWAccordion:GetDataC()
    local list1, list2 = {}, {}
    for parent_key, v in pairs(self.data) do
        if v.children ~= nil then
            for key, z in pairs(v.children) do
                table.insert(list1, {
                    parent_name = v.name,
                    parent_key = parent_key,
                    child_key = key,
                    info = z
                })
                list2[z] = true
            end
        end
    end
    return list1, list2
end

function UIWAccordion:OnDestroy()
	if self.step_set_data_timer ~= nil then
		Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer)
		self.step_set_data_timer = nil
	end

    for name, item in pairs(self.cache_list[AccordionItemType.parent]) do
        if item.OnRelease ~= nil then 
            item:OnRelease()
        end 
    end
    
    for info, item in pairs(self.cache_list[AccordionItemType.child]) do
        if item.OnRelease ~= nil then 
            item:OnRelease()
        end 
    end

    for type, items in pairs(self.cache_list) do
        for _, item in pairs(items) do
            UnityGameObj.Destroy(type == AccordionItemType.content and item or item.gameObject)
        end
    end

    if nil ~= self.cache_item_c then
        UnityGameObj.Destroy(self.cache_item_c)
    end

    if nil ~= self.init_jumpTimer then 
        TimeHelper:CancelTimer(self.init_jumpTimer)
        self.init_jumpTimer = nil
    end

    TimeHelper:CancelTimer(self.AutoJumpTimer)
end

function UIWAccordion:OnClickParent(info)
    local item_parent_list = self.item_list[AccordionItemType.parent]
    local item_parent = item_parent_list[info.name]
    if item_parent then
        if item_parent.is_selected then
            self.ToggleGroup:SetAllTogglesOff()
        end
        if item_parent.OnSelect then
            item_parent:OnSelect(info.is_sel)
        end
    end
    local is_sel = info.is_sel
    self.record_p[info.name] = info.is_sel
    local item_child_list = self.item_list[AccordionItemType.child]
    if self.record_c and item_child_list[self.record_c] then
        item_child_list[self.record_c].mono:SetSelected(info.is_sel)
    end
end

-- 判断单个父节点是否收缩了
function UIWAccordion:IsParentClose(name)
    local item_parent_list = self.item_list[AccordionItemType.parent]
    local item_parent = item_parent_list[name]
    if item_parent then
        return not item_parent.Selector.isOn
    end
    return true
end

--需要Parent互斥的
function UIWAccordion:OnClickParentMutex(info)
    if info.is_sel == true then
        self:ClickChild(self.data_p[info.name].children[1])
    end
end

function UIWAccordion:OnClickChild(data)
    if self.record_c ~= data then
        local item_child_list = self.item_list[AccordionItemType.child]
        local child = item_child_list[self.record_c]
        if child and child.mono and child.mono.Selector then
            child.mono.Selector.isOn = false
        end
    end
    self.record_c = data
end

function UIWAccordion:ClickChild(data)
    -- self:CacheClick(data)
    local item_child_list = self.item_list[AccordionItemType.child]
    if item_child_list[data] then
        self.record_c = data
        item_child_list[self.record_c].parent:SetSelected(true)
        item_child_list[self.record_c].mono:Click()
    else
		self.record_c = data
    end
end

function UIWAccordion:CacheClick(data)
    if self.data_cc[data] then
        if nil == self.cache_item_c then
            self.cache_item_c = self.cache_item_c or self.item_child_obj:Clone(self.ContentParent)
            local le = self.cache_item_c:AddComponent(typeof(UnityEngine.UI.LayoutElement))
            le.ignoreLayout = true
            self.cache_item_c:SetLocalPosition(Vector3.New(99999,99999,0))
            self.cache_item_c:SetActive(true)
        end
        local mono = self.cache_item_c:GetLuaComponent(self.ItemChild:GetTypeName())
        mono:SetData(data)
        mono:Click()
    end
end

function UIWAccordion:FlushChildItem()
    for k, v in pairs(self.item_list[AccordionItemType.child]) do
        v.mono:SetData(v.mono:GetData())
    end
end

--纵向跳转到某个item ContentRect可不加，这个只适用于初始化创建完全部子节点后纵向跳一次
--[count:单列即为index 多列自行index / col 纵向来讲]
--[hight:item的高度 + 行间距 纵向来讲]
--[parent_pos 父位置]
function UIWAccordion:JumpVerticalIndex(count,hight,parent_pos,parent_hight)
	if not self.ContentRect then
		self.ContentRect = self.ContentParent:GetComponent(typeof(UnityEngine.RectTransform))
	end
	local pos = self.ContentRect.localPosition
    pos.y = hight * (count - 1) + (parent_pos-1)  * parent_hight
    self.record_jump = pos
end

--横向跳转到某个item ContentRect可不加
--[count:单行即为index 多行自行index / cow 横向来讲]
--[width:item的宽度 + 列间距 横向来讲]
-- function UIWAccordion:JumpHorizontalIndex(count,width)
-- 	if not self.ContentRect then
-- 		self.ContentRect =self.ContentParent:GetComponent(typeof(UnityEngine.RectTransform))
-- 	end
-- 	local pos = self.ContentRect.localPosition
-- 	pos.x = width * (count - 1)
-- 	self.ContentRect.localPosition = pos
-- end

-- 获取childMono
function UIWAccordion:GetChildMono(data)
    local item_child_list = self.item_list[AccordionItemType.child]
    return item_child_list[data].mono
end

-- 获取parentMono
function UIWAccordion:GetParentMono(data)
    local item_parent_list = self.item_list[AccordionItemType.parent]
    return item_parent_list[data.name]
end

-- 纵向跳转到某个item，这个只适用于创建完全部子节点后纵向跳一次
-- 适用于互斥的父节点使用，需要自己先选中该子节点
-- content的y锚点为1
function UIWAccordion:AutoJumpVertical(count, hight, parent_pos, parent_hight)
    if not self.is_init then
        return
    end
    if not self.ContentRect then
		self.ContentRect = self.ContentParent:GetComponent(typeof(UnityEngine.RectTransform))
	end
	local pos = self.ContentRect.localPosition
    pos.y = hight * (count - 1) + (parent_pos - 1) * parent_hight
    -- 检测是否跳跃越界
    if pos.y < 0 then
        pos.y = 0
    end
    local max_y = UH.SizeDelta(self.ContentRect).y - UH.SizeDelta(self.gameObject).y
    if pos.y > max_y then
        pos.y = max_y
    end
    TimeHelper:CancelTimer(self.AutoJumpTimer)
    self.AutoJumpTimer = TimeHelper:AddDelayFrameTimer(function()
        self.ContentRect.localPosition = pos
    end, 3)
end

AccordionParentItem = AccordionParentItem or DeclareMono("AccordionParentItem", UIWidgetBaseItem)

function AccordionParentItem:SetData(data)
    UH.SetText(self.LblTitle, data.name)
    UIWidgetBaseItem.SetData(self, data)
end

function AccordionParentItem:OnSelect(selected)
    if self.Arrow then
        self.Arrow:SetActive(not selected)
    end
end

AccordionChildItem = AccordionChildItem or DeclareMono("AccordionChildItem", UIWidgetBaseItem)

function AccordionChildItem:SetData(data)
    UH.SetText(self.Name, data.name)
    UIWidgetBaseItem.SetData(self, data)
end

function AccordionChildItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

-- 只是拿来计算content_clild的高度 -- 加了这个就不要加content size fitter了
AccordionContentChild = AccordionContentChild or DeclareMono("AccordionContentChild")
function AccordionContentChild:SetData(data)
    if data == nil then
        return
    end
    self.data = data
    self.count = #data
    local child_h = self.ChildRectTrans.rect.size.y
    self.height =  self.LayoutGroup.padding.top +  self.LayoutGroup.padding.bottom + (self.LayoutGroup.spacing + child_h) * self.count - self.LayoutGroup.spacing
    self.RectTrans:SetSizeWithCurrentAnchors(1, self.height)
end
function AccordionContentChild:GetData(data)
    return self.data
end