-- >>>>>>>>>>>>>>>>> 废弃了 用的是fairy_dragon_soul_new_up <<<<<<<<<<<<<< 

-- 龙魂升级
FiaryDragonSoulUpView = FiaryDragonSoulUpView or DeclareView("FiaryDragonSoulUpView", "fairy_dragon/fairy_dragon_soul_up_view")
function FiaryDragonSoulUpView:FiaryDragonSoulUpView()
    self.Board:SetData(self:GetType(),Language.FairyDragon.Title2,Vector2.New(470, 726))
end


FiaryDragonSoulUpPanel = FiaryDragonSoulUpPanel or DeclareMono("FiaryDragonSoulUpPanel", UIWFlushPanel)
function FiaryDragonSoulUpPanel:FiaryDragonSoulUpPanel()
    self.data = FairyDragonSoulData.Instance
    self.data_cares = {
        {data = self.data.sel_stuff_list, func = self.FlushStuff, init_call = false},
        {data = FairyDragonSoulData.Instance.soul_item_list, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushStuff, init_call = false},
    }
end

function FiaryDragonSoulUpPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function FiaryDragonSoulUpPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetUpSoul(nil)
    self.data:ClearSelStuff()
end

function FiaryDragonSoulUpPanel:FlushPanel()
    self:FlushBaseInfo()
    self:FlushStuff()
end

function FiaryDragonSoulUpPanel:FlushBaseInfo()
    self.item = self.data:GetUpSoul()
    if TableIsEmpty(self.item) then
        ViewMgr:CloseView(FiaryDragonSoulUpView)
        return
    end
    self.ItemCell:SetData(self.item)
    UH.SetText(self.Name, self.item:QuaName())
    local is_max = self.item:IsFullLevel()
    self.CtrlBtn:SetActive(not is_max)
    self.MaxBtn:SetActive(is_max)
end

function FiaryDragonSoulUpPanel:FlushStuff()
    local cur_level = self.item:Level()
    local stuff_exp = self.data:GetSelStuffExp()
    local preview_level = self.data:GetPreviewLevel(self.item, stuff_exp)
    --LogDG("预览等级",preview_level,stuff_exp)
    self.LevelUpFlag:SetActive(preview_level > cur_level)
    UH.SetText(self.Level1, string.format(Language.FairyDragon.Level2, cur_level))
    if preview_level > cur_level then
        UH.SetText(self.Level2, string.format(Language.FairyDragon.Level2, preview_level))
    end

    local cur_exp = self.item:CurExp()
    local next_exp = self.item:NextExp()
    local pro_exp = cur_exp + stuff_exp
    self.ProgressBar:SetProgress(pro_exp / next_exp)
    if next_exp ~= 0 then
        self.ProgressBar:SetText(string.format("%s/%s", pro_exp, next_exp))
    else
        self.ProgressBar:SetText("")
    end

    local rand_attri_list = self.item:RandAttriList()
    self.RandAttriList:SetData(rand_attri_list)
    self.EmptyState:SetActive(TableIsEmpty(rand_attri_list))
    local next_random_cfg = self.item:GetNextSpInCfg()
    if not next_random_cfg then
        UH.SetText(self.RandTip, "")
    else
        UH.SetText(self.RandTip, string.format(Language.FairyDragon.Text16, next_random_cfg.soul_level))
    end

    local base_and_next_attri_list = self.item:BaseAndNextAttriList(preview_level)
    --LogError("升级属性预览", preview_level,stuff_exp,base_and_next_attri_list)
    self.BaseAttriList:SetData(base_and_next_attri_list)
    local skill = self.item:Skill()
    UH.SetText(self.SkillDesc, skill:Desc())

    self:FlushStuffList()
end

function FiaryDragonSoulUpPanel:FlushStuffList()
    local cfg = self.data:StuffCfg()
    local list = {}
    for i,v in pairs(cfg) do
        local num = Item.GetNum(i)
        if num > 0 then
            local item = Item.Create({item_id = i, num = num})
            table.insert(list, item)
        end
    end
    table.sort(list, function (a,b)
        return a:Color() < b:Color()
    end)
    for i = #list + 1 , 10 do
        table.insert(list, Item.Create({item_id = 0}))
    end
    self.StuffList:SetData(list)
end

-- 全部品质
function FiaryDragonSoulUpPanel:OnAllSelClick2()
    if self.item:IsFullLevel() then
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.MaxLevelTip)
        return
    end
    local cfg = self.data:StuffCfg()
    local list = {}
    for i,v in pairs(cfg) do
        table.insert(list, {item_id = i, exp = v})
    end
    table.sort(list, function (a,b)
        return a.exp > b.exp
    end)
    for i = 1, 2000 do
        local max_need_exp = self.item:FullExp()
        local stuff_exp = self.data:GetSelStuffExp()
        local sub_exp = max_need_exp - stuff_exp
        if sub_exp <= 0 then
            break
        else
            local is_added = false
            for _,v in pairs(list) do
                local sel_num = self.data:GetSelExpStuffNum(v.item_id)
                if Item.GetNum(v.item_id) > sel_num and v.exp <= sub_exp then
                    self.data:AddExpStuff(v.item_id, 1)
                    is_added = true
                    break
                end
            end
            if not is_added then
                for j = #list,1, -1 do
                    local v = list[j]
                    local sel_num = self.data:GetSelExpStuffNum(v.item_id)
                    if Item.GetNum(v.item_id) > sel_num then
                        self.data:AddExpStuff(v.item_id, 1)
                        is_added = true
                        break
                    end
                end
                -- 说明没有材料可添加了，退出循环
                if not is_added then
                    return
                end
            end
        end
    end
