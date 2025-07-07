MallView = MallView or DeclareView("MallView", "mall/MallView",Mod.Mall.Trade)

VIEW_DECLARE_LEVEL(MallView,ViewLevel.L0)

function MallView:MallView()
    MarketCtrl.Instance:SendMarketReq()
    TradeCtrl.Instance:SendTradeMarketReq()
end

function MallView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(), Language.Mall.Title)
    self.Board:SetTabbarVisible(true)
    self.Board.Tabbar:SetData(
        {
            {name = Language.Mall.Tab2, tab_objs = {self.Tab2},mod = TabMod(Mod.Mall.Shop), open = Mod.Mall.Shop},
            {name = Language.Mall.Tab3, tab_objs = {self.Tab3},mod = TabMod(Mod.Mall.Market), open = Mod.Mall.Market},
            {name = Language.Mall.Tab1, tab_objs = {self.Tab1},mod = TabMod(Mod.Mall.Trade), open = Mod.Mall.Trade},
        }
    )
    self.Board.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Mall, self.Board.Tabbar, param_t.open_param or 3))
    self.Currency:DefaultSet()
	
end

function MallView:CloseCallback()
    if not ViewMgr:IsOpen(FightAutoSealSetView) then
        TaskData.Instance:TaskChainSubContinue()
    end
    TaskData.shop_auto_close = nil
end

--===========================BoardMall===========================
BoardMall = BoardMall or DeclareMono("BoardMall")

function BoardMall:BoardMall()
	self.title_str = ""
	self.view_type = nil
end

function BoardMall:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end

function BoardMall:SetData(view_type,title_str)
	self.view_type = view_type
	self.title_str = title_str
	self.Title.text = self.title_str
	if IS_AUDIT_VERSION then
		if not GLOBAL_CONFIG:PkgInfo().is_youyan then
			ResMgr:LoadObject("foundation_res/ui/ui_picture/boardmallauditbg",function(tex)
                if self.BgAuditRawSet then
                    self.BgAuditRawSet.texture = tex
                end
            end,false)
			self.BgAudit:SetActive(true)
		else
			local img_path, stream_has = ChannelAgentConsts.GetUIStremingAsset("RechargeBigBg")
			if stream_has then
				self.BgAuditRawObj:SetActive(true)
				ChannelAgentConsts.SetRawImg(self.BgAuditRaw,img_path)
			end
		end
	end
end

function BoardMall:SetTabbarVisible(visible)
	if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
		self.TabbarObj:SetActive(false)
		self.TabbarAuditObj:SetActive(visible)
	else
		self.TabbarObj:SetActive(visible)
		self.TabbarAuditObj:SetActive(false)
	end
end

function BoardMall:OnRelease()
	if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
		self.TabbarAudit:SetObjActive(false)
	else
		self.Tabbar:SetObjActive(false)
	end
end