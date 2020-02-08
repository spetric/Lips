require ("_prolog")
require ("_opencv")
function RequireParams()
   lips_RequireParams("features_detector = combo([AKAZE, ORBS, BRISK] 0);"
     .. "matching_algorithm = combo([Brute Hamming, Brute force matching, FLANN] 2);"
     .. "homography = combo([LS, RANSAC, LMEDS, RHO] 1);"
     .. "match_ratio = float([0.1, 2.0] 0.7);")
   end   
function main()
  if (not OcvEnabled) then
     lips_SendMessage(errOcvDisabed)  
     return
  end         
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end     
  lips_SendCommand("LayerClearAll")
  lips_SendCommand("LayerBackground", "target")
  -- set source and target image
  ocv_SetImage("source", SourceImage)
  ocv_SetImage("target", TargetImage)     
  local params = tostring(features_detector)
  -- detect and compute
  ocv_Process("Features2D", params)
  params = tostring(matching_algorithm)..", "..tostring(match_ratio)..", true"
  -- true => OpenCV matching image is created/exported
  ocv_Process("Matches2D", params) 
  ocv_FlipExport()   -- because image is upside down in OpenCV   
  local id = ocv_GetLastImageId()  
  local pictMatch = lips_GetImageFromId(id)
  if (pictMatch ~= nil) then
     lips_ExportImage(pictMatch, "matches")
  end 
  params = "6, "..tostring(homography)..", true, true, false"
  -- image is created/exported and alpha channel added (if necessary)
  ocv_Process("Homography2D", params) 
  ocv_FlipExport()   -- because image is upside down in OpenCV  
  id = ocv_GetLastImageId()    
  local pictHomography = lips_GetImageFromId(id)
  if (pictHomography ~= nil) then
     lips_ExportImage(pictHomography, "aligned (warped) image")
     lips_SendCommand("LayerAdd", "last")
  end 
end