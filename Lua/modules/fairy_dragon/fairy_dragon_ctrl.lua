FILE_REQ("modules/fairy_dragon/fairy_dragon_data")
FILE_REQ("modules/fairy_dragon/fairy_dragon_cfg")
FILE_REQ("modules/fairy_dragon/fairy_dragon")
FILE_REQ("modules/fairy_dragon/fairy_dragon_soul_data")

VIEW_REQ("modules/fairy_dragon/fairy_dragon_view")
VIEW_REQ("modules/fairy_dragon/fairy_dragon_foster_panel")
VIEW_REQ("modules/fairy_dragon/fairy_dragon_star_panel")
VIEW_REQ("modules/fairy_dragon/fairy_dragon_skin_panel")
VIEW_REQ("modules/fairy_dragon/fairy_dragon_soul_panel")
VIEW_REQ("modules/fairy_dragon/fairy_dragon_pray_panel")
VIEW_REQ("modules/fairy_dragon/fairy_dragon_model_panel")
VIEW_REQ("modules/fairy_dragon/fairy_dragon_up_tip")
VIEW_REQ("modules/fairy_dragon/fairy_dragon_soul_item_info")
VIEW_REQ("modules/fairy_dragon/fairy_dragon_soul_bag_view")
--VIEW_REQ("modules/fairy_dragon/fairy_dragon_soul_up_view")
VIEW_REQ("modules/fairy_dragon/fairy_dragon_soul_new_up")

-- 开启系统道具：64556
-- 龙魂道具id: 63100
-- 龙魂经验 56310 - 56315
-- 龙魄 56317 - 56319

FairyDragonCtrl = FairyDragonCtrl or BaseClass(BaseCtrl)

function FairyDragonCtrl:__init()
	if FairyDragonCtrl.Instance ~= nil then
		Debuger.LogError("[FairyDragonCtrl] attempt to create singleton twice!")
		return
	end
	FairyDragonCtrl.Instance = self
    FairyDragonData.New()
    FairyDragonSoulData.New()
    self.data = FairyDragonData.Instance

    ItemInfoData.Instance:RegisterBtnRemind(ItemInfoFunc.DragonSoulRep2.name, function(info)
        local item = info.item_list[1]
        local num = self.data:RemindSoulRep(item)
        return num
    end)

    RemindCtrl.Instance:Remind():Register(
        Mod.FairyDragon.Foster,
        FairyDragon.Instance.base_data,
        function ()
            return self.data:RemindFosterTab()
        end
    )

    RemindCtrl.Instance:Remind():Register(
        Mod.FairyDragon.Star,
        FairyDragon.Instance.base_data,
        function ()
            return self.data:RemindStarTab()
        end
    )

    RemindCtrl.Instance:Remind():Register(
        Mod.FairyDragon.Skin,
        FairyDragon.Instance.base_data,
        function ()
            return self.data:RemindSkinTab()
        end
    )

    RemindCtrl.Instance:Remind():Register(
        Mod.FairyDragon.Soul,
        FairyDragon.Instance.base_data,
        function ()
            return self.data:RemindSoulTab()
        end
    )

    RemindCtrl.Instance:Remind():Register(
        Mod.FairyDragon.Pray,
        FairyDragon.Instance.base_data,
        function ()
            return self.data:RemindPrayTab()
        end
    )

    BagData.Instance:ListenItems(self.data:GetRemindItemList(),function ()
        self.data:RemindNotice()
    end)

    self:RegisterAllProtocols()

    FunOpen.Instance:SelfCondition(Mod.FairyDragon,function()
        return self.data:IsOpen()
    end)
    FunOpen.Instance:SelfCondition(Mod.FairyDragon.Main,function()
        return self.data:IsOpen()
    end)
end

function FairyDragonCtrl:OnInit()
    -- 角色登录追加协议
    -- EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,function ()     
    -- end)
end

function FairyDragonCtrl:__delete()
    FairyDragonCtrl.Instance = nil
end

function FairyDragonCtrl:OnUnloadGameLogic()
    self.data:ClearData()
    FairyDragonSoulData.Instance:ClearData()
