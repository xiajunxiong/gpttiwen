-- 宠物强化活动
ActPetStrPanel = ActPetStrPanel or DeclareMono("ActPetStrPanel", UIWFlushPanel)
function ActPetStrPanel:ActPetStrPanel()
    self.data = ActPetStrData.Instance
    self.data_cares = {
        {data = self.data.info_data, func = self.FlushPanel, init_call = false},
        {data = self.data.info_data, func = self.FlushTabList, init_call = false}
    }
end

function ActPetStrPanel:Awake()
    UIWFlushPanel.Awake(self)
    local tab_list = self.data:GetTabList()
    self.tab_list = tab_list
    self.TabList:SetData(tab_list)
    local sel_index = self.data:GetJumpTab()
    if not TableIsEmpty(tab_list) then
        self.TabList:SetectItem(sel_index, true)
    end
    self:FlushPanel()
end

function ActPetStrPanel:FlushTabList()
    self.TabList:SetData(self.tab_list)
end

function ActPetStrPanel:FlushPanel()
    if TableIsEmpty(self.sel_data) then
        return
    end
    local qua = self.sel_data.type
    local reward_list = self.data:GetRewardList(qua)
    local jump_list = self.data:GetJumpList(qua)
    self.RewardList:SetData(reward_list)
    self.JumpList:SetData(jump_list)
    local can_get_free = self.data:IsFreeReward()
    UH.SetInter(self.FreeInter, can_get_free)
    self.FreeRedPoint:SetNum(can_get_free and 1 or 0)
end

function ActPetStrPanel:OnTabClick(data)
    self.sel_data = data
    self:FlushPanel()
end

function ActPetStrPanel:OnGotoClick()
    ViewMgr:OpenViewByKey(Mod.Pet.PetStrChange)
end

function ActPetStrPanel:OnFreeClick()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_PET_STR, ActPetStrCtrl.ReqType.FreeReward)
end





ActPetStrTabItem = ActPetStrTabItem or DeclareMono("ActPetStrTabItem", UIWidgetBaseItem)
function ActPetStrTabItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name)
    local remind = ActPetStrData.Instance:GetTabRed(data.type)
    self.RedPoint:SetNum(remind)
end

ActPetStrJumpItem = ActPetStrJumpItem or DeclareMono("ActPetStrJumpItem", UIWidgetBaseItem)
function ActPetStrJumpItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.get_name)
    self.Flag:SetActive(data.is_proposal == 1)
end

function ActPetStrJumpItem:OnClick()
    ViewMgr:OpenViewByKey(self.data.open_panel)
end


ActPetStrRewardItem = ActPetStrRewardItem or DeclareMono("ActPetStrRewardItem", UIWidgetBaseItem)
function ActPetStrRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item_list = DataHelper.FormatItemList(data.reward_item)
    self.ItemList:SetData(item_list)
    local flag = ActPetStrData.Instance:GetFlag(data)
    self.Flag0:SetActive(flag == 0)
    self.Flag1:SetActive(flag == 1)
    self.FlagGeted:SetActive(flag == -1)
    
    local qua = data.type
    local pet_type = ColorStr(Language.Pet.ColorLevelT[qua], COLORSTR.Green9)
    local num_str = ColorStr(" +".. data.st_level, COLORSTR.Green9)
    local title = string.format(Language.Pet.PetStrActTxt1, pet_type, num_str)
    UH.SetText(self.Title, title)
end

function ActPetStrRewardItem:OnGetClick()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_PET_STR, ActPetStrCtrl.ReqType.GetReward,self.data.type,self.data.seq1)
end