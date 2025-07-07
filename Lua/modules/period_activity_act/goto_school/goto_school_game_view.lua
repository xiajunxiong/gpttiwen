-- 我要上学
GotoSchoolGameView = GotoSchoolGameView or DeclareView("GotoSchoolGameView", "period_activity_act/goto_school/goto_school_game_view")
VIEW_DECLARE_MASK(GotoSchoolGameView,ViewMask.None)
function GotoSchoolGameView:LoadCallback()
    
end

GotoSchoolGamePanel = GotoSchoolGamePanel or DeclareMono("GotoSchoolGamePanel", UIWFlushPanel)
function GotoSchoolGamePanel:GotoSchoolGamePanel()
    self.data = GotoSchoolData.Instance
    self.data_cares = {
        {data = self.data.tip_data, func = self.FlushTip, init_call = false}
    }
end

function GotoSchoolGamePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushTip()
end

function GotoSchoolGamePanel:FlushTip()
    local tip = self.data.tip_data.tip
    if tip == GotoSchoolCtrl.Tip.Time then
        self:PlayTime()
    elseif tip == GotoSchoolCtrl.Tip.GetTask then
        self.TipImgAnim:Play("tip")
        UH.SpriteName(self.TipImg, "_LocHuoDeRenWu")
    elseif tip == GotoSchoolCtrl.Tip.TaskFinish then
        self.TipImgAnim:Play("tip")
        UH.SpriteName(self.TipImg, "_LocRenWuWanCheng")
    end
end

function GotoSchoolGamePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_t)
    self.data:SetTip(GotoSchoolCtrl.Tip.None)
end

function GotoSchoolGamePanel:PlayTime()
    local count_down_num = 3
    self.timer_t = TimeHelper:AddCountDownTTA(function ()
        self.CountDownObj:SetActive(true)
        if 0 == count_down_num then
            self.CountDownNum.text = ""
            self.CountDownAnim:SetTrigger("tri_go")
        else
            self.CountDownNum.text = count_down_num
            self.CountDownAnim:SetTrigger("tri_321")
        end
        count_down_num = count_down_num - 1
    end, function ()
        self.CountDownObj:SetActive(false)
    end, count_down_num + 2, 1, false)
end


-- 玩法说明
GotoSchoolDescView = GotoSchoolDescView or DeclareView("GotoSchoolDescView", "period_activity_act/goto_school/goto_school_desc_view")
function GotoSchoolDescView:LoadCallback()
    local desc_id_1 = Config.go_to_school_auto.other[1].desc1
    local desc_id_2 = Config.go_to_school_auto.other[1].desc2
    local tab = RichTextHelper.ColorStr("ttttt", COLORSTR.Transparent)
    UH.SetText(self.DescL, tab .. Config.language_cfg_auto.textdesc[desc_id_1].desc)
    UH.SetText(self.DescR, tab .. Config.language_cfg_auto.textdesc[desc_id_2].desc)
end

function GotoSchoolDescView:OnCloseClick()
    ViewMgr:CloseView(GotoSchoolDescView)
end