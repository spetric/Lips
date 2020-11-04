require ("_prolog")
require ("_opencv")
function RequireParams()
   lips_RequireParams("features_detector = combo([AKAZE, ORBS, BRISK, KAZE, SURF, SIFT, FAST_DAISY, FAST_FREAK] 0);"
     .. "matching_algorithm = combo([Brute Hamming, Brute force matching, FLANN] 2);"
     .. "homography = combo([LS, RANSAC, LMEDS, RHO] 1);"
     .. "match_ratio = float([0.1, 2.0] 0.7);"
     .. "detect_multiple = bool([] false);")
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
  local params = tostring(features_detector)..", 0, 0, 0"
  -- initialize/create object detector
  ocv_Process("ObjDetectorBegin")   
  -- detect and compute
  ocv_Process("Features2D", params)
  local matchParams = tostring(matching_algorithm)..", "..tostring(match_ratio)..", true"
  -- true => OpenCV matching image is created/exported
  ocv_Process("Matches2D", matchParams) 
  ocv_FlipExport()   -- because image is upside down in OpenCV   
  local id = ocv_GetLastImageId()  
  local pictMatch = lips_GetImageFromId(id)
  if (pictMatch ~= nil) then
     lips_ExportImage(pictMatch, "matches", true)
  end 
  params = "6, "..tostring(homography)..", true, true, false"
  -- image is created/exported and alpha channel added (if necessary)
  ocv_Process("Homography2D", params) 
  ocv_FlipExport()   -- because image is upside down in OpenCV  
  id = ocv_GetLastImageId()    
  --lips_SendMessage(tostring(id))    
  local pictHomography = lips_GetImageFromId(id)
  if (pictHomography ~= nil) then
     lips_ExportImage(pictHomography, "aligned image 1.")
     lips_SendCommand("LayerAdd", "last")
  end 
  if (not detect_multiple) then
     ocv_Process("ObjDetectorEnd")         
     return
  end  
  local rc = 0
  local objCounter = 2
  while (rc == 0) do
    ocv_FlipExport()   -- flip it back again because we'll use it for recalculation
    rc = ocv_Process("ReduceFeatures2D", "2, false")
    if rc ~= 0 then 
       break
    end   
    -- find new matches
    rc = ocv_Process("Matches2D", matchParams)
    if rc ~= 0 then
       break
    end   
    rc = ocv_Process("RecalcHomography2D")
    if rc == 0 then 
      ocv_FlipExport()   -- because image is upside down in OpenCV  
      id = ocv_GetLastImageId()   
      --lips_SendMessage(tostring(id))      
      pictHomography = lips_GetImageFromId(id)
      if (pictHomography ~= nil) then
        --lips_SendMessage(tostring(pictHomography.Width).." x "..tostring(pictHomography.Height))      
        lips_ExportImage(pictHomography, "aligned image "..tostring(objCounter)..".")
        lips_SendCommand("LayerAdd", "last")
        objCounter = objCounter + 1
      end 
    end  
  end 
  ocv_Process("ObjDetectorEnd")    
end