LiveUpLevelView = LiveUpLevelView or DeclareView("LiveUpLevelView","live/live_up_level_view")
function LiveUpLevelView:LiveUpLevelView()
    local cfg = LiveData.Instance:GetSkillTypeCfg() or {}
    local title = string.format(Language.Live.Txt8,cfg.name)
    self.BG:SetData(self:GetType(),title,Vector2.New(489, 588))
end

LiveUpLevelPanel = LiveUpLevelPanel or DeclareMono("LiveUpLevelPanel", UIWFlushPanel)
function LiveUpLevelPanel:LiveUpLevelPanel()
    self.data = LiveData.Instance
    self.data_cares = {
        {data = self.data.skill_all_info, func = self.FlushPanel, init_call = false},
    }
end

function LiveUpLevelPanel:Awake()
    UIWFlushPanel.Awake(self)
    local skill_type = self.data:GetSelSkillType()
    self.NextAddObj:SetActive(skill_type < 4)
    local pos_y = skill_type < 4 and 28.5 or 56
    self.NextInfo:SetLocalPosition(Vector3.New(0,pos_y,0))
    self:FlushPanel()
end

function LiveUpLevelPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data.other_data.height = 0
    self.data.other_data.left_data = {}
    TimeHelper:CancelTimer(self.time_ht)
end

function LiveUpLevelPanel:FlushPanel()
    local skill_type = self.data:GetSelSkillType()
    local skill_level = self.data:GetSkillTypeLevel(skill_type)
    local max_level = self.data:GetMaxLevel()
    if skill_level >= max_level then
        PublicPopupCtrl.Instance:Center(Language.Live.Tip9)
        ViewMgr:CloseView(LiveUpLevelView)
        return
    end
    local next_data = self.data:GetNextUnlockData(skill_type)
    self.next_data = next_data
    local skill_cfg = self.data:GetSkillTypeCfg(skill_type)
    if not skill_cfg then
        BuglyReport("生活技能配置为空 skill_type === ", skill_type)
        ViewMgr:CloseView(LiveUpLevelView)
        return
    end
    local skill_limit_level = self.data:LimitMaxLevel()
    self.Fish:SetActive(not next_data)
    UH.SpriteName(self.Icon, "Skill" .. skill_type)
    UH.SetText(self.SkillName, skill_cfg.name)
    UH.SetText(self.SkillLevel, Format(Language.Live.Txt9,skill_level, skill_limit_level))
    UH.SetText(self.LimitLevel, Format(Language.Live.Txt10,skill_limit_level))
    local rate = self.data:GetLevelAdditionCfg(skill_level + 1).pro_double_rate or 0
    rate = rate / 10 .. "%"
    rate = ColorStr(rate, COLORSTR.Green4)
    UH.SetText(self.Tip1, string.format(Language.Live.Txt11, rate))
    if not next_data then
        UH.SetText(self.Tip2, Language.Live.Tip10)
        self.List:SetData({})
    else
        local level_txt = ColorStr(next_data.level, COLORSTR.Green4)
        UH.SetText(self.Tip2, string.format(Language.Live.Txt12,level_txt))
        self.List:SetData(next_data.list_data)
    end

    local curr_num = RoleData.Instance:GetCurrency(CurrencyType.GuildCotribution)
    local need_curr_num = self.data:GetNeedCurrNum(skill_type, skill_level)
    if curr_num > 1000000 then
        if curr_num > need_curr_num then
            curr_num = ColorStr(DataHelper.ConverMoney(curr_num), COLORSTR.Green4)
        else
            curr_num = DataHelper.ConverMoney(curr_num)
        end
        UH.SetText(self.StuffNum, string.format("%s/%s",curr_num,need_curr_num))
    else
        UH.SetText(self.StuffNum, DataHelper.ConsumeNum(curr_num, need_curr_num, true))
    end
end

function LiveUpLevelPanel:OnUpClick()
    if self.uping then
        return
    end
    local skill_type = self.data:GetSelSkillType()
    local flag = self.data:GetSkillUpFlag(skill_type)
    if flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.Live.Tip4)
        return
    end
    if flag == -2 then
        PublicPopupCtrl.Instance:Error(Language.Live.Tip13)
        return
    end
    LiveCtrl.Instance:SendSkillReq(LiveCtrl.LIVE_SKILL_OP_TYPE.SKILl_LIFE_LEVEL_UP, skill_type)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
    self.Effect:SetActive(true)
    self.uping = true
    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        self.uping = false
        self.Effect:SetActive(false)
    end,0.5)

    --检查下此次升级是否有跳转
    local level = self.data:GetSkillTypeLevel(skill_type)
    -- 防止快点
    if self.last_level and self.last_level == level then
        return
    end
    self.last_level = level
    level = level + 1
    local list_data = self.data:GetContentLeftListData(skill_type)
    local jump_index
    for i,v in pairs(list_data) do
        if level == v.level then
            jump_index = i
            self.data.other_data.left_data = v
            break
        end
    end
    if jump_index and jump_index > 1 then
        local height = self:GetJumpHeight(jump_index,list_data)
        self.data.other_data.height = height
        self.data.other_data:Notify()
    end
end

function LiveUpLevelPanel:GetJumpHeight(jump_index, list_data)
    jump_index = jump_index > #list_data - 2 and #list_data - 2 or jump_index
    local h = 0
    for i = 1, jump_index - 1 do
        local data = list_data[i]
        if #data.list_data > 4 then
            h = h + 150 + (math.floor(#data.list_data / 4) * 100)
        else
            h = h + 150
        end
    end
    return h
end

LiveUpLevelItemCell = LiveUpLevelItemCell or DeclareMono("LiveUpLevelItemCell", UIWidgetBaseItem)
-- data : {item_id,cfg}
function LiveUpLevelItemCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data.item_id})
    self.ItemCell:SetData(item)
    UH.SetText(self.Name, item:Name())
end