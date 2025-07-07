--------------MarbasBatheSelectView--------------
MarbasBatheSelectView = MarbasBatheSelectView or DeclareView("MarbasBatheSelectView", "marbas/marbas_bathe_select")
VIEW_DECLARE_MASK(MarbasBatheSelectView, ViewMask.Block)
function MarbasBatheSelectView:MarbasBatheSelectView()

end

function MarbasBatheSelectView:OnCloseClick()
    ViewMgr:CloseView(MarbasBatheSelectView)
end

function MarbasBatheSelectView:OnHelpClick()
    local num = MarbasData.Instance.other.affix_desc
    if num then
        PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[num].desc)
    end
end


--------------MarbasBatheSelectPanel--------------
MarbasBatheSelectPanel = MarbasBatheSelectPanel or DeclareMono("MarbasBatheSelectPanel", UIWFlushPanel)
function MarbasBatheSelectPanel:MarbasBatheSelectPanel()
    self.Data = MarbasData.Instance
    self.language = Language.Marbas.Bathe
    self.data = self.Data.bathe_info
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.Data:BatheRemindItemList(), BindTool.Bind(self.FlushDown, self))
    end
end

function MarbasBatheSelectPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.Data:BatheShowEffect(-1)
    self.Data:BatheNeedShowNowEffect(false)
    BagData.Instance:RemoveListenItems(self.item_listen)
end

function MarbasBatheSelectPanel:Awake()
    UIWFlushPanel.Awake(self)
    local now_part = self.Data.left_equip_info.part
    local now_equip = self.Data.equip_list[now_part] or {}
    if TableIsEmpty(now_equip) then
        ViewMgr:CloseView(MarbasBatheSelectView)
        return
    end
    self:FlushAll()
end

function MarbasBatheSelectPanel:FlushAll()
    self.now_part = self.Data.left_equip_info.part
    self.now_equip = self.Data.equip_list[self.now_part] or {}
    self.now_bathe_info = self.data.info[self.now_part]
    self.now_slot_info = self.now_bathe_info.slot_info[self.data.now_select]
    self.slot_select_list = self.Data:BatheSlotSelectList(self.now_part, self.now_slot_info.index)
    self.slot_select_list_num = #self.slot_select_list
    self.Data:BatheNowSlotSelectListNum(self.slot_select_list_num)
    if TableIsEmpty(self.now_equip) or TableIsEmpty(self.now_bathe_info) or TableIsEmpty(self.now_slot_info) then
        return
    end
    self:FlushTop()
    self:FlushCenter()
    self:FlushDown()
end

function MarbasBatheSelectPanel:FlushTop()
    UH.SetText(self.TxtNowMax, Format(self.language.Level, self.Data:BatheAffixMaxLevel(self.now_equip.item_id)))
    UH.SetText(self.TxtHistoryMax, Format(self.language.Level, self.now_slot_info.history_level))
    local color = self.slot_select_list_num < 10 and COLORSTR.Green7 or COLORSTR.Red10
    UH.SetText(self.TxtTopMax, Format(self.language.TopMax, ColorStr(self.slot_select_list_num, color)))
end