end

function FairyDragonCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSGodDragonSystemReq)
    self:RegisterProtocol(CSGodDragonSystemUseSoulItemReq)  -- 龙魂使用经验道具升级 >>>>> 这个废弃了
    self:RegisterProtocol(CSGodDragonSystemSoulDecomposeReq)
    self:RegisterProtocol(CSGodDragonSystemSoulLevelUpSeq)

    self:RegisterProtocol(SCGodDragonSystemBaseInfo, "OnSCGodDragonSystemBaseInfo")
    self:RegisterProtocol(SCGodDragonSoulBag, "OnSCGodDragonSoulBag")
    self:RegisterProtocol(SCGodDragonSoul, "OnSCGodDragonSoul")
    self:RegisterProtocol(SCGodDragonSystemNoitce, "OnSCGodDragonSystemNoitce")
    self:RegisterProtocol(SCGodDragonSystemPrayResultInfo, "OnSCGodDragonSystemPrayResultInfo")
end

-- 神龙系统请求
-- req_type == FairyDragonCfg.REQ_TYPE
function FairyDragonCtrl:SendReq(req_type, p1, p2)
    local protocol = GetProtocol(CSGodDragonSystemReq)
    protocol.req_type = req_type
    protocol.p1 = p1 or 0
    protocol.p2 = p2 or 0
    SendProtocol(protocol)
end

--//!< 5485 神龙系统使用龙魂经验道具请求
function FairyDragonCtrl:SendUpSoulReq()
    local protocol = GetProtocol(CSGodDragonSystemUseSoulItemReq)
    local soul_item = FairyDragonSoulData.Instance:GetUpSoul()
    if TableIsEmpty(soul_item) then
        return
    end
    protocol.bag_index = soul_item.index
    local list = {}
    local stuff_list = FairyDragonSoulData.Instance:GetSelStuff()
    for i,v in pairs(stuff_list) do
        table.insert(list, {item_id = i, num = v})
    end
    protocol.stuff_list = list
    SendProtocol(protocol)
end

--//!< 5486 神龙系统龙魂分解请求
function FairyDragonCtrl:SendResolveSoulReq()
    local protocol = GetProtocol(CSGodDragonSystemSoulDecomposeReq)
    local list = FairyDragonSoulData.Instance.soul_resolve_sel_list or {}
    protocol.list = {}
    for i,v in pairs(list) do
        if v then
            table.insert(protocol.list, i)
        end
    end
    SendProtocol(protocol)
end

--5488 龙魂升级请求
function FairyDragonCtrl:SendSoulLevelUpReq(up_index)
    local protocol = GetProtocol(CSGodDragonSystemSoulLevelUpSeq)
    local list = FairyDragonSoulData.Instance.sel_stuff_soul or {}
    protocol.up_index = up_index
    protocol.list = {}
    for i,v in pairs(list) do
        if v and v > 0 then
            table.insert(protocol.list, i.index)
        end
    end
    SendProtocol(protocol)
end


--//!< 5484 神龙系统基础信息
function FairyDragonCtrl:OnSCGodDragonSystemBaseInfo(protocol)
    local base_data = protocol.info
    FairyDragon.Instance:SetBaseData(base_data)
end

-- 龙魂背包
function FairyDragonCtrl:OnSCGodDragonSoulBag(protocol)
    FairyDragonSoulData.Instance:SetItemList(protocol.item_list)
end

-- 龙魂单条下发
function FairyDragonCtrl:OnSCGodDragonSoul(protocol)
    FairyDragonSoulData.Instance:ItemChange(protocol)
end

--//!< 5483 神龙系统相关通知下发
function FairyDragonCtrl:OnSCGodDragonSystemNoitce(protocol)
    self.data:HandleNotice(protocol)
end

--//!< 5487 神龙系统祈愿结果信息下发
function FairyDragonCtrl:OnSCGodDragonSystemPrayResultInfo(protocol)
    --ViewMgr:OpenView(FairyDragonPrayResultView, {pray_list = protocol.pray_list})
    self.data.pray_result_info:Set(protocol.pray_list)
end