
function StoryData:StoryOptionsConfig()
    self.story_options = SmartData.New{
        cur_option_id = 0,
        state = StoryConfig.OptionsState.Start,
        options = {}
    }

    Cfg.Register("StoryOptionById",Cfg.RealTimeSearch(Config.story_auto.option,"id"))
    Cfg.Register("StoryOptionByTaskId",Cfg.RealTimeSearch2Key(Config.story_auto.option,"type","param1"))

    self.options_buttons = {}
    for _, button in ipairs(Config.story_auto.option_button) do
        self.options_buttons[button.button_group] = self.options_buttons[button.button_group] or {}
        if 2 == button.type then
            self.options_buttons[button.button_group][button.type_param1] = button
        else
            table.insert(self.options_buttons[button.button_group], button)
        end
    end

end

function StoryData:StoryOptionsData()
end

function StoryData:SetMainTaskChosedTaskList(protocol)
    -- self.story_options.options = protocol.options
    -- self.story_options.state = StoryConfig.OptionsState.Ready
end

-- function StoryData:PlayStoryOptionByTaskId(task_id)
--     local co = Cfg.StoryOptionByTaskId(0, task_id, false)
--     if co then
--         self:PlayStoryOptionByCo(co)
--     end
-- end

-- function StoryData:PlayStoryOptionById(id)
--     local co = Cfg.StoryOptionById(id, false)
--     if co then
--         self:PlayStoryOptionByCo(co)
--     else
--         LogError(string.format("story_auto表option分页id=%s配置不存在！", id))
--     end
-- end

-- function StoryData:PlayStoryOptionByCo(co)
--     self.story_options.cur_option_id = co.id
--     self.story_options.state = StoryConfig.OptionsState.Start
--     ViewMgr:OpenView(StoryOptionsView)
--     StoryCtrl.Instance:SendTaskChosedReqInfo()
    -- self:SetMainTaskChosedTaskList{options = {}}
-- end

function StoryData:ResetStoryOptions()
    self.story_options:Set{
        cur_option_id = 1,
        state = StoryConfig.OptionsState.Ready,
        options = {}
    }
end

function StoryData:NextStoryOptions()
    if self.story_options.cur_option_id < #Config.story_auto.option then
        self.story_options.cur_option_id = self.story_options.cur_option_id + 1
        self.story_options.state = StoryConfig.OptionsState.Ready
        return true
    else
        return false
    end
end

function StoryData:RecordOptions(button_group, button_id)
    self.story_options.options[button_group] = button_id
end

function StoryData:GetStoryOptionInfo()
    if self.story_options.cur_option_id > 0 and StoryConfig.OptionsState.Ready == self.story_options.state then
        self.story_options.show_text = {cur = "", pre = ""}
        for i = 1, self.story_options.cur_option_id do
            local co = Config.story_auto.option[i]
            if co then
                if self.story_options.cur_option_id == i then
                    self.story_options.show_text.cur = StoryData.ParseOptionSpecials(co.show_text)
                else
                    self.story_options.show_text.pre = self.story_options.show_text.pre .. StoryData.ParseOptionSpecials(co.show_text)
                end
                local buttons = self.options_buttons[co.button_group]
                local button_type = buttons[next(buttons)].type
                if i < self.story_options.cur_option_id then
                    local show_word
                    local button_id = self.story_options.options[co.button_group] or 0
                    for _, button in ipairs(buttons) do
                        show_word = show_word or RichTextHelper.ColorStr(button.show_word, button.show_color)
                        show_word = button.button_id == button_id and RichTextHelper.ColorStr(button.show_word, button.show_color) or show_word
                    end
                    self.story_options.show_text.pre = self.story_options.show_text.pre .. show_word
                elseif 2 == button_type then
                    self.story_options.options_buttons = {}
                    self.story_options.show_text.cur = self.story_options.show_text.cur .. (buttons[RoleData.Instance:GetRoleProfId()] or buttons[101]).show_word
                else
                    self.story_options.options_buttons = buttons
                end
            end
        end
    end
    return self.story_options
end

function StoryData:GetRecommend_job()
    local str_list = ""
    for k,v in pairs(Config.story_auto.recommend_job) do 
        local flag_1 = v.button1 == self.story_options.options[1]
        local flag_2 = v.button2 == self.story_options.options[2]
        local flag_3 = v.button3 == self.story_options.options[3]
        local flag_4 = v.button4 == self.story_options.options[4]
        if flag_1 and flag_2 and flag_3 and flag_4 then 
            str_list = v.recommend_job
        end 
    end 
    local prof_list = string.split(str_list, "|")
    local result = {}
    for i = 1,#prof_list do 
        local vo = {}
        vo.prof_id = tonumber(prof_list[i])
        table.insert( result, vo )
    end 

    return result
end

function StoryData.ParseOptionSpecials(show_text)
    return string.gsub(show_text, "<tab>", RichTextHelper.ColorStr("ttttt", COLORSTR.Transparent))
end
