--属性变化
AttrChange = AttrChange or DeclareView("AttrChange", "main_other/attr_change", 0, ViewLayer.Top - 2)
VIEW_DECLARE_LEVEL(AttrChange, ViewLevel.Lt)
VIEW_DECLARE_MASK(AttrChange, ViewMask.None)


AttrChangePanel = AttrChangePanel or DeclareMono("AttrChangePanel", UIWFlushPanel)
-- ParentObj = GameObject
-- AddObj = GameObject
-- SubObj = GameObject
-- TitleAnim = Animator
-- TitleText = Text
function AttrChangePanel:AttrChangePanel()
    self.data = AttrChangeData.Instance
    self.attr_cache = {}
    self.attr_item_cache = {}
    self.data_cares = {
        {data = self.data.change_info, func = self.FlushAttrInfo}
    }
    self.sameAttrTypeChanged = false
    self.showd_attr_count = 0
end
function AttrChangePanel:FlushAttrInfo()
    local info = self.data:GetChangeInfo()
    if table.nums(info) == 0 then
        return
    end
    local sortTable = {}
    --排序
    for _, v in ipairs(info) do
        if AttrTypeSort[v.key] ~= nil then
            sortTable[AttrTypeSort[v.key]] = v
        else
            sortTable[v.key] = v
        end
    end
    for _,v in pairs(sortTable) do
        table.insert(self.attr_cache, v)
    end
    self.data:ClearCache()
    if self.anim_event == nil then
        self.anim_event =
            self.TitleAnim:ListenEvent(
            "AttrFloat",
            function(eve_param, anim_st_info)
                if eve_param == "wait_end" then
                    -- TimeHelper:AddDelayTimer(BindTool.Bind(self.CheckText, self), 0.2)
                -- elseif eve_param == "hide_start" then
                --     self.title_hide_start = true
                elseif eve_param == "hide_end" then
                    self.title_hide_start = false
                    self.TitleObj:SetActive(false)
                end
            end
        )
    end
    function startCall()
        if self.unityMono:Equals(nil) then
            return
        end
        self.TitleText.text = string.format(Language.AttrChange.attrChangeTitleFormat, self.data:GetShowTypeStr(), self.data:GetShowName())

        self.TitleObj:SetActive(true)
        TimeHelper:AddDelayTimer(BindTool.Bind(self.CheckText, self), 0.4)
   
    end

    if self.title_hide_start then
        self.title_hide_start = false
        self.TitleObj:SetActive(false)
    end
    if self.force_hide_delay ~= nil then
        TimeHelper:CancelTimer(self.force_hide_delay)
        self.force_hide_delay = nil
    end
    self.force_hide_delay = TimeHelper:AddDelayTimer(function ()
        self:hideAll()
    end, #self.attr_cache)
    TimeHelper:AddDelayFrameTimer(function ()
        startCall()
    end,1)
end

function AttrChangePanel:CheckText()
    if not ViewMgr:IsOpen(AttrChange) then
        return
    end
    if table.nums(self.attr_cache) > 0 then
        self.showd_attr_count = self.showd_attr_count + 1

        if self.showd_attr_count <= 6 then
            local attr = table.remove(self.attr_cache, 1)
            self:flyText(attr)
            TimeHelper:AddDelayTimer(BindTool.Bind(self.CheckText, self), 0.2)
        else
            TimeHelper:AddDelayTimer(BindTool.Bind(self.hideAttrs, self), 0.2)
            TimeHelper:AddDelayTimer(BindTool.Bind(self.CheckText, self), 1)
        end

        if self.hideAllDelay ~= nil then
            TimeHelper:CancelTimer(self.hideAllDelay)
            self.hideAllDelay = nil
        end
    else
        if self.hideAllDelay == nil then
            self.hideAllDelay = TimeHelper:AddDelayTimer(BindTool.Bind(self.hideAll, self), 2)
        end
    end
end

function AttrChangePanel:hideAttrs()
    while table.nums(self.attr_item_cache) > 0 do
        local mono = table.remove(self.attr_item_cache, 1)
        xpcall(function()
            mono.Anim:SetTrigger(APH("Hide"))
        end,
        function (err)
                LogError("hideAttrs ErrorLog:"..err)
        end)
        
    end
    self.showd_attr_count = 0
end

function AttrChangePanel:hideAll()
    self.hideAllDelay = nil
    self:hideAttrs()
    if self.force_hide_delay ~= nil then
        TimeHelper:CancelTimer(self.force_hide_delay)
        self.force_hide_delay = nil
    end
    xpcall(function()
            self.title_hide_start = true
            self.TitleAnim:SetTrigger(APH("Hide"))
        end,
        function (err)
                LogError("TitleAnim SetTrigger Hide ErrorLog:"..err)
        end)
    self.showd_attr_count = 0
end

--创建属性变化信息条
function AttrChangePanel:flyText(attr)
    if not self:Destroyed() then
        local obj
        if attr.change_val > 0 then
            obj = self.AddObj:Clone(self.ParentObj)
        else
            obj = self.SubObj:Clone(self.ParentObj)
        end
        local mono = obj:GetLuaComponent("AttrChangeItem")
        mono:SetData(attr)
        table.insert(self.attr_item_cache, mono)
        obj:SetActive(true)
    else
        LogError("AttrChangePanel flyText unityMono is nil")
    end
end

function AttrChangePanel:OnDestroy()
    --LogError("AttrChangePanel OnDestroy")
    UIWFlushPanel.OnDestroy(self)
    if self.hideAllDelay ~= nil then
        TimeHelper:CancelTimer(self.hideAllDelay)
        self.hideAllDelay = nil
    end
end

AttrChangeItem = DeclareMono("AttrChangeItem", UIWidgetBaseItem)
-- LblValue = Text
-- ChangeValue = Text
function AttrChangeItem:SetData(attr)
    local old_value_str = ""
    local change_value_str = "" 
    local from_type = attr.from_type        -- 可为空
    local common = Language.Common
    if from_type == AttrChangeData.ChangeFromType.ShengQi then
        local attri_info = ShengQiData.Instance:FormatAttriInfo(attr.key, attr.old_val)
        local name = attri_info.name
        local old_value = attri_info.value_str
        old_value_str = Format(common.AttrAddChangeFormat,name,old_value)

        local change_val = attr.change_val > 0 and "+" or ""
        local change_info = ShengQiData.Instance:FormatAttriInfo(attr.key, attr.change_val)
        change_value_str = change_val .. change_info.value_str
    else
        local old_value = self:GetPercent(attr.old_val,attr.key)
        old_value_str = Format(common.AttrAddChangeFormat,common.AttrList[attr.key],old_value)

        local change_val = attr.change_val > 0 and "+" or ""
        change_value_str = change_val .. self:GetPercent(attr.change_val,attr.key)
    end

    UH.SetText(self.LblValue,old_value_str)
    UH.SetText(self.ChangeValue, change_value_str)

    if self.anim_event == nil then
        self.Anim:ListenEvent("AttrFloat",
        function(eve_param, anim_st_info)
            if eve_param == "hide_end" then
                self.gameObject:SetActive(false)
                UnityGameObj.Destroy(self.gameObject)
            end
        end)
    end
end

function AttrChangeItem:GetPercent(value,attr_type)
    if DataHelper.IsPercent(attr_type) then
		return Percent(value)
    end
    return value
end