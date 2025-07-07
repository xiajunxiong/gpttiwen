
MarryPartyGameView = MarryPartyGameView or DeclareView("MarryPartyGameView", MarryPartyConfig.ResPath .. "marry_party_game")
VIEW_DECLARE_LEVEL(MarryPartyGameView, ViewLevel.L1)

function MarryPartyGameView:LoadCallback()
end

function MarryPartyGameView:OnClickClose()
    ViewMgr:CloseView(MarryPartyGameView)
end

MarryPartyGameViewF = MarryPartyGameViewF or DeclareMono("MarryPartyGameViewF", UIWFlushPanel)

function MarryPartyGameViewF:MarryPartyGameViewF()
    self.data = MarryPartyCtrl.Instance:Data()
    self.language = Language.MarryParty.Game

    self.data_cares = {
		{data = self.data.marry_party_game_sm, func = self.FlushShow, init_call = false},
    }

    self.data.marry_party_game_sm:Set{}
end

function MarryPartyGameViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushTypes()
end

function MarryPartyGameViewF:FlushTypes()
    local show_list = self.data:GetMarryPartyGameList()
    for i = 1, self.TypeItems:Length() do
        self.TypeItems[i]:SetData(show_list[i])
    end

    local info = self.data.marry_party_scene_info
    if info.init then return end

    local item_count = self.TypeItems:Length()
    local sel_index = info.link_seq > item_count and item_count or info.link_seq

    self.TypeItems[sel_index]:SetSelected(true)
    self.TypeItems[sel_index]:OnSelect(true)
end

function MarryPartyGameViewF:FlushShow()
    local info = self.data.marry_party_game_sm
    local desc_co = Config.language_cfg_auto.textdesc[info.language_id]

    self.DescShow.text = desc_co and desc_co.desc or ""
    self.PageDownObj:SetActive(#info.reward_items > 0)
    self.RewardList:SetData(info.reward_items)
end

MarryPartyGameViewTypeItem = MarryPartyGameViewTypeItem or DeclareMono("MarryPartyGameViewTypeItem", UIWidgetBaseItem)
function MarryPartyGameViewTypeItem:MarryPartyGameViewTypeItem()
    self.marry_data = MarryPartyCtrl.Instance:Data()
end

function MarryPartyGameViewTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.TypeName.text = data.activity_name
end

function MarryPartyGameViewTypeItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function MarryPartyGameViewTypeItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.marry_data.marry_party_game_sm:Set(self.data)
    end
end