
MarryPartyBlessView = MarryPartyBlessView or DeclareView("MarryPartyBlessView", MarryPartyConfig.ResPath .. "marry_party_bless")
VIEW_DECLARE_LEVEL(MarryPartyBlessView, ViewLevel.L1)

function MarryPartyBlessView:MarryPartyBlessView()
    self.language = Language.MarryParty.Bless
	self.world_num_limit = 100

	self.InputField.onValueChanged:AddListener(function ()
		local word_num = DataHelper.GetStringWordNum(self.InputField.text)
		if word_num > self.world_num_limit and word_num > self.word_num_per then
			PublicPopupCtrl.Instance:Center(Language.Chat.WordNumTips)
		end
		self.word_num_per = word_num
		self.NumShow.text = string.format("%s/%s", word_num, self.world_num_limit)
	end)
end

function MarryPartyBlessView:OnFlush()
	self.NumShow.text = "0/" .. self.world_num_limit
end

function MarryPartyBlessView:OnClickClose()
	ViewMgr:CloseView(MarryPartyBlessView)
end

function MarryPartyBlessView:OnClickSend()
    local word_num = self.word_num_per or 0
    if 0 == word_num then
        PublicPopupCtrl.Instance:Center(self.language.EmptyTips)
		return
    elseif word_num > self.world_num_limit then
		PublicPopupCtrl.Instance:Center(Language.Chat.WordNumTips)
		return
	end

	ViewMgr:CloseView(MarryPartyBlessView)
    ViewMgr:OpenView(MarryPartyBlessLightView)
end

MarryPartyBlessLightView = MarryPartyBlessLightView or DeclareView("MarryPartyBlessLightView", MarryPartyConfig.ResPath .. "marry_party_bless_light")
VIEW_DECLARE_LEVEL(MarryPartyBlessLightView, ViewLevel.L1)

function MarryPartyBlessLightView:MarryPartyBlessLightView()
    self.data = MarryPartyCtrl.Instance:Data()
end

function MarryPartyBlessLightView:LoadCallback()
    self:FlushModelShow()
end

function MarryPartyBlessLightView:FlushModelShow()
    local res_path = DrawerHelper.GetNpcPath(self.data:ConfigOtherFlowerAmpResource())
    local uo = self.ModelShow:UiObj()
    if uo == nil then
        uo = UIObjDrawer.CreateByPath(res_path)
        self.ModelShow:SetShowData(
            {
                ui_obj = uo,
                view = MarryPartyBlessLightView,
                -- load_callback = BindTool.Bind(self.ModelCallback, self),
            }
        )
    else
        uo:SetData(res_path) 
    end
end

function MarryPartyBlessLightView:OnClickClose()
    ViewMgr:CloseView(MarryPartyBlessLightView)
end

function MarryPartyBlessLightView:OnClickBless()
    MarryPartyCtrl.Instance:SendQingYuanShengHuiBless()
    ViewMgr:CloseView(MarryPartyBlessLightView)
end