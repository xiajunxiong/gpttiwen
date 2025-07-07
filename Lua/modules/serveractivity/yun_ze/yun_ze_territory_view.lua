YunZeTerritoryView = YunZeTerritoryView or DeclareView("YunZeTerritoryView","serveractivity/yun_ze/yun_ze_territory_view",Mod.YunZe.Territory)
VIEW_DECLARE_LEVEL(YunZeTerritoryView, ViewLevel.L0)
function YunZeTerritoryView:OnClickClose()
    ViewMgr:CloseView(YunZeTerritoryView)
end
YunZeTerritoryPanel = YunZeTerritoryPanel or DeclareMono("YunZeTerritoryPanel", UIWFlushPanel)
function YunZeTerritoryPanel:YunZeTerritoryPanel()
    self.Data = YunZeData.Instance
    self.data = YunZeData.Instance.say_territory_data
    self.data_cares = {
        {data = self.data,func = self.FlushPanle,init_call = true},
    }
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_YUN_ZE_TERRITORY,TimeType.Type_Special_4)
    if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end
end
function YunZeTerritoryPanel:PlayEffect()
    if self.effect then
        self.EffectTool:Stop(self.effect)
        self.effect = nil
    end
    self.effect = self.EffectTool:Play(6161007)
end
function YunZeTerritoryPanel:FlushPanle()
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_YUN_ZE_TERRITORY)
    local info = self.Data:GetSayTerritoryData()[open_day]
    self.RewardList:SetData(info.reward_item)
    self.ExploreList:SetData(self.data.explore_list)
    UH.SpriteName(self.LeftImg,string.format(Language.YunZeTerritory.SpriteName1,info.picture))
    --for i = 1 , self.PictureArr:Length() do
    --    self.PictureArr[i]:SetActive(i == info.picture)
    --end
    UH.SetText(self.Times,string.format(Language.YunZeTerritory.Times,Config.yunzehuajing_auto.basics[1].frequency - self.data.times,Config.yunzehuajing_auto.basics[1].frequency))
    self.is_finish = self.data.times == Config.yunzehuajing_auto.basics[1].frequency
    self.Received:SetActive(self.is_finish)
    UH.SpriteName(self.RightImg,string.format(Language.YunZeTerritory.SpriteName2,info.picture))
    --活动最后一天 探索完成  显示已经完成探索  
    local show_text = Language.YunZeTerritory.BtnState[self.is_finish and 2 or 1]
    local activity_info = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_YUN_ZE_TERRITORY)
    if activity_info ~= nil and activity_info.status ~= ActStatusType.Close then
        local total_time = activity_info.next_status_switch_time - TimeHelper.GetServerTime()
        if total_time < 86400 and self.is_finish then
            show_text = Language.YunZeTerritory.BtnState[3]
            self.is_finish = true
        end
    end 
    UH.SetText(self.BtnText,show_text)
    self:SetRedNum()
end
function YunZeTerritoryPanel:OnClickExplore()
    if not self.is_finish then
        self:PlayEffect()
    end
    self.data.is_remind = false
end
function YunZeTerritoryPanel:OnClickError()
    self.data.is_remind = false
    if not self.is_finish then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
        --得到点击位置
        local _, pos =
        UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(
            self.CtrlContentRect,
            UnityEngine.Input.mousePosition,
            self.mainCanvasCom.worldCamera,
            nil)
        UH.AnchoredPosition(self.ErrorTipPos,pos)
        if self.show_error ~= nil then
            TimeHelper:CancelTimer(self.show_error)
            self.show_error = nil
        end
        self:ErrorTipState(true)
        self.show_error = TimeHelper:AddDelayTimer(BindTool.Bind(self.ErrorTipState,self,false),1)
    end
end
function YunZeTerritoryPanel:ErrorTipState(state)
    self.ErrorTipPos.gameObject:SetActive(state)
end
function YunZeTerritoryPanel:SetRedNum()
    local num = self.is_finish == false and self.data.is_remind and 1 or 0
    self.RedPos:SetNum(num)
end
function YunZeTerritoryPanel:OnDestroy()
    if self.show_error then
        TimeHelper:CancelTimer(self.show_error)
        self.show_error = nil
    end
    UIWFlushPanel.OnDestroy(self)
end
--------------------------
------Item
---------------------------
YunZeTerritoryItem = YunZeTerritoryItem or DeclareMono("YunZeTerritoryItem", UIWidgetBaseItem)
function YunZeTerritoryItem:SetData(data)
    if data.show_type then
        local pos_data = ServeractivityConfig.YunZeTerritory.ItemPos[YunZeData.Instance:GetSayTerritoryPicture()][data.index]
        UH.LocalPos(self.Pos, pos_data.pos)
        UH.LocalScale(self.Pos, pos_data.scale or Vector3.one)
        self.ShowIcon:SetActive(data.show_type >= 1)
    else
        self.item = Item.New(data)
        self.Cell:SetData(self.item)
    end
    UIWidgetBaseItem.SetData(self, data)
end
function YunZeTerritoryItem:OnClickItem()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    YunZeData.Instance.say_territory_data.is_remind = false
    if self.data.show_type == 0 and Config.yunzehuajing_auto.basics[1].frequency > YunZeData.Instance.say_territory_data.times then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_YUN_ZE_TERRITORY, 1,self.data.index - 1)
    end
end