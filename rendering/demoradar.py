import pygame
import time
import math
import random

# ========================= CONFIG =========================
USE_DUMMY = True              # ← Set to False when Arduino is ready

SERIAL_PORT = "COM3"          # Only used if USE_DUMMY = False
BAUD_RATE = 115200

WIDTH, HEIGHT = 900, 700
CENTER = (WIDTH//2, HEIGHT//2)
MAX_DISTANCE = 150

pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Ultrasonic Radar - Round Robin Tracking [DEMO]")
clock = pygame.time.Clock()
font = pygame.font.SysFont("consolas", 20)
bigfont = pygame.font.SysFont("consolas", 28, bold=True)

objects = []
sweep_angle = 0
sweep_direction = 1

if not USE_DUMMY:
    import serial
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.5)
        print(f"✅ Connected to {SERIAL_PORT}")
        time.sleep(2)
    except Exception as e:
        print("❌ Arduino connection failed. Running in Dummy mode instead.")
        USE_DUMMY = True

running = True

def generate_dummy_data():
    global sweep_angle, sweep_direction
    
    # Simulate sweep
    sweep_angle += 4 * sweep_direction
    if sweep_angle >= 180 or sweep_angle <= 0:
        sweep_direction *= -1
    
    # Fake objects (moving slowly)
    dummy_objects = []
    for a in [35, 80, 125, 155]:
        dist = 60 + math.sin(time.time() * 0.8 + a) * 25   # slight movement
        if random.random() < 0.7:   # sometimes appear
            dummy_objects.append((a, dist))
    
    return sweep_angle, dummy_objects


while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    screen.fill((5, 5, 15))

    # Radar background
    pygame.draw.circle(screen, (0, 40, 0), CENTER, 280, 3)
    pygame.draw.circle(screen, (0, 25, 0), CENTER, 200, 2)
    pygame.draw.circle(screen, (0, 15, 0), CENTER, 120, 2)
    
    for a in range(0, 181, 30):
        rad = math.radians(a - 90)
        ex = CENTER[0] + int(270 * math.cos(rad))
        ey = CENTER[1] + int(270 * math.sin(rad))
        pygame.draw.line(screen, (0, 60, 0), CENTER, (ex, ey), 1)

    # === DUMMY OR REAL DATA ===
    if USE_DUMMY:
        current_angle, new_objects = generate_dummy_data()
        objects = new_objects  # update detected objects
        
        # Simulate approaching sometimes
        if random.random() < 0.08:
            print("🚨 APPROACHING OBJECT DETECTED!")
    else:
        # Real Arduino reading (your previous code)
        try:
            line = ser.readline().decode('utf-8').strip()
            if line and line.startswith("S,"):
                parts = line.split(',')
                if len(parts) >= 3:
                    angle = int(parts[1])
                    dist = float(parts[2])
                    if 3 < dist < MAX_DISTANCE:
                        objects = [obj for obj in objects if abs(obj[0] - angle) > 10]
                        objects.append((angle, dist))
        except:
            pass

    # Draw detected objects
    for angle, dist in objects:
        rad = math.radians(angle - 90)
        x = CENTER[0] + int((dist / MAX_DISTANCE) * 260 * math.cos(rad))
        y = CENTER[1] + int((dist / MAX_DISTANCE) * 260 * math.sin(rad))
        
        color = (255, 60, 60) if dist < 70 else (0, 255, 100)   # red if close
        pygame.draw.circle(screen, color, (x, y), 11)
        pygame.draw.line(screen, (0, 180, 0), CENTER, (x, y), 3)

    # Draw sweeping line
    rad = math.radians(sweep_angle - 90 if USE_DUMMY else 90)
    sx = CENTER[0] + int(265 * math.cos(rad))
    sy = CENTER[1] + int(265 * math.sin(rad))
    pygame.draw.line(screen, (0, 255, 120), CENTER, (sx, sy), 3)

    # UI Text
    title = bigfont.render("ULTRASONIC RADAR SYSTEM", True, (0, 255, 120))
    screen.blit(title, (WIDTH//2 - title.get_width()//2, 15))
    
    info = font.render(f"Detected Objects: {len(objects)}   |   Max Range: {MAX_DISTANCE}cm   |   Mode: {'DEMO' if USE_DUMMY else 'LIVE'}", 
                      True, (0, 200, 100))
    screen.blit(info, (20, HEIGHT - 40))

    pygame.display.flip()
    clock.tick(60)

if not USE_DUMMY:
    ser.close()
pygame.quit()