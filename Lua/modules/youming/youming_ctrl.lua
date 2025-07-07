FILE_REQ("modules/youming/youming_data")
FILE_REQ("modules/youming/youming")
FILE_REQ("modules/youming/youming_cfg")

VIEW_REQ("modules/youming/youming_view")
VIEW_REQ("modules/youming/youming_lottery_view")
VIEW_REQ("modules/youming/youming_compound_view")
VIEW_REQ("modules/youming/youming_reslove_view")
VIEW_REQ("modules/youming/youming_dictionaries_view")
VIEW_REQ("modules/youming/youming_pet_select_view")
VIEW_REQ("modules/youming/youming_info_view")
VIEW_REQ("modules/youming/youming_seal_view")

VIEW_REQ("modules/youming_practice/youming_practice")
VIEW_REQ("modules/youming_practice/youming_base")
VIEW_REQ("modules/youming_practice/youming_gaizao")
VIEW_REQ("modules/youming_practice/youming_juexing")
VIEW_REQ("modules/youming_practice/youming_mingzhuang")
VIEW_REQ("modules/youming_practice/youming_sanhun")
VIEW_REQ("modules/youming_practice/youming_qipo")
VIEW_REQ("modules/youming_practice/youming_attr_detail")

VIEW_REQ("modules/youming_practice/youming_remodel")
VIEW_REQ("modules/youming_practice/youming_re_qua")
VIEW_REQ("modules/youming_practice/youming_skill_learn")

YouMingCtrl = YouMingCtrl or BaseClass(BaseCtrl)

function YouMingCtrl:__init()
	if YouMingCtrl.Instance ~= nil then
		Debuger.LogError("[YouMingCtrl] attempt to create singleton twice!")
		return
	end
	YouMingCtrl.Instance = self
    YouMingData.New()
    self.data = YouMingData.Instance
    self:RegisterAllProtocols()
end

function YouMingCtrl:__delete()
    YouMingCtrl.Instance = nil
end

function YouMingCtrl:OnInit()
    RemindCtrl.Instance:Remind():Register(
        Mod.YouMing.Lottery,
        self.data.remind_data.lottrey_remind,
        function ()
            return self.data:GetLottreyRemind()
        end
    )    

    RemindCtrl.Instance:Remind():Register(
        Mod.YouMingPractice.SanHun,
        self.data:GetSelectYouMing(),
        function ()
            return self.data:GetSanHunRemind()
        end
    ) 
    RemindCtrl.Instance:Remind():Register(
        Mod.YouMingPractice.QiPo,
        self.data:GetSelectYouMing(),
        function ()
            return self.data:QiPoRemind()
        end
    ) 
    RemindCtrl.Instance:Remind():Register(
        Mod.YouMingPractice.JueXing,
        self.data:GetSelectYouMing(),
        function ()
            return self.data:JueXingRemind()
        end
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.YouMingPractice.GaiZao1,
        self.data:GetRemodelYouMing(),
        function ()
            return self.data:GaiZao1Remind()
        end
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.YouMingPractice.MingZhuang,
        self.data:GetSelectYouMing(),
        function ()
            return self.data:MingZhuangRemind()
        end
    )

    RemindCtrl.Instance:Remind():Register(
        Mod.YouMing.Compound,
        self.data.remind_data.compound_remind,
        function ()
            return self.data:GetCompoundRemind()
        end
    )
    BagData.Instance:ListenItems(self.data:GetLottreyRemindItemList(), function ()
        self.data:CheckLottreyRemind()
    end)
    BagData.Instance:ListenItems(self.data:GetCompoundRemindItemList(), function ()
        self.data:CheckCompoundRemind()
    end)
    BagData.Instance:ListenItems({56605,56606}, function ()
        self.data:GetSelectYouMing():Notify()
    end)
    
    self.on_scene_data_disposed = EventSys:Bind(GlobalEventKeys.SceneDataDisposed,BindTool.Bind(self.OnSceneDataDisposed, self))
    self.touch_began_handle = EventSys:Bind(GlobalEventKeys.TOUCH_BEGAN, BindTool.Bind(self.onTouchBegan, self))
