IllustrateTreasureView = IllustrateTreasureView or DeclareView("IllustrateTreasureView","serveractivity/illustrate_treasure_view",Mod.RandActivity.IllustrateTreasure)
-- VIEW_DECLARE_LEVEL(IllustrateTreasureView, ViewLevel.L1)
function IllustrateTreasureView:OnClickClose()
    ViewMgr:CloseView(IllustrateTreasureView)
end

IllustrateTreasurePanel = IllustrateTreasurePanel or DeclareMono("IllustrateTreasurePanel", UIWFlushPanel)

function IllustrateTreasurePanel:IllustrateTreasurePanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.llustrate_treasure_data,func = self.FlusheTreasure,init_call = true}
    }
end

function IllustrateTreasurePanel:FlusheTreasure()
    self.Time:SetCallBack(function()
        ViewMgr:CloseView(IllustrateTreasureView)   
    end)
    self.Time:StampTime(self.data:GetIllustrateTreasureTimeStamp(),TimeType.Type_Special_4)
    self.List:SetData(self:SortList(Config.tujian_treasure_auto.treasure_reward))
    self.RewardList:SetData(Config.tujian_treasure_auto.login_reward)
end

function IllustrateTreasurePanel:SortList(list)
    table.sort(list,function(a,b)
        a.widget = self:GetFetch(a.seq,a.parameter3)
        b.widget = self:GetFetch(b.seq,b.parameter3)
        return a.widget < b.widget
    end)
    return list
end

function IllustrateTreasurePanel:GetFetch(seq,param)
    local info = self.data:GetIllustrateTreasureData(seq) or {}
    if info.is_fetch and info.is_fetch == 1 then
        return 1000 + seq
    elseif info.count and info.count >= param then
        return -1000 + seq
    end
    return seq
end

----------------------------IllustrateTreasureItem----------------------------
IllustrateTreasureItem = IllustrateTreasureItem or DeclareMono("IllustrateTreasureItem", UIWidgetBaseItem)

function IllustrateTreasureItem:SetData(data)
    self.Call:SetData(Item.New(data.item))
    UH.SetText(self.Desc,data.describe)

    local info = ServerActivityData.Instance:GetIllustrateTreasureData(data.seq)
    local count,param = info.count or 0,data.parameter3
    count = count > param and param or count

    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.BtnInter.gameObject, Vector3.New(74,21,0))
    end
    local is_fetch = info.is_fetch or 0

    local str
    if count >= param then
        str = Language.IllustrateTreasure.Progress .. Format(Language.IllustrateTreasure.Progress2, count, param)
        UH.SetText(self.Progress, ColorStr(str, COLORSTR.Green9))
    else
        str = Language.IllustrateTreasure.Progress .. ColorStr(Format(Language.IllustrateTreasure.Progress2, count, param), COLORSTR.Red14)
        UH.SetText(self.Progress, ColorStr(str, COLORSTR.Red17))
    end
    
    if is_fetch == 0 then
        UH.SetText(self.BtnName,Language.Recharge.BtnState[0])
        self.BtnInter.Interactable = count >= param
        self.NotFhish:SetActive(count < param)
        self.red_point:SetNum(count >= param and 1 or 0)

        self.GoInter.Interactable = FunOpen.Instance:GetFunIsOpened(data.open_panel)
        if self.GoInter.Interactable then
            UH.SetText(self.GoName,Language.DailyGuildTask.GoTo)
        else
            UH.SetText(self.GoName,Format(Language.Activity.ActItem.LevelLimit,FunOpen.Instance:GetFunOpenLevel(data.open_panel)))
        end
    else
        self.BtnInter.Interactable = false
        UH.SetText(self.BtnName,Language.Recharge.BtnState[1])
        self.NotFhish:SetActive(false)
        self.red_point:SetNum(0)
    end
    UIWidgetBaseItem.SetData(self, data)
end

function IllustrateTreasureItem:OnClickEnter()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ILLUSTRATE_TREASURE,2,self.data.seq)
end

function IllustrateTreasureItem:OnClickGo()
    local opened, open_tip = FunOpen.Instance:GetFunIsOpened(self.data.open_panel)
    if not opened then
        PublicPopupCtrl.Instance:Center(open_tip)
        return
    end
    ViewMgr:OpenViewByKey(self.data.open_panel)
    ViewMgr:CloseView(IllustrateTreasureView)
end

----------------------------IllustrateTreasureRewardItem----------------------------
IllustrateTreasureRewardItem = IllustrateTreasureRewardItem or DeclareMono("IllustrateTreasureRewardItem", UIWidgetBaseItem)

function IllustrateTreasureRewardItem:SetData(data)
    self.Call:SetData(Item.New(data.item))
    UH.SetText(self.Title,Format(Language.IllustrateTreasure.SignDays, ColorStr(data.sign_day, COLORSTR.Red14)))

    local days = ServerActivityData.Instance:GetIllustrateTreasureDays()
    local flag = ServerActivityData.Instance:GetIllustrateTreasureFlag(data.seq or 0)
    local can_reward = flag == 0 and days >= data.sign_day
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.Call.gameObject, Vector3.New(40,40,0)) 
    end
    self.Block:SetActive(can_reward)
    self.red_point:SetNum(can_reward and 1 or 0)
    self.Shade:SetActive(flag == 1)
    self.YiLingQu:SetActive(flag == 1)
    UIWidgetBaseItem.SetData(self, data)
end

function IllustrateTreasureRewardItem:OnClickItem()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ILLUSTRATE_TREASURE,1,self.data.seq)
end