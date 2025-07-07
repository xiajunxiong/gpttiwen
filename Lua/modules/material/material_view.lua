MaterialView = MaterialView or DeclareMono("MaterialView", UIWFlushPanel)

function MaterialView:MaterialView()
    self.make_num = 0
    self.data = MaterialData.Instance
    self.data_cares = {
        {data = self.data.reward_data, func = self.FlushRedNumView,init_call = false},
    }
    self:RegisterGetGuideUi()
end

function MaterialView:Awake()
    UIWFlushPanel.Awake(self)
    self.is_awake = true
    self.data_list = self.data:GetMaterialList()
    self.List:SetCompleteCallBack(function()
        if GuideManager.Instance:Guide() == nil then
            self:CleckRecommend()
            self:FlushPositionView()
        end
    end)
    self.List:SetData(self.data_list)
    self:FlushRewardView()
    self:FlushButtonView()
    self:OnEndDrag()
end

function MaterialView:OnDestroy()
    self.is_awake = false
    UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.ShowTimer)
end

function MaterialView:FlushRewardView(data)
    local data = data or self.data_list[self.make_num + 2] or {}
    local score = self.data:GetScoreNum(data.seq)
    local reward_list = DataHelper.SplitData(data.reward_show,":")
    UH.SetText(self.RewardNum,Format("%s/%s",score,data.max_score))
    self.RewardList:SetData(DataHelper.FormatItemList(reward_list))
end

function MaterialView:FlushRedNumView()
    for i,item in ipairs(self.List.item_list) do
        item:FlushRedNumView()
    end
end

function MaterialView:FlushPositionView(IsDrag)
    local index = DataHelper.SimpleCondIndex(self.List.item_list,function(item)
        return item and item:SetPosition(self.Rect.localPosition) == true and IsDrag == true
    end)
    if index ~= nil then
        self:FlushMaterialView(index - 2)
        self.ScrollRect:StopMovement()
    end
end

function MaterialView:FlushMaterialView(index)
    self.make_num = index
    self.List:ForceJumpHorizontal(-411 * self.make_num)
    self:FlushButtonView()
    self:FlushRewardView()
end

function MaterialView:FlushButtonView()
    self.BtnLeft:SetActive(self.make_num > 0)
    self.BtnRight:SetActive(self.make_num < #self.data_list - 3)
end

function MaterialView:OnClickItem(data)
    if self.make_num ~= data.seq then
        self:FlushMaterialView(data.seq)
        self:FlushPositionView()
    else
        if self.data:IsLastFirstKill(data) then
            self.data:SetSelectChapterData(data)
            ViewMgr:OpenView(MaterialChallengeView)
        else
            local config = self.data:GetChapterList(data.seq-1)[#self.data:GetChapterList(data.seq-1)]
            PublicPopupCtrl.Instance:Center(Format(Language.Material.ChapterTip,config.chapter_name,config.level_name))
        end
    end
end

function MaterialView:OnClickTurn(num)
    self:FlushMaterialView(self.make_num + num)
    self:FlushPositionView()
end

function MaterialView:OnEndDrag()
    self.ShowTimer = TimeHelper:AddCountDownTT(function()
        self:FlushPositionView(true)
	end,
    function()
    end,1,0.01)
end

function MaterialView:OnDrag()
    self:FlushPositionView(true)
end

function MaterialView:CleckRecommend()
    local recommend = self.data:GetRecommendData()
    if recommend ~= nil then
        local index = DataHelper.SimpleCondIndex(self.data_list,function(data)
            return data.seq and data.seq == recommend.chapter
        end)
        self.data:SetRecommendData()
        if index ~= nil then
            self:FlushMaterialView(index - 2)
        end
    else
        local index = 0
        for i,v in ipairs(self.data_list) do
            if v.seq and self.data:IsLastFirstKill(v) and i > index then
                index = v.seq
            end
        end
        self:FlushMaterialView(index)
    end
end

function MaterialView:RegisterGetGuideUi()
    GuideManager.Instance:RegisterGetGuideUi("MaterialLevelSelectBtn", function()
        if self.List and self.List.item_list and self.List.item_list[2] and self.is_awake then
            self:FlushMaterialView(1)
            return self.List.item_list[2]
        end
	end)
end

----------------------------MaterialCall----------------------------
MaterialCall = MaterialCall or DeclareMono("MaterialCall", UIWidgetBaseItem)

function MaterialCall:SetData(data)
    self.Call:SetData(data)
end

----------------------------MaterialItem----------------------------
MaterialItem = MaterialItem or DeclareMono("MaterialItem", UIWidgetBaseItem)

function MaterialItem:SetData(data)
    if data.chapter ~= nil then
        UIWidgetBaseItem.SetData(self, data)
        UH.SetText(self.Name,data.chapter or "")
        UH.SpriteName(self.ItemImg,"Chapter" .. data.seq)
        UH.SetText(self.Title,Format(Language.Material.Chapter,DataHelper.GetDaXie(data.seq + 1)))
        self.Inter.Interactable = MaterialData.Instance:IsLastFirstKill(data)
        self.Image:SetActive(not self.Inter.Interactable)
        self.RedPos:SetNum(MaterialData.Instance:GetChapterNum(data.seq))
    end
    self.Panel:SetActive(data.chapter ~= nil)
end

function MaterialItem:FlushRedNumView()
    if self.data and self.data.seq then
        self.RedPos:SetNum(MaterialData.Instance:GetChapterNum(self.data.seq))
    end
end

function MaterialItem:SetPosition(root)
    if not self.Panel.activeSelf then return end
    local pos = UH.GetRootPosition(self.gameObject,root)
    local size = Valve(self:GetWidthNum(math.abs(pos.x-2)),1)
    size = size <= 0.85 and 0.85 or size
    UH.LocalScaleG(self.Panel,Vector3.New(size,size,size))
    if math.abs(pos.x-2) < 50 then
        UH.LocalScaleG(self.Panel,Vector3.one)
        return true
    end
    return false
end

function MaterialItem:GetWidthNum(width)
    if width > 1644 then return 0.5 end
    return (1/1644) * (1644 - width)
end