-- 圣器技能学习
ShengQiSkillUpdateView = ShengQiSkillUpdateView or DeclareView("ShengQiSkillUpdateView", "sheng_qi/sheng_qi_skill_update")
function ShengQiSkillUpdateView:ShengQiSkillUpdateView()
    self.Board:SetData(self:GetType(),Language.ShengQi.Title7,Vector2.New(971, 662))
end


ShengQiSkillUpdatePanel = ShengQiSkillUpdatePanel or DeclareMono("ShengQiSkillUpdatePanel", UIWFlushPanel)
function ShengQiSkillUpdatePanel:ShengQiSkillUpdatePanel()
    self.data = ShengQiData.Instance
    self.cur_qua = 4
    self.cur_data = nil
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushStuffInfo, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushLeft, init_call = false},
        {data = self.data.sel_sheng_qi, func = self.FlushRight, init_call = true},
    }
end


function ShengQiSkillUpdatePanel:Awake()
    UIWFlushPanel.Awake(self)
    self.move_effect_ht = ShengQiMoveEffect.New()
    self.move_effect_start_pos = UH.GetRootPosition(self.EffectList[1])
    self:FlushPanel()
end

function ShengQiSkillUpdatePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.move_effect_ht:Destroy()
end

function ShengQiSkillUpdatePanel:FlushPanel()
    self:FlushLeft()
    self:FlushRight()
end

function ShengQiSkillUpdatePanel:FlushLeft(old)
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    local list = self.data:GetStudySkillGroupByQua(self.cur_qua,sheng_qi:ShengQiType())
    local skill_list = {}
    local s_q = ShengQiData.Instance:GetSelShengQi()
    for i,v in pairs(list) do
        local skill = ShengQiSkill.New({skill_id = v.skill_id, skill_type = SHENG_QI_SKILL_TYPE.INITIATIVE, sheng_qi = s_q})
        table.insert(skill_list, skill)
        skill.study_cfg = v
    end
    table.sort(skill_list, function (a,b)
        local a_num = a:IsNice() and 100 or 0
        local b_num = b:IsNice() and 100 or 0
        if a_num == b_num then
            return a.skill_id < b.skill_id
        end
        return a_num > b_num
    end)

    self.BookList:SetData(skill_list)
    if self.data_mono and not old then
        self.data_mono:SetSelected(false)
    end
end

function ShengQiSkillUpdatePanel:FlushRight()
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end

    self:FlushEffect()

    local skill_list = sheng_qi:GetUnlockInitativeSkillList()
    self.SkillList:SetData(skill_list)
    self:FlushStuffInfo()
end

function ShengQiSkillUpdatePanel:FlushStuffInfo()
    self.StuffInfo:SetActive(self.cur_data ~= nil)
    self.SelTip:SetActive(self.cur_data == nil)
    UH.SetInter(self.BtnInter, self.cur_data ~= nil)
    if self.cur_data ~= nil then
        local skill_id = self.cur_data.skill_id
        local stuff_cfg = Cfg.ShengQiSkillBook(skill_id)
        self.stuff_cfg = stuff_cfg
        local item_id = stuff_cfg.lingwu_item
        local has_num = Item.GetNum(item_id)
        local item = Item.Create({item_id = item_id, num = DataHelper.ConsumeNum(has_num, 1,true)})
        self.StuffCell:SetData(item)
        UH.SetText(self.StuffName, item:QuaName())
    end
end

function ShengQiSkillUpdatePanel:FlushEffect()
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    if TableIsEmpty(self.SkillList.item_list) then
        self.data:CheckSkillUpdate(sheng_qi)
        return
    end
    local update_skill = sheng_qi:GetStudyInitativeSkill()
    if update_skill then
        local item_index = update_skill.index + 1
        local pos_list = {}
        pos_list[1] = self.move_effect_start_pos
        pos_list[2] = UH.GetRootPosition(self.SkillList.item_list[item_index].gameObject)
        local effect = self.EffectList[update_skill:Qua() - 2]
        UH.LocalPosG(effect, pos_list[1])
        effect:SetActive(true)
        self.effect_ing = true
        self.move_effect_ht:Play(pos_list, effect, function ()
            self.effect_ing = false
            effect:SetActive(false)
            self.data:CheckSkillUpdate(sheng_qi)
        end, 10)
    end
