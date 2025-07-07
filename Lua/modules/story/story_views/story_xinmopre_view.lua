
StoryXinmopreView = StoryXinmopreView or DeclareView("StoryXinmopreView", StoryConfig.UIResPath .. "story_xinmopre")
VIEW_DECLARE_MASK(StoryXinmopreView, ViewMask.None)

--TalkTxt========Text

StoryXinmopreView.Data = SmartData.New({
    str = "",
    duration = 0,
    flush = false,
})

function StoryXinmopreView:StoryXinmopreView()
    self.smdata_handle = nil
    self.timer = nil
end

function StoryXinmopreView:LoadCallback(param)
    self.smdata_handle = StoryXinmopreView.Data:Care(BindTool.Bind(self.FlushText,self),"flush")
    self.SkipBtnObj:SetActive(IS_STANDALONE())
    if IS_STANDALONE() then
        UH.AnchoredPosition(self.SkipBtnRect, Vector2.New(0,0))
    end
    if param and param.story_info then
        self.story_info = param.story_info
    end
end

function StoryXinmopreView:OnClickSkip()
    if self.story_info and self.story_info.Stop then
        self.story_info:Stop()
    end
end

function StoryXinmopreView:FlushText()
    UH.SetText(self.TalkTxt,StoryXinmopreView.Data.str)
    self:CancelTimer()

    self.timer = TimeHelper:AddDelayTimer(function() 
            UH.SetText(self.TalkTxt,"")
            self.timer = nil
        end,
        StoryXinmopreView.Data.duration)

end

function StoryXinmopreView:CloseCallback()
    StoryXinmopreView.Data:Uncare(self.smdata_handle)
    self.smdata_handle = nil
    self:CancelTimer()
end

function StoryXinmopreView:CancelTimer()
    if self.timer ~= nil then
        TimeHelper:CancelTimer(self.timer)
        self.timer = nil
    end
end

function StoryXinmopreView.ShowText(txt,duration)
    StoryXinmopreView.Data.str = txt
    StoryXinmopreView.Data.duration = duration
    StoryXinmopreView.Data.flush = not StoryXinmopreView.Data.flush
end


