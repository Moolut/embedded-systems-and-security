txt = [
0x4b,
0x0a,     
0x68,
0x1b,     
0x4a,
0x09,     
0xf4,
0x43, 
0x43,
0x00,
0xf0,
0x43, 
0x03,
0x80,
0x60,
0x13,     
0x4b,
0x07,     
0x68,
0x1b,     
0x4a,
0x06,     
0xf0,
0x43, 
0x03,
0x03,
0x60,
0x13,     
0xf8,
0xdf, 
0xf0,
0x04                        
]

result = []

for i in txt:
    result.append(hex(i ^ 0x42)[2:].zfill(2))

# Reversing pairs and printing 16 hex codes per line
for i in range(0, len(result), 16):
    line = []
    for j in range(i, min(i + 16, len(result)), 2):
        if j+1 < len(result):
            line.append(result[j+1])
            line.append(result[j])
        else:
            line.append(result[j])
    print(' '.join(line))

print(f"LEN: {len(txt)}")
