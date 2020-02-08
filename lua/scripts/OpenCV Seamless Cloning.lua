require ("_prolog")
require ("_opencv")
function RequireParams()
  lips_RequireParams("cloning_mode = combo([Normal clone, Mixed clone, Monochrome transfer] 0);")   
  lips_SendCommand("selection", "SetCustom") -- sets custom image and mask from selection buffer
end   
function main()
  if (not OcvEnabled) then
     lips_SendMessage(errOcvDisabled)  
     return     
  end       
  -- target images must be loaded
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end    
  lips_SendCommand("selection", "SetTargetRoi") -- sets target roi from selection (overrides target roi)    
  if TargetRoi == nil or CustomImage.Plane == nil  then
     lips_SendMessage("No selection.")  
     lips_HostDialog("Selection does not exist. Create selection on the source image, copy selected area to buffer and reload the script.\r\nMove selection on target image and execute script to clone selected portion of source image on target image.\r\nSelected image portion is alive in buffer for entire session.", 0)        
     return
  end
  local sImage = CustomImage
  local sMask  = CustomMask 
  lips_SendCommand("selection", "DisableRoi")   -- disable selection roi for post-processing 
  -- set OpenCV src/tgt images
  ocv_SetImage("source", sImage)
  ocv_SetImage("srcmask", sMask)
  ocv_SetImage("target", TargetImage) 
  -- TODO: check boundaries
  local centerX = math.floor(TargetRoi.Left + (TargetRoi.Right - TargetRoi.Left)/2)     
  local centerY = math.floor(TargetRoi.Top + (TargetRoi.Bottom - TargetRoi.Top)/2)
  local scType = cloning_mode + 1
  -- OpenCV Mat is upside-down, so let's adjust centerY value
  centerY = TargetImage.Height - 1 - centerY
  local params = tostring(centerX)..", "..tostring(centerY)..", "..tostring(scType)
  ocv_Process("SeamlessClone", params)   
end
