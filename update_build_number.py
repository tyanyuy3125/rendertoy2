import re
from datetime import datetime

def update_build_number():
    try:
        with open('build_number', 'r') as file:
            build_number = int(file.read())
        build_number += 1
        with open('build_number', 'w') as file:
            file.write(str(build_number))
        with open('rendertoy_internal.h', 'r') as file:
            content = file.read()
        build_date = datetime.now().strftime("%Y-%m-%d+%H:%M:%S")
        content = re.sub(r'#define BUILD_NUMBER \d+', f'#define BUILD_NUMBER {build_number}', content)
        content = re.sub(r'#define BUILD_DATE ".*"', f'#define BUILD_DATE "{build_date}"', content)
        with open('rendertoy_internal.h', 'w') as file:
            file.write(content)
        return build_number
    except Exception as e:
        print(f"Error updating build number: {e}")
        return None

if __name__ == "__main__":
    new_build_number = update_build_number()
    if new_build_number is not None:
        print(f"Updated build number to {new_build_number}")
