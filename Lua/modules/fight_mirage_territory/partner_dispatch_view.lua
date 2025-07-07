
PartnerDispatchView = PartnerDispatchView or DeclareView("PartnerDispatchView","fight_mirage_territory/partner_dispatch_view")

function PartnerDispatchView:OnClickClose()
	ViewMgr:CloseView(PartnerDispatchView)
end

--------------伙伴派遣--------------
PartnerDispatchPanel = PartnerDispatchPanel or DeclareMono("PartnerDispatchPanel", UIWFlushPanel)
function PartnerDispatchPanel:PartnerDispatchPanel()
	self.data = FightMirageTerritoryData.Instance
    self.data_cares = {
		{data = self.data.dispatch_smart, func = self.FlushShow, keys = {"certain"},init_call = true},
    }
	self.data:DispatchReset()
	local data = self.data:GetClearReward()[self.data.cur_layer_data.pass_level]
	UH.SetText(self.TabTxt,string.format(Language.MirageTerritory.DispatchTip,DataHelper.GetDaXie(data.num),data.name))
end
function PartnerDispatchPanel:FlushShow()
	local list = {}
	for i = 1,self.PartnerList:Length() do
		self.PartnerList[i]:SetData(self.data.dispatch_partner_list[i])
		if self.data.dispatch_partner_list[i] then
			local info = self.data:GetPartnerArmsById(self.data.dispatch_partner_list[i].id)
			info.num = self.data:GetPartnerArmsNum() or 0
			table.insert(list,info)
		end
	end
	self.RewardList:SetData(list)
end

function PartnerDispatchPanel:OnClickDispatch()
	local num = 0
    for i = 1,2 do
        if self.data.dispatch_partner_list[i] then
            num = num + 1
        end 
    end
    if num < 2 then
        if num == 0 then
            PublicPopupCtrl.Instance:Center(Language.MirageTerritory.NotDispatch)
        else
            --提示确认框
			PublicPopupCtrl.Instance:DialogTips{
				content = Language.MirageTerritory.IsDispatchTips,
				confirm = {
					func = function()
						FightMirageTerritoryData.Instance:SetPlaySweep()
						ViewMgr:CloseView(DialogTipsView)						
					end
				}
			}
        end
        return
    end
	self.data:SetPlaySweep()
end

--------------伙伴选择--------------
DispatchSelectPanel = DispatchSelectPanel or DeclareMono("DispatchSelectPanel", UIWFlushPanel)
function DispatchSelectPanel:DispatchSelectPanel()
	self.data = FightMirageTerritoryData.Instance
	self.data_cares = {
		{data = self.data.dispatch_smart, func = self.FlushShow, keys = {"certain"},init_call = true},
    }	
end
function DispatchSelectPanel:FlushShow()
	self.PartnerList:SetData(self.data:GetPartnerList())
end

function DispatchSelectPanel:OnClickCloseSelect()
	FightMirageTerritoryData.Instance:DispatchCancelSelect()
	self:SetObjActive(false)
end
function DispatchSelectPanel:OnClickCancel()
	self:OnClickCloseSelect()
end

function DispatchSelectPanel:OnClickCertain()
	self.data:AddDispatchList()
	self:OnClickCloseSelect()
end

-------------ITEM--------------
DispatchPartnerItem = DeclareMono("DispatchPartnerItem", UIWidgetBaseItem)

function DispatchPartnerItem:DispatchPartnerItem()
	self.Data = FightMirageTerritoryData.Instance
end

function DispatchPartnerItem:OnDestroy()
    self:uncareData()
end

function DispatchPartnerItem:SetData(data)
	if self.RoleObj then
		self.RoleObj:SetActive(data ~= nil)
	end
	if data == nil then
		return 
	end
	UIWidgetBaseItem.SetData(self, data)
	self:uncareData()
	self.care_handles = {} 

	if self.RoleObj == nil then
		self:careData(self.Data.dispatch_smart, BindTool.Bind(self.FlushSelect, self), "partner")
		self.ItemObj:SetActive(self.Data:GetDispatchShow(data.id))
	end

	local info = data.info
    local vo = data.vo
	local prof_sp = RoleData.GetProfSp(info.job)
	local lock_state = PartnerConfig.LockState
	
	self.Name.text = string.format("<color=#%s>%s</color>", vo.is_unlocked == lock_state.unlock and COLORSTR.Yellow13 or COLORSTR.Gray4, info.name)
    self.Prof.SpriteName = prof_sp
	UH.SetIcon(self.IconSp, PartnerData.IconId(data), ICON_TYPE.ITEM)
	local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(data.id)
	PartnerInfo.SetQuality(self.Quality, info_quality.quality)
	if self.ItemInter then
		self.ItemInter.Interactable = vo.is_unlocked == lock_state.unlock
	end
end

function DispatchPartnerItem:FlushSelect()
	self.BgSp.SpriteName = self.Data.dispatch_smart.partner == self.data.id and FormationConfig.PartnerBgSp.select or FormationConfig.PartnerBgSp.normal
end

function DispatchPartnerItem:OnClickItem(value)
	if self.SelPanel then
		self.Data.dispatch_smart.pos = value
		self.SelPanel:SetActive(true)
		return
	end
	local is_lock = self.data.vo.is_unlocked == PartnerConfig.LockState.lock
	if is_lock then
		PublicPopupCtrl.Instance:Center(Language.MirageTerritory.PartnerNoLockTips)
		return
	end
	self.Data:SetDispatchSelectPartner(self.data)
end

-------------奖励ITEM--------------
DispatchPartnerRewardItem = DeclareMono("DispatchPartnerRewardItem", UIWidgetBaseItem)
function DispatchPartnerRewardItem:SetData(data)
	local item = Item.New({item_id = data.arms_id,num = data.num})
	self.Cell:SetData(item)
	UH.SetText(self.Name,item:Name())
end