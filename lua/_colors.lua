-- color conversion
-- rgb to hsv
local colrut = {}
function colrut.rgb2hsv(r, g, b)
  r, g, b = r / 255, g / 255, b / 255
  local max, min = math.max(r, g, b), math.min(r, g, b)
  local h, s, v
  v = max

  local d = max - min
  if max == 0 then s = 0 else s = d / max end

  if max == min then
    h = 0 -- achromatic
  else
    if max == r then
    h = (g - b) / d
    if g < b then h = h + 6 end
    elseif max == g then h = (b - r) / d + 2
    elseif max == b then h = (r - g) / d + 4    
    end
    h = h / 6
  end

  return h, s, v
end
-- hsv to rgb
function colrut.hsv2rgb(h, s, v)
  local r, g, b

  local i = math.floor(h * 6);
  local f = h * 6 - i;
  local p = v * (1 - s);
  local q = v * (1 - f * s);
  local t = v * (1 - (1 - f) * s);

  i = i % 6

  if i == 0 then r, g, b = v, t, p
  elseif i == 1 then r, g, b = q, v, p
  elseif i == 2 then r, g, b = p, v, t
  elseif i == 3 then r, g, b = p, q, v
  elseif i == 4 then r, g, b = t, p, v
  elseif i == 5 then r, g, b = v, p, q
  end

  return r * 255, g * 255, b * 255
end
-- rgb to hsl
function colrut.rgb2hsl(r, g, b)
  r, g, b = r / 255, g / 255, b / 255

  local max, min = math.max(r, g, b), math.min(r, g, b)
  local h, s, l

  l = (max + min) / 2

  if max == min then
    h, s = 0, 0 -- achromatic
  else
    local d = max - min
    local s
    if l > 0.5 then s = d / (2 - max - min) else s = d / (max + min) end
    if max == r then
      h = (g - b) / d
      if g < b then h = h + 6 end
    elseif max == g then h = (b - r) / d + 2
    elseif max == b then h = (r - g) / d + 4
    end
    h = h / 6
  end

  return h, s, l, a or 255
end
-- hsl to rgb
function colrut.hsl2rgb(h, s, l)
  local r, g, b

  if s == 0 then
    r, g, b = l, l, l -- achromatic
  else
    function hue2rgb(p, q, t)
      if t < 0   then t = t + 1 end
      if t > 1   then t = t - 1 end
      if t < 1/6 then return p + (q - p) * 6 * t end
      if t < 1/2 then return q end
      if t < 2/3 then return p + (q - p) * (2/3 - t) * 6 end
      return p
    end

    local q
    if l < 0.5 then q = l * (1 + s) else q = l + s - l * s end
    local p = 2 * l - q

    r = hue2rgb(p, q, h + 1/3)
    g = hue2rgb(p, q, h)
    b = hue2rgb(p, q, h - 1/3)
  end

  return r * 255, g * 255, b * 255
end
local refx,refy,refz = 95.047,100.000,108.883
local refu = (4 * refx) / (refx + (15 * refy) + (3 * refz))
local refv = (9 * refy) / (refx + (15 * refy) + (3 * refz))
-- rgb to xyz
function colrut.rgb2xyz(r, g, b)
  --(Observer = 2°, Illuminant = D65)
  if r > 0.04045 then
    r = ((r+0.055)/1.055)^2.4
  else
    r = r/12.92
  end
  if g > 0.04045 then
    g = ((g+0.055)/1.055)^2.4
  else
    g = g/12.92
  end
  if b > 0.04045 then
    b = ((b+0.055)/1.055)^2.4
  else
    b = b/12.92
  end

  r = r*100
  g = g*100
  b = b*100

  local x = r * 0.4124 + g * 0.3576 + b * 0.1805
  local y = r * 0.2126 + g * 0.7152 + b * 0.0722
  local z = r * 0.0193 + g * 0.1192 + b * 0.9505

  return x,y,z
end
-- xyz tzo lab
function colrut.xyz2lab(x, y, z)
  local l,a,b
  x,y,z = x/refx,y/refy,z/refz
  if x > 0.008856 then
    x = x^(1/3)
  else
    x = (7.787*x) + (16/116)
  end
  if y > 0.008856 then
    y = y^(1/3)
  else
    y = (7.787*y) + (16/116)
  end
  if z > 0.008856 then
    z = z^(1/3)
  else
    z = (7.787*z) + (16/116)
  end

  l = (116*y) - 16
  a = 500*(x-y)
  b = 200*(y-z)
  return l,a,b
end
-- lab to xyz
function colrut.lab2xyz(l, a, b)

  local y = (l+16) / 116
  local x = a / 500 + y
  local z = y - b / 200
  if x^3 > 0.008856 then
    x = x^3
  else
    x = (x - 16 / 116) / 7.787
  end
  if y^3 > 0.008856 then
    y = y^3
  else
    y = (y - 16 / 116) / 7.787
  end
  if z^3 > 0.008856 then
    z = z^3
  else
    z = (z - 16 / 116) / 7.787
  end

  return refx*x, refy*y, refz*z
end
-- xyz to rgb
function colrut.xyz2rgb(x, y, z)
  --(Observer = 2°, Illuminant = D65)
  local x,y,z = x/100,y/100,z/100
  local r,g,b

  r = x *  3.2406 + y * -1.5372 + z * -0.4986
  g = x * -0.9689 + y *  1.8758 + z *  0.0415
  b = x *  0.0557 + y * -0.2040 + z *  1.0570

  if r > 0.0031308 then
    r = 1.055*(r^(1/2.4))-0.055
  else
    r = r*12.92
  end
  if g > 0.0031308 then
    g = 1.055*(g^(1/2.4))-0.055
  else
    g = g*12.92
  end
  if b > 0.0031308 then
    b = 1.055*(b^(1/2.4))-0.055
  else
    b = b*12.92
  end

  return r, g, b
end
--rgb to lab
function colrut.rgb2lab(r, g, b)
  return colrut.xyz2lab(colrut.rgb2xyz(r, g, b))
end
-- lab to rgb
function colrut.lab2rgb(l, a, b)
  return colrut.xyz2rgb(colrut.lab2xyz(l, a, b))
end

return colrut

