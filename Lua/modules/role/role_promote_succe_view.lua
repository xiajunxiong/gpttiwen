RolePromoteSucceView = RolePromoteSucceView or DeclareView("RolePromoteSucceView", "role/role_promote_succe_view")

VIEW_DECLARE_LEVEL(RolePromoteSucceView, ViewLevel.L0)
VIEW_DECLARE_MASK(RolePromoteSucceView, ViewMask.BgBlock)

function RolePromoteSucceView:RolePromoteSucceView()
    self.data = RolePromoteData.Instance
    self.ObjGuideClose:SetActive(false)
    -- 延迟5秒后才能关闭
    self.ht = Invoke(function()
        self.ObjGuideClose:SetActive(true)
    end, 5)
    GuideManager.Instance:RegisterGetGuideUi("RolePromoteSucceCloseBtn",function ()
        return self.RolePromoteSucceClose,function ()
            ViewMgr:CloseView(RolePromoteSucceView)
        end 
    end)
    self.handle_effects = {}
    self.handle_times = {}
    self.point_sp = {
        [1] = {"JieDian1","JieDian1_JiHuo"},
        [2] = {"JieDian2","JieDian2_JiHuo"},
    }
    self.grade_group = {
        [1] = 5,
        [2-6] = 9, 
    }
end

function RolePromoteSucceView:LoadCallback(param_t)
    self.last_appearance = param_t.last_appearance
    self.cur_appearance = param_t.cur_appearance

    if self.cur_ui_obj == nil then
        self.cur_ui_obj = UIChDrawer.CreateByAppearance(self.cur_appearance)
        self.ModelShow2:SetShowData({ui_obj = self.cur_ui_obj,view = self})
    else
        self.cur_ui_obj:SetData(self.cur_appearance)
    end
    
    if self.last_ui_obj == nil then
        self.last_ui_obj = UIChDrawer.CreateByAppearance(self.last_appearance)
        self.ModelShow1:SetShowData({ui_obj = self.last_ui_obj,view = self})
    else
        self.last_ui_obj:SetData(self.last_appearance)
    end

    local prof_id = RoleData.Instance:GetRoleProfId()
    local prof_grade = RolePromoteData.Instance:GetProfGrade()
    local cur_config = RolePromoteData.GetProfConfig(prof_id,prof_grade)
    local last_config = RolePromoteData.GetLastSmallProfConfig()
    if TableIsEmpty(last_config) then
        last_config = cur_config
    end
    local cur_stage = RoleData.Instance:ProfStage()    
    -- cur_stage = 5
    self.cur_stage = cur_stage

    local last_stage = cur_stage - 1

    -- local  prof_grade = 2

    local isBig1 = cur_stage == 2 and prof_grade == 0
    local isBig2 = cur_stage > 2  and prof_grade == 0 --就是大进阶
    UH.SpriteName(self.TitleSp, (isBig1 or isBig2) and  "_LocGongXiChengGongTuPoZi" or "_LocGongXiChengGongShengJieZi")

    self.Group1:SetActive(isBig1 or cur_stage == 1)
    self.GroupOther:SetActive(not isBig1 and cur_stage > 1)

    local grade = prof_grade + 1 
    if isBig1 then
        grade = 5
    end
    if isBig2 then
        grade = 9
    end
    -- LogDG(isBig1, isBig2, last_stage, grade)
    if isBig1 or cur_stage == 1 then
        for i = 1, 5 do
            UH.SpriteName(self.PointList1[i],  "JieDian1")
        end
        if isBig1 then
            self:StartPlayPoint1Effect(grade)
        else
            local delay = 3 - (0.3 * grade)
            -- 3秒前完成点亮计算需要延迟
            local handle = TimeHelper:AddDelayTimer(BindTool.Bind(self.StartPlayPoint1Effect, self, grade), delay)
            table.insert(self.handle_times, handle)
        end
    else
        for i = 1, 9 do
            UH.SpriteName(self.PointListOther[i], Format("JieDian%d", cur_stage))
        end
        if isBig2 then
            self:StartPlayPoint2Effect(grade)
        else
            local delay = 3 - (0.3 * grade)
            -- 3秒前完成点亮计算需要延迟
            local handle = TimeHelper:AddDelayTimer(BindTool.Bind(self.StartPlayPoint2Effect, self, grade), delay)
            table.insert(self.handle_times, handle)
        end
    end
    local delay = 3.3
    if isBig1 or isBig2 then
        delay = (grade + 2) * 0.3
    end
    --7165089
    local handle = TimeHelper:AddDelayTimer(function ()
        if isBig2 then
            self.GroupOther:SetActive(false)
            self:EndPointShow(cur_stage, prof_grade)
            self.EffectB2:SetActive(true)
            self:RotationPoint(2.4, 2)
        elseif isBig1 then
            self.Group1:SetActive(false)
            self:EndPointShow(cur_stage, prof_grade)
            self.EffectB1:SetActive(true)
            self:RotationPoint(2.4, 1)
        else
            self.EffectS:SetActive(true)
            self:EndPointShow(cur_stage, prof_grade)
        end
    end, delay)
    table.insert(self.handle_times, handle)
    if not isBig2 and not isBig1 then
        self.Effect1:SetActive(true)
        self:RotationPoint(4, 0)
    end
    -- UH.SetText(self.SkillLevel,Format("Lv.%s",cur_config.max_skill_level))
    UH.SetText(self.NextLevel,Format("Lv.%s",cur_config.max_equip_level))
    UH.SetText(self.NextPower,cur_config.power_up)
    UH.SetText(self.SuitName,cur_config.suit_name)
    UH.SetText(self.Name,cur_config.job_name .. RolePromoteData.GetGradeName(cur_config.order))
    UH.SetText(self.Desc,cur_config.job_desc)
    UH.SetText(self.NextText3,cur_config.shengwangka_up)
    UH.SetText(self.NextText4,cur_config.juanzeng_up)

    self:FlushAttrView(last_config,cur_config)
    self:FlushAddView(last_config,cur_config)

    -- 只有大晋阶才显示这个
    self.ObjBigUpShow:SetActive(RolePromoteData.Instance:GetProfGrade() == 0)

    UH.SetText(self.NowSkill, Format(Language.Role.PromoteLevel, cur_config.max_skill_level))
    local last_advance_cfg = RolePromoteData.GetRoleProfLastConfig() or {}
    UH.SetText(self.LastSkill, Format(Language.Role.PromoteLevel, last_advance_cfg.max_skill_level or cur_config.max_skill_level))
    self:FlushSkill()
