--密医 试炼之地 远征魔冢
FILE_REQ("modules/conspiracy/conspiracy_data")
--VIEW_REQ("modules/conspiracy/conspiracy")
VIEW_REQ("modules/conspiracy/conspiracy_menu")
VIEW_REQ("modules/conspiracy/conspiracy_reward")
VIEW_REQ("modules/conspiracy/conspiracy_modrons")
VIEW_REQ("modules/conspiracy/conspiracy_finish")
ConspiracyCtrl = ConspiracyCtrl or BaseClass(BaseCtrl)
function ConspiracyCtrl:__init()
    if ConspiracyCtrl.Instance ~= nil then
        Debuger.LogError("[ConspiracyCtrl] attempt to create singleton twice!")
        return
    end
    ConspiracyCtrl.Instance = self
    self.data = ConspiracyData.New()
    self:RegisterProtocol(SCMiYiInfo, "RecvMiYiInfo")

    self:RegisterProtocol(SCMiYiPoker, "RecvMiYiPoker")
    self:RegisterProtocol(SCMiYiPokerInfo, "RecvMiYiPokerInfo")
    self:RegisterProtocol(SCMiYiSceneInfo, "SCMiYiSceneInfo")
    self:RegisterProtocol(SCMiYiPokerKillPassInfo, "SCMiYiPokerKillPassInfo")

    self:RegisterProtocol(CSMiYiReq)

    BehaviorData.Instance:CustomNpcTalkBtnsSeq(161, BindTool.Bind(self.SetupBehaviourBtn, self), true)
    local func = BindTool.Bind(self.FlushMainRemind, self)
    RemindCtrl.Instance:Remind():Register(Mod.Challenge.ShiLian,
        BagData.Instance.item_grid,func)
    RemindCtrl.Instance:Remind():Register(Mod.Challenge.ShiLian,
        MedalData.Instance.medal_redtips,func)
    RemindCtrl.Instance:Remind():Register(Mod.Challenge.ShiLian,
        RoleData.Instance:GetBaseData(),func,"level", "capability")


    RemindCtrl.Instance:Remind():Register(Mod.Challenge.ShiLian,
    self.data.other_info,function ()
        return self.data:HeightReward()
    end)
    --建议使用 PokerCtrl.Instance:OnOpenPokerView(翻牌列表mypoker,type)
    -- FinishData.Instance:SetCloseFunc(
    --     BATTLE_MODE.BATTLE_MODE_MI_YI,
    --     function()
    --         local info = self.data:GetFinishReward()
    --         -- PokerCtrl.Instance:OnOpenPokerView(info.mypoker, POKER_REQ_OPER_TYPE.TYPE_MI_YI)
	-- 		if info.mypoker ~= nil then
    --             ViewMgr:OpenView(ConspiracyEffect)
    --         end
    --         --判断翻牌
	-- 		-- local info = self.data:GetFinishReward()
    --         -- PokerCtrl.Instance:OnOpenPokerView(info.mypoker, POKER_REQ_OPER_TYPE.TYPE_MI_YI)
	-- 		--[[ if info.mypoker ~= nil then
    --             local all_cant = true
	-- 			local role_id = RoleData.Instance:GetRoleId()
	-- 			for i, v in pairs(info.mypoker) do
	-- 				if v.role_uid == role_id and v.index == -1 then
	-- 					all_cant = false
	-- 					break
	-- 				end
	-- 			end
    --             --修改先展示特效然后再打开翻牌界面
	-- 			if not all_cant then
	-- 				PokerCtrl.Instance:OpenPokerView(POKER_REQ_OPER_TYPE.TYPE_MI_YI)
	-- 			end
	-- 		end ]]
    --     end
    -- )
    ActivityData.Instance:RegisterCondition(ActType.Conspiracy,function()
        if ActivityData.Instance:GetActivityInfo(ActType.Conspiracy).total_num == 0 then
            return Language.Activity.ActItem.DoPreTask
        end
        local total = ConspiracyData.Instance:GetOffList()[1].time
		local num = total - ConspiracyData.Instance:JoinTimes()
		num = num < 0 and 0 or num
		return Format(Language.Activity.ActItem.TodayCount, num, total)
	end)
end

function ConspiracyCtrl:OnInit()
    ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Modrons.Main,function()
        --[[ local total = self.data :GetOffList()[1].time
        local num =  total - self.data:JoinTimes()
        num = num < 0 and 0 or num
        return num--,total ]]
        -- ChallengeNaviCtrl.SetFormatDescNum(desc_val,num,total)
        return self.data:WeeklySurplus()
    end)

end


