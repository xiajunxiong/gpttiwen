
PartnerViewStrengthen = PartnerViewStrengthen or DeclareMono("PartnerViewStrengthen", UIWFlushPanel)

function PartnerViewStrengthen:PartnerViewStrengthen()
    -- self.flush_events = {EventProtocol.ToEventKey(SCPartnerIntensifyInfo)}

    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Strengthen

    self.data_cares = {
        {data = self.data.item_flush, func = self.onFlush, init_call = false},
        -- {data = BagData.Instance.item_grid,func = self.onFlush, init_call = false},
        -- {data = self.data.partner_up_skillf_effect,func = self.FlushSkillFEffect, init_call = false},
        {data = self.data.partner_strengthen_up_effect, func = self.FlushEffect, init_call = false},
	}
    self.partner_id_anim = 0

    for i = 1, self.EffectListT:Length() do 
        self.EffectListT[i]:SetData({mono = self,end_func = function ()
            if 1 == i then self:EffectShow() end
            self.EffectListT[i]:AllHandleCancel()
        end})
    end
    self.item_red_points = nil
end

function PartnerViewStrengthen:OnEnable()
    self.data.cur_sel_tabbar.val = PartnerConfig.PartnerViewTabs.Strengthen
    self:onFlush()
end

function PartnerViewStrengthen:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_dft)
end

function PartnerViewStrengthen:onFlush()
    if self.data.partner_strengthen_up_effect.partner > 0 then return end
    if -1 == self.data.cur_sel_partner then return end
    
    self.BtnUpInter.Interactable = true

    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local info = data.info
    local vo = data.vo
    local intensify_level = vo.intensify_level
    self.NameShow.text = string.format(self.language.NameShow, vo.level, info.name)

    self.RankItem:SetRank(vo.rank)
    self.RankItem:SetActive(PartnerData.PartnerRankShow(vo.rank))

    local show_list = self.data:PartnerIntensifyAddShowList(self.data.cur_sel_partner)
    self.ShowList:SetData(show_list)

    TimeHelper:CancelTimer(self.timer_dft)
    self.timer_dft = TimeHelper:AddDelayFrameTimer(function ()
        self.ShowList:ForceJumpVertical(191 * (self.ShowList:GetByData(show_list[math.max(2, vo.intensify_level) - 1]) - 1))
    end)
    
    self:FlushModelShow(data)

    local is_max = PartnerConfig.PARTNER_STRENGTHEN_LEVEL_MAX == intensify_level
    self.DownObj:SetActive(not is_max)
    self.MaxTipObj:SetActive(is_max)
    if not is_max then
        local co = Cfg.ParnterIntensifyByIdLevel(self.data.cur_sel_partner, intensify_level)
        local str_sp1 = RichTextHelper.SpriteStrAtlas(PartnerConfig.StrLevelSp, 48, 32, nil, "partner")
        local str_sp2 = RichTextHelper.SpriteStrAtlas(PartnerConfig.StrLevelSp, 69, 46, nil, "partner")
        self.DownDescShow.text = string.format(self.language.DownDescShow, info.name, string.format(str_sp1, intensify_level), string.format(str_sp2, intensify_level + 1))
        self.BtnUpNum.text = co.coin
        if nil == co.consumes_show then PartnerData:PartnerStrConsumesParse(co) end
        self.data:ResetPartnerStrengthenSelItems()
        for i = 1, self.ConsumeItems:Length() do
            self.ConsumeItems[i]:SetData(co.consumes_items[i], i)
        end
    end
    
    self.item_red_points = self.item_red_points or {}
    for i=1,self.ConsumeItems:Length() do
        if data:StrengthenRedPoint() then
            if self.item_red_points[i] == nil then
                self.item_red_points[i] = UiRedPoint.New(self.ConsumeItems[i].gameObject, Vector3.New(34,34,0))
            end
            self.item_red_points[i]:SetNum(1)
        elseif self.item_red_points[i] ~= nil then
            self.item_red_points[i]:SetNum(0)
        end
    end
    -- self.red_point_strength = self.red_point_strength or UiRedPoint.New(self.BtnUpInter.gameObject, Vector3.New(95,20,0))
    -- self.red_point_strength:SetNum(data:StrengthenRedPoint() and 1 or 0)
end

