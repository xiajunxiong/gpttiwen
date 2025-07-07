VIEW_REQ("modules/serveractivity/farewell_ask_neo/farewell_ask_neo_view")
FarewellAskNeoCtrl = FarewellAskNeoCtrl or BaseClass(BaseCtrl)
function FarewellAskNeoCtrl:__init()
    if FarewellAskNeoCtrl.Instance ~= nil then
		Debuger.LogError("[FarewellAskNeoCtrl] attempt to create singleton twice!")
		return
    end	
    FarewellAskNeoCtrl.Instance = self 
    
    self:RegisterProtocol(SCRandActivityCJYXInof,"OnActivityCJYXInof")

    self.farewell_info = SmartData.New()
end

function FarewellAskNeoCtrl:__delete()
    self:UnRegisterProtocol(SCRandActivityCJYXInof)
end

function FarewellAskNeoCtrl:OnActivityCJYXInof(protocol) 
    self.farewell_info.today_answer = protocol.today_answer
    self.farewell_info.id = protocol.id
    self.farewell_info.has_answer_list = protocol.has_answer_list
end

function FarewellAskNeoCtrl:SetActType(type)
    self.act_type = type
end

function FarewellAskNeoCtrl:CatchActtypeData( )
    if self.act_type == ACTIVITY_TYPE.RAND_FAREWELL_ASK_NEO then
        return self.farewell_info
    end 
end

function FarewellAskNeoCtrl:CatchQuestInfo()
    local result = {}
    result.ActTitle = "_Loc_CiJiuYingXin" -- 标题图
    for k,v in pairs(Config.looks_new_auto.gift_configure) do 
        if v.question_id ==  self.farewell_info.id then 
            result.reward_item = DataHelper.FormatItemList(v.reward_item)
            break
        end 
    end 
    for k,v in pairs(Config.looks_new_auto.question_bank) do 
        if v.question_id == self.farewell_info.id then 
            result.answer_id = tonumber(v.answer_id) -- 答案
            result.question_title = v.question -- 问题题目
            result.answer_list = {}  -- 选项
            for i = 1,3 do 
                local vo = {
                    str = v["anwser_"..i],  --选项名字
                    index = i, -- 标号
                    answer = result.answer_id , -- 答案
                }
                table.insert(result.answer_list,vo)
            end 
            break
        end 
    end
    result.has_answer_list = self.farewell_info.has_answer_list -- 已回答（取自协议
    result.today_answer = self.farewell_info.today_answer -- 今日已完成与否

    return result
end

function FarewellAskNeoCtrl:GetFarewellRemind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_FAREWELL_ASK_NEO) then
        return 0
    end
    return self.farewell_info.today_answer == 0 and 1 or 0
end