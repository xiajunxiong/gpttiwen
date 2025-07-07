-- 幻兽技能学习
HuanShouSkillStudy = HuanShouSkillStudy or DeclareView("HuanShouSkillStudy", "huanshou/huanshou_skill_study")
HuanShouSkillStudy.book_skill = nil      -- 将要学习的技能
function HuanShouSkillStudy:LoadCallback()
    self.Board:SetData(self:GetType(), Language.HuanShou.Title2, Vector2.New(672, 668))
end

function HuanShouSkillStudy:CloseCallback()
    HuanShouSkillStudy.book_skill = nil
end


HuanShouSkillStudyPanel = HuanShouSkillStudyPanel or DeclareMono("HuanShouSkillStudyPanel", UIWFlushPanel)
function HuanShouSkillStudyPanel:HuanShouSkillStudyPanel()
    self.data = HuanShouData.Instance
    self.lock_skill_list = {}
    self.lock_item_id = Config.huanshou_auto.other[1].lock_item

    self.data_cares = {
        --{data = self.data.bag_item_list, func = self.FlushLockStuff, init_call = false},
        {data = self.data.sel_huanshou, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushLockStuff, init_call = false}
    }
    self.moveing = false
    self.move_func = ShengQiMoveEffect.New()
end

function HuanShouSkillStudyPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function HuanShouSkillStudyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.moveing then
        ViewMgr:OpenView(HuanShouSkillStudyResult)
    end
    self.move_func:Destroy()
end

function HuanShouSkillStudyPanel:FlushPanel()
    local huanshou = self.data:GetSelHuanShou()
    self.huanshou = huanshou
    local book_skill = HuanShouSkillStudy.book_skill
    self.book_skill = book_skill
    if TableIsEmpty(huanshou) or not book_skill then
        return
    end

    self:PlayerMove()

    self.SkillCell:SetData(book_skill)
    UH.SetText(self.SkillName, book_skill:Name())
    UH.SetText(self.SkillDescText, book_skill:Desc())


    local skill_list = huanshou:BookSkillList()
    local list_data = {}
    local skill_count = 0
    for i,v in pairs(skill_list) do
        local t = {}
        t.skill = v
        t.is_lock = self.lock_skill_list[v.index + 1] == 1
        table.insert(list_data, t)
        if v.skill_id > 0 then
            skill_count = skill_count + 1
        end
    end
    self.can_lock_count = 0
    local cfg = Config.huanshou_auto.other[1]
    if skill_count >= cfg.need_skill_num2 then
        self.can_lock_count = 2
    elseif skill_count >= cfg.need_skill_num1 then
        self.can_lock_count = 1
    end
    self.SkillList:SetData(list_data)

    local lock_num = table.nums(self.lock_skill_list)
    local lock_num_str = string.format(Language.Common.Slash, lock_num, self.can_lock_count)
    lock_num_str = string.format(Language.HuanShou.Text9,  lock_num < self.can_lock_count and COLORSTR.Green7 or COLORSTR.Red10, lock_num_str)
    UH.SetText(self.NoticeText, lock_num_str)

    self:FlushLockStuff()
end

function HuanShouSkillStudyPanel:PlayerMove()
     if self.moveing then
        return
    end
    local is_new = self.huanshou:IsNewBookSkill()
    if not is_new then
        self.MoveIcon.transform:SetParent(self.MoveParent.transform)
        self.MoveIcon:SetActive(true)
        self.MoveIcon:SetLocalPosition(Vector3.zero)
        return
    end
    self.moveing = true
    local index = self.huanshou.vo.new_skill_grid_idx + 1
    local item = self.SkillList:GetItem(index)
    self.MoveIcon.transform:SetParent(item.gameObject.transform)
    local pos_list = {}
    pos_list[1] = self.MoveIcon:GetLocalPosition()
    pos_list[3] = Vector3.zero()
    pos_list[2] = pos_list[1] + (pos_list[3] - pos_list[1]) + Vector3.New(100,0,0)
    self.move_func:Play(pos_list,self.MoveIcon,function()
        self.MoveIcon:SetActive(true)
        ViewMgr:OpenView(HuanShouSkillStudyResult)
        local book_item_id = self.book_skill:ItemId()
        if Item.GetNum(book_item_id) < 1 then
            ViewMgr:CloseView(HuanShouSkillStudy)
            return
        end
        self.moveing = false
    end,20)
