LinkageGiftView = LinkageGiftView or DeclareView("LinkageGiftView", "serveractivity/linkage_gift",Mod.RandActivity.LinkageGift)
VIEW_DECLARE_MASK(LinkageGiftView,ViewMask.BgBlockClose)
VIEW_DECLARE_LEVEL(LinkageGiftView, ViewLevel.L1)
--GetRewardInter====InteractorHandler
--GettableTxtObj====GameObject
--GettedTxtObj======GameObject
--TipsTxt===========Text
--TipsItem==========UIWSubstitute[UIWItemCell]
--TipsObj===========GameObject
--ReceiveTime=======UITimeMeter
--EffObj============GameObject
function LinkageGiftView:LinkageGiftView()
    self.tips_inited = false
    self.data = ServerActivityData.Instance.linkage_gift_data
end

function LinkageGiftView:LoadCallback()
    if self.ui_obj == nil then
        self.ui_obj = UIChDrawer.New()
        self.ui_obj:SetMainSolo("actor/npc/224.prefab")
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end
    UH.SetText(self.NameTxt, Config.partner_cfg_auto.partner_info[self.data.partner_id].name)
    self:flushView()
end

function LinkageGiftView:flushView()
    local getable = self.data.fetch_flag == 1
    local getted = self.data.fetch_flag == 2
    self.GetRewardInter.Interactable = getable
    self.GettableTxtObj:SetActive(not getted)
    self.GettedTxtObj:SetActive(getted)
    if self.data.fetch_flag == 0 then
        self.ReceiveTime:SetObjActive(true)
        self.ReceiveTime:StampTime(self.data.can_receive_tamestamp,TimeType.Type_Time_2,Language.LinkageGift.ReceiveTimeHint)
        self.ReceiveTime:SetCallBack(BindTool.Bind(self.onTimerComplete,self))
    else
        self.ReceiveTime:SetObjActive(false)
    end
    self.EffObj:SetActive(getable)
    if self.ui_obj then
        self.ui_obj:PlayAnim(ChAnimType.Show)
    end
end

function LinkageGiftView:onTimerComplete()
    self.data.fetch_flag = 1
    self:flushView()
end

function LinkageGiftView:OnClickTips()
    if self.tips_inited == false then
        local reward_cfg = ServerActivityData.Instance:GetLinkageGiftRewardCfg(self.data.gift_seq)
        local reward_item_cfg = reward_cfg.reward_item
        local reward_item = Item.Init(reward_item_cfg.item_id,reward_item_cfg.num,reward_item_cfg.is_bind)
        self.TipsItem:SetData(reward_item)
        local txt = string.format(Config.language_cfg_auto.textdesc[58].desc,
            COLORSTR.Red12,reward_item:QuaName())
        self.TipsTxt.text = txt
        self.tips_inited = true
    end
    self.TipsObj:SetActive(true)
end

function LinkageGiftView:OnClickGet()
    if self.data.fetch_flag == 0 then
        PublicPopupCtrl.Instance:Center(Language.LinkageGift.CantReceiveHint)
        return
    end
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.RAND_LINKAGE_GIFT
    param_t.opera_type = 1
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
    ViewMgr:CloseView(LinkageGiftView)
end

function LinkageGiftView:OnTipsMaskClick()
    self.TipsObj:SetActive(false)
end

function LinkageGiftView:CloseCallback()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
end

function LinkageGiftView:OnInfoClick()
    local partner_id = self.data.partner_id
    PartnerCtrl.Instance:Data().new_partner = partner_id
    local config = Config.partner_cfg_auto.partner_info[partner_id]
    ViewMgr:OpenView(PartnerViewLock,{partner_name = config.name})
end