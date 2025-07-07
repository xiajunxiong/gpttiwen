GuildInborn = GuildInborn or DeclareMono("GuildInborn", UIWFlushPanel)
function GuildInborn:GuildInborn()
    self.data = GuildData.Instance
    self.lang = Language.Guild
    self.data_cares = {
        { data = self.data:SmartInbornInfo(), func = self.FlushView, init_call = true },
        { data = BagData.Instance.item_grid, func = self.FlushView, init_call = false},
    }

    self.ToggleRemind.isOn = self.data:IsInbornRemind()
    self.is_in_effect = false
end

function GuildInborn:OnDestroy()
    TimeHelper:CancelTimer(self.move_handle)
    UIWFlushPanel.OnDestroy(self)
end

function GuildInborn:OnClickTip()
end

function GuildInborn:onFlush()
    self.PageList:SetData(self.data:GetInbornPageList())
    self.PageList:ClickItem(1)
    self.cur_inborn_type = 0
end

function GuildInborn:FlushView()
    if not self.is_in_effect then
        self.PageList:SetData(self.data:GetInbornPageList())
        local show_list = self.data:GetInbornShowList(self.cur_inborn_type or 0)
        self.SkillList:SetData(show_list)
        -- self.LineList:SetData(show_list)
        self:FlushInfo()
    end
end

function GuildInborn:OnClickPageItem(isOn, data)
    self.cur_inborn_type = data.inborn_type or 0
    local show_list = self.data:GetInbornShowList(self.cur_inborn_type)
    self.SkillList:SetData(show_list)
    -- self.LineList:SetData(show_list)
    if self.SkillList:GetItem(1).Toggle.isOn then
        self:OnClickSkillItem(show_list[1])
    else
        self.SkillList:ClickItem(1)
    end
end

function GuildInborn:OnClickSkillItem(data)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    self.select_data = data
    self.data:SetInbornSelectSkill(self.select_data.inborn_type, self.select_data.type_seq)
    self:FlushInfo()
end

function GuildInborn:FlushInfo()
    if self.select_data == nil then
        return
    end
    local data = self.select_data
    local skill_level = GuildData.Instance:GetInbornSkillLevel(data.inborn_type, data.type_seq)
    local next_cfg = GuildData.Instance:GetInbornSkillCfg(data.inborn_type, data.type_seq, skill_level + 1)
    local is_max = next_cfg == nil

    local skill_cfg = SkillData.Instance:GetInbornSkillDesc(data.inborn_skill, math.max(skill_level, 1)) or {}
    local level_str = Format(Language.Guild.GuildInbornLv, math.max(skill_level, 1))
    UH.SetText(self.NameText, skill_cfg.skill_name .. ColorStr(level_str, COLORSTR.Red9))
    UH.SetText(self.DescText, skill_cfg.description)

    self.SkillIcon:SetData(self.select_data)
    self.CanNode:SetActive(not is_max)
    self.MaxNode:SetActive(is_max)
    if not is_max then
        UH.SetText(self.LevelText, next_cfg.level_limit)
        UH.SetText(self.CostText, next_cfg.family_level_limit)
        -- UH.SetText(self.BtnLearnText, is_max and self.lang.GuildInbornLearn[2] or self.lang.GuildInbornLearn[1])
        if skill_level == 0 then
            UH.SetText(self.BtnLearnText, self.lang.GuildInbornLearn[1])
        else
            UH.SetText(self.BtnLearnText, self.lang.GuildInbornLearn[3])
        end

        UH.SetInter(self.BtnLearnInter, not is_max)

        local item_data = next_cfg.inborn_consume_item[0]
        local item = Item.Create(item_data)
        self.ItemCell:SetData(item)
        UH.SetText(self.ItemName, item:Name())
        UH.SetText(self.ItemNum, DataHelper.ConsumeNum(Item.GetNum(item_data.item_id), item_data.num))
    end
    UH.SetInter(self.BtnResetInter, self.data:IsInbornSkillActive(data.inborn_type, data.type_seq))
    -- UH.SetText(self.HaveText, string.format(self.lang.GuildInbornOwn, self.data:InbornRemainSkillPoints()))
end

