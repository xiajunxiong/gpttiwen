--================================RoleFlyUpPanel================================
RoleFlyUpPanel = RoleFlyUpPanel or DeclareMono("RoleFlyUpPanel", UIWFlushPanel)

function RoleFlyUpPanel:RoleFlyUpPanel()
    self.data = FlyUpData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid,func = self.FlushItemView,init_call = true},
        {data = self.data.red_piont_data,func = self.FlushRedNum,init_call = false},
        {data = self.data.info_data, func = self.OnFlush, init_call = true,keys = {"skill_level"}},
        {data = self.data.info_data, func = self.FlushProgress, init_call = false,keys = {"skill_exp"}},
    }
end

function RoleFlyUpPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Toggle:SetActive(PrefsInt([[RoleFlyUpNum]]..RoleId()) == 0)
end

function RoleFlyUpPanel:OnEnable()
	UIWFlushPanel.OnEnable(self)
    UH.StopEffect(self.Effect,self.effect_handle)
end

function RoleFlyUpPanel:OnFlush()
    local skill_level = self.data:GetSkillLevel()
    local next_config = self.data:GetSkillUpConfig(skill_level + 1)
    local config = next_config or self.data:GetSkillUpConfig(skill_level)
    local skill_config = SkillData.GetDecConfig(self.data:GetSkillId(),skill_level)
    UH.SetText(self.SkillName,skill_config.skill_name)
    UH.SetText(self.SkillTitle,skill_config.show_name)
    UH.SetIcon(self.SkillIcon,skill_config.icon_id,ICON_TYPE.SKILL)
    UH.SetText(self.Tips,Format(Language.FlyUp.SkillTips,config.role_level) .. "         ")
    UH.SetText(self.Level,skill_level)
    UH.SetText(self.Desc,Config.language_cfg_auto.textdesc[199].desc)
    UH.SetText(self.BtnName,Language.FlyUp.SkillBtnName[next_config and 1 or 2])
    UH.SetInter(self.BtnInter,next_config ~= nil)

    self.Tips:SetObjActive(RoleData.Instance:GetRoleLevel() < config.role_level)
    local curr_num,target_num = FightData.FightOtherDate().initial_sp,FightData.FightOtherDate().max_sp
    self.FuryProgress:SetProgress(curr_num / target_num)
    self.FuryProgress:SetText(Format("%s/%s",curr_num,target_num))

    if self.skill_level ~= skill_level then
        UH.StopEffect(self.Effect,self.effect_handle)
        self.effect_handle = UH.PlayEffect(self.Effect,[[3161058]])
        self:FlushListView()
    end
    self.skill_level = skill_level
    self.curr_config = config

    self:FlushProgress()
end

function RoleFlyUpPanel:FlushListView()
    local skill_level = self.data:GetSkillLevel()
    self.List:SetData(self.data:GetSkillList())
    self.List:AutoSlideRealy(skill_level-1,false)
end

function RoleFlyUpPanel:FlushItemView()
    local skill_level = self.data:GetSkillLevel()
    local next_config = self.data:GetSkillUpConfig(skill_level + 1)
    local config = next_config or self.data:GetSkillUpConfig(skill_level)
    -- UH.SetText(self.Name,Item.GetQuaName(config.item_id,true))
    self.Call:SetData(Item.Create({item_id = config.item_id}))
    self.Call:SetDataByItemIdWithNeed(config.item_id,config.consume)
    self:FlushRedNum()
end
function RoleFlyUpPanel:OnClickOneKey()
    FlyUpCtrl.Instance:SendFlyUpReq(7)
end
function RoleFlyUpPanel:FlushProgress()
    local skill_level = self.data:GetSkillLevel()
    local target_num = self.curr_config.need_exp
    local next_config = self.data:GetSkillUpConfig(skill_level + 1)
    if next_config then
        local exp_num = self.data:GetSkillExp()
        self.Progress:AddToProgress(exp_num / target_num)
        self.Progress:SetText(Format("%s/%s",exp_num,target_num))
        --UH.SetText(self.Exp,Format(Language.FlyUp.ExpTip,self.data:GetOtherConfig().add_exp))
        UH.SetText(self.Exp, Format(Language.FlyUp.ExpTip2, Item.GetQuaName(next_config.item_id,true), self.data:GetOtherConfig().add_exp))
    else
        self.Progress:SetText(Format("%s/%s",target_num,target_num))
        UH.SetText(self.Tips,Language.FlyUp.SkillMaxLevel)
        self.Progress:SetProgress(1)
        self.Tips:SetObjActive(true)
    end
end

function RoleFlyUpPanel:FlushRedNum()
    self.RedPiont:SetNum(self.data:GetRoleFlyUpNum())
end

function RoleFlyUpPanel:OnClickToggle()
    local is_active = GetActive(self.Toggle)
    self.Toggle:SetActive(not is_active)
    PrefsInt([[RoleFlyUpNum]]..RoleId(),is_active and 1 or 0)
    self.data.red_piont_data.val = not self.data.red_piont_data.val
end

function RoleFlyUpPanel:OnClickUp()
    if self.IsClickDown == true then
        return
    end
    if RoleLevel() < self.curr_config.role_level then
        PublicPopupCtrl.Instance:Center(Language.AllSoulsStorybook.RoleLevelNotEnough)
        self.IsClickDown = true
        return
    end
    if Item.GetNum(self.curr_config.item_id) < self.curr_config.consume then
        MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = self.curr_config.item_id})})
        PublicPopupCtrl.Instance:Center(ErrorText[24])
        self.IsClickDown = true
        return
    end
    FlyUpCtrl.Instance:SendFlyUpReq(4)
end

--长按功能-----------------------
function RoleFlyUpPanel:OnClickDown()
    self.IsClickDown = nil
    TimeHelper:CancelTimer(self.DownTimer)
    TimeHelper:CancelTimer(self.ShowTimer)
    self.DownTimer = UpdateTime(function()
        self:OnClickUp()
    end,function()
        self.ShowTimer = TimeHelper:AddRunTimer(function()
            self:OnClickUp()
        end,0.1)
    end,3,0.3)
end

function RoleFlyUpPanel:OnClickLift()
    TimeHelper:CancelTimer(self.DownTimer)
    TimeHelper:CancelTimer(self.ShowTimer)
    self.IsClickDown = nil
end

function RoleFlyUpPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ShowTimer)
    TimeHelper:CancelTimer(self.DownTimer)
end