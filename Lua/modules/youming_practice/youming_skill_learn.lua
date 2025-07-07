YouMingSkillLearn = YouMingSkillLearn or DeclareMono("YouMingSkillLearn", UIWFlushPanel)

function YouMingSkillLearn:YouMingSkillLearn()
    self.data = YouMingData.Instance
    self.data_cares = {{
        data = self.data:GetRemodelYouMing(),
        func = self.FlushBaseInfo
    },{
        data = self.data:GetRemodelUse(),
        func = self.FlushSkillItemUse,
        keys = {"skill_item"}
    }}
    self.select_index = 1
    self.select_skill = nil
    self.is_high_grasp = 0
    self.select_item_id = 0
    self.select_skill_item = nil
end
function YouMingSkillLearn:OnClickNormal()
    self.is_high_grasp = 0
    self:FlushRateCost()
end
function YouMingSkillLearn:OnClickHigh()
    self.is_high_grasp = 1
    self:FlushRateCost()
end
function YouMingSkillLearn:FlushSkillItemUse()
    self.select_skill_item = self.data:GetRemodelUse().skill_item
    self.select_item_id = self.select_skill_item == nil and 0 or self.select_skill_item.item_id
    -- self.use_item_id = self.select_item_id
    if self.select_item_id == 0 then
        self.AddItem:SetActive(true)
        self.Cell2:SetObjActive(false)
    else
        self.AddItem:SetActive(false)
        self.Cell2:SetObjActive(true)
        self.Cell2:SetDataByItemIdWithZeroNum(self.select_item_id)
        
    end
end
function YouMingSkillLearn:OnClickPreview()
    ViewMgr:OpenView(YouMingRemodelSkillPreview)
end

function YouMingSkillLearn:FlushBaseInfo()
    self.youming = self.data:GetRemodelYouMing()
    if self.youming.Aptitude == nil then
        return
    end
    UH.SetText(self.Rate,"")
    self.select_skill_item = nil
    self.select_item_id = 0
    self.data:SetSkillUse(nil)
    local list = self.youming:CurSkillList()
    self.SkillList:SetData(list)
    if table.nums(list) > 0 then
        self.SelectCell:SetObjActive(true)
        self.EmptyTip:SetActive(false)
        self.SkillList:ClickItem(self.select_index)
    else
        self.EmptyTip:SetActive(true)
        self.SelectCell:SetObjActive(false)
    end
    -- 显示普通领悟所需
    local item1 = self.data:NormalItem()
    self.Cell:SetDataByItemIdWithNeed(item1.item_id, item1.num)
    local item2 = self.data:HighItem()
    self.Cell1:SetDataByItemIdWithNeed(item2.item_id, item2.num)
    -- 高级领悟所需
    UH.SetText(self.Desc, Language.YouMingPractice.LearnSkillTip)
end

function YouMingSkillLearn:OnClickSkill(cell)
    local data = cell:GetData()
    self.select_index = self.SkillList:GetSelectedIndex(data)
    self.SelectCell:SetData(data)
    if self.select_skill == data then
        local pos = UH.GetRootPosition(cell.gameObject)
        ViewMgr:OpenView(YouMingSkillTip, {skill = data, pos = pos})
    end
    self.select_skill = data
    self:FlushRateCost()
end
function YouMingSkillLearn:FlushRateCost()
    -- 显示铜币
    local config = self.data:LingWuConfig(self.select_skill.change_id)
    UH.SetText(self.NeedCount, config.cost_coin)
    if self.is_high_grasp == 0 then
        UH.SetText(self.Rate, Format(Language.YouMingPractice.LearnRate, config.learn_change))
    else
        UH.SetText(self.Rate, Format(Language.YouMingPractice.LearnRate1, config.high_learn_change))
    end