function GuildInborn:OnClickLearn()
    if self.select_data == nil then
        return
    end
    local skill_level = GuildData.Instance:GetInbornSkillLevel(self.select_data.inborn_type, self.select_data.type_seq)
    local next_cfg = GuildData.Instance:GetInbornSkillCfg(self.select_data.inborn_type, self.select_data.type_seq, skill_level + 1)
    local is_max = next_cfg == nil
    -- 满级，前置未激活，等级不足，家族不足，材料不足
    if next_cfg == nil then
        PublicPopupCtrl.Instance:Center(self.lang.GuildInbornError6)
        return
    end
    if next_cfg.prepose_seq ~= -1 and not  self.data:IsInbornSkillActive(self.select_data.inborn_type, next_cfg.prepose_seq) then
        PublicPopupCtrl.Instance:Center(self.lang.GuildInbornError3)
        return
    end
    if next_cfg.level_limit > RoleData.Instance:GetRoleLevel() then
        PublicPopupCtrl.Instance:Center(string.format(self.lang.GuildInbornError1, next_cfg.level_limit))
        return
    end
    if next_cfg.family_level_limit > GuildData.Instance.guild_info.level then
        PublicPopupCtrl.Instance:Center(string.format(self.lang.GuildInbornError7, next_cfg.family_level_limit))
        return
    end
    local stuff_cfg = next_cfg.inborn_consume_item[0]
    if not MallCtrl.IsNotCurrency(nil, stuff_cfg.num, stuff_cfg.item_id) then
        return
    end
    GuildCtrl.Instance:SendGuildInbornReq(GUILD_INBORN_REQ_TYPE.TYPE_LEARN_SKILL, self.select_data.inborn_type, self.select_data.type_seq)
    self:PlayEffect()
end

function GuildInborn:OnClickReset()
    if not self.data:IsInbornSkillActive(self.select_data.inborn_type, self.select_data.type_seq) then
        PublicPopupCtrl.Instance:Center(self.lang.GuildInbornError4)
        return
    end
    ViewMgr:OpenView(GuildInbornResetView, { data = self.select_data })
end

function GuildInborn:OnClickToggle(param)
    self.data:SetIsInbornRemind(param)
    self:FlushView()
end

function GuildInborn:PlayEffect()
    local skill_level = GuildData.Instance:GetInbornSkillLevel(self.select_data.inborn_type, self.select_data.type_seq)
    self.up_param_t = {
        skill_level = skill_level,
        inborn_type = self.select_data.inborn_type,
        type_seq = self.select_data.type_seq,
        inborn_skill = self.select_data.inborn_skill
    }

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillGetNew)
    self.is_in_effect = true
    self.cur_step = 1
    self.default_step = 20
    TimeHelper:CancelTimer(self.move_handle)
    local start_pos = UH.GetRootPosition(self.BtnLearnInter.gameObject)
    local end_pos = UH.GetRootPosition(self.SkillList:GetItem(self.select_data.skill_reorder).gameObject)
    UH.LocalPosG(self.ActiveEffects[1], start_pos)
    UH.LocalPosG(self.ActiveEffects[2], end_pos)

    self.ActiveEffectNode:SetActive(true)
    self.ActiveEffects[1]:SetActive(true)
    self.ActiveEffects[2]:SetActive(false)

    self.curve = Game.CurveParabola(start_pos, Vector3.New(-1, 2, 0), end_pos, Vector3.New(-1, -0.5, 0))
    self.move_handle = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.RunEffect, self), 1, self.default_step)
end

function GuildInborn:RunEffect()
    local pos = self.curve:GetPos(self.cur_step / self.default_step)
    UH.LocalPosG(self.ActiveEffects[1], pos)
    self.cur_step = self.cur_step + 1
    if self.cur_step > self.default_step then
        TimeHelper:CancelTimer(self.move_handle)
        self.ActiveEffects[1]:SetActive(false)
        self.ActiveEffects[2]:SetActive(true)
        local function func_end()
            self.is_in_effect = false
            self.ActiveEffects[2]:SetActive(false)
            self.ActiveEffectNode:SetActive(false)
            self:FlushView()
        end
        self.move_handle = TimeHelper:AddDelayTimer(func_end, 0.6)

        if self.up_param_t ~= nil then
            ViewMgr:OpenView(GuildInbornUpView, self.up_param_t)
        end
    end
end

function GuildInborn:OnClickDesc()
    local data = self.select_data
    if data == nil then return end

    local desc = ""
    local level_list = GuildData.Instance:GetInbornSkillLevelListByType(data.inborn_type, data.type_seq)
    for i, level_cfg in ipairs(level_list or {}) do
        local skill_cfg = SkillData.Instance:GetInbornSkillDesc(level_cfg.inborn_skill, level_cfg.skill_level)
        if skill_cfg ~= nil then
            if i ~= 1 then
                desc = desc .. "\n"
            end
            desc = desc .. Format(Language.Guild.GuildInbornLevelTitle, skill_cfg.skill_name, level_cfg.skill_level)
            desc = desc .. skill_cfg.description
        end
    end
    if desc ~= "" then
        PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = desc, slide_size = Vector2.New(684, 500)})
    end
end

