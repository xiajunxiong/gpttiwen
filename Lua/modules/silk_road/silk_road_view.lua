SilkRoadView = SilkRoadView or DeclareView("SilkRoadView", "silk_road/silk_road_view",Mod.SilkRoad.Main)

function SilkRoadView:OnClickClose()
    ViewMgr:CloseView(SilkRoadView)
end

function SilkRoadView:LoadCallback()
    local reward_list = SilkRoadData.Instance:GetRewardList()
    for i=1,self.RewardList:Length() do
        if reward_list[i] then
            self.RewardList[i]:SetData(reward_list[i])
        end
    end
end

function SilkRoadView:CloseCallback()
    SilkRoadData.Instance:Initialization()
    ItemInfoCtrl.Instance:Data():SetItemInfoData()
end

function SilkRoadView:OpenCallback()
    SilkRoadCtrl.Instance:SendTransactionTaskReq()
end

--===========================SilkRoadPanel===========================
SilkRoadPanel = SilkRoadPanel or DeclareMono("SilkRoadPanel", UIWFlushPanel)

function SilkRoadPanel:SilkRoadPanel()
    self.data = SilkRoadData.Instance
    self.data_cares = {
        {data = self.data.single_data,  func = self.FlushSilkRoad,init_call = true},
        {data = self.data.select_item,  func = self.FlushItemView,init_call = false},
        {data = BagData.Instance.item_grid,func = self.FlushSelectItem,init_call = false},
    }
end

function SilkRoadPanel:FlushItemView()
    local data = self.data:GetSelectData()
    if data == nil then
        return
    end
    local reward = data.reward
    if not IsEmpty(data.money_num) then
        reward.item_id = CommonItemId.ImmortalCoin
        reward.num = data.money_num
    end
    self.Call:SetData(data.item)
    UH.SetText(self.Name,data.item:QuaName())
    UH.SetText(self.Num,"x ".. reward.num)
    UH.SetIcon(self.Icon, Item.GetIconId(reward.item_id), ICON_TYPE.ITEM)
    self:FlushBtnStateView(data)
    self.GetWay:SetActive(false)
end

function SilkRoadPanel:FlushSelectItem()
    if self.IsCanReceive then
        return
    end
    local data = self.data:GetSelectData()
    if self.List.index_item[data.index] then
        self.List.index_item[data.index]:SetData(data)
    end
    self:FlushBtnStateView(self.data:GetSelectData())
end

function SilkRoadPanel:FlushSilkRoad()
    local num = self.data:GetCompleteNum()
    local index = self.data:GetCanSelectIndex()
    if index == 0 then
        if #self.List:GetSelectedData() > 0 then
            self.List:CancelSelect()
        end
        self.List:SetData(self.data:GetSilkRoadSingleList())
        self.ItemCell:SetActive(false)
    else
        self.List:SetDataList(self.data:GetSilkRoadSingleList(),index)
    end
    UH.SetText(self.Progress,Format(Language.SilkRoad.ProgressTip,num,9))
    self.IsCanReceive = num >= 9
    self.Effect:SetActive(self.IsCanReceive and self.data:GetStatus() == 0)
    if self.IsCanReceive then
        UH.SetText(self.Submission,Language.SilkRoad.Prepared)
        self.BtnFind:SetActive(false)
    end
end

function SilkRoadPanel:FlushBtnStateView(data)
    if data.index then
        local num = Item.GetNum(data.need_item.item_id)
        local flag = SilkRoadData.Instance:GetSingleFlag(data.index)
        self.can_submission = num >= data.need_item.num and flag == 0
        local Submission = Language.SilkRoad.Submission
        UH.SetText(self.Submission,Submission[self.can_submission and 0 or (data.get_way == "" and 2 or 1)])
    end
    if self.IsCanReceive then
        UH.SetText(self.Submission,Language.SilkRoad.Prepared)
        self.GetWay:SetActive(false)
    end
end

