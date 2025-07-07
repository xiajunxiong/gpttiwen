
StoryShowBossView = StoryShowBossView or DeclareView("StoryShowBossView", StoryConfig.UIResPath .. "story_showboss")
VIEW_DECLARE_MASK(StoryShowBossView, ViewMask.None)

StoryShowBossViewF = StoryShowBossViewF or DeclareMono("StoryShowBossViewF", UIWFlushPanel)

function StoryShowBossViewF:StoryShowBossViewF()
    self.data = StoryCtrl.Instance:Data()

    self.data_cares = {
		{data = self.data.story_event, func = self.FlushShow, init_call = false},
	}
end

function StoryShowBossViewF:FlushShow()
    local id = self.data.story_event.id
    if nil == id then return end
    self["Story"..id]:SetActive(true)
    self.ShowBossAnim:SetTrigger(APH("Show"))
end


