-- local g_voice_agent = Game.GVoiceAgent
-- local g_voice_engine = g_voice_agent.Inst
-- GVoiceErrorNo = GCloud.GVoice.ErrorNo
-- GVoiceComCode = GCloud.GVoice.CompleteCode
-- local GVoiceLanguage = GCloud.GVoice.Language
GVoiceErrorNo = {

}
GVoiceComCode = {

}

VoiceRecorder = {
    inited = false,
    user_id = "",
    play_com_func = nil,       -- com_code[GVoiceComCode],rec_key[string]
    upload_com_func = {},      -- key:record_file_path, value:func( com_code[GVoiceComCode],rec_key[string],time[float] )
    translate_com_func = {},   -- key:rec_key, value:func( com_code[GVoiceComCode],rec_key[string],ret[string] )

    recording_file_path = nil,
    playing_file_path = nil,
}

local app_id = "650457048"
local app_key = "d236ddb9dd9bf38bdef4bc1dcdb52a1b"

local cache_bg_vol = nil
local cache_eff_vol = nil
local playing_vol_lower = 0.2


function VoiceRecorder.PlayRecVolReduce()
    -- if cache_bg_vol == nil then
    --     cache_bg_vol = AudioMgr:GetBgVolume()
    -- end
    -- if cache_eff_vol == nil then
    --     cache_eff_vol = AudioMgr:GetEffectVolume()
    -- end
    -- AudioMgr:SetBgVolume(playing_vol_lower * cache_bg_vol)
    -- AudioMgr:SetEffectVolume(playing_vol_lower * cache_eff_vol)
end

function VoiceRecorder.PlayRecVolRecover()
    -- if cache_bg_vol ~= nil then
    --     AudioMgr:SetBgVolume(cache_bg_vol)
    -- end
    -- if cache_eff_vol ~= nil then
    --     AudioMgr:SetEffectVolume(cache_eff_vol)
    -- end
end

function VoiceRecorder.SetUserId(user_id)
    -- if g_voice_engine == nil then return end
    -- local err_co = g_voice_agent.SetAppInfo(app_id,app_key,user_id)
    -- if err_co ~= GVoiceErrorNo.Succ and err_co ~= GVoiceErrorNo.MacEditor then
    --     Debuger.LogError("VoiceRecorder::SetUserId ERROR=%s",tostring(err_co))
    --     return
    -- else
    --     VoiceRecorder.user_id = user_id
    -- end   
    -- if VoiceRecorder.inited == false then
    --     g_voice_engine.OnApplyMessageKeyCompleteEvent = g_voice_engine.OnApplyMessageKeyCompleteEvent + VoiceRecorder.OnApplyMessageKeyCompleteEventHandler
    --     g_voice_engine.OnUploadReccordFileCompleteEvent = g_voice_engine.OnUploadReccordFileCompleteEvent + VoiceRecorder.UploadReccordFileCompleteEventHandler
    --     g_voice_engine.OnSpeechToTextEvent = g_voice_engine.OnSpeechToTextEvent + VoiceRecorder.OnSpeechToText
    --     g_voice_engine.OnDownloadRecordFileCompleteEvent = g_voice_engine.OnDownloadRecordFileCompleteEvent + VoiceRecorder.OnDownloadRecordFileComplete
    --     g_voice_engine.OnPlayRecordFilCompleteEvent = g_voice_engine.OnPlayRecordFilCompleteEvent + VoiceRecorder.OnPlayRecordFileComplete

    --     err_co = g_voice_agent.Init()
    --     if err_co ~= GVoiceErrorNo.Succ and err_co ~= GVoiceErrorNo.MacEditor then
    --         Debuger.LogError("VoiceRecorder::SetUserId Init ERROR=%s",tostring(err_co))
    --         return
    --     end
    --     err_co = g_voice_agent.ApplyMessageKey(10000)
    --     if err_co ~= GVoiceErrorNo.Succ and err_co ~= GVoiceErrorNo.MacEditor then
    --         Debuger.LogError("VoiceRecorder::SetUserId ApplyMessageKey ERROR=%s",tostring(err_co))
    --         return
    --     end
    --     VoiceRecorder.inited = true
    -- end 