end

function YouMingCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end

function YouMingCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSNetherWorldSystemReq)
    self:RegisterProtocol(CSNetherWorldBatchComposeReq)     -- //!< 5447 幽冥批量合成操作
    self:RegisterProtocol(CSNetherWorldBatchDecomposeReq)   -- //!< 5449 幽冥批量分解操作
    self:RegisterProtocol(CSNetherWorldGraspSkillReq)   -- //!< 5453 幽冥系统领悟技能请求
    self:RegisterProtocol(CSNetherWorldEquipAddExpReq)   -- //!< 5451 幽冥系统冥装添加经验请求
    self:RegisterProtocol(CSNetherWorldSealReq)             -- //!< 5760 幽冥抓捕申请
    self:RegisterProtocol(CSNetherWorldSystemAwakenLevelUpReq)             -- //!< 5455 幽冥觉醒升级
    self:RegisterProtocol(SCNetherWorldBagInfoAck, "OnSCNetherWorldBagInfoAck")         --//!< 5441 幽冥系统背包所有信息下发
    self:RegisterProtocol(SCNetherWorldBagSingleInfo, "OnSCNetherWorldBagSingleInfo")   --//!< 5442 幽冥系统背包单条信息下发
    self:RegisterProtocol(SCNetherWorldSystemBaseInfo, "OnSCNetherWorldSystemBaseInfo") --//!< 5443 幽冥系统基础信息下发
    self:RegisterProtocol(SCNetherWorldSystemNotice, "OnSCNetherWorldSystemNotice") --//!< 5444 幽冥系统相关通知下发
    self:RegisterProtocol(SCNetherWorldBatchComposeRet, "OnSCNetherWorldBatchComposeRet")   -- //!< 5448 幽冥批量合成 返回
    self:RegisterProtocol(SCNetherWorldAttrListInfo, "OnSCNetherWorldAttrListInfo")     --//!< 5450 幽冥系统幽冥属性信息下发
    self:RegisterProtocol(SCNetherWorldTreasureList, "OnSCNetherWorldTreasureList")     -- //!< 5452 幽冥系统探宝抽取下发
    self:RegisterProtocol(SCNetherWorldPossessList, "OnSCNetherWorldPossessList")     --//!< 5454 幽冥系统附身列表信息
    self:RegisterProtocol(SCNetherWorldSystemReformResultInfo, "OnSCNetherWorldSystemReformResultInfo")     --//!< 5454 幽冥系统附身列表信息
    self:RegisterProtocol(SCNetherWorldSystemWearSingleInfo, "OnSCNetherWorldSystemWearSingleInfo")     --//!< 5454 幽冥系统附身列表信息
    self:RegisterProtocol(SCNetherWorldSealInfo, "OnSCNetherWorldSealInfo")         --//!< 5761 幽冥抓捕信息
    self:RegisterProtocol(SCNetherWorldSealResult, "OnSCNetherWorldSealResult")     --//!< 5762 幽冥抓捕结果
    self:RegisterProtocol(SCNetherWorldAutoSealInfo, "OnSCNetherWorldAutoSealInfo")     --//!< 5762 幽冥是否自动抓捕
    self:RegisterProtocol(SCNetherWorldCheckTreasure, "OnSCNetherWorldCheckTreasure") -- 通知检查探宝信息
end

function YouMingCtrl:SendReq(req_type, p1, p2, p3)
    local protocol = GetProtocol(CSNetherWorldSystemReq)
    protocol.req_type = req_type
    protocol.p1 = p1 or 0
    protocol.p2 = p2 or 0
    protocol.p3= p3 or 0
    SendProtocol(protocol)
end

-- 批量合成
function YouMingCtrl:SendCompoundList(list)
    local protocol = GetProtocol(CSNetherWorldBatchComposeReq)
    protocol.list = list or {}
    SendProtocol(protocol)
end

