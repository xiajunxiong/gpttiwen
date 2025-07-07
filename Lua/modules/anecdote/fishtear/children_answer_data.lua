ChildrenAnswerData = ChildrenAnswerData or BaseClass()

ChildrenAnswerData.TaskTag = {
    Ask = 65240,
    AskAngin = 65241,
}

function ChildrenAnswerData:__init()
	if ChildrenAnswerData.Instance ~= nil then
		ChildrenAnswerData.LogError("[FishAnswerData] attempt to create singleton twice!")
		return
	end
    ChildrenAnswerData.Instance = self

    self.config = Config.children_question_auto
end 