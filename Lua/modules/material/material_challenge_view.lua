MaterialChallengeView = MaterialChallengeView or DeclareView("MaterialChallengeView", "material/material_challenge")

function MaterialChallengeView:OnClickClose()
    if not ViewMgr:IsOpen(FightFBView) then
		local _,param_t = Mod.ParseKey(Mod.FightFB.Material)
        param_t.is_not_show = true
        ViewMgr:OpenView(FightFBView,param_t)
    end
    ViewMgr:CloseView(MaterialChallengeView)
end

function MaterialChallengeView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(86)
end

--===========================MaterialChallengePanel===========================
MaterialChallengePanel = MaterialChallengePanel or DeclareMono("MaterialChallengePanel", UIWFlushPanel)

function MaterialChallengePanel:MaterialChallengePanel()
    self.data = MaterialData.Instance
    self.data_cares = {
        {data = self.data.info_data, func = self.FlushTimesView,init_call = false},
        {data = self.data.reward_data, func = self.FlushRewardView,init_call = false},
    }
    self:RegisterGetGuideUi()
end

function MaterialChallengePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushRewardView()
    self:FlushChallengeView()
end

function MaterialChallengePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.ShowTimer)
end

function MaterialChallengePanel:FlushChallengeView()
    local data = self.data:GetSelectChapterData()
    local list = self.data:GetChapterList(data.seq)
    local index = self:GetRecommendIndex(list) or 1
    self.List:SetDataList(list,index)
    self.List:SetCompleteCallBack(function()
        if GuideManager.Instance:Guide() == nil then
            if index > 4 then
                self.ScrollRect.verticalNormalizedPosition = 0
            end
            self:FlushPositionView()
        end
    end)
    UH.SetText(self.Title,data.chapter)
end

function MaterialChallengePanel:FlushRewardView()
    local data = self.data:GetSelectChapterData()
    local curr_num = self.data:GetScoreNum(data.seq)
    self.Progress:SetProgress(curr_num / data.max_score)
    self.Progress:SetText(Format(Language.Material.HasNum,curr_num))
    self.RewardList:SetData(self.data:GetBoxRewardList(data.seq))
end

function MaterialChallengePanel:OnClickItem(data)
    if MaterialData.Instance:IsLastItemFirstKill(data) then
        self.data:SetSelectData(data)
        self:FlushItemView()
    end
end

function MaterialChallengePanel:FlushItemView()
    local data = self.data:GetSelectData()
    UH.SetText(self.Name,Format("%s-%s %s",data.chapter + 1,data.level + 1,data.level_name))
    -- UH.SetText(self.LimitLevel,Format(Language.Material.LimitLevel,data.chapter_level_limit))
    UH.SetText(self.LimitLevel)
    local score = self.data:GetConditonScore(data)
    UH.SpriteName(self.Score,"ConditonScore" .. score)
    self.ScoreGame:SetActive(score ~= 0)
    if self.Texture:TextureIsNil() then
        self.Texture:SetByPath(MapData.Instance:GetMapMiniPath(data.scene_id))
    end
    for i=1,self.Condition:Length() do
        local flag = self.data:GetConditonFlag(data,i)
		self.Condition[i]:SetData({id = data["condition_"..i],flag = flag})
    end
    self.Grid:SetData(self.data:GetRewardList(data))
    UH.SetText(self.ExpNum,self.data:GetExpNum())
    self:FlushTimesView()
end

function MaterialChallengePanel:FlushTimesView()
    local data = self.data:GetSelectData()
    self.sweep_times = self.data:GetSweepTimes(data)
    local COLOR = self.sweep_times > 0 and COLORSTR.Green7 or COLORSTR.Red20
    UH.SetText(self.Times,Format(Language.Material.Times,COLOR,self.sweep_times))
    local strength_num = FightData.Instance:GetFightStrengthNum()
    self.SweepInter.Interactable = self.data:IsFirstKill(data) and self.sweep_times > 0 and strength_num >= self.data:GetFightStrengthNum() and self.data:GetConditonScore(data) == 3
    UH.SetText(self.StrengthNum,self.data:GetFightStrengthNum())
end

function MaterialChallengePanel:OnClickEnter()
    if FightCtrl.IsActFB() then
        return
    end
    local data = self.data:GetSelectData()
    if self.data:GetSweepTimes(data) == 0 and self.data:IsFirstKill(data) then
        PublicPopupCtrl.Instance:Center(Language.Material.NotTiemsTip[self.data:GetBuyTimes(data) == 0 and 1 or 2])
        return
    end
    if FightData.Instance:GetFightStrengthNum() < self.data:GetFightStrengthNum() then
        PublicPopupCtrl.Instance:Center(Language.Material.StrengthTip)
        ViewMgr:OpenView(FightCrystalSweepItemView)
        return
    end
    if TeamCtrl.Instance:Data():InTeam() then
        PublicPopupCtrl.Instance:AlertTip(Language.DreamNotes.TeamChallengeTip,function()
            TeamCtrl.Instance:SendExitTeam()
            self:OnChallenge(data.seq)
        end)
        return 
    end
    self:OnChallenge(data.seq)
end

function MaterialChallengePanel:OnChallenge(seq)
    MaterialCtrl.Instance:SendMaterialReq(0,seq)
    self.data:SetChallengeData(seq)
    ViewMgr:CloseView(FightFBView)
    ViewMgr:CloseView(MaterialChallengeView)
end

