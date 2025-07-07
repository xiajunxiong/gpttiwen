-- 幻兽蛋
HuanShouEggPanel = HuanShouEggPanel or DeclareMono("HuanShouEggPanel", UIWFlushPanel)
function HuanShouEggPanel:HuanShouEggPanel()
    self.data = HuanShouData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushEgg, init_call = false},
        {data = self.data.egg_record_data, func = self.FlushRecord, init_call = false},
        {data = self.data.open_egg_times_list, func = self.FlushEgg, init_call = false},
    }
end

function HuanShouEggPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function HuanShouEggPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    HuanShouCtrl.Instance:SendReq(HuanShouReqType.EggRecord)
end

function HuanShouEggPanel:FlushPanel()
    self:FlushEgg()
    self:FlushRecord()
end

function HuanShouEggPanel:FlushEgg()
    local list_data = Config.huanshou_auto.egg_item
    self.EggList:SetData(list_data)
end

function HuanShouEggPanel:FlushRecord()
    local list_data = self.data:GetEggRecord()
    self.RecordList:SetData(list_data)
end



HuanShouEggPanelItem = HuanShouEggPanelItem or DeclareMono("HuanShouEggPanelItem", UIWidgetBaseItem)
HuanShouEggPanelItem.opening = false
function HuanShouEggPanelItem:HuanShouEggPanelItem()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end

    self.effect_cfg = {
        [1077] = "7165484",
        [1076] = "7165483",
        [1078] = "7165485"
    }
end

function HuanShouEggPanelItem:OnDestroy()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
    TimeHelper:CancelTimer(self.time_ht)
    HuanShouEggPanelItem.opening = false
end

function HuanShouEggPanelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local num = Item.GetNum(data.item_id)
    UH.SetText(self.ItemNum, "  x" .. num)
    UH.SetIcon(self.ItemIcon, Item.GetIconId(data.item_id), ICON_TYPE.ITEM)
    UH.SetText(self.Name, Item.GetQuaName(data.item_id))
    if data.rare_guarantee > 0 then
        local egg_times = HuanShouData.Instance:GetOpenEggTimes(data.egg_id)
        egg_times = data.rare_guarantee - egg_times % data.rare_guarantee
        local baodi = ColorStr(string.format(Language.HuanShou.Text4, egg_times), COLORSTR.Green4)
        UH.SetText(self.Tip, string.format(Language.HuanShou.Text5, baodi))
    else
        UH.SetText(self.Tip, "")
    end
    self:FlushModel()
end

function HuanShouEggPanelItem:FlushModel()
    if self.is_flushed then
        return
    end
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(self.data.res_id))
    self.is_flushed = true
end

function HuanShouEggPanelItem:OnModelClick()
    if HuanShouEggPanelItem.opening then
        return
    end
    local item_id = self.data.item_id
    local egg = BagData.Instance:IsHasItemById(item_id)
    if not egg then
        PublicPopupCtrl.Instance:Error(Language.HuanShou.NotItemTip)
        MainOtherCtrl.Instance:GetWayViewById(item_id)
    else
        local effect = self.effect_cfg[self.data.res_id]
        self.EffectTool:Play(effect)
        self.ModelShow:SetObjActive(false)

        TimeHelper:CancelTimer(self.time_ht)
        --self.ui_obj:PlayAnim(ChAnimType.FishIdle)
        HuanShouEggPanelItem.opening = true
        self.time_ht = Invoke(function ()
            HuanShouCtrl.Instance:SendReq(HuanShouReqType.OpenEgg, egg.column_type, egg.index)
            self.time_ht = nil
            HuanShouEggPanelItem.opening = false
            self.ModelShow:SetObjActive(true)
        end,2.3) 
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.FaBaoShengJie)
    end
end

function HuanShouEggPanelItem:OnPreviewClick()
    ViewMgr:OpenView(HuanShouEggPreview, {egg_id = self.data.egg_id})
end

function HuanShouEggPanelItem:OnItemClick()
    local item_id = self.data.item_id
    local item = Item.Create({item_id = item_id})
    local info = {
		item_list = {item},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

HuanShouEggPanelRecordItem = HuanShouEggPanelRecordItem or DeclareMono("HuanShouEggPanelRecordItem", UIWidgetBaseItem)
function HuanShouEggPanelRecordItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local player_name = ColorStr(data.player_name, COLORSTR.Yellow15)
    local egg_item_id = data.egg_item_id
    local huanshou_id = data.huanshou_id
    local cfg = HuanShouData.Instance:GetCfgById(huanshou_id)
    local qua = cfg.huanshou_quality
    local qua_name, name, color = HuanShou.GetQuaName(huanshou_id)
    qua_name = ColorStr(string.format("【%s】", name), color)
    local str = string.format(Language.HuanShou.Text6,player_name, Item.GetQuaName(egg_item_id), Language.HuanShou.QuaStr3[qua],qua_name)
    UH.SetText(self.Text, str)
end





HuanShouEggPreview = HuanShouEggPreview or DeclareView("HuanShouEggPreview", "huanshou/huan_shou_egg_preview")
function HuanShouEggPreview:HuanShouEggPreview()
    self.Board:SetData(self:GetType(),Language.HuanShou.Title1,Vector2.New(798, 622))
end

function HuanShouEggPreview:LoadCallback(param_t)
    param_t = param_t or {}
    local egg_id = param_t.egg_id or 0
    local group_cfg = DataHelper.ConvertGroup(Config.huanshou_auto.reward_show, "egg_id")
    local cfg = group_cfg[egg_id]
    local list_cfg = DataHelper.ConvertGroup(cfg, "type")
    local list_data = {}
    for i,v in pairs(list_cfg) do
        local t = {}
        t.list = v
        t.qua = i
        table.insert(list_data, t)
    end
    table.sort(list_data, function (a,b)
        return a.qua < b.qua
    end)
    self.List:SetData(list_data)
end


HuanShouEggPreviewTypeItem = HuanShouEggPreviewTypeItem or DeclareMono("HuanShouEggPreviewTypeItem", UIWidgetBaseItem)
function HuanShouEggPreviewTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Title, Language.HuanShou.QuaStr2[data.qua])
    local list_data = {}
    for i,v in pairs(data.list) do
        local t = CfgTableCopy(v)
        t.is_show_line = i ~= #data.list
        table.insert(list_data, t)
    end
    self.List:SetData(list_data)
end


HuanShouEggPreviewChildItem = HuanShouEggPreviewChildItem or DeclareMono("HuanShouEggPreviewChildItem", UIWidgetBaseItem)
function HuanShouEggPreviewChildItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local huanshou_id = data.item
    local cfg = HuanShouData.Instance:GetCfgById(huanshou_id)
    local item = Item.Create({item_id = cfg.item_id})
    self.ItemCell:SetData(item)
    UH.SetText(self.RateText, string.format(Language.HuanShou.Text7, data.rate / 100))
    self.ImgObj:SetActive(data.is_show_line)
end
