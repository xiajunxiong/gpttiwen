IllusionSpLevelUpView = IllusionSpLevelUpView or DeclareView("IllusionSpLevelUpView", IllusionConfig.ResPath .. "illusion_sp_level_up")
VIEW_DECLARE_MASK(IllusionSpLevelUpView, ViewMask.BgBlock)
function IllusionSpLevelUpView:IllusionSpLevelUpView()
    self.Board:SetData(self:GetType(), Language.IllusionUp.Title, Vector2.New(1064, 582))
end

function IllusionSpLevelUpView:LoadCallback(param_t)
    
end

function IllusionSpLevelUpView:CloseCallback()

end

function IllusionSpLevelUpView:OnCloseClick()
    ViewMgr:CloseView(IllusionSpLevelUpView)
end

----------------------------IllusionSpLevelUpPanel----------------------------
IllusionSpLevelUpPanel = IllusionSpLevelUpPanel or DeclareMono("IllusionSpLevelUpPanel", UIWFlushPanel)
function IllusionSpLevelUpPanel:IllusionSpLevelUpPanel()
    self.Data = IllusionData.Instance
    self.language = Language.IllusionUp
    self.data = self.Data.illusion_lv_info
    self.select_data = self.Data.sp_lv_up_select
    self.num_count = 8
    self.data_cares = {
        {data = self.data, func = self.JumpNext, init_call = false},
        {data = self.select_data, func = self.FlushPage, keys = {"page"}, init_call = false},
        {data = self.select_data, func = self.FlushRight, keys = {"index"}, init_call = false},
    }
end

function IllusionSpLevelUpPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.point_datas = self.Data:GetSpLvUpData()
    self.min_page = 1
    self.max_page = #self.point_datas
    self:JumpNext()
end

function IllusionSpLevelUpPanel:JumpNext()
    self.ObjBlock:SetActive(false)
    local jump_lv = self.data.info.sp_level == self.Data:IllusionSpMaxLv() and self.Data:IllusionSpMaxLv() or (self.data.info.sp_level + 1)
    local page, index = self:GetPageIndex(jump_lv)
    self.Data:SpLvUpPage(page)
    self.Data:SpLvUpIndex(index)
    self.select_data:SetDirty("page")
end

-- 根据专精等级转换
function IllusionSpLevelUpPanel:GetPageIndex(lv)
    -- 8个一组
	local num = 8
    local k_1 = (lv - 1)
    return math.floor(k_1 / num) + 1, (k_1 % num) + 1
end

function IllusionSpLevelUpPanel:FlushPage()
    for i = 1, self.Points:Length() do
        self.Points[i]:SetData(self.point_datas[self.select_data.page][i])
    end
    self:PageChange(0)
    self:FlushIndex()
end

function IllusionSpLevelUpPanel:FlushIndex()
    self.Points[self.select_data.index]:SetToggle(true)
    self.select_data:SetDirty("index")
end

function IllusionSpLevelUpPanel:FlushRight()
    local now_data = self.point_datas[self.select_data.page][self.select_data.index]
    local now_sp_lv = self.data.info.sp_level
    local total_star = self.data.info.total_level
    UH.SetText(self.TxtLevel, Format(self.language.SpLv, now_data.level))
    self.AttrList:SetData(self:DealAttrData(now_data, now_sp_lv))

    local is_can = total_star >= now_data.star_level
    local color = is_can and COLORSTR.Green9 or COLORSTR.Red20
    UH.SetText(self.TxtCondition, Format(self.language.Condition, color, total_star, now_data.star_level))

    self.IHandler.Interactable = is_can
    UH.SetText(self.TxtUp, self.language.BtnUp[is_can and 1 or 2])
    self.ObjActived:SetActive(now_sp_lv >= now_data.level)
end

function IllusionSpLevelUpPanel:DealAttrData(now_data, now_sp_lv)
    local list = {}
    local is_old = now_sp_lv >= now_data.level
    local select_attr = self.Data:GetSpLvUpAttr(nil, now_data.level)
    if is_old then
        for k, v in pairs(select_attr) do
            local t = {}
            t.is_old = true
            t.now = v
            table.insert(list, t)
        end
    else
        local now_attr = self.Data:GetSpLvUpAttr()
        for k, v in pairs(select_attr) do
            local t = {}
            t.new = not now_attr[k]
            if not now_attr[k] then
                t.now = v
            else
                t.now = now_attr[k]
                if (v.attr_value - now_attr[k].attr_value) > 0 then
                    t.add = CommonStruct.AttrAdd(k, v.attr_value, -now_attr[k].attr_value)
                end
            end
            table.insert(list, t)
        end
    end
    table.sort(list, function(a, b)
        local a_index = a.new and 1000 or 0
        local b_index = b.new and 1000 or 0
        return (a.now.attr_type + a_index) < (b.now.attr_type + b_index)
    end)
    return list
