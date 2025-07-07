
ImmortalRecruitStudentData = ImmortalRecruitStudentData or BaseClass()
function ImmortalRecruitStudentData:__init()
    if ImmortalRecruitStudentData.Instance ~= nil then
        Debuger.LogError("[ImmortalRecruitStudentData] attempt to create singleton twice!")
        return
    end
    ImmortalRecruitStudentData.Instance = self
    self:ImmortalRecruitStudentData()
end

function ImmortalRecruitStudentData:ImmortalRecruitStudentData()
    self.config = Config.immortal_recruit_student_auto
    self.config_answer = self.config.answer

    self.answer_list = {}
    for k, v in ipairs(self.config_answer) do
        table.insert(self.answer_list, v.answer_number)
    end

    self.selections = {}
    for i = 1, 10 do
        table.insert(self.selections, {
            num = i % 10,
            num_show = DataHelper.GetDaXie(i % 10, true)
        })
    end

    self.answer_show = SmartData.New({
        answer_num = 10,
        selections = SmartData.New{
            [1] = -1,
            [2] = -1,
            [3] = -1,
            [4] = -1,
        },
        sel_seq = 1,
        result = 0,
        results = {},
        state = 0,
        flush_results = false,
        result_t = {},
    })
end

function ImmortalRecruitStudentData:GetRamdonAnswer()
    return self.answer_list[GameMath.Ramdon(1, #self.answer_list)]
end

function ImmortalRecruitStudentData:GetSelections()
    return self.selections
end

function ImmortalRecruitStudentData:SelectNum(num)
    local as = self.answer_show
    if as.sel_seq > 0 then
        as.selections[as.sel_seq] = num
        local sel_seq = 0
        for i = 1, 4 do
            if -1 == as.selections[i] then
                sel_seq = i
                break
            end
        end
        as.sel_seq = sel_seq
    end
end

function ImmortalRecruitStudentData:IsNumSelected(num)
    local selections = self.answer_show.selections
    for i = 1, 4 do
        if num == selections[i] then
            return true
        end
    end
    return false
end

function ImmortalRecruitStudentData:SelectAnswer(index)
    local as = self.answer_show
    as.sel_seq = index
    as.selections[index] = -1
end

function ImmortalRecruitStudentData:ResetAnswerShow(answer_num)
    self.answer_show.selections:Set{
        [1] = -1,
        [2] = -1,
        [3] = -1,
        [4] = -1,
    }
    self.answer_show.answer_num = answer_num or 10
    self.answer_show.sel_seq = 1
    if nil == answer_num then
        self.answer_show.result = self:GetRamdonAnswer()
        self.answer_show.result_t = self.NumToTable(self.answer_show.result)
        self.answer_show.results = {}
        self.answer_show.state = 0
    end
end

function ImmortalRecruitStudentData:ResultConfirm()
    local as = self.answer_show
    local selections = as.selections
    local result = 0
    local all_right, num_right = 0, 0
    for i = 1, 4 do
        result = result + math.pow(10, 4 - i) * selections[i]
        if -1 == selections[i] then
            PublicPopupCtrl.Instance:Center(Language.ImmortalRecruitStudent.AnswerTips)
            return
        end
    end
    for i = 1, 4 do
        local seq = as.result_t[selections[i]]
        if seq then
            if seq == i then
                all_right = all_right + 1
            else
                num_right = num_right + 1
            end
        end
    end

    if result == as.result then
        as.state = 2
        as.answer_num = as.answer_num - 1
    else
        if as.answer_num > 1 then
            self:ResetAnswerShow(as.answer_num - 1)
        else
            as.state = 1
            as.answer_num = as.answer_num - 1
        end
    end
    table.insert(as.results, {
        answer = result,
        all_right = all_right,
        num_right = num_right,
    })
    as.flush_results = not as.flush_results
end

function ImmortalRecruitStudentData.NumShow(num)
    local str = ""
    local value = 0
    for i = 1, 4 do
        local power10 = math.pow(10, i)
        value = num % power10
        str = DataHelper.GetDaXie(value * 10 / power10, true) .. str
        num = num - value
    end
    return str
end

function ImmortalRecruitStudentData.NumToTable(num)
    local t = {}
    local value = 0
    for i = 1, 4 do
        local power10 = math.pow(10, i)
        value = num % power10
        t[value * 10 / power10] = 5 - i
        num = num - value
    end
    return t
end