end
function RolePromoteSucceView:RotationPoint(time, isBig2)
    local handle = TimeHelper:AddDelayTimer(function ()
        if isBig2 == 2 then
            self.GroupOther:SetActive(true)
            self.RotationOther.enabled = true
        elseif isBig2 == 1 then
            self.GroupOther:SetActive(true)
            self.RotationOther.enabled = true
        else
            self.RotationOther.enabled = true
            self.Rotation1.enabled = true
        end
    end, time)
    table.insert(self.handle_times, handle)
end
function RolePromoteSucceView:StartPlayPoint1Effect(grade)
    local total = grade * 0.3
    local index = 1
    local handle = TimeHelper:AddCountDownTT(function ()
        local eff_id = index == grade and 7165085 or 7165086
        if self.PointList1[index] and index <= grade then
            self:PlayPointEffect(eff_id, self.PointList1[index].gameObject)
            UH.SpriteName(self.PointList1[index], "JieDian1_JiHuo")
        end
        index = index + 1
    end,function ()
    end, total, 0.3, false)
    table.insert(self.handle_times, handle)
end
function RolePromoteSucceView:StartPlayPoint2Effect(grade)
    local total = grade * 0.3
    local index = 1
    local handle = TimeHelper:AddCountDownTT(function ()
        local eff_id = index == grade and 7165085 or 7165086
        if self.PointListOther[index] and index <= grade then
            self:PlayPointEffect(eff_id, self.PointListOther[index].gameObject)
            UH.SpriteName(self.PointListOther[index], Format("JieDian%d_JiHuo", self.cur_stage))
        end
        index = index + 1
    end,function ()
    end, total, 0.3, false)
    table.insert(self.handle_times, handle)
end
function RolePromoteSucceView:EndPointShow(stage, grade)
    local cur_grade = grade + 1
    if stage > 1 then
        for i = 1, 9 do
            UH.SpriteName(self.PointListOther[i], (i <= cur_grade) and Format("JieDian%d_JiHuo", stage) or Format("JieDian%d", stage))
            if (i <= cur_grade) then
                self:PlayPointStayEffect(self.PointListOther[i].gameObject)
            end
        end
    else
        for i = 1, 5 do
            UH.SpriteName(self.PointList1[i],  (i <= cur_grade) and "JieDian1_JiHuo" or "JieDian1")
            if (i <= cur_grade) then
                self:PlayPointStayEffect(self.PointList1[i].gameObject)
            end
        end
    end
end
function RolePromoteSucceView:PlayPointEffect(eff_id, parent)
    local handle = self.EffectTool:Play(eff_id, parent)
    table.insert(self.handle_effects, handle)
end
function RolePromoteSucceView:PlayPointStayEffect(parent)
    local handle = self.EffectTool:Play(7165089, parent)
    table.insert(self.handle_effects, handle)
end

function RolePromoteSucceView:FlushAttrView(cur_config,next_config)
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

function RolePromoteSucceView:FlushAddView(cur_config,next_config)
    self.AddList:SetData(self.data:GetAddList(cur_config,next_config))
end

function RolePromoteSucceView:CloseCallback()
    Delete(self,[[cur_ui_obj]])
    Delete(self,[[last_ui_obj]])
    TimeHelper:CancelTimer(self.ht)
    for i, v in ipairs(self.handle_effects) do
        self.EffectTool:Stop(v)
    end
    self.handle_effects = nil
    for i, v in ipairs(self.handle_times) do
        TimeHelper:CancelTimer(v)
    end
    self.handle_times = nil
end

function RolePromoteSucceView:OnClickClose()
    ViewMgr:CloseView(RolePromoteSucceView)
end

-- 刷新技能
function RolePromoteSucceView:FlushSkill()
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