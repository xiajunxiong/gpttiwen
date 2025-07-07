


-- >>>>>>>>>>>>>>>>>>>>>>    整个文件已经被策划废弃掉了!!!      <<<<<<<<<<<<<<<<<<
-- >>>>>> 新文件 ：sheng_qi_skill_update.lua

-- 圣器技能领悟
ShengQiSkillStudyView = ShengQiSkillStudyView or DeclareView("ShengQiSkillStudyView", "sheng_qi/sheng_qi_skill_study_view")
function ShengQiSkillStudyView:ShengQiSkillStudyView()
    self.Board:SetData(self:GetType(),Language.ShengQi.Title6,Vector2.New(850, 665))
end

ShengQiSkillStudyPanel = ShengQiSkillStudyPanel or DeclareMono("ShengQiSkillStudyPanel", UIWFlushPanel)
function ShengQiSkillStudyPanel:ShengQiSkillStudyPanel()
    self.data = ShengQiData.Instance
    self.data_cares = {
        {data = self.data.study_skill_data, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushItem, init_call = false},
    }
end

function ShengQiSkillStudyPanel:Awake()
    UIWFlushPanel.Awake(self)

    self.move_effect_list = {}
    self.move_effect_start_post_list = {}
    for i = 1, 4 do
        self.move_effect_list[i] = ShengQiMoveEffect.New()
        self.move_effect_start_post_list[i] = UH.GetRootPosition(self.EffectList[i])
    end
    self:FlushPanel()
    self:FlushItem()
end

function ShengQiSkillStudyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    for i,v in pairs(self.move_effect_list or {}) do
        v:Destroy()
    end
end

function ShengQiSkillStudyPanel:FlushItem()
    local list_data = Config.shengqi_auto.obtain_item
    self.BookList:SetData(list_data)
    if not self.sel_index then
        self.BookList:SetectItem(1,true)
        self.sel_index = 1
    end
end

function ShengQiSkillStudyPanel:FlushPanel()
    local info = self.data:GetStudySkillData()
    local sheng_qi = info.sheng_qi
    if not sheng_qi then
        self.LInfo:SetData({})
        self.RInfo:SetData({})
        return
    end
    local skill_index = info.skill_index
    local l_skill = sheng_qi:GetInitativeSkill(skill_index)
    local r_skill = sheng_qi:GetStudyInitativeSkill(skill_index)
    self.LInfo:SetData(l_skill)
    self.RInfo:SetData(r_skill)
    self.SaveBtn:SetActive(not TableIsEmpty(r_skill) and r_skill.skill_id > 0)
end

function ShengQiSkillStudyPanel:OnSelBookClick(data)
    self.sel_data = data
end

function ShengQiSkillStudyPanel:OnStudyClick()
    if self.effect_ing then
        return
    end
    if not self.sel_data then
        return
    end

    if Item.GetNum(self.sel_data.item_id) < 1 then
        local name = Item.GetQuaName(self.sel_data.item_id)
        PublicPopupCtrl.Instance:Error(string.format(Language.ShengQi.Tip11, name))
        MainOtherCtrl.Instance:GetWayViewById(self.sel_data.item_id)
        return
    end
    local sheng_qi = self.data:GetSelShengQi()
    local info = self.data:GetStudySkillData()

    local pos_list = {}
    pos_list[1] = self.move_effect_start_post_list[self.sel_data.index + 1]
    pos_list[2] = UH.GetRootPosition(self.EndEffectObj)
    local effect_move = self.move_effect_list[self.sel_data.index + 1]
    local effect = self.EffectList[self.sel_data.index + 1]
    UH.LocalPosG(effect, pos_list[1])
    effect:SetActive(true)
    self.effect_ing = true
    effect_move:Play(pos_list, effect, function ()
        self.effect_ing = false
        effect:SetActive(false)
        self.EffectTool:Play("3165176")
        ShengQiCtrl.Instance:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_STUDY_NEW_SKILL,sheng_qi.index,info.skill_index,self.sel_data.index)
    end, 10)
end

function ShengQiSkillStudyPanel:OnSaveClick()
    local sheng_qi = self.data:GetSelShengQi()
    local info = self.data:GetStudySkillData()
    if TableIsEmpty(sheng_qi) or TableIsEmpty(info) then
        return
    end
    local skill_index = info.skill_index
    local r_skill = sheng_qi:GetStudyInitativeSkill(skill_index)
    if not r_skill:IsLock() then

        local l_skill = sheng_qi:GetInitativeSkill(skill_index)
        local skill1 = ColorStr(l_skill:Name(), COLORSTR.Yellow6)
        local skill2 = ColorStr(r_skill:Name(), COLORSTR.Yellow6)
        local t = {
            content = string.format(Language.ShengQi.Tip22, skill1, skill2),
            confirm = {
                func =  function()
                    ViewMgr:CloseView(DialogTipsView)
                    self.EffectTool:Play("3165175")
                    ShengQiCtrl.Instance:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_SAVE_NEW_SKILL,sheng_qi.index,info.skill_index)
                end 
            },
            new_tnr = DTNewTodayNotRemind.ShengQiStudySkill,
        }
        PublicPopupCtrl.Instance:DialogTips(t)
    end
end


ShengQiSkillStudyInfo = ShengQiSkillStudyInfo or DeclareMono("ShengQiSkillStudyInfo", UIWidgetBaseItem)
-- data == skill
function ShengQiSkillStudyInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_nothing = TableIsEmpty(data) or data.skill_id == 0
    self.Show:SetActive(not is_nothing)
    self.Nothing:SetActive(is_nothing)
    if not is_nothing then
        self.SkillItem:SetData(data)
        local cfg = data:Cfg()
        local desc1 = data:SkillTypeDesc()
        desc1 = desc1 .. "\n" .. string.format(Language.ShengQi.Text9, cfg.skill_cd)

        local desc2 = cfg.buff_dec
        local desc3 = cfg.skill_dec
        local is_nice = data:IsNice()
        if not is_nice then
            local names = data:GetNiceShengQiName()
            local tip = string.format(Language.ShengQi.Tip23,names)
            tip = ColorStr(tip, COLORSTR.Red20)
            desc2 = desc2 .. tip
        end
        UH.SetText(self.Desc1, desc1)
        UH.SetText(self.Desc2, desc2)
        UH.SetText(self.Desc3, desc3)
        if self.Flag then
            self.Flag:SetActive(is_nice)
        end
    end
end

ShengQiSkillStudyBookItem = ShengQiSkillStudyBookItem or DeclareMono("ShengQiSkillStudyBookItem", UIWidgetBaseItem)
function ShengQiSkillStudyBookItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local has_num = Item.GetNum(data.item_id)
    local item = Item.Create({item_id = data.item_id, num = DataHelper.ConsumeNum(has_num,1,true)})
    self.Cell:SetData(item)
    UH.SetText(self.ItemName, item:QuaName())
    --UH.SetText(self.ItemName, data.item_name)
    UH.SetText(self.Desc, data.item_dec)
end


-- >>>>>>>>>>>>>>>>>>>>>>    已经被策划废弃掉了!!!      <<<<<<<<<<<<<<<<<<