

AchieveSubView = AchieveSubView or DeclareMono("AchieveSubView",UIWFlushPanel)
--AchieveList====UIWAccordion
--SelectList====GameObject
--RecentList====UIWidgeList
--AboutList====UIWidgeList


function AchieveSubView:AchieveSubView()
    self.data_instance = AchieveData.Instance
    self.AchieveList:SetData(self.data_instance:GetGroupData()--[[,true]])
    -- self.AchieveList:ClickChild(self.data_instance:GetGroupData()[1].children[1])
    self.data_cares = {
        {data = self.data_instance.base_data , func = self.FlushRecentAndAboutData, keys = {"recv_fetch"} , init_call = true }
    }
end

function AchieveSubView:FlushRecentAndAboutData()
    self.data_instance:SetRecentAndAboutComplete()
    self.RecentList:SetData(self.data_instance.base_data.complete_info[self.data_instance.RA.r])
    self.AboutList:SetData(self.data_instance.base_data.complete_info[self.data_instance.RA.a])
end

AchieveRightAllView = AchieveRightAllView or DeclareMono("AchieveRightAllView",UIWFlushPanel)
--TotalProgress====UIWProgressBar
--GrowupProgress====UIWProgressBar
--WorkshopProgress====UIWProgressBar
--SocialProgress====UIWProgressBar
--TaskProgress====UIWProgressBar


function AchieveRightAllView:AchieveRightAllView()
    self.data_instance = AchieveData.Instance
    self.progress = self.data_instance.progress
    self.data_cares = {
        {data = self.progress, func = self.FlushProgress}
    }
end


function AchieveRightAllView:FlushProgress()
    self.data_instance:SetProgress()
    local total_value = self.data_instance:GetProgress(self.data_instance.P.Total)
    local total_all = self.data_instance:GetTotalMaxProgress()
    if total_all ~= 0 then self.TotalProgress:SetProgress(total_value / total_all) end
    self.TotalProgress:SetText(tostring(total_value).." / "..tostring(total_all))

    local grow_value = self.data_instance:GetProgress(self.data_instance.P.g)
    local grow_all = self.data_instance:GetMaxProgressByGroupID(self.data_instance.P.g)
    if grow_all ~= 0 then self.GrowupProgress:SetProgress(grow_value / grow_all) end
    self.GrowupProgress:SetText(tostring(grow_value).." / "..tostring(grow_all))

    local work_value = self.data_instance:GetProgress(self.data_instance.P.w)
    local work_all = self.data_instance:GetMaxProgressByGroupID(self.data_instance.P.w) 
    if work_all ~= 0 then self.WorkshopProgress:SetProgress(work_value / work_all) end
    self.WorkshopProgress:SetText(tostring(work_value).." / "..tostring(work_all))

    local social_value = self.data_instance:GetProgress(self.data_instance.P.s)
    local social_all = self.data_instance:GetMaxProgressByGroupID(self.data_instance.P.s)
    if social_all ~= 0 then self.SocialProgress:SetProgress(social_value / social_all) end
    self.SocialProgress:SetText(tostring(social_value).." / "..tostring(social_all))

    local task_value = self.data_instance:GetProgress(self.data_instance.P.t)
    local task_all = self.data_instance:GetMaxProgressByGroupID(self.data_instance.P.t)
    if task_all ~= 0 then self.TaskProgress:SetProgress(task_value / task_all) end
    self.TaskProgress:SetText(tostring(task_value).." / "..tostring(task_all))
end


AchieveListView = AchieveListView or DeclareMono("AchieveListView",UIWAccordion)
--AchieveName=====Text
--GranddSonProgress====UIWProgressBar
--RightAll====GameObject
--LeftBindRight====GameObject
--SelectList====UIWidgetList
--SelectBtnName====Text


function AchieveListView:AchieveListView()
    UIWAccordion:UIWAccordion()
    self.RightAll:SetActive(true)
    self.LeftBindRight:SetActive(false)
    self.data_instance = AchieveData.Instance
end

function AchieveListView:ClickTitle()
    self.RightAll:SetActive(true)
    self.LeftBindRight:SetActive(false)
end

function AchieveListView:OnClickParent(info)
    -- if info.is_sel then 
    --     self.RightAll:SetActive(false)
    --     self.LeftBindRight:SetActive(true)
    -- else
    --     self.RightAll:SetActive(true)
    --     self.LeftBindRight:SetActive(false)
    -- end
end

