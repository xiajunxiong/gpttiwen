AdvancedFBMenuView = AdvancedFBMenuView or DeclareMono("AdvancedFBMenuView", UIWFlushPanel)

function AdvancedFBMenuView:AdvancedFBMenuView()
    self.data_cares = {
        {data = SceneData.Instance:Base(),func = self.OnFlush},
        {data = FightData.Instance.advancefb_task,func = self.FlushTaskView},
        {data = VoMgr:DoorVoList(),func = self.FlushDoor},
    }
end

function AdvancedFBMenuView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.ObjBtnBoss:SetActive(false)
    self.ObjBtnDoor:SetActive(false)
    self:FlushDoor()
    self:FlushTaskView()
    self:OnFlush()
end

function AdvancedFBMenuView:OnFlush()
    if self.sceneid == SceneData.Instance:SceneId() then
        return
    end
    self.sceneid = SceneData.Instance:SceneId()
    local adv_config = FightData.Instance:GetAdvancementsSceneData(self.sceneid)
    if adv_config ~= nil then
        UH.SetText(self.Title,Format(Language.Fight.Progress,adv_config.level,adv_config.layer))
        UH.SetText(self.Goal,adv_config.txt)
        if adv_config.drop_num == "" then
            UH.SetText(self.Progress,"")
        end
    end
    self:FlushBtn()
end

function AdvancedFBMenuView:FlushTaskView()
    local adv_config = FightData.Instance:GetAdvancementsSceneData(SceneData.Instance:SceneId())
    if adv_config ~= nil then
        local info = FightData.Instance:GetAdvanceFbTaskInfo()
        if adv_config.drop_num ~= "" then
            UH.SetText(self.Progress,Format(Language.Fight.Collect,info.progress or 0,adv_config.drop_num))
        else
            UH.SetText(self.Progress,"")
        end
    end
    self:FlushBtn()
end

function AdvancedFBMenuView:FlushBtn()
    self.ObjBtnBoss:SetActive(false)
    if GetActive(self.ObjBtnDoor) then
        return
    end
    local adv_config = FightData.Instance:GetAdvancementsSceneData(SceneData.Instance:SceneId())
    if adv_config ~= nil then
        if not StringIsEmpty(adv_config.boss1_npc_id) or not StringIsEmpty(adv_config.boss2_npc_id) then
            local npc = nil
            SceneCtrl.Instance:Objs():ForeachSNPC(
                function(npc_obj)
                    if npc_obj.vo.id == adv_config.boss1_npc_id then
                        npc = npc_obj
                        return true
                    elseif npc_obj.vo.id == adv_config.boss2_npc_id then
                        npc = npc_obj
                        return true
                    end
                    return false
                end
            )
            if npc then 
                self.ObjBtnBoss:SetActive(true)
            end
        end
    end
end

function AdvancedFBMenuView:FlushDoor()
    if TeamData.Instance:InTeam() then
        if not TeamData.Instance:IsLeader() then
            PublicPopupCtrl.Instance:Center(Language.Team.TipsMove)
            return
        end
    end
    local adv_config = FightData.Instance:GetAdvancementsSceneData(self.sceneid)
    if adv_config and adv_config.level == adv_config.layer then
        local in_door = nil
        SceneCtrl.Instance:Objs():ForeachDoor(function(door)
            in_door = door
            return true
        end)
        if in_door then
            self.ObjBtnBoss:SetActive(false)
            self.ObjBtnDoor:SetActive(true)
        end
    end
end

function AdvancedFBMenuView:GoToBoss()
    if TeamData.Instance:InTeam() then
        if not TeamData.Instance:IsLeader() then
            PublicPopupCtrl.Instance:Center(Language.Team.TipsMove)
            return
        end
    end
    local adv_config = FightData.Instance:GetAdvancementsSceneData(SceneData.Instance:SceneId())
    if not TableIsEmpty(adv_config) then
        if not StringIsEmpty(adv_config.boss1_npc_id) or not StringIsEmpty(adv_config.boss2_npc_id) then
            local npc = nil
            SceneCtrl.Instance:Objs():ForeachSNPC(
                function(npc_obj)
                    if npc_obj.vo.id == adv_config.boss1_npc_id then
                        npc = npc_obj
                        return true
                    elseif npc_obj.vo.id == adv_config.boss2_npc_id then
                        npc = npc_obj
                        return true
                    end
                    return false
                end
            )
            if npc then 
                SceneLogic.Instance:AutoToNpc(npc.vo)
            end
        end
    end
end

function AdvancedFBMenuView:GoToDoor()
    if TeamData.Instance:InTeam() then
        if not TeamData.Instance:IsLeader() then
            PublicPopupCtrl.Instance:Center(Language.Team.TipsMove)
            return
        end
    end
    local in_door = nil
    SceneCtrl.Instance:Objs():ForeachDoor(function(door)
        in_door = door
        return true
    end)
    if in_door then
        SceneLogic.Instance:AutoToNpc(in_door.vo)
    end
end