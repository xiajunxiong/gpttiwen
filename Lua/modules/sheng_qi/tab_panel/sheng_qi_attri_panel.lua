ShengQiAttriPanel = ShengQiAttriPanel or DeclareMono("ShengQiAttriPanel", UIWFlushPanel)
function ShengQiAttriPanel:ShengQiAttriPanel()
    self.data = ShengQiData.Instance
    self.data_cares = {
        {data = self.data.sheng_qi_list, func = self.FlushShow, init_call = true},
        {data = self.data.sel_sheng_qi, func = self.FlushPanel, init_call = true},
        {data = self.data.follow_data, func = self.FlushFollow, init_call = true},
    }
    GuideManager.Instance:RegisterGetGuideUi("ShengQiAttriGuide",function ()
        return self.AttriGuide,function ()
            --self:OnDetailClick()
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("ShengQiSkillGuide",function ()
        return self.SkillGuide
    end)
end
function ShengQiAttriPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function ShengQiAttriPanel:FlushPanel()
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    self.ModelInfo:SetData(sheng_qi)
    -- self.StateInfo:SetData(sheng_qi)
    -- self.AttriInfo:SetData(sheng_qi)
    -- self.SkillInfo:SetData(sheng_qi)

    local init_skill_cfg = Config.shengqi_auto.skill_hole
    self.TnitativeList:SetData(init_skill_cfg)
    self:FlushFollow()
end

function ShengQiAttriPanel:FlushShow()
    local shengqi_list = self.data:GetShowList()
    local is_noting = TableIsEmpty(shengqi_list)
    self.Show:SetActive(not is_noting)
    self.Nothing:SetActive(is_noting)
end

function ShengQiAttriPanel:FlushFollow()
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    local follow_sheng_qi = self.data:GetFollowShengQi() or {index = -1}
    self.FollowOn:SetActive(sheng_qi.index == follow_sheng_qi.index)
    self.FollowOff:SetActive(sheng_qi.index ~= follow_sheng_qi.index)
end

function ShengQiAttriPanel:OnGetClick()
    ShengQiCtrl.GetShengQiJump()
end

function ShengQiAttriPanel:OnSkillLockClick()
    PublicPopupCtrl.Instance:AlertTip(Language.ShengQi.Tip8, function ()
        self.data:SetSelTab({open = Mod.ShengQi.Skill})
    end)
   -- PublicPopupCtrl.Instance:Center("可前往技能界面获取技能")
end

function ShengQiAttriPanel:OnFollowClick()
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    local follow_sheng_qi = self.data:GetFollowShengQi() or {index = -1}
    if sheng_qi.index == follow_sheng_qi.index then
        ShengQiCtrl.Instance:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_FOLLOW,-1)
    else
        ShengQiCtrl.Instance:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_FOLLOW,sheng_qi.index)
    end
end

function ShengQiAttriPanel:OnDetailClick()
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    ViewMgr:OpenView(ShengQiDetailView, {sheng_qi = sheng_qi})
end

function ShengQiAttriPanel:OnOpenPoolClick()
    ViewMgr:OpenView(ShengQiSkillPoolView)
end

function ShengQiAttriPanel:OnUpdateClick()
    ViewMgr:OpenView(ShengQiSkillUpdateView)
end

-- sheng_qi_state_info
ShengQiStateSeletor = ShengQiStateSeletor or DeclareMono("ShengQiStateSeletor")
function ShengQiStateSeletor:SetData(data)
    local state = data:State()
    self.state = state
    self.data = data
    for i = 1, 3 do
        self.StateToggles[i].isOn = i - 1 == state
    end
end

function ShengQiStateSeletor:OnStateClick(state)
    if state ~= self.state then
        ShengQiCtrl.Instance:SetShengQiState(self.data, state)
    end
end

-- sheng_qi_attri_info
ShengQiAttriInfo = ShengQiAttriInfo or DeclareMono("ShengQiAttriInfo")
function ShengQiAttriInfo:SetData(data)
    local active_attri,attri_list_data = data:AttriShowList()
    self.AttriOne:SetData(active_attri)
    self.AttriList:SetData(attri_list_data or {})
end

-- sheng_qi_skill_info.prefab
ShengQiSkillInfo = ShengQiSkillInfo or DeclareMono("ShengQiSkillInfo")
function ShengQiSkillInfo:Awake()
    self.Layout.enabled = true
end
function ShengQiSkillInfo:SetData(data)
    local is_two_talent = true     -- 是否有两个天赋技能
    for i = 1,self.TalentList:Length() do
        local skill = data:GetTalentSkill(i - 1)
        if TableIsEmpty(skill) or skill.skill_id == 0 then
            is_two_talent = false
            self.TalentList[i]:SetObjActive(false)
        else
            self.TalentList[i]:SetObjActive(true)
            self.TalentList[i]:SetData(skill)
        end
    end
    self.OutSkillTop:SetActive(not is_two_talent)
    self.OutSkillBottom:SetActive(is_two_talent)
    local skill_mono_list = is_two_talent and self.InitList2 or self.InitList1
    for i = 1, skill_mono_list:Length() do
        local skill = data:GetInitativeSkill(i - 1)
        skill_mono_list[i]:SetData(skill)
    end
end