function AchieveListView:OnClickChild(data)
    UIWAccordion.OnClickChild(self)
    self.SelectBtnName.text = Language.Achieve.All
    self.AchieveName.text = data.name.."："
    local select_data = self.data_instance:FindAchieveByGroupIdAndTypeId(data.group_id,data.type_id)
    --Debuger.LogError("table len :"..tostring(#select_data))
    if data.group_id == 1 and data.type_id == 2 then
        select_data = AchieveData.Instance:GetProfSelectData(select_data)
    end
    self.AchieveDetailList:SetData(select_data)
    self.data_instance:SetSelectData(select_data)
    
    
    self:AddSelectListData(select_data)
    local total = self.data_instance:GetMaxProgressByGroupIDAndTypeID(select_data,data.group_id,data.type_id)
    local value = self.data_instance:SumGrandSonProgress(data.group_id,data.type_id)
    if total ~= 0 then
    self.GranddSonProgress:SetProgress(value / total)
    end
    self.GranddSonProgress:SetText(tostring(value).." / "..tostring(total))
    --Debuger.LogError(tostring(data.group_id)..":"..tostring(data.type_id))

end

function AchieveListView:ClickItemChild()
    self.RightAll:SetActive(false)
    self.LeftBindRight:SetActive(true)
end

function AchieveListView:AddSelectListData(select_data)
    local select_list_data = {}
    local pre_name = ""
    table.insert(select_list_data,{name = Language.Achieve.All})
    table.insert(select_list_data,{name = Language.Achieve.Reach})
    table.insert(select_list_data,{name = Language.Achieve.NotReach})
    -- for k, v in pairs(select_data) do
    --     if v.name ~= pre_name then 
    --         table.insert(select_list_data,v)
    --         pre_name = v.name
    --     end
    -- end
    
    self.SelectList:SetData(select_list_data)
    self.SelectList:ClickItem(1)
end


SelectBtnView = SelectBtnView or DeclareMono("SelectBtnView",UIWAccordion)

function SelectBtnView:SelectBtnView()

end

function SelectBtnView:ClickParent()

end

function SelectBtnView:ClickChild()

end

ParentBtn = ParentBtn or DeclareMono("ParentBtn")
--SelectList====GameObject
--BtnName====Text
--UpImage====GameObject
--DownImage====GameObject

function ParentBtn:ParentBtn()
    self.click_count = 1
    self.BtnName.text = Language.Achieve.All
end

function ParentBtn:ClickParent()
    --Debuger.LogError(tostring(self.click_count))
    if self.click_count % 2 == 0 then 
        self.SelectList:SetActive(false)
        self.DownImage:SetActive(true)
        self.UpImage:SetActive(false)
    else
        self.SelectList:SetActive(true)
        self.DownImage:SetActive(false)
        self.UpImage:SetActive(true)
    end
    if self.click_count < 2 then
        self.click_count = self.click_count + 1
    else 
        self.click_count = 1
    end
end

SelectItem = SelectItem or DeclareMono("SelectItem",UIWidgetBaseItem)
--Name====Text
--BtnName====Text
--AchieveDetailList====UIWidgetList
--SelectList====GameObject
--UpImage====GameObject
--DownImage====GameObject
--ParentBtn====ParentBtn

function SelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.Name.text = data.name
    self.data = data
end

function SelectItem:ClickItem()
    self.BtnName.text = self.data.name
    self:SelectData()
    self.SelectList:SetActive(false)
    self.ParentBtn.click_count = 1
    self.DownImage:SetActive(true)
    self.UpImage:SetActive(false)
end

function SelectItem:SelectData()
    self.data_instance = AchieveData.Instance
    if self.data.name == Language.Achieve.All then
        self.AchieveDetailList:SetData(self.data_instance:GetSelectData())
    elseif self.data.name == Language.Achieve.Reach then
        --已达成
        self.AchieveDetailList:SetData(self.data_instance:GetSubCompleteOrNot().reach)
    elseif self.data.name == Language.Achieve.NotReach then
        --未达成
        self.AchieveDetailList:SetData(self.data_instance:GetSubCompleteOrNot().no)
    else
        local select_table = {}
        local select_data = self.data_instance:GetSelectData()
        --Debuger.LogError(tostring(self.data.name))
        for k, v in pairs(select_data) do
        --Debuger.LogError(tostring(v.name))
            if self.data.name == v.name then 
                table.insert(select_table,v)
            end
        end
        self.AchieveDetailList:SetData(select_table)
    end
end



AchieveSubViewAccordionItemParent = DeclareMono("AchieveSubViewAccordionItemParent", UIWidgetBaseItem)

function AchieveSubViewAccordionItemParent:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.data = data
    self.AchieveTypeName.text = data.name
end



AchieveSubViewAccordionItemChild = DeclareMono("AchieveSubViewAccordionItemChild",UIWidgetBaseItem)

function AchieveSubViewAccordionItemChild:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    --获取数据
    self.data = data
    self.AchieveName.text = data.name
end

function AchieveSubViewAccordionItemChild:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
    if self.Toggle then
        self.Toggle:Trigger()
    end
end


--子项
AchieveItem = DeclareMono("AchieveItem",UIWidgetBaseItem)
--AchieveNameText====Text
--AchieveContentText====Text
--ProgressInfoText====Text
--RewardTxt====Text
--HuobiImage====Text
--HuobiNum====Text
--CompleteIcomImage====GameObject
--Progress====UIWProgressBar

function AchieveItem:AchieveItem()
    self.data_instance = AchieveData.Instance
end


--在父列表中自动调用子项的SetData,前提：在子项的LuaUIEventParamLuaDel组件中调用GetData
function AchieveItem:SetData(data)
    self.data = data
    UIWidgetBaseItem.SetData(self,data)
    local item = Item.Create({item_id = data.reward_item[0].item_id})
    self.ItemCell:SetData(item)
    --self.ItemCell:SetDataByItemId(data.reward_item[0].item_id)
    --Debuger.LogError("name:"..item:IconId())
   -- UH.SetIcon(self.ItemICon,item:IconId())
    self.max_progress = self.data_instance:GetMaxProgressBySeq(data.seq)
    local progress_value = 0
    self.isReach = self.data_instance:GetFetchBySeq(data.seq)
    self.CompleteIcomImage:SetActive(self.isReach) 
    if self.isReach then
       progress_value = self.max_progress
    else 
       progress_value = self.data_instance:GetProgressBySeq(data.seq)
    end    

    self.AchieveNameText.text = data.name
    self.AchieveContentText.text = data.desc
    
    --Debuger.LogError(tostring(data.seq))
    self.ProgressInfoText.text = Language.Achieve.Progress 
    self.Progress:SetProgress(progress_value / self.max_progress)
    self.Progress:SetText(tostring(progress_value).." / "..tostring(self.max_progress))
end

function AchieveItem:ClickCell()
    local info = {
        item_list = {Item.Create({item_id = self.data.reward_item[0].item_id})}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end