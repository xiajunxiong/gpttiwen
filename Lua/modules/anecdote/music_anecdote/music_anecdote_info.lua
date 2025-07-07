MusicAnecdoteInfo = MusicAnecdoteInfo or BaseClass()

function MusicAnecdoteInfo:__init(info)
    self.id = info.music_id
    self.info = info

    self:MusicAnecdoteInfo()
    self:InitCheckPoint()
end

function MusicAnecdoteInfo:MusicAnecdoteInfo()
    self.button_pos = MusicAnecdoteData.Instance:MusicButtonPosListById(self.id)
end

function MusicAnecdoteInfo:InitCheckPoint()
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

function MusicAnecdoteInfo:Time()
    return self.info.time / 100
end

function MusicAnecdoteInfo:ButtonPos(cur_time)
    return self.button_pos[cur_time]
end

function MusicAnecdoteInfo:IsCombo(score_co)
    if 0 == self.info.combo_type then
        return 3 == score_co.type
    elseif 1 == self.info.combo_type then
        return score_co.type > 0
    end
end

function MusicAnecdoteInfo:ComboScale()
    return self.info.combo_score / 100
end

function MusicAnecdoteInfo:EndMusic()

end

function MusicAnecdoteInfo:Audio()
    return self.info.audio
end

function MusicAnecdoteInfo:Report(score, scale)

end

function MusicAnecdoteInfo:MaxScore()
    return self.info.max_score
end

function MusicAnecdoteInfo:CheckPoints()
    return self.check_points
end

-- function MusicAnecdoteInfo:CountDownNum()
--     local count_down_num = math.floor(SocietyData.Instance.shitu_chuangong_begin_time - TimeHelper.GetServerTime())
--     count_down_num = math.max(2, count_down_num)
--     return count_down_num
-- end

-- function MusicAnecdoteInfo:IsSeekMaster()
--     return SocietyData.Instance.shitu_chuangong_seek_master
-- end

-- function MusicAnecdoteInfo:AddScaleTips(score, scale_from, scale_to)
--     if scale_to > scale_from then
--         if self:IsSeekMaster() then
--             PublicPopupCtrl.Instance:Center(string.format(Language.MusicStation.AddScaleTips.master, score, scale_to))
--         else
--             PublicPopupCtrl.Instance:Center(string.format(Language.MusicStation.AddScaleTips.apprentice, score, scale_to))
--         end
--     end
-- end