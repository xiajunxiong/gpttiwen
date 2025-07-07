
StoryOptionsView = StoryOptionsView or DeclareView("StoryOptionsView", StoryConfig.UIResPath .. "story_options")
VIEW_DECLARE_MASK(StoryOptionsView, ViewMask.BgBlock)

function StoryOptionsView:StoryOptionsView()
    self.data = StoryCtrl.Instance:Data()
    self.data:ResetStoryOptions()
end

function StoryOptionsView:LoadCallback()
    self.data.story_options.state = StoryConfig.OptionsState.Ready
end

StoryOptionsViewF = StoryOptionsViewF or DeclareMono("StoryOptionsViewF", UIWFlushPanel)

function StoryOptionsViewF:StoryOptionsViewF()
    self.data = StoryCtrl.Instance:Data()

    self.data_cares = {
		{data = self.data.story_options, func = self.FlushShow, keys = {"state"}},
    }
    self.show_speed = 0.05
end

function StoryOptionsViewF:FlushShow()
    local story_options = self.data:GetStoryOptionInfo()
    if StoryConfig.OptionsState.Ready == story_options.state then
        if 1 == story_options.cur_option_id then
            self.ContentT:ShowContent(story_options.show_text.cur, self.show_speed)
        else
            self.ContentT:ShowContentAdd(story_options.show_text.cur, self.show_speed, story_options.show_text.pre)
        end
    elseif StoryConfig.OptionsState.Record == story_options.state then
        self.Options:SetObjActive(false)
        self.ContentT:ShowContentAddColor(story_options.show_word, self.show_speed, story_options.show_text.pre .. story_options.show_text.cur, story_options.show_color)
    end
end

function StoryOptionsViewF:TypeWriterEnd()
    local story_options = self.data.story_options
    if StoryConfig.OptionsState.Record == story_options.state and self.data:NextStoryOptions() then return end
    -- self.Options:SetObjActive(StoryConfig.OptionsState.Ready == story_options.state)
    -- self.Options:SetData(story_options.options_buttons)

    local flag_block = 0 == #story_options.options_buttons or StoryConfig.OptionsState.Record == story_options.state
    self.BlockObj:SetActive(true )
    -- if not flag_block then 
    --     self.BlockObj:SetActive(flag_block)
    -- else 
    --     self.ProfRecShow:SetActive(flag_block)
    --     self.ProfList:SetData(self.data:GetRecommend_job())
    --     TimeHelper:AddDelayTimer(function() self.BlockObj:SetActive(flag_block) end,0.2)

    --     RoleCreateData.Instance:SetRecommendJob(self.data:GetRecommend_job())
    -- end
end

function StoryOptionsViewF:OnClickClose()
    ViewMgr:CloseView(StoryOptionsView)

    self.timer = TimeHelper:AddDelayTimer(function() 
        RoleCreateData.Instance:DelayBlack(true) 
        self.timer = nil
    end,0.2)
end

function StoryOptionsViewF:OnClickSkip()
    if self.ContentT:Skip() then self:TypeWriterEnd() end
end

StoryOptionsViewItem = DeclareMono("StoryOptionsViewItem", UIWidgetBaseItem)

function StoryOptionsViewItem:StoryOptionsViewItem()
    self.story_data = StoryCtrl.Instance:Data()
end

function StoryOptionsViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Content.text = data.button_behavior
end

function StoryOptionsViewItem:OnClickItem()
    -- StoryCtrl.Instance:SendTaskChosedReqRecord(self.data.button_group, self.data.button_id)
    self.story_data:RecordOptions(self.data.button_group, self.data.button_id)
    self.story_data.story_options.state = StoryConfig.OptionsState.Record
    self.story_data.story_options.show_word = self.data.show_word
    self.story_data.story_options.show_color = self.data.show_color
end

StroyProfRecCell = DeclareMono("StroyProfRecCell", UIWidgetBaseItem)

function StroyProfRecCell:StroyProfRecCell()

end

function StroyProfRecCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.prof_icon.SpriteName = RoleData.GetProfSp(self.data.prof_id,true)
    self.prof_name.text = Language.Common.Prof[data.prof_id]
end
