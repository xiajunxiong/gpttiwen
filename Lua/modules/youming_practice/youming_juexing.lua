YouMingJueXing = YouMingJueXing or DeclareMono("YouMingJueXing", UIWFlushPanel)
function YouMingJueXing:YouMingJueXing()
    self.data = YouMingData.Instance
    self.data_cares = {{
        data = self.data:GetSelectYouMing(),
        func = self.FlushBaseInfo
    }, {
        data = self.data:GetRemodelUse(),
        func = self.FlushJueXingItems,
        keys = {"juexing_items"}
    }}
    self.can_play_effect = false
end

function YouMingJueXing:FlushBaseInfo()
    self.youming = self.data:GetSelectYouMing()
    if self.youming.Aptitude == nil then
        return
    end
    if self.can_play_effect == true then
        self:PlayEffect()
    else
        self:FlushBaseInfo2()
    end
end
function YouMingJueXing:OnClickTip()
    local skill = self.youming:ActiveSkill()
    PublicPopupCtrl.Instance:HelpTip(self.data:GetClickDesc2(skill))
end
function YouMingJueXing:FlushBaseInfo2()
    local level = self.youming:AwakeLevel()
    local config = Cfg.YouMingJueXing(self.youming.id, level, false)
    local next_config = Cfg.YouMingJueXing(self.youming.id, level + 1, false)
    if config == nil then
        return
    end
    self.MaxRoot:SetActive(level == YouMingCfg.MaxAwakenLevel)
    self.CostParent:SetActive(level ~= YouMingCfg.MaxAwakenLevel)
    local attr
    for i = 1, 4 do
        attr = {}
        attr.attr_type = config["attr_type_" .. i]
        attr.attr_value = config["attr_add_" .. i]
        attr.attr_value2 = next_config and next_config["attr_add_" .. i] or 0
        if next_config == nil then
            attr.hide_right = true
            attr.posX = -10
        end
        self.AttrItems[i]:SetData(attr)
    end
    if next_config then
        UH.LocalPosG(self.CurTitle, Vector3.New(269.7, 226.4, 0))
    else
        UH.LocalPosG(self.CurTitle, Vector3.New(296.53, 226.4, 0))
    end
    UH.SetText(self.NameLevel, Format(Language.YouMingPractice.JueXingName2, level))
    UH.SetText(self.CurName, Format(Language.YouMingPractice.JueXingName, level))
    UH.SetText(self.NextName, Format(Language.YouMingPractice.JueXingName, level + 1))
    if next_config then
        self.item = next_config.strengthen_consume_item[0]
        self.List:SetData(next_config.strengthen_consume_item)
        self:FlushJueXingItems()
    else
        self.item = config.strengthen_consume_item[0]
        self.List:SetData(config.strengthen_consume_item)
    end
    local skill = self.youming:ActiveSkill()
    UH.SetText(self.SkillDesc, Format(Language.YouMingPractice.SkillDesc, skill:Name(), skill:Desc()))
    self.SkillCell:SetData(skill)
    self.SkillList:SetData(self.youming:SkillList())
    UH.SetText(self.SkillLevel, Format(Language.YouMingPractice.NameLevel, skill:Name(), level))
end
function YouMingJueXing:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay2)
    self.data:SetAwakeLevelChange(nil)
    self.data:SetJueXingItems({})
end
function YouMingJueXing:PlayEffect()
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay2)
    self.EffectTool:Play(7167022)
    self.handle_delay = TimeHelper:AddDelayTimer(function()
        self.EffectTool:Play(7167023)
        self.handle_delay2 = TimeHelper:AddDelayTimer(function()
            self.EffectTool:Play(7167024)
            self.can_play_effect = false
            self:FlushBaseInfo2()
            local data = self.data:GetAwakeLevelChange()
            if data then
                ViewMgr:OpenView(YouMingJueXingSucc, data)
            end
        end, 1.85)
    end, 1.45)
