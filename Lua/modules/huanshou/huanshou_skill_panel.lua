-- 幻兽技能
HuanShouSkillPanel = HuanShouSkillPanel or DeclareMono("HuanShouSkillPanel", UIWFlushPanel)
function HuanShouSkillPanel:HuanShouSkillPanel()
    self.data = HuanShouData.Instance
    self.data_cares = {
        {data = self.data.sel_huanshou, func = self.FlushPanel, init_call = false}
    }
end

function HuanShouSkillPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end


function HuanShouSkillPanel:FlushPanel()
    self:FlushSkill()
end

function HuanShouSkillPanel:FlushSkill()
    local huanshou = self.data:GetSelHuanShou()
    if TableIsEmpty(huanshou) then
        return
    end
    if huanshou:IsNewBookSkill() and not ViewMgr:IsOpen(HuanShouSkillStudyResult) and not ViewMgr:IsOpen(HuanShouSkillStudy) then
        ViewMgr:OpenView(HuanShouSkillStudyResult)
    end
    local skill_list = huanshou:BookSkillList()
    self.SkillList:SetData(skill_list)
end


-- 技能书背包
HuanShouSkillBookBag = HuanShouSkillBookBag or DeclareMono("HuanShouSkillBookBag", UIWFlushPanel)
function HuanShouSkillBookBag:HuanShouSkillBookBag()
    self.data = HuanShouData.Instance
    self.tab_data = {
        {qua = 1},
        {qua = 2},
        {qua = 3},
    }
    self.data_cares = {
        {data = self.data.bag_item_list, func = self.FlushBook, init_call = false}
    }
    self.sel_tab = self.tab_data[1]
end

function HuanShouSkillBookBag:Awake()
    UIWFlushPanel.Awake(self)
    self.TabList:SetData(self.tab_data)
    self.TabList:SetectItem(1, true)
    self:FlushPanel()
end

function HuanShouSkillBookBag:FlushPanel()

end

function HuanShouSkillBookBag:FlushBook()
    local qua = self.sel_tab.qua
    local book_cfg_list = self.data:GetBookCfgList(qua)

    local grid_page_data = {}
    local index = 1
    local pageIndex = 1
    for i,v in ipairs(book_cfg_list) do
        if index > 12 then
            pageIndex = pageIndex + 1
            index = 1
        end
        grid_page_data[pageIndex] = grid_page_data[pageIndex] or {}
        table.insert(grid_page_data[pageIndex], v)
        index = index + 1
    end
    self.BookPageList:SetData(grid_page_data)
end

function HuanShouSkillBookBag:OnTabClick(data)
    self.sel_tab = data
    self:FlushBook()
end

HuanShouSkillBookBagTabItem = HuanShouSkillBookBagTabItem or DeclareMono("HuanShouSkillBookBagTabItem", UIWidgetBaseItem)
function HuanShouSkillBookBagTabItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local qua = data.qua
    local text = Language.HuanShou.BookQuaStr[qua]
    UH.SetText(self.TextOff, text)
    UH.SetText(self.TextOn, text)
end


HuanShouSkillBookBagPageItem = HuanShouSkillBookBagPageItem or DeclareMono("HuanShouSkillBookBagPageItem", UIWidgetBaseItem)
-- ItemGrid = UIWidgetList
function HuanShouSkillBookBagPageItem:SetData(data)
    self.ItemGrid:SetData(data)
end


HuanShouSkillBookBagItem = HuanShouSkillBookBagItem or DeclareMono("HuanShouSkillBookBagItem", UIWidgetBaseItem)
function HuanShouSkillBookBagItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local skill_id = data.skill_id
    local item_id = data.skill_item
    local has_num = Item.GetNum(item_id)
    local skill = HuanShouSkill.New(HuanShouData.SkillType.Book, skill_id)
    self.skill = skill
    self.SkillCell:SetData(skill)
    self.HasObj:SetActive(has_num > 0)
    if has_num > 0 then
        UH.SetText(self.HasNum, string.format(Language.HuanShou.Text8, has_num))
    end
end

function HuanShouSkillBookBagItem:OnClick()
    local item_id = self.data.skill_item
    local has_num = Item.GetNum(item_id)
    local show_type = has_num > 0 and HuanShouSkillTip.ShowType.Study or HuanShouSkillTip.ShowType.NotBook
    ViewMgr:OpenView(HuanShouSkillTip, {skill = self.skill, show_type = show_type})
end