function PartnerViewStrengthen:FlushModelShow(data)
    local skin_id = data.vo.skin_id
    local res_path = ""
    if skin_id > 0 and self.data.partner_skin_ids[skin_id] then
        local res_id = self.data.partner_skin_ids[skin_id].res_id
        res_path = string.format("actor/npc/%s.prefab", res_id)
    else
        local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
        res_path = string.format("actor/npc/%s.prefab", data.info.res_id)
    end
    if not self.ui_obj then
        self.ui_obj = UIObjDrawer.New()
        self.ui_obj:SetData(res_path, data.vo.soar_count)
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                load_callback = function ()
                    self.ui_obj:PlayAnim(ChAnimType.Show)
                end
            }
        )
    else
        self.ui_obj:SetData(res_path, data.vo.soar_count)
    end
    if self.partner_id_anim ~= self.data.cur_sel_partner then
        self.partner_id_anim = self.data.cur_sel_partner
        self.ui_obj:PlayAnim(ChAnimType.Show)
    end
end

function PartnerViewStrengthen:FlushEffect()
    if 0 == self.data.partner_strengthen_up_effect.partner then return end
    self.data:SetPartnerStrengthenUpEffect(0)
    for i = 1, self.EffectListT:Length() do 
       if self.EffectListT[i].ParentItem.activeSelf then
            self.EffectListT[i]:AllHandleCancel()
            self.EffectListT[i]:Play(3165007)
       end
    end 
end

function PartnerViewStrengthen:OnClickTips()
    ViewMgr:OpenView(PartnerStrengthenPreviewView)
end

function PartnerViewStrengthen:OnClickUp()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
    if PartnerCtrl.Instance:StrengthenLockTips() then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local info = data.info
    local vo = data.vo
    local co = Cfg.ParnterIntensifyByIdLevel(self.data.cur_sel_partner, vo.intensify_level)
    local items = self.data.strength_items.items
    if MallCtrl.IsNotCurrency(CurrencyType.CoinBind, co.coin) then
        for i = 1, #co.consumes_items do
            local consumes_item = co.consumes_items[i]
            if consumes_item then
                local need_num = consumes_item:Num()
                local have_num = self.data:GetPartnerStrengthenSelItemsNums(i)
                if have_num < need_num then
                    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = consumes_item.item_id})})
                    return
                end
            end
        end
        PartnerCtrl.Instance:SendPartnerIntensifyReq(self.data.cur_sel_partner, items, self.data:GetPartnerStrengthenSelItemsCount(2))
        self.BtnUpInter.Interactable = false
    end
end

function PartnerViewStrengthen:EffectShow()
    -- ViewMgr:OpenView(PartnerStrengthenAttrView)
    self.effect_handle = self.EffectTool:Play(3165162)
end

function PartnerViewStrengthen:OnEffectEvent(handle, time, event)
    if handle.effId == 3165162 then
        if EffectEvent.event_end == event then
            self:onFlush()
            -- ViewMgr:OpenView(PartnerStrengthenSuccView)
        end
    end
end

function PartnerViewStrengthen:OnClickAttrs()
    ViewMgr:OpenView(PartnerStrengthenAttrView)
end


PartnerViewStrengthenShowItem = DeclareMono("PartnerViewStrengthenShowItem", UIWidgetBaseItem)

function PartnerViewStrengthenShowItem:PartnerViewStrengthenShowItem()
    self.partner_data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Strengthen
end

function PartnerViewStrengthenShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local partner = self.partner_data:GetPartnerInfoById(self.partner_data.cur_sel_partner)
    local vo = partner.vo
    local is_active = vo.intensify_level >= data.intensify_level

    self.StrLevel.SpriteName = PartnerInfo.StrLevelSp(data.intensify_level)
    self.StrLevelPos.localScale = is_active and Vector3.New(1,1,1) or Vector3.New(0.7,0.7,1)
    self.TitleShow.text = is_active and self.language.AttrItemTitleShow1 or self.language.AttrItemTitleShow2
    self.StrLevelGray.enabled = not is_active
    self.AttrList:SetData(data.attrs_show)
end


PartnerViewStrengthenAttrItem = DeclareMono("PartnerViewStrengthenAttrItem", UIWidgetBaseItem)

function PartnerViewStrengthenAttrItem:PartnerViewStrengthenAttrItem()
    self.partner_data = PartnerCtrl.Instance:Data()
end

function PartnerViewStrengthenAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local partner = self.partner_data:GetPartnerInfoById(self.partner_data.cur_sel_partner)
    local vo = partner.vo
    local is_active = vo.intensify_level >= data.intensify_level

    self.AttrType.text = RichTextHelper.ColorStr(Language.Common.AttrList2[data.attr_type], is_active and COLORSTR.Green3 or COLORSTR.Gray4)
    self.AttrVal.text = RichTextHelper.ColorStr(string.format("+%s", DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value) or data.attr_value), is_active and COLORSTR.Green3 or COLORSTR.Gray4)
end


PartnerViewStrengthenConsumeItem = DeclareMono("PartnerViewStrengthenConsumeItem", UIWidgetBaseItem)

function PartnerViewStrengthenConsumeItem:PartnerViewStrengthenConsumeItem()
    self.partner_data = PartnerCtrl.Instance:Data()
end

function PartnerViewStrengthenConsumeItem:OnDestroy()
    self:uncareData()
end

function PartnerViewStrengthenConsumeItem:SetData(data, index)
    UIWidgetBaseItem.SetData(self, data)
    self.index = index

    local is_show = data:Num() > 0
    self.ItemObj:SetActive(is_show)

    self:uncareData()
    self.care_handles = {}
    if is_show then
        self:careData(self.partner_data.strength_items, BindTool.Bind(self.FlushShow, self), "flush")

        local name_str = string.split(data:Name(), "·")
        self.NameShow.text = name_str[1]
        self.EmptyObj:SetActive(true)
        self.ShowCellObj:SetActive(false)
    end
end

function PartnerViewStrengthenConsumeItem:FlushShow()
    local items = self.partner_data.strength_items.items[self.index]
    local have_num = self.partner_data:GetPartnerStrengthenSelItemsNums(self.index)
    if items and next(items) then
        self.EmptyObj:SetActive(false)
        self.ShowCellObj:SetActive(true)
        self.BtnSubObj:SetActive(true)
        self.ShowCell:SetData(Item.Init(next(items)))
    else
        self.EmptyObj:SetActive(true)
        self.ShowCellObj:SetActive(false)
        self.BtnSubObj:SetActive(false)
    end
    self.NumShow.text = string.format(Language.Common.Cstr, have_num >= self.data:Num() and QualityColorStr[ItemColor.Green] or QualityColorStr[ItemColor.Red], have_num, self.data:Num())
end

function PartnerViewStrengthenConsumeItem:OnClickItem()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    if PartnerCtrl.Instance:StrengthenLockTips() then return end
    self.partner_data:SetPartnerStrengthenSel(self.index, self.data)
    ViewMgr:OpenView(PartnerStrengthenSelView, {title_show = self.NameShow.text})

    local partner = self.partner_data:GetPartnerInfoById(self.partner_data.cur_sel_partner)
    -- if partner.strengthen_red_point_show then
        -- partner.strengthen_red_point_show = false
        -- self.partner_data:PartnerStrengthenRedPoint()
        -- ViewMgr:FlushView(PartnerView, PartnerViewStrengthen:GetTypeName())
    -- end
end

function PartnerViewStrengthenConsumeItem:OnClickSub()
    self.partner_data:SubPartnerStrengthenSelItemsByIndex(self.index)
end


PartnerStrengthenPreviewView = PartnerStrengthenPreviewView or DeclareView("PartnerStrengthenPreviewView", PartnerConfig.ResPath .. "partner_strengthen_preview")

function PartnerStrengthenPreviewView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.Partner.PartnerStrengthenPreview, Vector2.New(648,564))
end

PartnerStrengthenPreviewViewF = PartnerStrengthenPreviewViewF or DeclareMono("PartnerStrengthenPreviewViewF", UIWFlushPanel)

function PartnerStrengthenPreviewViewF:PartnerStrengthenPreviewViewF()
    self.data = PartnerCtrl.Instance:Data()
end


function PartnerStrengthenPreviewViewF:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushShow()
end

function PartnerStrengthenPreviewViewF:FlushShow()
    if -1 == self.data.cur_sel_partner then return end
    local show_list = self.data:PartnerIntensifyPreShowList(self.data.cur_sel_partner)
    self.ShowList:SetData(show_list)
end


PartnerStrengthenPreviewViewItem = DeclareMono("PartnerStrengthenPreviewViewItem", UIWidgetBaseItem)