end
function YouMingJueXing:FlushJueXingItems()
    local level = self.youming:AwakeLevel()
    -- local config = Cfg.YouMingJueXing(self.youming.id, level, false)
    local next_config = Cfg.YouMingJueXing(self.youming.id, level + 1, false)
    if next_config then
        -- 觉醒消耗本体
        self.select_list = self.data:GetJueXingItems()
        local num = table.nums(self.select_list)
        local need_num = next_config.cost_self
        UH.SetText(self.NeedCount, string.format(Language.Common.Cstr, num >= need_num and
            QualityColorStr[ItemColor.Green] or QualityColorStr[ItemColor.Red], num, need_num))
        if num > 0 then
            self.YouMingCell:SetObjActive(true)
            self.YouMingCell:SetData(self.youming)
            self.AddItem:SetActive(false)
        else
            self.YouMingCell:SetObjActive(false)
            self.AddItem:SetActive(true)
        end
    else
        UH.SetText(self.NeedCount, "0/0")
    end
end
function YouMingJueXing:OnClickAdd()
    local temp = {}
    temp.title = Format("+%s %s", self.youming:AwakeLevel(), self.youming:Name())
    ViewMgr:OpenView(YouMingJueXingCostSelect, temp)
end
function YouMingJueXing:OnClickJueXing()
    local level = self.youming:AwakeLevel()
    if level == YouMingCfg.MaxAwakenLevel then
        return
    end
    local list = self.data:GetJueXingItems()
    local temp = {}
    for i, v in ipairs(list) do
        table.insert(temp, v.index)
    end
    if table.nums(temp) > 0 and Item.GetNum(self.item.item_id) > 0 then
        self.can_play_effect = true
    else
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.Text6)
        return
    end

    YouMingCtrl.Instance:SendAwakenLevelUpReq(self.youming.index, temp)
    -- OperateFrequency.Operate(function()
    -- end, "youming_juexiong_shengji", 3.3)
    -- ViewMgr:OpenView(YouMingJueXingSucc)
end

YouMingJueXingItem = YouMingJueXingItem or DeclareMono("YouMingJueXingItem", UIWidgetBaseItem)
function YouMingJueXingItem:YouMingJueXingItem()

end
function YouMingJueXingItem:SetData(data)
    self.Cell:SetDataByItemIdWithNeed(data.item_id, data.num)
end

YouMingJueXingSucc = YouMingJueXingSucc or DeclareView("YouMingJueXingSucc", "youming_practice/youming_juexing_succ")
VIEW_DECLARE_MASK(YouMingJueXingSucc, ViewMask.BgBlockClose)
function YouMingJueXingSucc:YouMingJueXingSucc()
    self.data = YouMingData.Instance
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self
        })
    end
end

function YouMingJueXingSucc:LoadCallback(data)
    local index = data.p1
    local level = data.p2
    self.youming = self.data:GetYouMing(index)
    self.ui_obj:SetData(self.youming:ResPath())
    UH.SetText(self.Name, self.youming:Name())
    local cur, max = self.youming:Aptitude()
    UH.SetText(self.ZiZhi, Format("%s/%s", cur, max))
    UH.SetText(self.PingFen, self.youming:Score())
    -- local level = self.youming:AwakeLevel()
    local config = Cfg.YouMingJueXing(self.youming.id, level - 1, false)
    local next_config = Cfg.YouMingJueXing(self.youming.id, level, false)
    local attr
    for i = 1, 4 do
        attr = {}
        attr.attr_type = config["attr_type_" .. i]
        attr.attr_value = config["attr_add_" .. i]
        attr.attr_value2 = next_config and next_config["attr_add_" .. i] or 0
        attr.ingore_pos = true
        self.AttrItems[i]:SetData(attr)
    end
    local skill = self.youming:ActiveSkill()
    self.CurCell:SetData(skill)
    local last_skill = YouMingSkill.New({
        skill_id = skill.skill_id,
        skill_level = level - 1,
        skill_type = skill.skill_type
    })
    self.LastCell:SetData(last_skill)
    local skill_level_config = skill:LevelCfg(level - 1)
    if skill_level_config then
        UH.SetText(self.Desc1,
            Format(Language.YouMingPractice.SkillDesc2, skill:Name(), level - 1, skill_level_config.description))
        skill_level_config = skill:LevelCfg(level)
        if skill_level_config then
            UH.SetText(self.Desc2,
                Format(Language.YouMingPractice.SkillDesc2, skill:Name(), level, skill_level_config.description))
        end
    end
