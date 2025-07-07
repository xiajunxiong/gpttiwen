-- 圣器技能库
ShengQiSkillPoolView = ShengQiSkillPoolView or DeclareView("ShengQiSkillPoolView", "sheng_qi/sheng_qi_skill_pool_view")
function ShengQiSkillPoolView:LoadCallback(param)
    param = param or {}
    self.Panel:SetPanel(param)
    local title = param.tab_type == 1 and Language.ShengQi.Title9 or Language.ShengQi.Title5
    self.Board:SetData(self:GetType(),title,Vector2.New(971, 603))
end

ShengQiSkillPoolPanel = ShengQiSkillPoolPanel or DeclareMono("ShengQiSkillPoolPanel", UIWFlushPanel)
function ShengQiSkillPoolPanel:ShengQiSkillPoolPanel()
    self.data = ShengQiData.Instance
    self.tab_list = {
        {text = Language.ShengQi.SkillType[2], type = SHENG_QI_SKILL_TYPE.TALENT},
        {text = Language.ShengQi.SkillType[1], type = SHENG_QI_SKILL_TYPE.INITIATIVE},
    }

    self.cfg = {
        [SHENG_QI_SKILL_TYPE.TALENT] = {
            {text = Language.ShengQi.Text6, type = ShengQiCfg.TalentType.General},
            {text = Language.ShengQi.Text7, type = ShengQiCfg.TalentType.Advanced},
            {text = Language.ShengQi.Text21, type = ShengQiCfg.TalentType.SP},
        },
        -- [SHENG_QI_SKILL_TYPE.INITIATIVE] = {
        --     {text = Language.ShengQi.TypeName[1], type = ShengQiCfg.InitativeType.Attack},
        --     {text = Language.ShengQi.TypeName[2], type = ShengQiCfg.InitativeType.Help},
        --     {text = Language.ShengQi.TypeName[3], type = ShengQiCfg.InitativeType.Shoot},
        -- },
        [SHENG_QI_SKILL_TYPE.INITIATIVE] = {
            {text = Language.ShengQi.SkillQuaTypeT[4], type = 4},
            {text = Language.ShengQi.SkillQuaTypeT[5], type = 5},
            {text = Language.ShengQi.SkillQuaTypeT[6], type = 6},
        },
    }
    self.tab_data = self.tab_list[2]
    self.type_data = self.cfg[self.tab_data.type][1]

    ShengQiSkillPoolPanel.SelData = nil
end

function ShengQiSkillPoolPanel:SetPanel(param)
    local tab_type = param.tab_type or 2
    self:OnTabClick(self.tab_list[tab_type])
    self:FlushPanel()
end

function ShengQiSkillPoolPanel:Awake()
    UIWFlushPanel.Awake(self)
    -- self.TabList:SetData(self.tab_list)
    -- self.TabList:SetectItem(1,true)
end

function ShengQiSkillPoolPanel:FlushPanel()
    self:FlushList()
end

function ShengQiSkillPoolPanel:FlushList()
    if TableIsEmpty(self.tab_data) or TableIsEmpty(self.type_data) then
        return
    end
    local list_data
    if self.tab_data.type == SHENG_QI_SKILL_TYPE.TALENT then
        list_data = self.data:GetTalentSkillPool(self.type_data.type)
    elseif self.tab_data.type == SHENG_QI_SKILL_TYPE.INITIATIVE then
        local cfg = self.data:GetSkillGroupByQua(self.type_data.type)
        list_data = {}
        for i,v in pairs(cfg) do
            local skill = ShengQiSkill.New({skill_id = v.skill_id, skill_type = SHENG_QI_SKILL_TYPE.INITIATIVE, level = v.combo_level})
            table.insert(list_data, skill)
        end
    end

    if not ShengQiSkillPoolPanel.SelData and #list_data > 0 then
        ShengQiSkillPoolPanel.SelData = list_data[1]
    end
    self:FlushInfo()
    self.SkillList:SetData(list_data)
end

function ShengQiSkillPoolPanel:FlushInfo()
    if ShengQiSkillPoolPanel.SelData then
        self.SkillInfo:SetSkill(ShengQiSkillPoolPanel.SelData)
    end
end

