AccrssoryStrengthView = AccrssoryStrengthView or DeclareMono("AccrssoryStrengthView", UIWFlushPanel)
function AccrssoryStrengthView:AccrssoryStrengthView()
    self.Data = AccrssoryData.Instance
    self.language = Language.Accrssory
    self.data = self.Data.now_select_equip
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushShow, init_call = false},
        {data = self.data, func = self.FlushShow, init_call = false},
    }
    --抛物线1
    self.ParabolaBoard:SetData({x_offset = -2, speed_time = 4, complete_func = function()
        self.ObjBgEffect:SetActive(true)
        self.ObjBlock:SetActive(false)
    end})
    self.ObjBlock:SetActive(false)
end

function AccrssoryStrengthView:Awake()
    UIWFlushPanel.Awake(self)
end

function AccrssoryStrengthView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.ObjBgEffect:SetActive(false)
    self.Data.now_show_panel.mod_id = Mod.Accrssory.Strength
    self:FlushShow()
end

function AccrssoryStrengthView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function AccrssoryStrengthView:FlushShow()
    local info = self.data.info
    if TableIsEmpty(info) then
        UH.SetText(self.TxtStrength, self.language.BtnStrength[1])
        UH.SetText(self.TxtName, "")
        self.Cell:SetData({})
        self.Cell:SetObjActive(false)
        UH.SetText(self.TxtLevel, 0)
        self.LeftList:SetData({})
        self.RightList:SetData({})
        self.ConsumeList:SetData({})
        self.IHandlerBtn.Interactable = false
        self.ObjSelectShow:SetActive(false)
        self.ObjEmptyShow:SetActive(true)
    else
        local now_level = info:StrengthLevel()
        local cfg = self.Data:StrengthConfig(info.item_id, now_level + 1)
        UH.SetText(self.TxtName, info:Name())
        self.Cell:SetObjActive(true)
        self.Cell:SetData(info)
        UH.SetText(self.TxtLevel, now_level)
        local is_max = self.Data:IsStrengthLevelMax(now_level)
        UH.SetText(self.TxtStrength, self.language.BtnStrength[is_max and 2 or 1])
        if is_max then
            self.CenterList:SetData(info:StrengthAttr())
        else
            self.LeftList:SetData(info:StrengthAttr())
            self.RightList:SetData(self.Data:StrengthAttrConfig(info.item_id, now_level + 1))
        end
        self.CenterList:SetObjActive(is_max)
        self.ObjNotMax:SetActive(not is_max)
        local is_role_level = RoleLevel() >= (cfg.role_level or 0)
        self.IHandlerBtn.Interactable = is_role_level and not is_max

        local item_list = TableCopy(cfg.ltem_list or {}, 2)
        for k, v in pairs(item_list) do
            local now_num = Item.GetNum(v.item_id)
            local color = now_num >= v.num and COLORSTR.Green7 or COLORSTR.Red8
            v.num = Format(Language.Common.Cstr, color, now_num, v.num)
        end
        self.ConsumeList:SetData(DataHelper.FormatItemList(item_list))
        self.ObjSelectShow:SetActive(true)
        self.ObjEmptyShow:SetActive(false)
    end
end

function AccrssoryStrengthView:OnStrengthClick()
    if TableIsEmpty(self.data.info) then
        PublicPopupCtrl.Instance:Center(self.language.StrengthNowSelectEmpty)
        return
    end
    local info = self.data.info
    local now_level = info:StrengthLevel()
    -- 判断强化等级是否已满
    if self.Data:IsStrengthLevelMax(now_level) then
        PublicPopupCtrl.Instance:Center(self.language.StrengthMaxTip)
        return
    end
    local cfg = self.Data:StrengthConfig(info.item_id, now_level + 1)
    -- 判断角色等级是否不足
    if RoleLevel() < (cfg.role_level or 0) then
        PublicPopupCtrl.Instance:Center(Format(self.language.RoleLevelTip, cfg.role_level))
        return
    end
    -- 判断各个物品是否足够
    for k, v in pairs(cfg.ltem_list) do
        local now_num = Item.GetNum(v.item_id)
        if v.num > now_num then
            local need_num = v.num - now_num
            PublicPopupCtrl.Instance:Center(self.language.MaterialTip[1])
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = v.item_id, jump_need_num = need_num})})
            return
        end
    end
    local param_t = {}
    param_t.req_type = AccrssoryReqType.Strength
    param_t.param_1 = info.in_bag and 0 or 1
    if info.in_bag then
        param_t.param_2 = info:GridIndex()
    else
        param_t.param_2 = info:AccrssoryType()
    end
    AccrssoryCtrl.Instance:SendAccrssoryReq(param_t)
    self.ParabolaBoard:Play()
    self.ObjBgEffect:SetActive(false)
    self.ObjBlock:SetActive(true)
end