end

YouMingJueXingCostSelect = YouMingJueXingCostSelect or DeclareView("YouMingJueXingCostSelect", "youming_remodel/youming_juexing_cost")
VIEW_DECLARE_MASK(YouMingJueXingCostSelect, ViewMask.BgBlock)
function YouMingJueXingCostSelect:YouMingJueXingCostSelect()
end

function YouMingJueXingCostSelect:LoadCallback(param_t)
    self.Board:SetData(YouMingJueXingCostSelect, param_t.title, Vector2.New(850, 520))
end

function YouMingJueXingCostSelect:CloseCallback()

end

function YouMingJueXingCostSelect:OnGetWayClick()
    local get_way_info = {}
    local get_way_t = Split(Config.dim_pet_cfg_auto.other[1].get_way,"|")
    for k,v in pairs(get_way_t) do
        table.insert(get_way_info, BagData.Instance:GetWayCfg(tonumber(v)))
    end
    ViewMgr:OpenView(PetGuideTipView, {list = get_way_info, tip = Language.YouMing.Text2})
end

YouMingJueXingCostItem = YouMingJueXingCostItem or DeclareMono("YouMingJueXingCostItem", UIWidgetBaseItem)
-- data === YouMing
function YouMingJueXingCostItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.InfoItem:SetData(data)
    local cur, max = data:Aptitude()
    UH.SetText(self.ZiZhi, Format(Language.YouMingPractice.TianZi3, cur, max))
end

function YouMingJueXingCostItem:OnSelectClick(selected)
    if selected then
        -- 假如满足条件那就
        local list = self.Panel.ChooseList:GetSelectedData()
        local youming = YouMingData.Instance:GetSelectYouMing()
        if not TableIsEmpty(youming) then
            local level = youming:AwakeLevel()
            -- local config = Cfg.YouMingJueXing(self.youming.id, level, false)
            local next_config = Cfg.YouMingJueXing(youming.id, level + 1, false)
            if next_config then
                -- 觉醒消耗本体
                local num = table.nums(list)
                local need_num = next_config.cost_self
                if num > need_num then
                    PublicPopupCtrl.Instance:Center(Language.YouMingPractice.JueXingMaxCount)
                    self.Selector.isOn = false
                end
            end
        end
        -- YouMingData.Instance:SetSelectYouMing(self.data)
        -- 设置觉醒消耗的幽冥
    end
    UIWidgetBaseItem.OnSelect(self, selected)
end

----------------------------------------------------
YouMingJueXingCostSelectPanel = YouMingJueXingCostSelectPanel or
                                    DeclareMono("YouMingJueXingCostSelectPanel", UIWFlushPanel)
function YouMingJueXingCostSelectPanel:YouMingJueXingCostSelectPanel()
    self.data = YouMingData.Instance
    RichTextHelper.Bind(self.TipRichText, BindTool.Bind(self.OnClickTipRich, self))

end
function YouMingJueXingCostSelectPanel:onFlush()
    self.data:SetJueXingItems({})
    self.data:FlushJueXingSelect()
    self:FlushShowInfo()
end
function YouMingJueXingCostSelectPanel:FlushShowInfo()

    self:FlushChooseList()
    local get_way_str = ">>" .. RichTextHelper.LinkStr(Language.Pet.PetStrengthGetWay, "_ul", COLORSTR.Yellow2) .. "<<"
    UH.SetText(self.TipRichText, get_way_str)
end

function YouMingJueXingCostSelectPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

end

-- 选择副宠列表
function YouMingJueXingCostSelectPanel:FlushChooseList()
    local data = self.data:GetJueXingCostList()
    self.ChooseList:SetData(data)
    self.LblTip:SetActive(next(data) == nil)
end

-- 点击富文本回调
function YouMingJueXingCostSelectPanel:OnClickTipRich()

end

function YouMingJueXingCostSelectPanel:OnClickConfirmSelect()
    local list = self.ChooseList:GetSelectedData()
    --[[ for i, v in ipairs(list) do
        LogDG(v.index)
    end ]]
    self.data:SetJueXingItems(list)
    self.data:FlushJueXingSelect()
    ViewMgr:CloseView(YouMingJueXingCostSelect)
end
