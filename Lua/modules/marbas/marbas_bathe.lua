MarbasBathe = MarbasBathe or DeclareMono("MarbasBathe", UIWFlushPanel)
function MarbasBathe:MarbasBathe()
    self.Data = MarbasData.Instance
    self.language = Language.Marbas.Bathe
    self.data = self.Data.bathe_info
    self.data.now_select = 0
    self:EndDelay2()
    self.data_cares = {
        {data = self.data, func = self.OpenSelectView, keys = {"info"}, init_call = false},
        {data = self.data, func = self.FlushAll, keys = {"now_select"}, init_call = false},
        {data = self.Data.left_equip_info, func = self.FlushNowSelectPart, keys = {"part"}, init_call = false},
    }
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.Data:BatheRemindItemList(), BindTool.Bind(self.FlushAll, self))
    end
end

function MarbasBathe:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushSelect()
end

function MarbasBathe:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.Data.left_equip_info.mod_id = Mod.Marbas.Bathe
    self:EndDelay2()
    self:FlushAll()
end

function MarbasBathe:FlushNowSelectPart()
    self:EndDelay2()
    self:FlushAll()
end

function MarbasBathe:StatrDelay2()
    TimeHelper:CancelTimer(self.ht_delay2)
    self.Data:BatheNeedDelay(true)
    self.ObjBlock:SetActive(true)
end

function MarbasBathe:EndDelay2()
    TimeHelper:CancelTimer(self.ht_delay2)
    self.Data:BatheNeedDelay(false)
    self.ObjBlock:SetActive(false)
end

-- 选中第一个
function MarbasBathe:FlushSelect()
    -- 不延时一帧会报错
    TimeHelper:CancelTimer(self.ht_delay)
    self.ht_delay = TimeHelper:AddDelayFrameTimer(function()
        -- self.now_part = self.Data.left_equip_info.part
        -- self.now_equip = self.Data.equip_list[self.now_part] or {}
        -- self.now_bathe_info = self.data.info[self.now_part]
        -- local index = 0
        -- -- 如果没装备就都不能铸造
        -- if TableIsEmpty(self.now_equip) then
        --     for k, v in pairs(self.now_bathe_info.slot_info) do
        --         -- 查找第一个已解锁可铸造的孔位
        --         if v.is_unlock == 1 and v.now_attr.level == 0 then
        --             index = v.index
        --             break
        --         end
        --     end
        -- end
        -- self.data.now_select = index
        self.List:GetItem(self.data.now_select):MySelect()
    end, 1)
end

function MarbasBathe:FlushAll()
    if self.Data:BatheNeedDelay() then
        return
    end
    self.now_part = self.Data.left_equip_info.part
    self.now_equip = self.Data.equip_list[self.now_part] or {}
    self.now_bathe_info = self.data.info[self.now_part]
    self.now_slot_info = self.now_bathe_info.slot_info[self.data.now_select]
    self.slot_select_list = self.Data:BatheSlotSelectList(self.now_part, self.now_slot_info.index)
    self.slot_select_list_num = #self.slot_select_list
    self.Data:BatheNowSlotSelectListNum(self.slot_select_list_num)
    self:FlushTop()
    self:FlushDown()
end

function MarbasBathe:OpenSelectView()
    if self.Data:BatheNeedDelay() then
        return
    end
    self:FlushAll()
    local affix_num = self.Data:BatheAffixMaxNum(self.now_equip.item_id)
    if (self.now_slot_info.index + 1) > affix_num then
        -- 抽奖后如果快速选了未生效的格子，就不打开界面
        return
    end
    -- 抽奖后的刷新如果有可选的需要打开界面
    if not TableIsEmpty(self.slot_select_list) and not ViewMgr:IsOpen(MarbasBatheSelectView) then
        ViewMgr:OpenView(MarbasBatheSelectView)
    end
end

function MarbasBathe:FlushTop()
    self.Cell:SetData(self.now_equip)
    self.Cell:SetObjActive(not TableIsEmpty(self.now_equip))
    UH.SetText(self.TxtName, not TableIsEmpty(self.now_equip) and self.now_equip:QuaName() or self.language.NoName)
    local affix_limit = not TableIsEmpty(self.now_equip) and self.now_equip:LevelShow() or 0
    UH.SetText(self.TxtLevelLimit, Format(self.language.LevelLimit, self.Data:BatheAffixLimit(affix_limit)))
    UH.SpriteName(self.Icon, MarbasConfig.equip_type_sp[self.now_part])