end

function ShengQiSkillUpdatePanel:OnQuaClick(qua)
    self.cur_qua = qua
    self:FlushLeft()
end

function ShengQiSkillUpdatePanel:OnBookItemClick(mono,value)
    if not value then
        self.cur_data = nil
        self.data_mono = nil
    else
        self.cur_data = mono:GetData().study_cfg
        self.data_mono = mono
    end
    self:FlushStuffInfo()
end

function ShengQiSkillUpdatePanel:OnBtnClick()
    if self.effect_ing then
        return
    end
    if self.cur_data == nil or not self.stuff_cfg then
        return
    end
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end

    local item_id = self.stuff_cfg.lingwu_item
    local has_num = Item.GetNum(item_id)
    if has_num < 1 then
        PublicPopupCtrl.Instance:Error(Language.ShengQi.Tip26)
        MainOtherCtrl.Instance:GetWayViewById(item_id)
        return
    end

    ShengQiCtrl.Instance:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_STUDY_NEW_SKILL,sheng_qi.index, item_id)
end


ShengQiSkillUpdateBookItem = ShengQiSkillUpdateBookItem or DeclareMono("ShengQiSkillUpdateBookItem", UIWidgetBaseItem)
function ShengQiSkillUpdateBookItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    --local s_q = ShengQiData.Instance:GetSelShengQi()
    --local skill = ShengQiSkill.New({skill_id = data.skill_id, skill_type = SHENG_QI_SKILL_TYPE.INITIATIVE, sheng_qi = s_q})
    local skill = data
    self.SkillItem:SetData(skill)
    self.Flag:SetActive(skill:IsNice())
    local item_num = Item.GetNum(data.lingwu_item)
    if item_num > 0 then
        UH.SetText(self.NumDesc, string.format(Language.ShengQi.Text16, item_num))
    else
        UH.SetText(self.NumDesc, "")
    end
end




ShengQiSkillUpdateResult = ShengQiSkillUpdateResult or DeclareView("ShengQiSkillUpdateResult", "sheng_qi/sheng_qi_skill_update_result")
function ShengQiSkillUpdateResult:ShengQiSkillUpdateResult()
    self.data = ShengQiData.Instance
end

function ShengQiSkillUpdateResult:LoadCallback(param_t)
    local sheng_qi = param_t.sheng_qi
    if not sheng_qi then
        return
    end
    self.sheng_qi = sheng_qi
    local list_data_1 = sheng_qi:GetUnlockInitativeSkillList()
    local list_data_2 = sheng_qi:GetUnlockInitativeSkillList(true)

    self.SkillList:SetData(list_data_1)
    self.SkillUpdateList:SetData(list_data_2)
end

function ShengQiSkillUpdateResult:OnCancelClick()
    local param_t = {
        content = Language.ShengQi.Tip27,
        confirm = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                ShengQiCtrl.Instance:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_SAVE_NEW_SKILL,self.sheng_qi.index, 0)
            end 
        },
        new_tnr = DTNewTodayNotRemind.ShengQiStudySkillCancel
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)
end

function ShengQiSkillUpdateResult:OnOkClick()
    local new_skill = self.sheng_qi:GetStudyInitativeSkill()
    local old_skill = self.sheng_qi:GetInitativeSkill(new_skill.index)
    local skill1 = old_skill:QuaName()
    local skill2 = new_skill:QuaName()
    local t = {
        content = string.format(Language.ShengQi.Tip22, skill1, skill2),
        confirm = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                ShengQiCtrl.Instance:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_SAVE_NEW_SKILL,self.sheng_qi.index, 1)
            end 
        },
        new_tnr = DTNewTodayNotRemind.ShengQiStudySkill,
    }
    PublicPopupCtrl.Instance:DialogTips(t)
end