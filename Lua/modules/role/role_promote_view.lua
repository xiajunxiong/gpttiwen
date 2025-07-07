RolePromoteView = RolePromoteView or DeclareView("RolePromoteView", "role/role_promote_view",Mod.RolePromote.Main)

VIEW_DECLARE_LEVEL(RolePromoteView, ViewLevel.L0)

function RolePromoteView:OnClickClose()
    ViewMgr:CloseView(RolePromoteView)
end

--========================RolePromotePanel========================
RolePromotePanel = RolePromotePanel or DeclareMono("RolePromotePanel", UIWFlushPanel)

function RolePromotePanel:RolePromotePanel()
    self.data = RolePromoteData.Instance
    self.now_sub_level = self.data.info_data.sub_level
    self.data_cares = {
        {data = RoleData.Instance:GetBaseData(), keys = {"appearance"}, func = self.OnFlush},
        {data = RoleData.Instance:GetBaseData(), keys = {"profession_id"}, func = self.FlushView},
        {data = self.data.info_data, keys = {"sub_level"}, func = self.OpenSucce,init_call = false},
        {data = self.data.info_data, keys = {"sub_fight_finish"}, func = self.FlushView,init_call = false},
        {data = RoleData.Instance.promote_red_piont, func = self.FlushView,init_call = false},
        {data = RoleData.Instance:GetCurrencyData(), keys = {CurrencyType.CoinBind}, func = self.FlushView,init_call = false},
    }
end

function RolePromotePanel:Awake()
    UIWFlushPanel.Awake(self)
    self.data:SendTaskDemand()
end

function RolePromotePanel:OnFlush()
    self:FlushNextModel()
    self:FlushView()
end

function RolePromotePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self,[[ui_obj_next]])
    TimeHelper:CancelTimer(self.ShowTimer)
end

-- 现在升小阶也要打开这个界面
function RolePromotePanel:OpenSucce()
    -- 这里只是升小阶的时候弹，升大阶的时候收到外观改变的协议会弹
    -- 这个sub_level是每个大阶从0开始算的
    if self.data.info_data.sub_level > self.now_sub_level then
        local role_appearance = RoleData.Instance:GetApearance()
        ViewMgr:OpenView(RolePromoteSucceView, {last_appearance = TableCopy(role_appearance), cur_appearance = TableCopy(role_appearance)})
    end

    self.now_sub_level = self.data.info_data.sub_level
    self:FlushView()
end

function RolePromotePanel:FlushNextModel()
    local prof_id = RoleData.Instance:GetRoleProfId()
    local config = RolePromoteData.GetProfConfig(prof_id + 1)
    local role_appearance = TableCopy(RoleData.Instance:GetApearance())
    self.cur_appearance = {
		weapon_id = RoleProfessionSimpleWeaponId[role_appearance.prof_base] or 0,
		color = role_appearance.color,
		prof_base = role_appearance.prof_base,
        avatar_type = role_appearance.avatar_type,
        advance_time = role_appearance.advance_time
    }
    if config ~= nil then
        self.next_appearance = TableCopy(self.cur_appearance)
        self.next_appearance.advance_time = self.cur_appearance.advance_time + 1
        self.next_appearance.color = self.select_color or 0
        self.next_appearance.weapon_id = self.cur_appearance.weapon_id + 1
        if self.ui_obj_next == nil then
            self.ui_obj_next = UIChDrawer.CreateByAppearance(self.next_appearance)
            self.ModelUIShow:SetShowData({ui_obj = self.ui_obj_next,view = self})
        end
        self.ui_obj_next:SetData(self.next_appearance)
    else
        if self.ui_obj_next == nil then
            self.ui_obj_next = UIChDrawer.CreateByAppearance(self.cur_appearance)
            self.ModelUIShow:SetShowData({ui_obj = self.ui_obj_next,view = self})
        end
        self.ui_obj_next:SetData(self.cur_appearance)
        return
    end
    local role_app_cfg = RoleAppearChangeData.Instance:GetRoleAppCfgByAppear(self.next_appearance)
    if role_app_cfg == nil then return end 
    local cur_color_list = Split(role_app_cfg.color_list,",")
    for i = 1,4 do 
        UH.SpriteName(self.ColorsBtn[i], "PeiSe"..cur_color_list[i])
    end 
end

--刷新红点
function RolePromotePanel:FlushRemindView()
    self.RedPoint:SetActive(self.data:GetPromoteNum() == 1)
end

