MusicStationInfo = MusicStationInfo or BaseClass()

function MusicStationInfo:__init(info)
    self.id = info.music_id
    self.info = info

    self:MusicStationInfo()
    self:InitCheckPoint()
end

function MusicStationInfo:MusicStationInfo()
    self.button_pos = MusicStationData.Instance:MusicButtonPosListById(self.id)
end

function MusicStationInfo:InitCheckPoint()
    local temp_check = {0.2,0.5,0.8}

    self.check_points = {}
    for i = 1,3 do 
        local vo = {}
        vo.index = i
        vo.point = math.floor(temp_check[i] * self:MaxScore())
        vo.rate = temp_check[i]

        table.insert(self.check_points , vo)
    end 
end

function MusicStationInfo:Time()
    return self.info.time / 100
end

function MusicStationInfo:ButtonPos(cur_time)
    return self.button_pos[cur_time]
end

function MusicStationInfo:IsCombo(score_co)
    if 0 == self.info.combo_type then
        return 3 == score_co.type
    elseif 1 == self.info.combo_type then
        return score_co.type > 0
    end
end

function MusicStationInfo:ComboScale()
    return self.info.combo_score / 100
end

function MusicStationInfo:EndMusic()

end

function MusicStationInfo:Audio()
    return self.info.audio
end

function MusicStationInfo:Report(score, scale)

end

function MusicStationInfo:MaxScore()
    return self.info.max_score
end

function MusicStationInfo:CheckPoints()
    return self.check_points
end

-- function MusicStationInfo:CountDownNum()
--     local count_down_num = math.floor(SocietyData.Instance.shitu_chuangong_begin_time - TimeHelper.GetServerTime())
--     count_down_num = math.max(2, count_down_num)
--     return count_down_num
-- end

-- function MusicStationInfo:IsSeekMaster()
--     return SocietyData.Instance.shitu_chuangong_seek_master
-- end

-- function MusicStationInfo:AddScaleTips(score, scale_from, scale_to)
--     if scale_to > scale_from then
--         if self:IsSeekMaster() then
--             PublicPopupCtrl.Instance:Center(string.format(Language.MusicStation.AddScaleTips.master, score, scale_to))
--         else
--             PublicPopupCtrl.Instance:Center(string.format(Language.MusicStation.AddScaleTips.apprentice, score, scale_to))
--         end
--     end
-- end