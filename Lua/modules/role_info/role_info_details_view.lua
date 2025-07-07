RoleInfoDetailsView = RoleInfoDetailsView or DeclareView("RoleInfoDetailsView", "role_info/role_info_details")
VIEW_DECLARE_MASK(RoleInfoDetailsView, ViewMask.BgBlock)
--PetTabInteractor=====InteractorHandler
--PetDisableHintBtn====GameObject
function RoleInfoDetailsView:RoleInfoDetailsView()
    self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
end 

function RoleInfoDetailsView:LoadCallback()
    local pet_info_list = RoleInfoCtrl.Instance:Data():GetRoleEquips().pet_list
    if #pet_info_list == 0 then
        self.PetTabInteractor.Interactable = false
        self.PetDisableHintBtn:SetActive(true)
    end
    local partner_info_list = RoleInfoCtrl.Instance:Data():GetRoleEquips().partner_list
    if #partner_info_list == 0 then
    	self.PartnerInteractor.Interactable = false
    	self.PartnerDisableObj:SetActive(true)
    end
    local bicycle_info_list = RoleInfoCtrl.Instance:Data():GetRoleEquips().bicycle_list
    if #bicycle_info_list == 0 then
    	self.BicycleInteractor.Interactable = false
    	self.BicycleDisableObj:SetActive(true)
    end
    local sheng_qi_info_list = RoleInfoCtrl.Instance:Data():GetRoleEquips().sheng_qi_list
    if #sheng_qi_info_list == 0 then
    	self.ShengQiInteractor.Interactable = false
    	self.ShengQiDisableObj:SetActive(true)
    end

    self.ShengQiInteractor:SetObjActive(ShengQiData.Instance:IsOpen())
end

function RoleInfoDetailsView:OnCloseClick()
    ViewMgr:CloseView(RoleInfoDetailsView)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

function RoleInfoDetailsView:OnClickPetDisableHint()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    PublicPopupCtrl.Instance:Center(Language.ItemInfo.RoleInfoDesc.NoPetHint)
end

function RoleInfoDetailsView:OnClickPartnerDisableHit()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    PublicPopupCtrl.Instance:Center(Language.ItemInfo.RoleInfoDesc.NoPartnerHint)
end

function RoleInfoDetailsView:OnClickBicycleDisableHit()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    PublicPopupCtrl.Instance:Center(Language.ItemInfo.RoleInfoDesc.NoBicycleHint)
end

function RoleInfoDetailsView:OnClickShengQiDisableHit()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    PublicPopupCtrl.Instance:Center(Language.ItemInfo.RoleInfoDesc.NoShengQiHint)
end

function RoleInfoDetailsView:OnClickTab()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end