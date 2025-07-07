FILE_REQ("modules/sheng_qi/sheng_qi_data")
FILE_REQ("modules/sheng_qi/fu_shi_data")
FILE_REQ("modules/sheng_qi/sheng_qi_cfg")
FILE_REQ("modules/sheng_qi/sheng_qi")

VIEW_REQ("modules/sheng_qi/sheng_qi_view")
VIEW_REQ("modules/sheng_qi/sheng_qi_bag_view")
VIEW_REQ("modules/sheng_qi/fu_shi_reset_view")
VIEW_REQ("modules/sheng_qi/fu_shi_fusion_view")
VIEW_REQ("modules/sheng_qi/sheng_qi_skill_pool_view")
VIEW_REQ("modules/sheng_qi/sheng_qi_skill_study_view")
VIEW_REQ("modules/sheng_qi/sheng_qi_skill_tip")
VIEW_REQ("modules/sheng_qi/sheng_qi_item_info")
VIEW_REQ("modules/sheng_qi/tab_panel/sheng_qi_book_panel")
VIEW_REQ("modules/sheng_qi/tab_panel/sheng_qi_skill_panel")
VIEW_REQ("modules/sheng_qi/tab_panel/sheng_qi_attri_panel")
VIEW_REQ("modules/sheng_qi/tab_panel/sheng_qi_fu_shi_panel")
VIEW_REQ("modules/sheng_qi/tab_panel/sheng_qi_up_panel")
VIEW_REQ("modules/sheng_qi/sheng_qi_score")
VIEW_REQ("modules/sheng_qi/fu_shi_book_view")
VIEW_REQ("modules/sheng_qi/sheng_qi_skill_info_view")
VIEW_REQ("modules/sheng_qi/sheng_qi_skill_update")
VIEW_REQ("modules/sheng_qi/sheng_qi_book_change")
VIEW_REQ("modules/sheng_qi/sheng_qi_treasure_map")
VIEW_REQ("modules/sheng_qi/sheng_qi_treasure_map_roller")
VIEW_REQ("modules/sheng_qi/sheng_qi_detail_view")

-- gm命令
-- clearholybag: op_type
-- 0--全部清掉, 1--清掉圣器背包, 2--清掉符石背包
-- 材料id 26762 ~ 26765
-- 圣器id :56000
-- 符石id :56100

ShengQiCtrl = ShengQiCtrl or BaseClass(BaseCtrl)

ShengQiCtrl.OpType = {
    OP_TYPE_INFO_REQ = 0,		    -- //!< 请求信息
    OP_TYPE_PUT_ON = 1,			    -- //!< 更改圣器状态 param1 背包格子序号 param2 修改到的状态 HOLY_EQUIP_STATUS
    OP_TYPE_UNLOCK_HOLE = 2,	    -- //!< 解锁主动技能孔位 param1 要解锁的圣器index, param2 要解锁的孔位
    OP_TYPE_STUDY_NEW_SKILL = 3,    -- //!< 领悟新的主动技能 param1 要领悟的圣器格子序号, param2 道具id
    OP_TYPE_SAVE_NEW_SKILL = 4,	    -- //!< 保存洗练出来的新技能 param1 要保存的圣器格子序号, param2 1保存，2放弃

    OP_TYPE_INLAY_FU_SHI = 5,	    -- //!< 镶嵌符石 param1 要镶嵌的圣器格子序号, param2 要镶嵌的圣器的孔位, param3 要镶嵌的符石的格子序号
    OP_TYPE_TAKE_OFF_FU_SHI = 6,    -- //!< 取下符石 param1 要取下符石的圣器格子序号, param2 要取下符石的圣器的符石孔位
    OP_TYPE_FU_SHI_FUSION = 7,	    -- //!< 合成符石 param1 目标等级 param2 符石index1, param3 符石index2, param4 符石index3, param5 符石index4, param6 符石index5
    OP_TYPE_FU_SHI_RECAST = 8,	    -- //!< 重铸符石 param1 要重铸的符石index, param2 重铸消耗的符石index
    OP_TYPE_UNLOCK_FUSHI_HOLE = 9,  --//!< 解锁符石孔 param1 圣器index param2 符石孔位0-4
    OP_TYPE_FU_SHI_RECAST_BY_HOLY_EQUIP = 10,  --//!< 重铸圣器上的符石, param1 圣器index, param2 要重铸的符石孔位, param3 重铸消耗的符石index
    OP_TYPE_BLESS_PET = 11,         -- 圣佑宠物 p1:圣器index, p2:宠物index
    OP_TYPE_FOLLOW = 12,            -- 圣器跟随 p1 圣器index
    OP_TYPE_SKILL_CHANGE = 13,      -- 圣器技能书置换， p1技能书物品id,p2技能书物品id
    OP_TYPE_FU_SHI_UPLEVEL = 14,    -- 圣器上的符石升级 p1圣器index, p2 圣器孔位index 材料不够会自动购买
}

