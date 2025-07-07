FavorNpcBookView = FavorNpcBookView or DeclareView("FavorNpcBookView", "favor/favor_npc_book")
VIEW_DECLARE_MASK(FavorNpcBookView, ViewMask.BgBlockClose)
VIEW_DECLARE_LEVEL(FavorNpcBookView, ViewLevel.Lt)

function FavorNpcBookView:FavorNpcBookView()
end

function FavorNpcBookView:LoadCallback()
end

FavorNpcBookPanel = FavorNpcBookPanel or DeclareMono("FavorNpcBookPanel", UIWFlushPanel)

function FavorNpcBookPanel:FavorNpcBookPanel()
    self.data = FavorData.Instance
    self.Board:SetData(FavorNpcBookView, Language.Favor.Book)
    self.sel_zone_info = nil
    self.sel_npc_info = nil
    self:InitData()
end


function FavorNpcBookPanel:InitData()
    self.data_cares = {{data = self.data.favor_data_change, func = self.FlushData, init_call = false, keys = {"val"}}}
    self:FlushData()
    -- self.ListZone:ClickItemData(self.data:GetSelectZone(self.zone_list))
    -- self.ListNpc:ClickItemData(self.data:GetSelectNpc(self.sel_zone_info.npc_list))
    -- self.ListZone:ClickItem(1)
    -- self.ListNpc:ClickItem(1)
end

function FavorNpcBookPanel:FlushData()
    UH.SetText(self.TiLI,  self.data:ActionTimes())
    self.zone_list=self.data:GetNpcZoneList()
    if #self.zone_list == 0 then
        return
    end
    self.ListZone:SetData(self.zone_list)
    self:FlushZoneNpcList()
end

--区域npc列表
function FavorNpcBookPanel:FlushZoneNpcList()
    if self.zone_list then
        if self.sel_zone_info == nil then
            local sel_zone_index = self.data:GetSelectZone(self.zone_list)
            self.sel_zone_info = self.zone_list[sel_zone_index]
            self.ListZone:ClickItemData(self.sel_zone_info)
            self.zone_timer = TimeHelper:AddDelayFrameTimer(function()
                self.ListZone:JumpVerticalIndex(sel_zone_index, 117, 0)
            end, 5)
        end
        local npc_list = self.sel_zone_info.npc_list
        self.ListNpc:SetData(npc_list)
        if self.sel_npc_info == nil then
            local sel_npc_index = self.data:GetSelectNpc(self.sel_zone_info.npc_list)
            self.sel_npc_info = self.sel_zone_info.npc_list[sel_npc_index]
            self.ListNpc:ClickItemData(self.sel_npc_info)
            self.npc_timer = TimeHelper:AddDelayFrameTimer(function()
                self.ListNpc:JumpHorizontalIndex(sel_npc_index, -170, 0)
            end, 5)
        end
        self:FlushZoneData()
        self:FlushNpcData()
    end
end

--区域信息
function FavorNpcBookPanel:FlushZoneData()
    local zone_info=self.sel_zone_info
    local pro = "( "..zone_info.zhiyou_num.."/"..#zone_info.npc_list.." )"
    UH.SetText(self.ZoneRewardPro,pro)
    self.RedPos:SetNum(self.data:GetNpcZoneRed(zone_info.cfg_zone_reward.seq))
end

--npc信息
function FavorNpcBookPanel:FlushNpcData()
    if self.sel_npc_info then
        local npc_info = self.sel_npc_info
        self.banquet_list = self.data:GetNpcBanquet(npc_info.cfg)
        UH.SetText(self.FeastName1, self.banquet_list[1].feast_name)
        UH.SetText(self.FeastName2, self.banquet_list[2].feast_name)
        UH.SetText(self.FavorDesc, npc_info.cfg.like_desc)
        local npc_id = NpcHelper.NpcSeqToId(npc_info.cfg.npc_seq)
        local npc_config=NpcHelper.NpcCo(npc_id)
        UH.SetText(self.NpcName, npc_config.name)
        local progress_data = FavorData.Instance:GetNpcFavorLevelPro(npc_info.cfg)
        local val = progress_data.val
        local target = progress_data.next_target
        local level = progress_data.level
        self.ProGress:SetProgress(val / target)
        self.ProGress:SetText(string.format("%s/%s", val, target))
        UH.SetText(self.FavorTitle, self.data:GetNpcFavorTitle(level))
        UH.SetText(self.FavorLevel, level)
        self.is_fetched = self.data:IsNpcLevelFectch(npc_info.cfg)
        self.ObjFetched:SetActive(self.is_fetched)
        self:FlushModelShow(npc_config.res_id)
    end
end

