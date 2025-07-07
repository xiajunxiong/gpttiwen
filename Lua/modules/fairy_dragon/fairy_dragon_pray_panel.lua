-- 神龙祈愿
FairyDragonPrayPanel = FairyDragonPrayPanel or DeclareMono("FairyDragonPrayPanel", UIWFlushPanel)
function FairyDragonPrayPanel:FairyDragonPrayPanel()
    self.data = FairyDragonData.Instance
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = self,
                show_shadow = true,
			}
		)
    end
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushStuff, init_call = false},
        {data = self.data.pray_result_info, func = self.FlushResult, init_call = false}
    }
end

function FairyDragonPrayPanel:Awake()
    UIWFlushPanel.Awake(self)
    local list = self.data:GetPrayShowList()
    for i = 1, self.ShowItemList:Length() do
        local mono = self.ShowItemList[i]
        if list[i] then
            mono:SetObjActive(true)
            mono:SetData(list[i])
        else
            mono:SetObjActive(false)
        end
    end
    self.one_stuff = self.data:GetOnePrayStuff()
    self.ten_stuff = self.data:GetTenPrayStuff()
    local icon_1 = Item.GetIconId(self.one_stuff.item_id)
    local icon_2 = Item.GetIconId(self.ten_stuff.item_id)
    UH.SetIcon(self.Icon1, icon_1)
    UH.SetIcon(self.Icon10, icon_2)
    UH.SetIcon(self.StuffIcon1, icon_1)
    UH.SetIcon(self.StuffIcon2, icon_2)

    local res_id = self.data.other_cfg.qiyuan_res
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(res_id))

    self:FlushStuff()
end

function FairyDragonPrayPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
    end
    self:ShowResult()
end


function FairyDragonPrayPanel:OnOnePrayClick()
    local has_num = Item.GetNum(self.one_stuff.item_id)
    if has_num >= self.one_stuff.num then
        FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.SINGLE_PRAY)
    else
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.NotItemTip)
        MainOtherCtrl.Instance:GetWayViewById(self.one_stuff.item_id)
    end
end

function FairyDragonPrayPanel:OnTenPrayClick()
    local has_num = Item.GetNum(self.ten_stuff.item_id)
    if has_num >= self.ten_stuff.num then
        FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.TEN_PRAY)
    else
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.NotItemTip)
        MainOtherCtrl.Instance:GetWayViewById(self.ten_stuff.item_id)
    end
end

function FairyDragonPrayPanel:OnRateClick()
    ViewMgr:OpenView(FairyDragonPrayRateView)
end

function FairyDragonPrayPanel:OnAddClick(type)
    type = type or 1
    local item_id = type == 1 and self.one_stuff.item_id or self.ten_stuff.item_id
    --MarketData.Instance:SetJumpIndex(item_id)
    --ViewMgr:OpenViewByKey(Mod.Mall.Market)
    MallCtrl.Instance:Jump(item_id)
end

function FairyDragonPrayPanel:FlushStuff()
    local has_num_1 = Item.GetNum(self.one_stuff.item_id)
    local need_num = self.one_stuff.num
    local color = has_num_1 >= need_num and COLORSTR.Green3 or COLORSTR.Red8
    local num_str = ColorStr(has_num_1, color)
    UH.SetText(self.StuffNum1, string.format("%s/%s", num_str, need_num))

    local has_num_2 = Item.GetNum(self.ten_stuff.item_id)
    need_num = self.ten_stuff.num
    color = has_num_2 >= need_num and COLORSTR.Green3 or COLORSTR.Red8
    num_str = ColorStr(has_num_2, color)
    UH.SetText(self.StuffNum2, string.format("%s/%s", num_str, need_num))
    self.OneRemind:SetNum(self.data:RemindPrayOne())
    self.TenRemind:SetNum(self.data:RemindPrayTen())
end

