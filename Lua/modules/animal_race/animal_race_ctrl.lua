
FILE_REQ("modules/animal_race/animal_race_config")
FILE_REQ("modules/animal_race/animal_race_data")
FILE_REQ("modules/animal_race/animal_race_shots")

VIEW_REQ("modules/animal_race/animal_race_view")
VIEW_REQ("modules/animal_race/animal_race_dialogs")

AnimalRaceCtrl = AnimalRaceCtrl or BaseClass(BaseCtrl)

function AnimalRaceCtrl:__init()
	if AnimalRaceCtrl.Instance ~= nil then
		Debuger.LogError("[AnimalRaceCtrl] attempt to create singleton twice!")
		return
	end	
	AnimalRaceCtrl.Instance = self
	self.data = AnimalRaceData.New()

	self:RegisterAllProtocols()
end

function AnimalRaceCtrl:__delete()
	if AnimalRaceCtrl.Instance == self then
		AnimalRaceCtrl.Instance = nil
	end

	self:UnRegisterAllProtocols()
end

function AnimalRaceCtrl:RegisterAllProtocols()
	self:RegisterProtocol(SCRAAnimalRaceInfo, "OnRAAnimalRaceInfo")
	self:RegisterProtocol(SCRAAnimalRaceRoleInfo, "OnRAAnimalRaceRoleInfo")
	self:RegisterProtocol(SCRAAnimalRaceAnimationInfo, "OnRAAnimalRaceAnimationInfo")
	ActivityRandData.Instance:CustomClickHandle(ACTIVITY_TYPE.RAND_ANIMAL_RACE,function()
		SceneCtrl.Instance:RequestEnterFb(FB_TYPE.ANIMAL_RACE)
	end)
end

function AnimalRaceCtrl:UnRegisterAllProtocols()
end

function AnimalRaceCtrl:OnRAAnimalRaceInfo(protocol)
	self.data:SetRAAnimalRaceInfo(protocol)
end

function AnimalRaceCtrl:OnRAAnimalRaceRoleInfo(protocol)
	self.data:SetRAAnimalRaceRoleInfo(protocol)
end

function AnimalRaceCtrl:OnRAAnimalRaceAnimationInfo(protocol)
	self.data:SetRAAnimalRaceAnimationInfo(protocol)
end

function AnimalRaceCtrl:SendAnimalRaceOper(param_t)
	ServerActivityCtrl.Instance:SendRandActivityOperaReq{
		rand_activity_type = ACTIVITY_TYPE.RAND_ANIMAL_RACE,
		opera_type = param_t.oper_type,
		param_1 = param_t.param_1,
		param_2 = param_t.param_2,
	}
end

function AnimalRaceCtrl:SendAnimalRaceOperRoleInfo()
	self:SendAnimalRaceOper{
		oper_type = AnimalRaceConfig.OperType.role_info,
	}
end

function AnimalRaceCtrl:SendAnimalRaceOperBet(id, num)
	self:SendAnimalRaceOper{
		oper_type = AnimalRaceConfig.OperType.bet,
		param_1 = id,
		param_2 = num,
	}
end

function AnimalRaceCtrl:SendAnimalRaceOperActivityInfo()
	self:SendAnimalRaceOper{
		oper_type = AnimalRaceConfig.OperType.activity_info,
	}
end

function AnimalRaceCtrl:SendAnimalRaceOperAnimInfo()
	self:SendAnimalRaceOper{
		oper_type = AnimalRaceConfig.OperType.anim_info,
	}
end
function AnimalRaceCtrl:CreateAnimal()
	if AnimalRaceConfig.SceneId == SceneData.Instance:SceneId() then
        local animal_list = AnimalRaceData.Instance:GetPlayerList()
        for k, v in pairs(animal_list) do
            local obj_id = SceneObjHolder.CalcClientObjId(SceneData.Instance:SceneId(), SceneObjType.Animal, k)
			local vo = CommonStruct.NpcVo({})
			vo.obj_id = obj_id
			vo.res_id = v.res_id
			vo.pet_id = v.id
			vo.dir_y = 90
			VoMgr:AddGCObj(vo)
                -- self:CreateClientObj(SceneObjType.Animal, vo)
        end
    end
end