-- 批量分解
function YouMingCtrl:SendResloveList(list)
    local protocol = GetProtocol(CSNetherWorldBatchDecomposeReq)
    protocol.list = list or {}
    SendProtocol(protocol)
end
-- 改造领悟
function YouMingCtrl:SendGraspSkillReq(bag_index,reform_index, skill_index, is_high_grasp, skill_book_item_id)
    local protocol = GetProtocol(CSNetherWorldGraspSkillReq)
    protocol.bag_index = bag_index 
    protocol.reform_index = reform_index 
    protocol.skill_index = skill_index 
    protocol.is_high_grasp = is_high_grasp --0普通
    protocol.skill_book_item_id = skill_book_item_id 
    SendProtocol(protocol)
    LOG(protocol)
end
--冥装升级
function YouMingCtrl:SendEquipAddExpReq(bag_index, equip_grid_type,equip_bag_index_list)
    local protocol = GetProtocol(CSNetherWorldEquipAddExpReq)
    protocol.bag_index = bag_index 
    protocol.equip_grid_type = equip_grid_type 
    protocol.equip_bag_index_list = equip_bag_index_list
    SendProtocol(protocol)
    LOG(protocol)
end

--幽冥觉醒
function YouMingCtrl:SendAwakenLevelUpReq(bag_index, stuff_list)
    local protocol = GetProtocol(CSNetherWorldSystemAwakenLevelUpReq)
    protocol.bag_index = bag_index
    protocol.stuff_list = stuff_list
    SendProtocol(protocol)
end
function YouMingCtrl:OnSCNetherWorldBagInfoAck(protocol)
    --LogDG("-1全发幽冥系统基础信息下发",protocol.bag_type, protocol.item_count)
    local vo_list = protocol.vo_list
    for i,v in ipairs(vo_list) do
        if v.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NW then
            local youming = YouMing.New(v)
            self.data:SetYouMing(youming)
            self:SendReq(YouMingCfg.ReqType.ATTR_INFO, v.index)
        elseif v.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NWE then
            local youming = YouMingEquip.New(v)
            self.data:SetYouMingEquip(youming)
        end
    end
end

function YouMingCtrl:OnSCNetherWorldBagSingleInfo(protocol)
    --LogDG("单个幽冥下发", protocol.vo.index)
    self.data:OnSCNetherWorldBagSingleInfo(protocol)
    if protocol.vo.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NW then
        self:SendReq(YouMingCfg.ReqType.ATTR_INFO, protocol.vo.index)
    end
end

function YouMingCtrl:OnSCNetherWorldSystemBaseInfo(protocol)
    self.data.lottery_info:Set(protocol.lottery_info)
end

function YouMingCtrl:OnSCNetherWorldSystemNotice(protocol)
    YouMingCtrl.HandleSCNetherWorldSystemNotice(protocol)
