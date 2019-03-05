-- Symmetric Nearest Neighbour as described by Hardwood et al in Pattern
-- recognition letters 6 (1987) pp155-162.
-- modified for lipsEngine by Siniša Petriæ http://www.sigmapi-design.com, 2018
require ("_prolog")
function RequireParams()
   lips_RequireParams("radius = int([1, 20] 6);")
end   
function main() 
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end    
  snn_color(radius)
end

-- and a color version operating in CIE Lab 
function deltaE(l1,a1,b1,l2,a2,b2)
  return (l1-l2)*(l1-l2) + (a1-a2)*(a1-a2) + (b1-b2)*(b1-b2)
  --return math.sqrt( (l1-l2)*(l1-l2) + (a1-a2)*(a1-a2) + (b1-b2)*(b1-b2))
end
-- main routine
function snn_color(radius)
local labImage = lips_CreateImage("float3", SourceImage.Width, SourceImage.Height)
--convert image to lab
lips_SendMessage("Converting image to lab...")
lips_Convert_rgb2lab(SourceImage, labImage)
local x,y
for y=Roi.Top, Roi.Bottom do
	for x=Roi.Left, Roi.Right do
	    local sumL=0.0
	    local suma=0.0
	    local sumb=0.0
	    local count=0.0
	    local u,v
	    
	    local lc,ac,bc = lips_GetPixelSafe(labImage, y, x)
	    for v=-radius,radius do
	      for u=-radius,radius do
			     local l1,a1,b1 = lips_GetPixelSafe(labImage, y+v, x+u)
	         local l2,a2,b2 = lips_GetPixelSafe(labImage, y-v, x-u)
	         if (deltaE(lc,ac,bc,l1,a1,b1) <
	             deltaE(lc,ac,bc,l2,a2,b2)) then	             
               sumL = sumL + l1
               suma = suma + a1
               sumb = sumb + b1
	         else
	             sumL = sumL + l2
               suma = suma + a2
               sumb = sumb + b2	         
	         end
	         count = count + 1	         
	      end
	    end
      lips_SetPixel(labImage, y, x, sumL/count, suma/count, sumb/count)
	    end
  lips_ShowProgress(y - Roi.Top, Roi.Bottom - Roi.Top)      
  end 
  --convert image to rgb
  lips_SendMessage("Converting image to rgb...")
  lips_Convert_lab2rgb(labImage, TargetImage)  

end