function ConspiracyCtrl:FlushMainRemind()
    return MedalData.Instance:GetMedalRedNumQuick()
end
function ConspiracyCtrl:OnUnloadGameLogic()
    self.data:FinishReward({})
    self.data.other_info.click_panel = false
    ViewMgr:CloseView(ConspiracyFinish)
end
function ConspiracyCtrl:RecvMiYiPoker(protocol)
    -- LogError("收到的翻牌列表:", protocol)
    local info = {
        draw_group_id = protocol.draw_group_id,
        timeout = protocol.timeout,
        mypoker = protocol.mypoker
    }
    self.data:FinishReward(info)
end
--建议使用 PokerData.Instance:GetMypokerByrIndex(info,protocol)
function ConspiracyCtrl:OnMiYiPokerChange(protocol)
    -- local info = self.data:GetFinishReward()
    -- if info.mypoker ~= nil and info.draw_group_id ~= nil and info.draw_group_id == protocol.draw_group_id then
    --     for i = 1, 5 do
    --         local temp = info.mypoker[i]
    --         if temp.role_uid == protocol.role_uid and temp.index == -1 then
    --             info.mypoker[i].index = protocol.draw_index
    --             break 
    --         end
    --     end
    -- end
end

function ConspiracyCtrl:MiYiPokerReq(reason, index)
	local role_uid = RoleData.Instance:GetRoleId()
	BattleCtrl.Instance:SendPokerReq(POKER_REQ_OPER_TYPE.TYPE_MI_YI, role_uid, index, reason)
end
function ConspiracyCtrl:RecvMiYiInfo(protocal)
    self.data:SetPassInfo(protocal.pass_info)
    self.data:SetFetchFlag(protocal.fetch_flag)
    local count,total = self.data:DailyInfo()
    ActivityData.Instance:SetActivityFinishInfo(ActType.Conspiracy, count, total)
end

function ConspiracyCtrl:SetupBehaviourBtn(beh_btn_list, obj_vo)
    if obj_vo and obj_vo.seq == 161 and table.nums(beh_btn_list) == 6 then
        local new_list = {}
        for _, v in ipairs(beh_btn_list) do
            local button =
                NpcDialogBehBtnData.FuncCreate(
                v.dis_str,
                function()
                    --判断等级
                    local cfg = BehaviorCtrl.GetCfg(v.beh_id)
                    local info = self.data:SceneInfo(cfg.param2)
                    local role_level = RoleData.Instance:GetBaseData().level
                    --[[ local is_finish = TaskData.Instance:GetTaskIsCompleted(info.pre_task) ]]
                    if role_level >= info.level --[[ and is_finish ]] then
                        BehaviorCtrl.ExeByCfgVo(cfg)
                    elseif role_level < info.level then
                        PublicPopupCtrl.Instance:Center(string.format(ErrorText[28], info.level))
                    --[[ elseif not is_finish then
                    PublicPopupCtrl.Instance:Center(string.format(ErrorText[150], info.task_name)) ]]
                    end
                end
            )
            button:SetIconType(v.icon_type)
            table.insert(new_list, button)
        end
        return new_list
    end
    return beh_btn_list
end
function ConspiracyCtrl:EnterScene()
    --自动到某个NPC打开对话框提供选择进入
    -- SceneLogic.Instance:AutoToPos(self.data:GetSceneId())
    -- MainViewCtrl.Instance:SetGameState(MainViewGameState.conspiracy)
    SceneCtrl.Instance:RequestEnterFb(FB_TYPE.MIYI)
end

function ConspiracyCtrl:IsInConsp()
    return SceneData.Instance:GetMode() == SceneModeType.Conspiracy
end

function ConspiracyCtrl:RecvMiYiPokerInfo(protocol)
    -- LogError(protocol)
    self.data:SetJoinTimes(protocol)
end
function ConspiracyCtrl:SCMiYiSceneInfo(protocol)
    -- LogDG("远征",protocol)
    self.data:SceneKillInfo(protocol.kill_flag)
end
function ConspiracyCtrl:SCMiYiPokerKillPassInfo(protocol)
    -- LogDG("远征服务端下发战斗结果", protocol)
    self.data.is_first = (protocol.is_first_kill == 1)
    self.data.is_week_first = (protocol.is_week_kill == 1)
end
function ConspiracyCtrl.CSMiYiReq(req_type, param1, param2)
    -- LogDG("发送扫荡协议",req_type, param1, param2)
    local procotol = GetProtocol(CSMiYiReq)
    procotol.req_type = req_type or 0
    procotol.param1 = param1 or 0
    procotol.param2 = param2 or 0
    SendProtocol(procotol)
end