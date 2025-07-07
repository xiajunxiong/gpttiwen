
FILE_REQ("modules/task/du_chuang_zei_ying/du_chuang_zei_ying_config")
FILE_REQ("modules/task/du_chuang_zei_ying/du_chuang_zei_ying_data")

VIEW_REQ("modules/task/du_chuang_zei_ying/du_chuang_zei_ying_menu_view")

FILE_REQ("modules/task/du_chuang_zei_ying/du_chuang_zei_ying_effect")

DuChuangZeiYingCtrl = DuChuangZeiYingCtrl or BaseClass(BaseCtrl)

function DuChuangZeiYingCtrl:__init()
    if DuChuangZeiYingCtrl.Instance ~= nil then
        Debuger.LogError("[DuChuangZeiYingCtrl] attempt to create singleton twice!")
        return
    end
    DuChuangZeiYingCtrl.Instance = self
    self.data = DuChuangZeiYingData.New()

    self:RegisterAllProtocols()
end

function DuChuangZeiYingCtrl:__delete()
    if DuChuangZeiYingCtrl.Instance == self then
        DuChuangZeiYingCtrl.Instance = nil
    end
    self:UnRegisterAllProtocols()
end

function DuChuangZeiYingCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCDuChuangZeiYingSceneInfo, "OnDuChuangZeiYingSceneInfo")
    self:RegisterProtocol(SCDuChuangZeiYingNotice, "OnDuChuangZeiYingNotice")
end

function DuChuangZeiYingCtrl:UnRegisterAllProtocols()
end

function DuChuangZeiYingCtrl:OnDuChuangZeiYingSceneInfo(protocol)
	self.data:SetDuChuangZeiYingSceneInfo(protocol)
    self:CreateDuChuangZeiYingPatrolMonster()
end

function DuChuangZeiYingCtrl:OnDuChuangZeiYingNotice(protocol)
	self.data:SetDuChuangZeiYingNotice(protocol)
end
function DuChuangZeiYingCtrl:CreateDuChuangZeiYingPatrolMonster()
    if DuChuangZeiYingData.Instance:GetDuChuangZeiYingSceneId1() == SceneData.Instance:SceneId() then
        local patrol_monster_list = DuChuangZeiYingData.Instance:GetPatrolMonsterList()
        for k, v in pairs(patrol_monster_list) do
            local vo = CommonStruct.NpcVo(v)
            local obj_id = SceneObjHolder.CalcClientObjId(SceneData.Instance:SceneId(), SceneObjType.DuChuangZeiYingPatrolMonster, k)
            local is_show = self.data.patrol_monsters_show[vo.id]
            if is_show then
                vo.obj_id = obj_id
                vo.res_id = v.res_id
                vo.move_speed = v.move_speed
                vo.stop_time = v.stop_time
                vo.alway_show = true
                vo.obj_type = SceneObjType.DuChuangZeiYingPatrolMonster
                VoMgr:AddGCObj(vo)
            else
                VoMgr:DelGCObj(obj_id)
            end
        end
    end
end