--刷新页面
function RolePromotePanel:FlushView()
    local prof_id = RoleData.Instance:GetRoleProfId()
    local prof_grade = RolePromoteData.Instance:GetProfGrade()
    local cur_config = RolePromoteData.GetProfConfig(prof_id,prof_grade)
    local next_config = RolePromoteData.GetProfConfig(prof_id + 1)
    if cur_config == nil then
        return
    end
    if next_config ~= nil then
        local now_config = RolePromoteData.GetNextProfConfig(prof_id,prof_grade)
        local max_grade = DataHelper.GetDaXie(self.data:GetMaxGrade(prof_id) + 1)
        local cell_data = self.data:GetPromoteTaskList(cur_config)
        local next_name = next_config.job_name .. RolePromoteData.GetGradeName(next_config.order)
        UH.SetText(self.NowName,cur_config.job_name .. RolePromoteData.GetGradeName(cur_config.order))
        UH.SetText(self.TitleName,Format(Language.Role.PromoteOrderGradeTip,next_config.job_name))
        UH.SetText(self.Desc,Format(Language.Role.PromoteDescTip,next_name,next_config.suit_name))
        UH.SetText(self.SuitName,Format(Language.Role.PromoteOrderSuitTip,next_config.suit_name))
        UH.SetText(self.MaxGrade,Format(Language.Role.PromoteOrderMaxTip,max_grade))
        -- UH.SetText(self.SkillLevel,Format("Lv.%s",next_config.max_skill_level))
        UH.SetText(self.NextLevel,Format("Lv.%s",next_config.max_equip_level))
        UH.SetText(self.NextPower,next_config.power_up)
        UH.SetText(self.NextText3,next_config.shengwangka_up)
        UH.SetText(self.NextText4,next_config.juanzeng_up)
        local order_repress = Config.fight_other_auto.other[1].order_repress
        UH.SetText(self.ProfDesc,Format(Language.Role.PromoteProfDesc,order_repress,order_repress))
        for i=1,self.TaskCell:Length()do
            self.TaskCell[i]:SetData(cell_data[i] or {})
        end
        self:FlushAttrView(cur_config,now_config)
        self:FlushAddView(cur_config,now_config)
        self.suit_name = next_config.suit_name
        self.IsRolePromote = cur_config.job_id == now_config.job_id

        self.ObjSkillUp:SetActive(true)
        UH.SetText(self.NowSkill, Format(Language.Role.PromoteLevel, cur_config.max_skill_level))
        local next_advance_cfg = RolePromoteData.GetRoleProfNextConfig() or {}
        UH.SetText(self.NextSkill, Format(Language.Role.PromoteLevel, next_advance_cfg.max_skill_level or cur_config.max_skill_level))
        UH.SetText(self.BtnTex, now_config.order == 0 and Language.Role.TuPo or Language.Role.ShengJie)
    else
        UH.SetText(self.BtnTex, Language.Role.ShengJie)
        self:FlushMaxPanel()
    end
    self.MaxPanel:SetActive(next_config == nil)
    UH.SetText(self.TxtCoin, cur_config.need_bind_coin)
    self:FlushRemindView()
    self:FlushSkill()
end

-- 刷新技能
function RolePromotePanel:FlushSkill()
    local profession = RoleData.Instance:GetRoleProf()
	if profession == 0 then
		return
    end
    local config = SkillData.Instance.careerProfSkillList[profession]
    local skill_data = {}
    for i, v in ipairs(config[0]) do
        local now_skill_data = RoleCtrl.Instance.skill_data:getSkillDataById(v.skill_id) or {}
        -- 这里去除被动技能
        if now_skill_data.index2 ~= 0 then
            table.insert(skill_data, v)
        end
    end
    -- 排序
    for k, v in pairs(skill_data)do
        local skill_data = RoleCtrl.Instance.skill_data:getSkillDataById(v.skill_id)
        --未学习的多加100，为防止重复相加判断小于200
        if skill_data == nil then
            if v.index2 < 200 then
                v.index2 = v.index2 + 100 + v.skill_learn_level
            end
        else
            --已经学习了的，未设为常用的加
            if v.index2 == 99 then
                v.index2 = v.index2 + v.skill_learn_level - skill_data.skill_level
            end
        end
    end
    local sort_func = function (a,b)
        if a.index2 < b.index2 then
            return true
        elseif a.index2 == b.index2 then
            return a.skill_id < b.skill_id
        else
            return false
        end
    end
    table.sort(skill_data, sort_func)
    self.SkillList:SetData(skill_data)
end