-- 模型展示
function FavorNpcBookPanel:FlushModelShow(res_id)
    local res_path = DrawerHelper.GetNpcPath(res_id)
    local uo = self.ModelShow:UiObj()
    if uo == nil then
        uo = UIObjDrawer.CreateByPath(res_path)
        self.ModelShow:SetShowData({ui_obj = uo, view = FavorNpcBookView})
    else
        uo:SetData(res_path)
    end
end

function FavorNpcBookPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.zone_timer ~= nil then
        TimeHelper:CancelTimer(self.zone_timer)
        self.zone_timer = nil
    end
    if self.npc_timer ~= nil then
        TimeHelper:CancelTimer(self.npc_timer)
        self.npc_timer = nil
    end
end

function FavorNpcBookPanel:OnChnageNpcZone(data)
    if self.zone_list then
        self.sel_zone_info = data
        self.RedPos:SetNum(self.data:GetNpcZoneRed(self.sel_zone_info.cfg_zone_reward.seq))
        local npc_list = self.sel_zone_info.npc_list
        self.ListNpc:SetData(npc_list)
        self.ListNpc:ClickItem(1)
        self.sel_npc_info = npc_list[1]
        self:FlushZoneData()
        self:FlushNpcData()
    end
end

function FavorNpcBookPanel:OnChnageNpc(data)
    if self.zone_list then
        self.sel_npc_info = data
        self:FlushNpcData()
    end
end

function FavorNpcBookPanel:OnClickZhiYouReward()
    if self.sel_zone_info then
        ViewMgr:OpenView(FavorNpcZoneRewardView,{zone_info=self.sel_zone_info})
    end
end

function FavorNpcBookPanel:OnClickFeast(type)
    if self.banquet_list then
        local cfg = self.banquet_list[type]
        ViewMgr:OpenView(FavorNpcFeastView, {cfg_feast = cfg})
    end
end

function FavorNpcBookPanel:OnClickReward()
    if self.sel_npc_info then
        local npc_info = self.sel_npc_info
        -- if self.data:IsCanNpcFavorReward(npc_info.cfg) then
        --     local progress_data = FavorData.Instance:GetNpcFavorLevelPro(npc_info.cfg)
        --     FavorCtrl.Instance:SendFavorReq(FAVOR_NPC_OP_TYPE.FETCH_FAVORE_REWARD, npc_info.cfg.seq, progress_data.level - 1)
        -- else
            ViewMgr:OpenView(FavorNpcRewardView, {cfg_npc = npc_info.cfg})
        -- end
    end
end

function FavorNpcBookPanel:OnGoToNpc()
    if self.sel_npc_info then
        local npc_Seq=self.sel_npc_info.cfg.npc_seq
        local function func_call()
            SceneLogic.Instance:AutoToNpc(npc_Seq)
        end
        if SceneLogic.Instance:SceneID() ~= self.sel_npc_info.cfg.npc_sence then
            SceneLogic.Instance:AutoToPos(self.sel_npc_info.cfg.npc_sence, nil, func_call, nil, true)
        else
            func_call()
        end
        ViewMgr:CloseView(FavorNpcBookView)
    end
end


function FavorNpcBookPanel:OnTipTili()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[411].desc)
end

function FavorNpcBookPanel:OnTip()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[410].desc)
end


NpcZoneItem = NpcZoneItem or DeclareMono("NpcZoneItem", UIWidgetBaseItem)

function NpcZoneItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local name=MapData.Instance:GetMapShowData(data.cfg_zone_reward.sence_id).name
    UH.SetText(self.Name,name)
    UH.SetText(self.Num,Language.Favor.ZhiYou..data.zhiyou_num.."/"..#data.npc_list)
    local map_id=MapData.MapIdToIcon(data.cfg_zone_reward.sence_id)
    UH.SetIcon(self.MapIcon,map_id, ICON_TYPE.MAP)
    self.RedPos:SetNum(FavorData.Instance:GetNpcZoneRed(data.cfg_zone_reward.seq))
end

function NpcZoneItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

FavorNpcItem = FavorNpcItem or DeclareMono("FavorNpcItem", UIWidgetBaseItem)

function FavorNpcItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local progress_data = FavorData.Instance:GetNpcProGress(data.cfg, data.level)
    local val = progress_data.val
    local target = progress_data.target
    self.ProGress:SetProgress(val / target)
    UH.SetText(self.FavorLevel, data.level)
    local npc_id = NpcHelper.NpcSeqToId(data.cfg.npc_seq)
    local npc_info = NpcHelper.NpcCo(npc_id)
    UH.SetText(self.Name, npc_info.name)
    UH.SetIcon(self.Head, npc_info.npc_icon)
end

function FavorNpcItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end







 


