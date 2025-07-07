-- 圣器技能界面
ShengQiSkillPanel = ShengQiSkillPanel or DeclareMono("ShengQiSkillPanel", UIWFlushPanel)
function ShengQiSkillPanel:ShengQiSkillPanel()
    self.data = ShengQiData.Instance
    self.data_cares = {
        {data = self.data.sel_sheng_qi, func = self.FlushPanel, init_call = true},
    }
end

function ShengQiSkillPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function ShengQiSkillPanel:FlushPanel()
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    self.ModelInfo:SetData(sheng_qi)
    local init_skill_cfg = Config.shengqi_auto.skill_hole
    self.TnitativeList:SetData(init_skill_cfg)
    local talent_list = sheng_qi:GetTalentSkillList()
    local t_list = {}
    for i,v in pairs(talent_list) do
        if v.skill_id > 0 then
            table.insert(t_list, v)
        end
    end
    self.TalentList:SetData(t_list)
end

function ShengQiSkillPanel:OnOpenPoolClick()
    ViewMgr:OpenView(ShengQiSkillPoolView)
end

function ShengQiSkillPanel:OnUpdateClick()
    ViewMgr:OpenView(ShengQiSkillUpdateView)
end

-- 主动技能的
ShengQiSkillPanelInitativeSkillItem = ShengQiSkillPanelInitativeSkillItem or DeclareMono("ShengQiSkillPanelInitativeSkillItem", UIWidgetBaseItem)
-- data === Config.shengqi_auto.skill_hole
function ShengQiSkillPanelInitativeSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local sheng_qi = ShengQiData.Instance:GetSelShengQi()
    self.sheng_qi = sheng_qi
    if TableIsEmpty(sheng_qi) then
        return
    end
    local skill = sheng_qi:GetInitativeSkill(data.hole) or {}
    self.SkillCell:SetData(skill)
    local is_lock = skill:IsLock()
    self.Show:SetActive(not is_lock)
    self.Lock:SetActive(is_lock)
    if not is_lock then
        UH.SetText(self.TypeTip, string.format(Language.ShengQi.Text13, skill:CDDesc()))
        local is_nice = skill:IsNice()
        self.Flag:SetActive(is_nice)
    end

    local sheng_qi_qua = sheng_qi:Qua()
    local cfg = ShengQiData.Instance:GetSkillUnlockCfg(data.hole)
    local tip
    self.UnlockEffect:SetActive(sheng_qi_qua >= cfg.shengqi_quality and is_lock)
    self.ItemLock:SetActive(sheng_qi_qua < cfg.shengqi_quality and is_lock)
    if sheng_qi_qua >= cfg.shengqi_quality then
        tip = Language.ShengQi.Tip24
        self.unlock_tip = nil
    else
        tip = string.format(Language.ShengQi.Tip25,Language.ShengQi.SkillUnlockQua[cfg.shengqi_quality])
        self.unlock_tip = tip
    end
    UH.SetText(self.LockTip, tip)
end

function ShengQiSkillPanelInitativeSkillItem:OnStudySkillClick()
    ShengQiData.Instance:SetStudySkillData(self.sheng_qi,self.data.hole)
    ViewMgr:OpenView(ShengQiSkillStudyView)
end

function ShengQiSkillPanelInitativeSkillItem:UnLockClick()
    local sheng_qi = ShengQiData.Instance:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    if self.unlock_tip then
        PublicPopupCtrl.Instance:Error(self.unlock_tip)
        return
    end
    local item_id = self.data.unlock_cost
    local num = Item.GetNum(item_id)
    local item = Item.Create({item_id = item_id, num = DataHelper.ConsumeNum(num,self.data.num,true)})
    local item_flush_func = function (item)
        local num = Item.GetNum(item.item_id)
        num = DataHelper.ConsumeNum(num,self.data.num,true)
        item.num = num
    end

    local param = {
        show_toggle = false,
        context = string.format(Language.ShengQi.Tip12, self.data.num, Item.GetQuaName(item_id)),
        item = item,
        item_flush_func = item_flush_func,
        r_func = function ()
            local has_num = Item.GetNum(item_id)
            if has_num >= self.data.num then
                local hole = sheng_qi:GetSkillUnlockHole() or self.data.hole
                ShengQiCtrl.Instance:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_UNLOCK_HOLE, sheng_qi.index, hole)
            else
                PublicPopupCtrl.Instance:Error(string.format(Language.ShengQi.Tip11,item:QuaName()))
                local item = Item.Create({item_id = item_id, jump_need_num = self.data.num - has_num})
                MainOtherCtrl.Instance:GetWayView({item = item})
            end
        end,
    }
    ViewMgr:OpenView(PetStuffBuyDialog,param)
end