function PartnerStrengthenPreviewViewItem:PartnerStrengthenPreviewViewItem()
end

function PartnerStrengthenPreviewViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.StrLevelFrom.SpriteName = PartnerInfo.StrLevelSp(data.intensify_level)
    self.StrLevelTo.SpriteName = PartnerInfo.StrLevelSp(data.intensify_level + 1)
    self.Consumes:SetData(data.consumes_show)
end


PartnerStrengthenSelView = PartnerStrengthenSelView or DeclareView("PartnerStrengthenSelView", PartnerConfig.ResPath .. "partner_strengthen_sel")

function PartnerStrengthenSelView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), string.format(Language.Partner.PartnerStrengthenSel, param_t.title_show), Vector2.New(1017,656))
end

PartnerStrengthenSelViewF = PartnerStrengthenSelViewF or DeclareMono("PartnerStrengthenSelViewF", UIWFlushPanel)

function PartnerStrengthenSelViewF:PartnerStrengthenSelViewF()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.StrengthenSel
    RichTextHelper.Bind(self.TipRichText, BindTool.Bind(self.OnClickTipRich, self))

    self.data_cares = {
        {data = self.data.strength_items, func = self.FlushNum, keys = {"flush"}},
	}
end

function PartnerStrengthenSelViewF:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushShow()
end

function PartnerStrengthenSelViewF:FlushShow()
    local show_list, is_empty = self.data:GetPartnerStrengthenSelShowList()
    self.ShowList:SetData(show_list)
    self.EmptyObj:SetActive(is_empty)
    local get_way_str = ">>"..RichTextHelper.LinkStr(self.language.PartnerStrengthGetWay, "_ul",COLORSTR.Yellow2).."<<"
    self.TipRichText.text = get_way_str
end

function PartnerStrengthenSelViewF:FlushNum()
    local sel_item = self.data.strength_items.sel_item
    local sel_nums = self.data:GetPartnerStrengthenSelItemsNums()
    self.SelShow.text = string.format(self.language.SelShow, sel_nums, sel_item:Num())
end

function PartnerStrengthenSelViewF:OnClickConfirm()
    ViewMgr:CloseView(PartnerStrengthenSelView)
end

function PartnerStrengthenSelViewF:OnClickTipRich()
    ViewMgr:OpenView(TreasureHuntView,{auto_select = 5})
end


PartnerStrengthenSelViewItem = DeclareMono("PartnerStrengthenSelViewItem", UIWidgetBaseItem)

function PartnerStrengthenSelViewItem:PartnerStrengthenSelViewItem()
    self.partner_data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.StrengthenSel
end

function PartnerStrengthenSelViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.NameShow.text = data:Name()
    self.HaveNum.text = string.format(self.language.HaveNum, data:Num())
    self.ShowCell:SetData(data)

    self.sel_num = self.partner_data:GetPartnerStrengthenSelItemsNumByItemId(self.data.item_id)
    self.max_num = data:Num()
    self.SelNum.text = self.sel_num
end

function PartnerStrengthenSelViewItem:FlushSelNum()
    self.SelNum.text = self.sel_num
    self.partner_data:SetPartnerStrengthenSelItems(self.data.item_id, self.sel_num)
end

function PartnerStrengthenSelViewItem:OnClickAdd()
    if self.partner_data:IsPartnerStrengthenSelItemsMaxNum() then
        PublicPopupCtrl.Instance:Center(self.language.SelNumMax)
    elseif self.max_num > self.sel_num then
		self.sel_num = self.sel_num + 1
		self:FlushSelNum()
	else
		PublicPopupCtrl.Instance:Center(self.language.MaxNum)
	end
end

function PartnerStrengthenSelViewItem:OnClickSub()
    if 0 < self.sel_num then
		self.sel_num = self.sel_num - 1
		self:FlushSelNum()
	else
		PublicPopupCtrl.Instance:Center(self.language.MinNum)
	end
end


PartnerStrengthenSuccView = PartnerStrengthenSuccView or DeclareView("PartnerStrengthenSuccView", PartnerConfig.ResPath .. "partner_strengthen_succ")

function PartnerStrengthenSuccView:LoadCallback(param_t)
end

function PartnerStrengthenSuccView:OnClickClose()
    ViewMgr:CloseView(PartnerStrengthenSuccView)