function MarbasBatheSelectPanel:FlushCenter()
    if self.Data:BatheNeedShowNowEffect() then
        self:PlayNowAttrEffect()
        self.Data:BatheNeedShowNowEffect(false)
    end
    if self.now_slot_info.now_attr.level == 0 then
        PublicPopupCtrl.Instance:Center(self.language.ErrorTip[1])
        return
    end
    self.NowItem:SetData(self.now_slot_info.now_attr)
    self.List:SetCompleteCallBack(function()
        if not TableIsEmpty(self.slot_select_list) then
            if #self.slot_select_list > 5 then
                self.ScrollRect:StopMovement()
                self.List:AutoSlide(self.slot_select_list[#self.slot_select_list], true)
            end
        end
    end)
    self.List.StepCount = #self.slot_select_list
    self.List:SetData(self.slot_select_list)
end

function MarbasBatheSelectPanel:OnClearAllClick()
    if self.slot_select_list_num > 0 then
        -- 判断是否存在比当前更高级
        local is_has_better = false
        for k, v in pairs(self.slot_select_list) do
            if v.level > self.now_slot_info.now_attr.level then
                is_has_better = true
                break
            end
        end
        -- 普通提示弹窗和存在比当前更高级弹窗
        local dialog_info = {
            content = self.language.ClearAllTip[is_has_better and 3 or 1],
            confirm = {
                func = function()
                    MarbasCtrl.Instance:SendEquipOperPing(MarbasReqTypeP.BatheRemoveAll, self.now_part, self.now_slot_info.index)
                    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaShiBai)
                    ViewMgr:CloseView(DialogTipsView)
                end
            },
            new_tnr = is_has_better and DTNewTodayNotRemind.MarbasBatheCastingClearAll2 or DTNewTodayNotRemind.MarbasBatheCastingClearAll,
        }
        PublicPopupCtrl.Instance:DialogTips(dialog_info)
    else
        PublicPopupCtrl.Instance:Center(self.language.ClearAllTip[2])
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    end
end

function MarbasBatheSelectPanel:FlushDown()
    local item_id = self.Data:BatheRemindItemList()[1]
    local level = self.now_slot_info.now_attr.level or 0

    self.IHCasting.Interactable = self.slot_select_list_num < 10
    self.IHClearAll.Interactable = self.slot_select_list_num > 0

    UH.SetIcon(self.Icon1, Item.GetIconId(item_id), ICON_TYPE.ITEM)
    UH.SetIcon(self.Icon2, Item.GetIconId(item_id), ICON_TYPE.ITEM)
    local has = Item.GetNum(item_id)
    local qua_cfg = Cfg.MarbasBatheAffixQua(level)
    local history_cfg = Cfg.MarbasBatheAffixQua(self.now_slot_info.history_level)
    local need = history_cfg.expends_num
    local ten_need = history_cfg.ten_expends_num
    local color = is_enough and COLORSTR.Green9 or COLORSTR.Red10
    UH.SetText(self.TxtNum1, Format(Language.Common.StuffNumColorFirst, has >= need and COLORSTR.Green4 or COLORSTR.Red8, has, need))
    UH.SetText(self.TxtNum2, Format(Language.Common.StuffNumColorFirst, has >= ten_need and COLORSTR.Green4 or COLORSTR.Red8, has, ten_need))

    self.ObjDeleteAllEffect:SetActive(self.slot_select_list_num == 10)

    local need = history_cfg.mini_get_times
    self.Progress:SetProgress(self.now_slot_info.now_times / need)
    self.Progress:SetText(Format(Language.Common.Cstr, COLORSTR.Yellow16, self.now_slot_info.now_times > need and need or self.now_slot_info.now_times, need))
end

function MarbasBatheSelectPanel:OnCastingClick()
    if self:CastingJudge(1) then
        self:CastingDialogTip(1, function()
            MarbasCtrl.Instance:SendEquipOperPing(MarbasReqTypeP.BatheDraw, self.now_part, self.now_slot_info.index, 1)
            self.Data:BatheShowEffect(1)
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.OpenGift)
            ViewMgr:CloseView(DialogTipsView)
        end)
    end
end

function MarbasBatheSelectPanel:OnCastingTenClick()
    if self:CastingJudge(10) then
        self:CastingDialogTip(10, function()
            MarbasCtrl.Instance:SendEquipOperPing(MarbasReqTypeP.BatheDraw, self.now_part, self.now_slot_info.index, 10)
            self.Data:BatheShowEffect(0)
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.OpenGift)
            ViewMgr:CloseView(DialogTipsView)
        end)
    end
end

