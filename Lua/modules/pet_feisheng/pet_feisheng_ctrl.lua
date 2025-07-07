FILE_REQ("modules/pet_feisheng/pet_feisheng_data")
VIEW_REQ("modules/pet_feisheng/pet_feisheng")
VIEW_REQ("modules/pet_feisheng/pet_feisheng_succ")
VIEW_REQ("modules/pet_feisheng/pet_feisheng_skill_sel")
VIEW_REQ("modules/pet_feisheng/pet_fsskill_lvup_tip_view")
VIEW_REQ("modules/pet_feisheng/pet_fsskill_lvup_ret_view")

PetFeiShengCtrl = PetFeiShengCtrl or BaseClass(BaseCtrl)


function PetFeiShengCtrl:__init()
    if PetFeiShengCtrl.Instance ~= nil then
        Debuger.LogError("[PetFeiShengCtrl] attempt to create singleton twice!")
        return
    end
    PetFeiShengCtrl.Instance = self
    self.data = PetFeiShengData.New()
    self:RegisterProtocol(SCPetFeiShengTaskInfo, "OnSCPetFeiShengTaskInfo")

end

function PetFeiShengCtrl:__delete()
    self:UnRegisterProtocol(SCPetFeiShengTaskInfo)

    self.data:DeleteMe()
    self.data = nil
    PetFeiShengCtrl.Instance = nil
end

function PetFeiShengCtrl:OnUnloadGameLogic()
	self.data:Clear()
end


function PetFeiShengCtrl:OnSCPetFeiShengTaskInfo(protocol)
    local pet_info = self.data:CurTaskingPet()
    if pet_info ~= nil then
        if pet_info:UniqueId1() == protocol.info.pet_unique_id1 and 
            pet_info:UniqueId2() == protocol.info.pet_unique_id2 and
            self.data:CurTaskId() == protocol.info.task_id and 
            protocol.info.today_is_finish ~= 0 and 
            self.data:IsTodayTaskFinished() == false then
            PetData.Instance:SetSelect(pet_info:Index())
            ViewMgr:OpenView(PetFeiSheng)
        end    
    end
    self.data:SetPetFeiShengTaskInfo(protocol)
end

function PetFeiShengCtrl:RequestTask(pet_index)
    PetCtrl.SendPetOp(PET_OP_TYPE.StartPetFeiShengTask, pet_index)
end

function PetFeiShengCtrl:RequestLvupPassiveSkill(pet_index,skill_index, is_lian_yao)
    -- LogError("PetFeiShengCtrl:RequestLvupPassiveSkill===",pet_index,skill_index - 1)
    PetCtrl.SendPetOp(PET_OP_TYPE.UpLevelPassiveSkill, pet_index,skill_index - 1, is_lian_yao)   --服务端是从0开始，所以减1
end

function PetFeiShengCtrl:RequestFeiShengSkillSelect(pet_index,skill_index)
    -- LogError("PetFeiShengCtrl:RequestFeiShengSkillSelect===",pet_index,skill_index - 1)
    PetCtrl.SendPetOp(PET_OP_TYPE.StartPetFeiSheng, pet_index,skill_index - 1)  --服务端是0-2，所以减1
end

function PetFeiShengCtrl:RequestFeiShengSkillChange(pet_index,skill_index)
    -- LogError("PetFeiShengCtrl:RequestFeiShengSkillChange===",pet_index,skill_index - 1)
    PetCtrl.SendPetOp(PET_OP_TYPE.FeiShengSkillChange, pet_index,skill_index - 1)
end

function PetFeiShengCtrl:RequestFeiShengSkillLvup(pet_index)
    PetCtrl.SendPetOp(PET_OP_TYPE.FSSkillUpLv, pet_index)
end