end

-- 最低品质一种
function FiaryDragonSoulUpPanel:OnAllSelClick()
    if self.item:IsFullLevel() then
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.MaxLevelTip)
        return
    end
    local cfg = self.data:StuffCfg()
    local list = {}
    for i,v in pairs(cfg) do
        if Item.GetNum(i) > 0 then
            table.insert(list, {item_id = i, exp = v})
        end
    end
    table.sort(list, function (a,b)
        return a.exp < b.exp
    end)

    list = {list[1]} or {}
    if TableIsEmpty(list) then
        return
    end

    for i = 1, 2000 do
        local max_need_exp = self.item:FullExp()
        local stuff_exp = self.data:GetSelStuffExp()
        local sub_exp = max_need_exp - stuff_exp
        if sub_exp <= 0 then
            break
        else
            local is_added = false
            for _,v in pairs(list) do
                local sel_num = self.data:GetSelExpStuffNum(v.item_id)
                if Item.GetNum(v.item_id) > sel_num and v.exp <= sub_exp then
                    self.data:AddExpStuff(v.item_id, 1)
                    is_added = true
                    break
                end
            end
            if not is_added then
                for j = #list,1, -1 do
                    local v = list[j]
                    local sel_num = self.data:GetSelExpStuffNum(v.item_id)
                    if Item.GetNum(v.item_id) > sel_num then
                        self.data:AddExpStuff(v.item_id, 1)
                        is_added = true
                        break
                    end
                end
                -- 说明没有材料可添加了，退出循环
                if not is_added then
                    return
                end
            end
        end
    end
end

function FiaryDragonSoulUpPanel:OnUpClick()
    if self.item:IsFullLevel() then
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.MaxLevelTip)
        return
    end
    FairyDragonCtrl.Instance:SendUpSoulReq()
    self.data:ClearSelStuff()
end

function FiaryDragonSoulUpPanel:OnMaxUpClick()
    PublicPopupCtrl.Instance:Error(Language.FairyDragon.MaxLevelTip)
end




FiaryDragonSoulUpAttriItem = FiaryDragonSoulUpAttriItem or DeclareMono("FiaryDragonSoulUpAttriItem", UIWidgetBaseItem)
function FiaryDragonSoulUpAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local attri_type = data.attri_type
    self.Img:SetActive(data.cur_attri_value ~= data.next_attri_value)
    self.AddValue:SetObjActive(data.cur_attri_value ~= data.next_attri_value)
    local attri_info = CommonStruct.AttrStruct(attri_type,data.cur_attri_value)
    UH.SetText(self.AttriValue, attri_info.attr_percent_value)
    UH.SetText(self.AttriName, string.format("%s",attri_info.attr_name))
    if data.cur_attri_value ~= data.next_attri_value then
        local add_info = CommonStruct.AttrStruct(attri_type,data.next_attri_value)
        UH.SetText(self.AddValue, add_info.attr_percent_value)
    end
end


FiaryDragonSoulUpStuffItem = FiaryDragonSoulUpStuffItem or DeclareMono("FiaryDragonSoulUpStuffItem", UIWidgetBaseItem)
function FiaryDragonSoulUpStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local cur_num = FairyDragonSoulData.Instance:GetSelExpStuffNum(data.item_id)
    self.SelImg:SetActive(cur_num > 0)
    self.StuffEvent:SetActive(data.item_id > 0)
    self.ItemCell:SetData(data)
    self.RemoveBtn:SetActive(cur_num > 0)

    if data.item_id > 0 then
        local num = data:Num()
        if cur_num > 0 then
            CellFlushs.SetNum(self.ItemCell, string.format("%s/%s",cur_num,num))
        else
            CellFlushs.SetNum(self.ItemCell, num)
        end
    end
end

function FiaryDragonSoulUpStuffItem:OnDestroy()
    TimeHelper:CancelTimer(self.touch_ht)
end

function FiaryDragonSoulUpStuffItem:OnAddStuffClick()
    local result = FairyDragonSoulData.Instance:AddExpStuff(self.data.item_id, 1)
    if result == 1 then
        -- 无操作
    elseif result == 0 then
        PublicPopupCtrl.Instance:Center(Language.FairyDragon.NotItemTip)
        TimeHelper:CancelTimer(self.touch_ht)
    elseif result == -1 then
        PublicPopupCtrl.Instance:Center(Language.FairyDragon.Tip4)
        TimeHelper:CancelTimer(self.touch_ht)
    end
end

function FiaryDragonSoulUpStuffItem:OnStuffDown()
    TimeHelper:CancelTimer(self.touch_ht)
	self.touch_ht = TimeHelper:AddRunTimer(BindTool.Bind(self.OnAddStuffClick,self),0.2)
end

function FiaryDragonSoulUpStuffItem:OnStuffUp()
    TimeHelper:CancelTimer(self.touch_ht)
end

function FiaryDragonSoulUpStuffItem:OnRemoveStuffClick()
    FairyDragonSoulData.Instance:AddExpStuff(self.data.item_id, -1)
end