
StoryMarryBlessView = StoryMarryBlessView or DeclareView("StoryMarryBlessView", StoryConfig.UIResPath .. "story_marry_bless")
VIEW_DECLARE_MASK(StoryMarryBlessView, ViewMask.None)

function StoryMarryBlessView:OnClickBless()
    ViewMgr:OpenView(WeddingFBBlessView)
end


