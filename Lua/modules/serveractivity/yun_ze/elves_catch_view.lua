ElvesCatchView = ElvesCatchView or DeclareView("ElvesCatchView", "serveractivity/yun_ze/elves_catch", Mod.YunZe.ElvesCatch)
VIEW_DECLARE_LEVEL(ElvesCatchView, ViewLevel.L1)
VIEW_DECLARE_MASK(ElvesCatchView, ViewMask.BgBlock)
function ElvesCatchView:ElvesCatchView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_ELVES_CATCH, TimeType.Type_Time_3)
end

function ElvesCatchView:OnCloseClick()
    ViewMgr:CloseView(ElvesCatchView)
end

function ElvesCatchView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.jinglingdazhuabu_auto.other[1].tips)
end

----------------------------ElvesCatchPanel----------------------------
ElvesCatchPanel = ElvesCatchPanel or DeclareMono("ElvesCatchPanel", UIWFlushPanel)
function ElvesCatchPanel:ElvesCatchPanel()
    self.Data = YunZeData.Instance
    self.reward_config = self.Data:GetElvesCatchRewardConfig()
    self.other = Config.jinglingdazhuabu_auto.other[1]
    self.language = Language.ElvesCatch
    self.data = self.Data.elves_catch_data
    -- 每次一键抓捕最大次数
    self.max_all_num = 99
    -- 每次一键抓捕最大消耗数量
    self.max_consume_num = self.max_all_num * self.other.quantity_item
    self.data_cares = {
        {data = self.data, func = self.OpenGetView, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushByItem, init_call = false}
    }
end

function ElvesCatchPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.item_name = Item.GetName(self.other.cost_item)
    UH.SetText(self.TxtItemName, self.item_name)
    UH.SetText(self.TxtGet, self.other.desc)
    for i = 1, self.List:Length() do
        self.List[i]:SetData(Config.jinglingdazhuabu_auto.basics[i])
    end
    UH.SpriteName(self.ImgSetExtraHead, "Head_" .. self.other.icon)
    UH.SpriteName(self.ImgSetExtraBox, "Box_" .. self.other.icon)
    self.extra_item_list = self.reward_config[self.other.reward_group_added].reward_item
    self:FlushByItem()
end

function ElvesCatchPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function ElvesCatchPanel:GetOne()
    if self.is_enought then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ELVES_CATCH, 1)
    else
        if self.other.act_type > 0 then
            if ActivityData.IsOpen(self.other.act_type) then
                ViewMgr:OpenViewByKey(self.other.mod_key)
                ViewMgr:CloseView(ElvesCatchView)
                ViewMgr:CloseView(YunZeView)
            else
                PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
            end
        else
            ViewMgr:OpenViewByKey(self.other.mod_key)
            ViewMgr:CloseView(ElvesCatchView)
            ViewMgr:CloseView(YunZeView)
        end
    end
end

function ElvesCatchPanel:GetAll()
    if self.is_enought then
        local can_catch_num = math.floor(self.has_item / self.other.quantity_item)
        local need_consume_num = can_catch_num * self.other.quantity_item
        local is_reach_max = can_catch_num > self.max_all_num
        local str = Format(self.language.AllCatch, is_reach_max and self.max_all_num or can_catch_num, is_reach_max and self.max_consume_num or need_consume_num, self.item_name, self.max_all_num)
        PublicPopupCtrl.Instance:AlertTip(str, function()
            ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ELVES_CATCH, 2)
        end)
    else
        PublicPopupCtrl.Instance:Center(Format(self.language.NotEnought, self.item_name))
        self:OpenGetWay()
    end
end

function ElvesCatchPanel:OpenGetWay()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.other.cost_item})})
end

