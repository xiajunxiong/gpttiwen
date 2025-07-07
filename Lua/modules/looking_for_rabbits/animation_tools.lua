AnimaHelper = {}

--前提是播放
function AnimaHelper.ListenStop(anima, end_func)
    local handle_runner = Runner.Instance:RunUntilTrue(function ()
        if anima.isPlaying == false then
            if end_func then
                end_func()
            end
            return true
        end
    end)
    return handle_runner
end
function AnimaHelper.CopyClip(anima1, anima2, clip_name)
    local clip = anima1.GetClip(clip_name)
    anima2.AddClip(clip, clip_name)
end
