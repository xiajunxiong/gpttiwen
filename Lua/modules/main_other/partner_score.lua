--属性变化
PartnerScore = PartnerScore or DeclareView("PartnerScore", "main_other/partner_score", 0, ViewLayer.Top - 2)
VIEW_DECLARE_LEVEL(PartnerScore, ViewLevel.Lt)
VIEW_DECLARE_MASK(PartnerScore, ViewMask.None)

PartnerScorePanel = PartnerScorePanel or DeclareMono("PartnerScorePanel", UIWFlushPanel)
function PartnerScorePanel:PartnerScorePanel()
end
function PartnerScorePanel:onFlush()
    self.OpenAnim:Play()
    self.CloseAnim:Stop()
    --动画帧必须比停止帧长
    --24帧1秒
    local anim_frame = 18
    local end_frame = 15
    local close_anim_time = 3
    local close_view_time = 3.5
    --获取旧值
    local score, old_score = PartnerData.Instance:GetScoreChange()
    local offset = score - old_score
    if offset == 0 or (offset > 0 and offset < 1) or (offset < 0 and offset > -1) then
        ViewMgr:CloseView(PartnerScore)
    end
    UH.SpriteName(self.ArrowSp, offset > 0 and "FengYinJianTou" or "JianTouXiangXia")
    UH.Color(self.ScoreOffset, offset > 0 and COLORS.Green7 or COLORS.Red10)
    UH.SetText(self.ScoreText, old_score)
    UH.SetText(self.ScoreOffset, offset)
    TimeHelper:CancelTimer(self.handle_run)
    self.handle_run =
        TimeHelper:AddRunFrameTimer(
        function()
            old_score = old_score + (offset / (anim_frame - (anim_frame - end_frame)))
            UH.SetText(self.ScoreText, math.floor(old_score))
        end,
        1,
        anim_frame
    )
    TimeHelper:CancelTimer(self.handle_delay)
    self.handle_delay =
        TimeHelper:AddDelayFrameTimer(
        function()
            TimeHelper:CancelTimer(self.handle_run)
            UH.SetText(self.ScoreText, score)
        end,
        end_frame
    )
    TimeHelper:CancelTimer(self.handle_delay3)
    self.handle_delay3 =
        TimeHelper:AddDelayTimer(
        function()
            self.CloseAnim:Play()
        end,
        close_anim_time
    )
    TimeHelper:CancelTimer(self.handle_delay2)
    self.handle_delay2 =
        TimeHelper:AddDelayTimer(
        function()
            ViewMgr:CloseView(PartnerScore)
        end,
        close_view_time
    )
end
function PartnerScorePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_run)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay2)
    TimeHelper:CancelTimer(self.handle_delay3)
end
