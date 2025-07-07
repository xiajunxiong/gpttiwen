SpecialPower360View = SpecialPower360View or DeclareView("SpecialPower360View", "special_power_360/special_power_360_view")
VIEW_DECLARE_MASK(SpecialPower360View,ViewMask.BgBlock)
function SpecialPower360View:SpecialPower360View()
end
function SpecialPower360View:OnClickClose()
    ViewMgr:CloseView(SpecialPower360View)
end
function SpecialPower360View:CloseCallback()
    SpecialPowerData.Instance:SetIndex(1)
end
-----------------------------------
SpecialPower360Panel = SpecialPower360Panel or DeclareMono("SpecialPower360Panel", UIWFlushPanel)
function SpecialPower360Panel:SpecialPower360Panel()
    self.data = SpecialPowerData.Instance
    self.data_cares = {
        {data = self.data.sm_data, func = self.FlushView, keys = {"view_index"}}
    }
end

function SpecialPower360Panel:OnClickToggle(index)
    self.data:SetIndex(index)
end

function SpecialPower360Panel:FlushView()
    self.ShowList:SetData(self.data:Get360SpecialReward())
    self.BtnInteractor.Interactable = self.data:CanGet360Reward()
end

function SpecialPower360Panel:OnClickGet()
    if self.data:CanGet360Reward() then
        SpecialPowerCtrl.Instance:Send360SpecialReq(1,self.data.sm_data.view_index == 1 and 1 or 0)
    else
        PublicPopupCtrl.Instance:Center(Language.SpecialPower["HasGet"..self.data.sm_data.view_index])
    end
end
-----------------------------------
DoublePlat360View = DoublePlat360View or DeclareView("DoublePlat360View", "special_power_360/double_plat_360_view")
VIEW_DECLARE_MASK(DoublePlat360View, ViewMask.BgBlock)
function DoublePlat360View:DoublePlat360View()
    self.data = SpecialPowerData.Instance
end
function DoublePlat360View:OnClickClose()
    ViewMgr:CloseView(DoublePlat360View)
end

function DoublePlat360View:LoadCallback()
    local img_name = SpecialPowerData.ErWeiMaName[GLOBAL_CONFIG:PkgInfo().agent_id]
    UH.SpriteName(self.ErWeiMaImg, img_name)
    local str_index = SpecialPowerData.TipsIndex[GLOBAL_CONFIG:PkgInfo().agent_id] or 1
    UH.SetText(self.TipsText, Language.PCNotice["DoublePlatStr"..str_index])
end

function DoublePlat360View:OnClickGet()
    local state, tips = self.data:DoublePlatRewardState360()
    if state == 0 then
        SpecialPowerCtrl.Instance:Send360SpecialReq(1,2)
    else
        PublicPopupCtrl.Instance:Center(tips)
    end
end

function DoublePlat360View:OnCLickDownload()
    if GLOBAL_CONFIG:PkgInfo().agent_id == "pa1" then
        UnityApp.OpenURL("http://yxdt.game.keniub.com/weiduan/wlshzjpc-360.exe")
    end
end
--------------------------------------
DoublePlat360Panel = DoublePlat360Panel or DeclareMono("DoublePlat360Panel", UIWFlushPanel)
function DoublePlat360Panel:DoublePlat360Panel()
    self.data = SpecialPowerData.Instance
    self.data_cares = {
        {data = self.data.sm_data, func = self.FlushRewardState, keys = {"view_index"}}
    }
end

function DoublePlat360Panel:FlushRewardState()
    local state = self.data:DoublePlatRewardState360()
    self.ShowList:SetData(self.data:Get360DoublePlatReward())
    if GLOBAL_CONFIG:PkgInfo().agent_id == "pa1" then
        self.GetBtnObj:SetActive(state ~= 2)
        if state ~= 2 then
            self.BtnInteractor.Interactable = state ~= 1
        end
        self.DownloadBtnObj:SetActive(state == 2)
    elseif GLOBAL_CONFIG:PkgInfo().agent_id == "ps1" then
        self.DownloadBtnObj:SetActive(false)
        self.GetBtnObj:SetActive(true)
        self.BtnInteractor.Interactable = state == 0
        
    end
end