function ElvesCatchPanel:FlushByItem()
    self.has_item = Item.GetNum(self.other.cost_item)
    self.is_enought = self.has_item >= self.other.quantity_item
    self.OneRemind:SetNum(self.is_enought and 1 or 0)
    self.AllRemind:SetNum(self.is_enought and 1 or 0)
    UH.SetText(self.TxtNum, Format(Language.Common.Cstr, self.is_enought and COLORSTR.Green7 or COLORSTR.Red8, self.has_item, self.other.quantity_item))
    UH.SetText(self.TxtGetOne, self.is_enought and self.language.Catch or self.language.GotoGet)
end

function ElvesCatchPanel:OpenGetView()
    ViewMgr:OpenView(ElvesCatchGetView)
end

function ElvesCatchPanel:ExtraShow()
    local tab = {
        pos = UH.GetRootPosition(self.ObjExtraShowPos),
        item_list = DataHelper.FormatItemList(self.extra_item_list),
        pivot = 0,
    }
    ViewMgr:OpenView(ShowItemView, tab)
end

----------------------------ElvesCatchItem----------------------------
ElvesCatchItem = ElvesCatchItem or DeclareMono("ElvesCatchItem", UIWidgetBaseItem)
ElvesCatchPersonEffect = {
    [1] = "3161066",
    [2] = "3161064",
    [3] = "3161065",
}
function ElvesCatchItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.reward_config = YunZeData.Instance:GetElvesCatchRewardConfig()
    TimeHelper:CancelTimer(self.delay_handler)
    self.delay_handler = TimeHelper:AddRunFrameTimer(function()
        self.EffectTool:Play(ElvesCatchPersonEffect[data.contour], self.ObjPerson)
    end)
end

function ElvesCatchItem:OnClick()
    local tab = {
        pos = UH.GetRootPosition(self.ObjShowPos),
        item_list = DataHelper.FormatItemList(self.reward_config[self.data.reward_group].reward_item),
        pivot = tonumber(self.ShowPivot),
    }
    ViewMgr:OpenView(ShowItemView, tab)
end

function ElvesCatchItem:OnDestroy()
    TimeHelper:CancelTimer(self.delay_handler)
end


-------------------ElvesCatchGetView-------------------
ElvesCatchGetView = ElvesCatchGetView or DeclareView("ElvesCatchGetView", "serveractivity/yun_ze/elves_catch_get")
VIEW_DECLARE_MASK(ElvesCatchGetView, ViewMask.BgBlockClose)
function ElvesCatchGetView:ElvesCatchGetView()
    self.ht = YunZeData.Instance.elves_catch_data:Care(BindTool.Bind(self.FlushPanel, self))
    self:FlushPanel()
end

function ElvesCatchGetView:FlushPanel()
    local config = YunZeData.Instance:GetElvesCatchGetData()
    local num = #config
    self.ScrollRect.vertical = num > 2
    if num == 1 then
        UH.SizeDelta(self.List.gameObject, Vector2.New(750, 140))
    elseif num == 2 then
        UH.SizeDelta(self.List.gameObject, Vector2.New(750, 260))
    else
        UH.SizeDelta(self.List.gameObject, Vector2.New(750, 300))
    end
    self.List:SetData(config)
end

function ElvesCatchGetView:CloseCallback()
    YunZeData.Instance.elves_catch_data:Uncare(self.ht)
end

----------------------------ElvesCatchGetItem----------------------------
ElvesCatchGetItem = ElvesCatchGetItem or DeclareMono("ElvesCatchGetItem", UIWidgetBaseItem)
function ElvesCatchGetItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.ImgSetHead, "Head_" .. data.contour)
    UH.SetText(self.TxtReward, data.is_sp and Language.ElvesCatch.Reward[2] or Language.ElvesCatch.Reward[1])
    UH.SetText(self.TxtRewardNum, Format(Language.ElvesCatch.GetNum, data.num))
    self.ObjLine:SetActive(data.is_show_line)
    local reward_config = {}
    for k, v in pairs(data.reward) do
        local t = TableCopy(v)
        t.num = t.num * data.num
        table.insert(reward_config, t)
    end
    self.RewardList:SetData(DataHelper.FormatItemList(reward_config))
end