function FairyDragonPrayPanel:OnStuffClick(type)
    type = type or 1
    local item_id = type == 1 and self.one_stuff.item_id or self.ten_stuff.item_id
    local data = Item.Create({item_id = item_id})
    local info = {
		item_list = {data},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function FairyDragonPrayPanel:FlushResult()
    local info = self.data.pray_result_info
    if TableIsEmpty(info) then
        return
    end
    local count = table.nums(info)
    local qua_list = {}
    for i, v in pairs(info) do
        local cfg = self.data:GetPrayItemCfg(v)
        local item_id = cfg.item.item_id
        local qua = Item.GetColor(item_id)
        table.insert(qua_list, qua)
    end
    self.ui_obj:PlayAnim(ChAnimType.Show2)
    self.Bottom:SetActive(false)
    self.TaiZi:SetActive(false)
    self.Lock:SetActive(true)
    if count == 1 then
        self.EffectFunc:PlayOne(qua_list[1], function ()
            self:ShowResult()
            --self.ui_obj:PlayAnim(ChAnimType.Attack)
            self.Bottom:SetActive(true)
            self.TaiZi:SetActive(true)
            self.Lock:SetActive(false)
        end)
    else
        self.EffectFunc:PlayTen(qua_list, function ()
            self:ShowResult()
            --self.ui_obj:PlayAnim(ChAnimType.Attack)
            self.Bottom:SetActive(true)
            self.TaiZi:SetActive(true)
            self.Lock:SetActive(false)
        end)
    end
end

function FairyDragonPrayPanel:ShowResult()
    local info = self.data.pray_result_info:Val()
    if TableIsEmpty(info) then
        return
    end
    ViewMgr:OpenView(FairyDragonPrayResultView, {pray_list = info})
    self.data.pray_result_info:Set({})
end



FairyDragonPrayShowItem = FairyDragonPrayShowItem or DeclareMono("FairyDragonPrayShowItem", UIWidgetBaseItem)
function FairyDragonPrayShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item_id = data.item.item_id
    UH.SetIcon(self.Icon, Item.GetIconId(item_id), ICON_TYPE.ITEM)
end

function FairyDragonPrayShowItem:OnClick()
    local item = Item.Create(self.data.item)
    local info = {
		item_list = {item},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end


-- 祈愿结果
FairyDragonPrayResultView = FairyDragonPrayResultView or DeclareView("FairyDragonPrayResultView","fairy_dragon/fairy_dragon_pray_result_view")
VIEW_DECLARE_MASK(FairyDragonPrayResultView,ViewMask.BgBlockClose)
function FairyDragonPrayResultView:FairyDragonPrayResultView()
    self.data = FairyDragonData.Instance
end

function FairyDragonPrayResultView:LoadCallback(param_t)
    local list = param_t.pray_list
    if TableIsEmpty(list) then
        return
    end
    self.ItemOne:SetObjActive(#list == 1)
    self.ItemList:SetObjActive(#list > 1)
    local cfg_list = {}
    local item_list = {}
    for i, v in pairs(list) do
        local cfg = self.data:GetPrayItemCfg(v)
        table.insert(cfg_list, cfg)
        local item = Item.Create(cfg.item)
        table.insert(item_list, item)
    end
    if #list == 1 then
        self.ItemOne:SetData(cfg_list[1])
    end
    if #list > 1 then
        self.ItemList:SetData(cfg_list)
    end
    PublicPopupCtrl.Instance:ShowCustom(item_list)
end


FairyDragonPrayResultItem = FairyDragonPrayResultItem or DeclareMono("FairyDragonPrayResultItem", UIWidgetBaseItem)
function FairyDragonPrayResultItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.NiceBG:SetActive(data.is_broadcast == 1)
    self.ComBG:SetActive(data.is_broadcast ~= 1)
    local item = Item.Create(data.item)
    self.ItemCell:SetData(item)
    UH.SetText(self.ItemName, item:QuaName(true))
end



-- 概率展示
FairyDragonPrayRateView = FairyDragonPrayRateView or DeclareView("FairyDragonPrayRateView","fairy_dragon/fairy_dragon_pray_rate_view")
function FairyDragonPrayRateView:FairyDragonPrayRateView()
    self.Board:SetData(self:GetType(),Language.FairyDragon.Title1,Vector2.New(800, 624))
end

function FairyDragonPrayRateView:LoadCallback(param_t)
    local cfg = Config.god_dragon_auto.rate_show
    local group_cfg = DataHelper.ConvertGroup(cfg,"type")
    local list_data = {}
    for i,v in pairs(group_cfg) do
        local data = {}
        data.name = v[1].type_name or ""
        data.list = v
        data.type = i
        table.insert(list_data, data)
    end
    table.sort(list_data, function (a,b)
        return a.type < b.type
    end)
    self.List:SetData(list_data)
end


FairyDragonPrayRateItem = FairyDragonPrayRateItem or DeclareMono("FairyDragonPrayRateItem", UIWidgetBaseItem)
function FairyDragonPrayRateItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.List:SetData(data.list)
    UH.SetText(self.Title, data.name)
end


FairyDragonPrayRateItemChildItem = FairyDragonPrayRateItemChildItem or DeclareMono("FairyDragonPrayRateItemChildItem", UIWidgetBaseItem)
function FairyDragonPrayRateItemChildItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data.item})
    local rate = string.format(Language.FairyDragon.Text4, (data.rate / 100))
    self.ItemCell:SetData(item)
    UH.SetText(self.Rate, rate)
end



FairyDragonPrayEffect = FairyDragonPrayEffect or DeclareMono("FairyDragonPrayEffect")
function FairyDragonPrayEffect:FairyDragonPrayEffect()
    self.show_effect_list = {}
    self.qua_cfg = {
        [ItemColor.Green] = "3165300",
        [ItemColor.Blue] = "3165301",
        [ItemColor.Purple] = "3165302",
        [ItemColor.Orange] = "3165303",
        [ItemColor.Red] = "3165304",
        [ItemColor.Gold] = "3165304",
        [ItemColor.Black] = "3165304",
    }
end

function FairyDragonPrayEffect:OnDestroy()
    TimeHelper:CancelTimer(self.time_ht)
end

function FairyDragonPrayEffect:PlayTen(qua_list, func)
    if self.time_ht then
        return
    end
    table.insert(self.show_effect_list, self.TenEffectObj)
    local child_count = self.TenEffectRot.transform.childCount
    local qua_count = table.nums(qua_list)
    if child_count ~= qua_count then
        LogDG("神龙祈神播放特效异常", qua_list)
    end
    for i = 0, child_count - 1 do
        local obj = self.TenEffectRot.transform:GetChild(i)
        local qua = qua_list[i + 1]
        local child_name = self.qua_cfg[qua]
        local effect_obj = obj.transform:Find(child_name).gameObject
        table.insert(self.show_effect_list, effect_obj)
    end
    self.end_func = func
    self:Play(6)
end

function FairyDragonPrayEffect:PlayOne(qua, func)
    if self.time_ht then
        return
    end
    table.insert(self.show_effect_list, self.OneEffectObj)
    local child_count = self.OneEffectRot.transform.childCount
    for i = 0, child_count - 1 do
        local obj = self.OneEffectRot.transform:GetChild(i)
        local child_name = self.qua_cfg[qua] or "3165300"
        local effect_obj = obj.transform:Find(child_name).gameObject
        table.insert(self.show_effect_list, effect_obj)
    end
    self.end_func = func
    self:Play(5.5)
end

function FairyDragonPrayEffect:Play(time)
    self.SoulAnim:get_Item("pray_item").speed = 1
    self.SoulAnim:Play("pray_item")
    table.insert(self.show_effect_list, self.GuoDu)
    for i,v in pairs(self.show_effect_list) do
        v:SetActive(true)
    end
    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        self:PlayEnd()
    end, time or 6)
end

function FairyDragonPrayEffect:PlayEnd()
    for i,v in pairs(self.show_effect_list) do
        v:SetActive(false)
    end
    if self.end_func then
        self.end_func()
    end
    self.end_func = nil
    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = nil
    local anim_clip = self.SoulAnim:get_Item("pray_item")
    anim_clip.time = anim_clip.length
    anim_clip.speed = -1
    self.SoulAnim:Play("pray_item")
end