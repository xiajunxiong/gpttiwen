AccRechargeView = AccRechargeView or DeclareView("AccRechargeView", "serveractivity/acc_recharge_view", Mod.RandActivity.AccRecharge)

VIEW_DECLARE_MASK(AccRechargeView, ViewMask.BgBlock)
VIEW_DECLARE_LEVEL(AccRechargeView, ViewLevel.L1)


----------------------------AccRechargePanel----------------------------
AccRechargePanel = AccRechargePanel or DeclareMono("AccRechargePanel", UIWFlushPanel)
function AccRechargePanel:AccRechargePanel()
    self.config = ServerActivityData.Instance:GetAccRechargeConfig()
    self.other = Config.acc_recharge_auto.other[1]
    self.acc_recharge_data = ServerActivityData.Instance.acc_recharge_data
    self.weapon_show = Config.acc_recharge_auto.weapon_show
    -- 是否是累计充值，用于区分该界面是用累计充值形式还是直购
    self.is_acc = ServerActivityData.Instance:GetAccRechargeIsAcc()
    self.profession = RoleData.Instance:GetProfession()
    self.data_cares = {
        {data = self.acc_recharge_data, func = self.OnFlushView, init_call = false, keys = {"info"}},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function AccRechargePanel:Awake()
    UIWFlushPanel.Awake(self)
    if not self.config then
        return
    end
    if next(self.config) == nil then
        ViewMgr.CloseView(AccRechargeView)
        return
    end
    if TableIsEmpty(self.weapon_show[self.profession]) then
        ViewMgr.CloseView(AccRechargeView)
        return
    end
    if not self.ui_obj then
        self.ui_obj = UIChDrawer.New()
        local path = DrawerHelper.GetWeaponPath(self.weapon_show[self.profession].res_id)
        self.ui_obj:SetMainSolo(path)
    end
    self.ModelShow:SetShowData({ui_obj = self.ui_obj, view = self})
    self:CreateList()
    self:OnFlushView()
    self.ObjAcc:SetActive(self.is_acc)
    self.ObjTextAcc:SetActive(self.is_acc)
    UH.SetText(self.TxtDesc, self.other.discount_desc)
    if not self.is_acc then
        UH.LocalPosG(self.ObjList, Vector3.New(210, -80, 0))
        UH.LocalPosG(self.ObjBtnRecharge, Vector3.New(214, -230, 0))
    end
    self:FlushToggle()
end

function AccRechargePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
end

function AccRechargePanel:OnGiftClick()
    if self.acc_recharge_data.info.is_received_free < 1 then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ACC_RECHARGE, 3)
    else
        local tab = {
            pos = UH.GetRootPosition(self.ObjShowPos),
            item_list = DataHelper.FormatItemList(self.other.free_reward_item),
            pivot = 0
        }
        ViewMgr:OpenView(ShowItemView, tab)
    end
end

-- 根据奖励数量创建道具列表
function AccRechargePanel:CreateList()
    -- 第一个固定是职业武器
    local pro_tab = TableCopy(self.config.reward_item[0])
    pro_tab.item_id = Cfg.RewardJobBagByIdType(pro_tab.item_id, self.profession).new_id
    self.item_id = pro_tab.item_id
    -- pro_tab.param = {}
    self:CreateAttribute(pro_tab)

    if self.config.reward_num <= 3 then
        local tab = {}
        for k, v in pairs(self.config.reward_item) do
            if k == 0 then
                table.insert(tab, pro_tab)
            else
                table.insert(tab, v)
            end
        end
        self.ItemList:SetData(DataHelper.FormatItemList(tab))
    else
        local tab1 = {}
        local tab2 = {}
        for k, v in pairs(self.config.reward_item) do
            if k < 2 then
                if k == 0 then
                    table.insert(tab1, pro_tab)
                else
                    table.insert(tab1, v)
                end
            else
                table.insert(tab2, v)
            end
        end
        self.ItemList1:SetData(DataHelper.FormatItemList(tab1, 1))
        self.ItemList2:SetData(DataHelper.FormatItemList(tab2, 1))
    end
    self.ListBg1:SetActive(self.config.reward_num <= 3)
    self.ListBg2:SetActive(self.config.reward_num > 3)