-- 铸造通用判断
function MarbasBatheSelectPanel:CastingJudge(times)
    if TableIsEmpty(self.now_equip) then
        -- 未穿戴装备判断
        PublicPopupCtrl.Instance:Center(self.language.CastingTip[2])
        return
    end
    if self.now_slot_info.is_unlock == 0 then
        -- 未解锁判断
        PublicPopupCtrl.Instance:Center(self.language.CastingTip[1])
        return
    end
    if times == 1 and not self.IHCasting.Interactable then
        -- 单抽并且存储词缀已满
        PublicPopupCtrl.Instance:Center(self.language.CastingTip[3])
        return
    end
    local history_cfg = Cfg.MarbasBatheAffixQua(self.now_slot_info.history_level)
    local item_id = self.Data:BatheRemindItemList()[1]
    local has = Item.GetNum(item_id)
    local need = history_cfg.expends_num
    if times == 10 then
        need = history_cfg.ten_expends_num
    end
    if need > has then
        -- 材料判断
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = item_id, jump_need_num = need - has})})
        return
    end
    return true
end

-- 铸造通用弹窗判断
function MarbasBatheSelectPanel:CastingDialogTip(times, func)
    -- 这里要拿历史最高来判断
    local now_level = self.now_slot_info.history_level
    -- local now_level = self.now_slot_info.now_attr.level
    local now_max_level = self.Data:BatheAffixMaxLevel(self.now_equip.item_id)
    local dialog_info = {}
    if times == 1 then
        if now_level >= self.Data:BatheAffixLevelMax() then
            -- 是否达到当前珑铸词缀等级最高上限
            dialog_info = {
                content = self.language.CastingTip[4],
                confirm = {
                    func = func
                },
                new_tnr = DTNewTodayNotRemind.MarbasBatheCasting1,
            }
        elseif now_level >= now_max_level then
            -- 是否达到当前珑铸词缀等级上限
            dialog_info = {
                content = self.language.CastingTip[5],
                confirm = {
                    func = func
                },
                new_tnr = DTNewTodayNotRemind.MarbasBatheCasting2,
            }
        else
            -- 直接抽
            func()
        end
    elseif times == 10 then
        if now_level >= self.Data:BatheAffixLevelMax() then
            -- 是否达到当前珑铸词缀等级最高上限
            dialog_info = {
                content = self.language.CastingTip[8],
                confirm = {
                    func = func
                },
                new_tnr = DTNewTodayNotRemind.MarbasBatheCastingTen3,
            }
        elseif now_level >= now_max_level then
            -- 是否达到当前珑铸词缀等级上限
            dialog_info = {
                content = self.language.CastingTip[7],
                confirm = {
                    func = func
                },
                bg_v2 = Vector2.New(446,280),
                today_v2 = Vector2.New(-70,-16.3),
                btn_v2 = Vector2.New(0,-81.3),
                content_v2 = Vector2.New(0, 43),
                new_tnr = DTNewTodayNotRemind.MarbasBatheCastingTen2,
            }
        else
            dialog_info = {
                content = self.language.CastingTip[6],
                confirm = {
                    func = func
                },
                new_tnr = DTNewTodayNotRemind.MarbasBatheCastingTen1,
            }
        end
    end
    if not TableIsEmpty(dialog_info) then
        PublicPopupCtrl.Instance:DialogTips(dialog_info)
    end
end

function MarbasBatheSelectPanel:ShowItemGetWay()
    local item_id = self.Data:BatheRemindItemList()[1]
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = item_id})})
end

function MarbasBatheSelectPanel:PlayNowAttrEffect()
    self.ObjNowShowEffect:SetActive(false)
    self.ObjNowShowEffect:SetActive(true)
end

--------------MarbasBatheSelectItem--------------
MarbasBatheSelectItem = MarbasBatheSelectItem or DeclareMono("MarbasBatheSelectItem", UIWidgetBaseItem)
function MarbasBatheSelectItem:MarbasBatheSelectItem()
    self.Data = MarbasData.Instance
    self.language = Language.Marbas.Bathe
end

function MarbasBatheSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.now_part = self.Data.left_equip_info.part
    self.now_equip = self.Data.equip_list[self.now_part] or {}
    self.now_slot = self.Data.bathe_info.now_select
    self.now_bathe_info = self.Data.bathe_info.info[self.now_part]
    self.now_slot_info = self.now_bathe_info.slot_info[self.now_slot]
    self.now_attr = self.now_slot_info.now_attr

    if not TableIsEmpty(self.now_equip) then
        local affix_id = data.affix_id
        local level = data.level
        local name, attr, icon = self.Data:BatheAffixAttr(affix_id, level)
        if TableIsEmpty(attr) or level == 0 then
            PublicPopupCtrl.Instance:Center(self.language.ErrorTip[2])
            return
        end
        local qua = Cfg.MarbasBatheAffixQua(level).quality
        local color = QualityColorStr[qua]

        if data.index == 0 then
            self:ShowBg(1)
        elseif (level - self.now_attr.level) == 1 then
            self:ShowBg(3)
        elseif (level - self.now_attr.level) > 1 then
            self:ShowBg(4)
        else
            self:ShowBg(2)
        end
        self.ObjClear:SetActive(data.index ~= 0)
        UH.SetText(self.TxtAttrDesc, Format(self.language.Level2, level, ColorStr(name, color)))
        UH.SpriteName(self.Icon, icon)
        UH.SpriteName(self.QuaIcon, "qua_"..qua)
        UH.SetText(self.TxtAttr, Format(self.language.Attr, color, attr.attr_name, attr.attr_percent_value))
        UH.SetText(self.TxtScore, Format(self.language.Score, self.Data:CalculateScore({attr})))
        self.Remind:SetNum(level > self.now_attr.level and 1 or 0)
    end

    if self.ObjShowEffect then
        TimeHelper:CancelTimer(self.ht_delay)
        self.ObjShowEffect:SetActive(false)
        self.ObjShowHightEffect:SetActive(false)
        if self.Data:BatheShowEffect() == 0 then
            self:PlayEffect()
        elseif self.Data:BatheShowEffect() == 1 then
            if data.index == self.Data:BatheNowSlotSelectListNum() then
                self:PlayEffect()
            end
        end
        if self.ObjShowEffect.activeSelf or self.ObjShowHightEffect.activeSelf then
            self.ObjNode:SetActive(false)
            self.ht_delay = Invoke(function()
                self.ObjNode:SetActive(true)
            end, 0.5)
        else
            self.ObjNode:SetActive(true)
        end
    end
end

-- 判断需要放哪个特效
function MarbasBatheSelectItem:PlayEffect()
    if self.ObjShowEffect then
        if (self.data.level - self.now_attr.level) >= 1 then
            self.ObjShowHightEffect:SetActive(true)
        else
            self.ObjShowEffect:SetActive(true)
        end
    end
end

function MarbasBatheSelectItem:ShowBg(index)
    for i = 1, self.ObjBgs:Length() do
        self.ObjBgs[i]:SetActive(i == index)
    end
    if self.ObjHightEffect then
        self.ObjHightEffect:SetActive(index == 3)
    end
    if self.ObjBestEffect then
        self.ObjBestEffect:SetActive(index == 4)
    end
end

function MarbasBatheSelectItem:OnSelectClick()
    ViewMgr:OpenView(MarbasBatheChangeView, {now = self.now_attr, change = self.data})
end

function MarbasBatheSelectItem:OnClearClick()
    if self.data.index == 0 then
        return
    end
    local name, attr = self.Data:BatheAffixAttr(self.data.affix_id, self.data.level)
    local qua = Cfg.MarbasBatheAffixQua(self.data.level).quality
    local color = QualityColorStr[qua]
    local dialog_info = {
        content = Format(self.language.ClearOneTip, self.data.level, ColorStr(name, color)),
        confirm = {
            func = function()
                MarbasCtrl.Instance:SendEquipOperPing(MarbasReqTypeP.BatheRemove, self.now_part, self.now_slot_info.index, self.data.index - 1)
                self.Data:BatheShowEffect(-1)
                AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaShiBai)
                ViewMgr:CloseView(DialogTipsView)
            end
        },
        new_tnr = DTNewTodayNotRemind.MarbasBatheCastingClear,
    }
    PublicPopupCtrl.Instance:DialogTips(dialog_info)