end

function MarbasBathe:FlushDown()
    self.List:SetData(self.now_bathe_info.slot_info)

    local item_id = self.Data:BatheRemindItemList()[1]
    local level = self.now_slot_info.now_attr.level or 0

    local is_take_effect = true
    if not TableIsEmpty(self.now_equip) then
        -- 未生效判断
        local affix_num = self.Data:BatheAffixMaxNum(self.now_equip.item_id)
        if (self.now_slot_info.index + 1) > affix_num then
            self.IHCasting.Interactable = false
            self.IHTenCasting.Interactable = false
            is_take_effect = false
        else
            self.IHCasting.Interactable = self.slot_select_list_num < 10
            self.IHTenCasting.Interactable = true
        end
    else
        self.IHCasting.Interactable = false
        self.IHTenCasting.Interactable = false
    end
    self.ObjBtnResult:SetActive(not TableIsEmpty(self.slot_select_list) and is_take_effect)
    self.ResultRemind:SetNum(self.Data:BatheSlotResultRemind(self.now_part, self.now_slot_info.index))
    self.CastingRemind:SetNum(self.Data:BatheSlotCastingRemind(self.now_part, self.now_slot_info.index))
    self.HLayoutGroup.spacing = self.ObjBtnResult.activeSelf and 70 or 130
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
end

function MarbasBathe:OnResultClick()
    if not TableIsEmpty(self.slot_select_list) and not ViewMgr:IsOpen(MarbasBatheSelectView) then
        ViewMgr:OpenView(MarbasBatheSelectView)
    end
end

function MarbasBathe:OnCastingClick()
    if self:CastingJudge(1) then
        self:CastingDialogTip(1, function()
            MarbasCtrl.Instance:SendEquipOperPing(MarbasReqTypeP.BatheDraw, self.now_part, self.now_slot_info.index, 1)
            self.Data:BatheShowEffect(1)
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.OpenGift)
            ViewMgr:CloseView(DialogTipsView)
        end)
    end
end

function MarbasBathe:OnCastingTenClick()
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
function MarbasBathe:CastingJudge(times)
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
    -- 未生效判断
    local affix_num = self.Data:BatheAffixMaxNum(self.now_equip.item_id)
    if (self.now_slot_info.index + 1) > affix_num then
        if self.now_slot_info.now_attr.affix_id > 0 then
            PublicPopupCtrl.Instance:Center(self.language.CastingTip[9])
        else
            PublicPopupCtrl.Instance:Center(self.language.CastingTip[10])
        end
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
function MarbasBathe:CastingDialogTip(times, func)
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

function MarbasBathe:OnShowClick()
    ViewMgr:OpenView(MarbasBatheAffixShowView, {level = TableIsEmpty(self.now_equip) and 0 or self.now_equip:LevelShow()})
end

function MarbasBathe:OnBookClick()
    if TableIsEmpty(self.now_equip) then
        PublicPopupCtrl.Instance:Center(self.language.BookTip)
        return
    end
    ViewMgr:OpenView(MarbasBatheBookView, {item_id = self.now_equip.item_id})
end

function MarbasBathe:ShowItemGetWay()
    local item_id = self.Data:BatheRemindItemList()[1]
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = item_id})})
end

function MarbasBathe:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    BagData.Instance:RemoveListenItems(self.item_listen)
    TimeHelper:CancelTimer(self.ht_delay)
    TimeHelper:CancelTimer(self.ht_delay2)
end

function MarbasBathe:OnUnLockClick(mono)
    local data = mono:GetData()
    if TableIsEmpty(self.now_equip) then
        PublicPopupCtrl.Instance:Center(self.language.UnLockTip)
    else
        mono:MySelect()
        MarbasCtrl.Instance:SendEquipOperPing(MarbasReqTypeP.BatheUnLock, self.now_part, data.index)
        self:StatrDelay2()
        mono:PlayUnlockEffect()
        self.ht_delay2 = Invoke(function()
            self.ObjBlock:SetActive(false)
            self.Data:BatheNeedDelay(false)
            self:FlushAll()
        end, 1)
    end