end

function VoiceRecorder.Unload()
    if g_voice_engine == nil then return end
    -- g_voice_engine.OnApplyMessageKeyCompleteEvent = g_voice_engine.OnApplyMessageKeyCompleteEvent - VoiceRecorder.OnApplyMessageKeyCompleteEventHandler
    -- g_voice_engine.OnUploadReccordFileCompleteEvent = g_voice_engine.OnUploadReccordFileCompleteEvent - VoiceRecorder.UploadReccordFileCompleteEventHandler
    -- g_voice_engine.OnSpeechToTextEvent = g_voice_engine.OnSpeechToTextEvent - VoiceRecorder.OnSpeechToText
    -- g_voice_engine.OnDownloadRecordFileCompleteEvent = g_voice_engine.OnDownloadRecordFileCompleteEvent - VoiceRecorder.OnDownloadRecordFileComplete
    -- g_voice_engine.OnPlayRecordFilCompleteEvent = g_voice_engine.OnPlayRecordFilCompleteEvent - VoiceRecorder.OnPlayRecordFileComplete
end

local rec_file_dir = "/GCloudVoice/"
if IS_EDITOR then
    rec_file_dir = string.format("%s%s",string.sub(UnityApp.dataPath,1,#UnityApp.dataPath - #"/Assets"),rec_file_dir)
else
    rec_file_dir = string.format("%s%s",string.gsub(UnityApp.persistentDataPath,"\\","/"),rec_file_dir)
end
 

local function get_record_file_path()
    return string.format("%s%s",rec_file_dir,tostring(os.date("%Y_%m_%d_%H_%M_%S")))
end

local function record_file_path_by_name(file_name)
    return string.format("%s%s",rec_file_dir,file_name)
end



---------------------------------接口-----------------------------------------------

-- LogError("pppppp===",get_record_file_path())
--开始录音
function VoiceRecorder.StartRecording()
    -- if RecordPermission ~= nil and RecordPermission == false and not IS_EDITOR and VersionTag.HasTag(VersionTag.NotAndroidXJar) then
    --     -- ChannelAgent.ToPlatWithParam("recordPermission")
    --     return
    -- end
    -- VoiceRecorder.recording_file_path = get_record_file_path()
    -- local err_co = g_voice_agent.StartRecording(VoiceRecorder.recording_file_path,true)
    -- if err_co ~= GVoiceErrorNo.Succ and err_co ~= GVoiceErrorNo.MacEditor then
    --     Debuger.LogError("VoiceRecorder.StartRecording ERROR=%s",tostring(err_co))
    -- end
    -- return err_co
end

--结束录音
--ready_func (com_code[GVoiceComCode], rec_key[string], time[float])  文件可以正常使用时回调(翻译，播放)
function VoiceRecorder.StopRecording(ready_func)
    -- local err_co = g_voice_agent.StopRecording()
    -- if err_co == GVoiceErrorNo.Succ then        --录音成功，等待上传完成给到回调
    --     VoiceRecorder.upload_com_func[VoiceRecorder.recording_file_path] = ready_func
    --     VoiceRecorder.recording_file = nil
    -- else
    --     if err_co ~= GVoiceErrorNo.MacEditor then
    --         Debuger.LogError("VoiceRecorder.StopRecording ERROR=%s",tostring(err_co))
    --     end
    --     if ready_func ~= nil then
    --         ready_func(err_co,"",0)
    --     end
    -- end
    -- return err_co
end

--转换文字
--rec_key 录音文件key
--com_func (com_code[GVoiceComCode], rec_key[string], ret[string]) 异步，转换完成给到的回调
function VoiceRecorder.SpeechToText(rec_key,com_func)
    -- local err_co = g_voice_agent.SpeechToText(rec_key,GVoiceLanguage.China,10000);
    -- if err_co ~= GVoiceErrorNo.Succ and err_co ~= GVoiceErrorNo.MacEditor then
    --     if err_co ~= GVoiceErrorNo.STTing then  --正在转换其它语音时不抛异常
    --         Debuger.LogError("VoiceRecorder.SpeechToText ERROR=%s,rec_key=%s",tostring(err_co),rec_key)
    --     end
    --     com_func(err_co,rec_key,nil)
    --     return
    -- end
    -- VoiceRecorder.translate_com_func[rec_key] = com_func
end

--播放录音
--rec_key 录音文件key
--com_func (com_code[GVoiceComCode], rec_key[string])  正常播放完成时的回调，手动stop不会回调
function VoiceRecorder.PlayRecord(rec_key,com_func)
    -- if g_voice_engine == nil then return end
    -- local rec_file_path = record_file_path_by_name(rec_key)
    -- local error_co
    -- local playing = false
    -- if FileTool.Exists(rec_file_path) then
    --     g_voice_engine:SetMode(1)
    --     error_co = g_voice_engine:PlayRecordedFile(rec_file_path)
    --     -- LogError("VoiceRecorder.PlayRecord::PlayRecordedFile,path=",rec_file_path,",error_co=",error_co)
    --     playing = true
    -- else
    --     -- local rec_dir = Path.GetDirectoryName(rec_file_path)
    --     if not FileTool.DirExists(rec_file_dir) then
    --         FileTool.CreateDir(rec_file_dir)
    --     end
    --     g_voice_engine:SetMode(1)
    --     error_co = g_voice_engine:DownloadRecordedFile(rec_key,rec_file_path,10000)
    --     -- LogError("VoiceRecorder.PlayRecord::DownloadRecordedFile,path=",rec_file_path,",error_co=",error_co)
    -- end
    -- if error_co ~= GVoiceErrorNo.Succ and error_co ~= GVoiceErrorNo.MacEditor then
    --     Debuger.LogWarning("VoiceRecorder.PlayRecord ERROR=%s,rec_key=%s",tostring(error_co),rec_key)
    --     com_func(GVoiceComCode.Unknown,rec_key)
    -- else
    --     VoiceRecorder.playing_file_path = rec_file_path
    --     VoiceRecorder.play_com_func = com_func
    --     if playing then
    --         VoiceRecorder.PlayRecVolReduce()
    --     end
    -- end
    -- return error_co
end

--停止播放当前正在播放的录音
function VoiceRecorder.StopPlayRecord()
    -- VoiceRecorder.PlayRecVolRecover()
    -- if g_voice_engine == nil then return end
    -- g_voice_engine:SetMode(1)
    -- local error_no = g_voice_engine:StopPlayFile()
    -- if error_no ~= GVoiceErrorNo.Succ and error_no ~= GVoiceErrorNo.MacEditor then
    --     Debuger.LogError("VoiceRecorder.StopPlayRecord ERROR=%s",tostring(error_no))
    -- end
    -- return error_no
end

---------------------------------接口结束-----------------------------------------------



-------------------回调函数---------------------------
function VoiceRecorder.OnApplyMessageKeyCompleteEventHandler(com_code)
    -- if com_code ~= GVoiceComCode.MessageKeyAppliedSucc and com_code ~= GVoiceErrorNo.MacEditor then
    --     Debuger.LogError("VoiceRecorder.OnApplyMessageKeyCompleteEventHandler ERROR=%s",tostring(com_code))
    -- end
end


function VoiceRecorder.UploadReccordFileCompleteEventHandler(com_code,file_path,file_id)
    -- local ucf = VoiceRecorder.upload_com_func[file_path]
    -- if ucf ~= nil then
    --     VoiceRecorder.upload_com_func[file_path] = nil
    -- else
    --     -- Debuger.LogError(
    --     --     "VoiceRecorder.UploadReccordFileCompleteEventHandler ERROR ComFunc NotExit,com_code=%s,file_path=%s,file_id=%s",
    --     --     tostring(com_code),file_path,file_id)
    -- end
    -- if com_code ~= GVoiceComCode.UploadRecordDone and com_code ~= GVoiceErrorNo.MacEditor then
    --     Debuger.LogError("VoiceRecorder.UploadReccordFileCompleteEventHandler ERROR=%s,file_path=%s,file_id=%s",
    --         tostring(com_code),file_path,file_id)
    --     if ucf ~= nil then
    --         ucf(com_code,file_id,0)
    --     end
    --     return
    -- else
    --     local param_err_no,byte_size,secend = g_voice_agent.GetFileParam(file_path,0,0)
    --     if param_err_no ~= GVoiceErrorNo.Succ and param_err_no ~= GVoiceErrorNo.MacEditor then
    --         Debuger.LogError("VoiceRecorder.UploadReccordFileCompleteEventHandler:GetFileParam ERROR=%s,file_path=%s",param_err_no,file_path)
    --     end
    --     local to_path = record_file_path_by_name(file_id)
    --     FileTool.Move(file_path,to_path)
    --     if ucf ~= nil then
    --         ucf(com_code,file_id,secend or 0)
    --     end
    -- end
    
end

function VoiceRecorder.OnSpeechToText(com_code,file_id,result)
    -- if com_code ~= GVoiceComCode.STTSucc and com_code ~= GVoiceErrorNo.MacEditor then
    --     Debuger.LogError("VoiceRecorder.OnSpeechToText ERROR=%s,file_id=%s",tostring(com_code),file_id)
    -- end
    -- local com_func = VoiceRecorder.translate_com_func[file_id]
    -- VoiceRecorder.translate_com_func[file_id] = nil
    -- if com_func ~= nil then
    --     com_func(com_code,file_id,result)
    -- end
end

function VoiceRecorder.OnDownloadRecordFileComplete(com_code,file_path,file_id)
    -- if com_code ~= GVoiceComCode.DownloadRecordDone and com_code ~= GVoiceErrorNo.MacEditor then
    --     Debuger.LogError("VoiceRecorder.OnDownloadRecordFileComplete ERROR=%s,rec_key=%s",tostring(com_code),file_id)
    --     if file_path == VoiceRecorder.playing_file_path then
    --         VoiceRecorder.play_com_func(com_code,file_id)
    --         VoiceRecorder.play_com_func = nil
    --         VoiceRecorder.playing_file_path = nil
    --     end
    --     return
    -- end
    -- if file_path == VoiceRecorder.playing_file_path and g_voice_engine ~= nil then
    --     g_voice_engine:SetMode(1)
    --     local error_co = g_voice_engine:PlayRecordedFile(file_path)
    --     if error_co ~= GVoiceErrorNo.Succ and error_co ~= GVoiceErrorNo.MacEditor then
    --         Debuger.LogError("VoiceRecorder.OnDownloadRecordFileComplete PlayFile ERROR=%s,rec_key=%s",tostring(error_co),file_id)
    --         VoiceRecorder.play_com_func(GVoiceComCode.Unknown,file_id)
    --         VoiceRecorder.play_com_func = nil
    --         VoiceRecorder.playing_file_path = nil
    --     else
    --         VoiceRecorder.PlayRecVolReduce()
    --     end
    -- end
end

function VoiceRecorder.OnPlayRecordFileComplete(com_code,file_path)
    -- if com_code ~= GVoiceComCode.PlayFileDone and com_code ~= GVoiceErrorNo.MacEditor then
    --     Debuger.LogError("VoiceRecorder.OnPlayRecordFileComplete=%s,file_path=%s",tostring(com_code),file_path)
    -- end
    -- if file_path == VoiceRecorder.playing_file_path then
    --     VoiceRecorder.PlayRecVolRecover()
    --     local rec_key = FileTool.GetFileName(file_path)
    --     VoiceRecorder.play_com_func(com_code,rec_key)
    --     VoiceRecorder.play_com_func = nil
    --     VoiceRecorder.playing_file_path = nil
    -- end
end


-- 两个可以用来测试的rec_key
-- 3056020100044f304d020100040f6465765f6c696e6b313032325f383002037a1db9020420fd03b702045ebb93a5042037393537663431653938386531303030643464333262353739376535633434340201000201000400
-- 3056020100044f304d020100040f6465765f6c696e6b313032325f383002037a1db9020420fd03b702045ebb93b1042066376431393639393266306531393161376362626432336335303865336134330201000201000400