function ShengQiSkillPoolPanel:OnTabClick(data)
    self.tab_data = data
    self.TypeList:SetData(self.cfg[self.tab_data.type])
    self.type_data = self.cfg[self.tab_data.type][1]
    self.TypeList:SetectItem(1,true)
    ShengQiSkillPoolPanel.SelData = nil
    self:FlushList()
end

function ShengQiSkillPoolPanel:OnTypeClick(data)
    self.type_data = data
    ShengQiSkillPoolPanel.SelData = nil
    self:FlushList()
end

function ShengQiSkillPoolPanel:OnCellClick(data)
    ShengQiSkillPoolPanel.SelData = data
    self:FlushList()
end

ShengQiSkillPoolSkillItem = ShengQiSkillPoolSkillItem or DeclareMono("ShengQiSkillPoolSkillItem", UIWidgetBaseItem)
function ShengQiSkillPoolSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.SelImg:SetActive(ShengQiSkillPoolPanel.SelData ~= nil and data.skill_id == ShengQiSkillPoolPanel.SelData.skill_id)
    self.SkillItem:SetData(data)
end

ShengQiSkillPoolSkillInfo = ShengQiSkillPoolSkillInfo or DeclareMono("ShengQiSkillPoolSkillInfo")
function ShengQiSkillPoolSkillInfo:SetSkill(skill)
    if TableIsEmpty(skill) then
        return
    end
    self.skill = skill
    self.SkillItem:SetData(skill)
    local type = skill:Type()
    self.Show1:SetObjActive(type == SHENG_QI_SKILL_TYPE.INITIATIVE)
    self.Show2:SetObjActive(type == SHENG_QI_SKILL_TYPE.TALENT)
    if type == SHENG_QI_SKILL_TYPE.INITIATIVE then
        self.Show1:SetSkill(skill)
    else
        self.Show2:SetSkill(skill)
    end
end

-- 主动技能描述信息
ShengQiSkillPoolSkillInfoInitative = ShengQiSkillPoolSkillInfoInitative or DeclareMono("ShengQiSkillPoolSkillInfoInitative")
function ShengQiSkillPoolSkillInfoInitative:SetSkill(skill)
    self.skill = skill
    local cfg = skill:Cfg()
    local level = cfg.combo_level
    if not level then
        return
    end
    UH.SetText(self.Desc1,cfg.skill_cd)
    UH.SetText(self.Desc2,cfg.skill_dec)
    self.Desc2Rect:SetSizeWithCurrentAnchors(1,self.Desc2.preferredHeight)
    UH.SetText(self.Desc3,cfg.buff_dec)
    self.Desc3Rect:SetSizeWithCurrentAnchors(1,self.Desc3.preferredHeight)
    UH.SetText(self.Desc4,cfg.combo_dec)
    self.Desc4Rect:SetSizeWithCurrentAnchors(1,self.Desc4.preferredHeight)
    
    local sheng_qi_id_list = Split(cfg.shanchang_shengqi, "|")
    local sheng_qi_cfg_list = {}
    for i,v in pairs(sheng_qi_id_list) do
        local id = tonumber(v)
        local s_cfg = ShengQiData.Instance:GetCfgById(id) or {}
        table.insert(sheng_qi_cfg_list, s_cfg)
    end
    self.NameList:SetData(sheng_qi_cfg_list)
    local h = 40 + math.ceil( #sheng_qi_cfg_list / 3 ) * 35 - 5
    self.SkillRect:SetSizeWithCurrentAnchors(1,h)
end

-- 天赋技能描述信息
ShengQiSkillPoolSkillInfoTalent = ShengQiSkillPoolSkillInfoTalent or DeclareMono("ShengQiSkillPoolSkillInfoTalent")
function ShengQiSkillPoolSkillInfoTalent:SetSkill(skill)
    self.skill = skill
    local cfg = skill:Cfg()
    local qua_str = Language.ShengQi.QuaType2[cfg.skill_quality]
    if cfg.appear_type == 0 then
        UH.SetText(self.Desc1,string.format(Language.ShengQi.Text22, qua_str))
    else
        local type_str = Language.ShengQi.TypeName[cfg.appear_type]
        UH.SetText(self.Desc1,string.format(Language.ShengQi.Text23, qua_str, type_str))
    end
    UH.SetText(self.Desc2,Language.ShengQi.Text8)
    UH.SetText(self.Desc3,cfg.skill_dec)
end