end

--------------MarbasBatheItem--------------
MarbasBatheItem = MarbasBatheItem or DeclareMono("MarbasBatheItem", UIWidgetBaseItem)
function MarbasBatheItem:MarbasBatheItem()
    self.Data = MarbasData.Instance
    self.language = Language.Marbas.Bathe
end

function MarbasBatheItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.now_part = self.Data.left_equip_info.part
    self.now_equip = self.Data.equip_list[self.now_part] or {}

    -- 解锁了该孔位，但没有属性，然后又未生效了，就需要显示
    self.ObjSpTip:SetActive(false)

    if TableIsEmpty(self.now_equip) then
        self.ObjNotActive:SetActive(true)
        self.Icon:SetObjActive(false)
        self.QuaIcon:SetObjActive(false)
        self.ObjAttrName:SetActive(false)
        
        self.ObjUnLock:SetActive(false)
        self.ObjCanCasting:SetActive(false)
        self.ObjCannotUnLock:SetActive(true)
        self.ObjHasCasting:SetActive(false)
        self.ObjInvalid:SetActive(false)
        self.Remind:SetNum(0)
    else
        self.ObjCannotUnLock:SetActive(false)
        self.ObjUnLock:SetActive(false)
        self.ObjCanCasting:SetActive(false)
        self.ObjInvalid:SetActive(false)

        local affix_num = self.Data:BatheAffixMaxNum(self.now_equip.item_id)
        -- 未生效和未解锁的时候显示
        if (data.index + 1) > affix_num or data.is_unlock == 0 then
            self.ObjNotActive:SetActive(true)
            -- 同时满足，就当前不能解锁
            if (data.index + 1) > affix_num and data.is_unlock == 0 then
                self.ObjCannotUnLock:SetActive(true)
            end
        else
            self.ObjNotActive:SetActive(false)
        end

        local affix_id = data.now_attr.affix_id
        local level = data.now_attr.level
        -- 解锁了该孔位，但没有属性，然后又未生效了，就需要显示
        if (data.index + 1) > affix_num and data.is_unlock == 1 and affix_id == 0 then
            self.ObjSpTip:SetActive(true)
        end
        -- 有装备的孔位时候显示
        if affix_id > 0 then
            local name, attr, icon = self.Data:BatheAffixAttr(affix_id, level)
            local qua = Cfg.MarbasBatheAffixQua(level).quality
            local color = QualityColorStr[qua]
            UH.SpriteName(self.Icon, icon)
            UH.SpriteName(self.QuaIcon, "qua_"..qua)
            self.Icon:SetObjActive(true)
            self.QuaIcon:SetObjActive(true)
            self.ObjAttrName:SetActive(true)
            UH.SetText(self.TxtAttrDesc, Format(self.language.Level2, level, ColorStr(name, color)))
            self.ObjHasCasting:SetActive(true)
            UH.SetText(self.TxtAttr, Format(self.language.Attr, color, attr.attr_name, attr.attr_percent_value))
            UH.SetText(self.TxtScore, Format(self.language.Score, self.Data:CalculateScore({attr})))
            -- 装备了孔位，但未生效
            if (data.index + 1) > affix_num then
                self.ObjInvalid:SetActive(true)
            end
            self.Remind:SetNum(self.Data:BatheSlotResultRemind(self.now_part, data.index))
        else
            self.Icon:SetObjActive(false)
            self.QuaIcon:SetObjActive(false)
            self.ObjAttrName:SetActive(false)
            self.ObjHasCasting:SetActive(false)
            -- 就算材料不满足也要显示可铸造
            if affix_num >= (data.index + 1) and data.is_unlock == 1 then
                self.ObjCanCasting:SetActive(true)
            end
            -- 没装备的孔位时候才有可能有可解锁或者可铸造的红点
            if self.Data:BatheSlotUnLockRemind(self.now_part, data.index) > 0 then
                self.ObjUnLock:SetActive(true)
                self.Remind:SetNum(1)
            elseif self.Data:BatheSlotCastingRemind(self.now_part, data.index) > 0 then
                self.Remind:SetNum(1)
            else
                self.Remind:SetNum(0)
            end
        end
    end
    self.ObjUnLockNormalEffect:SetActive(self.ObjUnLock.activeSelf)
    self.ObjUnLockingEffect:SetActive(false)
