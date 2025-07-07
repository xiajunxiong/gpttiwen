----------------------------SplaceAnecdoteData----------------------------
SplaceAnecdoteData = SplaceAnecdoteData or BaseClass()
function SplaceAnecdoteData:__init()
	if SplaceAnecdoteData.Instance ~= nil then
		Debuger.LogError("[SplaceAnecdoteData] attempt to create singleton twice!")
		return
	end
    SplaceAnecdoteData.Instance = self

    ActivityRandData.Instance:CustomClickHandle(ACTIVITY_TYPE.RAND_SPlace_Anecdote, function() 
        if ViewMgr:IsOpen(LabourDayView) or ViewMgr:IsOpen(AllSoulsFullView) then
            SceneLogic.Instance:AutoToNpc(Config.ambassador_activity_auto.other[1].npc_id,nil,false,true)
        else 
            ServerActivityCtrl.Instance:SendRandActivityOperaReq({rand_activity_type = ACTIVITY_TYPE.RAND_SPlace_Anecdote,opera_type = 0})
            if Item.GetNum(Config.may_day_activity_auto.other[1].item_id) == 0 then 
                AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
            end 
        end 
    end)

    self.quest_info = SmartData.New({question_id = -1,answer_index = -1,npc_id = 0})
    self.shop_info = SmartData.New({shop_list = {}})

    self.mall_cfg = KeyList(Config.getway_cfg_auto.get_way,"id")

    self.close_view = {
        [1] = SplaceAnecQuestView,
        [2] = SplaceAnecShopView,
    }
end 

function SplaceAnecdoteData:SplaceAnecQuestionInfo(protocol)
    -- LogError("?SplaceAnecQuestionInfo?",protocol)
    self.quest_info.question_id = protocol.question_id
    self.quest_info.answer_index = protocol.answer_index
    self.quest_info.is_true = protocol.is_true
end 

function SplaceAnecdoteData:SplaceAnecShopInfo(protocol)
    -- LogError("?SplaceAnecShopInfo?",protocol)
    self.shop_info.shop_list = protocol.shop_list
end 

function SplaceAnecdoteData:SplaceAnecShopBuyTimeInfo(protocol)
    -- LogError("?SplaceAnecShopBuyTimeInfo?",protocol)
    for i = 1, 15 do 
        if self.shop_info.shop_list[i] then 
            self.shop_info.shop_list[i].buy_time = protocol.buy_times[i]
        end 
    end 
end 

function SplaceAnecdoteData:SetOperateNpc(npc_id)
    self.quest_info.npc_id = npc_id
end 

function SplaceAnecdoteData:GetShopList(  )    
    local cur_list = {}
    for k,v in pairs(self.shop_info.shop_list) do 
        table.insert(cur_list,v)
    end 
    
	table.sort(cur_list, function(a, b)
		local itemA = a.buy_time - a.m_buy_time
		local itemB = b.buy_time - b.m_buy_time
        if itemA > itemB then
            return true
        else
            return false
        end
	end)

    for k,v in pairs(cur_list) do v.panel_index = k end 

    return cur_list
end

function SplaceAnecdoteData:SetCloseWindos(protocol)
    ViewMgr:CloseView(self.close_view[protocol.reason])
end