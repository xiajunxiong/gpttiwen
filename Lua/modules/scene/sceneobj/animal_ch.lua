AnimalCh = AnimalCh or BaseClass(SceneObj)
function AnimalCh:__init()
    self.vo = nil
end
function AnimalCh:__delete()
    AnimalRaceData.Instance.ar_info:Uncare(self.animal_ar_info_sm_care)
    TimeHelper:CancelTimer(self.timer_animal_race)
end

function AnimalCh:InitEnd()
    self.animal_ar_info_sm_care = AnimalRaceData.Instance.ar_info:Care(BindTool.Bind(self.FlushAnimalState, self), "info_type")
    self:FlushAnimalState()
end

function AnimalCh:FlushAnimalState()
    local ar_info = AnimalRaceData.Instance.ar_info
    local start_x, end_x, start_y = AnimalRaceConfig.AnimalPoints.StartX, AnimalRaceConfig.AnimalPoints.EndX, AnimalRaceConfig.AnimalPoints.StartY[self.vo.pet_id]
    local distance = end_x - start_x
    TimeHelper:CancelTimer(self.timer_animal_race)
    if AnimalRaceConfig.InfoType.RoleDoing == ar_info.info_type then
        self.timer_animal_race = TimeHelper:AddRunTimer(function ()
            local pass_time = math.floor(AnimalRaceConfig.RaceAllTime - (ar_info.timestamp - TimeHelper.GetServerTime()))
            if 0 < pass_time and pass_time < AnimalRaceConfig.RaceMaxTime then
                local percent = AnimalRaceData.Instance:GetAnimalPosPercent(self.vo.pet_id, pass_time)
                self:AnimalMoveTo(start_x + percent * distance, start_y)
                if 1 == percent then
                    TimeHelper:CancelTimer(self.timer_animal_race)
                end
            else
                self:SetAnimalPos(0 == pass_time and start_x or end_x, start_y)
                TimeHelper:CancelTimer(self.timer_animal_race)
            end
        end, 1)
    else
        self:SetAnimalPos(start_x, start_y)
    end
end

function AnimalCh:SetAnimalPos(x, y)
    local pos, sample_set = Vector3.New(x, 0, y)
    pos.y, sample_set = SceneCtrl.Instance:CurScene():GetPosHeight(x, y)
    self:SetPos(pos, sample_set)
end

function AnimalCh:AnimalMoveTo(x, y)
    local pos, sample_set = Vector3.New(x, 0, y)
    pos.y, sample_set = SceneCtrl.Instance:CurScene():GetPosHeight(x, y)
    SceneCh.MoveTo(self, pos)
end

function AnimalCh:FlushDrawer()
    local data = string.format(DrawerHelper.GetNpcPath(self.vo.res_id))
    self.body:SetData(data)
end