end
function YouMingSkillLearn:OnClickLearn()
    self.youming = self.data:GetRemodelYouMing()
    if self.youming.Aptitude == nil then
        return
    end
    if self.select_skill == nil then
        return
    end
    if self.select_skill:IsLock() then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.SkillLock)
        return
    end
    -- self.is_high_grasp = 0
    if self.is_high_grasp == 1 and self.select_item_id ~= 0 then
        
        local item2 = self.data:HighItem()

        if Item.GetNum(item2.item_id) < item2.num then
            PublicPopupCtrl.Instance:Center(Language.YouMingPractice.LearnZero2)
            return
        end
        if Item.GetNum(self.select_item_id) == 0 then
            PublicPopupCtrl.Instance:Center(Language.YouMingPractice.LearnZero)
            return
        end
        --已有不能重复学习
        local list = self.youming:CurSkillList()
        for i, v in ipairs(list) do
            if self.select_skill_item and v.skill_id == self.select_skill_item:Config().param1 then
                PublicPopupCtrl.Instance:Center(Language.YouMingPractice.SameSkill)
                return
            end
        end
    elseif self.is_high_grasp == 1 and self.select_item_id == 0 then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.LearnZero4)
    else
        local item1 = self.data:NormalItem()
        if Item.GetNum(item1.item_id) < item1.num then
            PublicPopupCtrl.Instance:Center(Language.YouMingPractice.LearnZero3)
            return
        end
    end
    YouMingCtrl.Instance:SendGraspSkillReq(self.youming.index, self.select_skill.change_id - 1,
        self.select_skill.index - 1, self.is_high_grasp, self.select_item_id)
end
function YouMingSkillLearn:OnClickAdd()
    ViewMgr:OpenView(YouMingSkillBookUse)
end

YouMingLearnSkillItem = YouMingLearnSkillItem or DeclareMono("YouMingLearnSkillItem", UIWidgetBaseItem)
function YouMingLearnSkillItem:YouMingLearnSkillItem()

end
function YouMingLearnSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
end

function YouMingLearnSkillItem:Click()
    self.EventClick:Trigger()
end
function YouMingLearnSkillItem:OnSelect(selected)
    if selected then
        self:Click()
    end
end


--==高级技能书使用
YouMingSkillBookUse = YouMingSkillBookUse or DeclareView("YouMingSkillBookUse", "youming_remodel/youming_remodel_skill")
function YouMingSkillBookUse:YouMingSkillBookUse()
    self.data = YouMingData.Instance
    self.select_item = nil
end
function YouMingSkillBookUse:LoadCallback()
    self.Board:SetData(self:GetType(), Language.YouMingPractice.SkillUse, Vector2.New(560, 440))
    self.data:SetSkillUse(nil)
    -- self.EmptyTip
    local list = self.data:SkillGroup()
    self.List:SetData(list)
    self.EmptyTip:SetActive(table.nums(list) == 0)
end

function YouMingSkillBookUse:OnClickOk()
    self.data:SetSkillUse(self.select_item)
    ViewMgr:CloseView(YouMingSkillBookUse)
end

function YouMingSkillBookUse:OnGetWayClick()
    local get_way_info = {}
    local get_way_t = Split(Config.dim_pet_cfg_auto.other[1].skill_book_get_way,"|")
    for k,v in pairs(get_way_t) do
        table.insert(get_way_info, BagData.Instance:GetWayCfg(tonumber(v)))
    end
    ViewMgr:OpenView(PetGuideTipView, {list = get_way_info, tip = Language.YouMingPractice.Text4})
end
function YouMingSkillBookUse:OnClickItem(data)
    if self.select_item == data then
        ItemInfoCtrl.Instance:ItemInfoView{item_list = {data}}
    end
    self.select_item = data

end

YouMingSkillBookUseItem = YouMingSkillBookUseItem or DeclareMono("YouMingSkillBookUseItem", UIWidgetBaseItem)
function YouMingSkillBookUseItem:YouMingSkillBookUseItem()
    
end
function YouMingSkillBookUseItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
end
function YouMingSkillBookUseItem:Click()
    self.EventClick:Trigger()
end

function YouMingSkillBookUseItem:OnSelect(selected)
    if selected then
        self:Click()
    end
end