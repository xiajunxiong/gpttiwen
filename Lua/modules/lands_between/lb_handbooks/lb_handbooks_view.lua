LBHandBookView = LBHandBookView or DeclareView("LBHandBookView", "lb_handbook/lb_handbook",Mod.LandsBetweenExtra.HandBooks)
function LBHandBookView:LBHandBookView()

end

function LBHandBookView:OnClickClose()
    ViewMgr:CloseView(LBHandBookView)
end

function LBHandBookView:OpenCallback()

end

LBHandBookPanel = LBHandBookPanel or DeclareMono("LBHandBookPanel", UIWFlushPanel)
function LBHandBookPanel:LBHandBookPanel()
    self.data = LBHandBooksData.Instance

    self.data_cares = {
        {data = self.data.operating, func = self.FlushPanel,init_call = false},
        {data = self.data.info_list, func = self.FlushPanel,init_call = false},
    }
    self.main_list = self.data:GetDescList()

    self.HandBooks:SetCompleteCallBack(function()
        self.HandBooks.item_list[0].ItemClick:Trigger()
    end)
end 

function LBHandBookPanel:Awake()
    UIWFlushPanel.Awake(self)

    self:InitView()
end 

function LBHandBookPanel:InitView()
    self.HandBooks:SetData(self.main_list)
    -- self.GuideList:SetData(self.main_list[0])
    -- self.RewardList:SetData(self.main_list[0].reward_list)
end

function LBHandBookPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

end

function LBHandBookPanel:FlushPanel()
    self.selected = self.main_list[self.data.operating.select]
    self.GuideList:SetData(self.selected.paging_list)
    self.RewardList:SetData(self.selected.reward_list)
    UH.SetText(self.total_title,self.selected.small_title)
    self.RewardInter.Interactable = not self.data:CheckIsRewareded(self.selected.act)
end

function LBHandBookPanel:OnClickHandBooks(data)
    if not self.data:CheckIsReaded(data.act) then
        LBHandBooksCtrl.Instance:SendClicked(data.act)
    end

    self.data:SetSelect(data.act)
end

function LBHandBookPanel:OnClickReward()
    if not self.RewardInter.Interactable then 
        PublicPopupCtrl.Instance:Center(Language.LBHandBook.RewardedError)
        return 
    end 

    LBHandBooksCtrl.Instance:SendReward(self.selected.act)
end


LBHandBookHandsBookCells = DeclareMono("LBHandBookHandsBookCells", UIWidgetBaseItem)
function LBHandBookHandsBookCells:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name,data.act_title)
    UH.SetText(self.SelectName,data.act_title)
    UH.SpriteName(self.Icon, data.act_icon)
end

LBHandBookHandsGuideCells = DeclareMono("LBHandBookHandsGuideCells", UIWidgetBaseItem)
function LBHandBookHandsGuideCells:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Desc,data.paging_dec)
    UH.SpriteName(self.Image, data.paging_pic)
end 