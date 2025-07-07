
StoryShowEffectView = StoryShowEffectView or DeclareView("StoryShowEffectView", StoryConfig.UIResPath .. "story_showeffect")
VIEW_DECLARE_MASK(StoryShowEffectView, ViewMask.None)
VIEW_DECLARE_LEVEL(StoryShowEffectView, ViewLevel.Lt)

StoryShowEffectViewF = StoryShowEffectViewF or DeclareMono("StoryShowEffectViewF", UIWFlushPanel)

function StoryShowEffectViewF:StoryShowEffectViewF()
    self.data = StoryCtrl.Instance:Data()

    self.data_cares = {
		{data = self.data.story_event, func = self.FlushShow, init_call = false},
	}
end

function StoryShowEffectViewF:FlushShow()
    local id = self.data.story_event.id
    if nil == id then return end
    self["Story"..id]:SetActive(true)
end