--刷新满级
function RolePromotePanel:FlushMaxPanel()
    local prof_id = RoleData.Instance:GetRoleProfId()
    local prof_grade = RolePromoteData.Instance:GetProfGrade()
    local prof_type = RoleData.Instance:GetRoleProf()
    local cur_config = RolePromoteData.GetProfConfig(prof_id,prof_grade)
    local prof_name = Language.Common.Prof[prof_type]
    UH.SetText(self.Desc,Format(Language.Role.PromoteDescMaxTip,cur_config.suit_name,prof_name))
    UH.SetText(self.SuitName,Format(Language.Role.PromoteOrderSuitMaxTip,cur_config.suit_name))
    UH.SetText(self.SkillLevel,ColorStr(Format("Lv.%s",cur_config.max_skill_level),COLORSTR.Yellow15))
    UH.SetText(self.NextLevel,ColorStr(Format("Lv.%s",cur_config.max_equip_level),COLORSTR.Yellow15))
    UH.SetText(self.ProfTip,Format(Language.Role.PromoteOrderProfTip,prof_name))
    UH.SetText(self.NextPower,ColorStr(cur_config.power_up,COLORSTR.Yellow15))
    UH.SetText(self.NextText3,ColorStr(cur_config.shengwangka_up,COLORSTR.Yellow15))
    UH.SetText(self.NextText4,ColorStr(cur_config.juanzeng_up,COLORSTR.Yellow15))
    UH.SetText(self.TitleName,Language.Role.PromoteOrderGradeMaxTip)
    UH.SetText(self.MaxName[1],cur_config.job_name)
    UH.SetText(self.MaxName[2],cur_config.job_name)
    local order_repress = Config.career_advancements_auto.other[1].order_repress
    UH.SetText(self.ProfDesc,Format(Language.Role.PromoteProfDesc,order_repress,order_repress))
    self.AttrList:SetData(DataHelper.FormAttrList(cur_config))
    self.AddList:SetData(self.data:GetMaxAddList(cur_config))

    self.ObjSkillUp:SetActive(false)
    UH.SetText(self.NowSkill, Format(Language.Role.PromoteLevel, cur_config.max_skill_level))
    UH.SetText(self.NextSkill, Format(Language.Role.PromoteLevel, cur_config.max_skill_level))
end

function RolePromotePanel:FlushAttrView(cur_config,next_config)
    local attr_list = {}
    local cur_list = DataHelper.FormAttrList(cur_config,true)
    local next_list = DataHelper.FormAttrList(next_config,true)
    for k,v in pairs(cur_list) do
        if next_list[k] and next_list[k].attr_value ~= 0 then
            v.new_value = next_list[k].attr_value
            if v.new_value - v.attr_value ~= 0 then
                table.insert(attr_list,v)
            end
        end
    end
    self.AttrList:SetData(attr_list)
end

function RolePromotePanel:FlushAddView(cur_config,next_config)
    self.AddList:SetData(self.data:GetAddList(cur_config,next_config))
end

function RolePromotePanel:OnClickSelect(color)
    self.select_color = color
    self:FlushNextModel()
end

function RolePromotePanel:OnClickPromote()
    if self:GetTaskTip() then
        PublicPopupCtrl.Instance:Center(self:GetTaskTip())
        return
    end
    
    -- 判断铜币
    local prof_id = RoleData.Instance:GetRoleProfId()
    local prof_grade = RolePromoteData.Instance:GetProfGrade()
    local config = RolePromoteData.GetProfConfig(prof_id, prof_grade)
    if not MallCtrl.IsNotCurrency(CurrencyType.CoinBind, config.need_bind_coin) then
        return 
    end

    if self.IsRolePromote == true then
        RoleCtrl.Instance:SendRoleCareerAdvance(RoleData.Instance:GetRoleProfId() + 1,self.select_color)
        self:ShowRolePromoteEffect()
    else
        PublicPopupCtrl.Instance:AlertTip(Format(Language.Role.PromoteUpTip,self.suit_name),function()
            RoleCtrl.Instance:SendRoleCareerAdvance(RoleData.Instance:GetRoleProfId() + 1,self.select_color)
            -- self:ShowRolePromoteEffect()
        end)
    end
end

function RolePromotePanel:ShowRolePromoteEffect()
    UH.StopEffect(self.EffectTool,self.effect_handle1)
    self.effect_handle1 = UH.PlayEffect(self.EffectTool,[[6165122]])
    TimeHelper:CancelTimer(self.ShowTimer)
    self.ShowTimer = Invoke(function()
        UH.StopEffect(self.EffectTool,self.effect_handle3)
        self.effect_handle3 = UH.PlayEffect(self.EffectTool,[[6165124]])
        if not self.IsRolePromote then
            UH.StopEffect(self.EffectTool,self.effect_handle2)
            self.effect_handle2 = UH.PlayEffect(self.EffectTool,[[6165123]])
        end
    end)
