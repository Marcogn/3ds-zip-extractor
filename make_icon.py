#!/usr/bin/env python3
from PIL import Image, ImageDraw, ImageFont

# Crea un'immagine 48x48 RGBA (richiesto per 3DS)
img = Image.new('RGBA', (48, 48), color=(59, 89, 152, 255))

# Disegna un quadrato bianco al centro
draw = ImageDraw.Draw(img)
draw.rectangle([6, 6, 42, 42], fill=(255, 255, 255, 255), outline=(0, 0, 0, 255), width=1)

# Aggiungi il testo "ZIP"
try:
    font = ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf', 16)
except:
    font = ImageFont.load_default()

text = 'ZIP'
bbox = draw.textbbox((0, 0), text, font=font)
text_width = bbox[2] - bbox[0]
text_height = bbox[3] - bbox[1]
x = (48 - text_width) // 2
y = (48 - text_height) // 2 - 2

draw.text((x, y), text, fill=(59, 89, 152, 255), font=font)

# Salva l'icona
img.save('icon.png', 'PNG')
print('Icon 48x48 RGBA created successfully!')
