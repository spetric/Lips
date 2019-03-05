-- maximum luminance difference
-- search for maximum luminance difference and replace current pixel
-- with pixel having max lum difference
-- author: Siniša Petriæ http://www.sigmapi-design.com, 2018
require ("_prolog")
function RequireParams()
   lips_RequireParams("radius = int([1, 20] 2); ")
end   
function main()
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end    
   maxlumdiff(radius)
end
--return pixel position of maxlumdiff
function pos_maxlumdiff (x, y, size)
   local x_pos, y_pos, lum, lum_max, lum_center
   x_pos, y_pos = x, y   
   lum = 0
   lum_max = 0  
   lum_center = lips_GetLuminanceSafe(SourceImage, y, x)
   for i = -size, size do
       for j = -size, size do
           lum = math.abs(lips_GetLuminanceSafe(SourceImage, y+i, x+i) - lum_center)
           if lum > lum_max then
              lum_max = lum
              x_pos, y_pos = x + j, y + i                      
           end   
       end
   end
   return x_pos, y_pos
end
--execute maxlumdiff
function maxlumdiff(size)
  for y = Roi.Top, Roi.Bottom do
      for x = Roi.Left, Roi.Right do
         local new_x, new_y = pos_maxlumdiff(x, y, size) 
         local r, g, b = lips_GetPixelSafe(SourceImage, new_y, new_x)
         lips_SetPixel(TargetImage, y, x, r, g, b)        
      end
      lips_ShowProgress(y - Roi.Top, Roi.Bottom - Roi.Top)      
  end
end   
                    