end

function RolePromotePanel:OnClickHelp()
    ViewMgr:OpenView(RolePromoteHelpView)
end

function RolePromotePanel:GetTaskTip()
    for i=1,self.TaskCell:Length()do
        if self.TaskCell[i].data and self.TaskCell[i].data.target then
            return Language.Role.PromoteTaskTip[i]
        end
    end
end

--========================RolePromoteTaskItem========================
RolePromoteTaskItem = RolePromoteTaskItem or DeclareMono("RolePromoteTaskItem", UIWidgetBaseItem)

function RolePromoteTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local promote_tip = Language.Role.PromoteTip
    local promote_task = data.promote_task or Language.Role.PromoteTask[self.Index]
    UH.SetText(self.Desc,Format(promote_task, data.task_desc))
    UH.SetText(self.BtnTip,data.target and promote_tip[self.Index] or promote_tip[0])
    self.Finish:SetActive(not data.target)
end

function RolePromoteTaskItem:OnClick()
    if self.data.event then
        self.data.event()
    end
end

--========================RolePromoteAttrCall========================
RolePromoteAttrCall = RolePromoteAttrCall or DeclareMono("RolePromoteAttrCall", UIWidgetBaseItem)

function RolePromoteAttrCall:SetData(data)
    local old_value = data.attr_value or 0
    local new_value = data.new_value or 0

    if DataHelper.IsPercent(data.attr_type) then
		old_value = Percent(old_value)
		new_value = Percent(new_value)
    end
    UH.SetText(self.Name,data.attr_name)
    UH.SetText(self.OldNum,old_value)
    UH.SetText(self.NewNum,new_value)

    if data.new_value == nil then
        UH.SetText(self.NewNum,ColorStr(old_value,COLORSTR.Yellow15))
        self.Image:SetActive(false)
        UH.SetText(self.OldNum)
    else
        self.Image:SetActive(true)
    end
end

--========================RolePromoteAddCall========================
RolePromoteAddCall = RolePromoteAddCall or DeclareMono("RolePromoteAddCall", UIWidgetBaseItem)

function RolePromoteAddCall:SetData(data)
    UH.SetText(self.Name,data.name)
    local old_value = Percent(data.old_value or 0)
    local new_value = Percent(data.new_value or 0)
    UH.SetText(self.OldNum,old_value)
    UH.SetText(self.NewNum,new_value)

    if data.new_value == nil then
        UH.SetText(self.NewNum,ColorStr(old_value,COLORSTR.Yellow15))
        self.Image:SetActive(false)
        UH.SetText(self.OldNum)
    end
end

--========================RolePromoteMaxCall========================
RolePromoteMaxCall = RolePromoteMaxCall or DeclareMono("RolePromoteMaxCall", UIWidgetBaseItem)

function RolePromoteMaxCall:SetData(data)
    UH.SetText(self.Name,data.name)
    UH.SetText(self.Num,data.new_value)
end

--========================RolePromoteSkillClickItem========================
RolePromoteSkillClickItem = RolePromoteSkillClickItem or DeclareMono("RolePromoteSkillClickItem", UIWidgetBaseItem)

function RolePromoteSkillClickItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local skill_cfg = Cfg.SkillDescPartnerAndRole(self.data.skill_id, 1)
    if skill_cfg then
        UH.SetIcon(self.SkillIcon, skill_cfg.icon_id, ICON_TYPE.SKILL)
    end
    local is_learn = not TableIsEmpty(RoleCtrl.Instance.skill_data:getSkillDataById(data.skill_id))
    self.InteractorHandler.Interactable = is_learn
    self.ObjLimit:SetActive(not is_learn)
    if not is_learn then
        local is_limit, limitType, limitStr = SkillData.Instance:GetSkillLimit(data.skill_id)
        if is_limit and limitType == RoleSkillLimitType.RoleLevel then
            UH.SetText(self.TxtLimit, Format(Language.Role.PromoteClickSkill[0], limitStr))
        else
            UH.SetText(self.TxtLimit, Language.Role.PromoteClickSkill[1])
        end
    end
end

function RolePromoteSkillClickItem:onClick()
    ViewMgr:OpenView(RoleSkillInfoView, self.data)
end