--技能
GuildInbornSkillItem = GuildInbornSkillItem or DeclareMono("GuildInbornSkillItem", UIWidgetBaseItem)
local skill_pos = {
    [1] = Vector2.New(-449, -145),
    [2] = Vector2.New(-420, 122),
    [3] = Vector2.New(-309, 5),
    [4] = Vector2.New(-215, -122),
    [5] = Vector2.New(-52, -32),
    [6] = Vector2.New(-98, 104)
}
function GuildInbornSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local skill_level = GuildData.Instance:GetInbornSkillLevel(data.inborn_type, data.type_seq)
    local skill_cfg = SkillData.Instance:GetInbornSkillDesc(self.data.inborn_skill, math.max(skill_level, 1)) or {}
    UH.SetIcon(self.Icon, skill_cfg.icon_id, ICON_TYPE.SKILL)
    self.Grade:SetActive(data.grade == 1)

    local is_lock = false
    local is_active = GuildData.Instance:IsInbornSkillActive(data.inborn_type, data.type_seq)
    if not is_active then
        if data.level_limit > RoleData.Instance:GetRoleLevel() then
            is_lock = true
        end
        if not is_lock and data.prepose_seq ~= -1 and not GuildData.Instance:IsInbornSkillActive(data.inborn_type, data.prepose_seq) then
            is_lock = true
        end
    end
    self.Lock:SetActive(is_lock)
    self.Mask:SetActive(not is_active)
    self.Actived:SetActive(is_active)
    UH.SetInter(self.IconInter, is_active)

    local pos = skill_pos[self.data.skill_reorder]
    UH.LocalPosG(self.gameObject, Vector3.New(pos.x, pos.y, 0))

    local level = GuildData.Instance:GetInbornSkillLevel(data.inborn_type, data.type_seq)
    UH.SetText(self.LevelText, level)

    local red_num = GuildData.Instance:InbornSkillRedNum(data.inborn_type, data.type_seq)
    self.RedNum:SetNum(red_num)
end

function GuildInbornSkillItem:Click()
    if self.ToggleClick then
        self.ToggleClick:Trigger()
    end
end

GuildInbornLineItem = GuildInbornLineItem or DeclareMono("GuildInbornLineItem", UIWidgetBaseItem)
function GuildInbornLineItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local pos = skill_pos[self.data.skill_reorder]
    UH.LocalPosG(self.gameObject, Vector3.New(pos.x, pos.y, 0))

    self.Line:SetActive(self.data.prepose_seq ~= -1)
    if self.data.prepose_seq ~= -1 then
        local before_cfg = GuildData.Instance:GetInbornSkillCfg(self.data.inborn_type, self.data.prepose_seq, 1)
        local before_pos = skill_pos[before_cfg.skill_reorder]
        local dis_pos = Vector2.New(before_pos.x - pos.x, before_pos.y - pos.y)
        UH.SizeDelta(self.Line, Vector2.New(math.sqrt(dis_pos.x * dis_pos.x + dis_pos.y * dis_pos.y), 10))
        local angle = math.atan2(dis_pos.y, dis_pos.x) * 180 / math.pi
        self.Line:SetLocalEulerAngles(Vector3.New(0, 0, angle))
    end
end

--标签页
GuildInbornPageItem = GuildInbornPageItem or DeclareMono("GuildInbornPageItem", UIWidgetBaseItem)
function GuildInbornPageItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name1, data.name_t[1])
    UH.SetText(self.Name2, data.name_t[2])

    local red_num = GuildData.Instance:InbornPageRedNum(data.inborn_type)
    self.RedNum:SetNum(red_num)
end

function GuildInbornPageItem:Click()
    if self.ToggleClick then
        self.ToggleClick:Trigger()
    end
end

--图标
GuildInbornIcon = GuildInbornIcon or DeclareMono("GuildInbornIcon", UIWidgetBaseItem)
function GuildInbornIcon:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local skill_level = GuildData.Instance:GetInbornSkillLevel(data.inborn_type, data.type_seq)
    local skill_cfg = SkillData.Instance:GetInbornSkillDesc(self.data.inborn_skill, math.max(skill_level, 1)) or {}
    UH.SetIcon(self.Icon, skill_cfg.icon_id, ICON_TYPE.SKILL)
    self.Grade:SetActive(data.grade == 1)
    UH.SetText(self.LevelText, math.max(skill_level, 1))

    local is_lock = false
    local is_active = GuildData.Instance:IsInbornSkillActive(data.inborn_type, data.type_seq)
    -- if not is_active then
    --     if data.level_limit > RoleData.Instance:GetRoleLevel() then
    --         is_lock = true
    --     end
    --     if not is_lock and data.prepose_seq ~= -1 and not GuildData.Instance:IsInbornSkillActive(data.inborn_type, data.prepose_seq) then
    --         is_lock = true
    --     end
    -- end
    -- self.Lock:SetActive(is_lock)
    self.Mask:SetActive(not is_active)
    self.Actived:SetActive(is_active)
end
