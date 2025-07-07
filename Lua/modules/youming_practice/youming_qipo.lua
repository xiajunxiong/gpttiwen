YouMingQiPo = YouMingQiPo or DeclareMono("YouMingQiPo", UIWFlushPanel)

function YouMingQiPo:YouMingQiPo()
    self.data = YouMingData.Instance
    self.data_cares = {
        {data = self.data:GetSelectYouMing(), func = self.FlushBaseInfo}
    }
    self.select_index = 1
    self.can_play_effect = false
end

function YouMingQiPo:FlushBaseInfo()
    self.youming = self.data:GetSelectYouMing()
    if self.youming.Aptitude == nil then
        return
    end

    if self.can_play_effect then
        self:FlyEffectFunc()
        TimeHelper:CancelTimer(self.delay_handle2)
        self.delay_handle2 = TimeHelper:AddDelayTimer(function ()
            self:FlushBaseInfo2()
        end, 2)
    else
        self:FlushBaseInfo2()
    end
end
function YouMingQiPo:FlushBaseInfo2()
    local list = self.youming:QiPoList()
    for i = 1, 7 do
        self.QiPoItems[i]:SetData(list[i])
    end
    local total_level = 0
    for k, v in pairs(list) do
        total_level = total_level + v.level
    end

    self.QiPoItems[self.select_index]:Click()
    local skill = self.youming:PossessSkill()
    local skill_id = skill.skill_id
    local skill_level,_ = self.youming:PossessSkillLevel()
    local skill_config = Cfg.AppendageSkillLevelCfg(skill_id, skill_level, false)
    local hide_right = false
    if skill_config then
        local next_config = Cfg.AppendageSkillLevelCfg(skill_id, skill_level + 1, false)
        if next_config then
            if total_level >= skill_config.spirits_total_level then
                UH.SetText(self.Desc, Format(Language.YouMingPractice.QiPoDesc, next_config.spirits_total_level, skill_level + 1))
            else
                if skill_level == 1 then
                    hide_right = true
                    UH.SetText(self.Desc, Format(Language.YouMingPractice.QiPoDesc, skill_config.spirits_total_level, 1))
                else
                    UH.SetText(self.Desc, Format(Language.YouMingPractice.QiPoDesc, skill_config.spirits_total_level, skill_level))
                end
            end
        else
            hide_right = true
            UH.SetText(self.Desc, Format(Language.YouMingPractice.QiPoDesc, total_level, skill_level))
        end
    end

    self.SkillCell:SetData(skill)
    self.SkillCell2:SetData(skill)
    UH.SetText(self.SkillName, skill:Name())
    local config = skill:CurCfg()
    local next_config = skill:NextCfg()
    local attr
    for i = 1, 5 do
        attr = {}
        attr.attr_type = config["attr_type_" .. i]
        attr.attr_value = config["attr_value_" .. i]
        attr.attr_value2 = next_config and next_config["attr_value_" .. i] or 0
        attr.hide_right = hide_right or false
        self.SkillItems[i]:SetData(attr)
    end
    local level,_ = self.youming:PossessSkillLevel()
    UH.SetText(self.SkillLevel, Format(Language.YouMingPractice.NameLevel, skill:Name(), level))
end
function YouMingQiPo:FlyEffectFunc()
    self.target_pos = self.QiPoItems[self.select_index].gameObject.transform.localPosition
    self.FlyEffect.gameObject:SetActive(true)
    UH.LocalPosG(self.FlyEffect, self.CostCell.gameObject.transform.localPosition)
    Runner.Instance:RemoveRunUntilTrue(self.time_handle)
    TimeHelper:CancelTimer(self.delay_handle)
    self.time_handle = UH.MoveToTarget(self.FlyEffect, self.target_pos, 45,function()
        self.FlyEffect.gameObject:SetActive(false)
        self.EffectTool:Play(3165006, self.QiPoItems[self.select_index].Icon.gameObject)
        self.delay_handle = TimeHelper:AddDelayTimer(function ()
           self.EffectTool:Play(7167021)
        end,1)
    end)
end
function YouMingQiPo:OnDestroy()
    BagData.Instance:RemoveListenItems(self.handle_lis)
    Runner.Instance:RemoveRunUntilTrue(self.time_handle)
    TimeHelper:CancelTimer(self.delay_handle)
    TimeHelper:CancelTimer(self.delay_handle2)
    TimeHelper:CancelTimer(self.delay_handle3)
    UIWFlushPanel.OnDestroy(self)
