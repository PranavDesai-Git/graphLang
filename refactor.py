import os

replacements = {
    'USER_DATA': 'FOREIGN',
    'createUserData': 'createForeign',
    'LIST': 'CONS',
    'createList': 'createCons',
    'listLiteral': 'consLiteral',
    'NATIVE_FUNC': 'FOREIGN'
}

for root, dirs, files in os.walk('.'):
    if '.git' in root or 'out' in root:
        continue
    for f in files:
        if f.endswith(('.c', '.h')):
            path = os.path.join(root, f)
            with open(path, 'r') as file:
                content = file.read()
            
            orig = content
            # Special case for TreeNode.h to remove NATIVE_FUNC from enum completely
            if f == 'TreeNode.h':
                content = content.replace('    CLOSURE,\n    NATIVE_FUNC', '    CLOSURE')
            
            for k, v in replacements.items():
                content = content.replace(k, v)
                
            if content != orig:
                with open(path, 'w') as file:
                    file.write(content)
                print(f"Updated {path}")
