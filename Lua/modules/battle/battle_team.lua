BattleTeam = BattleTeam or DeclareView("BattleTeam", "battle/battle_team",0,ViewLayer.Normal + 1)
VIEW_DECLARE_MASK(BattleTeam, ViewMask.BlockClose)
function BattleTeam:BattleTeam()
    -- body
end
function BattleTeam:CloseCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ToggleIn)
end
BattleTeamPanel = BattleTeamPanel or DeclareMono("BattleTeamPanel", UIWFlushPanel)
function BattleTeamPanel:BattleTeamPanel()
    --监听人员变化
    self.battle_data = BattleData.Instance
    self.data_cares = {
        {data = self.battle_data:ChInfo(), func = self.FlushList, init_call = true}
    }
end
function BattleTeamPanel:FlushList()
    local player_idx = self.battle_data:PlayerIdx()
    local pet_idx = self.battle_data:GetPetIdxByPlayerIdx(player_idx)
    local ch_list = self.battle_data:ChInfo()
    local list1,list2 = {is_other = false,list = {}},{is_other = true, list = {}}
    -- 这里只显示本队伍也就是
    for key, value in pairs(ch_list) do
        -- if key ~= player_idx then
            if key == player_idx or key == pet_idx then
                table.insert(list1.list, value)
            else
                if not BattleData.IsEnemyIdx(key) then
                    if value.character_type == 3 and (not TeamData.Instance:InTeam() or TeamData.Instance:IsLeader()) then
                        table.insert(list1.list, value)
                    else
                        table.insert(list2.list, value)
                    end
                end
            end
        --end
    end
    table.sort(list1.list, DataSorter.KeyLowerSorter("character_type"))
    table.sort(list2.list, DataSorter.KeyLowerSorter("character_type"))
    -- LogError(list2)
    -- 判断获取宠物和伙伴
    -- 剩下的就是其他队伍
    if next(list2.list) == nil then
        self.InfoList:SetData({list1})
    else
        self.InfoList:SetData({list1,list2})
    end
end



BattleTeamItem = BattleTeamItem or DeclareMono("BattleTeamItem", UIWidgetBaseItem)
function BattleTeamItem:BattleTeamItem()
end
function BattleTeamItem:SetData(data)
    self.List:SetData(data.list)
    self.Other:SetActive(data.is_other)
end

BattleTeamItem2 = BattleTeamItem2 or DeclareMono("BattleTeamItem2", UIWidgetBaseItem)

function BattleTeamItem2:BattleTeamItem2()
    self.battle_data = BattleData.Instance
end
function BattleTeamItem2:SetData(data)
    self:uncareData()
    UIWidgetBaseItem.SetData(self, data)
    if self.data.spirit_point > -1 then
        self.SP:SetProgress(self.data.spirit_point/FightData.FightOtherDate().max_sp)
        self.SP:SetText(self.data.spirit_point.."/"..FightData.FightOtherDate().max_sp)
        self.RootRect.sizeDelta = Vector2.New(355.52, 118)
        self.SP:SetObjActive(true)
    else
        self.RootRect.sizeDelta = Vector2.New(355.52, 93)
        self.SP:SetObjActive(false)
    end

    if SmartData.Check(data) then
        self:careData(data, BindTool.Bind(self.FlushHPMP, self), "maxhp", "curhp", "maxmp", "curmp")
    end
    UH.SetText(self.Name, data.name)
    if data.character_type == 0 then
        if data.grid_idx == self.battle_data:PlayerIdx() then
            UH.SetText(self.Type, Language.BattleTeam.CharaType[-1][2])
            UH.SpriteName(self.TypeSp,Language.BattleTeam.CharaType[-1][1])
        else
            UH.SetText(self.Type, Language.BattleTeam.CharaType[data.character_type][2])
            UH.SpriteName(self.TypeSp,Language.BattleTeam.CharaType[data.character_type][1])
        end
    else
        UH.SetText(self.Type, Language.BattleTeam.CharaType[data.character_type][2])
        UH.SpriteName(self.TypeSp,Language.BattleTeam.CharaType[data.character_type][1])
    end

    if data.prof > 0 then
        local prof = data.prof > 100 and RoleData.ProfIdToProfType(data.prof) or data.prof
        UH.SpriteName(self.ProfSp, ProfSettingB[prof])
        self.ProfSp.gameObject:SetActive(true)
    end
    local iType = ICON_TYPE.ITEM
    local icon_id = data.character_id
    if data.character_type == BATTLE_CH_TYPE.PARTNER or data.character_type == BATTLE_CH_TYPE.PARTNER_ROBOT then
       local config = PartnerData.Instance:GetPartnerInfoById(tonumber(icon_id))
       icon_id = config.info.icon_id
        if data.skin_id ~= nil and data.skin_id ~= 0 then
            local co = Cfg.PartnerSkinById(data.skin_id)
            icon_id = co and co.head_portrait or icon_id
        end
       PartnerInfo.SetQuality(self.PartnerQua, config.info.partner_quality)
    elseif data.character_type == BATTLE_CH_TYPE.ROLE or data.character_type == BATTLE_CH_TYPE.ROLE_SHADOW then
        PartnerInfo.SetQuality(self.PartnerQua, 0)
        if data.appearance.special_appearance_type == 1 then
			local config = PartnerData.Instance:GetPartnerInfoById(data.appearance.special_appearance_param)
			UH.SetIcon(self.Icon, config:IconId())
		else
			icon_id = data.appearance.avatar_type
            UH.SetAvatar(self.Icon, icon_id,data.appearance.avatar_id,data.appearance.avatar_quality)
		end
        return
    elseif data.character_type == BATTLE_CH_TYPE.MONSTER then
        local config = Cfg.MonsterById(icon_id)
        icon_id = config.icon_id
    elseif data.character_type == BATTLE_CH_TYPE.NPC_HELPER then
        local config = Config.npc_group_auto.npc_group[icon_id]
        icon_id = config.icon_id
    elseif data.character_type == BATTLE_CH_TYPE.PET or data.character_type == BATTLE_CH_TYPE.PET_SHADOW then
        local pet_cfg = PetData.Instance:GetPetCfg(data.character_id)    
        if data.skin_id ~= nil and data.skin_id ~= 0 then
            local co = Cfg.PetSkinAttr(data.skin_id)
            if co and co.head_portrait then
                icon_id = co.head_portrait
            end
        elseif pet_cfg ~= nil then
            icon_id = pet_cfg.icon_id
        end
    end
    UH.SetIcon(self.Icon, icon_id, iType)
end
function BattleTeamItem2:FlushHPMP()
    self.MP:SetProgress(self.data.curmp/self.data.maxmp)
    self.HP:SetProgress(self.data.curhp/self.data.maxhp)
    self.MP:SetText(self.data.curmp.."/"..self.data.maxmp)
    self.HP:SetText(self.data.curhp.."/"..self.data.maxhp)
    self.Die:SetActive(self.data.curhp == 0)
end
function BattleTeamItem2:OnDestroy()
    self:uncareData()
end