end

-- 根据武器id创建属性
function AccRechargePanel:CreateAttribute(pro_tab)
    local equip_data = Equip.New(pro_tab)
    equip_data.param = equip_data.param or {}
    equip_data.param.attr_list = Equip.GetSingleAttrSection(pro_tab.item_id, true)
    local attr_section = equip_data:AttrSection()
    for i = 1, self.Attributes:Length() do
        if attr_section[i] then
            if attr_section[i].max > 0 then
                local attr = DataHelper.Percent(attr_section[i].attr_type, attr_section[i].max)
                UH.SetText(self.Attributes[i], Language.Common.AttrList[attr_section[i].attr_type] .. "  " .. attr)
            else
                UH.SetText(self.Attributes[i], "")
            end
        else
            UH.SetText(self.Attributes[i], "")
        end
    end
end

function AccRechargePanel:OnFlushView()
    UH.SetText(self.NowRechargeTxt, self.acc_recharge_data.info.now_recharge)
    self.GiftRemind:SetNum(self.acc_recharge_data.info.is_received_free < 1 and 1 or 0)
    self.ObjGeted:SetActive(self.acc_recharge_data.info.is_received_free > 0)
    self:FlushBtn()
    self:JudgeIsReceive()
end

-- 判断是否已经领取过了，领取过需要关闭界面
function AccRechargePanel:JudgeIsReceive()
    if self.acc_recharge_data.info.is_get_reward == 1 then
        ViewMgr:CloseView(AccRechargeView)
    end
end

-- 更新充值按钮
function AccRechargePanel:FlushBtn()
    if self.is_acc then
        local remind_num = ServerActivityData.Instance:GetAccRechargeRemind()
        self.RedPoint:SetNum(remind_num)
        UH.SetText(self.BtnRechargeTxt, remind_num == 0 and Language.AccRecharge.CanNotReceive or Language.AccRecharge.CanReceive)
    else
        UH.SetText(self.TxtNotAcc, self.config.buy_money)
    end
end

function AccRechargePanel:OnCloseClick()
    ViewMgr:CloseView(AccRechargeView)
end

-- 点击前往充值按钮或领取奖励按钮
function AccRechargePanel:OnClickRecharge()
    if self.is_acc then
        if self.acc_recharge_data.info.now_recharge < self.config.acc_price then
            ViewMgr:OpenViewByKey(Mod.Store.Recharge)
        else
            ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ACC_RECHARGE, 1)
        end
    else
        local fee = self.config.buy_money
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_ACCRECHARGE, fee, ACTIVITY_TYPE.RAND_ACC_RECHARGE, 2, 0, 0, 0)
    end
end

function AccRechargePanel:SendBuy()
    if self.config == nil then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_ACCRECHARGE, self.config.buy_money * 10)
end

function AccRechargePanel:SetLookActive(value)
    if value then
        if not self.is_not_set then
            self.is_not_set = true
            self:SetDescText()
        else
            self:JumpFunc()
        end
    end
    self.ObjAttrAllPanel:SetActive(value)
end

