TipsPopupView = TipsPopupView or DeclareView("TipsPopupView", "public_popup/tips_popup")
VIEW_DECLARE_CACHE(TipsPopupView)

function TipsPopupView:TipsPopupView()
    self.data = PublicPopupCtrl.Instance.tips_popup_data
end

function TipsPopupView:LoadCallback(data)
    if data and data.pos then
        UH.AnchoredPosition(self.pos, data.pos)
    else
        UH.AnchoredPosition(self.pos, Vector2.zero)
    end
    local content = self.data:GetTipsValue()
    self.Tips.text = content
    local is_slide = string.len(content) >= GameEnum.TIPS_POPUP_MAX_SHOW_NUM
    if data and data.slide_size ~= nil then --Vector2
        is_slide = true
        UH.SizeDelta2(self.SlideRect, data.slide_size)
        self.SlideTips.text = content
    elseif is_slide then
        UH.SizeDelta2(self.SlideRect, Vector2.New(684, 600))
        self.SlideTips.text = content
    end
    self.Slide:SetActive(is_slide)
    self.All:SetActive(not is_slide)
    local try_split = Split(content, "|")

    if #try_split > 1 then 
        self.Slide:SetActive(false)
        self.SplitObj:SetActive(true)
        self.Slide:SetActive(false)
        self.All:SetActive(false)
        self.SplitList:SetData(try_split)
    else 
        self.SplitObj:SetActive(false)
    end
    if data.tip_bg_name then
        UH.SpriteName(self.TipBg,data.tip_bg_name)
    else
        UH.SpriteName(self.TipBg,"XinXiKuang_DiBan")
    end 
    if data.tip_color then
        UH.Color(self.Tips, data.tip_color)
    else
        UH.Color(self.Tips, COLORS.Yellow13)
    end
end

function TipsPopupView:OnClickBlank()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(TipsPopupView)
end

TipsPopupSplitItem = TipsPopupSplitItem or DeclareMono("TipsPopupSplitItem", UIWidgetBaseItem)
--CellTips=====Text
function TipsPopupSplitItem:SetData(data)
    self.data = data

    -- LogError(self.data)

    local data_string = Split(self.data, "@")

    self.TextParent.text = data_string[1]

    if data_string[2] then
        self.TextChild.text = data_string[2]
    end 

    if data_string[3] then
        self.TextEnd.text = data_string[3]
    end 
end