end
function YouMingQiPo:OnEnable()
    UIWFlushPanel.OnEnable(self)
    TimeHelper:CancelTimer(self.delay_handle3)
    self.delay_handle3 = TimeHelper:AddDelayFrameTimer(function ()
        self.QiPoItems[self.select_index]:Click()
    end,2)
end

function YouMingQiPo:OnClickItem(data)
    self.select_index = data.type + 1
    self.MaxRoot:SetActive(data.level == YouMingCfg.QiPoLevel)
    self.CostParent:SetActive(data.level ~= YouMingCfg.QiPoLevel)

    local config = Cfg.YouMingQiPoCfg(data.id, data.type, data.level, false)
    if config == nil then
        config = Cfg.YouMingQiPoCfg(data.id, data.type, 1, false)
    end
    UH.SetText(self.NameLevel, Format(Language.YouMingPractice.NameLevel, Language.YouMingPractice.PoNames[data.type], data.level))
    -- LogDG(config)
    if config then
        self.item = Item.Create({item_id = config.levelup_consume})
        if self.handle_lis == nil then
            self.handle_lis = BagData.Instance:ListenItems({config.levelup_consume},
                BindTool.Bind(self.FlushCostItem, self))
        end
        local next_config = Cfg.YouMingQiPoCfg(data.id, data.type, data.level + 1, false)
        self.CostCell:SetData(self.item)
        UH.SetText(self.CostName, self.item:Name())
        if next_config then
            self.next_config_cache = next_config
            local num = Item.GetNum(self.next_config_cache.levelup_consume)
            local need_num = self.next_config_cache.levelup_consume_num
            local str = string.format(Language.Common.Cstr, num >= need_num and QualityColorStr[ItemColor.Green] or
                QualityColorStr[ItemColor.Red], num, need_num)
            UH.SetText(self.CostCount, str)

            -- UH.SetText(self.CostCount, Format("%s/%s", Item.GetNum(config.levelup_consume), next_config.levelup_consume_num))
            local attr
            for i = 1, 4 do
                attr = {}
                attr.attr_type = config["attr_type_"..i]
                attr.attr_value = data.level == 0 and 0 or config["attr_add_"..i]
                attr.attr_value2 = next_config["attr_add_"..i]
               self.AttrItems[i]:SetData(attr)
            end
            UH.LocalPosG(self.CurTitle, Vector3.New(269.7,228.7,0))
        else
            UH.LocalPosG(self.CurTitle, Vector3.New(296.4,228.7,0))
            local attr
            for i = 1, 4 do
                attr = {}
                attr.attr_type = config["attr_type_"..i]
                attr.attr_value = data.level == 0 and 0 or config["attr_add_"..i]
                attr.attr_value2 = 0-- next_config["attr_add_"..i]
                attr.hide_right = true
                attr.posX = -10
               self.AttrItems[i]:SetData(attr)
            end
        end
    end
end
function YouMingQiPo:FlushCostItem()
    if self.item and self.next_config_cache then
        local num = Item.GetNum(self.next_config_cache.levelup_consume)
        local need_num = self.next_config_cache.levelup_consume_num
        local str = string.format(Language.Common.Cstr, num >= need_num and QualityColorStr[ItemColor.Green] or
            QualityColorStr[ItemColor.Red], num, need_num)
        UH.SetText(self.CostCount, str)
    end
end

function YouMingQiPo:OnClickUplevel()
    self.youming = self.data:GetSelectYouMing()
    if self.youming.Aptitude == nil then
        return
    end
    if Item.GetNum(self.item.item_id) > 0 then
        self.can_play_effect = true
    end
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.SPIRITS_LEVEL_UP, self.youming.index, self.select_index - 1)
    -- OperateFrequency.Operate(function ()
    -- end,"qipo_shengji", 2)
end
function YouMingQiPo:OnClickTip()
    local skill = self.youming:PossessSkill()
    PublicPopupCtrl.Instance:HelpTip(self.data:GetClickDesc(skill))
end



YouMingQiPoItem = YouMingQiPoItem or DeclareMono("YouMingQiPoItem", UIWidgetBaseItem)
function YouMingQiPoItem:YouMingQiPoItem()
    
end
function YouMingQiPoItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, Language.YouMingPractice.PoNames[data.type])
    UH.SetText(self.Level, Format(Language.YouMingPractice.Level2, data.level))
end
function YouMingQiPoItem:Click()
    self.ClickEvent:Trigger()
end