function ShengQiCtrl:__init()
	if ShengQiCtrl.Instance ~= nil then
		Debuger.LogError("[ShengQiCtrl] attempt to create singleton twice!")
		return
	end
	ShengQiCtrl.Instance = self
    ShengQiData.New()
    FuShiData.New()
    self.data = ShengQiData.Instance
    self.fu_shi_data = FuShiData.Instance

    RemindCtrl.Instance:Remind():Register(
        Mod.ShengQi.Up,
        self.data.remind_data,
        function ()
            return self.data:UpLevelTabRemind()
        end
    )

    RemindCtrl.Instance:Remind():Register(
        Mod.ShengQi.FuShi,
        self.data.remind_data,
        function ()
            return self.data:FuShiTabRemind()
        end
    )

    RemindCtrl.Instance:Remind():Register(
        Mod.ShengQi.Attri,
        self.data.remind_data,
        function ()
            return self.data:ShengQiUseRemind()
        end
    )

    BagData.Instance:ListenItems(self.data:AllStuffItemIdList(),function ()
        self.data:RemindNotice()
    end)

    self.data.sheng_qi_list:Care(function ()
        self.data:RemindNotice()
    end)
    FuShiData.Instance.fu_shi_list:Care(function ()
        self.data:RemindNotice()
    end)
end

function ShengQiCtrl:__delete()
    self:UnRegisterAllProtocols()
    ShengQiCtrl.Instance = nil
end

function ShengQiCtrl.Log(tab,...)
    if ShengQiCfg.IS_OPEN_LOG == true and DEBUG_MODE then
        LOG(tab,...)
    end
end

function ShengQiCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function ShengQiCtrl:OnInit()
    self:RegisterAllProtocols()
end

function ShengQiCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSHolyEquipOpReq)         -- //!<4830 圣器客户端操作请求
    self:RegisterProtocol(CSHolyEquipLevelUpReq)    -- //!<4831 圣器升级请求
    self:RegisterProtocol(SCHolyBagListInfo, "OnSCHolyBagListInfo")         -- //!<4832 圣器背包全部信息下发
    self:RegisterProtocol(SCFuShiBagListInfo, "OnSCFuShiBagListInfo")       -- //!<4833 符石背包全部信息下发
    self:RegisterProtocol(SCHolyBagItemChange, "OnSCHolyBagItemChange")     -- //!<4834 圣器/符石背包单个信息下发
    self:RegisterProtocol(SCSCHolyEquipLevelUpConsumeInfo, "OnSCSCHolyEquipLevelUpConsumeInfo") -- //!<4835 圣器 圣器圣器/符石合成消耗列表下发
    self:RegisterProtocol(SCShengQiFirstNotice, "OnSCShengQiFirstNotice")
    self:RegisterProtocol(SCShengQiFollowInfo, "OnSCShengQiFollowInfo")
      
