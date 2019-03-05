-- Sharpen_Luminance
-- apply a sharpening convolution to the value channel.
-- author: Dan Ritchie
-- modified for lipEngine by Siniša Petriæ http://www.sigmapi-design.com, 2018  
require ("_prolog")
colrut = require("_colors")
function main()
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end    
  kernel = { { -1, -1,  -1},
           { -1,  12,  -1},
           { -1, -1,  -1} }
  divisor = 4
  offset = 0

  sharpen_luminance()
end

function filter (x, y)
   local i, j, sum, v2 , dummy1, dummy2
   sum = 0

   for i= -1,1 do
      for j= -1,1 do
         dummy1, dummy2, v2= colrut.rgb2hsv(lips_GetPixelSafe(SourceImage, y + j, x + i))
         sum = sum + (kernel[j+2][i+2] * v2)
      end
   end
   return (sum / divisor) + offset
end

--main routine
function sharpen_luminance()
  for y = Roi.Top, Roi.Bottom do
      for x = Roi.Left, Roi.Right do
          local h, s, v = colrut.rgb2hsv(lips_GetPixel(SourceImage, y, x))   
          v = filter (x, y)   
          local r, g, b = lips_NormalizeRGB(colrut.hsv2rgb(h, s, v)) 
          lips_SetPixel(TargetImage, y, x, r, g, b)       
      end
  lips_ShowProgress(y - Roi.Top, Roi.Bottom - Roi.Top)      
  end
end 
