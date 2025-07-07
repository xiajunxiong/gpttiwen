VIEW_REQ("modules/treasure_hunt/treasure_hunt_view")
VIEW_REQ("modules/treasure_hunt/treasure_hunt_rate_view")
VIEW_REQ("modules/treasure_hunt/treasure_hunt_detail_view")
VIEW_REQ("modules/treasure_hunt/treasure_hunt_pet_exchange")
VIEW_REQ("modules/treasure_hunt/treasure_hunt_pet_pray_select")
VIEW_REQ("modules/treasure_hunt/treasure_hunt_xun_qi_panel")

FILE_REQ("modules/treasure_hunt/treasure_hunt_data")

TreasureHuntCtrl = TreasureHuntCtrl or BaseClass(BaseCtrl)

function TreasureHuntCtrl:__init()
    if TreasureHuntCtrl.Instance ~= nil then
        Debuger.LogError("[TreasureHuntCtrl] attempt to create singleton twice!")
        return
    end
    TreasureHuntCtrl.Instance = self
    self.data = TreasureHuntData.New()
    self:RegisterProtocol(SCTreasureHuntRet, "onSCTreasureHuntRet")
    self:RegisterProtocol(SCXunBaoOpenInfo, "onSCXunBaoOpenInfo")
    self:RegisterProtocol(SCXunBaoDrawTimesInfo,"onSCXunBaoDrawTimesInfo")
    self:RegisterProtocol(SCXunBaoBlessInfo,"onSCXunBaoBlessInfo")
    self:RegisterProtocol(CSTreasureHuntReq)
    self:RegisterProtocol(CSXunBaoExchange)
    self:RegisterProtocol(CSXunBaoBlessReq)
end

function TreasureHuntCtrl:__delete()
    if TreasureHuntCtrl.Instance ~= nil then
        TreasureHuntCtrl.Instance = nil
    end
end
function TreasureHuntCtrl:OnInit()
    RemindCtrl.Instance:Remind():Register(
        Mod.TreasureHunt.Main,
        BagData.Instance.item_grid[ItemColumnType.Item],
        function()
            TreasureHuntData.Instance.smart_data:SetDirty("FreeRemind")
        end
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.TreasureHunt.Main,
        TreasureHuntData.Instance.smart_data,
        BindTool.Bind(self.FlushHuntRemind, self, HUNT_TYPE.HUAN_LING),
        "FreeRemind"
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.TreasureHunt.XunQi,
        TreasureHuntData.Instance.smart_data,
        BindTool.Bind(self.FlushHuntRemind, self, HUNT_TYPE.XUN_QI),
        "FreeRemind"
    )
    Mod.RegisterView(Mod.TreasureHunt.Partner,TreasureHuntView)
    ViewMgr:RegisterOpenKeyFunc(Mod.TreasureHunt.Partner, function ()
        ViewMgr:OpenView(TreasureHuntView, {open_param = 1 ,auto_select = 5})
    end)
    self.data:initConfigByGroup()
end

function TreasureHuntCtrl:FlushHuntRemind(treasure_type)
    return self.data:CheckHuntNum(treasure_type)
end

function TreasureHuntCtrl:onCSTreasureHuntReq(count, treasure_id, is_guide_draw, comsume_info, use_free)
    -- if BattleData.Instance:IsServerFighting() then
    --     PublicPopupCtrl.Instance:Center(Language.TreasureHunt.CannotDoInBattle)
    --     return
    -- end
    TreasureHuntData.Instance.smart_data.is_guide_draw = is_guide_draw
    local protocol = GetProtocol(CSTreasureHuntReq)
    protocol.is_guide_draw = is_guide_draw and is_guide_draw or 0
    protocol.draw_times = count
    protocol.draw_type = treasure_id
    protocol.comsume_info = comsume_info or {}
    protocol.is_use_free = use_free and 1 or 0
    -- LogError("comsume_info",comsume_info,"is_use_free", protocol.is_use_free,"treasure_id",treasure_id)
    SendProtocol(protocol)
end

function TreasureHuntCtrl:onSCTreasureHuntRet(protocol)
    if IS_EDITOR then
        LogError("唤灵结果下发 protocol",protocol)
    end
    TreasureHuntData.Instance.smart_data.node_item_list = {}
    TreasureHuntData.Instance.smart_data.node_item_list = protocol.node_item_list
    -- LogError(protocol.node_item_list)
    TreasureHuntData.Instance.treasure_hunt_count = #protocol.node_item_list
    TreasureHuntData.Instance.smart_data.sended_req = false
    TreasureHuntData.Instance.smart_data.is_get = not TreasureHuntData.Instance.smart_data.is_get
    TreasureHuntData.Instance.last_index = -1
    for i=1, TreasureHuntData.Instance.treasure_hunt_count do
        local info = protocol.node_item_list[i]
        if info.type == 1 then
            -- 1是宠物
            ChatCtrl.Instance:ChannelChatCustom {pet_get = {index = info.id}}
        elseif info.type == 2 then
            -- 2是伙伴
            local co = Cfg.PartnerById(info.id)
            if co then
                local item = Item.Init(co.activation_item_norex, info.num)
                ChatCtrl.Instance:ChannelChatCustom {item_get = {item = item, num = info.num}}
            end
            TreasureHuntData.Instance.last_index = TreasureHuntData.Instance.treasure_hunt_count > 1 and i or -1
        elseif info.type == 0 then
            -- 0是道具
            local item = Item.Init(info.id, info.num)
            if item:ShowType() == ShowType.PetSkin then
                local co = Item.GetConfig(info.id)
                if PetData.Instance:GetTempSkinHasIsActive(co.param1) then
                    info.is_active = true
                    local skin_cfg = Cfg.SkinPetInfoByActiveStuffId(info.id)
                    -- 已激活就要给碎片
                    ChatCtrl.Instance:ChannelChatCustom {item_get = {item = Item.Init(skin_cfg.decompose_item, skin_cfg.decompose_num), num = skin_cfg.decompose_num}}
                else
                    info.is_active = false
                    -- 未激活给皮肤
                    ChatCtrl.Instance:ChannelChatCustom {item_get = {item = item, num = info.num}}
                    PetData.Instance:SetTempSkinHasIsActive(co.param1)
                end
            else
                ChatCtrl.Instance:ChannelChatCustom {item_get = {item = item, num = info.num}}
            end
        end
    end
    -- Debuger.LogError("aaaaaaaaaa")
    -- LogError(protocol.node_item_list)
    -- if not TreasureHuntData.Instance.is_detail_opened then
    --     TimeHelper:AddDelayTimer(function()
    --         ViewMgr:CloseView(TreasureHuntView)
    --         ViewMgr:OpenView(TreasureHuntDetailView)
    --     end,0.5)
    -- end