function SilkRoadPanel:OnClickSubmission()
    if self.IsCanReceive then
        return
    end
    local data = self.data:GetSelectData()
    if data.need_item then
        if self.can_submission then
            SilkRoadCtrl.Instance:SendTransactionTaskReq(1,data.index - 1,data.need_item.num)
        else
            local way_list = string.split(data.get_way or "", "|")
            local item_id = data.need_item.item_id
            if data.get_way == "" or #way_list == 1 then
                if data.open_panel == Mod.Mall.Shop then
                    CommerceData.Instance:SetJumpIndex(item_id)
                elseif data.open_panel == Mod.Mall.Market then
                    MarketData.Instance:SetJumpIndex(item_id)
                elseif data.open_panel == Mod.Fight.Gather then
                    local config = FightData.Instance:GetGatherFindType(item_id)
                    FightData.Instance:SetGatherSelectType(config.scene_type_1)
                    FightData.Instance.cur_sel_type = config.seq or 1
                elseif data.open_panel == Mod.Reputation.Main then
                    MallData.JumpItemNum = data.need_item.num - Item.GetNum(item_id)
                    MallCtrl.Instance:Jump(item_id,true)
                    TaskData.shop_auto_close = true
                    TaskData.shop_auto_open = true
                    ViewMgr:CloseView(SilkRoadView)
                    ReputationCtrl.Instance:SetSelectTaskCache(TaskConfig.TaskType.silk_road)
                    return
                end
                TaskData.shop_auto_close = true
                MallData.JumpItemNum = data.need_item.num - Item.GetNum(item_id)
                ViewMgr:OpenViewByKey(data.open_panel)
            else
                self.GetWay:SetActive(not self.GetWay.activeSelf)
                self.GetWayList:SetData(way_list)
                data.item.vo.jump_need_num = data.need_item.num - Item.GetNum(item_id)
                ItemInfoCtrl.Instance:Data():SetItemInfoData(data.item)
            end
        end
    end
end

function SilkRoadPanel:OnClickReceive()
    if self.data:GetStatus() == 0 and self.IsCanReceive then
        SilkRoadCtrl.Instance:SendTransactionTaskReq(2)
        ActivityData.Instance:SetActivityFinishInfo(ActType.SilkRoad,9,9)
        ViewMgr:CloseView(SilkRoadView)
    end
end

function SilkRoadPanel:OnClickFind()
    local num = self.data:GetRandom() - self.data:GetCompleteNum()
    local reward_list,cost_type = self.data:GetFindRewardData()
    ViewMgr:OpenView(DialogTipsBackView,{
        cost_type = cost_type,
        cost = RewardData.Instance:GetFindFree(FindType.SilkRoad,cost_type,num),
        name = Language.SilkRoad.Title,
        rewards = reward_list,
        confirm_func = function()
            TaskCtrl.Instance:SendOnekeyFinish(OnekeyFinishType.SilkRoad)
            ActivityData.Instance:SetActivityFinishInfo(ActType.SilkRoad,9,9)
            ViewMgr:CloseView(SilkRoadView)
            SilkRoadCtrl.IsOnekeyFinish = true
        end
    })
end

----------------------------SilkRoadItem----------------------------
SilkRoadItem = SilkRoadItem or DeclareMono("SilkRoadItem", UIWidgetBaseItem)

function SilkRoadItem:SetData(data)
    data.item = Item.New({item_id = data.need_item.item_id})
    self.Call:SetData(data.item)
    if data.index <= 3 then
        UH.SpriteName(self.Image,"ChangGouZi")
    end
    local flag = SilkRoadData.Instance:GetSingleFlag(data.index)
    local num = Item.GetNum(data.need_item.item_id)
    num = num >= data.need_item.num and data.need_item.num or num
    if flag == 1 then
        UH.SetText(self.Num,Language.SilkRoad.Submissioned)
        self.Mark:SetActive(true)
    else
        local color = num >= data.need_item.num and COLORSTR.Green9 or COLORSTR.Yellow9
        UH.SetText(self.Num,Format("<color=#%s>%s/%s</color>",color,num,data.need_item.num))
        self.Mark:SetActive(false)
    end
    UIWidgetBaseItem.SetData(self, data)
end

function SilkRoadItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected then
        SilkRoadData.Instance:SetSelectData(self.data)
        local val = SilkRoadData.Instance.select_item.val
        SilkRoadData.Instance.select_item.val = not val
    end
end

----------------------------SilkRoadRewardItem----------------------------
SilkRoadRewardItem = SilkRoadRewardItem or DeclareMono("SilkRoadRewardItem", UIWidgetBaseItem)

function SilkRoadRewardItem:SetData(data)
    self.Call:SetData(data)
end

--------------------------------SilkRoadPreview--------------------------------
SilkRoadPreview = SilkRoadPreview or DeclareView("SilkRoadPreview", "silk_road/silk_road_preview")

VIEW_DECLARE_MASK(SilkRoadPreview,ViewMask.BlockClose)

function SilkRoadPreview:LoadCallback(param_t)
    if #param_t.reward_list > 3 then
		self.Content.pivot = Vector2.New(0,0.5)
	end
    UH.LocalPos(self.Root,param_t.pos or Vector3.zero)
    UH.SetEdgePosition(self.Root,self.Rect)
    UH.SetText(self.Desc,param_t.desc or "")
    self.RewardList:SetData(param_t.reward_list)
    self.Finish:SetActive(param_t.is_finish or false)
end