function AccRechargePanel:SetDescText()
    local attr_tab = DivineCostumeData.Instance:GetAllAttrById(self.item_id)
    local item_list_data = {}
    local jump_temp = nil
    local god_temp = nil
    local total = #attr_tab[1]
    local select_level = 0
    -- 只显示星耀
    for i, v in ipairs(attr_tab[1]) do
        local t = {}
        t.index = v.level
        t.str_grade = Format(Language.AccRecharge.AttrLv[1], v.level)
        t.str_level = Format(Language.AccRecharge.Level, v.need_rolelevel)
        t.str_attr = ""
        for y, w in ipairs(v.attr) do
            if y > 1 then
                t.str_attr = t.str_attr .. "  "
            end
            t.str_attr = t.str_attr .. Format(Language.AccRecharge.Attr, w.attr_name, w.attr_percent_value)
        end
        if jump_temp and not self.jump_data then
            if RoleLevel() < v.need_rolelevel then
                self.jump_data = jump_temp
                self.god_data = god_temp
            elseif i == total then
                self.jump_data = t
                self.god_data = v
            end
        end
        -- 要选出当前限制等级中最低的属性
        if select_level ~= v.need_rolelevel then
            jump_temp = t
            god_temp = v
            select_level = v.need_rolelevel
        end
        table.insert(item_list_data, t)
    end
    self.TxtItemList:SetCompleteCallBack(BindTool.Bind(self.JumpFunc, self))
    self.TxtItemList.StepCount = #item_list_data
    self.TxtItemList:SetData(item_list_data)

    local weapon_item = BagData.Instance:GetEquipList(2)
    if weapon_item then
        local limit_prof = Equip.GetLimitProf(self.item_id)
        local flag_invalid = limit_prof ~= -1 and limit_prof ~= RoleData.Instance:GetRoleProfType()--true就是未生效
        self.ObjNotAttr:SetActive(flag_invalid)
        if not flag_invalid and weapon_item.BaseAttr then
            local now_txt_data = {}
            for i, v in ipairs(weapon_item:BaseAttr()) do
                if v.attr_value > 0 then
                    local attr = CommonStruct.AttrStruct(v.attr_type, v.attr_value)
                    table.insert(now_txt_data, {txt = Format(Language.AccRecharge.Attr2, attr.attr_name, attr.attr_percent_value)})
                end
            end
            self.NowAttrList:SetData(now_txt_data)
        end
    else
        self.ObjNotAttr:SetActive(true)
    end
    if self.jump_data and self.god_data then
        self.ObjNotAttr2:SetActive(false)
        local god_txt_data = {}
        for i, v in ipairs(self.god_data.attr) do
            table.insert(god_txt_data, {txt = Format(Language.AccRecharge.Attr2, v.attr_name, v.attr_percent_value)})
        end
        self.GodAttrList:SetData(god_txt_data)
    else
        self.ObjNotAttr2:SetActive(true)
    end
end

function AccRechargePanel:JumpFunc()
    if not TableIsEmpty(self.jump_data) then
        self.ScrollRect:StopMovement()
        self.TxtItemList:AutoSlide(self.jump_data, false)
    end
end

function AccRechargePanel:FlushToggle()
    self.ObjToggle:SetActive(RoleData.Instance:GetRoleLevel() >= self.other.close_tips)
    self.ToggleClick.isOn = ActivityRandData.GetRemind(ACTIVITY_TYPE.RAND_ACC_RECHARGE) == 1
end

function AccRechargePanel:OnToggleClick(value)
    self.ToggleClick.isOn = value
    ServerActivityData.Instance:SetAccRechargeNeedOpenValue(value and 1 or 0)
end

--------------AccRechargeTxtItem--------------
AccRechargeTxtItem = AccRechargeTxtItem or DeclareMono("AccRechargeTxtItem", UIWidgetBaseItem)
function AccRechargeTxtItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ObjBg:SetActive(data.index % 2 == 1)
    UH.SetText(self.TxtGrade, data.str_grade)
    UH.SetText(self.TxtLevel, data.str_level)
    UH.SetText(self.TxtAttr, data.str_attr)
end

--------------AccRechargeTxtItemTwo--------------
AccRechargeTxtItemTwo = AccRechargeTxtItemTwo or DeclareMono("AccRechargeTxtItemTwo", UIWidgetBaseItem)
function AccRechargeTxtItemTwo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Txt, data.txt)
end