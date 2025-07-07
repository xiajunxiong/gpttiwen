
function ChatData:ChatConfigWorldQuestion()
	Cfg.Register("WorldQuestionQById", Cfg.RealTimeSearch(Config.world_question_cfg_auto.question_bank, "question_id"))
	Cfg.Register("WorldQuestionAById", Cfg.RealTimeSearch(Config.world_question_cfg_auto.answer_group, "group_id"))
end

function ChatData:ChatDataWorldQuestion()
end

function ChatData:SetWorldQuestionNotice(protocol)
	local info = protocol.info
	self.world_question_info:Set(protocol.info)
	if 0 == info.is_broadcast then
		self.world_question_ret = info.answer_ret
		self.role_answer_id = info.role_answer_id
	elseif 1 == info.is_broadcast and 0 == info.show_answer_tamp then
		self.world_question_ret = 0
		self.role_answer_id = 0
	end
end

function ChatData:WorldQuestionCoById(question_id)
	local co_q = Cfg.WorldQuestionQById(question_id) or {}
	local answer = co_q.answer or 1
	local co_a = Cfg.WorldQuestionAById(answer) or {}
	return co_q, co_a
end