end

function MarbasBatheItem:OnSelect(value)
    if value then
        self.Data.bathe_info.now_select = self.data.index
    end
end

function MarbasBatheItem:MySelect()
    self.Selector:Trigger()
end

function MarbasBatheItem:PlayUnlockEffect()
    self.ObjUnLockingEffect:SetActive(true)
end

--------------MarbasBatheAffixShowView--------------
MarbasBatheAffixShowView = MarbasBatheAffixShowView or DeclareView("MarbasBatheAffixShowView", "marbas/marbas_bathe_affix_show")
VIEW_DECLARE_MASK(MarbasBatheAffixShowView, ViewMask.BgBlockClose)
function MarbasBatheAffixShowView:MarbasBatheAffixShowView()
    self.Data = MarbasData.Instance
    self.language = Language.Marbas.Bathe
end

function MarbasBatheAffixShowView:LoadCallback(param_t)
    local data = {}
    for i, v in ipairs(self.Data.config.affix_limit) do
        if i ~= 1 and v.is_show == 1 then
            local t = {}
            t.index = i - 1
            local level = ColorStr(v.equipment_level, param_t.level >= v.equipment_level and COLORSTR.Green4 or COLORSTR.Red8)
            t.desc1 = Format(self.language.AffixShowDesc[1], level)
            t.desc2 = Format(self.language.AffixShowDesc[2], v.affix_limit)
            table.insert(data, t)
        end
    end
    self.List:SetData(data)
end

--------------MarbasBatheAffixShowItem--------------
MarbasBatheAffixShowItem = MarbasBatheAffixShowItem or DeclareMono("MarbasBatheAffixShowItem", UIWidgetBaseItem)
function MarbasBatheAffixShowItem:MarbasBatheAffixShowItem()
    self.Data = MarbasData.Instance
    self.language = Language.Marbas.Bathe
end

function MarbasBatheAffixShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtDesc1, data.desc1)
    UH.SetText(self.TxtDesc2, data.desc2)
    self.ObjLine:SetActive(data.index ~= 1)
end

--------------MarbasBatheBookView--------------
MarbasBatheBookView = MarbasBatheBookView or DeclareView("MarbasBatheBookView", "marbas/marbas_bathe_book")
VIEW_DECLARE_MASK(MarbasBatheBookView, ViewMask.BgBlockClose)
function MarbasBatheBookView:MarbasBatheBookView()
    self.Data = MarbasData.Instance
    self.language = Language.Marbas.Bathe
    self.Board:SetData(self:GetType(), self.language.BookTitle, Vector2.New(748, 498))
end

function MarbasBatheBookView:LoadCallback(param_t)
    local cfg = self.Data:BatheAffixCfg(param_t.item_id)
    local data = {}
    for i, v in ipairs(cfg) do
        local t = {}
        t.index = i
        t.name = v.name
        t.type_name = Language.Common.AttrList[v.affix_attribute]
        t.desc1 = v.interval_1
        t.desc2 = v.interval_2
        t.desc3 = v.interval_3
        table.insert(data, t)
    end
    self.List:SetData(data)
end

--------------MarbasBatheBookItem--------------
MarbasBatheBookItem = MarbasBatheBookItem or DeclareMono("MarbasBatheBookItem", UIWidgetBaseItem)
function MarbasBatheBookItem:MarbasBatheBookItem()
    self.Data = MarbasData.Instance
    self.language = Language.Marbas.Bathe
end

function MarbasBatheBookItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.name)
    UH.SetText(self.TxtType, data.type_name)
    UH.SetText(self.TxtDesc1, data.desc1)
    UH.SetText(self.TxtDesc2, data.desc2)
    UH.SetText(self.TxtDesc3, data.desc3)
    self.ObjLine:SetActive(data.index ~= 1)
end