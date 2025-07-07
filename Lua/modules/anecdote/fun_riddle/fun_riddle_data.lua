FunRiddleData = FunRiddleData or BaseClass()
function FunRiddleData:__init()
    if FunRiddleData.Instance ~= nil then
        Debuger.LogError("[FunRiddleData] attempt to create singleton twice!")
        return
    end
    FunRiddleData.Instance = self

    self.config = Config.fun_riddle_auto

    Cfg.Register("FunRiddleQuetion", Cfg.RealTimeSearch(self.config.question, "question_id"))
    Cfg.Register("FunRiddleAnswer", Cfg.RealTimeSearch(self.config.answer, "group_id"))

    self.operate = SmartData.New({
        flush = 0,       -- 刷新提醒器
        complete = 0,    -- 已完成的题目数
        question = 0,    -- 当前题目
        answer = 0,      -- 当前作答
    })


end 

function FunRiddleData:__delete()
    FunRiddleData.Instance = nil
end


function FunRiddleData:GameStart()
    local question_id = math.random(0, #self.config.question - 1)

    self.operate.question = question_id
    self.operate.complete = 1
    self.operate.answer = 0 

    self.operate.flush = self.operate.flush + 1
end

function FunRiddleData:FlushAnswer(index)
    self.operate.answer = index

end

function FunRiddleData:CheckIsRight()
    local a_cfg = Cfg.FunRiddleAnswer(self.operate.question)
    local result = self.operate.answer == a_cfg.answer_true

    return result,a_cfg.answer_true 
end

function FunRiddleData:NextQuestion()
    self.operate.complete = self.operate.complete + 1
    self.operate.answer = 0
    local question_id = math.random(0, #self.config.question - 1)
    self.operate.question = question_id
    self.operate.flush = self.operate.flush + 1
end

function FunRiddleData:CatchQuestionInfo()
    local q_cfg = Cfg.FunRiddleQuetion(self.operate.question)
    local a_cfg = Cfg.FunRiddleAnswer(self.operate.question)

    local param = {}
    param.question = q_cfg.question

    local answer_list = {}
    for i = 1,3 do 
        local vo = {}
        vo.index  = i
        vo.str = a_cfg["answer"..i]
        vo.is_right = i == a_cfg.answer_true
        answer_list[i] = vo
    end 
    param.answer_list = answer_list

    return param
end