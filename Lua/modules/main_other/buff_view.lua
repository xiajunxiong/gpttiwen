BuffView = BuffView or DeclareView("BuffView", "main_other/buff_view")

VIEW_DECLARE_MASK(BuffView,ViewMask.BlockClose)

BuffPanel = BuffPanel or DeclareMono("BuffPanel", UIWFlushPanel)

function BuffPanel:BuffPanel()
    self.data_cares = {
        {data = MainViewData.Instance.buff_list, func = self.FlushBuffView},
    }
end

function BuffPanel:FlushBuffView()
    local buff = MainViewData.Instance:TopOfBuff()
    if buff then
        self.BuffList:SetData(MainViewData.Instance.buff_list)
    end
    local count = MainViewData.Instance:GetBuffCount()
    if count ~= 0 then
        local num = count >= 3 and 2 or count
        local height = 140 * num + 70
        self.BuffRect.sizeDelta = Vector2.New(305,height)
    end
end

MainViewBuffItem = MainViewBuffItem or DeclareMono("MainViewBuffItem", UIWidgetBaseItem)

function MainViewBuffItem:SetData(data)
    self.data = data
    self.Name.text = Language.Main.GameBuff.Name[data.source]
    if data.source == 0 then
        self.Time:SetTime(Language.Main.GameBuff.FBING)
    else
        self.Time:StampTime(data.time_left or 0,TimeType.Type_Special_0)
    end
    self.Descr.text = MainViewData.Instance:GetBuffDescr(data)
    self.Icon.SpriteName = MainViewData.Instance:GetBuffIconName(data)
    if data.source == 1 then
        local info = ActivityData.Instance:GetTreviFountainInfo(data.end_time)
        local buff_list = Config.trevi_fountain_auto.buff_list
        UH.SetIcon(self.Icon,buff_list[info.buff_id or 1].buff_icon)
    elseif data.source == 0 then
        if data.buff_cfg then
            UH.SetIcon(self.Icon, data.buff_cfg.icon_id)
            self.Name.text = data.buff_cfg.buff_name
        end
    end
end

function MainViewBuffItem.GetItemHeightByData(data)
    local _, line = MainViewData.Instance:GetBuffDescr(data)
    return 125 + (line - 1) * 35
end

------------buff信息描述框---------------------------------------------------
BuffInfoTip = BuffInfoTip or DeclareView("BuffInfoTip", "main_other/buff_info_tip")
VIEW_DECLARE_MASK(BuffInfoTip, ViewMask.BlockClose)

function BuffInfoTip:LoadCallback(data)
    local buff_cfg = Config.buff_list_auto.buff_list[data.buff_id]
    if data.buff_cfg then
        buff_cfg = data.buff_cfg
        UH.SetText(self.BuffDescText, buff_cfg.desc_1 or "")
    else
        if buff_cfg == nil then
            --尝试获取特效
            buff_cfg = Cfg.MedalEffectById(data.buff_id)
            UH.SetText(self.BuffDescText, buff_cfg.description)
        else
            UH.SetText(self.BuffDescText, buff_cfg.desc)
        end
    end
    UH.SetIcon(self.BuffIconImg, buff_cfg.icon_id)
    UH.SetText(self.BuffNameText, buff_cfg.buff_name or buff_cfg.name)
    if data.from_view == "battle" then
        if buff_cfg.pos then
            UH.AnchoredPosition(self.PosRect, buff_cfg.pos)
        else
            UH.AnchoredPosition(self.PosRect, Vector2.New(-304, -211))
        end
    else
        UH.AnchoredPosition(self.PosRect, Vector2.New(251, -625))
    end
    if self.handle_care then
        BattleData.Instance:BaseData():Uncare(self.handle_care)
        self.handle_care = nil
    end
    self.handle_care = BattleData.Instance:BaseData():Care(function ()
        if not BattleData.Instance:IsServerFighting() then
            ViewMgr:CloseView(BuffInfoTip)
        end
    end,"server_fighting")
end
function BuffInfoTip:OnDestroy()
    if self.handle_care then
        BattleData.Instance:BaseData():Uncare(self.handle_care)
        self.handle_care = nil
    end
end

------（特殊）buff列表显示框-------------------------
BuffInfoTip2 = BuffInfoTip2 or DeclareView("BuffInfoTip2", "main_other/buff_info_list")
VIEW_DECLARE_MASK(BuffInfoTip2, ViewMask.BlockClose)
function BuffInfoTip2:BuffInfoTip2()
    self.data = MainViewData.Instance
end
function BuffInfoTip2:LoadCallback(param_t)
    if param_t.index == 2 then
        local data = self.data:BattleDebuffList()
        self.List:SetData(data)
    else
        local data = self.data:BattleBuffList()
        self.List:SetData(data)
    end
end

BuffInfoListItem = BuffInfoListItem or DeclareMono("BuffInfoListItem", UIWidgetBaseItem)
function BuffInfoListItem:SetData(data)
    UH.SetText(self.BuffDescText, data.desc)
    UH.SetText(self.BuffNameText, data.buff_name)
    UH.SetIcon(self.BuffIconImg, data.icon_id)
end