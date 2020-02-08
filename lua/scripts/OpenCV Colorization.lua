require ("_prolog")
require ("_opencv")
function RequireParams()  
  lips_RequireParams("color_rebalance = bool([] true);")
  --
  end
--[[
https://raw.githubusercontent.com/richzhang/colorization/master/colorization/models/colorization_deploy_v2.prototxt
 http://eecs.berkeley.edu/~rich.zhang/projects/2016_colorization/files/demo_v2/colorization_release_v2.caffemodel
http://eecs.berkeley.edu/~rich.zhang/projects/2016_colorization/files/demo_v2/colorization_release_v2_norebal.caffemodel 
--]]
function main()
  if (not OcvEnabled) then
     lips_SendMessage(errOcvDisabled) 
     return     
  end     
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end    
  if (SourceImage.Width ~= TargetImage.Width or SourceImage.Height ~= TargetImage.Height) then
     lips_SendMessage("Source.Size <> Target.Size")
     return     
  end     
  local model = ExePath
  local config = ExePath.."data/colorization/colorization_deploy_v2.prototxt"  
  local model_dnld, model_sufix
  local config_dnld = "https://raw.githubusercontent.com/richzhang/colorization/master/colorization/models/colorization_deploy_v2.prototxt"
  if color_rebalance then
     model_sufix = "rebalance"
     model = model.."data/colorization/colorization_release_v2.caffemodel"
     model_dnld ="https://people.eecs.berkeley.edu/~rich.zhang/projects/2016_colorization/files/demo_v2/colorization_release_v2.caffemodel"
  else
     model_sufix = "no-rebalance"
     model = model.."data/colorization/colorization_release_v2_norebal.caffemodel"
     model_dnld = "https://people.eecs.berkeley.edu/~rich.zhang/projects/2016_colorization/files/demo_v2/colorization_release_v2_norebal.caffemodel"  
  end 
  -- let's check if files exist
  if (not lips_Check("FileExists", config)) then
    lips_SendMessage("Error: Config file missing.")  
    lips_HostDialog('Config file missing. Download config file: <A href="'..config_dnld..'">colorization  prototxt</A><br>and place it in <b>data/colorization/</b> folder.', 2)
    return
  end  
  if (not lips_Check("FileExists", model)) then
    lips_SendMessage("Error: Model file missing.")      
    lips_HostDialog('Model file missing. Download model file: <A href="'..model_dnld..'">colorization  caffemodel '..model_sufix..'</A><br> and place it in <b>data/colorization/</b> folder.', 2)    
    return
  end    
  --[[
    colorization DNN is sensitive to image orientation and because openCV Mat is upside-down
    we need to flip source image.
  ]]--
  -- set source and target image
  local inpImage = lips_CreateImage("rgb24", SourceImage.Width, SourceImage.Height)  
  local outImage = lips_CreateImage("rgb24", TargetImage.Width, TargetImage.Height)    
  lips_CopyImage(SourceImage, inpImage)  
  -- set OpenCV src/tgt images
  ocv_SetImage("source", inpImage)
  ocv_SetImage("target", outImage)  
  ocv_FlipSource()
    -- create empty DK
  local dkIdx = ocv_CreateDK()
  -- set DK idx  
  local rc = ocv_SetDK(dkIdx)
  -- load style (model)
  lips_SendMessage("Loading model.");  
  ocv_Process("LoadDNN", model, config)  
  lips_SendMessage("Applying colorization.");
  ocv_Process("Colorize") 
  ocv_FlipTarget() -- flip target image back    
  lips_CopyImage(outImage, TargetImage)    
  -- release dk
  ocv_ClearDK(dkIdx)  
end
