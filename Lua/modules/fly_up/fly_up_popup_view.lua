FlyUpPopupView = FlyUpPopupView or DeclareView("FlyUpPopupView", "fly_up/fly_up_popup_view")

VIEW_DECLARE_LEVEL(FlyUpPopupView,ViewLevel.L0)

function FlyUpPopupView:FlyUpPopupView()
    self.data = FlyUpData.Instance
end

function FlyUpPopupView:LoadCallback(param_t)
    self:FlushModelUIShow()
    -- self:FlushSkillView()
    self:FlushTitleView()
    self:FlushTaskView()
    self:FlushDescView()
    self:FlushRedNumView()
    self:FlushNameView()
    if param_t.guide_func then
        param_t.guide_func()
    end
end

function FlyUpPopupView:FlushModelUIShow()
    local role_appearance = TableCopy(RoleData.Instance:GetApearance())
    self.cur_appearance = {
		weapon_id = RoleProfessionSimpleWeaponId[role_appearance.prof_base] or 0,
		color = role_appearance.color,
		prof_base = role_appearance.prof_base,
        avatar_type = role_appearance.avatar_type,
        advance_time = role_appearance.advance_time
    }
    if self.ModelGame == nil then
        self.ModelGame = UIChDrawer.CreateByAppearance(self.cur_appearance)
        self.ModelUIShow:SetShowData({ui_obj = self.ModelGame,view = self,load_callback = function()
            self.ModelGame:PlayAnim(ChAnimType.WeaponShow)
        end})
    else
        self.ModelGame:SetData(self.cur_appearance)
    end
end

function FlyUpPopupView:FlushSkillView()
    if FlyUpStageType.Low == FlyUpData.Instance:GetNextStage() then
        local skill_config = SkillData.GetDecConfig(self.data:GetSkillId())
        UH.SetText(self.SkillName,skill_config.skill_name)
        UH.SetText(self.SkillTitle,Language.FlyUp.SkillOpenTip[1])
        UH.SetText(self.SkillDesc,skill_config.description)
        UH.SetIcon(self.SkillIcon,skill_config.icon_id,ICON_TYPE.SKILL)
        self.Cell:SetObjActive(false)
    else
        UH.SetText(self.SkillName,Language.FlyUp.SkillOpenTip[3])
        UH.SetText(self.SkillTitle,Language.FlyUp.SkillOpenTip[2])
        UH.SetText(self.SkillDesc,Language.FlyUp.SkillTip)
        -- self.Cell:SetData(Item.Create({item_id = 56013}))
        self.Cell:ClearCare()
        self.Cell:ClearFlush()
        self.Cell.data = Item.Create({item_id = 56013})
        CellFlushs.SetIconQuality(self.Cell, self.Cell.data.item_id)
        CellFlushs.SetIcon(self.Cell, 56013)
        CellFlushs.SetQuaSpAnim(self.Cell)
        -- UH.SetIcon(self.SkillIcon,56013, ICON_TYPE.ITEM)
        self.Cell:SetObjActive(true)
        -- self.Cell:SetObjActive(false)
    end
end

function FlyUpPopupView:FlushTitleView()
    local stage = FlyUpData.Instance:GetNextStage()
    local config = FlyUpData.Instance:GetOtherConfig()
    local title_id = config["feisheng_title" .. stage]
    local title_config = TitleData.GetTitleConfig(title_id)
    if title_config then
        -- UH.SetText(self.TitleName,Format(Language.FlyUp.TitleName,title_config.name))
        -- self.TitleIcon:SetData(title_config)
        UH.SetText(self.TitleName2,Format(Language.FlyUp.TitleName,title_config.name))
        self.TitleIcon2:SetData(title_config)
    end
end

function FlyUpPopupView:FlushTaskView()
    local task_list = self.data:GetTaskList()
    --[[ for i=1,self.TaskList:Length() do
        self.TaskList[i]:SetData(task_list[i])
    end ]]
    for i=1,self.TaskList2:Length() do
        self.TaskList2[i]:SetData(task_list[i])
    end
end

function FlyUpPopupView:FlushDescView()
    local stage = FlyUpData.Instance:GetNextStage()
    local OpenDesc = Language.FlyUp.OpenDesc[stage]
    --[[ for i=1,self.DescList:Length() do
        UH.SetText(self.DescList[i],OpenDesc[i])
    end ]]
    for i=1,self.DescList:Length() do
        UH.SetText(self.DescList2[i],OpenDesc[i])
    end
    local limit_str = FlyUpData.Instance:IsLevelLimit() and Language.Role.LevelLimit2 or ""
    -- UH.SetText(self.LevelLimit, limit_str)
    UH.SetText(self.LevelLimit2, limit_str)
    self.LevelLimitDi:SetActive(FlyUpData.Instance:IsLevelLimit())
end

function FlyUpPopupView:FlushRedNumView()
    self.RedPoint:SetActive(self:GetTaskTip() == nil)
end

function FlyUpPopupView:FlushNameView()
    UH.SetText(self.Name,Language.FlyUp.Name[self.data:GetNextStage()])
end

function FlyUpPopupView:CloseCallback()
    Delete(self,[[ModelGame]])
end

function FlyUpPopupView:OnClickClose()
    ViewMgr:CloseView(FlyUpPopupView)
end

function FlyUpPopupView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(195+FlyUpData.Instance:GetNextStage())
end

function FlyUpPopupView:OnClickEnter()
    local TaskTip = self:GetTaskTip()
    if TaskTip then
        PublicPopupCtrl.Instance:Center(TaskTip)
        return
    end
    FlyUpCtrl.Instance:SendFlyUpReq(2)
    ViewMgr:CloseView(FlyUpPopupView)
end

function FlyUpPopupView:GetTaskTip()
    for i=1,self.TaskList2:Length()do
        if self.TaskList2[i].data.target then
            return Language.FlyUp.PromoteTaskTip[i]
        end
    end
end

FlyUpPopupPanel = FlyUpPopupPanel or DeclareMono("FlyUpPopupPanel", UIWFlushPanel)

--========================FlyUpPopupTaskItem========================
FlyUpPopupTaskItem = FlyUpPopupTaskItem or DeclareMono("FlyUpPopupTaskItem", UIWidgetBaseItem)

function FlyUpPopupTaskItem:SetData(data)
    UH.SetText(self.Desc,data.desc)
    self.Finish:SetActive(not data.target)
    UH.SetText(self.Integral,data.integral)
    self.BtnTip:SetActive(data.target and data.event ~= nil)
    if self.RedPoint then
        self.RedPoint:SetNum(FlyUpData.Instance:IsHasConsumeNum() and 1 or 0)
    end
    if self.TiaoJianSp then
        UH.SpriteName(self.TiaoJianSp, (not data.target) and "TiaoJianZhuangShiYi" or "TiaoJianZhuangShiWei")
    end
    UIWidgetBaseItem.SetData(self, data)
end

function FlyUpPopupTaskItem:OnClick()
    if self.data.event then
        self.data.event()
    end
end