end

-- //!<4830 圣器客户端操作请求
function ShengQiCtrl:SendCSHolyEquipOpReq(op_type,p1,p2,p3,p4,p5,p6)
    local protocol = GetProtocol(CSHolyEquipOpReq)
    protocol.op_type = op_type
    protocol.p1 = p1 or 0
    protocol.p2 = p2 or 0
    protocol.p3 = p3 or 0
    protocol.p4 = p4 or 0
    protocol.p5 = p5 or 0
    protocol.p6 = p6 or 0
    SendProtocol(protocol)
end

-- //!<4831 圣器升级请求
function ShengQiCtrl:SendCSHolyEquipLevelUpReq(info)
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    info = info or self.data:GetAllSelStuff()
    local protocol = GetProtocol(CSHolyEquipLevelUpReq)
    protocol.sheng_qi_index = sheng_qi.index
    protocol.info = info
    SendProtocol(protocol)
    self.data:ClearSelStuff()
end

-- //!<4832 圣器背包全部信息下发
function ShengQiCtrl:OnSCHolyBagListInfo(protocol)
    self.data.sheng_qi_list:Set(protocol.sheng_qi_list)
end

-- //!<4833 符石背包全部信息下发
function ShengQiCtrl:OnSCFuShiBagListInfo(protocol)
    self.fu_shi_data.fu_shi_list:Set(protocol.fu_shi_list)
end

-- //!<4834 圣器/符石背包单个信息下发
function ShengQiCtrl:OnSCHolyBagItemChange(protocol)
    self.data:ItemChange(protocol)
end

-- //!<4835 圣器 圣器圣器/符石合成消耗列表下发
function ShengQiCtrl:OnSCSCHolyEquipLevelUpConsumeInfo(protocol)
    local bag_type = protocol.bag_type
    local index_list = protocol.index_list or {}
    for i,index in pairs(index_list) do
        if bag_type == ShengQiCfg.BagType.ShengQi then
            self.data:RemoveShengQi(index)
        elseif bag_type == ShengQiCfg.BagType.FuShi then
            self.fu_shi_data:RemoveFuShi(index)
        end
    end
end

function ShengQiCtrl:OnSCShengQiFirstNotice(protocol)
    if protocol.first_flag ~= 1 then
        return
    end
    --self:OpenGuide()
end

function ShengQiCtrl:OpenGuide()
    local guide_id = Config.shengqi_auto.other[1].guide_id
    if guide_id then
        GuideManager.Instance:Start(guide_id)
    end
end

-- 圣器跟随设置
function ShengQiCtrl:OnSCShengQiFollowInfo(protocol)
    self.data.follow_data.index = protocol.follow_index
    self:FlushShengQiFollow()
end

function ShengQiCtrl:FlushShengQiFollow()
    self.data:FlushFollow()
end


-- 设置圣器状态
function ShengQiCtrl:SetShengQiState(sheng_qi, state)
    self:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_PUT_ON,sheng_qi.index, state)
end

-- 镶嵌符石
function ShengQiCtrl:UseFuShi(item,sheng_qi)
    sheng_qi = sheng_qi or self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    local hole_index = sheng_qi:GetFuShiInsertIndex(item)
    if hole_index >= 0 and hole_index <= 4 then
        self:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_INLAY_FU_SHI,sheng_qi.index, hole_index,item.index)
    elseif hole_index == -1 then
        PublicPopupCtrl.Instance:Center(Language.ShengQi.Tip18)
    elseif hole_index == -2 then
        PublicPopupCtrl.Instance:Center(Language.ShengQi.Tip19)
    end
end

-- 卸下符石
function ShengQiCtrl:UnfixFuShi(item)
    local sheng_qi = item:ShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    self:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_TAKE_OFF_FU_SHI,sheng_qi.index, item.hole_index)
end

