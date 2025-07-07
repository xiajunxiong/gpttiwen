BagConfig = {
    --背包获得道具下发 1507行为
    GET_RESON_FUNC = {
        [GET_REASON_TYPE.MONSTER_WAVE] = function(protocol)
            FightDenData.Instance:SetFinishReward(protocol.item_list)
        end,
        [GET_REASON_TYPE.TEAMSURE_MAP_TASK] = function(protocol)
            PublicPopupCtrl.Instance:Center(string.format(ErrorText[68],Item.GetQuaName(protocol.item_list[1].item_id)))
        end,
        [GET_REASON_TYPE.CRYSTAL_FB_BOSS] = function(protocol)
            FightData.Instance:SetCrystalRewardData(TableCopy(protocol.item_list), protocol.param1)
        end,
        [GET_REASON_TYPE.FANTASY_FB] = function(protocol)
            FightData.Instance:SetFantasyRewardData(protocol.item_list)
        end,
        [GET_REASON_TYPE.TASK] = function(protocol)
            local item_list = DataHelper.FormatItemList(protocol.item_list)
            if MedalTaskData.Instance.task_config_list[protocol.param1] then 
                local info = MedalTaskData.Instance:GetSpReward(nil, protocol.param1)
                if info ~= nil then
                    ViewMgr:OpenView(TreasureMapShow, {item = Item.Create(protocol.item_list[1]), close_func = function ()
                        ConspiracyData.Instance:SetOpenTask(protocol.param1 + 1)
                    end})
                end
            elseif protocol.param1 == 70 or protocol.param1 == 362 then
                AllSoulsStorybookCtrl.Instance:OpenRewardView(item_list)
            elseif protocol.param1 == 410 then
                local info = PokerData.Instance:GetNormalPoker(POKER_REQ_OPER_TYPE.NORMAL_FALSE, protocol.item_list)
	            PokerData.Instance:FinishReward(info)
                PokerCtrl.Instance:OnOpenPokerFalseView(info.mypoker,info.draw_group_type)
            else
                PublicPopupCtrl.Instance:ShowCustom(item_list)
            end
        end,
        [GET_REASON_TYPE.RAND_GIFT] = function(protocol)
            local item_id = tonumber(protocol.param1) or 0
            local use_num = tonumber(protocol.param2) or 0
            local list = {}
            -- 神印圣器符石不走背包，剔除
            for i,v in pairs(protocol.item_list) do
                local item_type = Item.GetType(v.item_id)
                if item_type ~= ItemType.PetStamp and item_type ~= ItemType.ShengQi and item_type ~= ItemType.FuShi and item_type ~= ItemType.FairyDragonSoul then
                    table.insert(list, v)
                end
            end
            local item_list = DataHelper.FormatItemList(list, 1)
            if use_num == 1 then
                if BagData.IsGiftPopup(item_id) then  --是否是礼包表的
                    ViewMgr:OpenView(GiftOpenView,{item_id = item_id,item_list = item_list}) --特殊根据item_id弹对应的动画
                else 
                    local item_data = protocol.item_list[1] --主动消耗表的随机物品
                    if BagData.IsSurprise(item_id,item_data.item_id) then
                        ViewMgr:OpenView(GiftOpenView,{item = Item.New(item_data)}) --特殊根据item弹对应的动画
                    else
                        PublicPopupCtrl.Instance:ShowCustom(item_list)
                    end
                end
            else
                PublicPopupCtrl.Instance:ShowCustom(item_list)
            end
        end,
        [GET_REASON_TYPE.WILD_BOSS] = function(protocol)
            FightFieldBossData.Instance:SetWorldBossRewardData(protocol.item_list)
        end,
        [GET_REASON_TYPE.SHXY_BOSS] = function(protocol)
            FightFieldBossData.Instance:SetWorldBossRewardData(protocol.item_list)
        end,
        [GET_REASON_TYPE.GhostNight] = function(protocol)
            GhostNightData.Instance:SetWorldBossRewardData(protocol.item_list)
        end,
        [GET_REASON_TYPE.ANECDOTE_TASK] = function(protocol)
            if SceneData.Instance:GetMode() == SCENE_TYPE.WEN_XIANG_LOU then SceneCtrl.Instance:RequestLeaveFb() end --温香楼特殊处理
		    MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(protocol.item_list),GIFT_OPEN_TYPE.CONGRATULATIONS,true)
        end,
        [GET_REASON_TYPE.TASK_CHAIN_REWARD] = function(protocol)
            MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(protocol.item_list),GIFT_OPEN_TYPE.CONGRATULATIONS,true)
        end,
        [GET_REASON_TYPE.EQUIP_DECOMPOSE] = function(protocol)
            MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(protocol.item_list),nil,true,"_LocFenJieHuoDe")
        end,
        [GET_REASON_TYPE.PARTNER_LIENCE] = function(protocol)
            MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(protocol.item_list),nil,false,"_LocZhuanHuanSuiPian",true)
        end,
        [GET_REASON_TYPE.MATERIAL] = function(protocol)
            if BagData.Instance:CheckIsSweepByGetWay(protocol.item_list) then 
                if tonumber(protocol.param1) == 0 then
                    MaterialCtrl.Instance:OnSweepReward(DataHelper.FormatItemList(protocol.item_list))
                else
                    MaterialData.Instance:SetFinishRewardList(DataHelper.FormatItemList(protocol.item_list))
                end
            end
        end,
        [GET_REASON_TYPE.DREAMNOTES_PASS] = function(protocol)
            DreamNotesData.Instance:SetRandRewardData(protocol.item_list)
        end,
        [GET_REASON_TYPE.DREAMNOTES_SWEEP] = function(protocol)
            DreamNotesData.Instance:SetRandRewardData(protocol.item_list)
        end,
        [GET_REASON_TYPE.FABAO_ACTED] = function(protocol)
            MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(protocol.item_list),GIFT_OPEN_TYPE.SPECIAL,false,"_LocZhuanHuanJingHua")
		    PublicPopupCtrl.Instance:Center(Language.Fabao.ActedHint)
        end,
        [GET_REASON_TYPE.TIAN_DI_DAO_HEN] = function(protocol)
            MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(protocol.item_list),GIFT_OPEN_TYPE.CONGRATULATIONS,true)
        end,
        [GET_REASON_TYPE.TaskChain] = function(protocol)
            MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(protocol.item_list),GIFT_OPEN_TYPE.SPECIAL,true)
        end,
        [GET_REASON_TYPE.RAND_ACTIVITY] = function(protocol)
            ServerActivityCtrl.Instance:HandleRewardItems(protocol.param1, protocol.item_list)
        end,
        [GET_REASON_TYPE.GO_THROUGH] = function(protocol)
            GoThroughData.Instance:SetRewardData(protocol.item_list)
        end,
        [GET_REASON_TYPE.REALM_BREAK_FIGHT] = function(protocol)
            -- 删除灵力值
            -- ChatCtrl.Instance:ChannelChatCustom {item_get = {item = Item.Init(CommonItemId.LingLiZhi), num = protocol.param1}}
            for i = 1, #protocol.item_list do
                local item = protocol.item_list[i]
                if CommonItemId.Exp ~= item.item_id then
                    ChatCtrl.Instance:ChannelChatCustom {item_get = {item = Item.Create(item), num = item.num}}
                end 
            end
            -- table.insert(protocol.item_list, 1, {item_id = CommonItemId.LingLiZhi, num = protocol.param1, is_bind = 1})
            FightData.Instance:SetFightSoulRewardData(protocol.item_list)
        end,
        [GET_REASON_TYPE.HORCRUX_TIANCHENG] = function(protocol)
            if protocol.param1 >= 0 then
                table.insert(protocol.item_list, 1, {item_id = HuoBi[protocol.param1], num = protocol.param2, is_bind = 1})
            end
            ChatCtrl.Instance:ChannelChatCustom {item_get = {item = Item.Create({item_id = protocol.item_list[1].item_id, num = protocol.item_list[1].num})}}
            if 0 == UnityPlayerPrefs.GetInt(PrefKeys.HorcruxTianchengSkip()) then
                TimeHelper:AddDelayTimer(function ()
                    MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(protocol.item_list),GIFT_OPEN_TYPE.CONGRATULATIONS,true)
                end, 5)
            else
                MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(protocol.item_list),GIFT_OPEN_TYPE.CONGRATULATIONS,true)
            end
        end,
        [GET_REASON_TYPE.ENDLESS_TOWER] = function(protocol)
            EndlessTowerData.Instance:SetFinishRewardData(protocol.item_list)
            EndlessTowerData.Instance:SetRewardDataList(protocol.item_list,Format(Language.RushDungeon.LevelName,EndlessTowerData.Instance:GetPassLevel()))
        end,
        [GET_REASON_TYPE.CLOUD_SWAMP] = function(protocol)
            ServerActivityData.Instance:SetCloudSwampFinishData(protocol.item_list)
        end,
        [GET_REASON_TYPE.YUNZEERSANSHI] = function(protocol)
            TimeHelper:AddDelayTimer(function ()
                MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(protocol.item_list),GIFT_OPEN_TYPE.CONGRATULATIONS,true)
            end, 5)
        end,
        [GET_REASON_TYPE.ALL_SOULS_FB] = function (protocol)
            local item_list = {}
            for _,v in pairs(protocol.item_list) do
                if v.item_id ~= CommonItemId.Exp then
                    table.insert(item_list, v)
                end
            end
            local list = DataHelper.FormatItemList(item_list)
            AllSoulsStorybookData.Instance:CacheReward(list)
            -- AllSoulsStorybookCtrl.Instance:OpenRewardView(list)
            MainOtherCtrl.Instance:OpenGiftView(list,GIFT_OPEN_TYPE.CONGRATULATIONS,false, nil, false, 3)
        end,
        [GET_REASON_TYPE.JIXINGGAOZHAO] = function (protocol)
            if protocol.param1 > 0 then
                table.insert(protocol.item_list,1,{item_id = HuoBi[CurrencyType.Gold], num = protocol.param1})
            end
            for k, v in pairs(protocol.item_list) do
                ChatCtrl.Instance:ChannelChatCustom {item_get = {item = Item.Create(v), num = v.num}}
            end
            if #protocol.item_list > 1 then
                ActRewardTipData.Instance:OpenActRewardTipViewByData(ServeractivityConfig.ActRewerdTip.RewardType.Success, protocol.reward_list)
            -- else
            --     ActJiXingGaoZhaoData.Instance:SetGetReward(protocol)
            end
        end,
        [GET_REASON_TYPE.BOUNTY_TASK_REWARD] = function (protocol)
            ViewMgr:OpenView(GetRewardView,{
                exp = protocol.param1,
                is_not_show = true,
                desc = Format(Language.Task.BountyTaskRewardTitle,10),
                item_list = DataHelper.FormatItemList(protocol.item_list),
            })
        end,
        [GET_REASON_TYPE.TREASURE_TASK] = function (protocol)
            TreasureTaskData.Instance:SetResultInfo(TableCopy(protocol))
        end,
        [GET_REASON_TYPE.TREASURE_TASK_ALL] = function (protocol)
            TreasureTaskData.Instance:SetResultInfoAll(protocol)
        end,
        [GET_REASON_TYPE.FALLEN_GOD] = function (protocol)
            FallenGodData.Instance:SetRewardList(DataHelper.FormatItemList(protocol.item_list))
        end,
        [GET_REASON_TYPE.XIN_XIN_XIANG_YING] = function (protocol)
            local coin_count = protocol.param1
            local item_list = DataHelper.FormatItemList(protocol.item_list)
            if coin_count > 0 then
                local item = Item.Create({item_id = CommonItemId.Coin, num = coin_count})
                table.insert(item_list, item)
            end
            ViewMgr:OpenView(GiftOpenView,{item_list = item_list,gift_type = GIFT_OPEN_TYPE.SPECIAL})
        end,
        [GET_REASON_TYPE.PARTNER_PACT] = function (protocol)
            for k, v in pairs(protocol.item_list) do
                local item_data = Item.Create(v)
                PublicPopupCtrl.Instance:CenterI({item_id = v.item_id, num = v.num, color_str = item_data:ColorStr()})
                ChatCtrl.Instance:ChannelChatCustom {item_get = {item = item_data, num = v.num}}
            end
        end,
        [GET_REASON_TYPE.SHITU_CHUANGONG] = function (protocol)
            SocietyData.Instance:SetShiTuChuanGongRewardShow(protocol)
        end,
        [GET_REASON_TYPE.PUZZLE_FAIRY] = function(protocol)
		    MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(protocol.item_list),GIFT_OPEN_TYPE.CONGRATULATIONS)
        end,
        -- 山海灯会
        [GET_REASON_TYPE.MS_LANTERN] = function(protocol)
            MSLanternData.Instance:SetLastItemList(DataHelper.FormatItemList(protocol.item_list))
        end,
        -- 科举考试
        [GET_REASON_TYPE.IMPERIAL_EXAM] = function(protocol)
            ImperialExamData.Instance:SetLastItemList(DataHelper.FormatItemList(protocol.item_list))
        end,
        [GET_REASON_TYPE.MONEY_BOX] = function(protocol)
            ItemInfoData.Instance.money_box_info:Set{param2 = protocol.param2}
        end,
        [GET_REASON_TYPE.LIANHUAN_TASK] = function(protocol)
            local co = Config.tasklist_auto.chain_go_on[protocol.param1]
            if co then
                local chapter = Cfg.TaskListChainShowByGroupChapter(co.task_group, co.chain_chapter)
                if co.chain_chapter < 5 then
                    if 1 == co.task_group then
                        MainOtherCtrl.Instance:SetWaitViewFunc(function()
                            ViewMgr:OpenView(BranchSerialTaskView)
                        end)
                    elseif 2 == co.task_group then
                        MainOtherCtrl.Instance:SetWaitViewFunc(function()
                            ViewMgr:OpenView(BranchMarbasTaskView)
                        end)
                    else
                        MainOtherCtrl.Instance:SetWaitViewFunc(function()
                            ViewMgr:OpenView(BranchRelicTaskView)
                        end)
                    end
                end
                ViewMgr:OpenView(GetRewardView,{
                    is_not_show = true,
                    exp = protocol.param2,
                    start_guide = (co.chain_chapter == 5 and 2 == co.task_group) and 1 or 0,
                    desc = string.format(Language.Task.BranchSerialTask.RewardTip, string.format(Language.Task.BranchSerialTask.TitleNameShow2, chapter.chapter_show, chapter.chapter_name)),
                    item_list = DataHelper.FormatItemList(protocol.item_list),
                    task_id = co.word_task,
                })
            end
        end,
        [GET_REASON_TYPE.BUN_COME] = function (protocol)
            MainOtherCtrl.Instance:OpenGiftOpenBunComeView(DataHelper.FormatItemList(protocol.item_list), 1 == protocol.param1, 5)
        end,
        [GET_REASON_TYPE.VITALITY_FIREWORK] = function(protocol)
            ViewMgr:OpenView(VitalityFireworkUseView, {show_list = DataHelper.FormatItemList(protocol.item_list)})
        end,
        [GET_REASON_TYPE.VITALITY_CARD] = function(protocol)
            local item = Item.Create(protocol.item_list[1])
            VitalityFlopData.Instance:SetCardReward(item, true)
            -- ViewMgr:OpenView(VitalityFireworkUseView, {show_list = DataHelper.FormatItemList(protocol.item_list)})
        end,
        [GET_REASON_TYPE.HARMONY_HEAVEN] = function(protocol)
            HarmonyHeavenData.Instance:SetResultInfoAll(protocol)
        end,
        [GET_REASON_TYPE.GET_REASON_ZAO_HUA_DISCOMPOSE] = function(protocol)
            ViewMgr:OpenView(MarbasResolveReward, {item_list = protocol.item_list})
        end,
        [GET_REASON_TYPE.DUAN_YANG_XIANG_NANG]= function(protocol) 
            ViewMgr:OpenView(NoonRewardView, {item_list = protocol.item_list})
        end,
        [GET_REASON_TYPE.SOUP_BATH]= function(protocol)
            SoupBathData.Instance:SetLastReward(protocol.item_list)
        end,
        [GET_REASON_TYPE.TIAN_YI_YAO_SHI] = function (protocol)
            ViewMgr:OpenView(TianYiYaoShiResultView, {list = protocol.item_list})
        end,
        [GET_REASON_TYPE.LING_QI_ACT_CHANGE] = function (protocol)
            MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(protocol.item_list),nil,false,"_LocZhuanHuaLingQi",true)
        end,
        [GET_REASON_TYPE.TEAM_CHALLENGE_REWARD] = function(protocol) -- 组队挑战奖励
            if #protocol.item_list ~= 0 then
                ChallengeTaskData.Instance.team_succ_reward = protocol.item_list
            end
        end,
        [GET_REASON_TYPE.YUE_XI_DENG_MI] = function(protocol)
            local timer = Invoke(function()
                TimeHelper:CancelTimer(timer)
                ViewMgr:OpenView(YxdmRewardView, {
                    item_list = protocol.item_list
                })
                for k, v in pairs(protocol.item_list) do
                    local item_data = Item.Create(v)
                    PublicPopupCtrl.Instance:CenterI({item_id = v.item_id,num = v.num,color_str = item_data:ColorStr(),})
                    ChatCtrl.Instance:ChannelChatCustom{ item_get = { item = item_data,num = v.num,},}
            end
            end, 1)
        end,
        [GET_REASON_TYPE.LING_BAO_DECOMPOSE]= function(protocol)
            ViewMgr:OpenView(LingBaoResolveReward, {item_list = protocol.item_list})
        end,
    }
}