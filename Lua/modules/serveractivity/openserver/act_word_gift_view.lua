ActWordGiftView = ActWordGiftView or DeclareMono("ActWordGiftView", UIWFlushPanel)
function ActWordGiftView:ActWordGiftView()
    self.data_cares = {
        {data = ActWordGiftData.Instance:WordInfo(), func = self.FlushList, init_call = false},
        {data = ActWordGiftData.Instance:TotalInfo(), func = self.FlushList, init_call = false},
        {data = ActWordGiftData.Instance.word_redinfo, func = self.FlushRed, key = {"daily"}}
    }
    self.prefKey = PrefKeys.ActReward(ACTIVITY_TYPE.KF_WORD_GIFT)
end
function ActWordGiftView:FlushRed()
    local flag = ActWordGiftData.Instance.word_redinfo.daily
    self.RedPoint:SetNum(flag == 0 and 1 or 0)
end
function ActWordGiftView:Awake()
    UIWFlushPanel.Awake(self)
    local data = ActWordGiftData.Instance:GetActCfg()
    self.List:SetData(data)
    local value = UnityPlayerPrefs.GetInt(self.prefKey)
    self.GouObj:SetActive(value == TimeCtrl.Instance:GetCurOpenServerDay())
    --Type_Time_3
    self.Time:CreateActTime(ACTIVITY_TYPE.KF_WORD_GIFT,TimeType.Type_Special_4,Language.WordGift.Time)
end
--[[ function ActWordGiftView:OnEnable()
    UIWFlushPanel.OnEnable(self)
end ]]
function ActWordGiftView:FlushList()
    local data = ActWordGiftData.Instance:GetActCfg()
    self.List:SetData(data)
end
function ActWordGiftView:OnClickCloseTip()
    if self.GouObj.activeSelf then
        UnityPlayerPrefs.SetInt(self.prefKey, 0)
    else
        UnityPlayerPrefs.SetInt(self.prefKey, TimeCtrl.Instance:GetCurOpenServerDay())
    end
    self.GouObj:SetActive(not self.GouObj.activeSelf)
    self:FlushList()
    ActWordGiftData.Instance.word_redinfo.red = not self.GouObj.activeSelf
end
function ActWordGiftView:OnClickDailyReward()
    ActWordGiftCtrl.Instance:Daily()
end
ActWordGiftItem = DeclareMono("ActWordGiftItem", UIWidgetBaseItem)
function ActWordGiftItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    for i=1, 5 do
        if data["words"..i] == 0 then
            self.CellList[i].gameObject:SetActive(false)
        else
            self.CellList[i]:SetDataByItemIdWithNeed(data["words"..i],data["number"..i])
        end
    end
    self.CellList[6]:SetData(Item.Create(data.item))
    local num = ActWordGiftData.Instance:WordCount(data.type + 1)
    if data.number == 0 then
        num = ActWordGiftData.Instance:TotalCount(data.type + 1)
        UH.SetText(self.Count, string.format(Language.WordGift.Count2, data.total_exchange - num))
    else
        UH.SetText(self.Count, string.format(Language.WordGift.Count, data.number - num))
    end

    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.BtnObj,Vector3.New(75,21,0))
        self.red_point:SetNum(0)
    end
    local red, real = ActWordGiftData.Instance:GetWordGiftRemind2(data, true)
    self.Gray1.enabled = (real == 0)
    self.Gray2.enabled = (real == 0)
    self.red_point:SetNum(red)
end
function ActWordGiftItem:OnRelease()
    self.red_point = nil
end
function ActWordGiftItem:OnClickGet()
    local num = ActWordGiftData.Instance:WordCount(self.data.type + 1)
    if self.data.number == 0 then
        num = ActWordGiftData.Instance:TotalCount(self.data.type + 1)
        if (self.data.total_exchange - num) == 0 then
            PublicPopupCtrl.Instance:Center(ErrorText[140])
        else
            ActWordGiftCtrl.Instance:Get(self.data.type)
        end
    else
        if (self.data.number - num) == 0 then
            PublicPopupCtrl.Instance:Center(ErrorText[135])
        else
            ActWordGiftCtrl.Instance:Get(self.data.type)
        end
    end
end