function MaterialChallengePanel:OnClickSweep()
    local data = self.data:GetSelectData()
    if not self.data:IsFirstKill(data) or self.data:GetConditonScore(data) ~= 3 then
        PublicPopupCtrl.Instance:Center(Language.Material.NotSweepTip)
        return
    end
    if self.data:GetSweepTimes(data) == 0 then
        PublicPopupCtrl.Instance:Center(Language.Material.NotTiemsTip[self.data:GetBuyTimes(data) == 0 and 1 or 2])
        return
    end
    if FightData.Instance:GetFightStrengthNum() < self.data:GetFightStrengthNum() then
        PublicPopupCtrl.Instance:Center(Language.Material.StrengthTip)
        ViewMgr:OpenView(FightCrystalSweepItemView)
        return
    end
    MaterialCtrl.Instance:SendMaterialReq(1,data.seq)
end

function MaterialChallengePanel:OnClickAdd()
    local data = self.data:GetSelectData()
    if self.data:GetBuyTimes(data) == 0 then
        PublicPopupCtrl.Instance:Center(Language.Material.NotHasBuyTimes)
        return
    end
    PublicPopupCtrl.Instance:AlertTip(Format(Language.Material.BuyTimesTip,self.data:GetTimesPrice(data),self.data:GetBuyTimes(data)),function()
        MaterialCtrl.Instance:SendMaterialReq(3,data.seq)
    end,CurrencyType.Gold)
end

function MaterialChallengePanel:GetRecommendIndex(list)
    local recommend_data = self.data:GetRecommendData()
    if recommend_data ~= nil then
        return DataHelper.SimpleIndex(list,"level",recommend_data.level)
    else
        local index = 1
        for i,v in ipairs(list) do
            if self.data:IsLastItemFirstKill(v) and i > index then
                index = i
            end
        end
        return index
    end
end

function MaterialChallengePanel:RegisterGetGuideUi()
    GuideManager.Instance:RegisterGetGuideUi("MaterialChallengeSelectBtn", function()
        if self.List and self.List.item_list and self.List.item_list[1] then
            self.ScrollRect.verticalNormalizedPosition = 1
            return self.List.item_list[1],function()
                self.List.item_list[1]:Click()
            end
        end
	end)
end

----------------------------滑动相关----------------------------
function MaterialChallengePanel:OnEndDrag()
    TimeHelper:CancelTimer(self.ShowTimer)
    self.ShowTimer = TimeHelper:AddCountDownTT(function()
        self:FlushPositionView()
	end,
    function()end,1,0.01)
end

function MaterialChallengePanel:OnDrag()
    self:FlushPositionView()
end
--Rect 目标List节点 必须是中心节点
function MaterialChallengePanel:FlushPositionView()
    for i,v in ipairs(self.List.item_list) do
        v:SetPosition(self.Rect.localPosition)
    end
end
----------------------------MaterialChallengeItem----------------------------
MaterialChallengeItem = MaterialChallengeItem or DeclareMono("MaterialChallengeItem", UIWidgetBaseItem)

function MaterialChallengeItem:SetData(data)
    UH.SetText(self.Name,Format("%s-%s  %s",data.chapter + 1,data.level + 1,data.level_name))
    for i=1,self.GrayList:Length() do
		self.GrayList[i].enabled = MaterialData.Instance:GetConditonFlag(data,i) == 0
    end
    self.Inter.Interactable = MaterialData.Instance:IsLastItemFirstKill(data)
    self.Block:SetActive(not self.Inter.Interactable)
    UIWidgetBaseItem.SetData(self, data)
end

function MaterialChallengeItem:OnClickBlock()
	PublicPopupCtrl.Instance:Center(Language.CourageChallenge.LockTips[2])
end

function MaterialChallengeItem:Click()
    self.Toggle:Trigger()
end

--圆形滑动 Panel
--m_height 三角形的高（一半）
--m_width  三角形的宽（一半 因为Item起始位置是三角形的一半左右）
function MaterialChallengeItem:SetPosition(root)
    local m_width,m_height = 70,245
    local pos = UH.GetRootPosition(self.Panel, root)
    local height,ratio = math.abs(pos.y),m_width / m_height
    UH.SetRectPosition(self.Panel,{x = height > m_height and 0 or (m_height - height) * ratio})
end

----------------------------MaterialChallengeConditionItem----------------------------
MaterialChallengeConditionItem = MaterialChallengeConditionItem or DeclareMono("MaterialChallengeConditionItem", UIWidgetBaseItem)

function MaterialChallengeConditionItem:SetData(data)
    local desc = CourageChallengeData.Instance:GetConditionDesc(data.id)
    UH.SetText(self.Desc,data.flag == 1 and ColorStr(desc,COLORSTR.Green7) or desc)
    self.Complete:SetActive(data.flag == 1)
end

----------------------------MaterialRewardItem----------------------------
MaterialRewardItem = MaterialRewardItem or DeclareMono("MaterialRewardItem", UIWidgetBaseItem)

function MaterialRewardItem:SetData(data)
    local num = MaterialData.Instance:GetScoreNum(data.chapter)
    if data.max_score ~= 0 then
        local flag = MaterialData.Instance:GetRewardFlag(data.seq)
        self.Effect:SetActive(flag == 0 and num >= data.max_score)
        self.Gray.enabled = flag == 1
        UH.SetText(self.Num,data.max_score)
    end
    self.Panel:SetActive(data.max_score ~= 0)
    self.Label:SetActive(num >= data.max_score)
    UIWidgetBaseItem.SetData(self, data)
end

function MaterialRewardItem:OnClickItem()
    if self.Effect.activeSelf then
        MaterialCtrl.Instance:SendMaterialReq(2,self.data.seq)
	    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.OpenGift)
    else
		ViewMgr:OpenView(SilkRoadPreview,{
            pos = Vector3.New(-505,-100),
            desc = self.Gray.enabled and Language.Material.NoramlGetTip or nil,
            reward_list = MaterialData.Instance:GetRewardItemList(self.data)
        })
    end
end