-- 重铸符石
function ShengQiCtrl:ResetFuShi(item1,item2)
    local sheng_qi = item1:ShengQi()
    if TableIsEmpty(sheng_qi) then
        self:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_FU_SHI_RECAST,item1.index, item2.index)
    else
        self:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_FU_SHI_RECAST_BY_HOLY_EQUIP,sheng_qi.index,item1.hole_index, item2.index)
    end
end

-- 升级符石，只能升级已经在圣器上的
function ShengQiCtrl:UpLevelFushi(item)
    if item:IsMax() then
        return
    end
    local shengqi = item:ShengQi()
    if not shengqi then
        return
    end
    local hole_index = item.hole_index
    local need_count = item:GetUpNeedNormalNum()
    local has_count = 0
    local list = FuShiData.Instance.fu_shi_list
    for i,v in pairs(list) do
        if v:Level() <= item:Level() then
            has_count = has_count + v:GetPriceStuffNum()
        end
    end
    if has_count >= need_count then
        self:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_FU_SHI_UPLEVEL, shengqi.index, hole_index)
    else
        local normal_item_id = item:NormalItemId()
        local price = MarketData.Instance:GetItemPrice(normal_item_id) * (need_count - has_count)
        local next_item_id = item:NextItemId() 
        local tip = string.format(Language.ShengQi.Text24,price, need_count-has_count,Item.GetQuaName(normal_item_id),Item.GetQuaName(next_item_id))
        local param = {
            item = item,
            context = tip,
            ok_func = function()
                ViewMgr:CloseView(FuShiQuickBuy)
                if not MallCtrl.IsNotCurrency(CurrencyType.ImmortalCoin,price) then
                    PublicPopupCtrl.Instance:Center(Language.Common.ImmortalCoinLack)
                    MallData.JumpItemNum = MarketData.Instance:GetNeedGoldNum(price)
                else
                    self:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_FU_SHI_UPLEVEL, shengqi.index, hole_index)
                end
            end,
        }
        ViewMgr:OpenView(FuShiQuickBuy, param)
    end
end


function ShengQiCtrl.ClickShengQi(data)
	if TableIsEmpty(data) then
		return
	end
	local info = {
		item_list = {data},
		func = function ()
			return { ItemInfoFunc.ShengQiFight, ItemInfoFunc.ShengQiReady}
		end
		}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function ShengQiCtrl.ClickFuShi(data)
	if TableIsEmpty(data) then
		return
    end
    local data_list = {data}
    local is_rep = false
    local sheng_qi = ShengQiData.Instance:GetSelShengQi()
    if not TableIsEmpty(sheng_qi) then
        for i = 0, 4 do
            local cur_fushi = sheng_qi:GetFuShi(i)
            if cur_fushi.item_id ~= 0 and cur_fushi:FuShiType() == data:FuShiType() then
                table.insert(data_list, cur_fushi)
                is_rep = true
                break
            end
        end
    end
	local info = {
        func = function ()
            local func_list = {}
            if is_rep then
                table.insert(func_list,ItemInfoFunc.FuShiRep)
            else
                table.insert(func_list,ItemInfoFunc.FuShiUse)
            end
            if data:Level() > 1 then
                table.insert(func_list,ItemInfoFunc.FuShiReset)
            end
            table.insert(func_list,ItemInfoFunc.FuShiFusion)

            if data:Price() ~= 0 then
                table.insert(func_list, ItemInfoFunc.Sell)
            end

			return func_list
        end,
        item_list = data_list,
    }
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

-- 获取圣器跳转
function ShengQiCtrl.GetShengQiJump()
    --TreasureHuntCtrl.Instance:OpenTreasureShengQi()
    if ShengQiData.Instance:IsOpen() then
        FightData.Instance:FightFBEliteDungeonOpenSel(67)
    end
    ViewMgr:OpenViewByKey(Mod.Challenge.FightElite)
    --ViewMgr:CloseView(ShengQiView)
end