end
function YouMingCtrl.HandleSCNetherWorldSystemNotice(protocol)
    local notice_type = protocol.notice_type
    local data = YouMingData.Instance
    if notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_POSSESS_CHANGE then
        local youming = data:GetYouMing(protocol.p1)
        local pet = PetData.Instance:GetPetInfoByUniqueId(protocol.p4A,protocol.p4B)
        if pet then
            youming:SetPetIndex(pet.index)
            data:SetBlessInfo(protocol.p1, {pet_unique_id1 = protocol.p4A, pet_unique_id2 = protocol.p4B})
        else
            youming:SetPetIndex(nil)
            data:SetBlessInfo(protocol.p1, nil)
        end
        data:SetYouMing(youming)
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_SOUL_UNLOCK then
        -- LogDG("三魂解锁了！！！！！！！", protocol.p1, protocol.p2, protocol.p3)
        local youming = data:GetYouMing(protocol.p1)
        youming:SanHunLevelChange(protocol.p2 + 1, protocol.p3)
        data:GetSelectYouMing():Notify()
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_SOUL_LEVEL_UP then
        local youming = data:GetYouMing(protocol.p1)
        youming:SanHunLevelChange(protocol.p2 + 1, protocol.p3)
        data:GetSelectYouMing():Notify()
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_SPIRITS_LEVEL_UP then
        local youming = data:GetYouMing(protocol.p1)
        youming:QiPoLevelChange(protocol.p2 + 1, protocol.p3)
        data:GetSelectYouMing():Notify()
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_AWAKEN_LEVEL_UP then
        data:SetAwakeLevelChange(protocol)
        local youming = data:GetYouMing(protocol.p1)
        youming:AwakeLevelChange(protocol.p2)
        data:GetSelectYouMing():Notify()
        data:SetJueXingItems({})
        data:FlushJueXingSelect()
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_REFORM then
       --改造刷新
       local youming = data:GetRemodelYouMing()
       youming:Notify()
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_SAVE_OR_CANCEL_REFORM then
        -- local youming = data:GetRemodelYouMing()
        -- youming:Notify()
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_GRASP_SKILL then
        -- LogDG("领悟成功")
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.LingWuChengGong)
        data:SetSkillUse(nil)
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_GRASP_SKILL_FALIED then
        -- LogDG("领悟失败")
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.LingWuShiBai)
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_REMODELING then
        -- LogDG("重塑")
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_ADD_GROWTH then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.AddGrouthSucc)
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_SAVE_OR_CANCEL_REMODELING then
        -- LogDG("重塑保存或者取消", protocol)
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_USE_FRUIT then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.Text7)
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_USE_FRUIT_1 then
        YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.SINGLE_INFO, YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NW, protocol.p1)
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_WEAR_EQUIP_ADD_EXP then
        --对选择的冥装进行刷新
        local youming = data:GetSelectYouMing()
        if youming.index == protocol.p1 then
            local equip = youming:GetEquip(protocol.p2 + 1)
            equip:LevelChange(protocol.p3, protocol.p4)
            data:GetYouMingEquip():Notify()
            youming:Notify()
        end
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_STR_WEAR_EQUIP_UP then
        local youming = data:GetSelectYouMing()
        if youming.index == protocol.p1 then
            local equip = youming:GetEquip(protocol.p2 + 1)
            equip:StrLevelChange(protocol.p3, protocol.p4A)
            data:GetYouMingEquip():Notify()
            youming:Notify()
        end
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_TURN_TREASURE then
        data.lottery_info[protocol.p1].lucky_value = protocol.p2
        data.lottery_info:Notify()
    elseif notice_type == YouMingCfg.NOTICE_TYPE.NOTICE_TYPE_TREASURE_INFO_CHANGE then
        data.lottery_info[protocol.p1].lucky_value = protocol.p2
        data.lottery_info[protocol.p1].reward_flag = bit:And({protocol.p3},true)
        data.lottery_info:Notify()
    end
end

function YouMingCtrl:OnSCNetherWorldAttrListInfo(protocol)
    local index = protocol.index
    local youming = self.data:GetYouMing(index)
    if youming then
        youming.param.score = protocol.capaility
        youming.param.attri_list = protocol.attri_list
        self.data:SetAttrList(protocol.index, protocol.attr_list)
    end
end

function YouMingCtrl:OnSCNetherWorldTreasureList(protocol)
    self.data.lottery_result:Set(protocol.result_list)
end

function YouMingCtrl:OnSCNetherWorldBatchComposeRet(protocol)
    local youming_list = {}
    for i,v in pairs(protocol.bag_index_list) do
        local youming = self.data:GetYouMing(v)
        table.insert(youming_list, youming)
    end
    ViewMgr:OpenView(YouMingShowView, {list = youming_list})
end

function YouMingCtrl:OnSCNetherWorldPossessList(protocol)
    for i,v in pairs(protocol.list) do
        if v.pet_unique_id1 > 0 then
            local youming = self.data:GetYouMing(v.bag_index)
            local pet = PetData.Instance:GetPetInfoByUniqueId(v.pet_unique_id1, v.pet_unique_id2)
            if pet then
                youming:SetPetIndex(pet.index)
            end
            self.data:SetBlessInfo(v.bag_index, v)
        end
    end
