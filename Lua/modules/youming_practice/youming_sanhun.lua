YouMingSanHun = YouMingSanHun or DeclareMono("YouMingSanHun", UIWFlushPanel)
function YouMingSanHun:YouMingSanHun()
    self.data = YouMingData.Instance
    self.data_cares = {{
        data = self.data:GetSelectYouMing(),
        func = self.FlushBaseInfo
    }}
    self.select_index = 1
    self.can_play_effect = false
end
function YouMingSanHun:OnDestroy()
    BagData.Instance:RemoveListenItems(self.handle_lis)
    TimeHelper:CancelTimer(self.handle_delay1)
    TimeHelper:CancelTimer(self.handle_delay2)
    TimeHelper:CancelTimer(self.handle_delay3)
    UIWFlushPanel.OnDestroy(self)
end
function YouMingSanHun:FlushBaseInfo()
    local youming = self.data:GetSelectYouMing()
    if youming.Aptitude == nil then
        return
    end

    if self.can_play_effect == true then
        self.EffectTool:Play(7167017)
        TimeHelper:CancelTimer(self.handle_delay1)
        TimeHelper:CancelTimer(self.handle_delay2)
        self.handle_delay1 = TimeHelper:AddDelayTimer(function()
            self.EffectTool:Play(7167018)
            self.handle_delay2 = TimeHelper:AddDelayTimer(function()
                self.EffectTool:Play(7167019)
                self:FlushBaseAttrInfo(youming)
            end, 1.4)
        end, 1.42)
        self.can_play_effect = false
    else
        self:FlushBaseAttrInfo(youming)
    end
end
function YouMingSanHun:FlushBaseAttrInfo(youming)
    local list = youming:SanHunList()
    for i = 1, 3 do
        self.SanHunItems[i]:SetData(list[i])
    end
    self.SanHunItems[self.select_index]:Click()
end
function YouMingSanHun:OnEnable()
    UIWFlushPanel.OnEnable(self)
    TimeHelper:CancelTimer(self.handle_delay3)
    self.handle_delay3 = TimeHelper:AddDelayFrameTimer(function()
        self.SanHunItems[self.select_index]:Click()
    end, 2)
end
function YouMingSanHun:OnClickItem(data)
    local youming = self.data:GetSelectYouMing()
    if youming.Aptitude == nil then
        return
    end
    local is_lock = youming:SanHunIsLock(data.type)
    if is_lock then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.SoulUnlockTip)
        is_lock = youming:SanHunIsLock(self.select_index - 1)
        if is_lock then
            self.select_index = 1
        end
        self.SanHunItems[self.select_index]:Click()
        return
    end
    self.select_index = data.type + 1
    self.MaxRoot:SetActive(data.level == YouMingCfg.SanHunLevel)
    self.CostParent:SetActive(data.level ~= YouMingCfg.SanHunLevel)
    local config = Cfg.YouMingSanHunCfg(data.id, data.type, data.level, false)
    -- LogDG(config)
    if config == nil then
        config = Cfg.YouMingSanHunCfg(data.id, data.type, 1, false)
    end
    if config then
        self.item = Item.Create({
            item_id = config.levelup_consume
        })
        if self.handle_lis == nil then
            self.handle_lis = BagData.Instance:ListenItems({config.levelup_consume},
                BindTool.Bind(self.FlushCostItem, self))
        end
        local next_config = Cfg.YouMingSanHunCfg(data.id, data.type, data.level + 1, false)
        self.next_config_cache = next_config
        self.CostCell:SetData(self.item)
        UH.SetText(self.CostName, self.item:Name())
        if next_config then
            local num = Item.GetNum(config.levelup_consume)
            local need_num = next_config.levelup_consume_num
            local str = string.format(Language.Common.Cstr, num >= need_num and QualityColorStr[ItemColor.Green] or
                QualityColorStr[ItemColor.Red], num, need_num)
            UH.SetText(self.CostCount, str)
            local attr
            for i = 1, 4 do
                attr = {}
                attr.attr_type = config["attr_type_" .. i]
                attr.attr_value = data.level == 0 and 0 or config["attr_add_" .. i]
                attr.attr_value2 = next_config["attr_add_" .. i]
                attr.posX = -34.5
                self.AttrItems[i]:SetData(attr)
            end
            UH.LocalPosG(self.CurTitle, Vector3.New(269.7,45.8,0))
        else
            UH.LocalPosG(self.CurTitle, Vector3.New(295.3,45.8,0))
            local attr
            for i = 1, 4 do
                attr = {}
                attr.attr_type = config["attr_type_" .. i]
                attr.attr_value = data.level == 0 and 0 or config["attr_add_" .. i]
                attr.attr_value2 = 0--next_config["attr_add_" .. i]
                attr.hide_right = true
                attr.posX = -10
                self.AttrItems[i]:SetData(attr)
            end
        end
    end
end
function YouMingSanHun:FlushCostItem()
    if self.item and self.next_config_cache then
        local num = Item.GetNum(self.next_config_cache.levelup_consume)
        local need_num = self.next_config_cache.levelup_consume_num
        local str = string.format(Language.Common.Cstr, num >= need_num and QualityColorStr[ItemColor.Green] or
            QualityColorStr[ItemColor.Red], num, need_num)
        UH.SetText(self.CostCount, str)
    end
