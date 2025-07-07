
MarryPartyNoticeView = MarryPartyNoticeView or DeclareView("MarryPartyNoticeView", MarryPartyConfig.ResPath .. "marry_party_notice", Mod.MarryParty.Notice)
VIEW_DECLARE_LEVEL(MarryPartyNoticeView, ViewLevel.L0)

function MarryPartyNoticeView:LoadCallback()
end

MarryPartyNoticeViewF = MarryPartyNoticeViewF or DeclareMono("MarryPartyNoticeViewF", UIWFlushPanel)

function MarryPartyNoticeViewF:MarryPartyNoticeViewF()
    self.data = MarryPartyCtrl.Instance:Data()
    self.language = Language.MarryParty

    -- self.data_cares = {
	-- 	{data = self.data.marry_order_info, func = self.FlushShow, init_call = false},
    -- }
end

function MarryPartyNoticeViewF:OnClickClose()
    ViewMgr:CloseView(MarryPartyNoticeView)
end

function MarryPartyNoticeViewF:OnClickOrder()
end

function MarryPartyNoticeViewF:OnClickTips()
end