end

PartnerStrengthenSuccViewF = PartnerStrengthenSuccViewF or DeclareMono("PartnerStrengthenSuccViewF", UIWFlushPanel)

function PartnerStrengthenSuccViewF:PartnerStrengthenSuccViewF()
    self.data = PartnerCtrl.Instance:Data()
end

function PartnerStrengthenSuccViewF:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushShow()
    self:FlushModel()
end

function PartnerStrengthenSuccViewF:FlushShow()
    local data = self.data:GetPartnerInfoById(self.data.new_partner)
    local info = data.info
    local vo = data.vo
    local str_level = vo.intensify_level
    self.NameShow.text = info.name
    self.StrLevelFrom.SpriteName = PartnerInfo.StrLevelSp(math.max(1, str_level - 1))
    self.StrLevelTo.SpriteName = PartnerInfo.StrLevelSp(str_level)

    local str_co = Cfg.ParnterIntensifyByIdLevel(data.id, str_level)
    if str_co then
        local attr_list = PartnerData.Instance:PartnerStrAttrsParse(str_co)
        self.AttrList:SetData(attr_list)
    end
end

function PartnerStrengthenSuccViewF:FlushModel()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    local skin_id = data.vo.skin_id
    local res_path = ""
    if skin_id > 0 and self.data.partner_skin_ids[skin_id] then
        local res_id = self.data.partner_skin_ids[skin_id].res_id
        res_path = string.format("actor/npc/%s.prefab", res_id)
    else
        local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
        res_path = string.format("actor/npc/%s.prefab", data.info.res_id)
    end
    if not self.ui_obj then
        self.ui_obj = UIObjDrawer.New()
        self.ui_obj:SetData(res_path, data.vo.soar_count)
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                load_callback = function ()
                    self.ui_obj:PlayAnim(ChAnimType.Show)
                end
            }
        )
    else
        self.ui_obj:SetData(res_path, data.vo.soar_count)
    end
    self.ui_obj:PlayAnim(ChAnimType.Show)
end


PartnerStrengthenSuccViewAttrItem = DeclareMono("PartnerStrengthenSuccViewAttrItem", UIWidgetBaseItem)

function PartnerStrengthenSuccViewAttrItem:PartnerStrengthenSuccViewAttrItem()
    
end

function PartnerStrengthenSuccViewAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.AttrType.text = Language.Common.AttrList[data.attr_type]
    self.AttrVal.text = string.format("+%s", DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value) or data.attr_value)
    self.AttrSp.SpriteName = string.format("ShuXingDi%s", data.index % 2)
end


PartnerStrengthenAttrView = PartnerStrengthenAttrView or DeclareView("PartnerStrengthenAttrView", PartnerConfig.ResPath .. "partner_strengthen_attr")
VIEW_DECLARE_MASK(PartnerStrengthenAttrView,ViewMask.BlockClose)

function PartnerStrengthenAttrView:LoadCallback(param_t)
end

PartnerStrengthenAttrViewF = PartnerStrengthenAttrViewF or DeclareMono("PartnerStrengthenAttrViewF", UIWFlushPanel)

function PartnerStrengthenAttrViewF:PartnerStrengthenAttrViewF()
    self.data = PartnerCtrl.Instance:Data()
end

function PartnerStrengthenAttrViewF:Awake()
    UIWFlushPanel.Awake(self)
    
    self:FlushShow()
end

function PartnerStrengthenAttrViewF:FlushShow()
    if self.data.partner_strengthen_up_effect.partner > 0 then return end
    if -1 == self.data.cur_sel_partner then return end
    
    local show_list = self.data:PartnerIntensifyAllShowList(self.data.cur_sel_partner)
    self.ShowList:SetData(show_list)
    self.EmptyObj:SetActive(0 == #show_list)
    self.InfoObj:SetActive(#show_list > 0)
end

PartnerStrengthenAttrViewItem = DeclareMono("PartnerStrengthenAttrViewItem", UIWidgetBaseItem)

function PartnerStrengthenAttrViewItem:PartnerStrengthenAttrViewItem()
    self.partner_data = PartnerCtrl.Instance:Data()
end

function PartnerStrengthenAttrViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.AttrType.text = Language.Common.AttrList2[data.attr_type]
    self.AttrVal.text = string.format("+%s", DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value) or data.attr_value)
end