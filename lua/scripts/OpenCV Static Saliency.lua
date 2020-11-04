require ("_prolog")
require ("_opencv")
function RequireParams()  
   lips_RequireParams("saliency_type = combo([Fine grained, Spectral residual] 0); "
     .."show = combo([Export saliency map, Export binarized map, Export both maps, Blur with saliency map, Blur with binarized map, Median with saliency map, Median with binarized map] 0); "
     .."filter_radius = int([1, 15] 1);")
end
function main()
  if (not OcvEnabled) then
     lips_SendMessage(errOcvDisabled)  
     return     
  end     
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end      
  -- set source and target image
  ocv_SetImage("source", SourceImage)
  ocv_SetImage("target", TargetImage)   
  --local gsMask = lips_CreateImage("grayscale", SourceImage.Width, SourceImage.Height)
  --lips_Convert_rgb2grayscale(SourceImage, gsMask)  
  ocv_Process("StaticSaliency", tostring(saliency_type))      
  ocv_FlipExport()   -- because image is upside down in OpenCV  
  local id = ocv_GetLastImageId()    
  local expMap = lips_GetImageFromId(id)
  local salMap, binMap
  local radius
  -- static saliency returns:
  -- 1-channel 8U saliency map as expMap image and
  -- 1-channel 8U binary map as expMap alpha channel
  -- we will convert
  if (expMap ~= nil and show < 3) then    
     salMap = lips_CreateImage("rgb24", expMap.Width, expMap.Height)    
     binMap = lips_CreateImage("rgb24", expMap.Width, expMap.Height)    
     lips_Convert_grayscale2rgb(expMap, salMap)
     lips_Convert_alpha2rgb(expMap, binMap)     
  end
  if (show == 0) then
     lips_ExportImage(salMap, "saliency map")  
  elseif (show == 1) then 
     lips_ExportImage(binMap, "binarized map")      
  elseif (show == 2) then 
     lips_ExportImage(salMap, "saliency map")      
     lips_ExportImage(binMap, "binarized map")
  elseif (show < 5) then 
     radius = tostring(filter_radius)      
     ocv_Process("blur", radius..", "..radius)
     if (show == 3) then
        lips_Blend(SourceImage, expMap, TargetImage, 0)  
     else
        lips_Blend(SourceImage, expMap, TargetImage, 1)  
     end      
  else
     radius = tostring(filter_radius * 2 + 1)      
     ocv_Process("median", radius)
     if (show == 5) then
        lips_Blend(SourceImage, expMap, TargetImage, 0)  
     else
        lips_Blend(SourceImage, expMap, TargetImage, 1)  
     end         
  end
  --lips_Blend(SourceImage, gsMask, TargetImage, false)  
end
