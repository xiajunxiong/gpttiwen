FishGroundMenuView = FishGroundMenuView or DeclareView('FishGroundMenuView', 'fish/fish_ground_menu')


FishGroundMenuPanel = FishGroundMenuPanel or DeclareMono("FishGroundMenuPanel", UIWFlushPanel)
function FishGroundMenuPanel:FishGroundMenuPanel()
    self.data = FishData.Instance
    self.fight_data = FightData.Instance
    self.data_cares = {
        {data = self.data:Fish(), func = self.FlushFishingList, key = {'state'}}
    }
end
function FishGroundMenuPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    UH.SetText(self.Name, SceneData.Instance:SceneName())
    UH.SetText(self.Times, self.fight_data.gather_energy.val)
    local list = self.data:FishList()
	self.ItemList:SetData(list)
end
function FishGroundMenuPanel:FlushFishingList()
    UH.SetText(self.Times, self.fight_data.gather_energy.val)
    local list = self.data:FishList()
	self.ItemList:SetData(list)
end
function FishGroundMenuPanel:OnClickGaiLv()
    local info = self.data:MapInfo()
    if info then
        PublicPopupCtrl.Instance:HelpTip(info.fish_des)
    end
end
function FishGroundMenuPanel:OnClickBack()
    local mode = SceneData.Instance:GetMode()
    PublicPopupCtrl.Instance:AlertTip(Language.Chat.BackTips .. FightCtrl.GetWarnTips(mode),function()
		if SceneData.Instance:GetMode() == SceneModeType.Material then
			MaterialCtrl.Instance:OnBackChallengeView()
		else
            MainPlayer:StopMove()
            ViewMgr:CloseView(FishView)
            FishData.Instance:ClearSetting()
			SceneCtrl.Instance:RequestLeaveFb()
		end
	end)
end