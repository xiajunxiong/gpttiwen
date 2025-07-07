--宠物详细(精简版)
PetDetailSView = PetDetailSView or DeclareMono("PetDetailSView", UIWFlushPanel)

function PetDetailSView:PetDetailSView()
    self.data = PetData.Instance
    self.beh_data = BehaviorData.Instance

    self.data_cares = {
		{data = self.data.cur_sel_detail, func = self.FlushInfo, init_call = false},
	}

end

function PetDetailSView:Awake()
    UIWFlushPanel.Awake(self)
    self.SubmitBtnObj:SetActive(nil ~= TaskChainItemSel.cur_sel_data)
    self:FlushInfo()
    if nil ~= TaskChainItemSel.cur_sel_data and self.beh_data.auto_behavior > 0 then
        self.timer_auto_beh = TimeHelper:AddDelayTimer(function ()
            ViewMgr:CloseView(TaskChainItemSel)
            self:OnSubmitClick()
        end,
        self.beh_data.auto_behavior)
    end
end

function PetDetailSView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_auto_beh)
end

function PetDetailSView:FlushInfo()
    self.pet = self.data.cur_sel_detail

    self.PetName.text = self.pet:Name()
    self.PetName.color = QualityColor[Item.GetColor(self.pet:ID()) or 1]
    self.Level.text = self.pet:Level() .. Language.Common.Level

    self.QuaSp.SpriteName = PetData.PetLevel[self.pet.config.quality]
    PetData.SetStrLevel(nil, self.StrLevel, self.pet:StrLevel())

    self.AttrList:SetPet(self.pet)

    -- local total = {}
    -- for _, v in ipairs(self.pet:GetSkillAndGiftGroupList()) do
    --     table.insert(total, v)
    -- end
    -- for i=#total + 1 ,8 do
    --     total[i] = {skill_id = 0}
    -- end
    -- self.SkillList:SetData(total)

    local total = {}
    --若宠物数据中无已经学会的技能列表则取配置表中的默认技能列表（与宠物百科中展示的相同）
    --例：寻宝中概率展示中的宠物图标
    local skill_list = self.use_config and self.pet:GetSkillAndGiftGroupList() or self.pet:GetSkillAndGiftListNoEmpty()
    -- if self.pet.skill_list == nil and DEBUG_MODE then
    --     Debuger.LogWarning("PetDetailView self.pet.skill_list is nil，show skill_list in config")
    -- end
    for _, v in ipairs(skill_list) do
        table.insert(total, v)
    end
    self.SkillList:SetData(total)
end

function PetDetailSView:OnSubmitClick()
    if TaskChainItemSel.custom_func ~= nil then 
        TaskChainItemSel.custom_func()
        return 
    end 
    TaskCtrl.Instance:SubmitTaskChainItemSel()
end