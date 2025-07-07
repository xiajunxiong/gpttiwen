TrainFinish = TrainFinish or DeclareView("TrainFinish", "guild/train_finish")
-- VIEW_DECLARE_LEVEL(TrainFinish,ViewLevel.L1)

function TrainFinish:TrainFinish()
    self.data = GuildData.Instance
    self.lang = Language.Guild
end

function TrainFinish:LoadCallback()
    local result = self.data:GetTrainResult()
    local pet = PetData.Instance:GetPetInfo(result.pet_index)
    UH.SetText(self.Name, pet:Name())
    local t_time = TimeManager.FormatDHMS(result.time)
    UH.SetText(self.Time, string.format(self.lang.TrainResult[1], t_time.hour, t_time.min))
    UH.SetText(self.Exp, string.format(self.lang.TrainResult[2], result.exp))
end

function TrainFinish:OnClickClose()
    ViewMgr:CloseView(TrainFinish)
end
