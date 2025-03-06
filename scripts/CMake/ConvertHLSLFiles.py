#!/usr/bin/env python3
import sys, os

def convert_file(filepath):
    # Read file in its original encoding (e.g., UTF-8)
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    # Write file in UTF-16
    with open(filepath, 'w', encoding='utf-16') as f:
        f.write(content)
    print(f"Converted: {filepath}")

def main(root_dir):
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            if file.lower().endswith('.hlsl'):
                filepath = os.path.join(root, file)
                convert_file(filepath)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: convert_hlsl.py <asset_dir>")
        sys.exit(1)
    main(sys.argv[1])