end

function YouMingCtrl:OnSCNetherWorldSystemReformResultInfo(protocol)
    
end
function YouMingCtrl:OnSCNetherWorldSystemWearSingleInfo(protocol)
    local youming = self.data:GetYouMing(protocol.bag_index)
    if youming then
        local equip = YouMingEquip.New(protocol.equip_info)
        youming:EquipListChange(protocol.equip_grid_type, equip)
        local select_youming = self.data:GetSelectYouMing()
        if select_youming and youming.index == select_youming.index then
            select_youming:Notify()
        end
    end
end

--req_type 0:直购封印卡     p1:seq
--req_type 1:购买封印卡     p1:seq
--req_type 2:设置自动封印   p1：1开启 0关闭
function YouMingCtrl:SendNetherWorldSealReq(req_type, param1)
    local protocol = GetProtocol(CSNetherWorldSealReq)
    protocol.req_type = req_type
    protocol.param1 = param1 or 0
    SendProtocol(protocol)
end

function YouMingCtrl:OnSCNetherWorldSealInfo(protocol)
    self.data:SetCardBuyInfo(protocol)
end

function YouMingCtrl:OnSCNetherWorldSealResult(protocol)
    LOG("OnSCNetherWorldSealResult",protocol)
end

function YouMingCtrl:OnSCNetherWorldAutoSealInfo(protocol)
    self.data:SetAutoSealState(protocol)
    if self.data:IsInSealScene() then
        if protocol.is_auto_seal == 1 then
            SceneLogic.Instance:GoOnPatrol()
        else
            SceneLogic.Instance:ClearMoveCache()
            MainPlayer:StopMove()
        end
    end
end

function YouMingCtrl:OnSceneDataDisposed(scene_id, new_si)
    if not self.data:IsInSealScene(new_si) then
        if YinYangData.Instance:YinYangIsOpened() then
            YinYangCtrl.CSYinYangEyeOpReq(0)
        end
        ViewMgr:CloseView(YouMingSealView)
        self.cache_scene_id = nil
    elseif self.data:IsInSealScene(new_si) then
        ViewMgr:OpenView(YouMingSealView)
        if not self.data:IsInSealScene(scene_id) then
            self.cache_scene_id = scene_id
        end
    end
end

function YouMingCtrl:LeaveSealScene()
    local scene_id = self.cache_scene_id and self.cache_scene_id or 108
    scene_id = scene_id == 0 and 108 or scene_id
    if self.data:IsInSealScene(scene_id) then
        scene_id = 108
    end
    if scene_id ~= 108 then
        local scene_cfg = SceneData.Instance:GetSceneCfg(scene_id)
        if scene_cfg and scene_cfg.scene_type ~= SCENE_TYPE.COMMON_SCENE then
            scene_id = 108
        end
    end
    
    SceneLogic.Instance:FlyToPos(scene_id, function ()
        SceneLogic.Instance:ClearMoveCache()
        MainPlayer:StopMove()
    end)
end

function YouMingCtrl:GoToSealScene(scene_id, callback)
    local cb = function ()
        callback()
        if not YinYangData.Instance:YinYangIsOpened() then
            YinYangCtrl.CSYinYangEyeOpReq(0)
        end
        ViewMgr:OpenView(YouMingSealView)
        YouMingData.Instance.seal_smart:SetDirty("flush_scene_title")
    end
    if SceneData.Instance:SceneId() == scene_id then
        cb()
        return
    end
    SceneLogic.Instance:FlyToPos(scene_id,function ()
        cb()
    end)
end

function YouMingCtrl:onTouchBegan()
    if ViewMgr:IsOpen(YouMingSealView) and self.data.seal_smart.auto_seal then
        self:SendNetherWorldSealReq(2,0)
    end
end

function YouMingCtrl:OnSCNetherWorldCheckTreasure()
    self:SendReq(YouMingCfg.ReqType.CHECK_TREASURE)
end