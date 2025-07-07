MusicGameInfo = MusicGameInfo or BaseClass()

function MusicGameInfo:__init(info)
    self.id = info.music_id
    self.info = info

    self:MusicGameInfo()
end

function MusicGameInfo:MusicGameInfo()
    self.button_pos = MusicGameData.Instance:MusicButtonPosListById(self.id)
end

function MusicGameInfo:Time()
    return self.info.time / 100
end

function MusicGameInfo:ButtonPos(cur_time)
    return self.button_pos[cur_time]
end

function MusicGameInfo:IsCombo(score_co)
    if 0 == self.info.combo_type then
        return 3 == score_co.type
    elseif 1 == self.info.combo_type then
        return score_co.type > 0
    end
end

function MusicGameInfo:ComboScale()
    return self.info.combo_score / 100
end

function MusicGameInfo:EndMusic()
    SocietyData.Instance.shitu_chuangong_loop = false
end

function MusicGameInfo:Audio()
    return self.info.audio
end

function MusicGameInfo:Report(score, scale)
    SocietyCtrl.Instance:SendShiTuChuanGongFBReqGameInfo(score, score * scale)
end

function MusicGameInfo:CountDownNum()
    local count_down_num = math.floor(SocietyData.Instance.shitu_chuangong_begin_time - TimeHelper.GetServerTime())
    count_down_num = math.max(2, count_down_num)
    return count_down_num
end

function MusicGameInfo:IsSeekMaster()
    return SocietyData.Instance.shitu_chuangong_seek_master
end

function MusicGameInfo:AddScaleTips(score, scale_from, scale_to)
    if scale_to > scale_from then
        if self:IsSeekMaster() then
            PublicPopupCtrl.Instance:Center(string.format(Language.MusicGame.AddScaleTips.master, score, scale_to))
        else
            PublicPopupCtrl.Instance:Center(string.format(Language.MusicGame.AddScaleTips.apprentice, score, scale_to))
        end
    end
end