end

function HuanShouSkillStudyPanel:FlushLockStuff()
    local lock_num = table.nums(self.lock_skill_list)
    self.CostObj:SetActive(lock_num > 0)
    if lock_num > 0 then
        local cfg = Config.huanshou_auto.other[1]
        local has_num = Item.GetNum(self.lock_item_id)
        local need_num = 0
        if lock_num > 1 then
            need_num = cfg.need_item_num2 + cfg.need_item_num1
        else
            need_num = cfg.need_item_num1
        end
        self.need_lock_item_num = need_num
        local item = Item.Create({item_id = self.lock_item_id})
        self.CostItem:SetData(item)
        UH.SetText(self.CostNum, DataHelper.ConsumeNum(has_num, need_num, true))
        UH.SetText(self.costName, item:Name())
        self.is_need_buy_lock = has_num < need_num
    end
end

function HuanShouSkillStudyPanel:OnItemClick(data)
    if self.moveing then
        return
    end
    if data.is_lock then
        self.lock_skill_list[data.skill.index + 1] = nil
    elseif data.skill.skill_id > 0 then
        local lock_num = table.nums(self.lock_skill_list)
        if lock_num < self.can_lock_count then
            self.lock_skill_list[data.skill.index + 1] = 1
        end
    end
    self:FlushPanel()
end

function HuanShouSkillStudyPanel:OnStudyClick()
    if self.moveing then
        return
    end
    if self.is_need_buy_lock then
        PublicPopupCtrl.Instance:Error(Language.HuanShou.NotItemTip)
        MainOtherCtrl.Instance:GetWayViewById(self.lock_item_id)
    else
        local item_id = self.book_skill:ItemId()
        local item = self.data:GetBagItemByItemId(item_id)
        if item then
            local p3 = bit:b16d(self.lock_skill_list)
            HuanShouCtrl.Instance:SendReq(HuanShouReqType.StudySkill, self.huanshou.id, item.index, p3)
        end
    end
end



HuanShouSkillStudyItem = HuanShouSkillStudyItem or DeclareMono("HuanShouSkillStudyItem", UIWidgetBaseItem)
function HuanShouSkillStudyItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.SkillCell:SetData(data.skill)
    self.Lock:SetActive(data.is_lock == true)
end



HuanShouSkillStudyResult = HuanShouSkillStudyResult or DeclareView("HuanShouSkillStudyResult", "huanshou/huanshou_study_result")
function HuanShouSkillStudyResult:HuanShouSkillStudyResult()
    self.data = HuanShouData.Instance
    --self.Board:SetData(self:GetType(), "学习成功", Vector2.New(786, 678))
end

function HuanShouSkillStudyResult:LoadCallback()
    local huanshou = self.data:GetSelHuanShou()
    self.huanshou = huanshou
    if TableIsEmpty(huanshou) then
        return
    end
    local has_skill_list = huanshou:BookSkillList()
    local new_skill_list = huanshou:NewBookSkillList()
    self.HasSkillList:SetData(has_skill_list)
    self.NewSkillList:SetData(new_skill_list)
end


function HuanShouSkillStudyResult:OnCancelClick()
    local param_t = {
        content = Language.HuanShou.Tip5,
        confirm = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                HuanShouCtrl.Instance:SendReq(HuanShouReqType.SaveSkill, self.huanshou.id, 0)
                ViewMgr:CloseView(HuanShouSkillStudyResult)
            end 
        },
        new_tnr = DTNewTodayNotRemind.HuanShouStudyResult
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)
end

function HuanShouSkillStudyResult:OnOkClick()
    HuanShouCtrl.Instance:SendReq(HuanShouReqType.SaveSkill, self.huanshou.id, 1)
    ViewMgr:CloseView(HuanShouSkillStudyResult)
    ViewMgr:CloseView(HuanShouSkillStudy)
end

function HuanShouSkillStudyResult:OnCloseClick()
    self:OnCancelClick()
end