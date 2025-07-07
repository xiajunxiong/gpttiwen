ShengQiSKillInfoView = ShengQiSKillInfoView or DeclareView("ShengQiSKillInfoView", "sheng_qi/sheng_qi_skill_info_view")
VIEW_DECLARE_MASK(ShengQiSKillInfoView,ViewMask.BlockClose)
function ShengQiSKillInfoView:ShengQiSKillInfoView()
    
end

function ShengQiSKillInfoView:LoadCallback(param_t)
    param_t = param_t or {}
    local skill = param_t.skill
    if TableIsEmpty(skill) or skill.skill_id == 0 then
        return
    end
    self.is_book = param_t.is_book -- 是否是技能图鉴

    -- self.BuffDescObj:SetActive(self.is_book == true)
    -- self.TriggerConditionObj:SetActive(not self.is_book)

    local cfg
    if self.is_book then
        cfg = ShengQiData.Instance:GetSkillBookCfg(skill.skill_id)
    else
        cfg = skill:Cfg()
    end
    local pos = param_t.pos
    if pos then
        self:CheckPos(pos)
        UH.LocalPos(self.PosRect,pos)
    end
    self.SkillItem:SetData(skill)
    UH.SetText(self.SkillDesc, cfg.skill_dec)
    UH.SetText(self.TargetText, cfg.target or "")
    UH.SetText(self.TimeText, cfg.skill_cd or "")
    UH.SetText(self.SkillDesc2, cfg.buff_dec or "")
    UH.SetText(self.TriggerCondition, skill:TriggerDesc())
    local sheng_qi_name = ""
    local sheng_qi_id_list = Split(cfg.shanchang_shengqi, "|")
    local sheng_qi_cfg_list = {}
    for i,v in pairs(sheng_qi_id_list) do
        local id = tonumber(v)
        local s_cfg = ShengQiData.Instance:GetCfgById(id) or {}
        table.insert(sheng_qi_cfg_list, s_cfg)
    end
    self.NameList:SetData(sheng_qi_cfg_list)

    local h = 40 + self.SkillDesc.preferredHeight
    self.DescRect:SetSizeWithCurrentAnchors(1,h)
    h = 40 + self.SkillDesc2.preferredHeight
    self.ShanChangRect:SetSizeWithCurrentAnchors(1,h)
    h = 40 + self.TriggerCondition.preferredHeight
    self.FaDongRect:SetSizeWithCurrentAnchors(1,h)
    h = 40 + math.ceil(#sheng_qi_cfg_list / 3) * 35 - 5
    self.BenMingSkillRect:SetSizeWithCurrentAnchors(1,h)

    local qua = cfg.skill_quality or 1
    UH.SpriteName(self.InfoQuaImg, ItemInfoConfig.QualitySp[qua])
end

function ShengQiSKillInfoView:CheckPos(pos)
    if pos.x > 450 then
        pos.x = 450
    end
    if pos.x < -450 then
        pos.x = -450
    end
    if pos.y > 125 then
        pos.y = 125
    end
    if pos.y < 10 then
        pos.y = 10
    end
end

ShengQiSKillInfoNameItem = ShengQiSKillInfoNameItem or DeclareMono("ShengQiSKillInfoNameItem", UIWidgetBaseItem)
function ShengQiSKillInfoNameItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item_id = data.item_id
    local qua = data.shengqi_quality or 1
    UH.SetText(self.Name, Item.GetQuaName(item_id))
    local color = QualityColor[qua]
    UH.Color(self.Line, color)
end

function ShengQiSKillInfoNameItem:OnClick()
    if not ShengQiData.Instance:IsOpen() then
        return
    end
    ViewMgr:CloseView(ShengQiSkillPoolView)
    ViewMgr:CloseView(ShengQiSKillInfoView)
    ShengQiData.Instance:SetTargetBook(self.data.item_id)
    ViewMgr:OpenViewByKey(Mod.ShengQi.Book)
end