end

function TreasureHuntCtrl:onSCXunBaoOpenInfo(protocol)
    TreasureHuntData.Instance:SetOpenInfo(protocol.open_info)
    TreasureHuntData.Instance:FlushNeedItem()

    -- 新手寻宝获得第一个宠物的指引屏蔽 22.12.10
    -- if TreasureHuntData.Instance.force_open_guide then
    --     local item_id = GuideData.Instance:GetTreasureHuntPetHuntItem()
    --     if BagData.Instance:GetNumByItemId(item_id) ~= 0 then
    --         TreasureHuntData.Instance:GetOpenedTreasureID()
    --         GuideManager.Instance:Start(56) --id56是寻宝获得新手宠物的指引id
    --         TreasureHuntData.Instance.force_open_guide = false
    --     end
    -- end
    -- TreasureHuntData.Instance:CheckRumorOpen()
end

function TreasureHuntCtrl:onSCXunBaoDrawTimesInfo(protocol)
    TreasureHuntData.Instance:SetGroupTimes(protocol)
    TreasureHuntData.Instance:SetFirstDrawFlag(protocol.first_draw_flag)
    TreasureHuntData.Instance:SetSecondDrawFlag(protocol.second_draw_flag)
    TreasureHuntData.Instance:SetFreeTimes(protocol.treasure_group_free_times)
    -- LogError("onSCXunBaoDrawTimesInfo",protocol)
    self.data.smart_data:SetDirty("FreeRemind")
end

function TreasureHuntCtrl:onSCXunBaoBlessInfo(protocol)
    TreasureHuntData.Instance:SetBlessInfo(protocol)
end

function TreasureHuntCtrl.sendBlessReq(index,pet_id)
    local protocol = GetProtocol(CSXunBaoBlessReq)
    protocol.index = TreasureHuntData.Instance.pets_bless_info.cur_pray_index
    protocol.pet_id = TreasureHuntData.Instance.pets_bless_info.cur_pray_id
    SendProtocol(protocol)
end

function TreasureHuntCtrl:SendCSXunBaoExchange()
    if BattleData.Instance:IsServerFighting() then
        PublicPopupCtrl.Instance:Center(Language.TreasureHunt.CannotExchangeInBattle)
        return
    end
    local protocol = GetProtocol(CSXunBaoExchange)
    protocol.draw_type = self.data.smart_data.cur_treasure_id   --对应寻宝id
    protocol.get_index = self.data.exchange_smart.compose_index - 1  --对应寻宝id的奖励组中选择的奖励的序号
    protocol.comsume_index1 = self.data.exchange_smart.comsume_index1 - 1 --消耗材料在对应消耗组的序号
    protocol.comsume_index2 = self.data.exchange_smart.comsume_index2 - 1
    protocol.comsumeList = {}
    for i=1, 2 do
        local t = {}
        t.comsume_type = i - 1 --消耗材料组序号（0是组1，1是组2）
        t.type = self.data.exchange_smart["comsume_type"..i]          --物品所在队列（0是宠物列表，1是宠物仓库）
        t.index = self.data.exchange_smart["comsume_type_index"..i]         --消耗材料所在队列中的index
        table.insert(protocol.comsumeList, t)
    end
    -- LogError("SendCSXunBaoExchange protocol",protocol)
    SendProtocol(protocol)
end

function TreasureHuntCtrl:AutoTreasureHuntReq()
    local data = self.data.auto_buy_data or {}
    local item_num, items = BagData.Instance:GetAllItemsByItemId(data.item_id)
    if item_num > 0 then
        self:onCSTreasureHuntReq(data.draw_times, data.treasure_id, false, {[1] = {col = ItemColumnType.Item, idx = items[1]:GridIndex(), num = 1}})
        self.data.smart_data.auto_use = false
        self.data.auto_buy_data = {}
    end
end

-- 外部直接打开寻器中的魂器
function TreasureHuntCtrl:OpenTreasureHunQi()
    ViewMgr:OpenView(TreasureHuntView,{open_param = HUNT_TYPE.XUN_QI, auto_xunqi_type = XUN_QI_TYPE.HUN_QI})
end

-- 外部直接打开寻器中的圣器
function TreasureHuntCtrl:OpenTreasureShengQi()
    ViewMgr:OpenView(TreasureHuntView,{open_param = HUNT_TYPE.XUN_QI, auto_xunqi_type = XUN_QI_TYPE.SHENG_QI})
end