end

function MarbasBatheSelectItem:OnDestroy()
    TimeHelper:CancelTimer(self.ht_delay)
end

--------------MarbasBatheChangeView--------------
MarbasBatheChangeView = MarbasBatheChangeView or DeclareView("MarbasBatheChangeView", "marbas/marbas_bathe_change")
VIEW_DECLARE_MASK(MarbasBatheChangeView, ViewMask.BgBlock)
function MarbasBatheChangeView:MarbasBatheChangeView()
    self.Data = MarbasData.Instance
    self.language = Language.Marbas.Bathe
    self.Board:SetData(self:GetType(), self.language.ChangeTitle, Vector2.New(540, 467))
end

function MarbasBatheChangeView:LoadCallback(param_t)
    self.now_part = self.Data.left_equip_info.part
    self.now_equip = self.Data.equip_list[self.now_part] or {}
    self.now_slot = self.Data.bathe_info.now_select
    self.now_bathe_info = self.Data.bathe_info.info[self.now_part]
    self.now_slot_info = self.now_bathe_info.slot_info[self.now_slot]
    self.now = param_t.now
    self.change = param_t.change
    if TableIsEmpty(self.now) or TableIsEmpty(self.change) then
        ViewMgr:CloseView(MarbasBatheChangeView)
        return
    end
    self.NowItem:SetData(self.now)
    self.ChangeItem:SetData(self.change)
end

function MarbasBatheChangeView:OnChangeClick()
    self.Data:BatheShowEffect(-1)
    self.Data:BatheNeedShowNowEffect(true)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ChuShiDuanWeiXingXing)
    MarbasCtrl.Instance:SendEquipOperPing(MarbasReqTypeP.BatheChange, self.now_part, self.now_slot_info.index, self.change.index - 1)
    self:OnCloseClick()
end

function MarbasBatheChangeView:OnCloseClick()
    ViewMgr:CloseView(MarbasBatheChangeView)
end

--------------MarbasBatheChangeItem--------------
MarbasBatheChangeItem = MarbasBatheChangeItem or DeclareMono("MarbasBatheChangeItem", UIWidgetBaseItem)
function MarbasBatheChangeItem:MarbasBatheChangeItem()
    self.Data = MarbasData.Instance
    self.language = Language.Marbas.Bathe
end

function MarbasBatheChangeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.now_part = self.Data.left_equip_info.part
    self.now_equip = self.Data.equip_list[self.now_part] or {}
    self.now_slot = MarbasData.Instance.bathe_info.now_select
    self.now_bathe_info = MarbasData.Instance.bathe_info.info[self.now_part]
    self.now_slot_info = self.now_bathe_info.slot_info[self.now_slot]
    self.now_attr = self.now_slot_info.now_attr

    if not TableIsEmpty(self.now_equip) then
        local affix_id = data.affix_id
        local level = data.level
        local name, attr, icon = self.Data:BatheAffixAttr(affix_id, level)
        local qua = Cfg.MarbasBatheAffixQua(level).quality
        local color = QualityColorStr[qua]

        UH.SetText(self.TxtAttrDesc, Format(self.language.Level2, level, ColorStr(name, color)))
        UH.SpriteName(self.Icon, icon)
        UH.SpriteName(self.QuaIcon, "qua_"..qua)
        UH.SetText(self.TxtAttr, Format(self.language.Attr, color, attr.attr_name, attr.attr_percent_value))
        UH.SetText(self.TxtScore, Format(self.language.Score, self.Data:CalculateScore({attr})))
    end
end