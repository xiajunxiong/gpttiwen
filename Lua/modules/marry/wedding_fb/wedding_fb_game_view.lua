
WeddingFBGameView = WeddingFBGameView or DeclareView("WeddingFBGameView", MarryConfig.ResPath .. "wedding_fb_game")
VIEW_DECLARE_LEVEL(WeddingFBGameView, ViewLevel.L1)

function WeddingFBGameView:LoadCallback()
end

function WeddingFBGameView:OnClickClose()
    ViewMgr:CloseView(WeddingFBGameView)
end

WeddingFBGameViewF = WeddingFBGameViewF or DeclareMono("WeddingFBGameViewF", UIWFlushPanel)

function WeddingFBGameViewF:WeddingFBGameViewF()
    self.data = MarryCtrl.Instance:Data()
    self.language = Language.Marry.WeddingFBGame

    self.data_cares = {
		{data = self.data.wedding_game_sm, func = self.FlushShow, init_call = false},
    }

    self.data.wedding_game_sm:Set{}
end

function WeddingFBGameViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushTypes()
end

function WeddingFBGameViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    TimeHelper:CancelTimer(self.timer_ct)
end

function WeddingFBGameViewF:FlushTypes()
    local show_list = self.data:GetWeddingFBGameList()
    for i = 1, self.TypeItems:Length() do
        self.TypeItems[i]:SetData(show_list[i])
    end

    TimeHelper:CancelTimer(self.timer_ct)

    local info = self.data.wedding_scene_info
    if info.init then return end

    local sel_index = info.link_seq > MarryConfig.FBParseType.sub_act and info.link_seq - 3 or 1

    self.TypeItems[sel_index]:SetSelected(true)
    self.TypeItems[sel_index]:OnSelect(true)

    local end_time = info.fb_begin_time + self.data:GetWeddingFBGamePrepareTime()
    if end_time - TimeHelper.GetServerTime() > 0 then
        self.TimeShow:SetObjActive(true)
        self.timer_ct = TimeHelper:AddCountDownCT(function ()
            local last_time = end_time - TimeHelper.GetServerTime()
            local ft = TimeHelper.FormatDHMS(last_time)
            self.TimeShow.text = string.format(self.language.TimeShow, ft.min, ft.s)
        end, function ()
            self.TimeShow:SetObjActive(false)
        end, end_time)
    else
        self.TimeShow:SetObjActive(false)
    end
end

function WeddingFBGameViewF:FlushShow()
    local info = self.data.wedding_game_sm
    local desc_co = Config.language_cfg_auto.textdesc[info.language_id]
    if info.activity_id > 1 then
        self.ImageSetter:SetByPath(string.format(MarryConfig.GameImgPngPath, info.activity_id))
        self.ShowDesc2.text = desc_co and desc_co.desc or ""
    else
        self.ShowDesc1.text = desc_co and desc_co.desc or ""
    end
end

WeddingFBGameViewTypeItem = WeddingFBGameViewTypeItem or DeclareMono("WeddingFBGameViewTypeItem", UIWidgetBaseItem)
function WeddingFBGameViewTypeItem:WeddingFBGameViewTypeItem()
    self.marry_data = MarryCtrl.Instance:Data()
end

function WeddingFBGameViewTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.TypeName.text = data.activity_name
end

function WeddingFBGameViewTypeItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function WeddingFBGameViewTypeItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.marry_data.wedding_game_sm:Set(self.data)
    end
end