end
function YouMingSanHun:OnClickUpLevel()
    local youming = self.data:GetSelectYouMing()
    if Item.GetNum(self.item.item_id) > 0 then
        self.can_play_effect = true
    end
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.SOUL_LEVEL_UP, youming.index, self.select_index - 1)
    -- OperateFrequency.Operate(function()
    -- end, "sanhun_shengji", 2.8)
end
function YouMingSanHun:OnClickReset()
    -- Config.reset_consume
    local youming = self.data:GetSelectYouMing()
    if youming:Level() > 1 then
        ViewMgr:OpenView(SanHunChongZhi, youming)
    else
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.SanHunOneLevel)
    end
    -- YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.RESET_SOUL, youming.index)
end

YouMingSanHunItem = YouMingSanHunItem or DeclareMono("YouMingSanHunItem", UIWidgetBaseItem)
function YouMingSanHunItem:YouMingSanHunItem()

end
function YouMingSanHunItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, Language.YouMingPractice.HunNames[data.type])
    UH.SetText(self.Level, data.level)
    UH.SetText(self.LockDesc, Language.YouMingPractice.SoulUnlock[data.type + 1])
    local is_lock = true
    local youming = YouMingData.Instance:GetSelectYouMing()
    if not TableIsEmpty(youming) then
        is_lock = youming:SanHunIsLock(data.type)
    end
    self.Lock2:SetActive(is_lock)
    self.Lock:SetActive(is_lock)
    self.Gray.enabled = is_lock
end
function YouMingSanHunItem:Click()
    self.ClickEvent:Trigger()
end
--[[ function YouMingSanHunItem:OnSelect(selected)
    if selected then
        self:Click()
    end
end ]]

YouMingSanHunAttrItem = YouMingSanHunAttrItem or DeclareMono("YouMingSanHunAttrItem", UIWidgetBaseItem)
function YouMingSanHunAttrItem:YouMingSanHunAttrItem()

end
function YouMingSanHunAttrItem:SetData(data)
    if data.attr_value == 0 and data.attr_value2 == 0 then
        self.gameObject:SetActive(false)
    else
        if data.hide_right and data.hide_right == true then
            if self.RightObjs then    
                for i = 1, self.RightObjs:Length() do
                    self.RightObjs[i]:SetActive(false)
                end
            end
        else
            if self.RightObjs then    
                for i = 1, self.RightObjs:Length() do
                    self.RightObjs[i]:SetActive(true)
                end
            end
        end
        if not data.ingore_pos then
            if data.posX then
                UH.LocalPosG(self.Cur, Vector3.New(data.posX,0,0))
            else
                UH.LocalPosG(self.Cur, Vector3.New(-34.5,0,0))
            end
        end
        UH.SetText(self.Desc, Language.Common.AttrList[data.attr_type])
        UH.SetText(self.Cur, DataHelper.Percent(data.attr_type, data.attr_value))
        UH.SetText(self.Next, DataHelper.Percent(data.attr_type, data.attr_value2))
        self.gameObject:SetActive(true)
    end
end

------------------------------------
SanHunChongZhi = SanHunChongZhi or DeclareView("SanHunChongZhi", "youming_practice/sanhun_chongzhi")
function SanHunChongZhi:SanHunChongZhi()
    self.data = YouMingData.Instance
end

function SanHunChongZhi:LoadCallback(data)
    self.Board:SetData(self:GetType(), Language.YouMingPractice.SanHunReset, Vector2.New(682, 421))
    UH.SetText(self.CoinNum, Config.dim_pet_cfg_auto.other[1].reset_consume)
    self.youming = data
    UH.SetText(self.Desc, string.format(Language.YouMingPractice.SanHunTip, self.youming:Level(), self.youming:Name(),
        Config.dim_pet_cfg_auto.other[1].reset_consume))

    local list = self.youming:SanHunList()
    local item_id = 0
    local item_num = 0
    for i, v in ipairs(list) do
        if v.level > 0 then
            for i2, v2 in ipairs(Config.dim_pet_cfg_auto.dim_pet_level) do
                if self.youming.id == v2.id and v.type == v2.type
                and v2.level ~= 1 and v2.level <= self.youming:Level() then
                    item_id = v2.levelup_consume
                    item_num = item_num + v2.levelup_consume_num        
                end
            end
        end
    end
    if item_id > 0 then
        local new_list = {}
        table.insert(new_list, {item_id = item_id, num = item_num})
        self.PetPillList:SetData(new_list)
    end
end

function SanHunChongZhi:OnClickCancel()
    ViewMgr:CloseView(SanHunChongZhi)
end

function SanHunChongZhi:OnClickOk()
    if RoleData.Instance:GetCurrency(CurrencyType.CoinBind) < Config.dim_pet_cfg_auto.other[1].reset_consume then
        PublicPopupCtrl.Instance:AlertTip(Language.Mall.NotCurrency[2], function()
            MallCtrl.Instance:Jump(24326)
        end)
        return
    end
    local youming = self.data:GetSelectYouMing()
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.RESET_SOUL, youming.index)
    ViewMgr:CloseView(SanHunChongZhi)
end

function SanHunChongZhi:CloseCallback()

end
