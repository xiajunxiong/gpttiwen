QuietlyDownloadView = QuietlyDownloadView or DeclareView("QuietlyDownloadView", "quietly_download")
VIEW_DECLARE_LEVEL(QuietlyDownloadView,ViewLevel.L1)
VIEW_DECLARE_MASK(QuietlyDownloadView,ViewMask.BlockClose)

--ProgImg=======Image(Filled)
--ProgTxt=======Text
--SizeTxt=======Text
--CurNetTxt=====Text
--PauseToggle===Toggle
--SizeObj=======GameObject
function QuietlyDownloadView:QuietlyDownloadView()
    self.update_fresh_timer = nil
end

function QuietlyDownloadView:LoadCallback()
    self.update_fresh_timer = TimeHelper:AddRunTimer(BindTool.Bind(self.UpdateToRefreshProg,self,false), 0.1, -1, false)
    self.CurNetTxt.text = string.format(Language.QuietlyDownload.CurNetState,
        BundleProviderCtrl.Instance:InternetReachability() == UnityNetReachablility.LocalAreaNetwork and "WIFI" or "4G")
    self.PauseToggle.isOn =  not BundleProviderCtrl.Instance:IsPaused() 
    self:UpdateToRefreshProg(true)
end

function QuietlyDownloadView:CloseCallback()
    if self.update_fresh_timer ~= nil then
        TimeHelper:CancelTimer(self.update_fresh_timer)
        self.update_fresh_timer = nil
    end
end

function QuietlyDownloadView:UpdateToRefreshProg(ignoreToggle)
    if ignoreToggle ~= true and not self.PauseToggle.isOn then   --暂停之后刷新界面
        return
    end
    local prog = BundleProviderCtrl.Instance:QuitelyDownloadingProg()
    if prog == nil then
        self.SizeObj:SetActive(false)
        self.ProgTxt.text = "100%"
        self.ProgImg.fillAmount = 1
        self.PauseToggle.interactable = false
        return 
    end
    self.ProgImg.fillAmount = prog.total_percent
    self.ProgTxt.text = string.format("%s%%", math.floor(prog.total_percent * 100))
    self.SizeTxt.text = string.format("%.2fM/%.2fM",prog.downloaded_size/1024,prog.total_size/1024)
end

function QuietlyDownloadView:OnOkClick()
    ViewMgr:CloseView(QuietlyDownloadView)
end

function QuietlyDownloadView:OnPauseToggle()
    if  BundleProviderCtrl.Instance:QuitelyDownloadingProg() == nil then        --下载完了，暂停按钮不需要点击了
        return 
    end
    if self.PauseToggle.isOn then
        BundleProviderCtrl.Instance:resumeQuietlyCache()
    else
        BundleProviderCtrl.Instance:pauseQuietlyCache()
        self:UpdateToRefreshProg(true)
    end
end



QuietlyDownloadPanel = QuietlyDownloadPanel or DeclareMono("QuietlyDownloadPanel", UIWFlushPanel)
--RewardObj=====GameObject
--RewardGettedObj==GameObject
--RewardList=======UIWidgetList[UIWItemCell]
--GetBtnInteractor===InteractorHandler
function QuietlyDownloadPanel:QuietlyDownloadPanel()

	self.data_cares = {
		{data = BundleProviderCtrl.Instance:ViewData(), func = self.FlushGettable, keys = {"reward_getable_flashed"}},
		{data = BundleProviderCtrl.Instance:ViewData(), func = self.FlushGetState, keys = {"reward_getted_flag"}},
	}
end

function QuietlyDownloadPanel:FlushGettable()
    local gettable = BundleProviderCtrl.Instance:RewardGettable()
    self.GetBtnInteractor.Interactable = gettable
end

function QuietlyDownloadPanel:FlushGetState()
    local reward_has_getted = BundleProviderCtrl.Instance:HasRewardGetted()
    self.RewardObj:SetActive(not reward_has_getted)
    self.RewardGettedObj:SetActive(reward_has_getted)
    if not reward_has_getted then
        local reward_items = Config.sundries_auto.other[1].download_reward
        local items = {}
        for _,v in pairs(reward_items) do
            table.insert(items,Item.Create(v))
        end
        self.RewardList:SetData(items)
    end
end

function QuietlyDownloadPanel:OnGetRewardClick()
    BundleProviderCtrl.Instance:ReqFetchResourceDownloadReward()
end