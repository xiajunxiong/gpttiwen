ShengHenSkillView = ShengHenSkillView or DeclareView("ShengHenSkillView", "ling_bao/sheng_hen_skill")
function ShengHenSkillView:ShengHenSkillView()
end

function ShengHenSkillView:LoadCallback(param_t)
end

function ShengHenSkillView:OnCloseClick()
    ViewMgr:CloseView(ShengHenSkillView)
end

----------------------------ShengHenSkillPanel----------------------------
ShengHenSkillPanel = ShengHenSkillPanel or DeclareMono("ShengHenSkillPanel", UIWFlushPanel)
function ShengHenSkillPanel:ShengHenSkillPanel()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
    self.data = self.Data.info_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
end

function ShengHenSkillPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.now_select_shenghen = self.Data:MainShengHenType()
    self.now_select_skill = self.Data:MainShengHenSkillIndex()
    self:FlushAll()
end

function ShengHenSkillPanel:FlushAll()
    local str_lv = self.Data:ShengHenSkillLv(self.now_select_shenghen, self.now_select_skill)
    local now_cfg = self.Data:ShengHenSkillCfg(self.now_select_shenghen, self.now_select_skill, str_lv)
    local next_cfg = self.Data:ShengHenSkillCfg(self.now_select_shenghen, self.now_select_skill, str_lv + 1)
    UH.SetText(self.TxtBtn, self.language.SkillBtn[str_lv == 0 and 0 or 1])
    local can_up, str, p1, p2 = self.Data:ShengHenSkillIsCanUp(self.now_select_shenghen, self.now_select_skill)
    local is_max_lv = TableIsEmpty(next_cfg)
    if str_lv == 0 then
        -- 0级
        self.ShenHenSkillItemNow:SetData(next_cfg)
        self.ShenHenSkillItemNext:SetObjActive(false)
    elseif is_max_lv then
        -- 满级
        self.ShenHenSkillItemNow:SetData(now_cfg)
        self.ShenHenSkillItemNext:SetObjActive(false)
    else
        self.ShenHenSkillItemNow:SetData(now_cfg)
        self.ShenHenSkillItemNext:SetData(next_cfg)
        self.ShenHenSkillItemNext:SetObjActive(true)
    end

    self.ObjMaxShow:SetActive(is_max_lv)
    if (not can_up) and str then
        UH.SetText(self.TxtLimit, str)
    else
        UH.SetText(self.TxtLimit, "")
    end
    self.ObjArrow:SetActive(str_lv ~= 0 and (not is_max_lv))
    self.Remind:SetNum(can_up and 1 or 0)

    if not self.now_str_lv then
        self.now_str_lv = str_lv
    else
        if self.now_str_lv ~= str_lv then
            self:PlayEffect(is_max_lv and "3165220" or "3165244")
            self.now_str_lv = str_lv
        end
    end
end

function ShengHenSkillPanel:PlayEffect(str)
    if self.handle then
        UH.StopEffect(self.EffectTool, self.handle)
    end
    self.handle = UH.PlayEffect(self.EffectTool, str)
end

function ShengHenSkillPanel:OnSkillUpClick()
    local can_up, str, p1, p2 = self.Data:ShengHenSkillIsCanUp(self.now_select_shenghen, self.now_select_skill)
    if can_up then
        LingBaoCtrl.Instance:SendReq(LingBaoReqType.ShengHenSkillUp, self.now_select_shenghen, self.now_select_skill)
    else
        PublicPopupCtrl.Instance:Center(str)
    end
end

function ShengHenSkillPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

--------------ShengHenSkillUpItem--------------
ShengHenSkillUpItem = ShengHenSkillUpItem or DeclareMono("ShengHenSkillUpItem", UIWidgetBaseItem)
function ShengHenSkillUpItem:ShengHenSkillUpItem()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
end

function ShengHenSkillUpItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if not TableIsEmpty(data) then
        local sp_cfg = Cfg.SpecialEffect(data.trace_skill)
        UH.SetIcon(self.Icon, sp_cfg.icon_id, ICON_TYPE.BUFF)
        UH.SetText(self.TxtNameLv, Format(self.language.SkillNameLv, sp_cfg.name, data.trace_skill_level))
        UH.SetText(self.TxtDesc, sp_cfg.description)
    end
end