end

function IllusionSpLevelUpPanel:PageChange(value)
    self.select_data.page = self.select_data.page + value
    if self.select_data.page < self.min_page then
        self.select_data.page = self.min_page
    end
    if self.select_data.page > self.max_page then
        self.select_data.page = self.max_page
    end
    self.ObjLeft:SetActive(self.select_data.page ~= self.min_page)
    self.ObjRight:SetActive(self.select_data.page ~= self.max_page)
end

function IllusionSpLevelUpPanel:OnSelect(mono, value)
    UIWidgetBaseItem.OnSelect(mono, value)
    local mono_data = mono:GetData()
    if value then
        self.now_select_mono = mono
        self.Data:SpLvUpIndex(mono_data.my_index)
    end
end

function IllusionSpLevelUpPanel:OnUpClick()
    local now_data = self.point_datas[self.select_data.page][self.select_data.index]
    local now_sp_lv = self.data.info.sp_level
    if now_sp_lv >= now_data.level then
        PublicPopupCtrl.Instance:Center(self.language.Tip[7])
        return
    elseif now_data.level > (now_sp_lv + 1) then
        PublicPopupCtrl.Instance:Center(self.language.Tip[6])
        return
    end
    local can_up, str, p1, p2, p3 = self.Data:IsCanSpLvUp()
    if can_up then
        if self.now_select_mono then
            self.now_select_mono:PlayEffect()
            self.ObjBlock:SetActive(true)
        end
    else
        if str then
            PublicPopupCtrl.Instance:Center(str)
        end
    end
end

function IllusionSpLevelUpPanel:Send()
    IllusionCtrl.Instance:SendIllusionUp(IllusionConfig.IllusionUpReqType.SpLvUp)
end

function IllusionSpLevelUpPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

--------------IllusionSpLevelUpStarItem--------------
IllusionSpLevelUpStarItem = IllusionSpLevelUpStarItem or DeclareMono("IllusionSpLevelUpStarItem", UIWidgetBaseItem)
function IllusionSpLevelUpStarItem:IllusionSpLevelUpStarItem()
    self.Data = IllusionData.Instance
end

function IllusionSpLevelUpStarItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local now_sp_lv = self.Data.illusion_lv_info.info.sp_level
    self.ObjNext:SetActive(data.level == (now_sp_lv + 1))
    self.ObjAct:SetActive(now_sp_lv >= data.level)
    if self.ObjActLine then
        self.ObjActLine:SetActive(now_sp_lv >= data.level)
    end
    UH.SetText(self.TxtNum, data.star_level)
end

function IllusionSpLevelUpStarItem:SetToggle(value)
    self.Selector.isOn = value
end

function IllusionSpLevelUpStarItem:PlayEffect()
    if self.ef_ht then
        self.EffectTool:Stop(self.ef_ht)
    end
    self.ef_ht = self.EffectTool:Play("7165460")
end

function IllusionSpLevelUpStarItem:OnDestroy()
    
end

--------------IllusionSpLevelUpAttrItem--------------
IllusionSpLevelUpAttrItem = IllusionSpLevelUpAttrItem or DeclareMono("IllusionSpLevelUpAttrItem", UIWidgetBaseItem)
function IllusionSpLevelUpAttrItem:IllusionSpLevelUpAttrItem()
    self.Data = IllusionData.Instance
end

function IllusionSpLevelUpAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local color = data.new and COLORSTR.Green9 or COLORSTR.Red13
    UH.SetText(self.TxtName, ColorStr(data.now.attr_name, color))
    self.ObjOld:SetActive(data.is_old or false)
    if not data.is_old then
        if data.add then
            self.ObjArrow:SetActive(true)
            UH.SetText(self.TxtAdd, data.add.attr_percent_value)
            self.TxtAdd:SetObjActive(true)
        else
            self.ObjArrow:SetActive(false)
            self.TxtAdd:SetObjActive(false)
        end
        UH.SetText(self.TxtValue, ColorStr("+"..data.now.attr_percent_value, color))
        
        self.ObjNew:SetActive(data.new or false)
        
    else
        UH.SetText(self.TxtOld, data.now.attr_percent_value)
    end
end

function IllusionSpLevelUpAttrItem:OnDestroy()
    
end